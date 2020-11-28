//=============================================================================
// File:       msgid.cpp
// Contents:   Definitions for DwMsgId
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
#include <time.h>
#ifdef DW_USE_ANSI_IOSTREAM
#  include <iostream>
#else
#  include <iostream.h>
#endif

#include <mimepp/string.h>
#include <mimepp/msgid.h>
#include <mimepp/token.h>
#include <mimepp/system.h>

const char* const DwMsgId::sClassName = "DwMsgId";
const char* DwMsgId::sHostName = 0;


DwMsgId* (*DwMsgId::sNewMsgId)(const DwString&, DwMessageComponent*) = 0;


DwMsgId* DwMsgId::NewMsgId(const DwString& aStr, DwMessageComponent* aParent)
{
    if (sNewMsgId) {
        return sNewMsgId(aStr, aParent);
    }
    else {
        return new DwMsgId(aStr, aParent);
    }
}


DwMsgId::DwMsgId()
{
    mClassId = kCidMsgId;
    mClassName = sClassName;
}


DwMsgId::DwMsgId(const DwMsgId& aMsgId)
  : DwFieldBody(aMsgId),
    mLocalPart(aMsgId.mLocalPart),
    mDomain(aMsgId.mDomain)
{
    mClassId = kCidMsgId;
    mClassName = sClassName;
}


DwMsgId::DwMsgId(const DwString& aStr, DwMessageComponent* aParent)
  : DwFieldBody(aStr, aParent)
{
    mClassId = kCidMsgId;
    mClassName = sClassName;
}


DwMsgId::~DwMsgId()
{
}


const DwMsgId& DwMsgId::operator = (const DwMsgId& aMsgId)
{
    if (this == &aMsgId) {
        return *this;
    }
    DwFieldBody::operator = (aMsgId);
    mLocalPart = aMsgId.mLocalPart;
    mDomain = aMsgId.mDomain;
    return *this;
}


const DwString& DwMsgId::LocalPart() const
{
    return mLocalPart;
}


void DwMsgId::SetLocalPart(const DwString& aLocalPart)
{
    mLocalPart = aLocalPart;
    SetModified();
}


const DwString& DwMsgId::Domain() const
{
    return mDomain;
}


void DwMsgId::SetDomain(const DwString& aDomain)
{
    mDomain = aDomain;
    SetModified();
}


void DwMsgId::Parse()
{
    mIsModified = DwFalse;

    mLocalPart = "";
    mDomain = "";

    int ch;
    DwRfc822Tokenizer tokenizer(mString);

    // Advance to '<'
    int type = tokenizer.Type();
    DwBool found = DwFalse;
    while (!found && type != kTkNull) {
        if (type == kTkSpecial && tokenizer.Token()[0] == '<') {
            found = DwTrue;
        }
        ++tokenizer;
        type = tokenizer.Type();
    }
    // Get the local part
    found = DwFalse;
    while (!found && type != kTkNull) {
        switch (type) {
        case kTkSpecial:
            ch = tokenizer.Token()[0];
            switch (ch) {
            case '@':
                found = DwTrue;
                break;
            case '.':
                mLocalPart += tokenizer.Token();
                break;
            }
            break;
        case kTkAtom:
        case kTkQuotedString:
            mLocalPart += tokenizer.Token();
            break;
        }
        ++tokenizer;
        type = tokenizer.Type();
    }
    // Get the domain
    found = DwFalse;
    while (!found && type != kTkNull) {
        switch (type) {
        case kTkSpecial:
            ch = tokenizer.Token()[0];
            switch (ch) {
            case '>':
                found = DwTrue;
                break;
            case '.':
                mDomain += tokenizer.Token();
                break;
            }
            break;
        case kTkAtom:
            mDomain += tokenizer.Token();
            break;
        case kTkDomainLiteral:
            mDomain += tokenizer.Token();
            break;
        }
        ++tokenizer;
        type = tokenizer.Type();
    }
}


void DwMsgId::Assemble()
{
    if (!mIsModified) {
        return;
    }
    mString = "<";
    mString += mLocalPart;
    mString += "@";
    mString += mDomain;
    mString += ">";
    mIsModified = DwFalse;
}


DwMessageComponent* DwMsgId::Clone() const
{
    return new DwMsgId(*this);
}



void DwMsgId::CreateDefault()
{
    static const char* const hexChars = "0123456789ABCDEF";
    char buffer[80];
    int pos = 0;
    buffer[pos++] = '<';

    // current time

    time_t calendarTime = time(NULL);
    DwUint32 scalarTime = DwCalendarTimeToUnixTime(calendarTime);
    buffer[pos++] = hexChars[(scalarTime >> 28) & 0x0f];
    buffer[pos++] = hexChars[(scalarTime >> 24) & 0x0f];
    buffer[pos++] = hexChars[(scalarTime >> 20) & 0x0f];
    buffer[pos++] = hexChars[(scalarTime >> 16) & 0x0f];
    buffer[pos++] = hexChars[(scalarTime >> 12) & 0x0f];
    buffer[pos++] = hexChars[(scalarTime >>  8) & 0x0f];
    buffer[pos++] = hexChars[(scalarTime >>  4) & 0x0f];
    buffer[pos++] = hexChars[(scalarTime      ) & 0x0f];

    // sequence number

    buffer[pos++] = '.';
    DwUint32 seq = DwNextInteger();
    buffer[pos++] = hexChars[(seq >> 20) & 0x0f];
    buffer[pos++] = hexChars[(seq >> 16) & 0x0f];
    buffer[pos++] = hexChars[(seq >> 12) & 0x0f];
    buffer[pos++] = hexChars[(seq >>  8) & 0x0f];
    buffer[pos++] = hexChars[(seq >>  4) & 0x0f];
    buffer[pos++] = hexChars[(seq      ) & 0x0f];

    // thread or process id

    buffer[pos++] = '.';
    DwUint32 pid = DwGetThreadId();
    buffer[pos++] = char(pid / 10000 % 10 + '0');
    buffer[pos++] = char(pid / 1000  % 10 + '0');
    buffer[pos++] = char(pid / 100   % 10 + '0');
    buffer[pos++] = char(pid / 10    % 10 + '0');
    buffer[pos++] = char(pid         % 10 + '0');

    buffer[pos++] = '@';

    // fully qualified domain name

    char hostname[80];
    hostname[0] = 0;
    DwGetFullyQualifiedDomainName(hostname, 80);
    hostname[79] = 0;
    char* cp = hostname;
    while (*cp && pos < 78) {
        buffer[pos++] = *cp++;
    }
    buffer[pos++] = '>';
    buffer[pos] = 0;
    mString = buffer;
    mIsModified = DwFalse;
    Parse();
}


#if defined (DW_DEBUG_VERSION)
void DwMsgId::PrintDebugInfo(DW_STD ostream& aStrm, int /*aDepth*/) const
{
    aStrm <<
    "----------------- Debug info for DwMsgId class -----------------\n";
    _PrintDebugInfo(aStrm);
}
#else // if !defined (DW_DEBUG_VERSION)
void DwMsgId::PrintDebugInfo(DW_STD ostream& /*aStrm*/, int /*aDepth*/) const
{
}
#endif // !defined (DW_DEBUG_VERSION)


#if defined (DW_DEBUG_VERSION)
void DwMsgId::_PrintDebugInfo(DW_STD ostream& aStrm) const
{
    DwFieldBody::_PrintDebugInfo(aStrm);
    aStrm << "Local part:       " << mLocalPart << '\n';
    aStrm << "Domain:           " << mDomain    << '\n';
}
#else // if !defined (DW_DEBUG_VERSION)
void DwMsgId::_PrintDebugInfo(DW_STD ostream& /*aStrm*/) const
{
}
#endif // !defined (DW_DEBUG_VERSION)


void DwMsgId::CheckInvariants() const
{
#if defined (DW_DEBUG_VERSION)
    DwFieldBody::CheckInvariants();
    mLocalPart.CheckInvariants();
    mDomain.CheckInvariants();
#endif // defined (DW_DEBUG_VERSION)
}
