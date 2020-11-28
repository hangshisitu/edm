//=============================================================================
// File:       token.h
// Contents:   Declarations for DwTokenizer, DwRfc822Tokenizer
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

#ifndef DW_TOKEN_H
#define DW_TOKEN_H

#ifndef DW_CONFIG_H
#include <mimepp/config.h>
#endif

#ifndef DW_STRING_H
#include <mimepp/string.h>
#endif

// RFC 822 and RFC 1521 define slightly different grammars for the field
// bodies they define.  The differences are that RFC 822 defines a basic
// type 'atom' while RFC 1521 defines a basic type 'token', and RFC 822
// defines a character class 'special' while RFC 1521 defines a character
// class 'tspecial'. For this reason, we have two tokenizer classes:
// Rfc822Tokenizer and Rfc1521Tokenizer. Since the basic types
// quoted string, comment, and domain literal are common to both RFC 822
// and RFC 1521, the common code of both tokenizer classes is
// combined into a Tokenizer base class. The Tokenizer class has no public
// constructors, since only objects of class Rfc822Tokenizer or
// Rfc1521Tokenizer will ever be instantiated. 
//
// Note that we do not use polymorphism here: Tokenizer has no virtual
// functions. We do this for efficiency, since there is some overhead
// involved with virtual functions. If the classes were more complicated
// than they currently are, then virtual functions would be justified in
// order to reduce the duplication of code. As it stands, though, the
// classes are fairly simple and efficient.
// In addition, polymorphism is not needed to use the tokenizer classes.

enum {
    kTkError=-1,
    kTkNull=0,
    kTkSpecial,
    kTkAtom,
    kTkComment,
    kTkQuotedString,
    kTkDomainLiteral,
    kTkTspecial,
    kTkToken
};


class DW_CORE_EXPORT DwTokenizer {
    friend class DwTokenString;
public:
    const DwString& Token() const;
    int Type() const;
    void StripDelimiters();
    static DW_STD ostream* mDebugOut;
protected:
    DwTokenizer(const DwString& aStr);
    DwTokenizer(const char* aCStr);
    virtual ~DwTokenizer();
    void PrintToken(DW_STD ostream*);
    // Quoted strings, comments, and domain literals are parsed
    // identically in RFC822 and RFC1521
    void ParseQuotedString();
    void ParseComment();
    void ParseDomainLiteral();
    // Data members
    const DwString mString;
    DwString mToken;
    size_t mTokenStart;
    size_t mTokenLength;
    size_t mNextStart;
    int mTkType;
};


class DW_CORE_EXPORT DwRfc822Tokenizer : public DwTokenizer {
    friend class DwAddressParser;
public:
    DwRfc822Tokenizer(const DwString& aStr);
    DwRfc822Tokenizer(const char* aCStr);
    virtual ~DwRfc822Tokenizer();
    int Restart();
    int operator ++ (); // prefix increment operator
private:
    void ParseToken();
    void ParseAtom();
};


class DW_CORE_EXPORT DwRfc1521Tokenizer : public DwTokenizer {
public:
    DwRfc1521Tokenizer(const DwString& aStr);
    DwRfc1521Tokenizer(const char* aCStr);
    virtual ~DwRfc1521Tokenizer();
    int Restart();
    int operator ++ (); // prefix increment operator
private:
    void ParseToken();
    void ParseAtom();
	//add by rock
	//if mGetValueStatus == true, will skip some tspecial chars
	DwBool mGetValueStatus;
};


// DwTokenString allows us to build a DwString of tokens by concatenating
// them.  This is not the normal string concatenation: the tokens are
// assumed to have the same string rep, and the concatenated string shares
// the rep.

class DW_CORE_EXPORT DwTokenString {
public:
    DwTokenString(const DwString&);
    virtual ~DwTokenString();
    const DwString& Tokens() const;
    void SetFirst(const DwTokenizer&);
    void SetLast(const DwTokenizer&);
    void ExtendTo(const DwTokenizer&);
    // void Concatenate(const DwTokenizer&);
protected:
    const DwString mString;
    DwString mTokens;
    size_t mTokensStart;
    size_t mTokensLength;
};

#endif
