//=============================================================================
// File:       entity.h
// Contents:   Declarations for DwEntity
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

#ifndef DW_ENTITY_H
#define DW_ENTITY_H

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
class DwBody;

//=============================================================================
//+ Name DwEntity -- Abstract class representing a MIME entity
//+ Description
//. RFC-2045 defines an <i>entity</i> as either a <i>message</i> or a
//. <i>body part</i>, both of which have a collection of headers and
//. a <i>body</i>.  In MIME++, an entity is represented by the class
//. <var>DwEntity</var>, which contains both a <var>DwHeaders</var> object
//. and a <var>DwBody</var> object.
//.
//. In the tree (broken-down) representation of message, a <var>DwEntity</var>
//. object may be either a root node, having child nodes but no parent
//. node, or an intermediate node, having both a parent node and child nodes.
//. A <var>DwEntity</var> object that is a root node must also be a
//. <var>DwMessage</var> object.  If a <var>DwEntity</var> object is an
//. intermediate node, its parent must be a <var>DwBody</var> object.  The
//. child nodes of a <var>DwEntity</var> object are the <var>DwHeaders</var>
//. and <var>DwBody</var> objects it contains.
//.
//. Since <var>DwEntity</var> is an abstract base class, you cannot create
//. instances of it directly.  <var>DwEntity</var> has two derived classes,
//. <var>DwMessage</var> and <var>DwBodyPart</var>, which are concrete classes.
//.
//. To access the contained <var>DwHeaders</var> object, use the member
//. function <var>Headers()</var>.  To access the contained <var>DwBody</var>
//. object, use the member function <var>Body()</var>.
//=============================================================================
// Last modified 1998-04-28
//+ Noentry ~DwEntity mHeaders mBody _PrintDebugInfo

class DW_CORE_EXPORT DwEntity : public DwMessageComponent {

public:

    DwEntity();
    DwEntity(const DwEntity& aEntity);
    DwEntity(const DwString& aStr, DwMessageComponent* aParent=0);
    //. The first constructor is the default constructor, which sets the
    //. <var>DwEntity</var> object's string representation to the empty string
    //. and sets its parent to <var>NULL</var>.
    //.
    //. The second constructor is the copy constructor, which performs
    //. a deep copy of <var>aEntity</var>.
    //. The parent of the new <var>DwEntity</var> object is set to
    //. <var>NULL</var>.
    //.
    //. The third constructor copies <var>aStr</var> to the
    //. <var>DwEntity</var>  object's string representation and sets
    //. <var>aParent</var> as its parent. The virtual member function
    //. <var>Parse()</var> should be called immediately after this
    //. constructor in order to parse the string representation. Unless
    //. it is <var>NULL</var>, <var>aParent</var> should point to an object
    //. of a class derived from <var>DwBody</var>.

    virtual ~DwEntity();

    const DwEntity& operator = (const DwEntity& aEntity);
    //. This is the assignment operator, which performs a deep copy of
    //. <var>aEntity</var>.  The parent node of the <var>DwEntity</var> object
    //. is not changed.

    virtual void Parse();
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. executes the parse method for <var>DwEntity</var> objects.  The parse
    //. method creates or updates the broken-down representation from the
    //. string representation.  For <var>DwEntity</var> objects, the parse
    //. method parses the string representation and sets the values of
    //. the <var>DwHeaders</var> and <var>DwBody</var> objects it contains.
    //. This member function also calls the <var>Parse()</var> member functions
    //. of the contained <var>DwHeaders</var> and <var>DwBody</var> objects.
    //.
    //. You should call this member function after you set or modify the
    //. string representation, and before you access either the contained
    //. headers or body.
    //.
    //. This function clears the is-modified flag.

    virtual void Assemble();
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. executes the assemble method for <var>DwEntity</var> objects.  The
    //. assemble method creates or updates the string representation from
    //. the broken-down representation.  In more concrete terms, the
    //. assemble method builds the string representation from the string
    //. representations of the contained <var>DwHeaders</var> and
    //. <var>DwBody</var> objects.  This member function calls the
    //. <var>Assemble()</var> member functions of its <var>DwHeaders</var>
    //. and <var>DwBody</var> objects.
    //.
    //. You should call this member function after you modify either the
    //. contained headers or body, and before you retrieve the string 
    //. representation.
    //.
    //. This function clears the is-modified flag.

    DwHeaders& Headers() const;
    //. This function returns the <var>DwHeaders</var> object contained by
    //. this object.

    DwBody& Body() const;
    //. This function returns the <var>DwBody</var> object contained by
    //. this object.

protected:

    DwHeaders* mHeaders;
    DwBody*    mBody;

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
