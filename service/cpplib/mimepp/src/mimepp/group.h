//=============================================================================
// File:       group.h
// Contents:   Declarations for DwGroup
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

#ifndef DW_GROUP_H
#define DW_GROUP_H

#ifndef DW_CONFIG_H
#include <mimepp/config.h>
#endif

#ifndef DW_STRING_H
#include <mimepp/string.h>
#endif

#ifndef DW_MAILBOX_H
#include <mimepp/mailbox.h>
#endif

#ifndef DW_MBOXLIST_H
#include <mimepp/mboxlist.h>
#endif

#ifndef DW_ADDRESS_H
#include <mimepp/address.h>
#endif

//=============================================================================
//+ Name DwGroup -- Class representing an RFC-822 address group
//+ Description
//. <var>DwGroup</var> represents a <i>group</i> as described in RFC-822.
//. A group contains a group name and a (possibly empty) list of
//. <i>mailboxes</i>. In MIME++, a <var>DwGroup</var> object contains a
//. string for the group name and a <var>DwMailboxList</var> object for
//. the list of mailboxes.
//.
//. In the tree (broken-down) representation of message, a <var>DwGroup</var>
//. object may be only an intermediate node, having both a parent and a single
//. child node.  Its parent node must be a <var>DwField</var> or a
//. <var>DwAddressList</var>.  Its child is a <var>DwMailboxList</var>.
//=============================================================================
// Last modified 1998-04-28
//+ Noentry ~DwGroup _PrintDebugInfo


class DW_CORE_EXPORT DwGroup : public DwAddress {

public:

    DwGroup();
    DwGroup(const DwGroup& aGroup);
    DwGroup(const DwString& aStr, DwMessageComponent* aParent=0);
    //. The first constructor is the default constructor, which sets the
    //. <var>DwGroup</var> object's string representation to the empty string
    //. and sets its parent to <var>NULL</var>.
    //.
    //. The second constructor is the copy constructor, which performs
    //. a deep copy of <var>aGroup</var>.
    //. The parent of the new <var>DwGroup</var> object is set to
    //. <var>NULL</var>.
    //.
    //. The third constructor copies <var>aStr</var> to the <var>DwGroup</var>
    //. object's string representation and sets <var>aParent</var> as its
    //. parent. The virtual member function <var>Parse()</var> should be
    //. called immediately after this constructor in order to parse the string
    //. representation. Unless it is <var>NULL</var>, <var>aParent</var>
    //. should point to an object of a class derived from <var>DwField</var>
    //. or <var>DwAddressList</var>.

    virtual ~DwGroup();

    const DwGroup& operator = (const DwGroup& aGroup);
    //. This is the assignment operator, which performs a deep copy of
    //. <var>aGroup</var>.  The parent node of the <var>DwGroup</var> object
    //. is not changed.

    virtual void Parse();
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. executes the parse method for <var>DwGroup</var> objects. The parse
    //. method creates or updates the broken-down representation from the
    //. string representation.  For <var>DwGroup</var> objects, the parse
    //. method parses the string representation to extract the group name
    //. and to create a <var>DwMailboxList</var> object from the list of
    //. mailboxes. This member function also calls the <var>Parse()</var>
    //. member function of the <var>DwMailboxList</var> object it creates.
    //.
    //. You should call this member function after you set or modify the
    //. string representation, and before you access the group name or the
    //. mailbox list.
    //.
    //. This function clears the is-modified flag.

    virtual void Assemble();
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. executes the assemble method for <var>DwGroup</var> objects. The
    //. assemble method creates or updates the string representation from
    //. the broken-down representation.  That is, the assemble method
    //. builds the string representation from its group name and mailbox
    //. list.  Before it builds the string representation, this function
    //. calls the <var>Assemble()</var> member function of its contained
    //. <var>DwMailboxList</var> object.
    //.
    //. You should call this member function after you set or modify either
    //. the group name or the contained <var>DwMailboxList</var> object, and
    //. before you retrieve the string representation.
    //.
    //. This function clears the is-modified flag.

    virtual DwMessageComponent* Clone() const;
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. creates a new <var>DwGroup</var> on the free store that has the same
    //. value as this <var>DwGroup</var> object.  The basic idea is that of
    //. a virtual copy constructor.

    const DwString& GroupName() const;
    //. Returns the name of the group.

    const DwString& Phrase() const;
    //. Returns the name of the phrase part of a group as described in
    //. RFC-822.  The phrase is the same as the group name.

    void SetGroupName(const DwString& aName);
    //. Sets the name of the group.

    void SetPhrase(const DwString& aPhrase);
    //. Sets the name of the phrase part of a group as described in RFC-822.
    //. The phrase is the same as the group name.

    DwMailboxList& MailboxList() const;
    //. Provides access to the list of mailboxes that is part of a group as
    //. described in RFC-822.

    static DwGroup* NewGroup(const DwString& aStr,
        DwMessageComponent* aParent);
    //. Creates a new <var>DwGroup</var> object on the free store.
    //. If the static data member <var>sNewGroup</var> is <var>NULL</var>,
    //. this member function will create a new <var>DwGroup</var>
    //. and return it.  Otherwise, <var>NewGroup()</var> will call
    //. the user-supplied function pointed to by <var>sNewGroup</var>,
    //. which is assumed to return an object from a class derived from
    //. <var>DwGroup</var>, and return that object.

    //+ Var sNewGroup
    static DwGroup* (*sNewGroup)(const DwString&, DwMessageComponent*);
    //. If <var>sNewGroup</var> is not <var>NULL</var>, it is assumed to
    //. point to a  user-supplied function that returns an object from a
    //. class derived from  <var>DwGroup</var>.

protected:

    DwMailboxList* mMailboxList;
    //. Points to the <var>DwMailboxList</var> object.


private:

    DwString mGroupName;
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
