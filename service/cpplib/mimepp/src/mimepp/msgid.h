//=============================================================================
// File:       msgid.h
// Contents:   Declarations for DwMsgId
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

#ifndef DW_MSGID_H
#define DW_MSGID_H

#ifndef DW_CONFIG_H
#include <mimepp/config.h>
#endif

#ifndef DW_FIELDBDY_H
#include <mimepp/fieldbdy.h>
#endif

//=============================================================================
//+ Name DwMsgId -- Class representing an RFC-822 msg-id
//+ Description
//. <var>DwMsgId</var> represents a <i>msg-id</i> as described in RFC-822.  In
//. the BNF grammar in RFC-822, a msg-id has a <i>local-part</i> and a
//. <i>domain</i>.  In MIME++, a <var>DwMsgId</var> contains strings that
//. contain the local-part and the domain.
//.
//. In the tree (broken-down) representation of message, a <var>DwMsgId</var>
//. object may only be a leaf node, having a parent but no child nodes.
//. Its parent node must be a <var>DwField</var> object.
//.
//. <var>DwMsgId</var> has member functions for getting or setting its
//. local-part and its domain.  You can have the library to create the
//. contents of a <var>DwMsgId</var> object for you by calling the member
//. function <var>CreateDefault()</var>.
//=============================================================================
// Last modified 1998-04-28
//+ Noentry ~DwMsgId mLocalPart mDomain sClassName _PrintDebugInfo


class DW_CORE_EXPORT DwMsgId : public DwFieldBody {

public:

    DwMsgId();
    DwMsgId(const DwMsgId& aMsgId);
    DwMsgId(const DwString& aStr, DwMessageComponent* aParent=0);
    //. The first constructor is the default constructor, which sets the
    //. <var>DwMsgId</var> object's string representation to the empty string
    //. and sets its parent to NULL.
    //.
    //. The second constructor is the copy constructor, which performs
    //. a deep copy of <var>aMsgId</var>.
    //. The parent of the new <var>DwMsgId</var> object is set to NULL.
    //.
    //. The third constructor copies <var>aStr</var> to the <var>DwMsgId</var>
    //. object's string representation and sets <var>aParent</var> as its
    //. parent. The virtual member function <var>Parse()</var> should be
    //. called immediately after this constructor in order to parse the string
    //. representation. Unless it is NULL, <var>aParent</var> should point to
    //. an object of a class derived from <var>DwField</var>.

    virtual ~DwMsgId();

    const DwMsgId& operator = (const DwMsgId& aMsgId);
    //. This is the assignment operator, which performs a deep copy of
    //. <var>aMsgId</var>.  The parent node of the <var>DwMsgId</var> object
    //. is not changed.

    virtual void Parse();
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. executes the parse method for <var>DwMsgId</var> objects.  The parse
    //. method parses the local-part and the domain from the string
    //. representation.
    //.
    //. You should call this member function after you set or modify the
    //. string representation, and before you retrieve local-part or
    //. domain.
    //.
    //. This function clears the is-modified flag.

    virtual void Assemble();
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. executes the assemble method for <var>DwMsgId</var> objects.  The
    //. assemble method creates or updates the string representation
    //. from the local-part and the domain.
    //.
    //. You should call this member function after you modify the
    //. local-part or the domain, and before you retrieve the string
    //. representation.
    //.
    //. This function clears the is-modified flag.

    virtual DwMessageComponent* Clone() const;
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. creates a new <var>DwMsgId</var> on the free store that has the same
    //. value as this <var>DwMsgId</var> object.  The basic idea is that of
    //. a ``virtual copy constructor.''

    virtual void CreateDefault();
    //. Creates a value for the msg-id.  Uses the current time, process ID,
    //. and fully qualified domain name for the host.  <B>Important:</B>
    //. The resolution of the current time is only one second.  That means
    //. if two message IDs are created using this function in the same one
    //. second interval, they will probably not be unique.  While this may
    //. not be a problem for most mail user agent applications, it may be
    //. a problem for some server applications.  Because this function is
    //. virtual, it can be overridden in a derived class.

    const DwString& LocalPart() const;
    //. Returns the local-part of the msg-id.

    void SetLocalPart(const DwString& aLocalPart);
    //. Sets the local-part of the msg-id.

    const DwString& Domain() const;
    //. Returns the domain of the msg-id.

    void SetDomain(const DwString& aDomain);
    //. Sets the domain of the msg-id.

    static DwMsgId* NewMsgId(const DwString& aStr,
        DwMessageComponent* aParent);
    //. Creates a new <var>DwMsgId</var> object on the free store.
    //. If the static data member <var>sNewMsgId</var> is NULL, 
    //. this member function will create a new <var>DwMsgId</var>
    //. and return it.  Otherwise, <var>NewMsgId()</var> will call
    //. the user-supplied function pointed to by <var>sNewMsgId</var>,
    //. which is assumed to return an object from a class derived from
    //. <var>DwMsgId</var>, and return that object.

    //+ Var sNewMsgId
    static DwMsgId* (*sNewMsgId)(const DwString&, DwMessageComponent*);
    //. If <var>sNewMsgId</var> is not NULL, it is assumed to point to a 
    //. user-supplied function that returns an object from a class derived from
    //. <var>DwMsgId</var>.

    static const char* sHostName;
    //. Host name of machine, used to create msg-id string.  This data member
    //. is ignored if the platform supports a gethostname() function call.

private:

    DwString mLocalPart;
    DwString mDomain;
    DwInt32 sSerialNum;
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
