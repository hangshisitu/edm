//=============================================================================
// File:       entity.cpp
// Contents:   Definitions for DwEntity
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
#include <mimepp/entity.h>
#include <mimepp/headers.h>
#include <mimepp/body.h>
#include <mimepp/mediatyp.h>


class DwEntityParser {
    friend class DwEntity;
private:
    DwEntityParser(const DwString&);
    void Parse();
    const DwString mString;
    DwString mHeaders;
    DwString mBody;
};


DwEntityParser::DwEntityParser(const DwString& aStr)
  : mString(aStr)
{
    Parse();
}


void DwEntityParser::Parse()
{
    const char* buf = mString.data();
    size_t bufEnd = mString.length();
    size_t pos = 0;
    size_t headersStart = 0;
    size_t headersLength = 0;
//    size_t lineStart = pos;
//    DwBool isHeaderLine = DwFalse;
    // If first character is a LF (ANSI C or UNIX)
    // or if first two characters are CR LF (MIME or DOS),
    // there are no headers.
    if (pos < bufEnd && buf[pos] != '\n' 
        && ! (buf[pos] == '\r' && pos+1 < bufEnd && buf[pos+1] == '\n')) {

        while (pos < bufEnd) {
			// changed by ele: 2003.06.18 -->
            // End of line marked by LF
			if (buf[pos] == '\n') {
				++pos;
				// Check for LF LF
				if (pos < bufEnd && buf[pos] == '\n') {
					break;
				}
				if (pos+1 < bufEnd && buf[pos] == '\r'
					&& buf[pos+1] == '\n') {
						break;
				}
			}
			// End of line marked by CRLF
			else if (buf[pos] == '\r' && pos+1 < bufEnd
				&& buf[pos+1] == '\n') {
					pos += 2;
					// Check for CR LF CR LF
					if (pos < bufEnd &&  buf[pos] == '\n')
						break;
					if (pos+1 < bufEnd && buf[pos] == '\r'
						&& buf[pos+1] == '\n') {
							break;
						}
				}
			else {
				++pos;
			}
			// <------------------------------
			// below is origin source
			// there r some bugs where I sign out
			// I think MIMEPP added isHeaderLine was to fix the bugs
			/*
            if (buf[pos] == '\n') {
                ++pos;
                if (!isHeaderLine) {
                    pos = lineStart;
                    break;
                }
                // Check for LF LF
                else if (pos < bufEnd && buf[pos+1] == '\n') { //!!!before has ++pos, so buf[pos+1] will not be '\n'
                    break;
                }
                lineStart = pos;
                isHeaderLine = DwFalse;
            }
            // End of line marked by CRLF
            else if (buf[pos] == '\r' && pos+1 < bufEnd
                && buf[pos+1] == '\n') {
                pos += 2;
                if (!isHeaderLine) {
                    pos = lineStart;
                    break;
                }
                // Check for CR LF CR LF
                else if (pos+1 < bufEnd && buf[pos] == '\r'
                    && buf[pos+1] == '\n') {
                    break;
                }
                lineStart = pos;
                isHeaderLine = DwFalse;
            }
            else if (buf[pos] == ':') {
                isHeaderLine = DwTrue;
                ++pos;
            }
            else if (pos == lineStart &&
                (buf[pos] == ' ' || buf[pos] == '\t')) {
                isHeaderLine = DwTrue;
                ++pos;
            }
            else {
                ++pos;
            }
			*/
        }
    }
    headersLength = pos;
    mHeaders = mString.substr(headersStart, headersLength);
    // Skip blank line
    // LF (ANSI C or UNIX)
    if (pos < bufEnd && buf[pos] == '\n') {
        ++pos;
    }
    // CR LF (MIME or DOS)
    else if (pos < bufEnd && buf[pos] == '\r'
        && pos+1 < bufEnd && buf[pos+1] == '\n') {

        pos += 2;
    }
    size_t bodyStart = pos;
    size_t bodyLength = mString.length() - bodyStart;
    mBody = mString.substr(bodyStart, bodyLength);
}


//==========================================================================


const char* const DwEntity::sClassName = "DwEntity";


DwEntity::DwEntity()
{
    mClassId = kCidEntity;
    mClassName = sClassName;
    mHeaders = 0;
    mBody = 0;

    mHeaders = DwHeaders::NewHeaders("", this);
    DW_ASSERT(mHeaders != 0);
    mBody = DwBody::NewBody("", this);
    DW_ASSERT(mBody != 0);
}


DwEntity::DwEntity(const DwEntity& aEntity)
  : DwMessageComponent(aEntity)
{
    mClassId = kCidEntity;
    mClassName = sClassName;
    mHeaders = 0;
    mBody = 0;

    mHeaders = (DwHeaders*) aEntity.mHeaders->Clone();
    DW_ASSERT(mHeaders != 0);
    mHeaders->SetParent(this);
    mBody = (DwBody*) aEntity.mBody->Clone();
    DW_ASSERT(mBody != 0);
    mBody->SetParent(this);
}


DwEntity::DwEntity(const DwString& aStr, DwMessageComponent* aParent /* =0 */)
  : DwMessageComponent(aStr, aParent)
{
    mClassId = kCidEntity;
    mClassName = sClassName;
    mHeaders = 0;
    mBody = 0;

    mHeaders = DwHeaders::NewHeaders("", this);
    DW_ASSERT(mHeaders != 0);
    mBody = DwBody::NewBody("", this);
    DW_ASSERT(mBody != 0);
}


DwEntity::~DwEntity()
{
    if (mHeaders != 0) {
        delete mHeaders;
        mHeaders = 0;
    }
    if (mBody != 0) {
        delete mBody;
        mBody = 0;
    }
}


const DwEntity& DwEntity::operator = (const DwEntity& aEntity)
{
    if (this == &aEntity) {
        return *this;
    }
    DwMessageComponent::operator = (aEntity);
    // Note: Because of the derived assignment problem, we cannot use the
    // assignment operator for DwHeaders and DwBody in the following.
    if (mHeaders != 0) {
        delete mHeaders;
        mHeaders = 0;
    }
    if (mBody != 0) {
        delete mBody;
        mBody = 0;
    }
    mHeaders = (DwHeaders*) aEntity.mHeaders->Clone();
    DW_ASSERT(mHeaders != 0);
    mHeaders->SetParent(this);
    mBody = (DwBody*) aEntity.mBody->Clone();
    DW_ASSERT(mBody != 0);
    mBody->SetParent(this);
    if (mParent != 0) {
        mParent->SetModified();
    }
    return *this;
}


void DwEntity::Parse()
{
    mIsModified = DwFalse;
    DwEntityParser parser(mString);
    mHeaders->FromString(parser.mHeaders);
    mBody->FromString(parser.mBody);
    mHeaders->Parse();
    mBody->Parse();
}


void DwEntity::Assemble()
{
    if (!mIsModified) {
        return;
    }
    mString = "";
    mBody->Assemble();
    mHeaders->Assemble();
    mString += mHeaders->AsString();
    mString += DW_EOL;
    mString += mBody->AsString();
    mIsModified = DwFalse;
}


DwHeaders& DwEntity::Headers() const
{
    DW_ASSERT(mHeaders != 0);
    return *mHeaders;
}


DwBody& DwEntity::Body() const
{
    DW_ASSERT(mBody != 0);
    return *mBody;
}


#if defined(DW_DEBUG_VERSION)
void DwEntity::PrintDebugInfo(DW_STD ostream& aStrm, int aDepth) const
{
    aStrm << "------------ Debug info for DwEntity class ------------\n";
    _PrintDebugInfo(aStrm);
    int depth = aDepth - 1;
    depth = (depth >= 0) ? depth : 0;
    if (aDepth == 0 || depth > 0) {
        mHeaders->PrintDebugInfo(aStrm, depth);
        mBody->PrintDebugInfo(aStrm, depth);
    }
}
#else // if !defined(DW_DEBUG_VERSION)
void DwEntity::PrintDebugInfo(DW_STD ostream& /*aStrm*/, int /*aDepth*/) const
{
}
#endif // !defined(DW_DEBUG_VERSION)


#if defined(DW_DEBUG_VERSION)
void DwEntity::_PrintDebugInfo(DW_STD ostream& aStrm) const
{
    DwMessageComponent::_PrintDebugInfo(aStrm);
    aStrm << "Headers:          " << mHeaders->ObjectId() << '\n';
    aStrm << "Body:             " << mBody->ObjectId() << '\n';
}
#else // if !defined(DW_DEBUG_VERSION)
void DwEntity::_PrintDebugInfo(DW_STD ostream& /*aStrm*/) const
{
}
#endif // !defined(DW_DEBUG_VERSION)


void DwEntity::CheckInvariants() const
{
#if defined(DW_DEBUG_VERSION)
    DwMessageComponent::CheckInvariants();
    DW_ASSERT(mHeaders != 0);
    DW_ASSERT(mBody != 0);
    mHeaders->CheckInvariants();
    DW_ASSERT((DwMessageComponent*) this == mHeaders->Parent());
    mBody->CheckInvariants();
    DW_ASSERT((DwMessageComponent*) this == mBody->Parent());
#endif // defined(DW_DEBUG_VERSION)
}
