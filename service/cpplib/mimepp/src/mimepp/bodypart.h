//=============================================================================
// File:       bodypart.h
// Contents:   Declarations for DwBodyPart
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

#ifndef DW_BODYPART_H
#define DW_BODYPART_H

#ifndef DW_CONFIG_H
#include <mimepp/config.h>
#endif

#ifndef DW_STRING_H
#include <mimepp/string.h>
#endif

#ifndef DW_ENTITY_H
#include <mimepp/entity.h>
#endif

class DwMessage;
class DwEntity;
class DwBody;


//=============================================================================
//+ Name DwBodyPart -- Class representing a MIME body-part
//+ Description
//. <var>DwBodyPart</var> represents a <i>body part</i>, as described in
//. RFC-2045 and RFC-2046.  A body part is an <i>entity</i>, so it has a
//. collection of headers and a <i>body</i>.  A body part is different
//. from a <i>message</i> in that a body part is part of a multipart body.
//.
//. In MIME++, a <var>DwBodyPart</var> is a subclass of <var>DwEntity</var>;
//. therefore, it contains both a <var>DwHeaders</var> object and a
//. <var>DwBody</var> object, and it is contained in a multipart
//. <var>DwBody</var> object.
//.
//. As with <var>DwMessage</var>, most of the functionality of
//. <var>DwBodyPart</var> is implemented by the abstract class
//. <var>DwEntity</var>.
//=============================================================================
// Last modified 1998-04-28
//+ Noentry ~DwBodyPart _PrintDebugInfo sClassName


class DW_CORE_EXPORT DwBodyPart : public DwEntity {

public:

    DwBodyPart();
    DwBodyPart(const DwBodyPart& aPart);
    DwBodyPart(const DwString& aStr, DwMessageComponent* aParent=0);
    //. The first constructor is the default constructor, which sets the
    //. <var>DwBodyPart</var> object's string representation to the empty
    //. string and sets its parent to <var>NULL</var>.
    //.
    //. The second constructor is the copy constructor, which performs
    //. a deep copy of <var>aPart</var>.
    //. The parent of the new <var>DwBodyPart</var> object is set to
    //. <var>NULL</var>.
    //.
    //. The third constructor copies <var>aStr</var> to the <var>DwBodyPart
    //. </var> object's string representation and sets <var>aParent</var>
    //. as its parent. The virtual member function <var>Parse()</var> should
    //. be called immediately after this constructor in order to parse the
    //. string representation. Unless it is <var>NULL</var>, <var>aParent
    //. </var> should point to an object of a class derived from
    //. <var>DwBody</var>.

    virtual ~DwBodyPart();

    const DwBodyPart& operator = (const DwBodyPart& aPart);
    //. This is the assignment operator, which performs a deep copy of
    //. <var>aPart</var>.  The parent node of the <var>DwBodyPart</var> object
    //. is not changed.

    virtual DwMessageComponent* Clone() const;
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. creates a new <var>DwBodyPart</var> on the free store that has the same
    //. value as this <var>DwBodyPart</var> object.  The basic idea is that of
    //. a virtual copy constructor.

    static DwBodyPart* NewBodyPart(const DwString& aStr,
        DwMessageComponent* aParent);
    //. Creates a new <var>DwBodyPart</var> on the free store.
    //. If the static data member <var>sNewBodyPart</var> is <var>NULL</var>, 
    //. this member function will create a new <var>DwBodyPart</var>
    //. and return it.  Otherwise, <var>NewBodyPart()</var> will call
    //. the user-supplied function pointed to by <var>sNewBodyPart</var>,
    //. which is assumed to return an object from a class derived from
    //. <var>DwBodyPart</var>, and return that object.

    //+ Var sNewBodyPart
    static DwBodyPart* (*sNewBodyPart)(const DwString&, DwMessageComponent*);
    //. If <var>sNewBodyPart</var> is not <var>NULL</var>, it is assumed
    //. to point to a user-supplied function that returns an object from
    //. a class derived from <var>DwBodyPart</var>.

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

private:

    static const char* const sClassName;

};

#endif

