//=============================================================================
// File:       body.h
// Contents:   Declarations for DwBody
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

#ifndef DW_BODY_H
#define DW_BODY_H

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
class DwBodyPart;

//=============================================================================
//+ Name DwBody -- Class representing a MIME message body
//+ Description
//. <var>DwBody</var> represents a <i>body</i>, as described in RFC-2045.
//. A body is always part of an <i>entity</i>, which could be either a
//. <i>message</i> or a <i>body part</i>.  An entity has a collection of
//. <i>header fields</i> and a body.  If the content type of a body is
//. ``multipart,'' then the body contains one or more body parts.  If the
//. content type is ``message,'' then the body contains an encapsulated
//. message.  In all content types, the body contains a string of characters.
//.
//. In MIME++, a <var>DwBody</var> object is contained in a <var>DwEntity
//. </var> object. The <var>DwBody</var> object may contain a discrete
//. body consisting only of a string of characters, or it may be a composite
//. body, consisting of several contained <var>DwBodyPart</var> objects or
//. a single contained <var>DwMessage</var> object.  The only reliable way
//. to determine the type of <var>DwBody</var> is to access the Content-Type
//. header field from the <var>DwHeaders</var> object of the <var>DwEntity
//.</var> that contains it.  For this reason, a <var>DwBody</var> should
//. always be part of a <var>DwEntity</var>.
//.
//. In the tree (broken-down) representation of a message, a <var>DwBody</var>
//. object can be an intermediate node, having both a parent node and
//. one or more child nodes, or a leaf node, having a parent but no child
//. nodes.  In either case, the parent node is the <var>DwEntity</var> object
//. that contains it.  If it is an intermediate node, it must be of type
//. multipart with <var>DwBodyPart</var> objects as child nodes, or of type
//. message with a single <var>DwMessage</var> object as its child node.
//. (See the man page for <var>DwMessageComponent</var> for a discussion of
//. the tree representation of a message.)
//.
//. Normally, you do not create a <var>DwBody</var> object directly, but you
//. access it through the <var>Body()</var> member function of
//. <var>DwEntity</var>, which creates the <var>DwBody</var> object for you.
//.
//. To add a <var>DwBodyPart</var> to a multipart <var>DwBody</var>, call
//. the member function <var>AddBodyPart()</var> or <var>InsertBodyPartAt()
//. </var>. To get the number of body parts, call <var>NumBodyParts()</var>.
//. To access a particular body part, call <var>BodyPartAt()</var>.  To
//. remove a single body part without deleting it, call
//. <var>RemoveBodyPartAt()</var>.  To remove and delete all body parts,
//. call <var>DeleteAllBodyParts()</var>.
//.
//. To get the <var>DwMessage</var> contained in a <var>Body</var> with message
//. content type, call <var>Message()</var>.  To set the contained message,
//. call <var>SetMessage()</var>.
//=============================================================================
// Last modified 1998-04-28
//+ Noentry ~DwBody sClassName _PrintDebugInfo


class DW_CORE_EXPORT DwBody : public DwMessageComponent {

public:

    DwBody();
    DwBody(const DwBody& aBody);
    DwBody(const DwString& aStr, DwMessageComponent* aParent=0);
    //. The first constructor is the default constructor, which sets the
    //. <var>DwBody</var> object's string representation to the empty string
    //. and sets its parent to <var>NULL</var>.
    //.
    //. The second constructor is the copy constructor, which performs
    //. a deep copy of <var>aBody</var>.
    //. The parent of the new <var>DwBody</var> object is set to
    //. <var>NULL</var>.
    //.
    //. The third constructor copies <var>aStr</var> to the <var>DwBody</var>
    //. object's string representation and sets <var>aParent</var> as its
    //. parent. The virtual member function <var>Parse()</var> should be
    //. called immediately after this constructor in order to parse the
    //. string representation. Unless it is <var>NULL</var>, <var>aParent
    //. </var> should point to an object of a class derived from
    //. <var>DwEntity</var>.

    virtual ~DwBody();

    const DwBody& operator = (const DwBody& aBody);
    //. This is the assignment operator, which performs a deep copy of
    //. <var>aBody</var>.  The parent node of the <var>DwBody</var> object
    //. is not changed.

    virtual void Parse();
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. executes the parse method for <var>DwBody</var> objects.  The parse
    //. method creates or updates the broken-down representation from the
    //. string representation.  For a multipart <var>DwBody</var> object, the
    //. parse method creates a collection of <var>DwBodyPart</var> objects.
    //. For a message <var>DwBody</var>, the parse method creates a single
    //. <var>DwMessage</var> object.  For any other type of <var>DwBody</var>,
    //. the parse method does nothing.  This member function calls the
    //. <var>Parse()</var> member function of any objects it creates.
    //.
    //. Note: If the <var>DwBody</var> object has no parent node -- that is,
    //. it is not contained by a <var>DwEntity</var> object -- then the parse
    //. method does nothing, since it is unable to determine the type of
    //. body.
    //.
    //. You should call this member function after you set or modify the
    //. string representation, and before you access a contained
    //. <var>DwBodyPart</var> or <var>DwMessage</var>.
    //.
    //. This function clears the is-modified flag.

    virtual void Assemble();
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. executes the assemble method for <var>DwBody</var> objects.  The
    //. assemble method creates or updates the string representation
    //. from the broken-down representation.  Only <var>DwBody</var> objects
    //. with content type of multipart or message require assembling.
    //. In either case, the <var>DwBody</var> object must be able to find the
    //. headers of the message or body part that contains it.  Therefore,
    //. if the <var>DwBody</var> object is not the child of a
    //. <var>DwEntity</var> (<i>i.e.</i>, <var>DwMessage</var> or
    //. <var>DwBodyPart</var>) object, the <var>DwBody</var> cannot be
    //. assembled because the content type cannot be determined.
    //.
    //. This function calls the <var>Parse()</var> member function of any
    //. <var>DwBodyPart</var> or <var>DwMessage</var> object it contains.
    //.
    //. You should call this member function after you add a
    //. <var>DwBodyPart</var> object to a multipart body, or add a
    //. <var>DwMessage</var> object to a message body, and before you
    //. access the object's string representation.
    //.
    //. This function clears the is-modified flag.

    virtual DwMessageComponent* Clone() const;
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. creates a new <var>DwBody</var> on the free store that has the same
    //. value as this <var>DwBody</var> object.  The basic idea is that of
    //. a virtual copy constructor.

    int NumBodyParts() const;
    //. Returns the number of <var>DwBodyPart</var> objects contained in this
    //. <var>DwBody</var> object.

    void AddBodyPart(DwBodyPart* aPart);
    //. Appends a <var>DwBodyPart</var> object to the list managed by
    //. this <var>DwBody</var> object.
    //. 
    //. Any <var>DwBodyPart</var> objects contained in a <var>DwBody</var>
    //. object's list will be deleted by the <var>DwBody</var> object's
    //. destructor.

    void DeleteAllBodyParts();
    //. Removes and deletes all <var>DwBodyPart</var> objects contained in
    //. this <var>DwBody</var> object.

    DwBodyPart& BodyPartAt(int aIndex) const;
    //. Returns the <var>DwBodyPart</var> object at position <var>aIndex</var>
    //. in this <var>DwBody</var> object's list.
    //.
    //. If the library is compiled to throw exceptions, the function will
    //. throw a <var>DwBoundsException</var> if <var>aIndex</var> is out of
    //. range. Otherwise, (if the library is compiled <i>not</i> to throw
    //. exceptions) the behavior is undefined if <var>aIndex</var> is out of
    //. range.  Valid values for <var>aIndex</var> are 0 &lt;=
    //. <var>aIndex</var> &lt; <var>NumBodyParts()</var>.

    void InsertBodyPartAt(int aIndex, DwBodyPart* aPart);
    //. Inserts <var>aPart</var> into the list of <var>DwBodyPart</var>
    //. objects at the position given by <var>aIndex</var>.  A value of
    //. zero for <var>aIndex</var> will insert <var>aPart</var> as the
    //. first one in the list; a value of <var>NumBodyParts()</var> will
    //. insert it as the last one in the list.
    //. If <var>aIndex</var> is less than <var>NumBodyParts()</var>, the
    //. <var>DwBodyPart</var> objects at position <var>aIndex</var> or greater
    //. will be shifted down the list to make room to insert <var>aPart</var>.
    //. 
    //. Any <var>DwBodyPart</var> objects contained in a <var>DwBody</var>
    //. object's list will be deleted by the <var>DwBody</var> object's
    //. destructor.
    //.
    //. If the library is compiled to throw exceptions, the function will
    //. throw a <var>DwBoundsException</var> if <var>aIndex</var> is out of
    //. range. Otherwise, (if the library is compiled <i>not</i> to throw
    //. exceptions) the behavior is undefined if <var>aIndex</var> is out of
    //. range.  Valid values for <var>aIndex</var> are 0 &lt;=
    //. <var>aIndex</var> &lt;= <var>NumBodyParts()</var>.

    DwBodyPart* RemoveBodyPartAt(int aIndex);
    //. Removes the <var>DwBodyPart</var> object at position <var>aIndex</var>
    //. from the list and returns it.
    //. If <var>aIndex</var> is less than <var>NumBodyParts()-1</var>, then any
    //. <var>DwBodyPart</var> objects at a position greater than
    //. <var>aIndex</var> will be shifted up in the list after the requested
    //. <var>DwBodyPart</var> is removed.
    //.
    //. If the library is compiled to throw exceptions, the function will
    //. throw a <var>DwBoundsException</var> if <var>aIndex</var> is out
    //. of range. Otherwise, (if the library is compiled <i>not</i> to throw
    //. exceptions) the behavior is undefined if <var>aIndex</var> is out of
    //. range.  Valid values for <var>aIndex</var> are 0 &lt;=
    //. <var>aIndex</var> &lt; <var>NumBodyParts()</var>.

    DwMessage* Message() const;
    //. For a <var>DwBody</var> with content type of message, this member
    //. function returns the <var>DwMessage</var> encapsulated in it.

    void SetMessage(DwMessage* aMessage);
    //. For a <var>DwBody</var> with content type of message, this member
    //. function sets the <var>DwMessage</var> object it contains.
    //.
    //. The <var>DwMessage</var> object will be destroyed by the
    //. <var>DwBody</var> object's destructor.

    const DwString& Preamble() const;
    //. Returns the <i>preamble</i> for a multipart body. The preamble is
    //. the part of the body that precedes the first body part. It is not
    //. normally shown in a message viewer, and usually contains a message
    //. that's meaningful to users of non-MIME capable message viewers.

    void SetPreamble(const DwString& aStr);
    //. Sets the <i>preamble</i> for a multipart body. The preamble is
    //. the part of the body that precedes the first body part. It is not
    //. normally shown in a message viewer, and usually contains a message
    //. that's meaningful to users of non-MIME capable message viewers.

    const DwString& Epilogue() const;
    //. Returns the <i>epliogue</i> for a multipart body. The epilogue is
    //. the part of the body that follows the last body part. It is not
    //. normally shown in a message viewer, and is empty in most messages.

    void SetEpilogue(const DwString& aStr);
    //. Sets the <i>epliogue</i> for a multipart body. The epilogue is
    //. the part of the body that follows the last body part. It is not
    //. normally shown in a message viewer, and is empty in most messages.

    static DwBody* NewBody(const DwString& aStr, DwMessageComponent* aParent);
    //. Creates a new <var>DwBody</var> object on the free store.
    //. If the static data member <var>sNewBody</var> is <var>NULL</var>, 
    //. this member function will create a new <var>DwBody</var>
    //. and return it.  Otherwise, <var>NewBody()</var> will call
    //. the user-supplied function pointed to by <var>sNewBody</var>,
    //. which is assumed to return an object from a class derived from
    //. <var>DwBody</var>, and return that object.

    //+ Var sNewBody
    static DwBody* (*sNewBody)(const DwString&, DwMessageComponent*);
    //. If <var>sNewBody</var> is not <var>NULL</var>, it is assumed to point
    //. to a user-supplied function that returns an object from a class
    //. derived from <var>DwBody</var>.

	//. Add by pqf 2002.12
	//. 此变量用于指定在Parse调用中是否一直分解到邮件的最深层
	//. 此值为true，则一直分解邮件到最深层，此值为false则只分解到当前层
	//. 此值缺省为false，注意如果要修改此值，需要尽早修改（最好在main的第一行修改）
	//. 同时要注意多线程问题
	static bool mParseToEnd;

protected:

    void _CopyBodyParts(const DwBody& aBody);
    //. Copies the list of <var>DwBodyPart</var> objects from <var>aBody</var>.

    void _AddBodyPart(DwBodyPart* aPart);
    //. Same as <var>AddBodyPart()</var>, but does not set is-modified flag.

    void _InsertBodyPartAt(int aIndex, DwBodyPart* aPart);
    //. Performs all the work of adding <var>DwBodyPart</var> objects to the
    //. list.  This function does not set the is-modified flag.

    void _DeleteAllBodyParts();
    //. Deletes all body parts.  Differs from <var>DeleteAllBodyParts()</var>
    //. in that it does not set the is-modified flag.

    void _SetMessage(DwMessage* aMessage);
    //. Same as <var>SetMessage()</var>, but it does not set the is-modified
    //. flag.

private:

    DwString    mBoundaryStr;
    //. A cache for the boundary string, which is obtained from the
    //. headers associated with this body.

    DwString    mPreamble;
    //. Contains the preamble -- the text preceding the first boundary --
    //. in a ``multipart/*'' media type.

    DwString    mEpilogue;
    //. Contains the epilogue -- the text following the last boundary --
    //. in a ``multipart/*'' media type.

    int          mNumBodyParts;
    DwBodyPart** mBodyParts;
    int          mBodyPartsSize;

    DwMessage*  mMessage;
    //. Points to the contained message, in a ``message/*'' media type.

    static const char* const sClassName;

    void DwBody_Init();

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

