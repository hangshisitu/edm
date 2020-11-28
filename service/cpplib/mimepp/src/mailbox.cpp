//=============================================================================
// File:       mailbox.cpp
// Contents:   Definitions for DwMailbox
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
#include <mimepp/mailbox.h>
#include <mimepp/token.h>
#include <mimepp/text.h>
#include <mimepp/utility.h>

void RemoveCrAndLf(DwString& aStr);

const char* const DwMailbox::sClassName = "DwMailbox";
    

DwMailbox* (*DwMailbox::sNewMailbox)(const DwString&, DwMessageComponent*) = 0;


DwMailbox* DwMailbox::NewMailbox(const DwString& aStr,
    DwMessageComponent* aParent)
{
    if (sNewMailbox != 0) {
        return sNewMailbox(aStr, aParent);
    }
    else {
        return new DwMailbox(aStr, aParent);
    }
}


DwMailbox::DwMailbox()
{
    mClassId = kCidMailbox;
    mClassName = sClassName;
}


DwMailbox::DwMailbox(const DwMailbox& aMailbox)
  : DwAddress(aMailbox),
    mFullName(aMailbox.mFullName),
    mFullNameNonAscii(aMailbox.mFullNameNonAscii),
    mFullNameCharset(aMailbox.mFullNameCharset),
    mRoute(aMailbox.mRoute),
    mLocalPart(aMailbox.mLocalPart),
    mDomain(aMailbox.mDomain)
{
    mClassId = kCidMailbox;
    mClassName = sClassName;
}


DwMailbox::DwMailbox(const DwString& aStr, DwMessageComponent* aParent)
  : DwAddress(aStr, aParent)
{
    mClassId = kCidMailbox;
    mClassName = sClassName;
}


DwMailbox::~DwMailbox()
{
}


const DwMailbox& DwMailbox::operator = (const DwMailbox& aMailbox)
{
    if (this == &aMailbox) {
        return *this;
    }
    DwAddress::operator = (aMailbox);
    mFullName         = aMailbox.mFullName;
    mFullNameNonAscii = aMailbox.mFullNameNonAscii;
    mFullNameCharset  = aMailbox.mFullNameCharset;
    mRoute            = aMailbox.mRoute;
    mLocalPart        = aMailbox.mLocalPart;
    mDomain           = aMailbox.mDomain;
    return *this;
}


const DwString& DwMailbox::FullName() const
{
    return mFullName;
}


void DwMailbox::SetFullName(const DwString& aFullName)
{
    _SetFullName(aFullName);
    SetModified();
}


void DwMailbox::GetFullName(DwString& aFullName, DwString& aCharset) const
{
    aFullName = mFullNameNonAscii;
    aCharset = mFullNameCharset;
}


void DwMailbox::SetFullName(const DwString& aFullName,
     const DwString& aCharset, int aEncoding)
{
    mFullNameNonAscii = aFullName;
    mFullNameCharset = aCharset;
    DwString encoded;
    if (aEncoding == 'Q' || aEncoding == 'q') {
        DwQEncode(aFullName, encoded);
        mFullName = "=?";
        mFullName += aCharset;
        mFullName += "?Q?";
        mFullName += encoded;
        mFullName += "?=";
    }
    else if (aEncoding == 'B' || aEncoding == 'b') {
        DwBEncode(aFullName, encoded);
        mFullName = "=?";
        mFullName += aCharset;
        mFullName += "?B?";
        mFullName += encoded;
        mFullName += "?=";
    }
    else {
        mFullName = aFullName;
    }
    SetModified();
}


void DwMailbox::_SetFullName(const DwString& aFullName)
{
    mFullName = aFullName;
    DwText text(aFullName);
    text.Parse();
    mFullNameNonAscii = "";
    mFullNameCharset = "";
    int lastEncoded = 1;
    int numEncodedWords = text.NumEncodedWords();
    for (int i=0; i < numEncodedWords; ++i) {
        DwEncodedWord& word = text.EncodedWordAt(i);
        int curEncoded = word.EncodingType();
        if (i > 0 && !(lastEncoded && curEncoded)) {
            mFullNameNonAscii += " ";
        }
        mFullNameNonAscii += word.DecodedText();
        const DwString& charset = word.Charset();
        if (DwStrcasecmp(charset,"us-ascii") != 0
            && mFullNameCharset.length() == 0) {
            mFullNameCharset = charset;
        }
        lastEncoded = curEncoded;
    }
}


const DwString& DwMailbox::Route() const
{
    return mRoute;
}


void DwMailbox::SetRoute(const DwString& aRoute)
{
    mRoute = aRoute;
    SetModified();
}


const DwString& DwMailbox::LocalPart() const
{
    return mLocalPart;
}


void DwMailbox::SetLocalPart(const DwString& aLocalPart)
{
    mLocalPart = aLocalPart;
    SetModified();
}


const DwString& DwMailbox::Domain() const
{
    return mDomain;
}


void DwMailbox::SetDomain(const DwString& aDomain)
{
    mDomain = aDomain;
    SetModified();
}


// Some mailboxes to test
//
//  John Doe <john.doe@acme.com>
//  John@acme.com (John Doe)
//  John.Doe@acme.com (John Doe)
//  John.Doe (Jr) @acme.com (John Doe)
//  John <@domain1.com,@domain2.com:jdoe@acme.com>
//  <jdoe@acme>
//  <@node1.[128.129.130.131],@node2.uu.edu:
//      jdoe(John Doe)@node3.[131.130.129.128]> (John Doe)
//
void DwMailbox::Parse()
{
    mIsModified = DwFalse;
    DwString emptyString("");
    DwString space(" ");
    DwBool isFirstPhraseNull = DwTrue;
    DwBool isSimpleAddress = DwTrue;
    DwString firstPhrase(emptyString);
    DwString lastComment(emptyString);
    mRoute     = emptyString;
    mLocalPart = emptyString;
    mDomain    = emptyString;
    mFullName  = emptyString;
    DwRfc822Tokenizer tokenizer(mString);
    int ch;
    
    enum {
        kStart,       // start
        kLtSeen,      // less-than-seen
        kInRoute,     // in-route
        kInAddrSpec,  // in-addr-spec
        kAtSeen,      // at-seen
        kGtSeen       // greater-than-seen
    };

    // Start state -- terminated by '<' or '@'

    int type = tokenizer.Type();
    int state = kStart;
    while (state == kStart && type != kTkNull) {
        switch (type) {
        case kTkSpecial:
            ch = tokenizer.Token()[0];
            switch (ch) {
            case '@':
                state = kAtSeen;
                break;
            case '<':
                isSimpleAddress = DwFalse;
                mLocalPart = emptyString;
                state = kLtSeen;
                break;
            case '.':
                mLocalPart += tokenizer.Token();
                break;
            }
            break;
        case kTkAtom:
        case kTkQuotedString:
            if (isFirstPhraseNull) {
                firstPhrase = tokenizer.Token();
                isFirstPhraseNull = DwFalse;
            }
            else {
                firstPhrase += space;
                firstPhrase += tokenizer.Token();
            }
            mLocalPart += tokenizer.Token();
            break;
        case kTkComment:
            tokenizer.StripDelimiters();
            lastComment = tokenizer.Token();
            break;
        }
        ++tokenizer;
        type = tokenizer.Type();
    }

    // Less-than-seen state -- process only one valid token and transit to
    // in-route state or in-addr-spec state

    while (state == kLtSeen && type != kTkNull) {
        switch (type) {
        case kTkSpecial:
            ch = tokenizer.Token()[0];
            switch (ch) {
            case '@':
                // '@' immediately following '<' indicates a route
                mRoute = tokenizer.Token();
                state = kInRoute;
                break;
            }
            break;
        case kTkAtom:
        case kTkQuotedString:
            mLocalPart = tokenizer.Token();
            state = kInAddrSpec;
            break;
        }
        ++tokenizer;
        type = tokenizer.Type();
    }

    // In-route state -- terminated by ':'

    while (state == kInRoute && type != kTkNull) {
        switch (type) {
        case kTkSpecial:
            ch = tokenizer.Token()[0];
            switch (ch) {
            case ':':
                state = kInAddrSpec;
                break;
            case '@':
            case ',':
            case '.':
                mRoute += tokenizer.Token();
                break;
            }
            break;
        case kTkAtom:
            mRoute += tokenizer.Token();
            break;
        case kTkDomainLiteral:
            mRoute += tokenizer.Token();
            break;
        }
        ++tokenizer;
        type = tokenizer.Type();
    }

    // in-addr-spec state -- terminated by '@'

    while (state == kInAddrSpec && type != kTkNull) {
        switch (type) {
        case kTkSpecial:
            ch = tokenizer.Token()[0];
            switch (ch) {
            case '@':
                state = kAtSeen;
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

    // at-seen state -- terminated by '>' or end of string
    
    while (state == kAtSeen && type != kTkNull) {
        switch (type) {
        case kTkSpecial:
            ch = tokenizer.Token()[0];
            switch (ch) {
            case '>':
                state = kGtSeen;
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
        case kTkComment:
            tokenizer.StripDelimiters();
            lastComment = tokenizer.Token();
            break;
        }
        ++tokenizer;
        type = tokenizer.Type();
    }

    // greater-than-seen state -- terminated by end of string

    while (state == kGtSeen && type != kTkNull) {
        switch (type) {
        case kTkComment:
            tokenizer.StripDelimiters();
            lastComment = tokenizer.Token();
            break;
        }
        ++tokenizer;
        type = tokenizer.Type();
    }

    // Get full name, if possible

    if (isSimpleAddress) {
        _SetFullName(lastComment);
    }
    else if (firstPhrase != emptyString) {
        _SetFullName(firstPhrase);
    }
    else if (lastComment != emptyString) {
        _SetFullName(lastComment);
    }

    // Check validity

    if (mLocalPart.length() > 0 && mDomain.length() > 0) {
        mIsValid = DwTrue;
    }
    else {
        mIsValid = DwFalse;
    }

    // Remove CR or LF from local-part or full name

    RemoveCrAndLf(mFullName);
    RemoveCrAndLf(mLocalPart);
}


void DwMailbox::Assemble()
{
    if (!mIsModified) {
        return;
    }
    mIsValid = DwTrue;
    if (mLocalPart.length() == 0 || mDomain.length() == 0) {
        mIsValid = DwFalse;
        mString = "";
        return;
    }
    mString = "";
    if (mFullName.length() > 0) {
        mString += mFullName;
        mString += " ";
    }
    mString += "<";
    if (mRoute.length() > 0) {
        mString += mRoute;
        mString += ":";
    }
    mString += mLocalPart;
    mString += "@";
    mString += mDomain;
    mString += ">";
    mIsModified = DwFalse;
}

DwMessageComponent* DwMailbox::Clone() const
{
    return new DwMailbox(*this);
}


#if defined(DW_DEBUG_VERSION)
void DwMailbox::PrintDebugInfo(DW_STD ostream& aStrm, int /*aDepth*/) const
{
    aStrm <<
    "---------------- Debug info for DwMailbox class ----------------\n";
    _PrintDebugInfo(aStrm);
}
#else // if !defined(DW_DEBUG_VERSION)
void DwMailbox::PrintDebugInfo(DW_STD ostream& /*aStrm*/, int /*aDepth*/) const
{
}
#endif // !defined(DW_DEBUG_VERSION)


#if defined(DW_DEBUG_VERSION)
void DwMailbox::_PrintDebugInfo(DW_STD ostream& aStrm) const
{
    DwAddress::_PrintDebugInfo(aStrm);
    aStrm << "Full Name:        " << mFullName << '\n';
    aStrm << "Route:            " << mRoute << '\n';
    aStrm << "Local Part:       " << mLocalPart << '\n';
    aStrm << "Domain:           " << mDomain << '\n';
}
#else // if !defined(DW_DEBUG_VERSION)
void DwMailbox::_PrintDebugInfo(DW_STD ostream& /*aStrm*/) const
{
}
#endif // !defined(DW_DEBUG_VERSION)


void DwMailbox::CheckInvariants() const
{
#if defined(DW_DEBUG_VERSION)
    DwAddress::CheckInvariants();
    mFullName.CheckInvariants();
    mRoute.CheckInvariants();
    mLocalPart.CheckInvariants();
    mDomain.CheckInvariants();
#endif // defined(DW_DEBUG_VERSION)
}


void RemoveCrAndLf(DwString& aStr)
{
    // Do a quick check to see if at least one CR or LF is present

    size_t n = aStr.find_first_of("\r\n");
    if (n == DwString::npos) {
        return;
    }

    // At least one CR or LF is present, so copy the string

    const DwString& in = aStr;
    size_t inLen = in.length();
    DwString out;
    out.reserve(inLen);
    int lastChar = 0;
    size_t i = 0;
    while (i < inLen) {
        int ch = in[i];
        if (ch == (int) '\r') {
            out += ' ';
        }
        else if (ch == (int) '\n') {
            if (lastChar != (int) '\r') {
                out += ' ';
            }
        }
        else {
            out += (char) ch;
        }
        lastChar = ch;
        ++i;
    }
    aStr = out;
}
