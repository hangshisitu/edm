//=============================================================================
// File:       fieldbdy.cpp
// Contents:   Definitions for DwFieldBody
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
#include <mimepp/fieldbdy.h>
#include <mimepp/field.h>


const char* const DwFieldBody::sClassName = "DwFieldBody";


DwFieldBody::DwFieldBody()
{
    mLineOffset = 0;
    mDoFolding = DwTrue;
    mClassId = kCidFieldBody;
    mClassName = sClassName;
}


DwFieldBody::DwFieldBody(const DwFieldBody& aFieldBody)
  : DwMessageComponent(aFieldBody)
{
    mLineOffset = aFieldBody.mLineOffset;
    mDoFolding = aFieldBody.mDoFolding;
    mClassId = kCidFieldBody;
    mClassName = sClassName;
}


DwFieldBody::DwFieldBody(const DwString& aStr, DwMessageComponent* aParent)
  : DwMessageComponent(aStr, aParent)
{
    mLineOffset = 0;
    mDoFolding = DwTrue;
    mClassId = kCidFieldBody;
    mClassName = sClassName;
}


DwFieldBody::~DwFieldBody()
{
}


const DwFieldBody& DwFieldBody::operator = (const DwFieldBody& aFieldBody)
{
    if (this == &aFieldBody) return *this;
    DwMessageComponent::operator = (aFieldBody);
    mLineOffset = aFieldBody.mLineOffset;
    return *this;
}


void DwFieldBody::SetOffset(int aOffset) 
{ 
    mLineOffset = aOffset; 
}


void DwFieldBody::SetFolding(DwBool aTrueOrFalse)
{
    mDoFolding = aTrueOrFalse;
}


DwBool DwFieldBody::IsFolding() const
{
    return mDoFolding;
}


#if defined (DW_DEBUG_VERSION)
void DwFieldBody::PrintDebugInfo(DW_STD ostream& aStrm, int /*aDepth*/) const
{
    aStrm <<
    "--------------- Debug info for DwFieldBody class ---------------\n";
    _PrintDebugInfo(aStrm);
}
#else // if !defined (DW_DEBUG_VERSION)
void DwFieldBody::PrintDebugInfo(DW_STD ostream& /*aStrm*/, int /*aDepth*/) const
{
}
#endif // !defined (DW_DEBUG_VERSION)


#if defined (DW_DEBUG_VERSION)
void DwFieldBody::_PrintDebugInfo(DW_STD ostream& aStrm) const
{
    DwMessageComponent::_PrintDebugInfo(aStrm);
    aStrm << "LineOffset:       " << mLineOffset << '\n';
    aStrm << "IsFolding:        " << (IsFolding() ? "True" : "False") << '\n';
}
#else // if !defined (DW_DEBUG_VERSION)
void DwFieldBody::_PrintDebugInfo(DW_STD ostream& /*aStrm*/) const
{
}
#endif // !defined (DW_DEBUG_VERSION)


void DwFieldBody::CheckInvariants() const
{
#if defined (DW_DEBUG_VERSION)
    DwMessageComponent::CheckInvariants();
#endif // defined (DW_DEBUG_VERSION)
}

