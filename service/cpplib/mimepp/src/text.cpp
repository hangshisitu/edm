//=============================================================================
// File:       text.cpp
// Contents:   Definitions for DwText
// Maintainer: Doug Sauder <dwsauder@hunnysoft.com>
// WWW:        http://www.hunnysoft.com/mimepp/
//
// Copyright (c) 1996-2000 Douglas W. Sauder
// All rights reserved.
// 
// IN NO EVENT SHALL DOUGLAS W. SAUDER BE LIABLE TO ANY PARTY FOR DIRECT,
// INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF
// THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF DOUGLAS W. SAUDER
// HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// DOUGLAS W. SAUDER SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT
// NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
// PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
// BASIS, AND DOUGLAS W. SAUDER HAS NO OBLIGATION TO PROVIDE MAINTENANCE,
// SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
//
//=============================================================================

#define DW_CORE_IMPLEMENTATION

#include <mimepp/config.h>
#include <mimepp/debug.h>

#ifdef DW_USE_ANSI_IOSTREAM
#  include <iostream>
#else
#  include <iostream.h>
#endif

#include <ctype.h>
#include <mimepp/bounds.h>
#include <mimepp/string.h>
#include <mimepp/encoded.h>
#include <mimepp/text.h>
#include <mimepp/utility.h>

extern const char* dw_char_map;

#define ISSPACE(c) (dw_char_map[(c)&0xff]&0x02)


class DwTextParser {
    friend class DwText;
private:
    DwTextParser(const DwString&);
    void NextWord(size_t* start, size_t* len);
    const DwString mString;
    size_t mPos;
};


DwTextParser::DwTextParser(const DwString& aStr)
  : mString(aStr)
{
    mPos = 0;
}


void DwTextParser::NextWord(size_t* start, size_t* len)
{
    DW_ASSERT(start != 0);
    DW_ASSERT(len != 0);
    const char* buf = mString.data();
    size_t bufEnd = mString.length();
    // Skip initial white space
    size_t pos = mPos;
    while (pos < bufEnd) {
        char ch = buf[pos];
        if (! ISSPACE(ch)) {
            break;
        }
        ++pos;
    }
    // Now at word start.  Advance to word end.
    *start = pos;
    *len = 0;
    while (pos < bufEnd) {
        char ch = buf[pos];
        if (ISSPACE(ch)) {
            break;
        }
        ++pos;
    }
    mPos = pos;
    *len = pos - *start;
}


//============================================================================


const char* const DwText::sClassName = "DwText";
DwText* (*DwText::sNewText)(const DwString&, DwMessageComponent*) = 0;


DwText* DwText::NewText(const DwString& aStr, DwMessageComponent* aParent)
{
    DwText* text;
    if (sNewText) {
        text = sNewText(aStr, aParent);
    }
    else {
        text = new DwText(aStr, aParent);
    }
    return text;
}


DwText::DwText(DwBool bHeaderSubjectField)
{
    DwText_Init();
	m_bHeaderSubjectField = bHeaderSubjectField;
}


DwText::DwText(const DwText& aText)
  : DwFieldBody(aText)
{
    DwText_Init();
    _CopyEncodedWords(aText);
	m_bHeaderSubjectField = aText.m_bHeaderSubjectField;
}


DwText::DwText(const DwString& aStr, DwMessageComponent* aParent)
  : DwFieldBody(aStr, aParent)
{
    DwText_Init();
	m_bHeaderSubjectField = DwFalse;
}


DwText::~DwText()
{
    _DeleteAllEncodedWords();
    if (mEncodedWords != 0) {
        delete [] mEncodedWords;
        mEncodedWords = 0;
        mEncodedWordsSize = 0;
    }
}


void DwText::DwText_Init()
{
    mClassId = kCidText;
    mClassName = sClassName;
    mNumEncodedWords = 0;
    mEncodedWords = 0;
    mEncodedWordsSize = 0;
    
    const int encodedWordsSize = 10;
    mEncodedWords = new DwEncodedWord* [encodedWordsSize];
    if (mEncodedWords != 0) {
        mEncodedWordsSize = encodedWordsSize;
        for (int i=0; i < mEncodedWordsSize; ++i) {
            mEncodedWords[i] = 0;
        }
    }
}


const DwText& DwText::operator = (const DwText& aText)
{
    if (this != &aText) {
        _DeleteAllEncodedWords();
        DwFieldBody::operator = (aText);
        _CopyEncodedWords(aText);
        if (mParent != 0) {
            mParent->SetModified();
        }
    }
    return *this;
}


void DwText::Parse()
{
    mIsModified = DwFalse;
    _DeleteAllEncodedWords();
    DwTextParser parser(mString);
    DwString str;
    while (1) {
        size_t wordOffset;
        size_t wordLen;
        parser.NextWord(&wordOffset, &wordLen);
        if (wordLen == 0) {
            break;
        }
        // Check if word is encoded word
        DwBool isEncoded = DwFalse;
        if (wordLen >= 8) {
            const char* s = mString.data();
            if (s[wordOffset] == '=' &&
                s[wordOffset+1] == '?' &&
                s[wordOffset+wordLen-2] == '?' &&
                s[wordOffset+wordLen-1] == '=') {

                isEncoded = DwTrue;
            }
        }
        // If it's an encoded word, first add any pending unencoded word,
        // then add encoded word
        DwEncodedWord* word = 0;
        if (isEncoded) {
            if (str.length() > 0) {
                word = new DwEncodedWord(str);
                _InsertEncodedWordAt(mNumEncodedWords, word);
                word->Parse(m_bHeaderSubjectField);
            }
            str = mString.substr(wordOffset, wordLen);
            word = new DwEncodedWord(str);
            _InsertEncodedWordAt(mNumEncodedWords, word);
            word->Parse(m_bHeaderSubjectField);
            str = "";
        }
        // If it's not an encoded word, just append it to the pending
        // unencoded word
        else /* if (! isEncoded) */ {
            if (str.length() > 0) {
                str += " ";
            }
            str += mString.substr(wordOffset, wordLen);
        }
    }
    // Add any pending unencoded word
    if (str.length() > 0) {
        DwEncodedWord* word = new DwEncodedWord(str);
        _InsertEncodedWordAt(mNumEncodedWords, word);
        word->Parse(m_bHeaderSubjectField);
    }
}


void DwText::Assemble()
{
    if (mIsModified) {
        mString = "";
        for (int i=0; i < mNumEncodedWords; ++i) {
            mEncodedWords[i]->Assemble();
            if (i > 0) {
                mString += " ";
            }
            mString += mEncodedWords[i]->AsString();
        }
        DwFoldLine(mString, 15);
        mIsModified = DwFalse;
    }
}


DwMessageComponent* DwText::Clone() const
{
    return new DwText(*this);
}


int DwText::NumEncodedWords() const
{
    return mNumEncodedWords;
}


void DwText::AddEncodedWord(DwEncodedWord* aWord)
{
    DW_ASSERT(aWord != 0);
    if (aWord != 0) {
        _AddEncodedWord(aWord);
        SetModified();
    }
}


void DwText::DeleteAllEncodedWords()
{
    _DeleteAllEncodedWords();
    SetModified();
}


DwEncodedWord& DwText::EncodedWordAt(int aIndex) const
{
    // Check the index for a valid value. If it's invalid, throw an exception
    // or abort.
#if defined(DW_USE_EXCEPTIONS)
    if (! (0 <= aIndex && aIndex < mNumEncodedWords)) {
        throw DwBoundsException();
    }
#else
    DW_ASSERT(0 <= aIndex && aIndex < mNumEncodedWords);
    if (! (0 <= aIndex && aIndex < mNumEncodedWords)) {
        abort();
    }
#endif
    return *mEncodedWords[aIndex];
}


void DwText::InsertEncodedWordAt(int aIndex, DwEncodedWord* aWord)
{
    // Check the index for a valid value. If it's invalid, throw an exception
    // or recover.

#if defined(DW_USE_EXCEPTIONS)
    if (! (0 <= aIndex && aIndex <= mNumEncodedWords)) {
        throw DwBoundsException();
    }
#else
    DW_ASSERT(0 <= aIndex && aIndex <= mNumEncodedWords);
    // If there is a bounds error, recover by reassigning the index to
    // a valid value.
    if (aIndex < 0) {
        aIndex = 0;
    }
    else if (mNumEncodedWords < aIndex) {
        aIndex = mNumEncodedWords;
    }
#endif

    // Make the insertion and set the is-modified flag

    DW_ASSERT(aWord != 0);
    if (aWord != 0) {
        _InsertEncodedWordAt(aIndex, aWord);
        SetModified();
    }
}


DwEncodedWord* DwText::RemoveEncodedWordAt(int aIndex)
{
    // Check the index for a valid value. If it's invalid, throw an exception
    // or recover.

#if defined(DW_USE_EXCEPTIONS)
    if (! (0 <= aIndex && aIndex < mNumEncodedWords)) {
        throw DwBoundsException();
    }
#else
    DW_ASSERT(0 <= aIndex && aIndex < mNumEncodedWords);
    if (! (0 <= aIndex && aIndex < mNumEncodedWords)) {
        return 0;
    }
#endif

    // Save the requested encoded word as return value

    DwEncodedWord* ret = mEncodedWords[aIndex];
    ret->SetParent(0);

    // Shift other encoded words in the array

    for (int i=aIndex; i < mNumEncodedWords-1; ++i) {
        mEncodedWords[i] = mEncodedWords[i+1];
    }
    --mNumEncodedWords;
    mEncodedWords[mNumEncodedWords] = 0;
    SetModified();

    return ret;
}


void DwText::_CopyEncodedWords(const DwText& aText)
{
    DW_ASSERT(0 == mNumEncodedWords);

    // Copy the encoded words

    int numEncodedWords = aText.mNumEncodedWords;
    for (int i=0; i < numEncodedWords; ++i) {
        DwEncodedWord* word = (DwEncodedWord*) aText.mEncodedWords[i]->Clone();
        // If we're out of memory, and no exception is thrown, just
        // recover by returning.
        if (word == 0) {
            break;
        }
        _AddEncodedWord(word);
    }
}


void DwText::_AddEncodedWord(DwEncodedWord* aWord)
{
    int index = mNumEncodedWords;
    _InsertEncodedWordAt(index, aWord);
}


void DwText::_InsertEncodedWordAt(int aIndex, DwEncodedWord* aWord)
{
    // Reallocate a larger array, if necessary

    if (mNumEncodedWords == mEncodedWordsSize) {
        int newEncodedWordsSize = 2*mEncodedWordsSize;
        if (newEncodedWordsSize == 0) {
            newEncodedWordsSize = 10;
        }
        DwEncodedWord** newEncodedWords =
            new DwEncodedWord* [newEncodedWordsSize];
        if (newEncodedWords == 0) {
            return;
        }
        int i;
        for (i=0; i < mNumEncodedWords; ++i) {
            newEncodedWords[i] = mEncodedWords[i];
            mEncodedWords[i] = 0;
        }
        for ( ; i < newEncodedWordsSize; ++i) {
            newEncodedWords[i] = 0;
        }
        delete [] mEncodedWords;
        mEncodedWords = newEncodedWords;
        mEncodedWordsSize = newEncodedWordsSize;
    }

    // Make room in the array, then assign the encoded word

    for (int i=mNumEncodedWords; i > aIndex; --i) {
        mEncodedWords[i] = mEncodedWords[i-1];
    }

    mEncodedWords[aIndex] = aWord;
    ++mNumEncodedWords;
    aWord->SetParent(this);
}


void DwText::_DeleteAllEncodedWords()
{
    while (mNumEncodedWords > 0) {
        int i = mNumEncodedWords - 1;
        if (mEncodedWords[i] != 0) {
            delete mEncodedWords[i];
            mEncodedWords[i] = 0;
        }
        --mNumEncodedWords;
    }
}



#if defined (DW_DEBUG_VERSION)
void DwText::PrintDebugInfo(DW_STD ostream& aStrm, int /*aDepth*/) const
{
    aStrm << 
    "------------------ Debug info for DwText class -----------------\n";
    _PrintDebugInfo(aStrm);
}
#else // if !defined (DW_DEBUG_VERSION)
void DwText::PrintDebugInfo(DW_STD ostream& /*aStrm*/, int /*aDepth*/) const
{
}
#endif // !defined (DW_DEBUG_VERSION)


#if defined (DW_DEBUG_VERSION)
void DwText::_PrintDebugInfo(DW_STD ostream& aStrm) const
{
    DwFieldBody::_PrintDebugInfo(aStrm);
}
#else // if !defined (DW_DEBUG_VERSION)
void DwText::_PrintDebugInfo(DW_STD ostream& /*aStrm*/) const
{
}
#endif // !defined (DW_DEBUG_VERSION)


void DwText::CheckInvariants() const
{
#if defined (DW_DEBUG_VERSION)
    DwFieldBody::CheckInvariants();
#endif // defined (DW_DEBUG_VERSION)
}

