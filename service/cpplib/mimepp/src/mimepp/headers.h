//=============================================================================
// File:       headers.h
// Contents:   Declarations for DwHeaders
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

#ifndef DW_HEADERS_H
#define DW_HEADERS_H

#ifndef DW_CONFIG_H
#include <mimepp/config.h>
#endif

#ifndef DW_STRING_H
#include <mimepp/string.h>
#endif

#ifndef DW_BOUNDS_H
#include <mimepp/bounds.h>
#endif

#ifndef DW_MSGCMP_H
#include <mimepp/msgcmp.h>
#endif

#ifndef DW_ENTITY_H
#include <mimepp/entity.h>
#endif

#ifndef DW_MSGID_H
#include <mimepp/msgid.h>
#endif

#ifndef DW_MAILBOX_H
#include <mimepp/mailbox.h>
#endif

#ifndef DW_MEDIATYP_H
#include <mimepp/mediatyp.h>
#endif

#ifndef DW_DATETIME_H
#include <mimepp/datetime.h>
#endif

#ifndef DW_MECHANSM_H
#include <mimepp/mechansm.h>
#endif

#ifndef DW_DISPTYPE_H
#include <mimepp/disptype.h>
#endif

#ifndef DW_STAMP_H
#include <mimepp/stamp.h>
#endif

class DwMessage;
class DwBodyPart;
class DwField;
class DwFieldBody;
class DwDateTime;
class DwMailboxList;
class DwAddressList;
class DwMediaType;
class DwMechanism;
class DwText;

//=============================================================================
//+ Name DwHeaders -- Class representing the collection of header fields in a message or body part
//+ Description
//. <var>DwHeaders</var> represents the collection of <i>header fields</i>
//. (often called just <i>headers</i>) in an <i>entity</i> (either a message
//. or body part), as described in RFC-822 and RFC-2045.  A <var>DwHeaders
//. </var> object manages a list of <var>DwField</var> objects, which represent
//. the individual header fields.
//.
//. In the tree (broken-down) representation of a message, a <var>DwHeaders
//. </var> object is an intermediate node, having both a parent node and
//. several child nodes.  The parent node is the <var>DwEntity</var> object
//. that contains it.  The child nodes are the <var>DwField</var> objects in
//. the list it manages. (See the man page for <var>DwMessageComponent</var>
//. for a discussion of the tree representation of a message.)
//.
//. Normally, you do not create a <var>DwHeaders</var> object directly, but you
//. access it through the <var>Headers()</var> member function of <var>
//. DwEntity</var>, which creates the <var>DwHeaders</var> object for you.
//.
//. While <var>DwHeaders</var> has public member functions for managing the
//. list of <var>DwField</var> objects it contains, you will normally use
//. convenience functions to access the field bodies of the header fields
//. directly. You can access the field body for a specific well-known header
//. field by using the member function <var>&lt;Field&gt;()</var>, where
//. <var>&lt;Field&gt;</var> is the field name of the header field with
//. hyphens removed and the first word following a hyphen capitalized.
//. For example, to access the field body for the "MIME-version" header
//. field, use <var>MimeVersion()</var>. The member function <var>
//. &lt;Field&gt;()</var> will create a header field with field name
//. <var>&lt;Field&gt;</var> if such a header field does not already exist.
//. You can check for the existence of a particular well-known header field
//. by using the member function <var>Has&lt;Field&gt;()</var>.  For example,
//. to check for the existence of the MIME-version header field, use
//. <var>HasMimeVersion()</var>.  Well-known header fields are those
//. documented in RFC-822 (standard email), RFC-1036 (USENET messages),
//. RFC-2045 (MIME messages), and possibly other RFCs.
//.
//. In the case of an extension field or user-defined field, you can access
//. the field body of the header field by calling the member function
//. <var>FieldBody()</var> with the field name as its argument.  If the
//. extension field or user-defined field does not exist, <var>FieldBody()
//. </var> will create it.  You can check for the existence of an extension
//. field or user-defined field by using the member function <var>HasField()
//. </var> with the field name as its argument.
//.
//. <var>DwHeaders</var> has several other member functions provided for the
//. sake of completeness that are not required for most applications.
//. These functions are documented below.
//=============================================================================
// Last modified 1998-04-28
//+ Noentry ~DwHeaders sClassName


class DW_CORE_EXPORT DwHeaders : public DwMessageComponent {

public:

    DwHeaders();
    DwHeaders(const DwHeaders& aHeaders);
    DwHeaders(const DwString& aStr, DwMessageComponent* aParent=0);
    //. The first constructor is the default constructor, which sets the
    //. <var>DwHeaders</var> object's string representation to the empty
    //. string and sets its parent to <var>NULL</var>.
    //.
    //. The second constructor is the copy constructor, which performs a
    //. deep copy of <var>aHeaders</var>.
    //. The parent of the new <var>DwHeaders</var> object is set to
    //. <var>NULL</var>.
    //.
    //. The third constructor copies <var>aStr</var> to the <var>DwHeaders
    //. </var> object's string representation and sets <var>aParent</var>
    //. as its parent. The virtual member function <var>Parse()</var>
    //. should be called immediately after this constructor in order to
    //. parse the string representation. Unless it is <var>NULL</var>,
    //. <var>aParent</var> should point to an object of a class derived
    //. from <var>DwEntity</var>.

    virtual ~DwHeaders();

    const DwHeaders& operator = (const DwHeaders& aHeaders);
    //. This is the assignment operator, which performs a deep copy of
    //. <var>aHeaders</var>.  The parent node of the <var>DwHeaders</var>
    //. object is not changed.

    virtual void Parse();
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. executes the parse method for <var>DwHeaders</var> objects.  The parse
    //. method creates or updates the broken-down representation from the
    //. string representation.  For <var>DwHeaders</var> objects,
    //. <var>DwHeaders::Parse()</var> parses the string representation
    //. to create a list of <var>DwField</var> objects.  This member function
    //. also calls the <var>Parse()</var> member function of each
    //. <var>DwField</var> object in its list.
    //.
    //. You should call this member function after you set or modify the
    //. string representation, and before you access any of the header fields.
    //.
    //. This function clears the is-modified flag.

    virtual void Assemble();
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. executes the assemble method for <var>DwHeaders</var> objects.  The
    //. assemble method creates or updates the string representation from
    //. the broken-down representation.  That is, the assemble method
    //. builds the string representation from its list of <var>DwField</var>
    //. objects.  Before it builds the string representation, this function
    //. first calls the <var>Assemble()</var> member function of each
    //. <var>DwField</var> object in its list.
    //.
    //. You should call this member function after you set or modify any
    //. of the header fields, and before you retrieve the string
    //. representation.
    //.
    //. This function clears the is-modified flag.

    virtual DwMessageComponent* Clone() const;
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. creates a new <var>DwHeaders</var> on the free store that has the same
    //. value as this <var>DwHeaders</var> object.  The basic idea is that of
    //. a virtual copy constructor.

    // RFC-822 fields
    DwBool HasBcc() const;
    DwBool HasCc() const;
    DwBool HasComments() const;
    DwBool HasDate() const;
    DwBool HasEncrypted() const;
    DwBool HasFrom() const;
    DwBool HasInReplyTo() const;
    DwBool HasKeywords() const;
    DwBool HasMessageId() const;
    DwBool HasReceived() const;
    DwBool HasReferences() const;
    DwBool HasReplyTo() const;
    DwBool HasResentBcc() const;
    DwBool HasResentCc() const;
    DwBool HasResentDate() const;
    DwBool HasResentFrom() const;
    DwBool HasResentMessageId() const;
    DwBool HasResentReplyTo() const;
    DwBool HasResentSender() const;
    DwBool HasResentTo() const;
    DwBool HasReturnPath() const;
    DwBool HasSender() const;
    DwBool HasSubject() const;
    DwBool HasTo() const;
    // RFC-1036 fields
    DwBool HasApproved() const;
    DwBool HasControl() const;
    DwBool HasDistribution() const;
    DwBool HasExpires() const;
    DwBool HasFollowupTo() const;
    DwBool HasLines() const;
    DwBool HasNewsgroups() const;
    DwBool HasOrganization() const;
    DwBool HasPath() const;
    DwBool HasSummary() const;
    DwBool HasXref() const;
    // RFC-2045 fields
    DwBool HasContentDescription() const;
    DwBool HasContentId() const;
    DwBool HasContentTransferEncoding() const;
    DwBool HasCte() const;
    DwBool HasContentType() const;
    DwBool HasMimeVersion() const;
    // RFC-1806
    DwBool HasContentDisposition() const;

	DwBool HasXMailer() const;
    //. Each member function in this group returns a boolean value indicating
    //. whether a particular well-known header field is present in this
    //. object's collection of header fields.

    DwBool HasField(const char* aFieldName) const;
    DwBool HasField(const DwString& aFieldName) const;
    //. Returns true if the header field specified by <var>aFieldName</var> is
    //. present in this object's collection of header fields.  These member
    //. functions are used for extension fields or user-defined fields.

    // RFC-822 fields
    DwAddressList&  Bcc() ;
    DwAddressList&  Cc() ;
    DwText&         Comments();
    DwDateTime&     Date();
    DwText&         Encrypted();
    DwMailboxList&  From();
    DwText&         InReplyTo();
    DwText&         Keywords();
    DwMsgId&        MessageId();
    DwStamp&        Received();
    DwText&         References();
    DwAddressList&  ReplyTo();
    DwAddressList&  ResentBcc();
    DwAddressList&  ResentCc();
    DwDateTime&     ResentDate();
    DwMailboxList&  ResentFrom();
    DwMsgId&        ResentMessageId();
    DwAddressList&  ResentReplyTo();
    DwMailbox&      ResentSender();
    DwAddressList&  ResentTo();
    DwAddress&      ReturnPath();
    DwMailbox&      Sender();
    DwText&         Subject();
    DwAddressList&  To() ;

	// add by pqf
	// RFC-1544 fields
	DwText& ContentMd5();

    // RFC-1036 fields (USENET messages)
    DwText& Approved();
    DwText& Control();
    DwText& Distribution();
    DwText& Expires();
    DwText& FollowupTo();
    DwText& Lines();
    DwText& Newsgroups();
    DwText& Organization();
    DwText& Path();
    DwText& Summary();
    DwText& Xref();
    // RFC-2045 fields
    DwText&         ContentDescription();
    DwMsgId&        ContentId();
    DwMechanism&    ContentTransferEncoding();
    DwMechanism&    Cte();
    DwMediaType&    ContentType();
    DwText&         MimeVersion();
    // RFC-1806 Content-Disposition field
    DwDispositionType& ContentDisposition();

	DwText& XMailer();
    //. Each member function in this group returns a reference to a
    //. <var>DwFieldBody</var> object for a particular header field.  If the
    //. header field does not already exist, it is created.  Use the
    //. corresponding <var>Has&lt;Field&gt;()</var> function to test if the
    //. header field already exists without creating it.

    DwFieldBody& FieldBody(const char* aFieldName);
    //. Returns a reference to the <var>DwFieldBody</var> object for a
    //. particular header field with field name <var>aFieldName</var>.
    //. If the header field does not already exist, it is created.  Use
    //. <var>HasField()</var> to test if the header field already exists
    //. without creating it. This member function allows access to
    //. extension fields or user-defined fields.

    int NumFields() const;
    //. Returns the number of <var>DwField</var> objects contained in this
    //. <var>DwHeaders</var> object.

    void AddField(DwField* aField);
    //. Appends a <var>DwField</var> object to the list managed by
    //. this <var>DwHeaders</var> object.
    //. 
    //. Any <var>DwField</var> objects contained in a <var>DwHeaders</var>
    //. object's list will be deleted by the <var>DwHeaders</var> object's
    //. destructor.

    void DeleteAllFields();
    //. Removes and deletes all <var>DwField</var> objects contained in
    //. this <var>DwHeaders</var> object.

    DwField& FieldAt(int aIndex) const;
    //. Returns the <var>DwField</var> object at position <var>aIndex</var> in
    //. this <var>DwHeaders</var> object's list.
    //.
    //. If the library is compiled to throw exceptions, the function will
    //. throw a <var>DwBoundsException</var> if <var>aIndex</var> is out
    //. of range. Otherwise, (if the library is compiled <i>not</i> to throw
    //. exceptions) the behavior is undefined if <var>aIndex</var> is out of
    //. range.  Valid values for <var>aIndex</var> are 0 &lt;=
    //. <var>aIndex</var> &lt; <var>NumFields()</var>.

    void InsertFieldAt(int aIndex, DwField* aField);
    //. Inserts <var>aField</var> into the list of <var>DwField</var> objects
    //. at the position given by <var>aIndex</var>.  A value of zero for
    //. <var>aIndex</var> will insert <var>aField</var> as the first one
    //. in the list; a value of <var>NumFields()</var> will insert it as
    //. the last one in the list.
    //. If <var>aIndex</var> is less than <var>NumFields()</var>, the
    //. <var>DwField</var> objects at position <var>aIndex</var> or greater
    //. will be shifted down the list to make room to insert <var>aField</var>.
    //. 
    //. Any <var>DwField</var> objects contained in a <var>DwHeaders</var>
    //. object's list will be deleted by the <var>DwHeaders</var> object's
    //. destructor.
    //.
    //. If the library is compiled to throw exceptions, the function will
    //. throw a <var>DwBoundsException</var> if <var>aIndex</var> is out
    //. of range. Otherwise, (if the library is compiled <i>not</i> to throw
    //. exceptions) the behavior is undefined if <var>aIndex</var> is out of
    //. range.  Valid values for <var>aIndex</var> are 0 &lt;=
    //. <var>aIndex</var> &lt;= <var>NumFields()</var>.

    DwField* RemoveFieldAt(int aIndex);
    //. Removes the <var>DwField</var> object at position <var>aIndex</var>
    //. from the list and returns it.
    //. If <var>aIndex</var> is less than <var>NumFields()-1</var>, then any
    //. <var>DwField</var> objects at a position greater than <var>aIndex</var>
    //. will be shifted up in the list after the requested <var>DwField</var>
    //. is removed.
    //.
    //. If the library is compiled to throw exceptions, the function will
    //. throw a <var>DwBoundsException</var> if <var>aIndex</var> is out of
    //. range. Otherwise, (if the library is compiled <i>not</i> to throw
    //. exceptions) the behavior is undefined if <var>aIndex</var> is out of
    //. range.  Valid values for <var>aIndex</var> are 0 &lt;=
    //. <var>aIndex</var> &lt; <var>NumFields()</var>.

    DwField* FindField(const char* aFieldName) const;
    DwField* FindField(const DwString& aFieldName) const;
    //. Searches for a header field by its field name.  Returns <var>NULL</var>
    //. if the field is not found.  This is an <i>advanced</i> function:
    //. most applications should use the <var>&lt;Field&gt;()</var> or
    //. <var>Has&lt;Field&gt;()</var> family of functions.

    static DwHeaders* NewHeaders(const DwString& aStr,
        DwMessageComponent* aParent);
    //. Creates a new <var>DwHeaders</var> object on the free store.
    //. If the static data member <var>sNewHeaders</var> is <var>NULL</var>, 
    //. this member function will create a new <var>DwHeaders</var>
    //. and return it.  Otherwise, <var>NewHeaders()</var> will call
    //. the user-supplied function pointed to by <var>sNewHeaders</var>,
    //. which is assumed to return an object from a class derived from
    //. <var>DwHeaders</var>, and return that object.

    //+ Var sNewHeaders
    static DwHeaders* (*sNewHeaders)(const DwString&, DwMessageComponent*);
    //. If <var>sNewHeaders</var> is not <var>NULL</var>, it is assumed
    //. to point to a user-supplied function that returns an object from
    //. a class derived from <var>DwHeaders</var>.

protected:

    void _CopyFields(const DwHeaders& aHeaders);
    //. Copies the list of <var>DwField</var> objects from <var>aHeaders</var>.

    void _AddField(DwField* aField);
    //. Same as <var>AddField()</var>, but does not set is-modified flag.

    void _InsertFieldAt(int aIndex, DwField* aField);
    //. Performs all the work of adding <var>DwField</var> objects to the
    //. list.  This function does not set the is-modified flag.

    void _DeleteAllFields();
    //. Deletes all fields.  Differs from <var>DeleteAllFields()</var>
    //. in that it does not set the is-modified flag.

private:

    static const char* const sClassName;

    int       mNumFields;
    DwField** mFields;
    int       mFieldsSize;

    void DwHeaders_Init();

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

private:

    void _PrintDebugInfo(DW_STD ostream& aStrm) const;

};

#endif

