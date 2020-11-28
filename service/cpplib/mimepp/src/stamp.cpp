//=============================================================================
// File:       stamp.cpp
// Contents:   Definitions for DwStamp
// Maintainer: Doug Sauder <doug.sauder@hunnysoft.com>
// WWW:        http://www.hunnysoft.com/mimepp.htm
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

#include <string.h> // for debugging

#include <mimepp/string.h>
#include <mimepp/utility.h>
#include <mimepp/stamp.h>


const char* const DwStamp::sClassName = "DwStamp";


DwStamp* (*DwStamp::sNewStamp)(const DwString&, DwMessageComponent*) = 0;


DwStamp* DwStamp::NewStamp(const DwString& aStr, DwMessageComponent* aParent)
{
    if (sNewStamp != 0) {
        return sNewStamp(aStr, aParent);
    }
    else {
        return new DwStamp(aStr, aParent);
    }
}


DwStamp::DwStamp()
{
    mClassId = kCidStamp;
    mClassName = sClassName;
}


DwStamp::DwStamp(const DwStamp& aStamp)
  : DwFieldBody(aStamp)
{
    mClassId = kCidStamp;
    mClassName = sClassName;
    mFrom = aStamp.mFrom;
    mBy   = aStamp.mBy;
    mVia  = aStamp.mVia;
    mWith = aStamp.mWith;
    mId   = aStamp.mId;
    mFor  = aStamp.mFor;
    mDate = aStamp.mDate;
}


DwStamp::DwStamp(const DwString& aStr, DwMessageComponent* aParent)
  : DwFieldBody(aStr, aParent)
{
    mClassId = kCidStamp;
    mClassName = sClassName;
}


DwStamp::~DwStamp()
{
}


const DwStamp& DwStamp::operator = (const DwStamp& aStamp)
{
    if (this == &aStamp) {
        return *this;
    }
    DwFieldBody::operator = (aStamp);
    mFrom = aStamp.mFrom;
    mBy   = aStamp.mBy;
    mVia  = aStamp.mVia;
    mWith = aStamp.mWith;
    mId   = aStamp.mId;
    mFor  = aStamp.mFor;
    mDate = aStamp.mDate;
    return *this;
}


void DwStamp::Parse()
{
    DwString emptyStr;
    mFrom = emptyStr;
    mBy = emptyStr;
    mVia = emptyStr;
    mWith = emptyStr;
    mId = emptyStr;
    mFor = emptyStr;
    mDate.FromString(emptyStr);
    mDate.FromUnixTime(0, DwFalse);

    enum {
        kStarting=0,
        kInFrom,
        kInBy,
        kInVia,
        kInWith,
        kInId,
        kInFor
    };

    DwString str = mString;
    DwUnfoldLine(str);
    int strPos  = 0;
    int strLen  = (int) str.length();
    int fromPos = -1;
    int fromLen =  0;
    int byPos   = -1;
    int byLen   =  0;
    int viaPos  = -1;
    int viaLen  =  0;
    int withPos = -1;
    int withLen =  0;
    int idPos   = -1;
    int idLen   =  0;
    int forPos  = -1;
    int forLen  =  0;
    int datePos = -1;
    int dateLen = 0;
    DwString word;
    size_t n;

    // Search backwards for semicolon. The semicolon separates the date-time.
    // Since this field body is called "timestamp" by RFC 1123, the date
    // really should be present!

    size_t semicolonPos = str.rfind(';');
    if (semicolonPos != DwString::npos) {
        n = str.find_first_not_of(" \t\r\n", semicolonPos+1);
        n = (n != DwString::npos) ? n : strLen;
        datePos = (int) n;
        dateLen = strLen - datePos;
        strLen = semicolonPos;
    }

    // Parse words. Examine them to determine the state.
    //
    // A "word" can be any sequence of non-spaces or a comment delimited by
    // parentheses.

    int state = kStarting;
    while (strPos < strLen) {

        // Get a word

        n = str.find_first_not_of(" \t\r\n", strPos);
        int wordPos = (n != DwString::npos) ? (int) n : strLen;
        int wordLen = 0;
        if (wordPos < strLen) {
            if (str[wordPos] == '(') {
                n = str.find(')', wordPos);
                if (n != DwString::npos) {
                    wordLen = n - wordPos + 1;
                }
            }
            if (wordLen == 0) {
                n = str.find_first_of(" \t\r\n", wordPos);
                n = (n != DwString::npos) ? n : strLen;
                wordLen = n - wordPos;
            }
            if (wordPos + wordLen > strLen) {
                wordLen = strLen - wordPos;
            }
        }
        word = str.substr(wordPos, wordLen);

        // Process a word

        if (wordLen > 0) {

            switch (state) {

            case kStarting:

                // check for a keyword

                if (DwStrcasecmp(word, "from") == 0) {
                    state = kInFrom;
                }
                else if (DwStrcasecmp(word, "by") == 0) {
                    state = kInBy;
                }
                else if (DwStrcasecmp(word, "via") == 0) {
                    state = kInVia;
                }
                else if (DwStrcasecmp(word, "with") == 0) {
                    state = kInWith;
                }
                else if (DwStrcasecmp(word, "id") == 0) {
                    state = kInId;
                }
                else if (DwStrcasecmp(word, "for") == 0) {
                    state = kInFor;
                }
                break;

            case kInFrom:

                // check for a keyword

                if (DwStrcasecmp(word, "by") == 0) {
                    state = kInBy;
                }
                else if (DwStrcasecmp(word, "via") == 0) {
                    state = kInVia;
                }
                else if (DwStrcasecmp(word, "with") == 0) {
                    state = kInWith;
                }
                else if (DwStrcasecmp(word, "id") == 0) {
                    state = kInId;
                }
                else if (DwStrcasecmp(word, "for") == 0) {
                    state = kInFor;
                }
                else {
                    if (fromPos == -1) {
                        fromPos = wordPos;
                    }
                    fromLen = (wordPos + wordLen) - fromPos;
                }
                break;

            case kInBy:

                if (DwStrcasecmp(word, "via") == 0) {
                    state = kInVia;
                }
                else if (DwStrcasecmp(word, "with") == 0) {
                    state = kInWith;
                }
                else if (DwStrcasecmp(word, "id") == 0) {
                    state = kInId;
                }
                else if (DwStrcasecmp(word, "for") == 0) {
                    state = kInFor;
                }
                else {
                    if (byPos == -1) {
                        byPos = wordPos;
                    }
                    byLen = (wordPos + wordLen) - byPos;
                }
                break;

            case kInVia:

                if (DwStrcasecmp(word, "with") == 0) {
                    state = kInWith;
                }
                else if (DwStrcasecmp(word, "id") == 0) {
                    state = kInId;
                }
                else if (DwStrcasecmp(word, "for") == 0) {
                    state = kInFor;
                }
                else {
                    if (viaPos == -1) {
                        viaPos = wordPos;
                    }
                    viaLen = (wordPos + wordLen) - viaPos;
                }
                break;

            case kInWith:

                if (DwStrcasecmp(word, "id") == 0) {
                    state = kInId;
                }
                else if (DwStrcasecmp(word, "for") == 0) {
                    state = kInFor;
                }
                else {
                    if (withPos == -1) {
                        withPos = wordPos;
                    }
                    withLen = (wordPos + wordLen) - withPos;
                }
                break;

            case kInId:

                if (DwStrcasecmp(word, "for") == 0) {
                    state = kInFor;
                }
                else {
                    if (idPos == -1) {
                        idPos = wordPos;
                    }
                    idLen = (wordPos + wordLen) - idPos;
                }
                break;

            case kInFor:
                if (forPos == -1) {
                    forPos = wordPos;
                }
                forLen = (wordPos + wordLen) - forPos;
                break;

            default:
                break;
            }
        }
        strPos = wordPos + wordLen;
    }
    if (fromPos >= 0) {
        mFrom = str.substr(fromPos, fromLen);
    }
    if (byPos >= 0) {
        mBy = str.substr(byPos, byLen);
    }
    if (viaPos >= 0) {
        mVia = str.substr(viaPos, viaLen);
    }
    if (withPos >= 0) {
        mWith = str.substr(withPos, withLen);
    }
    if (idPos >= 0) {
        mId = str.substr(idPos, idLen);
    }
    if (forPos >= 0) {
        mFor = str.substr(forPos, forLen);
    }
    if (datePos >= 0) {
        mDate.FromString(str.substr(datePos, dateLen));
        mDate.Parse();
    }
}


void DwStamp::Assemble()
{
    if (!mIsModified) {
        return;
    }
    mString = "";
    mDate.Assemble();
    if (mFrom != "") {
        mString += "from ";
        mString += mFrom;
    }
    if (mBy != "") {
        if (mString != "") {
            mString += " ";
        }
        mString += "by ";
        mString += mBy;
    }
    if (mVia != "") {
        if (mString != "") {
            mString += " ";
        }
        mString += "via ";
        mString += mVia;
    }
    if (mWith != "") {
        if (mString != "") {
            mString += " ";
        }
        mString += "with ";
        mString += mWith;
    }
    if (mId != "") {
        if (mString != "") {
            mString += " ";
        }
        mString += "id ";
        mString += mId;
    }
    if (mFor != "") {
        if (mString != "") {
            mString += " ";
        }
        mString += "for ";
        mString += mFor;
    }
    mString += "; ";
    mString += mDate.AsString();
    DwFoldLine(mString, 10);
    mIsModified = DwFalse;
}


DwMessageComponent* DwStamp::Clone() const
{
    return new DwStamp(*this);
}


const DwString& DwStamp::From() const
{
    return mFrom;
}


void DwStamp::SetFrom(const DwString& aStr)
{
    mFrom = aStr;
    SetModified();
}


const DwString& DwStamp::By() const
{
    return mBy;
}


void DwStamp::SetBy(const DwString& aStr)
{
    mBy = aStr;
    SetModified();
}


const DwString& DwStamp::Via() const
{
    return mVia;
}


void DwStamp::SetVia(const DwString& aStr)
{
    mVia = aStr;
    SetModified();
}


const DwString& DwStamp::With() const
{
    return mWith;
}


void DwStamp::SetWith(const DwString& aStr)
{
    mWith = aStr;
    SetModified();
}


const DwString& DwStamp::Id() const
{
    return mId;
}


void DwStamp::SetId(const DwString& aStr)
{
    mId = aStr;
    SetModified();
}


const DwString& DwStamp::For() const
{
    return mFor;
}


void DwStamp::SetFor(const DwString& aStr)
{
    mFor = aStr;
    SetModified();
}


const DwDateTime& DwStamp::Date() const
{
    return mDate;
}


void DwStamp::SetDate(const DwDateTime& aDate)
{
    mDate = aDate;
    SetModified();
}


#if defined(DW_DEBUG_VERSION)
void DwStamp::PrintDebugInfo(DW_STD ostream& aStrm, int /*aDepth*/) const
{
    aStrm <<
    "----------------- Debug info for DwStamp class -----------------\n";
    _PrintDebugInfo(aStrm);
}
#else // if !defined(DW_DEBUG_VERSION)
void DwStamp::PrintDebugInfo(DW_STD ostream& /*aStrm*/, int /*aDepth*/) const
{
}
#endif // !defined(DW_DEBUG_VERSION)


#if defined(DW_DEBUG_VERSION)
void DwStamp::CheckInvariants() const
{
    DwFieldBody::CheckInvariants();
    mFrom.CheckInvariants();
    mBy.CheckInvariants();
    mVia.CheckInvariants();
    mWith.CheckInvariants();
    mId.CheckInvariants();
    mFor.CheckInvariants();
    mDate.CheckInvariants();
}
#else // if !defined(DW_DEBUG_VERSION)
void DwStamp::CheckInvariants() const
{
}
#endif // !defined(DW_DEBUG_VERSION)


#if defined(DW_DEBUG_VERSION)
void DwStamp::_PrintDebugInfo(DW_STD ostream& aStrm) const
{
    DwFieldBody::_PrintDebugInfo(aStrm);
    aStrm << "From:             " << mFrom << '\n';
    aStrm << "By:               " << mBy << '\n';
    aStrm << "Via:              " << mVia << '\n';
    aStrm << "With:             " << mWith << '\n';
    aStrm << "Id:               " << mId << '\n';
    aStrm << "For:              " << mFor << '\n';
    aStrm << "Date:             " << mDate.AsString() << '\n';
}
#else // if !defined(DW_DEBUG_VERSION)
void DwStamp::_PrintDebugInfo(DW_STD ostream& /* aStrm */) const
{
}
#endif // !defined(DW_DEBUG_VERSION)
