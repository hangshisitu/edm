//=============================================================================
// File:       encoded.cpp
// Contents:   Definitions for DwEncodedWord
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

#include <mimepp/string.h>
#include <mimepp/enum.h>
#include <mimepp/utility.h>
#include <mimepp/encoded.h>


const char* const DwEncodedWord::sClassName = "DwEncodedWord";


DwEncodedWord* (*DwEncodedWord::sNewEncodedWord)(const DwString&,
    DwMessageComponent*) = 0;


DwEncodedWord* DwEncodedWord::NewEncodedWord(const DwString& aStr, 
    DwMessageComponent* aParent)
{
    if (sNewEncodedWord) {
        return sNewEncodedWord(aStr, aParent);
    }
    else {
        return new DwEncodedWord(aStr, aParent);
    }
        
}


DwEncodedWord::DwEncodedWord()
{
    mClassId = kCidEncodedWord;
    mClassName = sClassName;
}


DwEncodedWord::DwEncodedWord(const DwEncodedWord& aWord)
  : DwMessageComponent(aWord)
{
    mClassId = kCidEncodedWord;
    mClassName = sClassName;
}


DwEncodedWord::DwEncodedWord(const DwString& aStr, DwMessageComponent* aParent)
  : DwMessageComponent(aStr, aParent)
{
    mClassId = kCidEncodedWord;
    mClassName = sClassName;
}


DwEncodedWord::~DwEncodedWord()
{
}


const DwEncodedWord& DwEncodedWord::operator = (const DwEncodedWord& aWord)
{
    if (this == &aWord) return *this;
    DwMessageComponent::operator = (aWord);
    return *this;
}


void DwEncodedWord::Parse()
{
	return Parse(DwFalse);
}

void DwEncodedWord::Parse(DwBool bHeaderSubjectField)
{
    // Set default values

    mIsModified = DwFalse;
    mCharset = "US-ASCII";
    mEncodingType = 0;
    mEncodedText = "";
    mDecodedText = mString;

    // Find the segments

    const char* str = mString.data();
    size_t strLen = mString.length();
    DwBool isEncoded = DwFalse;
    size_t charsetOffset = 0;
    size_t charsetLen = 0;
    size_t encodedTextOffset =0;
    size_t encodedTextLen = 0;
    char encodingType = '\0';
    size_t pos = 0;
    if (pos < strLen && str[pos] == '=') {
        ++pos;
        if (pos < strLen && str[pos] == '?') {
            ++pos;
            charsetOffset = pos;
            for ( ; pos < strLen; ++pos) {
                if (str[pos] == '?') {
                    charsetLen = pos - charsetOffset;
                    break;
                }
            }
            if (pos < strLen /* && str[pos] == '?' */) {
                ++pos;
                if (pos < strLen) {
                    encodingType = str[pos];
                    ++pos;
                    if (pos < strLen && str[pos] == '?') {
                        ++pos;
                        encodedTextOffset = pos;
                        for ( ; pos < strLen; ++pos) {
                            if (str[pos] == '?') {
                                encodedTextLen = pos - encodedTextOffset;
                                break;
                            }
                        }
                        if (pos < strLen /* && str[pos] == '?' */) {
                            ++pos;
                            if (pos < strLen && str[pos] == '=') {
                                isEncoded = DwTrue;
                            }
                        }
                    }
                }
            }
        }
    }

    // If it's an encoded word, then decode the encoded text

    if (isEncoded) {
        mCharset = mString.substr(charsetOffset, charsetLen);
        mEncodingType = encodingType;
        mEncodedText = mString.substr(encodedTextOffset, encodedTextLen);
        int err = -1;
        if (encodingType == 'q' || encodingType == 'Q') {
            err = DwDecodeQuotedPrintable(mEncodedText, mDecodedText, bHeaderSubjectField);
        }
        else if (encodingType == 'b' || encodingType == 'B') {
            err = DwDecodeBase64(mEncodedText, mDecodedText);
        }
        // If we couldn't decode, then just set defaults
        if (err) {
            mEncodedText = "";
            mDecodedText = mString;
            mEncodingType = 0;
            mCharset = "US-ASCII";
        }
    }
}


void DwEncodedWord::Assemble()
{
    if (!mIsModified) {
        return;
    }
    if (mEncodingType == 'q' || mEncodingType == 'Q') {
        DwQEncode(mDecodedText, mEncodedText);
        mString = "=?";
        mString += mCharset;
        mString += "?";
        mString += mEncodingType;
        mString += "?";
        mString += mEncodedText;
        mString += "?=";
    }
    else if (mEncodingType == 'b' || mEncodingType == 'B') {
        DwBEncode(mDecodedText, mEncodedText);
        mString = "=?";
        mString += mCharset;
        mString += "?";
        mString += mEncodingType;
        mString += "?";
        mString += mEncodedText;
        mString += "?=";
    }
    else {
        mString = mDecodedText;
    }
    mIsModified = DwFalse;
}


DwMessageComponent* DwEncodedWord::Clone() const
{
    return new DwEncodedWord(*this);
}


const DwString& DwEncodedWord::Charset() const
{
    return mCharset;
}


void DwEncodedWord::SetCharset(const DwString& aCharset)
{
    SetModified();
    mCharset = aCharset;
}


char DwEncodedWord::EncodingType() const
{
    return mEncodingType;
}


void DwEncodedWord::SetEncodingType(char aEncodingType)
{
    SetModified();
    mEncodingType = aEncodingType;
}


const DwString& DwEncodedWord::EncodedText() const
{
    return mEncodedText;
}


void DwEncodedWord::SetEncodedText(const DwString& aStr)
{
    SetModified();
    mEncodedText = aStr;
}


const DwString& DwEncodedWord::DecodedText() const
{
    return mDecodedText;
}


void DwEncodedWord::SetDecodedText(const DwString& aStr)
{
    SetModified();
    mDecodedText = aStr;
}


#if defined (DW_DEBUG_VERSION)
void DwEncodedWord::PrintDebugInfo(DW_STD ostream& aStrm, int /*aDepth*/) const
{
    aStrm <<
    "-------------- Debug info for DwEncodedWord class --------------\n";
    _PrintDebugInfo(aStrm);
}
#else // if !defined (DW_DEBUG_VERSION)
void DwEncodedWord::PrintDebugInfo(DW_STD ostream& /*aStrm*/,
    int /*aDepth*/) const
{
}
#endif // !defined (DW_DEBUG_VERSION)


#if defined (DW_DEBUG_VERSION)
void DwEncodedWord::_PrintDebugInfo(DW_STD ostream& aStrm) const
{
    DwMessageComponent::_PrintDebugInfo(aStrm);
}
#else // if !defined (DW_DEBUG_VERSION)
void DwEncodedWord::_PrintDebugInfo(DW_STD ostream& /*aStrm*/) const
{
}
#endif // !defined (DW_DEBUG_VERSION)


void DwEncodedWord::CheckInvariants() const
{
#if defined (DW_DEBUG_VERSION)
    DwMessageComponent::CheckInvariants();
#endif // defined (DW_DEBUG_VERSION)
}

