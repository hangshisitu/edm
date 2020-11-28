//=============================================================================
// File:       msgcmp.h
// Contents:   Declarations for DwMessageComponent
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

#ifndef DW_MSGCMP_H
#define DW_MSGCMP_H

#ifndef DW_CONFIG_H
#include <mimepp/config.h>
#endif

#ifndef DW_STRING_H
#include <mimepp/string.h>
#endif


//=============================================================================
//+ Name DwMessageComponent -- Abstract base class for all message components
//+ Description
//. <var>DwMessageComponent</var> is the root of an inheritance hierarchy from
//. which all MIME message components are derived.  Thus,
//. <var>DwMessageComponent</var> defines important features that are
//. inherited by nearly all other classes that represent components of
//. a MIME message. These features are the following:
//.
//. <ul>
//. <li>
//.    A string representation.  The <var>DwMessageComponent</var> class
//.    provides a member function <var>FromString()</var> to set the string
//.    representation and a member function <var>AsString()</var> to get the
//.    string representation.
//. <li>
//.    A broken-down, or parsed, representation.  An RFC-822 <i>date-time</i>,
//.    for example, has a year, month, day, hour, minute, second, and
//.    time zone as elements of its broken-down representation.
//.    <var>DwMessageComponent</var> does not deal directly with the
//.    broken-down representation, since it is component-specific.
//.    Derived classes bear all the responsibility for their broken-down
//.    representations.
//. <li>
//.    A parse method to extract the broken-down representation from
//.    the string representation.  In the <var>DwDateTime</var> class, for
//.    example, the parse method extracts the year, month, day, hour,
//.    minute, second, and time zone from the RFC-822 <i>date-time</i>
//.    contained in the string representation.  <var>DwMessageComponent</var>
//.    provides a pure virtual function <var>Parse()</var>, which executes 
//.    the parse method for a derived class.
//. <li>
//.    An assemble method to convert the broken-down representation to
//.    a string representation.  This is the opposite of the parse method.
//.    In the <var>DwDateTime</var> class, for example, the assemble method
//.    creates an RFC-822 <i>date-time</i> string from values of the
//.    year, month, day, hour, minute, second, and time zone.
//.    <var>DwMessageComponent</var> provides a pure virtual function
//.    <var>Assemble()</var>, which executes the assemble method for
//.    a derived class.
//. <li>
//.    An is-modified flag.  When the string representation and the
//.    broken-down representation are consistent, the assemble
//.    method does not need to be executed.  The is-modified flag is
//.    cleared when the two representations are consistent, and is set
//.    when they are inconsistent.  The flag is set automatically
//.    whenever a <var>DwMessageComponent</var> object's broken-down
//.    representation is changed by calling one of the object's member
//.    functions, and it is cleared when the assemble or parse method
//.    is executed.  <var>DwMessageComponent</var> also provides a member
//.    function <var>SetModified()</var> which forces the is-modified flag
//.    to be set.
//. <li>
//.    A parent.  Most message components are part of another component.
//.    A collection of headers is part of a message or body part, a header
//.    field is part of a collection of headers, a field-body is part
//.    of a header field, and so on.  The parent of
//.    a component is the component that contains it.  This tree structure
//.    is important, since a component's parent must be parsed before the
//.    component can be.  Also, a component's string representation must
//.    be assembled before its parent's.  To maintain consistency in the
//.    tree, whenever a component's is-modified flag is set,
//.    the component notifies its parent to also set its is-modified flag.
//.    In this way, an is-modified flag set anywhere in the tree always
//.    propagates up to the root component.
//. <li>
//.    Children.  The preceding discussion about a component's parent is
//.    relevant to an understanding of a component's children.  A component's
//.    parse method calls the parse methods of its children after it has
//.    executed its own parse method (and, in some cases, created
//.    all of its children).
//.    Also, a component typically calls the assemble method of its
//.    children before it executes its own.  A component's child may request
//.    that the component set its is-modified flag.
//.    <var>DwMessageComponent</var> does not deal directly with children.
//.    Derived classes bear all the responsibility for handling their
//.    children.
//. </ul>
//=============================================================================
// Last modified 1998-04-28
//+ Noentry ~DwMessageComponent _PrintDebugInfo mString mIsModified mParent
//+ Noentry mClassId mClassName

class DW_CORE_EXPORT DwMessageComponent {

private:

    DwUint32 mMagicNumber;

public:

    enum componentType {
        kCidError=-1,
        kCidUnknown=0,
        kCidAddress,
        kCidAddressList,
        kCidBody,
        kCidBodyPart,
        kCidDispositionType,
        kCidMechanism,
        kCidMediaType,
        kCidParameter,
        kCidDateTime,
        kCidEntity,
        kCidField,
        kCidFieldBody,
        kCidGroup,
        kCidHeaders,
        kCidMailbox,
        kCidMailboxList,
        kCidMessage,
        kCidMessageComponent,
        kCidMsgId,
        kCidStamp,
        kCidText,
        kCidEncodedWord
    };
    //. Class identifier enumeration

    DwMessageComponent();
    DwMessageComponent(const DwMessageComponent& aCmp);
    DwMessageComponent(const DwString& aStr, DwMessageComponent* aParent=0);
    //. The first constructor is the default constructor, which sets the
    //. <var>DwMessageComponent</var> object's string representation to the
    //. empty string and sets its parent to <var>NULL</var>.
    //.
    //. The second constructor is the copy constructor, which performs
    //. a deep copy of <var>aCmp</var>.  The parent of the new
    //. <var>DwMessageComponent</var> object is set to <var>NULL</var>.
    //.
    //. The third constructor copies <var>aStr</var> to the new
    //. <var>DwMessageComponent</var> object's string representation and sets
    //. <var>aParent</var> as its parent.  In typical cases, the virtual
    //. member function <var>Parse()</var> should be called immediately after
    //. this constructor to parse the new <var>DwMessageComponent</var> object
    //. and all of its children into their broken-down representations.

    virtual ~DwMessageComponent();

    const DwMessageComponent& operator = (const DwMessageComponent& aCmp);
    //. This is the assignment operator, which performs a deep copy of
    //. <var>aCmp</var>.

    virtual void Parse() = 0;
    //. A pure virtual function which provides an interface to the parse
    //. method.  The parse method, implemented in derived classes, is
    //. responsible for extracting the broken-down representation from
    //. the string representation.  In some derived classes, such as
    //. <var>DwHeaders</var>, the parse method is also responsible for
    //. creating the children of the object.  (In the case of
    //. <var>DwHeaders</var>, the children created are the <var>DwField
    //. </var>objects that represent the <i>fields</i> contained in the
    //. <i>headers</i>.)  The <var>Parse()</var> function always calls
    //. the <var>Parse()</var> function of all of its children.

    virtual void Assemble() = 0;
    //. A pure virtual function which provides an interface to the
    //. assemble method.  The assemble method, implemented in derived
    //. classes, is responsible for creating the string representation
    //. from the broken-down representation.  In other words, the 
    //. assemble method is the opposite of the parse method.  Before
    //. assembling its string representation, the assemble method calls
    //. the assemble method of each of its children.  In this way, the
    //. entire tree structure that represents a message may be traversed.
    //. If the is-modifed flag for a <var>DwMessageComponent</var> is cleared,
    //. the <var>Assemble()</var> function will return immediately without
    //. calling the <var>Assemble()</var> function of any of its children.

    virtual DwMessageComponent* Clone() const = 0;
    //. Creates a new <var>DwMessageComponent</var> on the free store that
    //. is of the same type as, and has the same value as, this object.  The
    //. basic idea is that of a ``virtual copy constructor.''

    void FromString(const DwString& aStr);
    void FromString(const char* aCstr);
    //. Sets the object's string representation.  <var>aCstr</var> must be
    //. NUL-terminated. This member function does not invoke the parse
    //. method.  Typically, the virtual member function <var>Parse()</var>
    //. should be called immediately after this member function to parse
    //. the <var>DwMessageComponent</var> object and all of its children into
    //. their broken-down representations.  See also 
    //. <var>DwMessageComponent::Parse()</var>

    const DwString& AsString() const;
    //. Returns the <var>DwMessageComponent</var> object's string
    //. representation. The assemble method is not called automatically.
    //. Typically, the <var>Assemble()</var> member function should be
    //. called immediately before this member function to insure that
    //. the broken-down representation and the string representation
    //. are consistent.  See also <var>DwMessageComponent::Assemble()</var>.

    DwMessageComponent* Parent();
    //. Returns the <var>DwMessageComponent</var> object that is the parent
    //. of this object.

    void SetParent(DwMessageComponent* aParent);
    //. Sets <var>aParent</var> as the <var>DwMessageComponent</var>
    //. object's parent.

    DwBool IsModified() const;
    //. Returns true value (<var>DwTrue</var>) if the is-modified flag is
    //. set for this <var>DwMessageComponent</var> object.

    void SetModified();
    //. Sets the is-modified (dirty) flag for this <var>DwMessageComponent
    //. </var> object and notifies the object's parent to also set its
    //. is-modified flag.

    int ClassId() const;
    //. Returns an integer id for the object's class.

    const char* ClassName() const;
    //. Returns the name of the class as a NUL-terminated
    //. char string.

    int ObjectId() const;
    //. Returns a object id that is unique among all DwMessageComponent
    //. objects.

protected:

    DwString mString;
    // String representation

    DwBool mIsModified;
    // Is-modified flag

    DwMessageComponent* mParent;
    // Component that contains this component

    componentType mClassId;
    // Class identifier for runtime type identification

    const char* mClassName;
    // Class name for runtime type identification

private:

    static const char* const sClassName;

public:

    virtual void PrintDebugInfo(DW_STD ostream& aStrm, int aDepth=0) const;
    //. This virtual function
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
