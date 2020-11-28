//=============================================================================
// File:       bodypart.cpp
// Contents:   Definitions for DwBodyPart
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

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#ifdef DW_USE_ANSI_IOSTREAM
#  include <iostream>
#else
#  include <iostream.h>
#endif

#include <mimepp/string.h>
#include <mimepp/headers.h>
#include <mimepp/bodypart.h>
#include <mimepp/body.h>
#include <mimepp/message.h>

const char* const DwBodyPart::sClassName = "DwBodyPart";


DwBodyPart* (*DwBodyPart::sNewBodyPart)(const DwString&,
    DwMessageComponent*) = 0;


DwBodyPart* DwBodyPart::NewBodyPart(const DwString& aStr,
    DwMessageComponent* aParent)
{
    if (sNewBodyPart != 0) {
        return sNewBodyPart(aStr, aParent);
    }
    else {
        return new DwBodyPart(aStr, aParent);
    }
}


DwBodyPart::DwBodyPart()
{
    mClassId = kCidBodyPart;
    mClassName = sClassName;
}


DwBodyPart::DwBodyPart(const DwBodyPart& aPart)
  : DwEntity(aPart)
{
    mClassId = kCidBodyPart;
    mClassName = sClassName;
}


DwBodyPart::DwBodyPart(const DwString& aStr, DwMessageComponent* aParent)
  : DwEntity(aStr, aParent)
{
    mClassId = kCidBodyPart;
    mClassName = sClassName;
}



DwBodyPart::~DwBodyPart()
{
}


const DwBodyPart& DwBodyPart::operator = (const DwBodyPart& aPart)
{
    if (this == &aPart) {
        return *this;
    }
    DwEntity::operator = (aPart);
    return *this;
}


DwMessageComponent* DwBodyPart::Clone() const
{
    return new DwBodyPart(*this);
}



#if defined(DW_DEBUG_VERSION)
void DwBodyPart::PrintDebugInfo(DW_STD ostream& aStrm, int aDepth) const
{
    aStrm << "----------- Debug info for DwBodyPart class -----------\n";
    _PrintDebugInfo(aStrm);
    int depth = aDepth - 1;
    depth = (depth >= 0) ? depth : 0;
    if (aDepth == 0 || depth > 0) {
        mHeaders->PrintDebugInfo(aStrm, depth);
        mBody->PrintDebugInfo(aStrm, depth);
    }
}
#else // if !defined(DW_DEBUG_VERSION)
void DwBodyPart::PrintDebugInfo(DW_STD ostream& /*aStrm*/, int /*aDepth*/) const
{
}
#endif // !defined(DW_DEBUG_VERSION)


#if defined(DW_DEBUG_VERSION)
void DwBodyPart::_PrintDebugInfo(DW_STD ostream& aStrm) const
{
    DwEntity::_PrintDebugInfo(aStrm);
}
#else // if !defined(DW_DEBUG_VERSION)
void DwBodyPart::_PrintDebugInfo(DW_STD ostream& /*aStrm*/) const
{
}
#endif // !defined(DW_DEBUG_VERSION)


void DwBodyPart::CheckInvariants() const
{
#if defined(DW_DEBUG_VERSION)
    DwEntity::CheckInvariants();
#endif // defined(DW_DEBUG_VERSION)
}

