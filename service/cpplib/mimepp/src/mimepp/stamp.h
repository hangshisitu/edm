//=============================================================================
// File:       stamp.h
// Contents:   Declarations for DwStamp
// Maintainer: Doug Sauder <doug.sauder@hunnysoft.com>
// WWW:        http://www.hunnysoft.com/mimepp.htm
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

#ifndef DW_STAMP_H
#define DW_STAMP_H

#ifndef DW_CONFIG_H
#include <mimepp/config.h>
#endif

#ifndef DW_FIELDBDY_H
#include <mimepp/fieldbdy.h>
#endif

#ifndef DW_DATETIME_H
#include <mimepp/datetime.h>
#endif

//=============================================================================
//+ Name DwStamp -- Class representing the field-body of an RFC-822 "Received" field
//+ Description
//. <var>DwStamp</var> represents a field body for the "Received" header
//. field as described in RFC-821 and RFC-822.  (RFC-821 uses the term
//. <i>time stamp</i> to describe the contents of the "Received" header
//. field.)  One "Received" header field is normally added by every host
//. that relays a message through the transport system, as required by
//. SMTP.  The field includes a time stamp indicating the time that
//. the message was received at the host, and possibly other information.
//.
//. The "Received" header field provides information that may be helpful in
//. resolving email delivery problems.  In practice, though, the most recent
//. time stamp is often used by email client applications to obtain the
//. time that the message was received by the client's host.  The other
//. information provided in the "Received" header field is optional and
//. inconsistent between implementations.  Some implementations do not
//. provide much of the optional information, while other implementations
//. provide it in non-standard forms.
//.
//. The following is a sample time stamp line, which shows all the optional
//. information:
//.
//. <pre>
//.    Received: from xyz.net by mx.org via TELENET with SMTP
//. <i></i>   id M12345 for smith@aol.com ; Tue, 6 Oct 1998 09:23:59 -0700
//. </pre>
//.
//. <var>DwStamp</var> provides access to all the information provided in
//. a "Received" header field body, including sending host ("xyz.net"),
//. receiving host ("mx.org"), physical mechanism ("TELENET"), protocol
//. ("SMTP"), internal message identifier ("M12345"), original addressee
//. ("smith@aol.com"), and date/time the message was received ("Tue,
//. 6 Oct 1998 09:23:59 -0700").
//.
//. <var>DwStamp</var> follows the protocol for <var>Parse()</var> and
//. <var>Assemble()</var> as required by <var>DwMessageComponent</var>.
//=============================================================================
// Last modified 1998-10-06
//+ Noentry ~DwStamp mFrom mBy mVia mWith mId mFor mDate _PrintDebugInfo


class DW_CORE_EXPORT DwStamp : public DwFieldBody {

public:

    DwStamp();
    DwStamp(const DwStamp& aStamp);
    DwStamp(const DwString& aStr, DwMessageComponent* aParent=0);
    //. The first constructor is the default constructor, which sets the
    //. <var>DwStamp</var> object's string representation to the empty
    //. string and sets its parent to <var>NULL</var>.
    //.
    //. The second constructor is the copy constructor, which performs
    //. deep copy of <var>aStamp</var>.
    //. The parent of the new <var>DwStamp</var> object is set to
    //. <var>NULL</var>.
    //.
    //. The third constructor copies <var>aStr</var> to the
    //. <var>DwStamp</var> object's string representation and sets
    //. <var>aParent</var> as its parent. The virtual member function
    //. <var>Parse()</var> should be called immediately after this
    //. constructor in order to parse the string representation. Unless it
    //. is <var>NULL</var>, <var>aParent</var> should point to an object of
    //. a class derived from <var>DwField</var>.

    virtual ~DwStamp();

    const DwStamp& operator = (const DwStamp& aStamp);
    //. This is the assignment operator, which performs a deep copy of
    //. <var>aStamp</var>.  The parent node of the <var>DwStamp</var>
    //. object is not changed.

    virtual void Parse();
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. executes the parse method for <var>DwStamp</var> objects.
    //. It should be called immediately after the string representation
    //. is modified and before the parts of the broken-down
    //. representation are accessed.
    //.
    //. This function clears the is-modified flag.

    virtual void Assemble();
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. executes the assemble method for <var>DwStamp</var> objects.
    //. It should be called whenever one of the object's attributes
    //. is changed in order to assemble the string representation from
    //. its broken-down representation.  It will be called
    //. automatically for this object by the parent object's
    //. <var>Assemble()</var> member function if the is-modified flag is set.
    //.
    //. This function clears the is-modified flag.

    virtual DwMessageComponent* Clone() const;
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. creates a new <var>DwStamp</var> object on the free store that
    //. has the same value as this <var>DwStamp</var> object.  The basic
    //. idea is that of a virtual copy constructor.

    const DwString& From() const;
    //. Returns the sending host if it is present.
    //. If the the sending host is not present, it returns an empty string.

    void SetFrom(const DwString& aSendingHost);
    //.  Sets the sending host.

    const DwString& By() const;
    //. Returns the receiving host.  The receiving host is the host that
    //. added this field to the headers.
    //. If the receiving host is not preset, it returns an empty string.

    void SetBy(const DwString&);
    //. Sets the receiving host.

    const DwString& Via() const;
    //. Returns the physical mechanism through which the message was received.
    //. If the physical mechanism is not preset, it returns an empty string.

    void SetVia(const DwString&);
    //. Sets the physical mechanism.

    const DwString& With() const;
    //. Returns the protocol that was used to send this message (that is,
    //. the protocol by which the receiving host received it).  Usually, the
    //. protocol is SMTP.
    //. If the protocol is not preset, it returns an empty string.

    void SetWith(const DwString&);
    //. Sets the protocol.

    const DwString& Id() const;
    //. Returns the internal message identifier.  This is a value that is
    //. meaningful to the receiving host, such as the name of a file that
    //. was used to temporarily store the message.
    //. If the identifier is not preset, it returns an empty string.

    void SetId(const DwString&);
    //. Sets the internal message identifier.

    const DwString& For() const;
    //. Returns the original addressee.  This information may important if
    //. some kind of address translation occurs.
    //. If the original addressee is not preset, it returns an empty string.

    void SetFor(const DwString&);
    //. Sets the original addressee.

    const DwDateTime& Date() const;
    //. Returns the date/time that the message was received.  This
    //. information is the only required information in the "Received"
    //. header field.

    void SetDate(const DwDateTime& aDate);
    //. Sets the date/time that the message was received.

    static DwStamp* NewStamp(const DwString&, DwMessageComponent*);
    //. Creates a new <var>DwStamp</var> object on the free store.
    //. If the static data member <var>sNewStamp</var> is <var>NULL</var>,
    //. this member function will create a new <var>DwStamp</var>
    //. and return it.  Otherwise, <var>NewStamp()</var> will call
    //. the user-supplied function pointed to by <var>sNewStamp</var>,
    //. which is assumed to return an object from a class derived from
    //. <var>DwStamp</var>, and return that object.

    //+ Var sNewStamp
    static DwStamp* (*sNewStamp)(const DwString&, DwMessageComponent*);
    //. If <var>sNewStamp</var> is not <var>NULL</var>, it is assumed to
    //. point to a  user-supplied function that returns an object from a
    //. class derived from <var>DwStamp</var>.

protected:

    DwString   mFrom;
    DwString   mBy;
    DwString   mVia;
    DwString   mWith;
    DwString   mId;
    DwString   mFor;
    DwDateTime mDate;

private:

    static const char* const sClassName;

public:

    virtual void PrintDebugInfo(DW_STD ostream& aStrm, int aDepth=0) const;
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. prints debugging information about this object to <var>aStrm</var>.
    //.
    //. This member function is available only in the debug version of
    //. the library.

    virtual void CheckInvariants() const;
    //. Aborts if one of the invariants of the object fails. Use this
    //. member function to track down bugs.
    //.
    //. This member function is available only in the debug version of
    //. the library.

protected:

    void _PrintDebugInfo(DW_STD ostream& aStrm) const;

};

#endif
