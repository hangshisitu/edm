//=============================================================================
// File:       address.cpp
// Contents:   Definitions for DwAddress
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

#include <mimepp/address.h>
#include <mimepp/token.h>
#include <mimepp/group.h>
#include <mimepp/mailbox.h>

const char* const DwAddress::sClassName = "DwAddress";


DwAddress::DwAddress()
{
    mIsValid = DwFalse;
    mClassId = kCidAddress;
    mClassName = sClassName;
}


DwAddress::DwAddress(const DwAddress& aAddr)
  : DwFieldBody(aAddr)
{
    mIsValid = aAddr.mIsValid;
    mClassId = kCidAddress;
    mClassName = sClassName;
}


DwAddress::DwAddress(const DwString& aStr, DwMessageComponent* aParent)
  : DwFieldBody(aStr, aParent)
{
    mIsValid = DwFalse;
    mClassId = kCidAddress;
    mClassName = sClassName;
}


DwAddress::~DwAddress()
{
}


const DwAddress& DwAddress::operator = (const DwAddress& aAddr)
{
    if (this == &aAddr) {
        return *this;
    }
    DwFieldBody::operator = (aAddr);
    mIsValid = aAddr.mIsValid;
    return *this;
}


DwBool DwAddress::IsMailbox() const
{
    DwBool ret = (mClassId == kCidMailbox) ? DwTrue : DwFalse;
    return ret;
}


DwBool DwAddress::IsGroup() const
{
    DwBool ret = (mClassId == kCidGroup) ? DwTrue : DwFalse;
    return ret;
}


DwBool DwAddress::IsValid() const
{
    return mIsValid;
}


#if defined (DW_DEBUG_VERSION)
void DwAddress::PrintDebugInfo(DW_STD ostream& aStrm, int /*aDepth*/) const
{
    aStrm << 
    "---------------- Debug info for DwAddress class ----------------\n";
    _PrintDebugInfo(aStrm);
}
#else // if !defined (DW_DEBUG_VERSION)
void DwAddress::PrintDebugInfo(DW_STD ostream& /*aStrm*/, int /*aDepth*/) const
{
}
#endif // !defined (DW_DEBUG_VERSION)


#if defined (DW_DEBUG_VERSION)
void DwAddress::_PrintDebugInfo(DW_STD ostream& aStrm) const
{
    DwFieldBody::_PrintDebugInfo(aStrm);
    aStrm << "IsValid:          ";
    if (mIsValid) {
        aStrm << "True\n";
    }
    else {
        aStrm << "False\n";
    }
}
#else // if !defined (DW_DEBUG_VERSION)
void DwAddress::_PrintDebugInfo(DW_STD ostream& /*aStrm*/) const
{
}
#endif // !defined (DW_DEBUG_VERSION)


void DwAddress::CheckInvariants() const
{
#if defined (DW_DEBUG_VERSION)
    DwFieldBody::CheckInvariants();
#endif // defined (DW_DEBUG_VERSION)
}
