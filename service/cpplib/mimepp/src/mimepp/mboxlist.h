//=============================================================================
// File:       mboxlist.h
// Contents:   Declarations for DwMailboxList
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

#ifndef DW_MBOXLIST_H
#define DW_MBOXLIST_H

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
//+ Name DwMailboxList -- Class representing a list of RFC-822 mailboxes
//+ Description
//. <var>DwMailboxList</var> represents a list of <i>mailboxes</i> as described
//. in RFC-822.  In MIME++, <var>DwMailboxList</var> is a container for objects
//. of type <var>DwMailbox</var>, and it contains various member functions to
//. manage its contained objects.  <var>DwMailboxList</var> is also a
//. <var>DwFieldBody</var>.  This reflects the fact that certain RFC-822 header
//. fields, such as the "From" header field, have a list of mailboxes as
//. their field bodies.
//=============================================================================
// Last modified 1998-04-28
//+ Noentry ~DwMailboxList _PrintDebugInfo

class DW_CORE_EXPORT DwMailboxList : public DwFieldBody {

public:

    DwMailboxList();
    DwMailboxList(const DwMailboxList& aList);
    DwMailboxList(const DwString& aStr, DwMessageComponent* aParent=0);
    //. The first constructor is the default constructor, which sets the
    //. <var>DwMailboxList</var> object's string representation to the empty
    //. string and sets its parent to <var>NULL</var>.
    //.
    //. The second constructor is the copy constructor, which copies the
    //. string representation and all <var>DwMailbox</var> objects from
    //. <var>aList</var>. The parent of the new <var>DwMailboxList</var>
    //. object is set to <var>NULL</var>.
    //.
    //. The third constructor copies <var>aStr</var> to the
    //. <var>DwMailboxList</var> object's string representation and sets
    //. <var>aParent</var> as its parent. The virtual member function
    //. <var>Parse()</var> should be called immediately after this constructor
    //. in order to parse the string representation. Unless it is
    //. <var>NULL</var>, <var>aParent</var> should point to an object of a
    //. class derived from <var>DwField</var>.

    virtual ~DwMailboxList();

    const DwMailboxList& operator = (const DwMailboxList& aList);
    //. This is the assignment operator, which performs a deep copy of
    //. <var>aList</var>.  The parent node of the <var>DwMailboxList</var>
    //. object is not changed.

    virtual void Parse();
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. executes the parse method for <var>DwMailboxList</var> objects. The
    //. parse method creates or updates the broken-down representation from the
    //. string representation.  For <var>DwMailboxList</var> objects, the parse
    //. method parses the string representation to create a list of
    //. <var>DwMailbox</var> objects.  This member function also calls the
    //. <var>Parse()</var> member function of each <var>DwMailbox</var> object
    //. in its list.
    //.
    //. You should call this member function after you set or modify the
    //. string representation, and before you access any of the contained
    //. <var>DwMailbox</var> objects.
    //.
    //. This function clears the is-modified flag.

    virtual void Assemble();
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. executes the assemble method for <var>DwMailboxList</var> objects. The
    //. assemble method creates or updates the string representation from
    //. the broken-down representation.  For <var>DwMailboxList</var> objects,
    //. the assemble method builds the string representation from its list
    //. of <var>DwMailbox</var> objects. Before it builds the string
    //. representation for the <var>DwMailboxList</var> object, this function
    //. first calls the <var>Assemble()</var> member function of each
    //. <var>DwMailbox</var> object in its list.
    //.
    //. You should call this member function after you set or modify any
    //. of the contained <var>DwMailbox</var> objects, and before you retrieve
    //. the string representation.
    //.
    //. This function clears the is-modified flag.

    virtual DwMessageComponent* Clone() const;
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. creates a new <var>DwMailboxList</var> on the free store that has the
    //. same value as this <var>DwMailboxList</var> object.  The basic idea
    //. is that of a virtual copy constructor.

    int NumMailboxes() const;
    //. Returns the number of <var>DwMailbox</var> objects contained in this
    //. <var>DwMailboxList</var> object.

    void AddMailbox(DwMailbox* aMailbox);
    //. Appends a <var>DwMailbox</var> object to the list managed by
    //. this <var>DwMailboxList</var> object.
    //. 
    //. Any <var>DwMailbox</var> objects contained in a
    //. <var>DwMailboxList</var> object's list will be deleted by the
    //. <var>DwMailboxList</var> object's destructor.

    void DeleteAllMailboxes();
    //. Removes and deletes all <var>DwMailbox</var> objects contained in
    //. this <var>DwMailboxList</var> object.

    DwMailbox& MailboxAt(int aIndex) const;
    //. Returns the <var>DwMailbox</var> object at position <var>aIndex</var>
    //. in this <var>DwMailboxList</var> object's list.
    //.
    //. If the library is compiled to throw exceptions, the function will
    //. throw a <var>DwBoundsException</var> if <var>aIndex</var> is out of
    //. range. Otherwise, (if the library is compiled <i>not</i> to throw
    //. exceptions) the behavior is undefined if <var>aIndex</var> is out of
    //. range.  Valid values for <var>aIndex</var> are 0 &lt;=
    //. <var>aIndex</var> &lt; <var>NumMailboxes()</var>.

    void InsertMailboxAt(int aIndex, DwMailbox* aMailbox);
    //. Inserts <var>aMailbox</var> into the list of <var>DwMailbox</var>
    //. objects at the position given by <var>aIndex</var>.  A value of zero
    //. for <var>aIndex</var> will insert <var>aMailbox</var> as the first
    //. one in the list; a value of <var>NumMailboxes()</var> will insert it
    //. as the last one in the list.
    //. If <var>aIndex</var> is less than <var>NumMailboxes()</var>, the
    //. <var>DwMailbox</var> objects at position <var>aIndex</var> or greater
    //. will be shifted down the list to make room to insert
    //. <var>aMailbox</var>.
    //. 
    //. Any <var>DwMailbox</var> objects contained in a
    //. <var>DwMailboxList</var> object's list will be deleted by the
    //. <var>DwMailboxList</var> object's destructor.
    //.
    //. If the library is compiled to throw exceptions, the function will
    //. throw a <var>DwBoundsException</var> if <var>aIndex</var> is out of
    //. range. Otherwise, (if the library is compiled <i>not</i> to throw
    //. exceptions) the behavior is undefined if <var>aIndex</var> is out of
    //. range.  Valid values for <var>aIndex</var> are 0 &lt;=
    //. <var>aIndex</var> &lt;= <var>NumMailboxes()</var>.

    DwMailbox* RemoveMailboxAt(int aIndex);
    //. Removes the <var>DwMailbox</var> object at position <var>aIndex</var>
    //. from the list and returns it.
    //. If <var>aIndex</var> is less than <var>NumMailboxes()-1</var>, then any
    //. <var>DwMailbox</var> objects at a position greater than
    //. <var>aIndex</var> will be shifted up in the list after the requested
    //. <var>DwMailbox</var> is removed.
    //.
    //. If the library is compiled to throw exceptions, the function will
    //. throw a <var>DwBoundsException</var> if <var>aIndex</var> is out of
    //. range. Otherwise, (if the library is compiled <i>not</i> to throw
    //. exceptions) the behavior is undefined if <var>aIndex</var> is out of
    //. range.  Valid values for <var>aIndex</var> are 0 &lt;=
    //. <var>aIndex</var> &lt; <var>NumMailboxes()</var>.

    static DwMailboxList* NewMailboxList(const DwString& aStr,
        DwMessageComponent* aParent);
    //. Creates a new <var>DwMailboxList</var> object on the free store.
    //. If the static data member <var>sNewMailboxList</var> is
    //. <var>NULL</var>,  this member function will create a new
    //. <var>DwMailboxList</var> and return it.  Otherwise,
    //. <var>NewMailboxList()</var> will call the user-supplied function
    //. pointed to by <var>sNewMailboxList</var>, which is assumed to return
    //. an object from a class derived from <var>DwMailboxList</var>, and
    //. return that object.

    //+ Var sNewMailboxList
    static DwMailboxList* (*sNewMailboxList)(const DwString&,
        DwMessageComponent*);
    //. If <var>sNewMailboxList</var> is not <var>NULL</var>, it is assumed
    //. to point to a user-supplied function that returns an object from a
    //. class derived from <var>DwMailboxList</var>.

protected:

    void _CopyMailboxes(const DwMailboxList& aList);
    //. Copies the list of <var>DwMailbox</var> objects from <var>aList</var>.

    void _AddMailbox(DwMailbox* aMailbox);
    //. Same as <var>AddMailbox()</var>, but does not set is-modified flag.

    void _InsertMailboxAt(int aIndex, DwMailbox* aMbox);
    //. Performs all the work of adding <var>DwMailbox</var> objects to the
    //. list.  This function does not set the is-modified flag.

    void _DeleteAllMailboxes();
    //. Deletes all mailboxes.  Differs from <var>DeleteMailboxes()</var>
    //. in that it does not set the is-modified flag.

private:

    static const char* const sClassName;

    int         mNumMailboxes;
    DwMailbox** mMailboxes;
    int         mMailboxesSize;

    void DwMailboxList_Init();

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


class DW_CORE_EXPORT DwMailboxListParser {
public:
    enum {
        kMboxError,
        kMboxGroup,
        kMboxMailbox,
        kMboxNull,
        kMboxEnd
    };
    DwMailboxListParser(const DwString& aStr);
    virtual ~DwMailboxListParser();
    const DwString& MboxString();
    int MboxType();
    DwBool IsNull();
    DwBool IsEnd();
    int Restart();
    int operator ++ (); // prefix increment operator
protected:
    void ParseNextMailbox();
    DwRfc822Tokenizer mTokenizer;
    DwTokenString mMboxString;
    int mMboxType;
};

#endif
