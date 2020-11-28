//=============================================================================
// File:       mechansm.cpp
// Contents:   Definitions for DwMechanism
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
#include <mimepp/mechansm.h>
#include <mimepp/enum.h>


const char* const DwMechanism::sClassName =
    "DwMechanism";


DwMechanism* (*DwMechanism::sNewMechanism)(const DwString&,
    DwMessageComponent*) = 0;


DwMechanism* DwMechanism::NewMechanism(const DwString& aStr, 
    DwMessageComponent* aParent)
{
    if (sNewMechanism) {
        return sNewMechanism(aStr, aParent);
    }
    else {
        return new DwMechanism(aStr, aParent);
    }
        
}


DwMechanism::DwMechanism()
{
    mCteEnum = DwMime::kCteNull;
    mClassId = kCidMechanism;
    mClassName = sClassName;
}


DwMechanism::DwMechanism(const DwMechanism& aMech)
  : DwFieldBody(aMech)
{
    mCteEnum = aMech.mCteEnum;
    mClassId = kCidMechanism;
    mClassName = sClassName;
}


DwMechanism::DwMechanism(const DwString& aStr, DwMessageComponent* aParent)
  : DwFieldBody(aStr, aParent)
{
    mCteEnum = DwMime::kCteNull;
    mClassId = kCidMechanism;
    mClassName = sClassName;
}


DwMechanism::~DwMechanism()
{
}


const DwMechanism& DwMechanism::operator = (const DwMechanism& aCte)
{
    if (this == &aCte) return *this;
    DwFieldBody::operator = (aCte);
    mCteEnum = aCte.mCteEnum;
    return *this;
}


int DwMechanism::AsEnum() const
{
    return mCteEnum;
}


void DwMechanism::FromEnum(int aEnum)
{
    mCteEnum = aEnum;
    EnumToString();
    SetModified();
}


void DwMechanism::Parse()
{
    mIsModified = DwFalse;
    StringToEnum();
}


void DwMechanism::Assemble()
{
    mIsModified = DwFalse;
}


DwMessageComponent* DwMechanism::Clone() const
{
    return new DwMechanism(*this);
}


void DwMechanism::EnumToString()
{
    switch (mCteEnum) {
    case DwMime::kCte7bit:
        mString = "7bit";
        break;
    case DwMime::kCte8bit:
        mString = "8bit";
        break;
    case DwMime::kCteBinary:
        mString = "binary";
        break;
    case DwMime::kCteBase64:
        mString = "base64";
        break;
    case DwMime::kCteQuotedPrintable:
        mString = "quoted-printable";
        break;
    }
}


void DwMechanism::StringToEnum()
{
    if (mString.length() == 0) {
        mCteEnum = DwMime::kCteNull;
    return;
    }
    int ch = mString[0];
    switch (ch) {
    case '7':
        if (DwStrcasecmp(mString, "7bit") == 0) {
            mCteEnum = DwMime::kCte7bit;
        }
        break;
    case '8':
        if (DwStrcasecmp(mString, "8bit") == 0) {
            mCteEnum = DwMime::kCte8bit;
        }
        break;
    case 'B':
    case 'b':
        if (DwStrcasecmp(mString, "base64") == 0) {
            mCteEnum = DwMime::kCteBase64;
        }
        else if (DwStrcasecmp(mString, "binary") == 0) {
            mCteEnum = DwMime::kCteBinary;
        }
        break;
    case 'Q':
    case 'q':
        if (DwStrcasecmp(mString, "quoted-printable") == 0) {
            mCteEnum = DwMime::kCteQuotedPrintable;
        }
        break;
    default:
        mCteEnum = DwMime::kCteUnknown;
        break;
    }
}


#if defined (DW_DEBUG_VERSION)
void DwMechanism::PrintDebugInfo(DW_STD ostream& aStrm, int /*aDepth*/) const
{
    aStrm <<
    "--------------- Debug info for DwMechanism class ---------------\n";
    _PrintDebugInfo(aStrm);
}
#else // if !defined (DW_DEBUG_VERSION)
void DwMechanism::PrintDebugInfo(DW_STD ostream& /*aStrm*/, int /*aDepth*/) const
{
}
#endif // !defined (DW_DEBUG_VERSION)


#if defined (DW_DEBUG_VERSION)
void DwMechanism::_PrintDebugInfo(DW_STD ostream& aStrm) const
{
    DwFieldBody::_PrintDebugInfo(aStrm);
    aStrm << "Cte enum:         " << mCteEnum << '\n';
}
#else // if !defined (DW_DEBUG_VERSION)
void DwMechanism::_PrintDebugInfo(DW_STD ostream& /*aStrm*/) const
{
}
#endif // !defined (DW_DEBUG_VERSION)


void DwMechanism::CheckInvariants() const
{
#if defined (DW_DEBUG_VERSION)
    DwFieldBody::CheckInvariants();
#endif // defined (DW_DEBUG_VERSION)
}

