//=============================================================================
// File:       msgcmp.cpp
// Contents:   Definitions for DwMessageComponent
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
#ifdef DW_USE_ANSI_IOSTREAM
#  include <iostream>
#else
#  include <iostream.h>
#endif

#include <mimepp/msgcmp.h>

#define kMagicNumber ((DwUint32) 0x22222222L)


const char* const DwMessageComponent::sClassName = "DwMessageComponent";


DwMessageComponent::DwMessageComponent()
{
    mMagicNumber = (DwUint32) kMagicNumber;
    mIsModified = DwFalse;
    mParent = 0;
    mClassId = kCidMessageComponent;
    mClassName = sClassName;
}


DwMessageComponent::DwMessageComponent(const DwMessageComponent& aCmp)
  : mString(aCmp.mString)
{
    mMagicNumber = (DwUint32) kMagicNumber;
    mIsModified = aCmp.mIsModified;
    mParent = 0;
    mClassId = kCidMessageComponent;
    mClassName = sClassName;
}


DwMessageComponent::DwMessageComponent(const DwString& aStr,
    DwMessageComponent* aParent)
  : mString(aStr)
{
    mMagicNumber = (DwUint32) kMagicNumber;
    mIsModified = DwFalse;
    mParent = aParent;
    mClassId = kCidMessageComponent;
    mClassName = sClassName;
}


DwMessageComponent::~DwMessageComponent()
{
#if defined (DW_DEBUG_VERSION) || defined (DW_DEVELOPMENT_VERSION)
    if (mMagicNumber != (DwUint32) kMagicNumber) {
        DW_STD cerr << "Bad value for 'this' in destructor\n";
        DW_STD cerr << "(Possibly 'delete' was called twice for same object)\n";
        abort();
    }
    mMagicNumber = 0;
#endif // defined (DW_DEBUG_VERSION) || defined (DW_DEVELOPMENT_VERSION)
}


const DwMessageComponent&
DwMessageComponent::operator = (const DwMessageComponent& aCmp)
{
    if (this == &aCmp) {
        return *this;
    }
    mString = aCmp.mString;
    mIsModified = aCmp.mIsModified;
    return *this;
}


void DwMessageComponent::FromString(const DwString& aStr)
{
    mString = aStr;
    mIsModified = DwFalse;
    if (mParent != 0) {
        mParent->SetModified();
    }
}


void DwMessageComponent::FromString(const char* aCstr)
{
    DW_ASSERT(aCstr != 0);
    mString = aCstr;
    if (mParent != 0) {
        mParent->SetModified();
    }
}


const DwString& DwMessageComponent::AsString() const
{
    return mString;
}


DwMessageComponent* DwMessageComponent::Parent()
{
    return mParent;
}


void DwMessageComponent::SetParent(DwMessageComponent* aParent)
{
    mParent = aParent;
}


DwBool DwMessageComponent::IsModified() const
{
    return mIsModified;
}


void DwMessageComponent::SetModified()
{
    mIsModified = DwTrue;
    if (mParent != 0) {
        mParent->SetModified();
    }
}


int DwMessageComponent::ClassId() const
{
    return mClassId;
}


const char* DwMessageComponent::ClassName() const
{
    return mClassName;
}


int DwMessageComponent::ObjectId() const
{
    return (int) (long) this;
}


#if defined (DW_DEBUG_VERSION)
void DwMessageComponent::PrintDebugInfo(DW_STD ostream& aStrm, int /*aDepth*/) const
{
    _PrintDebugInfo(aStrm);
}
#else // if !defined (DW_DEBUG_VERSION)
void DwMessageComponent::PrintDebugInfo(DW_STD ostream& /*aStrm*/, int /*aDepth*/) const
{
}
#endif // !defined (DW_DEBUG_VERSION)


#if defined (DW_DEBUG_VERSION)
void DwMessageComponent::_PrintDebugInfo(DW_STD ostream& aStrm) const
{
    aStrm << "ObjectId:         " << ObjectId() << '\n';
    aStrm << "ClassId:          ";
    switch (ClassId()) {
    case kCidError:
        aStrm << "kCidError";
        break;
    case kCidUnknown:
        aStrm << "kCidUnknown";
        break;
    case kCidAddress:
        aStrm << "kCidAddress";
        break;
    case kCidAddressList:
        aStrm << "kCidAddressList";
        break;
    case kCidBody:
        aStrm << "kCidBody";
        break;
    case kCidBodyPart:
        aStrm << "kCidBodyPart";
        break;
    case kCidDispositionType:
        aStrm << "kCidDispositionType";
        break;
    case kCidMechanism:
        aStrm << "kCidMechanism";
        break;
    case kCidMediaType:
        aStrm << "kCidMediaType";
        break;
    case kCidParameter:
        aStrm << "kCidParameter";
        break;
    case kCidDateTime:
        aStrm << "kCidDateTime";
        break;
    case kCidEntity:
        aStrm << "kCidEntity";
        break;
    case kCidField:
        aStrm << "kCidField";
        break;
    case kCidFieldBody:
        aStrm << "kCidFieldBody";
        break;
    case kCidGroup:
        aStrm << "kCidGroup";
        break;
    case kCidHeaders:
        aStrm << "kCidHeaders";
        break;
    case kCidMailbox:
        aStrm << "kCidMailbox";
        break;
    case kCidMailboxList:
        aStrm << "kCidMailboxList";
        break;
    case kCidMessage:
        aStrm << "kCidMessage";
        break;
    case kCidMessageComponent:
        aStrm << "kCidMessageComponent";
        break;
    case kCidMsgId:
        aStrm << "kCidMsgId";
        break;
    case kCidText:
        aStrm << "kCidText";
        break;
    }
    aStrm << '\n';
    aStrm << "ClassName:        " << ClassName() << '\n';
    aStrm << "String:           " << mString << '\n';
    aStrm << "IsModified:       " << (IsModified() ? "True" : "False") << '\n';
    aStrm << "Parent ObjectId:  ";
    if (mParent != 0) {
        aStrm << mParent->ObjectId() << '\n';
    }
    else {
        aStrm << "(none)\n";
    }
}
#else // if !defined (DW_DEBUG_VERSION)
void DwMessageComponent::_PrintDebugInfo(DW_STD ostream& /*aStrm*/) const
{
}
#endif // !defined (DW_DEBUG_VERSION)


void DwMessageComponent::CheckInvariants() const
{
#if defined (DW_DEBUG_VERSION)
    DW_ASSERT(mMagicNumber == kMagicNumber);
    DW_ASSERT(mClassName != 0);
    mString.CheckInvariants();
#endif // defined (DW_DEBUG_VERSION)
}


