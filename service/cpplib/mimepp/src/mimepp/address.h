//=============================================================================
// File:       address.h
// Contents:   Declarations for DwAddress
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

#ifndef DW_ADDRESS_H
#define DW_ADDRESS_H

#ifndef DW_CONFIG_H
#include <mimepp/config.h>
#endif

#ifndef DW_FIELDBDY_H
#include <mimepp/fieldbdy.h>
#endif

#ifndef DW_TOKEN_H
#include <mimepp/token.h>
#endif

class DwAddressList;
class DwMailboxList;

//=============================================================================
//+ Name DwAddress -- Abstract class representing an RFC-822 address
//+ Description
//. <var>DwAddress</var> represents an <i>address</i> as described in RFC-822.
//. You may not instantiate objects of type <var>DwAddress</var>, since
//. <var>DwAddress</var> is an abstract base class.  Instead, you must
//. instantiate objects of type <var>DwMailbox</var> or <var>DwGroup</var>,
//. which are subclasses of <var>DwAddress</var>.
//.
//. To determine the actual type of a <var>DwAddress</var> object, you can use
//. the member functions <var>IsMailbox()</var> and <var>IsGroup()</var>.
//.
//. If the string representation assigned to a <var>DwAddress</var> is
//. improperly formed, the parse method will fail.  To determine if the
//. parse method failed, call the member function <var>IsValid()</var>.
//=============================================================================
// Last modified 1998-04-28
//+ Noentry ~DwAddress mIsValid sClassName _PrintDebugInfo


class DW_CORE_EXPORT DwAddress : public DwFieldBody {

public:

    virtual ~DwAddress();

    DwBool IsMailbox() const;
    //. Returns true value if this object is a <var>DwMailbox</var>.

    DwBool IsGroup() const;
    //. Returns true value if this object is a <var>DwGroup</var>.

    DwBool IsValid() const;
    //. Returns true value if the last parse was successful.
    //. Returns false if the last parse failed (bad address) or
    //. the <var>Parse()</var> member function was never called.

protected:

    DwAddress();
    DwAddress(const DwAddress& aAddr);
    DwAddress(const DwString& aStr, DwMessageComponent* aParent=0);
    //. The first constructor is the default constructor, which sets the
    //. <var>DwAddress</var> object's string representation to the empty string
    //. and sets its parent to <var>NULL</var>.
    //.
    //. The second constructor is the copy constructor, which copies the
    //. string representation and all attributes from <var>aAddress</var>.
    //. The parent of the new <var>DwAddress</var> object is set to
    //. <var>NULL</var>.
    //.
    //. The third constructor copies <var>aStr</var> to the <var>DwAddress
    //.</var> object's string representation and sets <var>aParent</var>
    //. as its parent. The virtual member function <var>Parse()</var> should
    //. be called immediately after this constructor in order to parse the
    //. string representation. Unless it is <var>NULL</var>, <var>aParent</var>
    //. should point to an object of a class derived from <var>DwField</var>.

    const DwAddress& operator = (const DwAddress& aAddr);
    //. This is the assignment operator, which performs a deep copy of
    //. <var>aAddr</var>.  The parent node of the <var>DwAddress</var> object
    //. is not changed.

    DwBool mIsValid;
    //. This data member is set to true if the parse method was successful.

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
