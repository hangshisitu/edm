//=============================================================================
// File:       headers.cpp
// Contents:   Definitions for DwHeaders
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
#include <mimepp/headers.h>
#include <mimepp/field.h>
#include <mimepp/body.h>
#include <mimepp/datetime.h>
#include <mimepp/mailbox.h>
#include <mimepp/address.h>
#include <mimepp/mechansm.h>
#include <mimepp/mediatyp.h>
#include <mimepp/msgid.h>
#include <mimepp/text.h>
#include <mimepp/addrlist.h>
#include <mimepp/mboxlist.h>


class DwHeadersParser {
    friend class DwHeaders;
private:
    DwHeadersParser(const DwString&);
    void NextField(DwString&);
    const DwString mString;
    size_t mPos;
};


DwHeadersParser::DwHeadersParser(const DwString& aStr)
  : mString(aStr)
{
    mPos = 0;
}


void DwHeadersParser::NextField(DwString& aStr)
{
    const char* buf = mString.data();
    size_t bufEnd = mString.length();
    size_t pos = mPos;
    size_t start = pos;
    while (pos < bufEnd) {
        char ch = buf[pos++];
        if (ch == '\n'
            && pos < bufEnd
            && buf[pos] != ' '
            && buf[pos] != '\t') {

            break;
        }
    }
    size_t len = pos - start;
    aStr = mString.substr(start, len);
    mPos = pos;
}


//============================================================================


const char* const DwHeaders::sClassName = "DwHeaders";
DwHeaders* (*DwHeaders::sNewHeaders)(const DwString&, DwMessageComponent*) = 0;


DwHeaders* DwHeaders::NewHeaders(const DwString& aStr,
    DwMessageComponent* aParent)
{
    if (sNewHeaders != 0) {
        return sNewHeaders(aStr, aParent);
    }
    else {
        return new DwHeaders(aStr, aParent);
    }
}


DwHeaders::DwHeaders()
{
    DwHeaders_Init();
}


DwHeaders::DwHeaders(const DwHeaders& aHeaders)
  : DwMessageComponent(aHeaders)
{
    DwHeaders_Init();
    _CopyFields(aHeaders);
}


DwHeaders::DwHeaders(const DwString& aStr, DwMessageComponent* aParent)
  : DwMessageComponent(aStr, aParent)
{
    DwHeaders_Init();
}


DwHeaders::~DwHeaders()
{
    _DeleteAllFields();
    if (mFields != 0) {
        delete [] mFields;
        mFields = 0;
    }
}


void DwHeaders::DwHeaders_Init()
{
    mClassId = kCidHeaders;
    mClassName = sClassName;
    mNumFields = 0;
    mFields = 0;
    mFieldsSize = 0;

    const int fieldsSize = 30;
    mFields = new DwField* [fieldsSize];
    if (mFields != 0) {
        mFieldsSize = fieldsSize;
        for (int i=0; i < mFieldsSize; ++i) {
            mFields[i] = 0;
        }
    }
}


const DwHeaders& DwHeaders::operator = (const DwHeaders& aHeaders)
{
    if (this != &aHeaders) {
        _DeleteAllFields();
        DwMessageComponent::operator = (aHeaders);
        _CopyFields(aHeaders);
        if (mParent != 0) {
            mParent->SetModified();
        }
    }
    return *this;
}


void DwHeaders::Parse()
{
    mIsModified = DwFalse;
    _DeleteAllFields();
    DwHeadersParser parser(mString);
    DwString str;
    parser.NextField(str);
    while (str.length() > 0) {
        DwField* field = DwField::NewField(str, this);
        if (field == 0) {
            break;
        }
        field->Parse();
        _AddField(field);
        parser.NextField(str);
    }
}


void DwHeaders::Assemble()
{
    if (mIsModified) {
        mString = "";
        for (int i=0; i < mNumFields; ++i) {
            mFields[i]->Assemble();
            mString += mFields[i]->AsString();
        }
        mIsModified = DwFalse;
    }
}


DwMessageComponent* DwHeaders::Clone() const
{
    return new DwHeaders(*this);
}


int DwHeaders::NumFields() const
{
    return mNumFields;
}


void DwHeaders::AddField(DwField* aField)
{
    DW_ASSERT(aField != 0);
    if (aField != 0) {
        _AddField(aField);
        SetModified();
    }
}


void DwHeaders::DeleteAllFields()
{
    _DeleteAllFields();
    SetModified();
}


DwField& DwHeaders::FieldAt(int aIndex) const
{
    // Check the index for a valid value. If it's invalid, throw an exception
    // or abort.
#if defined(DW_USE_EXCEPTIONS)
    if (! (0 <= aIndex && aIndex < mNumFields)) {
        throw DwBoundsException();
    }
#else
    DW_ASSERT(0 <= aIndex && aIndex < mNumFields);
    if (! (0 <= aIndex && aIndex < mNumFields)) {
        abort();
    }
#endif
    return *mFields[aIndex];
}


void DwHeaders::InsertFieldAt(int aIndex, DwField* aField)
{
    // Check the index for a valid value. If it's invalid, throw an exception
    // or recover.

#if defined(DW_USE_EXCEPTIONS)
    if (! (0 <= aIndex && aIndex <= mNumFields)) {
        throw DwBoundsException();
    }
#else
    DW_ASSERT(0 <= aIndex && aIndex <= mNumFields);
    // If there is a bounds error, recover by reassigning the index to
    // a valid value.
    if (aIndex < 0) {
        aIndex = 0;
    }
    else if (mNumFields < aIndex) {
        aIndex = mNumFields;
    }
#endif

    // Make the insertion and set the is-modified flag

    DW_ASSERT(aField != 0);
    if (aField != 0) {
        _InsertFieldAt(aIndex, aField);
        SetModified();
    }
}


DwField* DwHeaders::RemoveFieldAt(int aIndex)
{
    // Check the index for a valid value. If it's invalid, throw an exception
    // or recover.

#if defined(DW_USE_EXCEPTIONS)
    if (! (0 <= aIndex && aIndex < mNumFields)) {
        throw DwBoundsException();
    }
#else
    DW_ASSERT(0 <= aIndex && aIndex < mNumFields);
    if (! (0 <= aIndex && aIndex < mNumFields)) {
        return 0;
    }
#endif

    // Save the requested field as return value

    DwField* ret = mFields[aIndex];
    ret->SetParent(0);

    // Shift other fields in the array

    for (int i=aIndex; i < mNumFields-1; ++i) {
        mFields[i] = mFields[i+1];
    }
    --mNumFields;
    mFields[mNumFields] = 0;
    SetModified();

    return ret;
}


void DwHeaders::_CopyFields(const DwHeaders& aHeaders)
{
    DW_ASSERT(0 == mNumFields);

    // Copy the fields

    int numFields = aHeaders.mNumFields;
    for (int i=0; i < numFields; ++i) {
        DwField* field = (DwField*) aHeaders.mFields[i]->Clone();
        // If we're out of memory, and no exception is thrown, just
        // recover by returning.
        if (field == 0) {
            break;
        }
        _AddField(field);
    }
}


void DwHeaders::_AddField(DwField* aField)
{
    int index = mNumFields;
    _InsertFieldAt(index, aField);
}


void DwHeaders::_InsertFieldAt(int aIndex, DwField* aField)
{
    // Reallocate a larger array, if necessary

    if (mNumFields == mFieldsSize) {
        int newFieldsSize = 2*mFieldsSize;
        if (newFieldsSize == 0) {
            newFieldsSize = 30;
        }
        DwField** newFields = new DwField* [newFieldsSize];
        if (newFields == 0) {
            return;
        }
        int i;
        for (i=0; i < mNumFields; ++i) {
            newFields[i] = mFields[i];
            mFields[i] = 0;
        }
        for ( ; i < newFieldsSize; ++i) {
            newFields[i] = 0;
        }
        delete [] mFields;
        mFields = newFields;
        mFieldsSize = newFieldsSize;
    }

    // Make room in the array, then assign the field

    for (int i=mNumFields; i > aIndex; --i) {
        mFields[i] = mFields[i-1];
    }

    mFields[aIndex] = aField;
    ++mNumFields;
    aField->SetParent(this);
}


void DwHeaders::_DeleteAllFields()
{
    while (mNumFields > 0) {
        int i = mNumFields - 1;
        if (mFields[i] != 0) {
            delete mFields[i];
            mFields[i] = 0;
        }
        --mNumFields;
    }
}


DwFieldBody& DwHeaders::FieldBody(const char* aFieldName)
{
    DW_ASSERT(aFieldName != 0);

    // Search for field

    DwField* field = FindField(aFieldName);

    // If the field is not found, create the field and its field body

    if (field == 0) {
        field = DwField::NewField("", this);
        field->SetFieldNameStr(aFieldName);
        DwFieldBody* fieldBody = DwField::CreateFieldBody(aFieldName,
            "", field);
        field->SetFieldBody(fieldBody);
        AddField(field);
    }

    // Get the field body

    DwFieldBody* fieldBody = field->FieldBody();

    // If it does not exist, create it

    if (fieldBody == 0) {
        fieldBody = DwField::CreateFieldBody(aFieldName, "", field);
        field->SetFieldBody(fieldBody);
        SetModified();
    }

    return *fieldBody;
}


DwField* DwHeaders::FindField(const char* aFieldName) const
{
    DW_ASSERT(aFieldName != 0);
    if (aFieldName == 0) {
        return 0;
    }
    DwField* ret = 0;
    for (int i=0; i < mNumFields; ++i) {
        DwField* field = mFields[i];
        if (DwStrcasecmp(field->FieldNameStr(), aFieldName) == 0) {
            ret = field;
            break;
        }
    }
    return ret;
}


DwField* DwHeaders::FindField(const DwString& aFieldName) const
{
    DwField* ret = 0;
    for (int i=0; i < mNumFields; ++i) {
        DwField* field = mFields[i];
        if (DwStrcasecmp(field->FieldNameStr(), aFieldName) == 0) {
            ret = field;
            break;
        }
    }
    return ret;
}


DwBool DwHeaders::HasBcc() const
{
    return FindField("bcc") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasCc() const
{
    return FindField("cc") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasComments() const
{
    return FindField("comments") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasDate() const
{
    return FindField("date") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasEncrypted() const
{
    return FindField("encrypted") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasFrom() const
{
    return FindField("from") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasInReplyTo() const
{
    return FindField("in-reply-to") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasKeywords() const
{
    return FindField("keywords") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasMessageId() const
{
    return FindField("message-id") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasReceived() const
{
    return FindField("received") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasReferences() const
{
    return FindField("references") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasReplyTo() const
{
    return FindField("reply-to") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasResentBcc() const
{
    return FindField("resent-bcc") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasResentCc() const
{
    return FindField("resent-cc") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasResentDate() const
{
    return FindField("resent-date") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasResentFrom() const
{
    return FindField("resent-from") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasResentMessageId() const
{
    return FindField("resent-message-id") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasResentReplyTo() const
{
    return FindField("resent-reply-to") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasResentSender() const
{
    return FindField("resent-sender") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasResentTo() const
{
    return FindField("resent-to") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasReturnPath() const
{
    return FindField("return-path") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasSender() const
{
    return FindField("sender") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasSubject() const
{
    return FindField("subject") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasTo() const
{
    return FindField("to") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasApproved() const
{
    return FindField("approved") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasControl() const
{
    return FindField("control") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasDistribution() const
{
    return FindField("distribution") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasExpires() const
{
    return FindField("expires") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasFollowupTo() const
{
    return FindField("followup-to") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasLines() const
{
    return FindField("lines") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasNewsgroups() const
{
    return FindField("newsgroups") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasOrganization() const
{
    return FindField("organization") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasPath() const
{
    return FindField("path") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasSummary() const
{
    return FindField("summary") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasXref() const
{
    return FindField("xref") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasContentDescription() const
{
    return FindField("content-description") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasContentId() const
{
    return FindField("content-id") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasContentTransferEncoding() const
{
    return FindField("content-transfer-encoding") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasCte() const
{
    return FindField("content-transfer-encoding") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasContentType() const
{
    return FindField("content-type") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasMimeVersion() const
{
    return FindField("mime-version") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasContentDisposition() const
{
    return FindField("content-disposition") ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasXMailer() const
{
	return FindField("X-Mailer") ? DwTrue : DwFalse;
}

DwBool DwHeaders::HasField(const char* aFieldName) const
{
    return FindField(aFieldName) ? DwTrue : DwFalse;
}


DwBool DwHeaders::HasField(const DwString& aFieldName) const
{
    return FindField(aFieldName) ? DwTrue : DwFalse;
}


DwAddressList& DwHeaders::Bcc()
{
    return (DwAddressList&) FieldBody("Bcc");
}


DwAddressList& DwHeaders::Cc()
{
    return (DwAddressList&) FieldBody("Cc");
}


DwText& DwHeaders::Comments()
{
    return (DwText&) FieldBody("Comments");
}


DwDateTime& DwHeaders::Date()
{
    return (DwDateTime&) FieldBody("Date");
}


DwText& DwHeaders::Encrypted()
{
    return (DwText&) FieldBody("Encrypted");
}


DwMailboxList& DwHeaders::From()
{
    return (DwMailboxList&) FieldBody("From");
}


DwText& DwHeaders::InReplyTo()
{
    return (DwText&) FieldBody("In-Reply-To");
}


DwText& DwHeaders::Keywords()
{
    return (DwText&) FieldBody("Keywords");
}


DwMsgId& DwHeaders::MessageId()
{
    return (DwMsgId&) FieldBody("Message-Id");
}


DwStamp& DwHeaders::Received()
{
    return (DwStamp&) FieldBody("Received");
}


DwText& DwHeaders::References()
{
    return (DwText&) FieldBody("References");
}


DwAddressList& DwHeaders::ReplyTo()
{
    return (DwAddressList&) FieldBody("Reply-To");
}


DwAddressList& DwHeaders::ResentBcc()
{
    return (DwAddressList&) FieldBody("Resent-Bcc");
}


DwAddressList& DwHeaders::ResentCc()
{
    return (DwAddressList&) FieldBody("Resent-Cc");
}


DwDateTime& DwHeaders::ResentDate()
{
    return (DwDateTime&) FieldBody("Resent-Date");
}


DwMailboxList& DwHeaders::ResentFrom()
{
    return (DwMailboxList&) FieldBody("Resent-From");
}


DwMsgId& DwHeaders::ResentMessageId()
{
    return (DwMsgId&) FieldBody("Resent-Message-Id");
}


DwAddressList& DwHeaders::ResentReplyTo()
{
    return (DwAddressList&) FieldBody("Resent-Reply-To");
}


DwMailbox& DwHeaders::ResentSender()
{
    return (DwMailbox&) FieldBody("Resent-Sender");
}


DwAddressList& DwHeaders::ResentTo()
{
    return (DwAddressList&) FieldBody("Resent-To");
}


DwAddress& DwHeaders::ReturnPath()
{
    return (DwAddress&) FieldBody("Return-Path");
}


DwMailbox& DwHeaders::Sender()
{
    return (DwMailbox&) FieldBody("Sender");
}


DwText& DwHeaders::Subject()
{
    return (DwText&) FieldBody("Subject");
}


DwAddressList& DwHeaders::To()
{
    return (DwAddressList&) FieldBody("To");
}


DwText& DwHeaders::Approved()
{
    return (DwText&) FieldBody("Approved");
}


DwText& DwHeaders::Control()
{
    return (DwText&) FieldBody("Control");
}


DwText& DwHeaders::Distribution()
{
    return (DwText&) FieldBody("Distribution");
}


DwText& DwHeaders::Expires()
{
    return (DwText&) FieldBody("Expires");
}


DwText& DwHeaders::FollowupTo()
{
    return (DwText&) FieldBody("FollowupTo");
}


DwText& DwHeaders::Lines()
{
    return (DwText&) FieldBody("Lines");
}


DwText& DwHeaders::Newsgroups()
{
    return (DwText&) FieldBody("Newsgroups");
}


DwText& DwHeaders::Organization()
{
    return (DwText&) FieldBody("Organization");
}


DwText& DwHeaders::Path()
{
    return (DwText&) FieldBody("Path");
}


DwText& DwHeaders::Summary()
{
    return (DwText&) FieldBody("Summary");
}


DwText& DwHeaders::Xref()
{
    return (DwText&) FieldBody("Xref");
}



DwText& DwHeaders::ContentDescription()
{
    return (DwText&) FieldBody("Content-Description");
}


DwMsgId& DwHeaders::ContentId()
{
    return (DwMsgId&) FieldBody("Content-Id");
}


DwMechanism& DwHeaders::ContentTransferEncoding()
{
    return (DwMechanism&)
        FieldBody("Content-Transfer-Encoding");
}


DwMechanism& DwHeaders::Cte()
{
    return (DwMechanism&)
        FieldBody("Content-Transfer-Encoding");
}


DwMediaType& DwHeaders::ContentType()
{
    return (DwMediaType&) FieldBody("Content-Type");
}


DwText& DwHeaders::MimeVersion()
{
    return (DwText&) FieldBody("MIME-Version");
}

DwText& DwHeaders::ContentMd5()
{
	return (DwText&) FieldBody("Content-MD5");
}

DwDispositionType& DwHeaders::ContentDisposition()
{
    return (DwDispositionType&) FieldBody("Content-Disposition");
}

				  
DwText& DwHeaders::XMailer()
{
	return (DwText&) FieldBody("X-Mailer");
}


#if defined (DW_DEBUG_VERSION)
void DwHeaders::PrintDebugInfo(DW_STD ostream& aStrm, int aDepth) const
{
    aStrm <<
    "---------------- Debug info for DwHeaders class ----------------\n";
    _PrintDebugInfo(aStrm);
    int depth = aDepth - 1;
    depth = (depth >= 0) ? depth : 0;
    if (aDepth == 0 || depth > 0) {
        for (int i=0; i < mNumFields; ++i) {
            mFields[i]->PrintDebugInfo(aStrm, depth);
        }
    }
}
#else // if !defined (DW_DEBUG_VERSION)
void DwHeaders::PrintDebugInfo(DW_STD ostream& /*aStrm*/, int /*aDepth*/) const
{
}
#endif // !defined (DW_DEBUG_VERSION)


#if defined (DW_DEBUG_VERSION)
void DwHeaders::_PrintDebugInfo(DW_STD ostream& aStrm) const
{
    DwMessageComponent::_PrintDebugInfo(aStrm);
    aStrm << "Fields:           ";
    if (mNumFields > 0) {
        for (int i=0; i < mNumFields; ++i) {
            if (i > 0) {
                aStrm << ' ';
            }
            aStrm << mFields[i]->ObjectId();
        }
        aStrm << '\n';
    }
    else {
        aStrm << "(none)\n";
    }
}
#else // if !defined (DW_DEBUG_VERSION)
void DwHeaders::_PrintDebugInfo(DW_STD ostream& /*aStrm*/) const
{
}
#endif // !defined (DW_DEBUG_VERSION)


void DwHeaders::CheckInvariants() const
{
#if defined (DW_DEBUG_VERSION)
    DwMessageComponent::CheckInvariants();
    for (int i=0; i < mNumFields; ++i) {
        mFields[i]->CheckInvariants();
        DW_ASSERT((DwMessageComponent*) this == mFields[i]->Parent());
    }
#endif // defined (DW_DEBUG_VERSION)
}
