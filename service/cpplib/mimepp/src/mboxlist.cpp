//=============================================================================
// File:       mboxlist.cpp
// Contents:   Definitions for DwMailboxList
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

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#ifdef DW_USE_ANSI_IOSTREAM
#  include <iostream>
#else
#  include <iostream.h>
#endif

#include <mimepp/string.h>
#include <mimepp/mailbox.h>
#include <mimepp/mboxlist.h>
#include <mimepp/token.h>
#include <mimepp/bounds.h>


const char* const DwMailboxList::sClassName = "DwMailboxList";
DwMailboxList* (*DwMailboxList::sNewMailboxList)(const DwString&,
    DwMessageComponent*) = 0;


DwMailboxList* DwMailboxList::NewMailboxList(const DwString& aStr,
    DwMessageComponent* aParent)
{
    if (sNewMailboxList != 0) {
        return sNewMailboxList(aStr, aParent);
    }
    else {
        return new DwMailboxList(aStr, aParent);
    }
}


DwMailboxList::DwMailboxList()
{
    DwMailboxList_Init();
}


DwMailboxList::DwMailboxList(const DwMailboxList& aList)
  : DwFieldBody(aList)
{
    DwMailboxList_Init();
    _CopyMailboxes(aList);
}


DwMailboxList::DwMailboxList(const DwString& aStr, DwMessageComponent* aParent)
  : DwFieldBody(aStr, aParent)
{
    DwMailboxList_Init();
}


DwMailboxList::~DwMailboxList()
{
    _DeleteAllMailboxes();
    if (mMailboxes != 0) {
        delete [] mMailboxes;
        mMailboxes = 0;
    }
}


void DwMailboxList::DwMailboxList_Init()
{
    mClassId = kCidMailboxList;
    mClassName = sClassName;
    mNumMailboxes = 0;
    mMailboxes = 0;
    mMailboxesSize = 0;

    const int mailboxesSize = 10;
    mMailboxes = new DwMailbox* [mailboxesSize];
    if (mMailboxes != 0) {
        mMailboxesSize = mailboxesSize;
        for (int i=0; i < mMailboxesSize; ++i) {
            mMailboxes[i] = 0;
        }
    }
}


const DwMailboxList& DwMailboxList::operator = (const DwMailboxList& aList)
{
    if (this != &aList) {
        _DeleteAllMailboxes();
        DwFieldBody::operator = (aList);
        _CopyMailboxes(aList);
        if (mParent != 0) {
            mParent->SetModified();
        }
    }
    return *this;
}


void DwMailboxList::Parse()
{
    mIsModified = DwFalse;
    _DeleteAllMailboxes();
    // Mailboxes are separated by commas.  Commas may also occur in a route.
    // (See RFC822 p. 27)
    DwMailboxListParser parser(mString);
    DwMailbox* mailbox;
    DwBool done = DwFalse;
    while (! done) {
        switch (parser.MboxType()) {
        case DwMailboxListParser::kMboxError:
        case DwMailboxListParser::kMboxEnd:
            done = DwTrue;
            break;
        case DwMailboxListParser::kMboxMailbox:
            mailbox = DwMailbox::NewMailbox(parser.MboxString(), this);
            if (mailbox == 0) {
                break;
            }
            mailbox->Parse();
            _AddMailbox(mailbox);
            break;
        case DwMailboxListParser::kMboxNull:
            break;
        }
        if (! done) {
            ++parser;
        }
    }
}


void DwMailboxList::Assemble()
{
    if (mIsModified) {
        mString = "";
        int count = 0;
        for (int i=0; i < mNumMailboxes; ++i) {
            DwMailbox* mbox = mMailboxes[i];
            mbox->Assemble();
            if (count > 0){
                if (IsFolding()) {
                    mString += "," DW_EOL "  ";
                }
                else {
                    mString += ", ";
                }
            }
            mString += mbox->AsString();
            ++count;
        }
        mIsModified = DwFalse;
    }
}


DwMessageComponent* DwMailboxList::Clone() const
{
    return new DwMailboxList(*this);
}


int DwMailboxList::NumMailboxes() const
{
    return mNumMailboxes;
}


void DwMailboxList::AddMailbox(DwMailbox* aMailbox)
{
    DW_ASSERT(aMailbox != 0);
    if (aMailbox != 0) {
        _AddMailbox(aMailbox);
        SetModified();
    }
}


void DwMailboxList::DeleteAllMailboxes()
{
    _DeleteAllMailboxes();
    SetModified();
}


DwMailbox& DwMailboxList::MailboxAt(int aIndex) const
{
    // Check the index for a valid value. If it's invalid, throw an exception
    // or abort.
#if defined(DW_USE_EXCEPTIONS)
    if (! (0 <= aIndex && aIndex < mNumMailboxes)) {
        throw DwBoundsException();
    }
#else
    DW_ASSERT(0 <= aIndex && aIndex < mNumMailboxes);
    if (! (0 <= aIndex && aIndex < mNumMailboxes)) {
        abort();
    }
#endif
    return *mMailboxes[aIndex];
}


void DwMailboxList::InsertMailboxAt(int aIndex, DwMailbox* aMailbox)
{
    // Check the index for a valid value. If it's invalid, throw an exception
    // or recover.

#if defined(DW_USE_EXCEPTIONS)
    if (! (0 <= aIndex && aIndex <= mNumMailboxes)) {
        throw DwBoundsException();
    }
#else
    DW_ASSERT(0 <= aIndex && aIndex <= mNumMailboxes);
    // If there is a bounds error, recover by reassigning the index to
    // a valid value.
    if (aIndex < 0) {
        aIndex = 0;
    }
    else if (mNumMailboxes < aIndex) {
        aIndex = mNumMailboxes;
    }
#endif

    // Make the insertion and set the is-modified flag

    DW_ASSERT(aMailbox != 0);
    if (aMailbox != 0) {
        _InsertMailboxAt(aIndex, aMailbox);
        SetModified();
    }
}


DwMailbox* DwMailboxList::RemoveMailboxAt(int aIndex)
{
    // Check the index for a valid value. If it's invalid, throw an exception
    // or recover.

#if defined(DW_USE_EXCEPTIONS)
    if (! (0 <= aIndex && aIndex < mNumMailboxes)) {
        throw DwBoundsException();
    }
#else
    DW_ASSERT(0 <= aIndex && aIndex < mNumMailboxes);
    if (! (0 <= aIndex && aIndex < mNumMailboxes)) {
        return 0;
    }
#endif

    // Save the requested mailbox as return value

    DwMailbox* ret = mMailboxes[aIndex];
    ret->SetParent(0);

    // Shift other mailboxes in the array

    for (int i=aIndex; i < mNumMailboxes-1; ++i) {
        mMailboxes[i] = mMailboxes[i+1];
    }
    --mNumMailboxes;
    mMailboxes[mNumMailboxes] = 0;
    SetModified();

    return ret;
}


void DwMailboxList::_CopyMailboxes(const DwMailboxList& aMboxList)
{
    DW_ASSERT(0 == mNumMailboxes);

    // Copy the mailboxes

    int numMailboxes = aMboxList.mNumMailboxes;
    for (int i=0; i < numMailboxes; ++i) {
        DwMailbox* mailbox = (DwMailbox*) aMboxList.mMailboxes[i]->Clone();
        // If we're out of memory, and no exception is thrown, just
        // recover by returning.
        if (mailbox == 0) {
            break;
        }
        _AddMailbox(mailbox);
    }
}


void DwMailboxList::_AddMailbox(DwMailbox* aMailbox)
{
    int index = mNumMailboxes;
    _InsertMailboxAt(index, aMailbox);
}


void DwMailboxList::_InsertMailboxAt(int aIndex, DwMailbox* aMailbox)
{
    // Reallocate a larger array, if necessary

    if (mNumMailboxes == mMailboxesSize) {
        int newMailboxesSize = 2*mMailboxesSize;
        if (newMailboxesSize == 0) {
            newMailboxesSize = 10;
        }
        DwMailbox** newMailboxes = new DwMailbox* [newMailboxesSize];
        if (newMailboxes == 0) {
            return;
        }
        int i;
        for (i=0; i < mNumMailboxes; ++i) {
            newMailboxes[i] = mMailboxes[i];
            mMailboxes[i] = 0;
        }
        for ( ; i < newMailboxesSize; ++i) {
            newMailboxes[i] = 0;
        }
        delete [] mMailboxes;
        mMailboxes = newMailboxes;
        mMailboxesSize = newMailboxesSize;
    }

    // Make room in the array, then assign the mailbox

    for (int i=mNumMailboxes; i > aIndex; --i) {
        mMailboxes[i] = mMailboxes[i-1];
    }

    mMailboxes[aIndex] = aMailbox;
    ++mNumMailboxes;
    aMailbox->SetParent(this);
}


void DwMailboxList::_DeleteAllMailboxes()
{
    while (mNumMailboxes > 0) {
        int i = mNumMailboxes - 1;
        if (mMailboxes[i] != 0) {
            delete mMailboxes[i];
            mMailboxes[i] = 0;
        }
        --mNumMailboxes;
    }
}


#if defined (DW_DEBUG_VERSION)
void DwMailboxList::PrintDebugInfo(DW_STD ostream& aStrm, int aDepth) const
{
    aStrm <<
    "-------------- Debug info for DwMailboxList class --------------\n";
    _PrintDebugInfo(aStrm);
    int depth = aDepth - 1;
    depth = (depth >= 0) ? depth : 0;
    if (aDepth == 0 || depth > 0) {
        for (int i=0; i < mNumMailboxes; ++i) {
            mMailboxes[i]->PrintDebugInfo(aStrm, depth);
        }
    }
}
#else // if !defined (DW_DEBUG_VERSION)
void DwMailboxList::PrintDebugInfo(DW_STD ostream& /*aStrm*/, int /*aDepth*/) const
{
}
#endif // !defined (DW_DEBUG_VERSION)


#if defined (DW_DEBUG_VERSION)
void DwMailboxList::_PrintDebugInfo(DW_STD ostream& aStrm) const
{
    DwFieldBody::_PrintDebugInfo(aStrm);
    aStrm << "Mailbox objects:  ";
    if (mNumMailboxes > 0) {
        for (int i=0; i < mNumMailboxes; ++i) {
            if (i > 0) {
                aStrm << ' ';
            }
            aStrm << mMailboxes[i]->ObjectId();
        }
        aStrm << '\n';
    }
    else {
        aStrm << "(none)\n";
    }
}
#else // if !defined (DW_DEBUG_VERSION)
void DwMailboxList::_PrintDebugInfo(DW_STD ostream& /*aStrm*/) const
{
}
#endif // !defined (DW_DEBUG_VERSION)


void DwMailboxList::CheckInvariants() const
{
#if defined (DW_DEBUG_VERSION)
    for (int i=0; i < mNumMailboxes; ++i) {
        mMailboxes[i]->CheckInvariants();
        DW_ASSERT((DwMessageComponent*) this == mMailboxes[i]->Parent());
    }
#endif // defined (DW_DEBUG_VERSION)
}


//-------------------------------------------------------------------------


DwMailboxListParser::DwMailboxListParser(const DwString& aStr)
  : mTokenizer(aStr),
    mMboxString(aStr)
{
    mMboxType = kMboxError;
    ParseNextMailbox();
}


DwMailboxListParser::~DwMailboxListParser()
{
}


const DwString& DwMailboxListParser::MboxString()
{
    return mMboxString.Tokens();
}


int DwMailboxListParser::MboxType()
{
    return mMboxType;
}


DwBool DwMailboxListParser::IsNull()
{
    DwBool ret = (mMboxType == kMboxNull) ? DwTrue : DwFalse;
    return ret;
}


DwBool DwMailboxListParser::IsEnd()
{
    DwBool ret = (mMboxType == kMboxEnd) ? DwTrue : DwFalse;
    return ret;
}


int DwMailboxListParser::Restart()
{
    mTokenizer.Restart();
    ParseNextMailbox();
    return mMboxType;
}


int DwMailboxListParser::operator ++ ()
{
    ParseNextMailbox();
    return mMboxType;
}


void DwMailboxListParser::ParseNextMailbox()
{
    mMboxString.SetFirst(mTokenizer);
    mMboxType = kMboxEnd;
    int type = mTokenizer.Type();
    if (type == kTkNull) {
        return;
    }
    enum {
        kTopLevel,
        kInRouteAddr
    } state;
    state = kTopLevel;
    mMboxType = kMboxMailbox;
    DwBool done = DwFalse;
    while (!done) {
        if (type == kTkNull) {
            mMboxString.ExtendTo(mTokenizer);
            break;
        }
        if (type == kTkSpecial) {
            int ch = mTokenizer.Token()[0];
            switch (state) {
            case kTopLevel:
                switch (ch) {
                case ',':
                    mMboxString.ExtendTo(mTokenizer);
                    done = DwTrue;
                    break;
                case '<':
                    state = kInRouteAddr;
                    break;
                }
                break;
            case kInRouteAddr:
                switch (ch) {
                case '>':
                    state = kTopLevel;
                    break;
                }
                break;
            }
        }
        ++mTokenizer;
        type = mTokenizer.Type();
    }
    if (mMboxString.Tokens().length() == 0) {
        mMboxType = kMboxNull;
    }
}

