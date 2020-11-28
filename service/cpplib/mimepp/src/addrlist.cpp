//=============================================================================
// File:       addrlist.cpp
// Contents:   Definitions for DwAddressList
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
# else
#  include <iostream.h>
#endif

#include <mimepp/string.h>
#include <mimepp/address.h>
#include <mimepp/addrlist.h>
#include <mimepp/token.h>
#include <mimepp/group.h>
#include <mimepp/mailbox.h>
#include <mimepp/bounds.h>


const char* const DwAddressList::sClassName = "DwAddressList";
DwAddressList* (*DwAddressList::sNewAddressList)(const DwString&,
    DwMessageComponent*) = 0;


DwAddressList* DwAddressList::NewAddressList(const DwString& aStr,
    DwMessageComponent* aParent)
{
    if (sNewAddressList != 0) {
        return sNewAddressList(aStr, aParent);
    }
    else {
        return new DwAddressList(aStr, aParent);
    }
}


DwAddressList::DwAddressList()
{
    DwAddressList_Init();
}


DwAddressList::DwAddressList(const DwAddressList& aList)
  : DwFieldBody(aList)
{
    DwAddressList_Init();
    _CopyAddresses(aList);
}


DwAddressList::DwAddressList(const DwString& aStr, DwMessageComponent* aParent)
  : DwFieldBody(aStr, aParent)
{
    DwAddressList_Init();
}


DwAddressList::~DwAddressList()
{
    _DeleteAllAddresses();
    if (mAddresses != 0) {
        delete [] mAddresses;
        mAddresses = 0;
    }
}


void DwAddressList::DwAddressList_Init()
{
    mClassId = kCidAddressList;
    mClassName = sClassName;
    mNumAddresses = 0;
    mAddresses = 0;
    mAddressesSize = 0;

    const int addressesSize = 10;
    mAddresses = new DwAddress* [addressesSize];
    if (mAddresses != 0) {
        mAddressesSize = addressesSize;
        for (int i=0; i < mAddressesSize; ++i) {
            mAddresses[i] = 0;
        }
    }
}


const DwAddressList& DwAddressList::operator = (const DwAddressList& aList)
{
    if (this != &aList) {
        _DeleteAllAddresses();
        DwFieldBody::operator = (aList);
        _CopyAddresses(aList);
        if (mParent != 0) {
            mParent->SetModified();
        }
    }
    return *this;
}


void DwAddressList::Parse()
{
    mIsModified = DwFalse;
    _DeleteAllAddresses();
    DwAddressListParser parser(mString);
    DwAddress* address;
    DwBool done = DwFalse;

    while (! done) {
        switch (parser.AddrType()) {
        case DwAddressListParser::kAddrError:
        case DwAddressListParser::kAddrEnd:
            done = DwTrue;
            break;
        case DwAddressListParser::kAddrMailbox:
            address = DwMailbox::NewMailbox(parser.AddrString(), this);
            address->Parse();
            _AddAddress(address);
            break;
        case DwAddressListParser::kAddrGroup:
            address = DwGroup::NewGroup(parser.AddrString(), this);
            address->Parse();
            _AddAddress(address);
            break;
        case DwAddressListParser::kAddrNull:
            break;
        }
        if (!done) {
            ++parser;
        }
    }
}


void DwAddressList::Assemble()
{
    if (mIsModified) {
        mString = "";
        int count = 0;
        for (int i=0; i < mNumAddresses; ++i) {
            DwAddress* addr = mAddresses[i];
            addr->Assemble();
            if (count > 0){
                if (IsFolding()) {
                    mString += "," DW_EOL " ";
                }
                else {
                    mString += ", ";
                }
            }
            mString += addr->AsString();
            ++count;
        }
        mIsModified = DwFalse;
    }
}


DwMessageComponent* DwAddressList::Clone() const
{
    return new DwAddressList(*this);
}


int DwAddressList::NumAddresses() const
{
    return mNumAddresses;
}


void DwAddressList::AddAddress(DwAddress* aAddr)
{
    DW_ASSERT(aAddr != 0);
    if (aAddr != 0) {
        _AddAddress(aAddr);
        SetModified();
    }
}


void DwAddressList::DeleteAllAddresses()
{
    _DeleteAllAddresses();
    SetModified();
}


DwAddress& DwAddressList::AddressAt(int aIndex) const
{
    // Check the index for a valid value. If it's invalid, throw an exception
    // or abort.
#if defined(DW_USE_EXCEPTIONS)
    if (! (0 <= aIndex && aIndex < mNumAddresses)) {
        throw DwBoundsException();
    }
#else
    DW_ASSERT(0 <= aIndex && aIndex < mNumAddresses);
    if (! (0 <= aIndex && aIndex < mNumAddresses)) {
        abort();
    }
#endif
    return *mAddresses[aIndex];
}


void DwAddressList::InsertAddressAt(int aIndex, DwAddress* aAddr)
{
    // Check the index for a valid value. If it's invalid, throw an exception
    // or recover.

#if defined(DW_USE_EXCEPTIONS)
    if (! (0 <= aIndex && aIndex <= mNumAddresses)) {
        throw DwBoundsException();
    }
#else
    DW_ASSERT(0 <= aIndex && aIndex <= mNumAddresses);
    // If there is a bounds error, recover by reassigning the index to
    // a valid value.
    if (aIndex < 0) {
        aIndex = 0;
    }
    else if (mNumAddresses < aIndex) {
        aIndex = mNumAddresses;
    }
#endif

    // Make the insertion and set the is-modified flag

    DW_ASSERT(aAddr != 0);
    if (aAddr != 0) {
        _InsertAddressAt(aIndex, aAddr);
        SetModified();
    }
}

 
DwAddress* DwAddressList::RemoveAddressAt(int aIndex)
{
    // Check the index for a valid value. If it's invalid, throw an exception
    // or recover.

#if defined(DW_USE_EXCEPTIONS)
    if (! (0 <= aIndex && aIndex < mNumAddresses)) {
        throw DwBoundsException();
    }
#else
    DW_ASSERT(0 <= aIndex && aIndex < mNumAddresses);
    if (! (0 <= aIndex && aIndex < mNumAddresses)) {
        return 0;
    }
#endif

    // Save the requested address as return value

    DwAddress* ret = mAddresses[aIndex];
    ret->SetParent(0);

    // Shift other addresses in the array

    for (int i=aIndex; i < mNumAddresses-1; ++i) {
        mAddresses[i] = mAddresses[i+1];
    }
    --mNumAddresses;
    mAddresses[mNumAddresses] = 0;
    SetModified();

    return ret;
}


void DwAddressList::_CopyAddresses(const DwAddressList& aAddrList)
{
    DW_ASSERT(0 == mNumAddresses);

    // Copy the addresses

    int numAddresses = aAddrList.mNumAddresses;
    for (int i=0; i < numAddresses; ++i) {
        DwAddress* address = (DwAddress*) aAddrList.mAddresses[i]->Clone();
        // If we're out of memory, and no exception is thrown, just
        // recover by returning.
        if (address == 0) {
            break;
        }
        _AddAddress(address);
    }
}


void DwAddressList::_AddAddress(DwAddress* aAddress)
{
    int index = mNumAddresses;
    _InsertAddressAt(index, aAddress);
}


void DwAddressList::_InsertAddressAt(int aIndex, DwAddress* aAddr)
{
    // Reallocate a larger array, if necessary

    if (mNumAddresses == mAddressesSize) {
        int newAddressesSize = 2*mAddressesSize;
        if (newAddressesSize == 0) {
            newAddressesSize = 10;
        }
        DwAddress** newAddresses = new DwAddress* [newAddressesSize];
        if (newAddresses == 0) {
            return;
        }
        int i;
        for (i=0; i < mNumAddresses; ++i) {
            newAddresses[i] = mAddresses[i];
            mAddresses[i] = 0;
        }
        for ( ; i < newAddressesSize; ++i) {
            newAddresses[i] = 0;
        }
        delete [] mAddresses;
        mAddresses = newAddresses;
        mAddressesSize = newAddressesSize;
    }

    // Make room in the array, then assign the address

    for (int i=mNumAddresses; i > aIndex; --i) {
        mAddresses[i] = mAddresses[i-1];
    }

    mAddresses[aIndex] = aAddr;
    ++mNumAddresses;
    aAddr->SetParent(this);
}


void DwAddressList::_DeleteAllAddresses()
{
    while (mNumAddresses > 0) {
        int i = mNumAddresses - 1;
        if (mAddresses[i] != 0) {
            delete mAddresses[i];
            mAddresses[i] = 0;
        }
        --mNumAddresses;
    }
}


#if defined (DW_DEBUG_VERSION)
void DwAddressList::PrintDebugInfo(DW_STD ostream& aStrm, int aDepth/*=0*/) const
{
    aStrm <<
    "-------------- Debug info for DwAddressList class --------------\n";
    _PrintDebugInfo(aStrm);
    int depth = aDepth - 1;
    depth = (depth >= 0) ? depth : 0;
    if (aDepth == 0 || depth > 0) {
        for (int i=0; i < mNumAddresses; ++i) {
            mAddresses[i]->PrintDebugInfo(aStrm, depth);
        }
    }
}
#else // if !defined (DW_DEBUG_VERSION)
void DwAddressList::PrintDebugInfo(DW_STD ostream& /*aStrm*/, int /*aDepth*/) const
{
}
#endif // !defined (DW_DEBUG_VERSION)


#if defined (DW_DEBUG_VERSION)
void DwAddressList::_PrintDebugInfo(DW_STD ostream& aStrm) const
{
    DwFieldBody::_PrintDebugInfo(aStrm);
    aStrm << "Address objects:  ";
    if (mNumAddresses > 0) {
        for (int i=0; i < mNumAddresses; ++i) {
            if (i > 0) {
                aStrm << ' ';
            }
            aStrm << mAddresses[i]->ObjectId();
        }
        aStrm << '\n';
    }
    else {
        aStrm << "(none)\n";
    }
}
#else // if !defined (DW_DEBUG_VERSION)
void DwAddressList::_PrintDebugInfo(DW_STD ostream& /*aStrm*/) const
{
}
#endif // !defined (DW_DEBUG_VERSION)


void DwAddressList::CheckInvariants() const
{
#if defined (DW_DEBUG_VERSION)
    for (int i=0; i < mNumAddresses; ++i) {
        mAddresses[i]->CheckInvariants();
        DW_ASSERT((DwMessageComponent*) this == mAddresses[i]->Parent());
    }
#endif // defined (DW_DEBUG_VERSION)
}


//-------------------------------------------------------------------------


DwAddressListParser::DwAddressListParser(const DwString& aStr)
  : mTokenizer(aStr),
    mAddrString(aStr)
{
    mAddrType = kAddrError;
    ParseNextAddress();
}


DwAddressListParser::~DwAddressListParser()
{
}


const DwString& DwAddressListParser::AddrString()
{
    return mAddrString.Tokens();
}


int DwAddressListParser::AddrType()
{
    return mAddrType;
}


int DwAddressListParser::IsGroup()
{
    return (mAddrType == kAddrGroup) ? 1 : 0;
}


int DwAddressListParser::IsMailbox()
{
    return (mAddrType == kAddrMailbox) ? 1 : 0;
}


int DwAddressListParser::IsNull()
{
    return (mAddrType == kAddrNull) ? 1 : 0;
}


int DwAddressListParser::IsEnd()
{
    return (mAddrType == kAddrEnd) ? 1 : 0;
}


int DwAddressListParser::Restart()
{
    mTokenizer.Restart();
    ParseNextAddress();
    return mAddrType;
}


int DwAddressListParser::operator ++ ()
{
    ParseNextAddress();
    return mAddrType;
}


void DwAddressListParser::ParseNextAddress()
{
    mAddrString.SetFirst(mTokenizer);
    mAddrType = kAddrEnd;
    int type = mTokenizer.Type();
    if (type == kTkNull) {
        return;
    }
    enum {
        kTopLevel,
        kInGroup,
        kInRouteAddr
    } state;
    state = kTopLevel;
    // The type will be a mailbox, unless we discover otherwise
    mAddrType = kAddrMailbox;
    int done = 0;
    while (!done) {
        if (type == kTkNull) {
            mAddrString.ExtendTo(mTokenizer);
            break;
        }
        else if (type == kTkSpecial) {
            int ch = mTokenizer.Token()[0];
            switch (state) {
            case kTopLevel:
                switch (ch) {
                case ',':
                    mAddrString.ExtendTo(mTokenizer);
                    done = 1;
                    break;
                case '<':
                    state = kInRouteAddr;
                    break;
                case ':':
                    mAddrType = kAddrGroup;
                    state = kInGroup;
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
            case kInGroup:
                switch (ch) {
                case ';':
                    state = kTopLevel;
                    break;
                }
                break;
            }
        }
        ++mTokenizer;
        type = mTokenizer.Type();
    }
    if (mAddrString.Tokens().length() == 0) {
        mAddrType = kAddrNull;
    }
}
