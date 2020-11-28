//=============================================================================
// File:       mailbox.h
// Contents:   Declarations for DwMailbox
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

#ifndef DW_MAILBOX_H
#define DW_MAILBOX_H

#ifndef DW_CONFIG_H
#include <mimepp/config.h>
#endif

#ifndef DW_STRING_H
#include <mimepp/string.h>
#endif

#ifndef DW_ADDRESS_H
#include <mimepp/address.h>
#endif

class DwGroup;

//=============================================================================
//+ Name DwMailbox -- Class representing an RFC-822 mailbox
//+ Description
//. RFC-822 defines a <i>mailbox</i> as an entity that can be the recipient
//. of a message.  A mailbox is more specific than an <i>address</i>, which
//. may be either a mailbox or a <i>group</i>.  An RFC-822 mailbox contains
//. a full name, a <i>local-part</i>, an optional <i>route</i>, and a
//. <i>domain</i>.  For example, in the mailbox
//.
//.   Joe Schmoe &lt;jschmoe@aol.com&gt;
//.
//. "Joe Schmoe" is the full name, "jschmoe" is the local-part, and
//. "aol.com" is the domain.  The optional route is rarely seen in current
//. usage, and is deprecated according to RFC-1123.
//.
//. In MIME++, an RFC-822 mailbox is represented by a <var>DwMailbox</var>
//. object. <var>DwMailbox</var> is a subclass of <var>DwAddress</var>, which
//. reflects the fact that a mailbox is also an address.  A <var>DwMailbox
//. </var> contains strings representing the full name, local-part, route,
//. and domain of a mailbox.
//.
//. In the tree (broken-down) representation of message, a <var>DwMailbox</var>
//. object may be only a leaf node, having a parent but no child nodes.
//. Its parent node must be a <var>DwField</var>, a <var>DwAddressList</var>,
//. or a <var>DwMailboxList</var> object.
//.
//. <var>DwMailbox</var> has member functions for getting or setting the
//. strings it contains.
//=============================================================================
// Last modified 1998-04-28
//+ Noentry ~DwMailbox
//+ Noentry mFullName mRoute mLocalPart mDomain sClassName _PrintDebugInfo


class DW_CORE_EXPORT DwMailbox : public DwAddress {

public:

    DwMailbox();
    DwMailbox(const DwMailbox& aMailbox);
    DwMailbox(const DwString& aStr, DwMessageComponent* aParent=0);
    //. The first constructor is the default constructor, which sets the
    //. <var>DwMailbox</var> object's string representation to the empty string
    //. and sets its parent to <var>NULL</var>.
    //.
    //. The second constructor is the copy constructor, which performs
    //. a deep copy of <var>aMailbox</var>.
    //. The parent of the new <var>DwMailbox</var> is set to <var>NULL</var>.
    //.
    //. The third constructor copies <var>aStr</var> to the
    //. <var>DwMailbox</var> object's string representation and sets
    //. <var>aParent</var> as its parent. The virtual member function
    //. <var>Parse()</var> should be called immediately after this
    //. constructor in order to parse the string representation. Unless
    //. it is <var>NULL</var>, <var>aParent</var> should point to an object
    //. of a class derived from <var>DwField</var>.

    virtual ~DwMailbox();

    const DwMailbox& operator = (const DwMailbox& aMailbox);
    //. This is the assignment operator, which performs a deep copy of
    //. <var>aMailbox</var>.  The parent node of the <var>DwMailbox</var>
    //. object is not changed.

    virtual void Parse();
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. executes the parse method for <var>DwMailbox</var> objects.  The parse
    //. method creates or updates the broken-down representation from the
    //. string representation.  For <var>DwMailbox</var> objects, the parse
    //. method parses the string representation into the substrings for
    //. the full name, local-part, route, and domain.
    //.
    //. You should call this member function after you set or modify the
    //. string representation, and before you retrieve the full name,
    //. local-part, route, or domain.
    //.
    //. This function clears the is-modified flag.

    virtual void Assemble();
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. executes the assemble method for <var>DwMailbox</var> objects. The
    //. assemble method creates or updates the string representation from
    //. the broken-down representation.  For <var>DwMailbox</var> objects, the
    //. assemble method builds the string representation from the full
    //. name, local-part, route, and domain strings.
    //.
    //. You should call this member function after you modify the full
    //. name, local-part, route, or domain, and before you retrieve the
    //. string representation.
    //.
    //. This function clears the is-modified flag.

    virtual DwMessageComponent* Clone() const;
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. creates a new <var>DwMailbox</var> on the free store that has the same
    //. value as this <var>DwMailbox</var> object.  The basic idea is that of
    //. a virtual copy constructor.

    const DwString& FullName() const;
    //. Returns the full name for this <var>DwMailbox</var> object.

    void SetFullName(const DwString& aFullName);
    //. Sets the full name for this <var>DwMailbox</var> object.

    void GetFullName(DwString& aFullName, DwString& aCharset) const;
    //. Gets the full name for this <var>DwMailbox</var> object.

    void SetFullName(const DwString& aFullName, const DwString& aCharset,
        int aEncoding='Q');
    //. Sets the full name for this <var>DwMailbox</var> object.

    const DwString& Route() const;
    //. Returns the route for this <var>DwMailbox</var> object.

    void SetRoute(const DwString& aRoute);
    //. Sets the route for this <var>DwMailbox</var> object.

    const DwString& LocalPart() const;
    //. Returns the local-part for this <var>DwMailbox</var> object.

    void SetLocalPart(const DwString& aLocalPart);
    //. Sets the local-part for this <var>DwMailbox</var> object.

    const DwString& Domain() const;
    //. Returns the domain for this <var>DwMailbox</var> object.

    void SetDomain(const DwString& aDomain);
    //. Sets the domain for this <var>DwMailbox</var> object.

    static DwMailbox* NewMailbox(const DwString& aStr, DwMessageComponent*
        aParent);
    //. Creates a new <var>DwMailbox</var> object on the free store.
    //. If the static data member <var>sNewMailbox</var> is <var>NULL</var>, 
    //. this member function will create a new <var>DwMailbox</var>
    //. and return it.  Otherwise, <var>NewMailbox()</var> will call
    //. the user-supplied function pointed to by <var>sNewMailbox</var>,
    //. which is assumed to return an object from a class derived from
    //. <var>DwMailbox</var>, and return that object.

    //+ Var sNewMailbox
    static DwMailbox* (*sNewMailbox)(const DwString&, DwMessageComponent*);
    //. If <var>sNewMailbox</var> is not <var>NULL</var>, it is assumed to
    //. point to a  user-supplied function that returns an object from a
    //. class derived from <var>DwMailbox</var>.

private:

    DwString mFullName;
    DwString mFullNameNonAscii;
    DwString mFullNameCharset;
    DwString mRoute;
    DwString mLocalPart;
    DwString mDomain;
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

    void _SetFullName(const DwString& aFullName);

};

#endif
