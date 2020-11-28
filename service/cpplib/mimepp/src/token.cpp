//=============================================================================
// File:       token.cpp
// Contents:   Definitions for DwTokenizer, DwRfc822Tokenizer
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
#ifdef DW_USE_ANSI_IOSTREAM
#  include <iostream>
#else
#  include <iostream.h>
#endif

#include <mimepp/string.h>
#include <mimepp/token.h>

extern const char* dw_char_map;

const char* dw_char_map =
        /* 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F */
/* 0 */ "\001\001\001\001\001\001\001\001\001\003\003\001\001\003\001\001"
/* 1 */ "\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001"
/* 2 */ "\002\000\014\000\000\000\000\000\014\014\000\000\014\000\004\010"
/* 3 */ "\000\000\000\000\000\000\000\000\000\000\014\014\014\010\014\010"
/* 4 */ "\014\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
/* 5 */ "\000\000\000\000\000\000\000\000\000\000\000\014\014\014\000\000"
/* 6 */ "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
/* 7 */ "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\001"
/* 8 */ "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
/* 9 */ "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
/* A */ "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
/* B */ "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
/* C */ "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
/* D */ "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
/* E */ "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
/* F */ "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000";

#define ISCONTROL(c)  (dw_char_map[(c)&0xff]&0x01)
#define ISSPACE(c)    (dw_char_map[(c)&0xff]&0x02)
#define ISSPECIAL(c)  (dw_char_map[(c)&0xff]&0x04)
#define ISTSPECIAL(c) (dw_char_map[(c)&0xff]&0x08)


DW_STD ostream* DwTokenizer::mDebugOut = 0;


DwTokenizer::DwTokenizer(const DwString& aStr)
  : mString(aStr)
{
    mTokenStart  = 0;
    mTokenLength = 0;
    mNextStart   = 0;
    mTkType      = kTkError;
}


DwTokenizer::DwTokenizer(const char* aCStr)
  : mString(aCStr)
{
    mTokenStart  = 0;
    mTokenLength = 0;
    mNextStart   = 0;
    mTkType      = kTkError;
}


DwTokenizer::~DwTokenizer()
{
}


const DwString& DwTokenizer::Token() const
{
    return mToken;
}


int DwTokenizer::Type() const
{
    return mTkType;
}


void DwTokenizer::StripDelimiters()
{
    if (mTokenLength < 2) {
        return;
    }
    // const ref -- avoids copy on write when using operator[]
    const DwString& token = mToken;
    switch (mTkType) {
    case kTkQuotedString:
        if (token[0] == '"') {
            mToken = mToken.substr(1);
            ++mTokenStart;
            --mTokenLength;
        }
        if (mTokenLength > 0 && token[mTokenLength-1] == '"') {
            mToken = mToken.substr(0, mTokenLength-1);
            --mTokenLength;
        }
        break;
    case kTkDomainLiteral:
        if (token[0] == '[') {
            mToken = mToken.substr(1);
            ++mTokenStart;
            --mTokenLength;
        }
        if (mTokenLength > 0 && token[mTokenLength-1] == ']') {
            mToken = mToken.substr(0, mTokenLength-1);
            --mTokenLength;
        }
        break;
    case kTkComment:
        if (token[0] == '(') {
            mToken = mToken.substr(1);
            ++mTokenStart;
            --mTokenLength;
        }
        if (mTokenLength > 0 && token[mTokenLength-1] == ')') {
            mToken = mToken.substr(0, mTokenLength-1);
            --mTokenLength;
        }
        break;
    }
}


void DwTokenizer::ParseQuotedString()
{
    size_t pos = mTokenStart;
    while (DwTrue) {
        ++pos;
        if (pos >= mString.length()) {
            // Ran out of string
            mTokenLength = 0;
            mToken = "";
            mNextStart = pos;
            mTkType = kTkError;
            break;
        }
        else if (mString[pos] == '\\') {
            // Quoted character
            ++pos;
            if (pos >= mString.length()) {
                // Ran out of string
                mTokenLength = 0;
                mToken = "";
                mNextStart = pos;
                mTkType = kTkError;
                break;
            }
        }
        else if (mString[pos] == '"') {
            // End of quoted string
            ++pos;
            mTokenLength = pos - mTokenStart;
            mToken = mString.substr(mTokenStart, mTokenLength);
            mNextStart = pos;
            break;
        }
    }
}


void DwTokenizer::ParseComment()
{
    size_t pos = mTokenStart;
    int level = 1;
    while (DwTrue) {
        ++pos;
        if (pos >= mString.length()) {
            // Ran out of string
            mTokenLength = 0;
            mToken = "";
            mNextStart = pos;
            mTkType = kTkError;
            break;
        }
        else if (mString[pos] == '\\') {
            // Quoted character
            ++pos;
            if (pos >= mString.length()) {
                // Ran out of string
                mTokenLength = 0;
                mToken = "";
                mNextStart = pos;
                mTkType = kTkError;
                break;
            }
        }
        else if (mString[pos] == ')') {
            --level;
            if (level == 0) {
                // End of comment
                ++pos;
                mTokenLength = pos - mTokenStart;
                mToken = mString.substr(mTokenStart, mTokenLength);
                mNextStart = pos;
                break;
            }
        }
        else if (mString[pos] == '(') {
            ++level;
        }
    }
}


void DwTokenizer::ParseDomainLiteral()
{
    size_t pos = mTokenStart;
    while (DwTrue) {
        ++pos;
        if (pos >= mString.length()) {
            // Ran out of string
            mTokenLength = 0;
            mToken = "";
            mNextStart = pos;
            mTkType = kTkError;
            break;
        }
        else if (mString[pos] == '\\') {
            // Quoted character
            ++pos;
            if (pos >= mString.length()) {
                // Ran out of string
                mTokenLength = 0;
                mToken = "";
                mNextStart = pos;
                mTkType = kTkError;
                break;
            }
        }
        else if (mString[pos] == ']') {
            // End of domain literal
            ++pos;
            mTokenLength = pos - mTokenStart;
            mToken = mString.substr(mTokenStart, mTokenLength);
            mNextStart = pos;
            break;
        }
    }
}


void DwTokenizer::PrintToken(DW_STD ostream* aOut)
{
    if (aOut == 0) {
        return;
    }
    const char* type = 0;
    switch (mTkType) {
    case kTkError:
        type = "error          ";
        break;
    case kTkNull:
        type = "null           ";
        break;
    case kTkSpecial:
        type = "special        ";
        break;
    case kTkAtom:
        type = "atom           ";
        break;
    case kTkComment:
        type = "comment        ";
        break;
    case kTkQuotedString:
        type = "quoted string  ";
        break;
    case kTkDomainLiteral:
        type = "domain literal ";
        break;
    case kTkTspecial:
        type = "tspecial       ";
        break;
    case kTkToken:
        type = "token          ";
        break;
    default:
        type = "unknown        ";
        break;
    }
    *aOut << type << mToken << '\n';
}


DwRfc822Tokenizer::DwRfc822Tokenizer(const DwString& aStr)
  : DwTokenizer(aStr)
{
    ParseToken();
}


DwRfc822Tokenizer::DwRfc822Tokenizer(const char* aCStr)
  : DwTokenizer(aCStr)
{
    ParseToken();
}


DwRfc822Tokenizer::~DwRfc822Tokenizer()
{
}


int DwRfc822Tokenizer::Restart()
{
    mNextStart = 0;
    ParseToken();
    return mTkType;
}


int DwRfc822Tokenizer::operator ++ ()
{
    ParseToken();
    return mTkType;
}


void DwRfc822Tokenizer::ParseToken()
{
    // Assume the field body has already been extracted.  That is, we don't
    // have to watch for the end of the field body or folding.  We just
    // treat any CRs or LFs as white space.
    mTokenStart = mNextStart;
    mTokenLength = 0;
    mTkType = kTkNull;
    if (mTokenStart >= mString.length()) {
        return;
    }
    // Skip leading space.  Also, since control chars are not permitted
    // in atoms, skip these, too.
    while (DwTrue) {
        if (mTokenStart >= mString.length()) {
            return;
        }
        int ch = mString[mTokenStart];
        if (!ISSPACE(ch) && !ISCONTROL(ch)) {
            break;
        }
        ++mTokenStart;
    }
    char ch = mString[mTokenStart];
    // Quoted string
    if (ch == '"') {
        mTkType = kTkQuotedString;
        ParseQuotedString();
    }
    // Comment
    else if (ch == '(') {
        mTkType = kTkComment;
        ParseComment();
    }
    // Domain literal
    else if (ch == '[') {
        mTkType = kTkDomainLiteral;
        ParseDomainLiteral();
    }
    // Special
    else if (ISSPECIAL(ch)) {
        mTkType = kTkSpecial;
        mTokenLength = 1;
        mToken = mString.substr(mTokenStart, 1);
        mNextStart = mTokenStart + 1;
    }
    // Atom
    else {
        mTkType = kTkAtom;
        ParseAtom();
    }
    if (mDebugOut != 0) {
        PrintToken(mDebugOut);
    }
}


void DwRfc822Tokenizer::ParseAtom()
{
    size_t pos = mTokenStart;
    while (DwTrue) {
        ++pos;
        char ch = (pos < mString.length()) ? mString[pos] : (char) 0;
        if (pos >= mString.length()
            || ISSPACE(ch)
            || ISCONTROL(ch)
            || ISSPECIAL(ch)) {

            mTokenLength = pos - mTokenStart;
            mToken = mString.substr(mTokenStart, mTokenLength);
            mNextStart = pos;
            break;
        }
    }
}


DwRfc1521Tokenizer::DwRfc1521Tokenizer(const DwString& aStr)
  : DwTokenizer(aStr)
{
	//////////////////add by rock
	//initial status: false
	mGetValueStatus = DwFalse;
    ParseToken();
}


DwRfc1521Tokenizer::DwRfc1521Tokenizer(const char* aCStr)
  : DwTokenizer(aCStr)
{
	//////////////////add by rock
	mGetValueStatus = DwFalse;
    ParseToken();
}


DwRfc1521Tokenizer::~DwRfc1521Tokenizer()
{
}


int DwRfc1521Tokenizer::Restart()
{
	//////////////////add by rock
	mGetValueStatus = DwFalse;
    mNextStart = 0;
    ParseToken();
    return mTkType;
}


int DwRfc1521Tokenizer::operator ++ ()
{
    ParseToken();
    return mTkType;
}


void DwRfc1521Tokenizer::ParseToken()
{
    // Assume the field body has already been extracted.  That is, we don't
    // have to watch for the end of the field body or folding.  We just
    // treat any CRs or LFs as white space.
    mTokenStart = mNextStart;
    mTokenLength = 0;
    mTkType = kTkNull;
    if (mTokenStart >= mString.length()) {
        return;
    }
    // Skip leading space.  Also, since control chars are not permitted
    // in atoms, skip these, too.
    while (1) {
        if (mTokenStart >= mString.length()) {
            return;
        }
        int ch = mString[mTokenStart];
        if (!ISSPACE(ch) && !ISCONTROL(ch)) {
            break;
        }
        ++mTokenStart;
    }
    char ch = mString[mTokenStart];
    // Quoted string
    if (ch == '"') {
        mTkType = kTkQuotedString;
        ParseQuotedString();
		//////////////////add by rock
		mGetValueStatus = DwFalse;
    }
    // Comment
    else if (ch == '(') {
        mTkType = kTkComment;
        ParseComment();
		//////////////////add by rock
		mGetValueStatus = DwFalse;
    }
    // Domain literal
    else if (ch == '[') {
        mTkType = kTkDomainLiteral;
        ParseDomainLiteral();
		//////////////////add by rock
		mGetValueStatus = DwFalse;
    }
    // Special
    //else if (ISTSPECIAL(ch)) {
	//modify by rock
	else if (ISTSPECIAL(ch) && !mGetValueStatus) {
        mTkType = kTkTspecial;
        mTokenLength = 1;
        mToken = mString.substr(mTokenStart, 1);
        mNextStart = mTokenStart + 1;
		//当遇到第一个"=",表明将要获取Value,设置mGetValueStatus为真
		if (!mGetValueStatus && ch == '=')
		{
			mGetValueStatus = DwTrue;
		}		
    }
    // Atom
    else {
        mTkType = kTkToken;
        ParseAtom();
    }
    if (mDebugOut != 0) {
        PrintToken(mDebugOut);
    }
}


void DwRfc1521Tokenizer::ParseAtom()
{
    size_t pos = mTokenStart-1;
    while (DwTrue) {
        ++pos;
        char ch = (pos < mString.length()) ? mString[pos] : (char) 0;
        if (pos >= mString.length()
            || ISSPACE(ch)
            || ISCONTROL(ch)
            || ISTSPECIAL(ch)) {

			//////////////////add by rock,GT-725
			//由于rfc1521比rfc822新增加三个特殊字符：= , ?, /,这三个特殊字符如果出现在Value中，只能出现在双引号中
			//但是某些信件的Content-Type字段中的Parameter的Value出现这几个字符却没有用双引号引住，按原来的逻辑是不
			//能解析出正确的Value的
			//新逻辑：为了支持更多的特殊字符(=,/,?,@,>等字符),在mGetValueStatus为真的状态下，遇到除";","("外的
			//特殊字符会把它作为Value一部分，遇到";"或"("或空白与控制字符时，结束查找，mGetValueStatus设置为假。
			if (mGetValueStatus && ISTSPECIAL(ch) && ch !=';'&& ch !='(')	
				continue;
			mGetValueStatus = DwFalse;
			///////////////////////////////
            mTokenLength = pos - mTokenStart;
            mToken = mString.substr(mTokenStart, mTokenLength);
            mNextStart = pos;
            break;
        }
    }
}


DwTokenString::DwTokenString(const DwString& aStr)
  : mString(aStr)
{
    mTokensStart  = 0;
    mTokensLength = 0;
}


DwTokenString::~DwTokenString()
{
}


const DwString& DwTokenString::Tokens() const 
{
    return mTokens;
}


void DwTokenString::SetFirst(const DwTokenizer& aTkzr)
{
    switch (aTkzr.Type()) {
    case kTkError:
    case kTkNull:
        mTokensStart  = aTkzr.mTokenStart;
        mTokensLength = 0;
        break;
    case kTkComment:
    case kTkDomainLiteral:
    case kTkQuotedString:
    case kTkSpecial:
    case kTkAtom:
    case kTkTspecial:
    case kTkToken:
        mTokensStart  = aTkzr.mTokenStart;
        mTokensLength = aTkzr.mTokenLength;
        break;
    }
    mTokens = mString.substr(mTokensStart, mTokensLength);
}


void DwTokenString::SetLast(const DwTokenizer& aTkzr)
{
    DW_ASSERT(aTkzr.mTokenStart >= mTokensStart);
    if (aTkzr.mTokenStart < mTokensStart) {
        return;
    }
    mTokensLength = aTkzr.mTokenStart + aTkzr.mTokenLength - mTokensStart;
    mTokens = mString.substr(mTokensStart, mTokensLength);
}


void DwTokenString::ExtendTo(const DwTokenizer& aTkzr)
{
    DW_ASSERT(aTkzr.mTokenStart >= mTokensStart);
    if (aTkzr.mTokenStart < mTokensStart) {
        return;
    }
    mTokensLength = aTkzr.mTokenStart - mTokensStart;
    mTokens = mString.substr(mTokensStart, mTokensLength);
}

