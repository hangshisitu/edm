//=============================================================================
// File:       param.h
// Contents:   Declarations for DwParameter
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

#ifndef DW_PARAM_H
#define DW_PARAM_H

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
//+ Name DwParameter -- Class representing a MIME field body parameter
//+ Description
//. <var>DwParameter</var> represents the <i>parameter</i> component of the
//. Content-Type header field as described in RFC-2045.  A parameter
//. consists of an attribute/value pair.  <var>DwParameter</var> has member
//. functions for getting or setting a parameter's attribute and value.
//=============================================================================
// Last modified 1998-04-28
//+ Noentry ~DwParameter _PrintDebugInfo

class DW_CORE_EXPORT DwParameter : public DwMessageComponent {

public:

    DwParameter();
    DwParameter(const DwParameter& aParam);
    DwParameter(const DwString& aStr, DwMessageComponent* aParent=0);
    //. The first constructor is the default constructor, which sets the
    //. <var>DwParameter</var> object's string representation to the empty
    //. string and sets its parent to NULL.
    //.
    //. The second constructor is the copy constructor, which copies the
    //. string representation, attribute, and value from <var>aParam</var>.
    //. The parent of the new <var>DwParameter</var> object is set to NULL.
    //.
    //. The third constructor copies <var>aStr</var> to the
    //. <var>DwParameter</var> object's string representation and sets
    //. <var>aParent</var> as its parent. The virtual member function
    //. <var>Parse()</var> should be called immediately after this
    //. constructor in order to parse the string representation. Unless
    //. it is NULL, <var>aParent</var> should point to an object of a class
    //. derived from <var>DwMediaType</var>.

    virtual ~DwParameter();

    const DwParameter& operator = (const DwParameter& aParam);
    //. This is the assignment operator.

    virtual void Parse();
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. executes the parse method for <var>DwParameter</var> objects.
    //. It should be called immediately after the string representation
    //. is modified and before the parts of the broken-down
    //. representation are accessed.

    virtual void Assemble();
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. executes the assemble method for <var>DwParameter</var> objects.
    //. It should be called whenever one of the object's attributes
    //. is changed in order to assemble the string representation from
    //. its broken-down representation.  It will be called
    //. automatically for this object by the parent object's
    //. <var>Assemble()</var> member function if the is-modified flag is set.

    virtual DwMessageComponent* Clone() const;
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. creates a new <var>DwParameter</var> on the free store that has the
    //. same value as this <var>DwParameter</var> object.  The basic idea is
    //. that of a ``virtual copy constructor.''

    const DwString& Attribute() const;
    //. Returns the attribute contained by this parameter.

    void SetAttribute(const DwString& aAttribute);
    //. Sets the attribute contained by this parameter.

    const DwString& Value() const;
    //. Returns the value contained by this parameter.

    void SetValue(const DwString& aValue);
    //. Sets the value contained by this parameter.

    static DwParameter* NewParameter(const DwString& aStr,
        DwMessageComponent* aParent);
    //. Creates a new <var>DwParameter</var> object on the free store.
    //. If the static data member <var>sNewParameter</var> is NULL, 
    //. this member function will create a new <var>DwParameter</var>
    //. and return it.  Otherwise, <var>NewParameter()</var> will call
    //. the user-supplied function pointed to by <var>sNewParameter</var>,
    //. which is assumed to return an object from a class derived from
    //. <var>DwParameter</var>, and return that object.

    //+ Var sNewParameter
    static DwParameter* (*sNewParameter)(const DwString&, DwMessageComponent*);
    //. If <var>sNewParameter</var> is not NULL, it is assumed to point to a 
    //. user-supplied function that returns an object from a class derived
    //. from  <var>DwParameter</var>.

private:

    DwString mAttribute;
    DwString mValue;
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
