//=============================================================================
// File:       text.h
// Contents:   Declarations for DwText
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

#ifndef DW_TEXT_H
#define DW_TEXT_H

#ifndef DW_CONFIG_H
#include <mimepp/config.h>
#endif

#ifndef DW_STRING_H
#include <mimepp/string.h>
#endif

#ifndef DW_ENCODED_H
#include <mimepp/encoded.h>
#endif

#ifndef DW_FIELDBDY_H
#include <mimepp/fieldbdy.h>
#endif

//=============================================================================
//+ Name DwText -- Class representing text in a RFC-822 header field-body
//+ Description
//. <var>DwText</var> represents an unstructured field body in a header field.
//. It roughly corresponds to the <i>text</i> element of the BNF grammar
//. defined in RFC-822.
//.
//. If non-ASCII text is not a concern, you can get or set the entire text
//. of the <var>DwText</var> object by using the inherited member functions
//. <var>DwMessageComponent::AsString()</var> and
//. <var>DwMessageComponent::FromString()</var>.  However, <var>DwText</var>
//. also supports non-ASCII text via the encoded words specified in RFC
//. 2047.  The encoded words are represented by objects of the class
//. <var>DwEncodedWord</var>.  <var>DwText</var> contains a list of
//. <var>DwEncodedWord</var> objects and provides member functions to manage
//. the list.
//.
//. To create a text field body that contains encoded words, create one or
//. more <var>DwEncodedWord</var> objects and add them, in order, to the
//. <var>DwText</var> object.  When the <var>Assemble()</var> method is
//. called, the text field body is built from the contained
//. <var>DwEncodedWord</var> objects.  (Note that the <var>Assemble()</var>
//. member function is normally not called directly, but is called by the
//. parent <var>DwMessageComponent</var> object.)  To create a text field
//. body that contains no encoded words (it must contain only US-ASCII
//. characters), use the inherited <var>DwMessageComponent::FromString()</var>
//. member function.
//.
//. Note: In the examples directory you can find code that demonstrates how
//. to use <code>DwText</code> and <code>DwEncodedWord</code> classes to
//. set non-ASCII text into the Subject header field of a message.
//=============================================================================
// Last modified 2000-02-21
//+ Noentry ~DwText sClassName _PrintDebugInfo


class DW_CORE_EXPORT DwText : public DwFieldBody {

public:

    DwText(DwBool bHeaderSubjectField = DwFalse);
    DwText(const DwText& aText);
    DwText(const DwString& aStr, DwMessageComponent* aParent=0);
    //. The first constructor is the default constructor, which sets the
    //. <var>DwText</var> object's string representation to the empty string
    //. and sets its parent to NULL.
    //.
    //. The second constructor is the copy constructor, which copies the
    //. string representation from <var>aText</var>.
    //. The parent of the new <var>DwText</var> object is set to NULL.
    //.
    //. The third constructor copies <var>aStr</var> to the <var>DwText</var>
    //. object's string representation and sets <var>aParent</var> as its
    //. parent. The virtual member function <var>Parse()</var> should be
    //. called immediately after this constructor in order to parse the
    //. string representation. Unless it is NULL, <var>aParent</var> should
    //. point to an object of a class derived from <var>DwField</var>.

    virtual ~DwText();

    const DwText& operator = (const DwText& aText);
    //. This is the assignment operator.

    virtual void Parse();
    //. This virtual member function is inherited from
    //. <var>DwMessageComponent</var>, where it is declared a pure virtual
    //. function.  For a <var>DwText</var> object, this member function does
    //. nothing, since <var>DwText</var> represents an unstructured field body
    //. (like the Subject header field) that does not have a broken-down
    //. form.
    //.
    //. Note, however, that this function should still be called consistently,
    //. since a subclass of <var>DwText</var> may implement a parse method.
    //.
    //. This function clears the is-modified flag.

    virtual void Assemble();
    //. This virtual member function is inherited from
    //. <var>DwMessageComponent</var>, where it is declared a pure virtual
    //. function.  For a <var>DwText</var> object, this member function does
    //. nothing, since <var>DwText</var> represents an unstructured field body
    //. (like the Subject header field) that does not have a broken-down
    //. form.
    //.
    //. Note, however, that this function should still be called consistently,
    //. since a subclass of <var>DwText</var> may implement an assemble method.
    //.
    //. This function clears the is-modified flag.

    virtual DwMessageComponent* Clone() const;
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. creates a new <var>DwText</var> on the free store that has the same
    //. value as this <var>DwText</var> object.  The basic idea is that of
    //. a ``virtual copy constructor.''

    int NumEncodedWords() const;
    //. Returns the number of <var>DwEncodedWord</var> objects contained in
    //. this <var>DwText</var> object.

    void AddEncodedWord(DwEncodedWord* aWord);
    //. Appends a <var>DwEncodedWord</var> object to the list managed by
    //. this <var>DwText</var> object.
    //. 
    //. Any <var>DwEncodedWord</var> objects contained in a
    //. <var>DwText</var> object's list will be deleted by the
    //. <var>DwText</var> object's destructor.

    void DeleteAllEncodedWords();
    //. Removes and deletes all <var>DwEncodedWord</var> objects contained in
    //. this <var>DwText</var> object.

    DwEncodedWord& EncodedWordAt(int aIndex) const;
    //. Returns the <var>DwEncodedWord</var> object at position
    //. <var>aIndex</var> in this <var>DwText</var> object's list.
    //.
    //. If the library is compiled to throw exceptions, the function will
    //. throw a <var>DwBoundsException</var> if <var>aIndex</var> is out of
    //. range. Otherwise, (if the library is compiled <i>not</i> to throw
    //. exceptions) the behavior is undefined if <var>aIndex</var> is out of
    //. range.  Valid values for <var>aIndex</var> are 0 &lt;=
    //. <var>aIndex</var> &lt; <var>NumEncodedWords()</var>.

    void InsertEncodedWordAt(int aIndex, DwEncodedWord* aWord);
    //. Inserts <var>aWord</var> into the list of <var>DwEncodedWord</var>
    //. objects at the position given by <var>aIndex</var>.  A value of zero
    //. for <var>aIndex</var> will insert <var>aWord</var> as the first one
    //. in the list; a value of <var>NumEncodedWords()</var> will insert it
    //. as the last one in the list.
    //. If <var>aIndex</var> is less than <var>NumEncodedWords()</var>, the
    //. <var>DwEncodedWord</var> objects at position <var>aIndex</var> or
    //. greater will be shifted down the list to make room to insert
    //. <var>aWord</var>.
    //. 
    //. Any <var>DwEncodedWord</var> objects contained in a
    //. <var>DwText</var> object's list will be deleted by the
    //. <var>DwText</var> object's destructor.
    //.
    //. If the library is compiled to throw exceptions, the function will
    //. throw a <var>DwBoundsException</var> if <var>aIndex</var> is out of
    //. range. Otherwise, (if the library is compiled <i>not</i> to throw
    //. exceptions) the behavior is undefined if <var>aIndex</var> is out of
    //. range.  Valid values for <var>aIndex</var> are 0 &lt;=
    //. <var>aIndex</var> &lt;= <var>NumEncodedWords()</var>.

    DwEncodedWord* RemoveEncodedWordAt(int aIndex);
    //. Removes the <var>DwEncodedWord</var> object at position
    //. <var>aIndex</var> from the list and returns it.
    //. If <var>aIndex</var> is less than <var>NumEncodedWords()-1</var>,
    //. then any <var>DwEncodedWord</var> objects at a position greater than
    //. <var>aIndex</var> will be shifted up in the list after the requested
    //. <var>DwEncodedWord</var> is removed.
    //.
    //. If the library is compiled to throw exceptions, the function will
    //. throw a <var>DwBoundsException</var> if <var>aIndex</var> is out of
    //. range. Otherwise, (if the library is compiled <i>not</i> to throw
    //. exceptions) the behavior is undefined if <var>aIndex</var> is out of
    //. range.  Valid values for <var>aIndex</var> are 0 &lt;=
    //. <var>aIndex</var> &lt; <var>NumEncodedWords()</var>.

    static DwText* NewText(const DwString& aStr, DwMessageComponent* aParent);
    //. Creates a new <var>DwText</var> object on the free store.
    //. If the static data member <var>sNewText</var> is NULL, 
    //. this member function will create a new <var>DwText</var>
    //. and return it.  Otherwise, <var>NewText()</var> will call
    //. the user-supplied function pointed to by <var>sNewText</var>,
    //. which is assumed to return an object from a class derived from
    //. <var>DwText</var>, and return that object.

    //+ Var sNewText
    static DwText* (*sNewText)(const DwString&, DwMessageComponent*);
    //. If <var>sNewText</var> is not NULL, it is assumed to point to a 
    //. user-supplied function that returns an object from a class derived
    //. from  <var>DwText</var>.

	void SetEncodeTextSymbol(DwBool bHeaderSubjectField) { m_bHeaderSubjectField = bHeaderSubjectField; }

private:

    static const char* const sClassName;
    int             mNumEncodedWords;
    DwEncodedWord** mEncodedWords;
    int             mEncodedWordsSize;

	//added by WELKIN to fix DwDecodeQuotedPrintable '_' in header
	DwBool			m_bHeaderSubjectField;

    void DwText_Init();
    void _CopyEncodedWords(const DwText& aText);
    void _AddEncodedWord(DwEncodedWord* aWord);
    void _InsertEncodedWordAt(int aIndex, DwEncodedWord* aWord);
    void _DeleteAllEncodedWords();

public:

    virtual void PrintDebugInfo(DW_STD ostream& aStrm, int aDepth=0) const;
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. prints debugging information about this object to <var>aStrm</var>.
    //. It will also call <var>PrintDebugInfo()</var> for any of its child
    //. components down to a level of <var>aDepth</var>.
    //.
    //. This member function is available only in the debug version of
    //. the library.

    virtual void CheckInvariants() const;
    //. Aborts if one of the invariants of the object fails.  Use this
    //. member function to track down bugs.
    //.
    //. This member function is available only in the debug version of
    //. the library.

protected:

    void _PrintDebugInfo(DW_STD ostream& aStrm) const;

};

#endif
