//=============================================================================
// File:       mediatyp.cpp
// Contents:   Definitions for DwMediaType
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
#include <time.h>
#ifdef DW_USE_ANSI_IOSTREAM
#  include <iostream>
#else
#  include <iostream.h>
#endif

#include <mimepp/string.h>
#include <mimepp/system.h>
#include <mimepp/param.h>
#include <mimepp/mediatyp.h>
#include <mimepp/token.h>
#include <mimepp/utility.h>
#include <mimepp/enum.h>
#include <mimepp/bounds.h>


const char* const DwMediaType::sClassName = "DwMediaType";
DwMediaType* (*DwMediaType::sNewMediaType)(const DwString&,
    DwMessageComponent*) = 0;


DwMediaType* DwMediaType::NewMediaType(const DwString& aStr,
    DwMessageComponent* aParent)
{
    if (sNewMediaType != 0) {
        return sNewMediaType(aStr, aParent);
    }
    else {
        return new DwMediaType(aStr, aParent);
    }
}


DwMediaType::DwMediaType()
{
    DwMediaType_Init();
}


DwMediaType::DwMediaType(const DwMediaType& aCntType)
  : DwFieldBody(aCntType),
    mTypeStr(aCntType.mTypeStr),
    mSubtypeStr(aCntType.mSubtypeStr),
    mBoundaryStr(aCntType.mBoundaryStr)
{
    DwMediaType_Init();

    mType = aCntType.mType;
    mSubtype = aCntType.mSubtype;
    _CopyParameters(aCntType);
}


DwMediaType::DwMediaType(const DwString& aStr, DwMessageComponent* aParent)
  : DwFieldBody(aStr, aParent)
{
    DwMediaType_Init();
}


DwMediaType::~DwMediaType()
{
    _DeleteAllParameters();
    if (mParameters != 0) {
        delete [] mParameters;
        mParameters = 0;
    }
}


void DwMediaType::DwMediaType_Init()
{
    mClassId = kCidMediaType;
    mClassName = sClassName;
    mType = DwMime::kTypeNull;
    mSubtype = DwMime::kSubtypeNull;
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


const DwMediaType& DwMediaType::operator = (const DwMediaType& aCntType)
{
    if (this != &aCntType) {
        _DeleteAllParameters();
        DwFieldBody::operator = (aCntType);
        mType        = aCntType.mType;
        mSubtype     = aCntType.mSubtype;
        mTypeStr     = aCntType.mTypeStr;
        mSubtypeStr  = aCntType.mSubtypeStr;
        mBoundaryStr = aCntType.mBoundaryStr;
        _CopyParameters(aCntType);
        if (mParent != 0) {
            mParent->SetModified();
        }
    }
    return *this;
}


void DwMediaType::Parse()
{
    mIsModified = DwFalse;
    mTypeStr = "";
    mSubtypeStr = "";
    mType = DwMime::kTypeNull;
    mSubtype = DwMime::kSubtypeNull;
    _DeleteAllParameters();
    if (mString.length() == 0) {
        return;
    }
    DwRfc1521Tokenizer tokenizer(mString);

    // Get type.
    int found = 0;
    while (!found && tokenizer.Type() != kTkNull) {
        if (tokenizer.Type() == kTkToken) {
            mTypeStr = tokenizer.Token();
            found = 1;
        }
        ++tokenizer;
    }
    // Get '/'
    found = 0;
    while (!found && tokenizer.Type() != kTkNull) {
        if (tokenizer.Type() == kTkTspecial
            && tokenizer.Token()[0] == '/') {
            found = 1;
        }
        ++tokenizer;
    }
    // Get subtype
    found = 0;
    while (!found && tokenizer.Type() != kTkNull) {
        if (tokenizer.Type() == kTkToken) {
            mSubtypeStr = tokenizer.Token();
            found = 1;
        }
        ++tokenizer;
    }
    // Get parameters
    DwTokenString tokenStr(mString);
    while (DwTrue) {
        // Get ';'
        found = 0;
        while (!found && tokenizer.Type() != kTkNull) {
            if (tokenizer.Type() == kTkTspecial
                && tokenizer.Token()[0] == ';') {
                found = 1;
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
        int attribFound = 0;
        while (!attribFound && tokenizer.Type() != kTkNull) {
            if (tokenizer.Type() == kTkToken) {
                attrib = tokenizer.Token();
                attribFound = 1;
            }
            ++tokenizer;
        }
        // Get '='
        found = 0;
        while (!found && tokenizer.Type() != kTkNull) {
            if (tokenizer.Type() == kTkTspecial
                && tokenizer.Token()[0] == '=') {
                found = 1;
            }
            ++tokenizer;
        }
        // Get value
        int valueFound = 0;
        while (!valueFound && tokenizer.Type() != kTkNull) {
            if (tokenizer.Type() == kTkToken
                || tokenizer.Type() == kTkQuotedString) {
                valueFound = 1;
            }
            ++tokenizer;
        }
        if (attribFound && valueFound) {
            tokenStr.ExtendTo(tokenizer);
            DwParameter* param =
                DwParameter::NewParameter(tokenStr.Tokens(), this);
            if (param == 0) {
                break;
            }
            param->Parse();
            _AddParameter(param);
        }
    }
    TypeStrToEnum();
    SubtypeStrToEnum();
}


void DwMediaType::Assemble()
{
    if (mIsModified) {
        mString = "";
        if (mTypeStr.length() != 0 && mSubtypeStr.length() != 0) {
            mString += mTypeStr;
            mString += '/';
            mString += mSubtypeStr;
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


DwMessageComponent* DwMediaType::Clone() const
{
    return new DwMediaType(*this);
}


int DwMediaType::NumParameters() const
{
    return mNumParameters;
}


void DwMediaType::AddParameter(DwParameter* aParam)
{
    DW_ASSERT(aParam != 0);
    if (aParam != 0) {
        _AddParameter(aParam);
        SetModified();
    }
}


void DwMediaType::DeleteAllParameters()
{
    _DeleteAllParameters();
    SetModified();
}


DwParameter& DwMediaType::ParameterAt(int aIndex) const
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


void DwMediaType::InsertParameterAt(int aIndex, DwParameter* aParam)
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

 
DwParameter* DwMediaType::RemoveParameterAt(int aIndex)
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


void DwMediaType::_CopyParameters(const DwMediaType& aType)
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


void DwMediaType::_AddParameter(DwParameter* aParameter)
{
    int index = mNumParameters;
    _InsertParameterAt(index, aParameter);
}


void DwMediaType::_InsertParameterAt(int aIndex, DwParameter* aParam)
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


void DwMediaType::_DeleteAllParameters()
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


int DwMediaType::Type() const
{
    return mType;
}


void DwMediaType::SetType(int aType)
{
    mType = aType;
    TypeEnumToStr();
    SetModified();
}


const DwString& DwMediaType::TypeStr() const
{
    return mTypeStr;
}


void DwMediaType::SetTypeStr(const DwString& aStr)
{
    mTypeStr = aStr;
    TypeStrToEnum();
    SetModified();
}


int DwMediaType::Subtype() const
{
    return mSubtype;
}


void DwMediaType::SetSubtype(int aSubtype)
{
    mSubtype = aSubtype;
    SubtypeEnumToStr();
    SetModified();
}


const DwString& DwMediaType::SubtypeStr() const
{
    return mSubtypeStr;
}


void DwMediaType::SetSubtypeStr(const DwString& aStr)
{
    mSubtypeStr = aStr;
    SubtypeStrToEnum();
    SetModified();
}


const DwString& DwMediaType::Boundary() const
{
    // Implementation note: this member function is const, which
    // forbids us from assigning to mBoundaryStr.  The following
    // cast gets around this.  (ANSI implementations could use the
    // "mutable" declaration).
    DwMediaType* _this = (DwMediaType*) this;
    _this->mBoundaryStr = "";
    for (int i=0; i < mNumParameters; ++i) {
        DwParameter* param = mParameters[i];
        if (DwStrcasecmp(param->Attribute(), "boundary") == 0) {
            // Boundary parameter found. Return its value.
            _this->mBoundaryStr = param->Value();
            break;
        }
    }
    return mBoundaryStr;
}


void DwMediaType::SetBoundary(const DwString& aStr)
{
    mBoundaryStr = aStr;
    // Search for boundary parameter in parameter list.  If found, set its
    // value.
    DwBool foundIt = DwFalse;
    for (int i=0; i < mNumParameters; ++i) {
        DwParameter* param = mParameters[i];
        if (DwStrcasecmp(param->Attribute(), "boundary") == 0) {
            param->SetValue(mBoundaryStr);
            foundIt = DwTrue;
            break;
        }
    }
    if (! foundIt) {
        // Boundary parameter not found. Add it.
        DwParameter*param = DwParameter::NewParameter("", 0);
        param->SetAttribute("boundary");
        param->SetValue(aStr);
        AddParameter(param);
    }
}


void DwMediaType::CreateBoundary(unsigned aLevel)
{
    // Create a random printable string and set it as the boundary parameter
    static char c[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char buf[80];
    strcpy(buf, "------------Boundary-");
    int pos = strlen(buf);
    int n = aLevel / 10;
    buf[pos++] = c[n % 10];
    n = aLevel;
    buf[pos++] = c[n % 10];
    buf[pos++] = '=';
    buf[pos++] = '_';
    DwUint32 r = (DwUint32) time(0);
    buf[pos++] = c[r % 36];
    r /= 36;
    buf[pos++] = c[r % 36];
    r /= 36;
    buf[pos++] = c[r % 36];
    r /= 36;
    buf[pos++] = c[r % 36];
    r = DwNextInteger() + 444444444;
    int i;
    for (i=0; i < 4; ++i) {
        r = dw_rand(&r);
        buf[pos++] = c[r % 36];
        r >>= 6;
        buf[pos++] = c[r % 36];
        r >>= 6;
        buf[pos++] = c[r % 36];
        r >>= 6;
        buf[pos++] = c[r % 36];
    }
    buf[pos] = 0;
    SetBoundary(buf);
}


const DwString& DwMediaType::Name() const
{
    // Implementation note: this member function is const, which
    // forbids us from assigning to mNameStr.  The following
    // trick gets around this.  (ANSI implementations could use the
    // "mutable" declaration).
    DwMediaType* _this = (DwMediaType*) this;
    _this->mNameStr = "";
    for (int i=0; i < mNumParameters; ++i) {
        DwParameter* param = mParameters[i];
        if (DwStrcasecmp(param->Attribute(), "name") == 0) {
            // Name parameter found. Return its value.
            _this->mNameStr = param->Value();
            break;
        }
    }
    return mNameStr;
}


void DwMediaType::SetName(const DwString& aStr)
{
    mNameStr = aStr;
    // Search for name parameter in parameter list.  If found, set its
    // value.
    DwBool foundIt = DwFalse;
    for (int i=0; i < mNumParameters; ++i) {
        DwParameter* param = mParameters[i];
        if (DwStrcasecmp(param->Attribute(), "name") == 0) {
            param->SetValue(mNameStr);
            foundIt = DwTrue;
            break;
        }
    }
    if (! foundIt) {
        // Name parameter not found. Add it.
        DwParameter* param = DwParameter::NewParameter("", 0);
        param->SetAttribute("name");
        param->SetValue(aStr);
        AddParameter(param);
    }
}


void DwMediaType::TypeEnumToStr()
{
    DwTypeEnumToStr(mType, mTypeStr);
}


void DwMediaType::TypeStrToEnum()
{
    mType = DwTypeStrToEnum(mTypeStr);
}


void DwMediaType::SubtypeEnumToStr()
{
    DwSubtypeEnumToStr(mSubtype, mSubtypeStr);
}


void DwMediaType::SubtypeStrToEnum()
{
    mSubtype = DwSubtypeStrToEnum(mSubtypeStr);
}


#if defined(DW_DEBUG_VERSION)
void DwMediaType::PrintDebugInfo(DW_STD ostream& aStrm, int aDepth) const
{
    aStrm <<
    "--------------- Debug info for DwMediaType class ---------------\n";
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
void DwMediaType::PrintDebugInfo(DW_STD ostream& /*aStrm*/, int /*aDepth*/) const
{
}
#endif // !defined(DW_DEBUG_VERSION)


#if defined(DW_DEBUG_VERSION)
void DwMediaType::_PrintDebugInfo(DW_STD ostream& aStrm) const
{
    DwFieldBody::_PrintDebugInfo(aStrm);
    aStrm << "Type:             " << mTypeStr    << " (" << mType    << ")\n";
    aStrm << "Subtype:          " << mSubtypeStr << " (" << mSubtype << ")\n";
    aStrm << "Boundary:         " << mBoundaryStr << '\n';
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
void DwMediaType::_PrintDebugInfo(DW_STD ostream& /*aStrm*/) const
{
}
#endif // !defined(DW_DEBUG_VERSION)


void DwMediaType::CheckInvariants() const
{
#if defined(DW_DEBUG_VERSION)
    mTypeStr.CheckInvariants();
    mSubtypeStr.CheckInvariants();
    mBoundaryStr.CheckInvariants();
    for (int i=0; i < mNumParameters; ++i) {
        mParameters[i]->CheckInvariants();
        DW_ASSERT((DwMessageComponent*) this == mParameters[i]->Parent());
    }
#endif // defined(DW_DEBUG_VERSION)
}
