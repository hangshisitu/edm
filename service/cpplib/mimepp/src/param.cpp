//=============================================================================
// File:       param.cpp
// Contents:   Definitions for DwParameter
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
#include <mimepp/param.h>
#include <mimepp/token.h>


const char* const DwParameter::sClassName = "DwParameter";


DwParameter* (*DwParameter::sNewParameter)(const DwString&,
    DwMessageComponent*) = 0;


DwParameter* DwParameter::NewParameter(const DwString& aStr,
    DwMessageComponent* aParent)
{
    if (sNewParameter != 0) {
        return sNewParameter(aStr, aParent);
    }
    else {
        return new DwParameter(aStr, aParent);
    }
}


DwParameter::DwParameter()
{
    mClassId = kCidParameter;
    mClassName = sClassName;
}


DwParameter::DwParameter(const DwParameter& aParam)
  : DwMessageComponent(aParam),
    mAttribute(aParam.mAttribute),
    mValue(aParam.mValue)
{
    mClassId = kCidParameter;
    mClassName = sClassName;
}


DwParameter::DwParameter(const DwString& aStr, DwMessageComponent* aParent)
    : DwMessageComponent(aStr, aParent)
{
    mClassId = kCidParameter;
    mClassName = sClassName;
}


DwParameter::~DwParameter()
{
}


const DwParameter& DwParameter::operator = (const DwParameter& aParam)
{
    if (this == &aParam) {
        return *this;
    }
    DwMessageComponent::operator = (aParam);
    mAttribute = aParam.mAttribute;
    mValue     = aParam.mValue;
    return *this;
}


const DwString& DwParameter::Attribute() const
{
    return mAttribute;
}


void DwParameter::SetAttribute(const DwString& aAttribute)
{
    mAttribute = aAttribute;
    SetModified();
}


const DwString& DwParameter::Value() const
{
    return mValue;
}


void DwParameter::SetValue(const DwString& aValue)
{
    mValue = aValue;
    SetModified();
}


void DwParameter::Parse()   
{
    mIsModified = DwFalse;
    mAttribute = mValue = "";
    if (mString.length() == 0) {
        return;
    }
    DwRfc1521Tokenizer tokenizer(mString);
    // Get attribute
    DwBool found = DwFalse;
    while (!found && tokenizer.Type() != kTkNull) {
        if (tokenizer.Type() == kTkToken) {
            mAttribute = tokenizer.Token();

            found = DwTrue;
        }
        ++tokenizer;
    }
    // Get '='
    found = DwFalse;
    while (!found && tokenizer.Type() != kTkNull) {
        if (tokenizer.Type() == kTkTspecial
            && tokenizer.Token()[0] == '=') {

            found = DwTrue;
        }
        ++tokenizer;
    }
    // Get value
    found = DwFalse;
    while (!found && tokenizer.Type() != kTkNull) {
        if (tokenizer.Type() == kTkToken) {
            mValue = tokenizer.Token();

            found = DwTrue;
        }
        else if (tokenizer.Type() == kTkQuotedString) {
            tokenizer.StripDelimiters();
            mValue = tokenizer.Token();

            found = DwTrue;
        }
        ++tokenizer;
    }
    // Some nonstandard MIME implementations use single quotes to quote
    // the boundary string.  This is incorrect, but we will try to detect
    // it and work with it.
    //
    // If the first character and last character of the boundary string
    // are single quote, strip them off.
    if (DwStrcasecmp(mAttribute, "boundary") == 0) {
        size_t len = mValue.length();
        if (len > 2 && mValue[0] == '\'' && mValue[len-1] == '\'') {
            mValue = mValue.substr(1, len-2);
        }
    }
}


void DwParameter::Assemble()
{
    if (! mIsModified) {
        return;
    }
    mString = "";
    mString += mAttribute;
    mString += "=\"";
    mString += mValue;
    mString += "\"";
    mIsModified = DwFalse;
}


DwMessageComponent* DwParameter::Clone() const
{
    return new DwParameter(*this);
}


#if defined (DW_DEBUG_VERSION)
void DwParameter::PrintDebugInfo(DW_STD ostream& aStrm, int /*aDepth*/) const
{
    aStrm << 
    "--------------- Debug info for DwParameter class ---------------\n";
    _PrintDebugInfo(aStrm);
}
#else // if !defined (DW_DEBUG_VERSION)
void DwParameter::PrintDebugInfo(DW_STD ostream& /*aStrm*/, int /*aDepth*/) const
{
}
#endif // !defined (DW_DEBUG_VERSION)


#if defined (DW_DEBUG_VERSION)
void DwParameter::_PrintDebugInfo(DW_STD ostream& aStrm) const
{
    DwMessageComponent::_PrintDebugInfo(aStrm);
    aStrm << "Attribute:        " << mAttribute << '\n';
    aStrm << "Value:            " << mValue << '\n';
}
#else // if !defined (DW_DEBUG_VERSION)
void DwParameter::_PrintDebugInfo(DW_STD ostream& /*aStrm*/) const
{
}
#endif // !defined (DW_DEBUG_VERSION)


void DwParameter::CheckInvariants() const
{
#if defined (DW_DEBUG_VERSION)
    DwMessageComponent::CheckInvariants();
    mAttribute.CheckInvariants();
    mValue.CheckInvariants();
#endif // defined (DW_DEBUG_VERSION)
}

