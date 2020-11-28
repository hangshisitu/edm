//=============================================================================
// File:       group.cpp
// Contents:   Definitions for DwGroup
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

#include <stdlib.h>
#include <string.h>
#ifdef DW_USE_ANSI_IOSTREAM
#  include <iostream>
#else
#  include <iostream.h>
#endif

#include <mimepp/string.h>
#include <mimepp/group.h>
#include <mimepp/token.h>

const char* const DwGroup::sClassName = "DwGroup";


DwGroup* (*DwGroup::sNewGroup)(const DwString&, DwMessageComponent*) = 0;


DwGroup* DwGroup::NewGroup(const DwString& aStr, DwMessageComponent* aParent)
{
    if (sNewGroup != 0) {
        return sNewGroup(aStr, aParent);
    }
    else {
        return new DwGroup(aStr, aParent);
    }
}


DwGroup::DwGroup()
{
    mMailboxList =
        DwMailboxList::NewMailboxList("", this);
    mClassId = kCidGroup;
    mClassName = sClassName;
}


DwGroup::DwGroup(const DwGroup& aGroup)
   : DwAddress(aGroup),
     mGroupName(aGroup.mGroupName)
{
    mMailboxList = (DwMailboxList*) aGroup.mMailboxList->Clone();
    mMailboxList->SetParent(this);
    mClassId = kCidGroup;
    mClassName = sClassName;
}


DwGroup::DwGroup(const DwString& aStr, DwMessageComponent* aParent)
   : DwAddress(aStr, aParent)
{
    mMailboxList =
        DwMailboxList::NewMailboxList("", this);
    mClassId = kCidGroup;
    mClassName = sClassName;
}


DwGroup::~DwGroup()
{
    delete mMailboxList;
    mMailboxList = 0;
}


const DwGroup& DwGroup::operator = (const DwGroup& aGroup)
{
    if (this == &aGroup) {
        return *this;
    }
    DwAddress::operator = (aGroup);
    mGroupName = aGroup.mGroupName;
    delete mMailboxList;
    mMailboxList = (DwMailboxList*) aGroup.mMailboxList->Clone();
    // *mMailboxList = *aGroup.mMailboxList;
    return *this;
}


const DwString& DwGroup::GroupName() const
{
    return mGroupName;
}


const DwString& DwGroup::Phrase() const
{
    return mGroupName;
}


void DwGroup::SetGroupName(const DwString& aName)
{
    mGroupName = aName;
}


void DwGroup::SetPhrase(const DwString& aPhrase)
{
    mGroupName = aPhrase;
}


DwMailboxList& DwGroup::MailboxList() const
{
    return *mMailboxList;
}


void DwGroup::Parse()
{
    mIsModified = DwFalse;
    mGroupName = "";
    DwBool isGroupNameNull = DwTrue;
    if (mMailboxList != 0) {
        delete mMailboxList;
    }
    mMailboxList = DwMailboxList::NewMailboxList("", this);
    mIsValid = DwFalse;
    DwRfc822Tokenizer tokenizer(mString);
    int type = tokenizer.Type();
    int ch;

    // Everything up to the first ':' is the group name
    DwBool done = DwFalse;
    while (!done && type != kTkNull) {
        switch (type) {
        case kTkSpecial:
            ch = tokenizer.Token()[0];
            switch (ch) {
            case ':':
                done = DwTrue;
            }
            break;
        case kTkQuotedString:
        case kTkAtom:
            if (isGroupNameNull) {
                isGroupNameNull = DwFalse;
            }
            else {
                mGroupName += " ";
            }
            mGroupName += tokenizer.Token();
            break;
        }
        ++tokenizer;
        type = tokenizer.Type();
    }

    // Find mailbox list, which ends with ';'
    DwTokenString tokenString(mString);
    tokenString.SetFirst(tokenizer);
    done = DwFalse;
    while (!done && type != kTkNull) {
        if (type == kTkSpecial && tokenizer.Token()[0] == ';') {
            tokenString.ExtendTo(tokenizer);
            break;
        }
        ++tokenizer;
        type = tokenizer.Type();
    }
    if (mMailboxList != 0) {
        delete mMailboxList;
    }
    mMailboxList = DwMailboxList::NewMailboxList(tokenString.Tokens(), this);
    mMailboxList->Parse();
    if (mGroupName.length() > 0) {
        mIsValid = DwTrue;
    }
    else {
        mIsValid = DwFalse;
    }
}


void DwGroup::Assemble()
{
    if (!mIsModified) {
        return;
    }
    if (mGroupName.length() == 0) {
        mIsValid = DwFalse;
        mString = "";
        return;
    }
    mMailboxList->Assemble();
    mString = "";
    mString += mGroupName;
    mString += ":";
    mString += mMailboxList->AsString();
    mString += ";";
    mIsModified = DwFalse;
}


DwMessageComponent* DwGroup::Clone() const
{
    return new DwGroup(*this);
}


#if defined (DW_DEBUG_VERSION)
void DwGroup::PrintDebugInfo(DW_STD ostream& aStrm, int aDepth) const
{
    aStrm << "------------ Debug info for DwGroup class ------------\n";
    _PrintDebugInfo(aStrm);
    int depth = aDepth - 1;
    depth = (depth >= 0) ? depth : 0;
    if (aDepth == 0 || depth > 0) {
        mMailboxList->PrintDebugInfo(aStrm, depth);
    }
}
#else // if !defined (DW_DEBUG_VERSION)
void DwGroup::PrintDebugInfo(DW_STD ostream& /*aStrm*/, int /*aDepth*/) const
{
}
#endif // !defined (DW_DEBUG_VERSION)


#if defined (DW_DEBUG_VERSION)
void DwGroup::_PrintDebugInfo(DW_STD ostream& aStrm) const
{
    DwAddress::_PrintDebugInfo(aStrm);
    aStrm << "Group name:       " << mGroupName << '\n';
    aStrm << "Mailbox list:     " << mMailboxList->ObjectId() << '\n';
}
#else // if !defined (DW_DEBUG_VERSION)
void DwGroup::_PrintDebugInfo(DW_STD ostream& /*aStrm*/) const
{
}
#endif // !defined (DW_DEBUG_VERSION)


void DwGroup::CheckInvariants() const
{
#if defined (DW_DEBUG_VERSION)
    DwAddress::CheckInvariants();
    mGroupName.CheckInvariants();
    mMailboxList->CheckInvariants();
    DW_ASSERT((DwMessageComponent*) this == mMailboxList->Parent());
#endif // defined (DW_DEBUG_VERSION)
}
