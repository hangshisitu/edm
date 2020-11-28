//=============================================================================
// File:       mechansm.h
// Contents:   Declarations for DwMechanism
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

#ifndef DW_MECHANSM_H
#define DW_MECHANSM_H

#ifndef DW_CONFIG_H
#include <mimepp/config.h>
#endif

#ifndef DW_STRING_H
#include <mimepp/string.h>
#endif

#ifndef DW_FIELDBDY_H
#include <mimepp/fieldbdy.h>
#endif


//=============================================================================
//+ Name DwMechanism -- Class representing a MIME content-transfer-encoding field-body
//+ Description
//. <var>DwMechanism</var> represents a field body for the
//. Content-Transfer-Encoding header field as described in RFC-2045.
//. <var>DwMechanism</var> provides convenience functions that allow you to
//. set or get the content-transfer-encoding attribute as an enumerated
//. value.
//=============================================================================
// Last modified 1998-04-28
//+ Noentry ~DwMechanism _PrintDebugInfo


class DW_CORE_EXPORT DwMechanism : public DwFieldBody {

public:

    DwMechanism();
    DwMechanism(const DwMechanism& aCte);
    DwMechanism(const DwString& aStr, DwMessageComponent* aParent=0);
    //. The first constructor is the default constructor, which sets the
    //. <var>DwMechanism</var> object's string representation to the empty
    //. string and sets its parent to <var>NULL</var>.
    //.
    //. The second constructor is the copy constructor, which copies the
    //. string representation from <var>aCte</var>.
    //. The parent of the new <var>DwMechanism</var> object is set to
    //. <var>NULL</var>.
    //.
    //. The third constructor copies <var>aStr</var> to the
    //. <var>DwMechanism</var> object's string representation and sets
    //. <var>aParent</var> as its parent. The virtual member function
    //. <var>Parse()</var> should be called immediately after this
    //. constructor in order to parse the string representation. Unless it
    //. is <var>NULL</var>, <var>aParent</var> should point to an object of
    //. a class derived from <var>DwField</var>.

    virtual ~DwMechanism();

    const DwMechanism& operator = (const DwMechanism& aCte);
    //. This is the assignment operator, which performs a deep copy of
    //. <var>aCte</var>.  The parent node of the <var>DwMechanism</var> object
    //. is not changed.

    virtual void Parse();
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. executes the parse method for <var>DwMechanism</var> objects.
    //. It should be called immediately after the string representation
    //. is modified and before any of the object's attributes are retrieved.
    //.
    //. This function clears the is-modified flag.

    virtual void Assemble();
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. executes the assemble method for <var>DwMechanism</var> objects.
    //. It should be called whenever one of the object's attributes
    //. is changed in order to assemble the string representation.
    //. It will be called automatically for this object by the parent
    //. object's <var>Assemble()</var> member function if the is-modified
    //. flag is set.
    //.
    //. This function clears the is-modified flag.

    virtual DwMessageComponent* Clone() const;
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. creates a new <var>DwMechanism</var> object on the free store that has
    //. the same value as this <var>DwMechanism</var> object.  The basic idea
    //. is that of a virtual copy constructor.

    int AsEnum() const;
    //. Returns the content transfer encoding as an enumerated value.
    //. Enumerated values are defined for all standard content transfer
    //. encodings in the file enum.h.  If the content transfer encoding
    //. is non-standard <var>DwMime::kCteUnknown</var> is returned.  The
    //. inherited member function <var>DwMessageComponent::AsString()</var>
    //. may be used to get the content transfer encoding, standard or
    //. non-standard, as a string.

    void FromEnum(int aCte);
    //. Sets the content transfer encoding from an enumerated value.
    //. Enumerated values are defined for all standard content transfer
    //. encodings in the file enum.h.  You may set the content transfer
    //. encoding to any string value, standard or non-standard, by using the
    //. inherited member function <var>DwMessageComponent::FromString()</var>.

    static DwMechanism*
        NewMechanism(const DwString& aStr, DwMessageComponent* aParent);
    //. Creates a new <var>DwMechanism</var> object on the free store.
    //. If the static data member <var>sNewMechanism</var> is <var>NULL</var>, 
    //. this member function will create a new <var>DwMechanism</var>
    //. and return it.  Otherwise, <var>NewMechanism()</var> will call
    //. the user-supplied function pointed to by
    //. <var>sNewMechanism</var>, which is assumed to return an
    //. object from a class derived from <var>DwMechanism</var>, and
    //. return that object.

    //+ Var sNewMechanism
    static DwMechanism*
        (*sNewMechanism)(const DwString&, DwMessageComponent*);
    //. If <var>sNewMechanism</var> is not <var>NULL</var>, it is assumed 
    //. to point to a user-supplied function that returns an object from
    //. a class derived from <var>DwMechanism</var>.

private:
    
    int mCteEnum;
    static const char* const sClassName;

    void EnumToString();
    void StringToEnum();

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
