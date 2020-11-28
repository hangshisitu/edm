//=============================================================================
// File:       disptype.cpp
// Contents:   Definitions for DwDispositionType
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

#include <mimepp/string.h>
#include <mimepp/param.h>
#include <mimepp/disptype.h>
#include <mimepp/token.h>
#include <mimepp/enum.h>
#include <mimepp/bounds.h>


const char* const DwDispositionType::sClassName = "DwDispositionType";
DwDispositionType* (*DwDispositionType::sNewDispositionType)(
    const DwString&, DwMessageComponent*) = 0;


DwDispositionType* DwDispositionType::NewDispositionType(
    const DwString& aStr, DwMessageComponent* aParent)
{
    if (sNewDispositionType != 0) {
        return sNewDispositionType(aStr, aParent);
    }
    else {
        return new DwDispositionType(aStr, aParent);
    }
}


DwDispositionType::DwDispositionType()
{
    DwDispositionType_Init();
}


DwDispositionType::DwDispositionType(const DwDispositionType& aDispType)
  : DwFieldBody(aDispType),
    mDispositionTypeStr(aDispType.mDispositionTypeStr),
    mFilenameStr(aDispType.mFilenameStr)
{
    DwDispositionType_Init();

    mDispositionType = aDispType.mDispositionType;
    _CopyParameters(aDispType);
}


DwDispositionType::DwDispositionType(const DwString& aStr, 
    DwMessageComponent* aParent)
  : DwFieldBody(aStr, aParent)
{
    DwDispositionType_Init();
}


DwDispositionType::~DwDispositionType()
{
    _DeleteAllParameters();
    if (mParameters != 0) {
        delete [] mParameters;
        mParameters = 0;
    }
}


void DwDispositionType::DwDispositionType_Init()
{
    mClassId = kCidDispositionType;
    mClassName = sClassName;
    mDispositionType = DwMime::kDispTypeNull;
    mNumParameters = 0;
    mParameters = 0;
    mParametersSize = 0;
    
    const int parametersSize = 10;
    mParameters = new DwParameter* [parametersSize];
    if (mParameters != 0) {
        mParametersSize = parametersSize;
        for (int i=0; i < mParametersSize; ++i) {
            mParameters[i] = 0;
        }
    }
}


const DwDispositionType& DwDispositionType::operator = (
    const DwDispositionType& aDispType)
{
    if (this != &aDispType) {
        _DeleteAllParameters();
        DwFieldBody::operator = (aDispType);
        mDispositionType    = aDispType.mDispositionType;
        mDispositionTypeStr = aDispType.mDispositionTypeStr;
        mFilenameStr        = aDispType.mFilenameStr;
        _CopyParameters(aDispType);
        if (mParent != 0) {
            mParent->SetModified();
        }
    }
    return *this;
}


void DwDispositionType::Parse()
{
    mIsModified = DwFalse;
    mDispositionType = DwMime::kDispTypeNull;
    mDispositionTypeStr = "";
    _DeleteAllParameters();
    if (mString.length() == 0) {
        return;
    }
    DwRfc1521Tokenizer tokenizer(mString);
    DwBool found = DwFalse;
    while (!found && tokenizer.Type() != kTkNull) {
        if (tokenizer.Type() == kTkToken) {
            mDispositionTypeStr = tokenizer.Token();
            found = DwTrue;
        }
        ++tokenizer;
    }
    // Get parameters
    DwTokenString tokenStr(mString);
    while (DwTrue) {
        // Get ';'
        found = DwFalse;
        while (!found && tokenizer.Type() != kTkNull) {
            if (tokenizer.Type() == kTkTspecial
                && tokenizer.Token()[0] == ';') {
                found = DwTrue;
            }
            ++tokenizer;
        }
        if (tokenizer.Type() == kTkNull) {
            // No more parameters
            break;
        }
        tokenStr.SetFirst(tokenizer);
        // Get attribute
        DwString attrib;
        DwBool attribFound = DwFalse;
        while (!attribFound && tokenizer.Type() != kTkNull) {
            if (tokenizer.Type() == kTkToken) {
                attrib = tokenizer.Token();
                attribFound = DwTrue;
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
        int valueFound = DwFalse;
        while (!valueFound && tokenizer.Type() != kTkNull) {
            if (tokenizer.Type() == kTkToken
                || tokenizer.Type() == kTkQuotedString) {
                valueFound = DwTrue;
            }
            ++tokenizer;
        }
        if (attribFound && valueFound) {
            tokenStr.ExtendTo(tokenizer);
            DwParameter* param = 
                DwParameter::NewParameter(tokenStr.Tokens(), this);
            param->Parse();
            _AddParameter(param);
        }
    }
    StrToEnum();
}


void DwDispositionType::Assemble()
{
    if (mIsModified) {
        mString = "";
        if (mDispositionTypeStr.length() > 0) {
            mString += mDispositionTypeStr;
            for (int i=0; i < mNumParameters; ++i) {
                DwParameter* param = mParameters[i];
                param->Assemble();
                if (IsFolding()) {
                    mString += ";" DW_EOL "  ";
                }
                else {
                    mString += "; ";
                }
                mString += param->AsString();
            }
            mIsModified = DwFalse;
        }
    }
}


DwMessageComponent* DwDispositionType::Clone() const
{
    return new DwDispositionType(*this);
}


int DwDispositionType::NumParameters() const
{
    return mNumParameters;
}


void DwDispositionType::AddParameter(DwParameter* aParam)
{
    DW_ASSERT(aParam != 0);
    if (aParam != 0) {
        _AddParameter(aParam);
        SetModified();
    }
}


void DwDispositionType::DeleteAllParameters()
{
    _DeleteAllParameters();
    SetModified();
}


DwParameter& DwDispositionType::ParameterAt(int aIndex) const
{
    // Check the index for a valid value. If it's invalid, throw an exception
    // or abort.
#if defined(DW_USE_EXCEPTIONS)
    if (! (0 <= aIndex && aIndex < mNumParameters)) {
        throw DwBoundsException();
    }
#else
    DW_ASSERT(0 <= aIndex && aIndex < mNumParameters);
    if (! (0 <= aIndex && aIndex < mNumParameters)) {
        abort();
    }
#endif
    return *mParameters[aIndex];
}


void DwDispositionType::InsertParameterAt(int aIndex, DwParameter* aParam)
{
    // Check the index for a valid value. If it's invalid, throw an exception
    // or recover.

#if defined(DW_USE_EXCEPTIONS)
    if (! (0 <= aIndex && aIndex <= mNumParameters)) {
        throw DwBoundsException();
    }
#else
    DW_ASSERT(0 <= aIndex && aIndex <= mNumParameters);
    // If there is a bounds error, recover by reassigning the index to
    // a valid value.
    if (aIndex < 0) {
        aIndex = 0;
    }
    else if (mNumParameters < aIndex) {
        aIndex = mNumParameters;
    }
#endif

    // Make the insertion and set the is-modified flag

    DW_ASSERT(aParam != 0);
    if (aParam != 0) {
        _InsertParameterAt(aIndex, aParam);
        SetModified();
    }
}

 
DwParameter* DwDispositionType::RemoveParameterAt(int aIndex)
{
    // Check the index for a valid value. If it's invalid, throw an exception
    // or recover.

#if defined(DW_USE_EXCEPTIONS)
    if (! (0 <= aIndex && aIndex < mNumParameters)) {
        throw DwBoundsException();
    }
#else
    DW_ASSERT(0 <= aIndex && aIndex < mNumParameters);
    if (! (0 <= aIndex && aIndex < mNumParameters)) {
        return 0;
    }
#endif

    // Save the requested parameter as return value

    DwParameter* ret = mParameters[aIndex];
    ret->SetParent(0);

    // Shift other parameters in the array

    for (int i=aIndex; i < mNumParameters-1; ++i) {
        mParameters[i] = mParameters[i+1];
    }
    --mNumParameters;
    mParameters[mNumParameters] = 0;
    SetModified();

    return ret;
}


void DwDispositionType::_CopyParameters(const DwDispositionType& aType)
{
    DW_ASSERT(0 == mNumParameters);

    // Copy the parameters

    int numParameters = aType.mNumParameters;
    for (int i=0; i < numParameters; ++i) {
        DwParameter* param = (DwParameter*) aType.mParameters[i]->Clone();
        // If we're out of memory, and no exception is thrown, just
        // recover by returning.
        if (param == 0) {
            break;
        }
        _AddParameter(param);
    }
}


void DwDispositionType::_AddParameter(DwParameter* aParameter)
{
    int index = mNumParameters;
    _InsertParameterAt(index, aParameter);
}


void DwDispositionType::_InsertParameterAt(int aIndex, DwParameter* aParam)
{
    // Reallocate a larger array, if necessary

    if (mNumParameters == mParametersSize) {
        int newParametersSize = 2*mParametersSize;
        if (newParametersSize == 0) {
            newParametersSize = 10;
        }
        DwParameter** newParameters = new DwParameter* [newParametersSize];
        if (newParameters == 0) {
            return;
        }
        int i;
        for (i=0; i < mNumParameters; ++i) {
            newParameters[i] = mParameters[i];
            mParameters[i] = 0;
        }
        for ( ; i < newParametersSize; ++i) {
            newParameters[i] = 0;
        }
        delete [] mParameters;
        mParameters = newParameters;
        mParametersSize = newParametersSize;
    }

    // Make room in the array, then assign the parameter

    for (int i=mNumParameters; i > aIndex; --i) {
        mParameters[i] = mParameters[i-1];
    }

    mParameters[aIndex] = aParam;
    ++mNumParameters;
    aParam->SetParent(this);
}


void DwDispositionType::_DeleteAllParameters()
{
    while (mNumParameters > 0) {
        int i = mNumParameters - 1;
        if (mParameters[i] != 0) {
            delete mParameters[i];
            mParameters[i] = 0;
        }
        --mNumParameters;
    }
}


int DwDispositionType::DispositionType() const
{
    return mDispositionType;
}


void DwDispositionType::SetDispositionType(int aType)
{
    mDispositionType = aType;
    EnumToStr();
    SetModified();
}


const DwString& DwDispositionType::DispositionTypeStr() const
{
    return mDispositionTypeStr;
}


void DwDispositionType::SetDispositionTypeStr(const DwString& aStr)
{
    mDispositionTypeStr = aStr;
    StrToEnum();
    SetModified();
}


const DwString& DwDispositionType::Filename() const
{
    for (int i=0; i < mNumParameters; ++i) {
        DwParameter* param = mParameters[i];
        if (DwStrcasecmp(param->Attribute(), "filename") == 0) {
            // Filename parameter found. Return its value.
            // Implementation note: this member function is const, which
            // forbids us from assigning to mFilenameStr.  The following
            // trick gets around this.  (ANSI implementations could use the
            // "mutable" declaration).
            DwDispositionType* _this = (DwDispositionType*) this;
            _this->mFilenameStr = param->Value();
            break;
        }
    }
    return mFilenameStr;
}


void DwDispositionType::SetFilename(const DwString& aStr)
{
    mFilenameStr = aStr;
    // Search for filename parameter in parameter list.  If found, set its
    // value.
    for (int i=0; i < mNumParameters; ++i) {
        DwParameter* param = mParameters[i];
        if (DwStrcasecmp(param->Attribute(), "filename") == 0) {
            param->SetValue(mFilenameStr);
            return;
        }
    }
    // Filename parameter not found. Add it.
    DwParameter* param = DwParameter::NewParameter("", 0);
    param->SetAttribute("Filename");
    param->SetValue(aStr);
    AddParameter(param);
}


void DwDispositionType::EnumToStr()
{
    switch (mDispositionType) {
    case DwMime::kDispTypeInline:
        mDispositionTypeStr = "inline";
        break;
    case DwMime::kDispTypeAttachment:
        mDispositionTypeStr = "attachment";
        break;
    }
}


void DwDispositionType::StrToEnum()
{
    switch (mDispositionTypeStr[0]) {
    case 'i':
        if (DwStrcasecmp(mDispositionTypeStr, "inline") == 0) {
            mDispositionType = DwMime::kDispTypeInline;
        }
        else {
            mDispositionType = DwMime::kDispTypeUnknown;
        }
        break;
    case 'a':
        if (DwStrcasecmp(mDispositionTypeStr, "attachment") == 0) {
            mDispositionType = DwMime::kDispTypeAttachment;
        }
        else {
            mDispositionType = DwMime::kDispTypeUnknown;
        }
        break;
    }
}


#if defined(DW_DEBUG_VERSION)
void DwDispositionType::PrintDebugInfo(DW_STD ostream& aStrm, int aDepth) const
{
    aStrm <<
    "------------ Debug info for DwDispositionType class ------------\n";
    _PrintDebugInfo(aStrm);
    int depth = aDepth - 1;
    depth = (depth >= 0) ? depth : 0;
    if (aDepth == 0 || depth > 0) {
        for (int i=0; i < mNumParameters; ++i) {
            mParameters[i]->PrintDebugInfo(aStrm, depth);
        }
    }
}
#else // if !defined(DW_DEBUG_VERSION)
void DwDispositionType::PrintDebugInfo(DW_STD ostream& /*aStrm*/, int /*aDepth*/) const
{
}
#endif // !defined(DW_DEBUG_VERSION)


#if defined(DW_DEBUG_VERSION)
void DwDispositionType::_PrintDebugInfo(DW_STD ostream& aStrm) const
{
    DwFieldBody::_PrintDebugInfo(aStrm);
    aStrm << "Disposition Type: " << mDispositionTypeStr 
        << " (" << mDispositionType    << ")\n";
    aStrm << "Filename:         " << mFilenameStr << "\n";
    aStrm << "Parameters:       ";
    if (mNumParameters > 0) {
        for (int i=0; i < mNumParameters; ++i) {
            if (i > 0) {
                aStrm << ' ';
            }
            aStrm << mParameters[i]->ObjectId();
        }
        aStrm << '\n';
    }
    else {
        aStrm << "(none)\n";
    }
}
#else // if !defined(DW_DEBUG_VERSION)
void DwDispositionType::_PrintDebugInfo(DW_STD ostream& /*aStrm*/) const
{
}
#endif // !defined(DW_DEBUG_VERSION)


void DwDispositionType::CheckInvariants() const
{
#if defined(DW_DEBUG_VERSION)
    mDispositionTypeStr.CheckInvariants();
    mFilenameStr.CheckInvariants();
    for (int i=0; i < mNumParameters; ++i) {
        mParameters[i]->CheckInvariants();
        DW_ASSERT((DwMessageComponent*) this == mParameters[i]->Parent());
    }
#endif // defined(DW_DEBUG_VERSION)
}
