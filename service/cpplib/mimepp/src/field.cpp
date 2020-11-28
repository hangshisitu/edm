//=============================================================================
// File:       field.cpp
// Contents:   Definitions for DwField
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
#include <mimepp/field.h>
#include <mimepp/headers.h>
#include <mimepp/fieldbdy.h>
#include <mimepp/datetime.h>
#include <mimepp/mailbox.h>
#include <mimepp/mboxlist.h>
#include <mimepp/address.h>
#include <mimepp/addrlist.h>
#include <mimepp/mechansm.h>
#include <mimepp/mediatyp.h>
#include <mimepp/msgid.h>
#include <mimepp/stamp.h>
#include <mimepp/text.h>

extern const char* dw_char_map;


inline int IsSpace(int ch)
{
    return dw_char_map[(ch)&0xff] & 0x02;
}


class DwFieldParser {
    friend class DwField;
private:
    DwFieldParser(const DwString&);
    void Parse();
    const DwString mString;
    DwString mName;
    DwString mBody;
};


DwFieldParser::DwFieldParser(const DwString& aStr)
  : mString(aStr)
{
    Parse();
}


void DwFieldParser::Parse()
{
    const char* buf = mString.data();
    size_t bufEnd   = mString.length();
    size_t pos   = 0;
    size_t start = 0;
    size_t len   = 0;
    // Get field name
    while (pos < bufEnd) {
        char ch = buf[pos];
        if (ch == ':') {
            break;
        }
        ++pos;
    }
    len = pos;
    if (pos < bufEnd) {
        ++pos;
    }
    // Remove any white space at end of field name
    while (len > 0) {
        char ch = buf[len-1];
        if (! IsSpace(ch)) {
            break;
        }
        --len;
    }
    mName = mString.substr(start, len);
    // Skip white space
    while (pos < bufEnd) {
        char ch = buf[pos];
        if (ch != ' ' && ch != '\t') {
            break;
        }
        ++pos;
    }
    // Get field body
    start = pos;
    pos = bufEnd - 1;
    while (pos > start) {
        char ch = buf[pos-1];
        if (! IsSpace(ch)) {
            break;
        }
        --pos;
    }
    len = pos - start;
    mBody = mString.substr(start, len);
}


//===========================================================================


const char* const DwField::sClassName = "DwField";


DwField* (*DwField::sNewField)(const DwString&, DwMessageComponent*) = 0;


DwFieldBody* (*DwField::sCreateFieldBody)(const DwString&,
    const DwString&, DwMessageComponent*) = 0;


DwField* DwField::NewField(const DwString& aStr, DwMessageComponent* aParent)
{
    if (sNewField != 0) {
        return sNewField(aStr, aParent);
    }
    else {
        return new DwField(aStr, aParent);
    }
}


DwField::DwField()
{
    mFieldBody = 0;
    mClassId = kCidField;
    mClassName = sClassName;
}


DwField::DwField(const DwField& aField)
  : DwMessageComponent(aField),
    mFieldNameStr(aField.mFieldNameStr),
    mFieldBodyStr(aField.mFieldBodyStr)
{
    mFieldBody = 0;
    mClassId = kCidField;
    mClassName = sClassName;

    if (aField.mFieldBody != 0) {
        mFieldBody = (DwFieldBody*) aField.mFieldBody->Clone();
        if (mFieldBody != 0) {
            mFieldBody->SetParent(this);
        }
    }
}


DwField::DwField(const DwString& aStr, DwMessageComponent* aParent)
  : DwMessageComponent(aStr, aParent)
{
    mFieldBody = 0;
    mClassId = kCidField;
    mClassName = sClassName;
}


DwField::~DwField()
{
    if (mFieldBody != 0) {
        delete mFieldBody;
        mFieldBody = 0;
    }
}


const DwField& DwField::operator = (const DwField& aField)
{
    if (this != &aField) {
        DwMessageComponent::operator = (aField);
        mFieldNameStr = aField.mFieldNameStr;
        mFieldBodyStr = aField.mFieldBodyStr;
        if (aField.mFieldBody != 0) {
            DwFieldBody* newFieldBody = (DwFieldBody*)
                aField.mFieldBody->Clone();
            _SetFieldBody(newFieldBody);
        }
        if (mParent != 0) {
            mParent->SetModified();
        }
    }
    return *this;
}


const DwString& DwField::FieldNameStr() const
{
    return mFieldNameStr;
}


void DwField::SetFieldNameStr(const DwString& aStr)
{
    mFieldNameStr = aStr;
    SetModified();
}


const DwString& DwField::FieldBodyStr() const
{
    return mFieldBodyStr;
}


void DwField::SetFieldBodyStr(const DwString& aStr)
{
    mFieldBodyStr = aStr;
    if (mFieldBody != 0) {
        delete mFieldBody;
        mFieldBody = 0;
    }
    SetModified();
}


DwFieldBody* DwField::FieldBody() const
{
    return mFieldBody;
}


void DwField::SetFieldBody(DwFieldBody* aFieldBody)
{
    if (mFieldBody != aFieldBody) {
        _SetFieldBody(aFieldBody);
        SetModified();
    }
}


void DwField::_SetFieldBody(DwFieldBody* aFieldBody)
{
    if (mFieldBody != aFieldBody) {
        if (mFieldBody != 0) {
            delete mFieldBody;
            mFieldBody = 0;
        }
        mFieldBody = aFieldBody;
        if (mFieldBody != 0) {
            mFieldBody->SetParent(this);
        }
    }
}


void DwField::Parse()
{
    mIsModified = DwFalse;
    DwFieldParser parser(mString);
    mFieldNameStr = parser.mName;
    mFieldBodyStr = parser.mBody;
    mFieldBody = CreateFieldBody(mFieldNameStr, mFieldBodyStr, this);
    DW_ASSERT(mFieldBody != 0);
    mFieldBody->Parse();
}


void DwField::Assemble()
{
    if (!mIsModified) {
        return;
    }
    if (mFieldBody != 0) {
        mFieldBody->Assemble();
        mFieldBodyStr = mFieldBody->AsString();
    }
    mString = "";
    mString += mFieldNameStr;
    mString += ": ";
    mString += mFieldBodyStr;
    mString += DW_EOL;
    mIsModified = DwFalse;
}


DwMessageComponent* DwField::Clone() const
{
    return new DwField(*this);
}


DwFieldBody* DwField::CreateFieldBody(const DwString& aFieldName,
    const DwString& aFieldBody, DwMessageComponent* aParent)
{
    DwFieldBody* fieldBody;
    if (sCreateFieldBody != 0) {
        fieldBody = sCreateFieldBody(aFieldName, aFieldBody, aParent);
    }
    else {
        fieldBody = _CreateFieldBody(aFieldName, aFieldBody, aParent);
    }
    return fieldBody;
}


DwFieldBody* DwField::_CreateFieldBody(const DwString& aFieldName,
    const DwString& aFieldBody, DwMessageComponent* aParent)
{
    enum {
        kAddressList,
        kDispositionType,
        kDateTime,
        kMailbox,
        kMailboxList,
        kMechanism,
        kMediaType,
        kMsgId,
        kStamp,
        kText
    } fieldBodyType;
    // Default field type is 'text'
    fieldBodyType = kText;
    int ch = aFieldName[0];
    ch = tolower(ch);
    switch (ch) {
    case 'b':
        if (DwStrcasecmp(aFieldName, "bcc") == 0) {
            fieldBodyType = kAddressList;
        }
        break;
    case 'c':
        if (DwStrcasecmp(aFieldName, "cc") == 0) {
            fieldBodyType = kAddressList;
        }
        else if (DwStrcasecmp(aFieldName, "content-id") == 0) {
            fieldBodyType = kMsgId;
        }
        else if (DwStrcasecmp(aFieldName, "content-transfer-encoding") == 0) {
            fieldBodyType = kMechanism;
        }
        else if (DwStrcasecmp(aFieldName, "content-type") == 0) {
            fieldBodyType = kMediaType;
        }
        else if (DwStrcasecmp(aFieldName, "content-disposition") == 0) {
            fieldBodyType = kDispositionType;
        }
        break;
    case 'd':
        if (DwStrcasecmp(aFieldName, "date") == 0) {
            fieldBodyType = kDateTime;
        }
        break;
    case 'f':
        if (DwStrcasecmp(aFieldName, "from") == 0) {
            fieldBodyType = kMailboxList;
        }
        break;
    case 'm':
        if (DwStrcasecmp(aFieldName, "message-id") == 0) {
            fieldBodyType = kMsgId;
        }
        break;
    case 'r':
        if (DwStrcasecmp(aFieldName, "received") == 0) {
            fieldBodyType = kStamp;
        }
        else if (DwStrcasecmp(aFieldName, "reply-to") == 0) {
            fieldBodyType = kAddressList;
        }
        else if (DwStrcasecmp(aFieldName, "resent-bcc") == 0) {
            fieldBodyType = kAddressList;
        }
        else if (DwStrcasecmp(aFieldName, "resent-cc") == 0) {
            fieldBodyType = kAddressList;
        }
        else if (DwStrcasecmp(aFieldName, "resent-date") == 0) {
            fieldBodyType = kDateTime;
        }
        else if (DwStrcasecmp(aFieldName, "resent-from") == 0) {
            fieldBodyType = kMailboxList;
        }
        else if (DwStrcasecmp(aFieldName, "resent-message-id") == 0) {
            fieldBodyType = kMsgId;
        }
        else if (DwStrcasecmp(aFieldName, "resent-reply-to") == 0) {
            fieldBodyType = kAddressList;
        }
        else if (DwStrcasecmp(aFieldName, "resent-sender") == 0) {
            fieldBodyType = kMailbox;
        }
        else if (DwStrcasecmp(aFieldName, "return-path") == 0) {
            fieldBodyType = kMailbox;
        }
        break;
    case 's':
        if (DwStrcasecmp(aFieldName, "sender") == 0) {
            fieldBodyType = kMailbox;
        }
        break;
    case 't':
        if (DwStrcasecmp(aFieldName, "to") == 0) {
            fieldBodyType = kAddressList;
        }
        break;
    }
    DwFieldBody* fieldBody;
    switch (fieldBodyType) {
    case kAddressList:
        fieldBody = DwAddressList::NewAddressList(aFieldBody, aParent);
        break;
    case kDispositionType:
        fieldBody = DwDispositionType::NewDispositionType(aFieldBody, aParent);
        break;
    case kMediaType:
        fieldBody = DwMediaType::NewMediaType(aFieldBody, aParent);
        break;
    case kMechanism:
        fieldBody = DwMechanism::NewMechanism(aFieldBody, aParent);
        break;
    case kDateTime:
        fieldBody = DwDateTime::NewDateTime(aFieldBody, aParent);
        break;
    case kMailbox:
        fieldBody = DwMailbox::NewMailbox(aFieldBody, aParent);
        break;
    case kMailboxList:
        fieldBody = DwMailboxList::NewMailboxList(aFieldBody, aParent);
        break;
    case kMsgId:
        fieldBody = DwMsgId::NewMsgId(aFieldBody, aParent);
        break;
    case kStamp:
        fieldBody = DwStamp::NewStamp(aFieldBody, aParent);
        break;
    case kText:
    default:
        fieldBody = DwText::NewText(aFieldBody, aParent);
        break;
    }
    return fieldBody;
}


#if defined (DW_DEBUG_VERSION)
void DwField::PrintDebugInfo(DW_STD ostream& aStrm, int aDepth) const
{
    aStrm <<
    "----------------- Debug info for DwField class -----------------\n";
    _PrintDebugInfo(aStrm);
    int depth = aDepth - 1;
    depth = (depth >= 0) ? depth : 0;
    if (mFieldBody != 0 && (aDepth == 0 || depth > 0)) {
        mFieldBody->PrintDebugInfo(aStrm, depth);
    }
}
#else // if !defined (DW_DEBUG_VERSION)
void DwField::PrintDebugInfo(DW_STD ostream& /*aStrm*/, int /*aDepth*/) const
{
}
#endif // !defined (DW_DEBUG_VERSION)


#if defined (DW_DEBUG_VERSION)
void DwField::_PrintDebugInfo(DW_STD ostream& aStrm) const
{
    DwMessageComponent::_PrintDebugInfo(aStrm);
    aStrm << "Field name:       " << mFieldNameStr << '\n';
    aStrm << "Field body:       " << mFieldBodyStr << '\n';
    aStrm << "Field body object:";
    if (mFieldBody != 0) {
        aStrm << mFieldBody->ObjectId() << '\n';
    }
    else {
        aStrm << "(none)\n";
    }
}
#else // if !defined (DW_DEBUG_VERSION)
void DwField::_PrintDebugInfo(DW_STD ostream& /*aStrm*/) const
{
}
#endif // !defined (DW_DEBUG_VERSION)


void DwField::CheckInvariants() const
{
#if defined (DW_DEBUG_VERSION)
    DwMessageComponent::CheckInvariants();
    mFieldNameStr.CheckInvariants();
    mFieldBodyStr.CheckInvariants();
    if (mFieldBody != 0) {
        mFieldBody->CheckInvariants();
    }
    if (mFieldBody != 0) {
        DW_ASSERT((DwMessageComponent*) this == mFieldBody->Parent());
    }
#endif // defined (DW_DEBUG_VERSION)
}
