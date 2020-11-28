//=============================================================================
// File:       field.h
// Contents:   Declarations for DwField
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

#ifndef DW_FIELD_H
#define DW_FIELD_H

#ifndef DW_CONFIG_H
#include <mimepp/config.h>
#endif

#ifndef DW_STRING_H
#include <mimepp/string.h>
#endif

#ifndef DW_MSGCMP_H
#include <mimepp/msgcmp.h>
#endif

class DwHeaders;
class DwFieldBody;

//=============================================================================
//+ Name DwField -- Class representing a MIME header field
//+ Description
//. <var>DwField</var> represents a header field as described in RFC-822.
//. According to RFC-822, a field contains a field name and a field body.
//. In MIME++, a <var>DwField</var> contains three elements: a <var>DwString
//. </var> that contains its field name, a <var>DwString</var> that contains
//. its field body, and a <var>DwFieldBody</var> object that contains a
//. broken-down (that is, parsed) version of its field body.
//.
//. In the tree (broken-down) representation of message, a <var>DwField</var>
//. object is always an intermediate node, having a parent node and a single
//. child node.  The parent node is the <var>DwHeaders</var> object that
//. contains it.  The child node is the <var>DwFieldBody</var> object it
//. contains.
//.
//. To get and set the field name, use the member functions
//. <var>FieldNameStr()</var> and <var>SetFieldNameStr()</var>.
//. To get and set the field body, use the member functions
//. <var>FieldBodyStr()</var> and <var>SetFieldBodyStr()</var>.
//. To get and set the <var>DwFieldBody</var> object, use <var>FieldBody()
//. </var> and <var>SetFieldBody()</var>.
//=============================================================================
// Last modified 1998-04-28
//+ Noentry ~DwField _CreateFieldBody mFieldNameStr mFieldBodyStr
//+ Noentry mFieldBody _PrintDebugInfo

class DW_CORE_EXPORT DwField : public DwMessageComponent {

public:

    DwField();
    DwField(const DwField& aField);
    DwField(const DwString& aStr, DwMessageComponent* aParent=0);
    //. The first constructor is the default constructor, which sets the
    //. <var>DwField</var> object's field name and field body to the empty
    //. string, set its parent to <var>NULL</var>, and sets its <var>DwFieldBody</var>
    //. object to <var>NULL</var>.
    //.
    //. The second constructor is the copy constructor, which performs
    //. a deep copy of <var>aField</var>.
    //. The parent of the new <var>DwField</var> object is set to
    //. <var>NULL</var>.
    //.
    //. The third constructor copies <var>aStr</var> to the <var>DwField</var>
    //. object's string representation and sets <var>aParent</var> as its
    //. parent. The virtual member function <var>Parse()</var> should be
    //. called immediately after this constructor in order to parse the
    //. string representation. Unless it is <var>NULL</var>, <var>aParent
    //. </var> should point to an object of a class derived from
    //. <var>DwHeaders</var>.

    virtual ~DwField();

    const DwField& operator = (const DwField& aField);
    //. This is the assignment operator, which performs a deep copy of
    //. <var>aField</var>.  The parent node of the <var>DwField</var> object
    //. is not changed.

    virtual void Parse();
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. executes the parse method for <var>DwField</var> objects.  The parse
    //. method creates or updates the broken-down representation from the
    //. string representation.  For <var>DwField</var> objects, the parse
    //. method parses the string representation, sets the values of the field
    //. name string and the field body string, and creates an instance
    //. of the appropriate subclass of <var>DwFieldBody</var>.  This member
    //. function also calls the <var>Parse()</var> member function of its
    //. contained <var>DwFieldBody</var> object.
    //.
    //. You should call this member function after you set or modify the
    //. string representation, and before you access the field name, the
    //. field body, or the contained <var>DwFieldBody</var> object.
    //.
    //. This function clears the is-modified flag.

    virtual void Assemble();
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. executes the assemble method for <var>DwField</var> objects. The
    //. assemble method creates or updates the string representation from
    //. the broken-down representation.  In more concrete terms, the
    //. assemble method builds the string representation from the field
    //. name and the string representation of the contained
    //. <var>DwFieldBody</var> object.  This member function calls the
    //. <var>Assemble()</var> member function of its contained
    //. <var>DwFieldBody</var> object.
    //.
    //. You should call this member function after you modify either the
    //. field name or the contained <var>DwFieldBody</var> object, and before
    //. you retrieve the string representation.
    //.
    //. This function clears the is-modified flag.

    virtual DwMessageComponent* Clone() const;
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. creates a new <var>DwField</var> on the free store that has the same
    //. value as this <var>DwField</var> object.  The basic idea is that of
    //. a virtual copy constructor.

    DwFieldBody* FieldBody() const;
    //. Returns the <var>DwFieldBody</var> object contained by this
    //. <var>DwField</var> object.  If there is no field body,
    //. <var>NULL</var> will be returned.

    const DwString& FieldNameStr() const;
    //. Returns the field name of this header field as a string.

    const DwString& FieldBodyStr() const;
    //. Returns the field body of this header field as a string.

    void SetFieldBody(DwFieldBody* aFieldBody);
    //. Sets the <var>DwFieldBody</var> object contained by this object.

    void SetFieldNameStr(const DwString& aStr);
    //. Sets the field name of this header field.

    void SetFieldBodyStr(const DwString& aStr);
    //. Sets the field body of this header field.

    static DwField* NewField(const DwString& aStr,
        DwMessageComponent* aParent);
    //. Creates a new <var>DwField</var> object on the free store.
    //. If the static data member <var>sNewField</var> is <var>NULL</var>,
    //. this member function will create a new <var>DwField</var>
    //. and return it.  Otherwise, <var>NewField()</var> will call
    //. the user-supplied function pointed to by <var>sNewField</var>,
    //. which is assumed to return an object from a class derived from
    //. <var>DwField</var>, and return that object.

    static DwFieldBody* CreateFieldBody(const DwString& aFieldName,
        const DwString& aFieldBody, DwMessageComponent* aParent);
    //. The static member function <var>CreateFieldBody()</var> is called from
    //. the <var>Parse()</var> member function and is responsible for creating
    //. a <var>DwFieldBody</var> object for this particular field.  A typical
    //. scenario might go as follows:
    //. This member function examines the field name for this field,
    //. finds that it contains "To", creates a <var>DwAddressList</var> object
    //. to contain the field body, calls the <var>Parse()</var> member
    //. function for the <var>DwAddressList</var>, and sets the
    //. <var>DwAddressList</var> object as this <var>DwField</var> object's
    //. <var>DwFieldBody</var>.
    //.
    //. If you want to override the behavior of <var>CreateFieldBody()</var>,
    //. you can do so by setting the public data member
    //. <var>sCreateFieldBody</var> to point to your own function.
    //. <var>CreateFieldBody()</var> first checks to see if
    //. <var>sCreateFieldBody</var> is <var>NULL</var>.  If it is not,
    //. <var>CreateFieldBody()</var> will assume that it points to a
    //. user-supplied function and will call that function.  If it is
    //. <var>NULL</var>, <var>CreateFieldBody()</var> will call
    //. <var>_CreateFieldBody()</var>, which actually creates the
    //. <var>DwFieldBody</var> object.  You may call <var>_CreateFieldBody()
    //. </var> from your own function for fields you do not wish to handle.

    static DwFieldBody* _CreateFieldBody(const DwString& aFieldName,
        const DwString& aFieldBody, DwMessageComponent* aParent);

    //+ Var sNewField
    static DwField* (*sNewField)(const DwString&, DwMessageComponent*);
    //. If <var>sNewField</var> is not <var>NULL</var>, it is assumed to point
    //. to a user-supplied function that returns an object from a class
    //. derived from <var>DwField</var>.

    //+ Var sCreateFieldBody
    static DwFieldBody* (*sCreateFieldBody)(const DwString& aFieldName,
        const DwString& aFieldBody, DwMessageComponent* aParent);
    //. See <var>CreateFieldBody()</var>.

protected:

    DwString mFieldNameStr;
    // the <i>field-name</i>

    DwString mFieldBodyStr;
    // the <i>field-body</i>

    DwFieldBody* mFieldBody;
    // pointer to the <var>DwFieldBody</var> object

    void _SetFieldBody(DwFieldBody* aFieldBody);
    //. Sets the <var>DwFieldBody</var> object contained by this object.  This
    //. function differs from <var>SetFieldBody()</var> in that it does not
    //. set the is-modified flag.

private:

    static const char* const sClassName;

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
