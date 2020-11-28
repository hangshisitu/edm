//=============================================================================
// File:       encoded.h
// Contents:   Declarations for DwEncodedWord
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

#ifndef DW_ENCODED_H
#define DW_ENCODED_H

#ifndef DW_CONFIG_H
#include <mimepp/config.h>
#endif

#ifndef DW_STRING_H
#include <mimepp/string.h>
#endif

#ifndef DW_MSGCMP_H
#include <mimepp/msgcmp.h>
#endif

//=============================================================================
//+ Name DwEncodedWord -- Class representing an RFC 2047 encoded-word
//+ Description

//. <var>DwEncoded</var> represents an encoded word in a non-structured
//. header field.  In Internet email, and in MIME, only 7-bit US-ASCII
//. characters are allowed in the header fields.  RFC 2047 describes the
//. method by which non-ASCII text can be encoded and inserted into certain
//. header fields.  In MIME++, these <i>encoded words</i> are used to
//. perform the encoding/decoding.
//.
//. Encoded words have a character set attribute, such as ISO-8859-1, and an
//. encoding type, which can be quoted-printable or
//. base64. <var>DwEncodedWord</var> provides member functions to set or get
//. the character set and encoding type.  The character set must be a
//. character set registered with the Internet Assigned Numbers Authority
//. (IANA).  The encoding type must be either quoted-printable (the 'Q'
//. encoding) or base64 (the 'B' encoding).
//.
//. <var>DwEncodedWord</var> also contains strings for the encoded text, the
//. decoded text, and the entire encoded word (use the inherited functions
//. <var>DwMessageComponent::FromString()</var> and
//. <var>DwMessageComponent::AsString()</var>).
//.
//. To use <var>DwEncodedWord</var> to decode an encoded word, set the text
//. of the encoded word using the inherited function
//. <var>DwMessageComponent::FromString()</var>, or set the encoded word
//. text using the constructor.  Then call <var>Parse()</var>.  Now you can
//. get the character set and the decoded text.  To use
//. <var>DwEncodedWord</var> to encode non-ASCII text, set the non-ASCII
//. text using <var>SetDecodedText()</var>.  Also set the character set
//. using <var>SetCharset()</var> and the encoding type using
//. <var>SetEncodingType()</var>.  Then call <var>Assemble()</var>.  Now you
//. can get the encoded word using the inherited function
//. <var>DwMessageComponent::AsString()</var>.  To set an encoded word into
//. a header field (which must be a non-structured header field), use the
//. function <var>DwText::AddEncodedWord()</var>.
//.
//. Because encoded words can be mixed with unencoded US-ASCII text, MIME++
//. provides pseudo-encoded words, which are treated like encoded words but
//. are not really encoded.  If the encoding type of a
//. <var>DwEncodedWord</var> is the NUL character, then no encoding or
//. decoding is done.  Other than the fact that no encoding is done, such
//. <i>pseudo</i>-encoded words can be treated exactly like regular encoded
//. words.
//.
//. If there is an error decoding an encoded word, <var>DwEncodedWord</var>
//. will present the entire encoded word as the decoded text.
//=============================================================================
// Last modified 1999-09-20
//+ Noentry ~DwEncodedWord _PrintDebugInfo mCharset mEncodingType
//+ Noentry mEncodedText mDecodedText


class DW_CORE_EXPORT DwEncodedWord : public DwMessageComponent {

public:
    
    DwEncodedWord();
    DwEncodedWord(const DwEncodedWord& aWord);
    DwEncodedWord(const DwString& aStr, DwMessageComponent* aParent=0);
    //. The first constructor is the default constructor, which assigns
    //. empty string to all the string attributes and sets the encoding
    //. type to NUL.
    //.
    //. The second constructor is the copy constructor.  The parent of
    //. the new <var>DwEncodedWord</var> object is set to <var>NULL</var>.
    //
    //. The third constructor sets <var>aStr</var> as the
    //. <var>DwEncodedWord</var> object's string representation and sets
    //. <var>aParent</var> as its parent. The virtual member function
    //. <var>Parse()</var> should be called after this constructor to
    //. perform the decoding operation.  Unless it is <var>NULL</var>,
    //. <var>aParent</var> should point to an object of a class derived from
    //. <var>DwText</var>.

    virtual ~DwEncodedWord();

    const DwEncodedWord& operator = (const DwEncodedWord& aWord);
    //. This is the assignment operator, which sets this
    //. <var>DwEncodedWord</var> object to the same value as
    //. <var>aEncodedWord</var>.

	void Parse(DwBool bHeaderSubjectField);
    virtual void Parse();
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. executes the parse method for <var>DwEncodedWord</var> objects. The
    //. parse method creates or updates the broken-down representation from
    //. the string representation.  For <var>DwEncodedWord</var> objects,
    //. the parse method parses the string representation to extract the
    //. charset, encoding type, the encoded text, and the decoded text.
    //.
    //. This function clears the is-modified flag.

    virtual void Assemble();
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. executes the assemble method for <var>DwEncodedWord</var> objects.
    //. It should be called whenever one of the object's attributes
    //. is changed in order to assemble the string representation from
    //. its broken-down representation.  It will be called
    //. automatically for this object by the parent object's
    //. <var>Assemble()</var> member function if the is-modified flag is set.
    //.
    //. This function clears the is-modified flag.

    virtual DwMessageComponent* Clone() const;
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. creates a new <var>DwEncodedWord</var> on the free store that has
    //. the same value as this <var>DwEncodedWord</var> object. The basic
    //. idea is that of a virtual copy constructor.

    const DwString& Charset() const;
    //. Returns the charset for this <var>DwEncodedWord</var> object.
    //. The charset must be one that is registered with the IANA, such
    //. as ISO-8859-1.

    void SetCharset(const DwString& aCharset);
    //. Sets the charset for this <var>DwEncodedWord</var> object.

    char EncodingType() const;
    //. Returns the encoding type for this <var>DwEncodedWord</var> object.

    void SetEncodingType(char anEncodingType);
    //. Sets the encoding type for this <var>DwEncodedWord</var> object.
    //. The encoding type must be one of: 'Q', 'q', 'B', 'b', or NUL.
    //. 'Q' and 'q' are equivalent and indicate the quoted-printable
    //. encoding.  'B' and 'b' are equivalent, and indicate the base64
    //. encoding.  The NUL character indicates no encoding, making this
    //. a pseudo-encoded word.

    const DwString& EncodedText() const;
    //. Returns the encoded text part of an encoded word.

    void SetEncodedText(const DwString& aStr);
    //. Sets the encoded text part of an encoded word.

    const DwString& DecodedText() const;
    //. Returns the decoded text in a <var>DwEncodedWords</var>.

    void SetDecodedText(const DwString& aStr);
    //. Sets the decoded text in a <var>DwEncodedWords</var>.

    static DwEncodedWord* NewEncodedWord(const DwString& aStr,
        DwMessageComponent* aParent);
    //. Creates a new <var>DwEncodedWord</var> object on the free store.  If
    //. the static data member <var>sNewDwEncodedWord</var> is
    //. <var>NULL</var>, this member function will create a new
    //. <var>DwEncodedWord</var> and return it.  Otherwise,
    //. <var>NewEncodedWord()</var> will call the user-supplied function
    //. pointed to by <var>sNewEncodedWord</var>, which is assumed to return
    //. an object from a class derived from <var>DwEncodedWord</var>, and
    //. return that object.

    //+ Var sNewEncodedWord
    static DwEncodedWord* (*sNewEncodedWord)(const DwString&,
        DwMessageComponent*);
    //. If <var>sNewEncodedWord</var> is not <var>NULL</var>, it is assumed to
    //. point to a  user-supplied function that returns an object from a
    //. class derived from <var>DwEncodedWord</var>.

    void PrintDebugInfo(DW_STD ostream& aStrm, int aDepth) const;
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. prints debugging information about this object to <var>aStrm</var>.
    //. It will also call <var>PrintDebugInfo()</var> for any of its child
    //. components down to a level of <var>aDepth</var>.
    //.
    //. This member function is available only in the debug version of
    //. the library.

    void CheckInvariants() const;
    //. Aborts if one of the invariants of the object fails.  Use this
    //. member function to track down bugs.
    //.
    //. This member function is available only in the debug version of
    //. the library.

protected:

    void _PrintDebugInfo(DW_STD ostream& aStrm) const;

private:

    static const char* const sClassName;

protected:

    DwString mCharset;
    char mEncodingType;
    DwString mEncodedText;
    DwString mDecodedText;

};

#endif
