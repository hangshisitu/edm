//=============================================================================
// File:       body.cpp
// Contents:   Definitions for DwBody
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
#include <mimepp/mediatyp.h>
#include <mimepp/enum.h>

enum {
    kParseSuccess,
    kParseFail
};


struct DwBodyPartStr {
    DwBodyPartStr(const DwString& aStr) : mString(aStr), mNext(0) {}
    DwString mString;
    DwBodyPartStr* mNext;
};


class DwBodyParser {
    friend class DwBody;
public:
    ~DwBodyParser();
private:
    DwBodyParser(const DwString& aStr, const DwString& aBoundaryStr);
    const DwString& Preamble() const     { return mPreamble; }
    const DwString& Epilogue() const     { return mEpilogue; }
    DwBodyPartStr* FirstBodyPart() const { return mFirstBodyPartStr; }
    int Parse();
    int FindBoundary(size_t aStartPos, size_t* aBoundaryStart, 
        size_t* aBoundaryEnd, DwBool* isFinal) const;
    void AddPart(size_t start, size_t len);
    void DeleteParts();
    const DwString mString;
    const DwString mBoundary;
    DwString mPreamble;
    DwBodyPartStr* mFirstBodyPartStr;
    DwString mEpilogue;
};


DwBodyParser::DwBodyParser(const DwString& aStr, const DwString& aBoundary)
  : mString(aStr), mBoundary(aBoundary)
{
    mFirstBodyPartStr = 0;
    Parse();
}


DwBodyParser::~DwBodyParser()
{
    DeleteParts();
}


int DwBodyParser::Parse()
{
    DeleteParts();
    // Find the preamble
    size_t pos = 0;
    size_t boundaryStart, boundaryEnd;
    DwBool isFinal;
    int result;
    result = FindBoundary(pos, &boundaryStart, &boundaryEnd, &isFinal);
    if (result == kParseFail) {
        mPreamble = mEpilogue = "";
        mFirstBodyPartStr = 0;
        return kParseFail;
    }
    int start = pos;
    int len = boundaryStart - pos;
    mPreamble = mString.substr(start, len);

    // Find the body parts
    pos = boundaryEnd;
    while (DwTrue) {
        result = FindBoundary(pos, &boundaryStart, &boundaryEnd, &isFinal);
        if (result == kParseFail) {
            // Whoops! There is no final boundary! Let's just pretend there
            // was one, but report a failure nonetheless.
            start = pos;
            len = mString.length() - pos;
            AddPart(start, len);
            mEpilogue = "";
            return kParseFail;
        }
        start = pos;
        len = boundaryStart - pos;
        AddPart(start, len);
        pos = boundaryEnd;
        if (isFinal) {
            break;
        }
    }

    // Find the epilogue
    start = pos;
    len = mString.length() - pos;
    mEpilogue = mString.substr(start, len);

    return kParseSuccess;
}


int DwBodyParser::FindBoundary(size_t aStartPos, size_t* aBoundaryStart, 
    size_t* aBoundaryEnd, DwBool* aIsFinal) const
{
    // Assume the starting position is the beginning of a line
    const char* buf = mString.data();
    size_t pos = aStartPos;
    size_t endPos = mString.length();
    size_t boundaryLen = mBoundary.length();
    const char* boundaryPtr = mBoundary.data();
    // Search for the first boundary.
    // The leading CR LF ('\n') is part of the boundary, but if there is
    // no preamble, there may be no leading CR LF ('\n').
    // The case of no leading CR LF ('\n') is a special case that will occur
    // only when '-' is the first character of the body.
    if (pos+boundaryLen+1 < endPos
        && buf[pos] == '-'
        && buf[pos+1] == '-'
        && strncmp(&buf[pos+2], boundaryPtr, boundaryLen) == 0
		// This line is added by pqf, 2003.10.29, for CQCM00000166
		&& (buf[pos+2+boundaryLen]=='\0' || buf[pos+2+boundaryLen]=='\r' 
			|| buf[pos+2+boundaryLen]=='\n' || buf[pos+2+boundaryLen]=='-') ){
		// End of add
 
		*aBoundaryStart = pos;
        pos += boundaryLen + 2;
        // Check for final boundary
        if (pos+1 < endPos
            && buf[pos] == '-'
            && buf[pos+1] == '-') {

            pos += 2;
            *aIsFinal = DwTrue;
        }
        else {
            *aIsFinal = DwFalse;
        }
        // Advance position past end of line
        while (pos < endPos) {
            if (buf[pos] == '\n') {
                ++pos;
                break;
            }
            ++pos;
        }
        *aBoundaryEnd = pos;
        return kParseSuccess;
    }
    DwBool isFound = DwFalse;
    while (pos+boundaryLen+2 < endPos) {
        // Case of leading LF
        if (buf[pos] == '\n'
            && buf[pos+1] == '-'
            && buf[pos+2] == '-'
            && strncmp(&buf[pos+3], boundaryPtr, boundaryLen) == 0
			// This line is added by pqf, 2003.10.29, for CQCM00000166
			&& (buf[pos+3+boundaryLen]=='\0' || buf[pos+3+boundaryLen]=='\r' 
				|| buf[pos+3+boundaryLen]=='\n' || buf[pos+3+boundaryLen]=='-') ){
			// End of add

            *aBoundaryStart = pos;
            pos += boundaryLen + 3;
            isFound = DwTrue;
        }
        // Case of leading CR LF
        else if (buf[pos] == '\r'
            && buf[pos+1] == '\n'
            && buf[pos+2] == '-'
            && pos+boundaryLen+3 < endPos
            && buf[pos+3] == '-'
            && strncmp(&buf[pos+4], boundaryPtr, boundaryLen) == 0
			// This line is added by pqf, 2003.10.29, for CQCM00000166
			&& (buf[pos+4+boundaryLen]=='\0' || buf[pos+4+boundaryLen]=='\r' 
				|| buf[pos+4+boundaryLen]=='\n' || buf[pos+4+boundaryLen]=='-') ){
			// End of add

            *aBoundaryStart = pos;
            pos += boundaryLen + 4;
            isFound = DwTrue;
        }
        if (isFound) {
            // Check for final boundary
            if (pos+1 < endPos
                && buf[pos] == '-'
                && buf[pos+1] == '-') {

                pos += 2;
                *aIsFinal = DwTrue;
            }
            else {
                *aIsFinal = DwFalse;
            }
            // Advance position past end of line
            while (pos < endPos) {
                if (buf[pos] == '\n') {
                    ++pos;
                    break;
                }
                ++pos;
            }
            *aBoundaryEnd = pos;
            return kParseSuccess;
        }
        ++pos;
    }
    // Exceptional case: no boundary found
    *aBoundaryStart = *aBoundaryEnd = mString.length();
    *aIsFinal = DwTrue;
    return kParseFail;
}


void DwBodyParser::AddPart(size_t start, size_t len)
{
    DwBodyPartStr* toAdd = new DwBodyPartStr(mString.substr(start, len));
    if (toAdd != 0) {
        DwBodyPartStr* curr = mFirstBodyPartStr;
        if (curr == 0) {
            mFirstBodyPartStr = toAdd;
            return;
        }
        while (curr->mNext != 0) {
            curr = curr->mNext;
        }
        curr->mNext = toAdd;
    }
}


void DwBodyParser::DeleteParts()
{
    DwBodyPartStr* curr = mFirstBodyPartStr;
    while (curr != 0) {
        DwBodyPartStr* next = curr->mNext;
        delete curr;
        curr = next;
    }
    mFirstBodyPartStr = 0;
}


//==========================================================================


const char* const DwBody::sClassName = "DwBody";
DwBody* (*DwBody::sNewBody)(const DwString&, DwMessageComponent*) = 0;
bool DwBody::mParseToEnd = false;

DwBody* DwBody::NewBody(const DwString& aStr, DwMessageComponent* aParent)
{
    if (sNewBody != 0) {
        return sNewBody(aStr, aParent);
    }
    else {
        return new DwBody(aStr, aParent);
    }
}


DwBody::DwBody()
{
    DwBody_Init();
}


DwBody::DwBody(const DwBody& aBody)
  : DwMessageComponent(aBody),
    mPreamble(aBody.mPreamble),
    mEpilogue(aBody.mEpilogue)
{
    DwBody_Init();
    _CopyBodyParts(aBody);
    const DwMessage* message = aBody.mMessage;
    if (message != 0) {
        DwMessage* msg = (DwMessage*) message->Clone();
        _SetMessage(msg);
    }
}


DwBody::DwBody(const DwString& aStr, DwMessageComponent* aParent)
  : DwMessageComponent(aStr, aParent)
{
    DwBody_Init();
}


DwBody::~DwBody()
{
    _DeleteAllBodyParts();
    if (mBodyParts != 0) {
        delete [] mBodyParts;
        mBodyParts = 0;
    }
    if (mMessage != 0) {
        delete mMessage;
        mMessage = 0;
    }
}


void DwBody::DwBody_Init()
{
    mClassId = kCidBody;
    mClassName = sClassName;
    mNumBodyParts = 0;
    mBodyParts = 0;
    mBodyPartsSize = 0;
    mMessage = 0;
    
    const int bodyPartsSize = 10;
    mBodyParts = new DwBodyPart* [bodyPartsSize];
    if (mBodyParts != 0) {
        mBodyPartsSize = bodyPartsSize;
        for (int i=0; i < mBodyPartsSize; ++i) {
            mBodyParts[i] = 0;
        }
    }
}


const DwBody& DwBody::operator = (const DwBody& aBody)
{
    if (this != &aBody) {
        _DeleteAllBodyParts();
        if (mMessage != 0) {
            delete mMessage;
            mMessage = 0;
        }
        DwMessageComponent::operator = (aBody);
        mPreamble = aBody.mPreamble;
        mEpilogue = aBody.mEpilogue;
        _CopyBodyParts(aBody);
        if (aBody.mMessage != 0) {
            DwMessage* myMessage = (DwMessage*) aBody.mMessage->Clone();
            _SetMessage(myMessage);
        }
        if (mParent != 0) {
            mParent->SetModified();
        }
    }
    return *this;
}


void DwBody::Parse()
{
    mIsModified = DwFalse;
    _DeleteAllBodyParts();
    if (mMessage != 0) {
        delete mMessage;
        mMessage = 0;
    }

    // Only types "multipart" and "message" need to be parsed, and
    // we cannot determine the type if there is no header.

    if (mParent == 0) {
        return;
    }

    // Get the content type from the headers

    DwEntity* entity = (DwEntity*) mParent;
    if (entity->Headers().HasContentType()) {
        const DwMediaType& contentType = entity->Headers().ContentType();
        int type = contentType.Type();
        if (type == DwMime::kTypeMultipart) {
            DwString boundaryStr = contentType.Boundary();
            // Now parse body into body parts
            DwBodyParser parser(mString, boundaryStr);
            mPreamble = parser.Preamble();
            mEpilogue = parser.Epilogue();
            DwBodyPartStr* partStr = parser.FirstBodyPart();
            while (partStr != 0) {
                DwBodyPart* part =
                    DwBodyPart::NewBodyPart(partStr->mString, this);
//                part->Parse();	//!!!!! ele: changed , parse it at LettParser
				// Change by pqf again on 2002.12.24
				if( mParseToEnd )
					part->Parse();
                _InsertBodyPartAt(mNumBodyParts, part);
                partStr = partStr->mNext;
            }
        }
        else if (type == DwMime::kTypeMessage) {
            mMessage = DwMessage::NewMessage(mString, this);
//            mMessage->Parse();	//!!!!! ele: changed , parse it at LettParser
			// Change by pqf again on 2002.12.24
			if( mParseToEnd )
				mMessage->Parse();
        }
    }
}


void DwBody::Assemble()
{
    if (mIsModified) {
        DwEntity* entity = (DwEntity*) mParent;
        if (entity != 0 && entity->Headers().HasContentType()) {
            const DwMediaType& contentType = entity->Headers().ContentType();
            int type = contentType.Type();
            if (type == DwMime::kTypeMultipart) {
                DwString boundaryStr = contentType.Boundary();
                mString = "";
                mString += mPreamble;
                for (int i=0; i < mNumBodyParts; ++i) {
                    DwBodyPart* part = mBodyParts[i];
                    part->Assemble();
                    mString += DW_EOL "--";
                    mString += boundaryStr;
                    mString += DW_EOL;
                    mString += part->AsString();
                }
                mString += DW_EOL "--";
                mString += boundaryStr;
                mString += "--";
                mString += DW_EOL;
                mString += mEpilogue;
            }
            else if (type == DwMime::kTypeMessage && mMessage != 0) {
                mMessage->Assemble();
                mString = mMessage->AsString();
            }
            else {
                // Empty block
            }
        }
        mIsModified = DwFalse;
    }
}


DwMessageComponent* DwBody::Clone() const
{
    return new DwBody(*this);
}


int DwBody::NumBodyParts() const
{
    return mNumBodyParts;
}


void DwBody::AddBodyPart(DwBodyPart* aPart)
{
    DW_ASSERT(aPart != 0);
    if (aPart != 0) {
        _AddBodyPart(aPart);
        SetModified();
    }
}

 
void DwBody::DeleteAllBodyParts()
{
    _DeleteAllBodyParts();
    SetModified();
}

 
DwBodyPart& DwBody::BodyPartAt(int aIndex) const
{
    // Check the index for a valid value. If it's invalid, throw an exception
    // or abort.
#if defined(DW_USE_EXCEPTIONS)
    if (! (0 <= aIndex && aIndex < mNumBodyParts)) {
        throw DwBoundsException();
    }
#else
    DW_ASSERT(0 <= aIndex && aIndex < mNumBodyParts);
    if (! (0 <= aIndex && aIndex < mNumBodyParts)) {
        abort();
    }
#endif
    return *mBodyParts[aIndex];
}


void DwBody::InsertBodyPartAt(int aIndex, DwBodyPart* aPart)
{
    // Check the index for a valid value. If it's invalid, throw an exception
    // or recover.

#if defined(DW_USE_EXCEPTIONS)
    if (! (0 <= aIndex && aIndex <= mNumBodyParts)) {
        throw DwBoundsException();
    }
#else
    DW_ASSERT(0 <= aIndex && aIndex <= mNumBodyParts);
    // If there is a bounds error, recover by reassigning the index to
    // a valid value.
    if (aIndex < 0) {
        aIndex = 0;
    }
    else if (mNumBodyParts < aIndex) {
        aIndex = mNumBodyParts;
    }
#endif

    // Make the insertion and set the is-modified flag

    DW_ASSERT(aPart != 0);
    if (aPart != 0) {
        _InsertBodyPartAt(aIndex, aPart);
        SetModified();
    }
}


DwBodyPart* DwBody::RemoveBodyPartAt(int aIndex)
{
    // Check the index for a valid value. If it's invalid, throw an exception
    // or recover.

#if defined(DW_USE_EXCEPTIONS)
    if (! (0 <= aIndex && aIndex < mNumBodyParts)) {
        throw DwBoundsException();
    }
#else
    DW_ASSERT(0 <= aIndex && aIndex < mNumBodyParts);
    if (! (0 <= aIndex && aIndex < mNumBodyParts)) {
        return 0;
    }
#endif

    // Save the requested body part as return value

    DwBodyPart* ret = mBodyParts[aIndex];
    ret->SetParent(0);

    // Shift other body parts in the array

    for (int i=aIndex; i < mNumBodyParts-1; ++i) {
        mBodyParts[i] = mBodyParts[i+1];
    }
    --mNumBodyParts;
    mBodyParts[mNumBodyParts] = 0;
    SetModified();

    return ret;
}


void DwBody::_CopyBodyParts(const DwBody& aBody)
{
    DW_ASSERT(0 == mNumBodyParts);

    // Copy the body parts

    int numBodyParts = aBody.mNumBodyParts;
    for (int i=0; i < numBodyParts; ++i) {
        DwBodyPart* part = (DwBodyPart*) aBody.mBodyParts[i]->Clone();
        // If we're out of memory, and no exception is thrown, just
        // recover by returning.
        if (part == 0) {
            break;
        }
        _AddBodyPart(part);
    }
}


void DwBody::_AddBodyPart(DwBodyPart* aPart)
{
    int index = mNumBodyParts;
    _InsertBodyPartAt(index, aPart);
}


void DwBody::_InsertBodyPartAt(int aIndex, DwBodyPart* aPart)
{
    // Reallocate a larger array, if necessary

    if (mNumBodyParts == mBodyPartsSize) {
        int newBodyPartsSize = 2*mBodyPartsSize;
        if (newBodyPartsSize == 0) {
            newBodyPartsSize = 10;
        }
        DwBodyPart** newBodyParts = new DwBodyPart* [newBodyPartsSize];
        if (newBodyParts == 0) {
            return;
        }
        int i;
        for (i=0; i < mNumBodyParts; ++i) {
            newBodyParts[i] = mBodyParts[i];
            mBodyParts[i] = 0;
        }
        for ( ; i < newBodyPartsSize; ++i) {
            newBodyParts[i] = 0;
        }
        delete [] mBodyParts;
        mBodyParts = newBodyParts;
        mBodyPartsSize = newBodyPartsSize;
    }

    // Make room in the array, then assign the body part

    for (int i=mNumBodyParts; i > aIndex; --i) {
        mBodyParts[i] = mBodyParts[i-1];
    }

    mBodyParts[aIndex] = aPart;
    ++mNumBodyParts;
    aPart->SetParent(this);
}

 
void DwBody::_DeleteAllBodyParts()
{
    while (mNumBodyParts > 0) {
        int i = mNumBodyParts - 1;
        if (mBodyParts[i] != 0) {
            delete mBodyParts[i];
            mBodyParts[i] = 0;
        }
        --mNumBodyParts;
    }
}


DwMessage* DwBody::Message() const
{
    return mMessage;
}


void DwBody::SetMessage(DwMessage* aMessage)
{
    if (aMessage != mMessage) {
        _SetMessage(aMessage);
        SetModified();
    }
}


const DwString& DwBody::Preamble() const
{
    return mPreamble;
}


void DwBody::SetPreamble(const DwString& aStr)
{
    mPreamble = aStr;
    SetModified();
}


const DwString& DwBody::Epilogue() const
{
    return mEpilogue;
}


void DwBody::SetEpilogue(const DwString& aStr)
{
    mEpilogue = aStr;
    SetModified();
}


void DwBody::_SetMessage(DwMessage* aMessage)
{
    if (mMessage != 0 && mMessage != aMessage) {
        delete mMessage;
    }
    mMessage = aMessage;
    if (aMessage != 0) {
        aMessage->SetParent(this);
    }
}


#if defined(DW_DEBUG_VERSION)
void DwBody::PrintDebugInfo(DW_STD ostream& aStrm, int aDepth) const
{
    aStrm <<
    "------------------ Debug info for DwBody class -----------------\n";
    _PrintDebugInfo(aStrm);
    int depth = aDepth - 1;
    depth = (depth >= 0) ? depth : 0;
    if (aDepth == 0 || depth > 0) {
        if (mMessage != 0) {
            mMessage->PrintDebugInfo(aStrm, depth);
        }
        for (int i=0; i < mNumBodyParts; ++i) {
            mBodyParts[i]->PrintDebugInfo(aStrm, depth);
        }
    }
}
#else // if !defined(DW_DEBUG_VERSION)
void DwBody::PrintDebugInfo(DW_STD ostream& /*aStrm*/, int /*aDepth*/) const
{
}
#endif // !defined(DW_DEBUG_VERSION)


#if defined(DW_DEBUG_VERSION)
void DwBody::_PrintDebugInfo(DW_STD ostream& aStrm) const
{
    DwMessageComponent::_PrintDebugInfo(aStrm);
    aStrm << "Preamble:         " << mPreamble << '\n';
    aStrm << "Epilogue:         " << mEpilogue << '\n';
    aStrm << "Body Parts:       ";
    if (mNumBodyParts > 0) {
        for (int i=0; i < mNumBodyParts; ++i) {
            if (i > 0) {
                aStrm << ' ';
            }
            aStrm << mBodyParts[i]->ObjectId();
        }
        aStrm << '\n';
    }
    else {
        aStrm << "(none)\n";
    }
    aStrm << "Message:       ";
    if (mMessage != 0) {
        aStrm << mMessage->ObjectId() << '\n';
    }
    else {
        aStrm << "(none)\n";
    }
}
#else // if !defined(DW_DEBUG_VERSION)
void DwBody::_PrintDebugInfo(DW_STD ostream& /*aStrm*/) const
{
}
#endif // !defined(DW_DEBUG_VERSION)


void DwBody::CheckInvariants() const
{
#if defined(DW_DEBUG_VERSION)
    DwMessageComponent::CheckInvariants();
    mPreamble.CheckInvariants();
    mEpilogue.CheckInvariants();
    for (int i=0; i < mNumBodyParts; ++i) {
        mBodyParts[i]->CheckInvariants();
        DW_ASSERT((DwMessageComponent*) this == mBodyParts[i]->Parent());
    }
    if (mMessage != 0) {
        mMessage->CheckInvariants();
        DW_ASSERT((DwMessageComponent*) this == mMessage->Parent());
    }
#endif // defined(DW_DEBUG_VERSION)
}
