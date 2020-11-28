//=============================================================================
// File:       message.h
// Contents:   Declarations for DwMessage
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

#ifndef DW_MESSAGE_H
#define DW_MESSAGE_H

#ifndef DW_CONFIG_H
#include <mimepp/config.h>
#endif

#ifndef DW_ENTITY_H
#include <mimepp/entity.h>
#endif

//=============================================================================
//+ Name DwMessage -- Class representing an RFC-822/MIME message
//+ Description
//. <var>DwMessage</var> represents an RFC-822/MIME <i>message</i>.
//.
//. A <i>message</i> contains both a collection of <i>header fields</i> and
//. a <i>body</i>. In the terminology of RFC-2045, the general term for the
//. headers-body combination is <i>entity</i>. In MIME++, <var>DwMessage</var>
//. is a direct subclass of <var>DwEntity</var>, and therefore contains both
//. a <var>DwHeaders</var> object and a <var>DwBody</var> object.
//.
//. In the tree (broken-down) representation of message, a <var>DwMessage</var>
//. object is almost always a root node, having child nodes but no parent node.
//. The child nodes are the <var>DwHeaders</var> object and the
//. <var>DwBody</var> object it contains.  A <var>DwMessage</var> may
//. sometimes be an intermediate node.  In this special case, the parent node
//. is a <var>DwBody</var> object of type "message/*" and the
//. <var>DwMessage</var> object represents an encapsulated message.
//.
//. To access the contained <var>DwHeaders</var> object, use the inherited
//. member function <var>DwEntity::Headers()</var>.  To access the contained
//. <var>DwBody</var> object, use the inherited member function
//. <var>DwEntity::Body()</var>.
//=============================================================================
// Last modified 1998-04-28
//+ Noentry ~DwMessage _PrintDebugInfo

class DW_CORE_EXPORT DwMessage : public DwEntity {

public:
    
    DwMessage();
    DwMessage(const DwMessage& aMessage);
    DwMessage(const DwString& aStr, DwMessageComponent* aParent=0);
    //. The first constructor is the default constructor, which sets the
    //. <var>DwMessage</var> object's string representation to the empty string
    //. and sets its parent to <var>NULL</var>.
    //.
    //. The second constructor is the copy constructor, which performs
    //. a deep copy of <var>aMessage</var>.
    //. The parent of the new <var>DwMessage</var> object is set to
    //. <var>NULL</var>.
    //.
    //. The third constructor copies <var>aStr</var> to the
    //. <var>DwMessage</var> object's string representation and sets
    //. <var>aParent</var> as its parent.
    //. The virtual member function <var>Parse()</var> should be called
    //. immediately after this constructor in order to parse the string
    //. representation.

    virtual ~DwMessage();

    const DwMessage& operator = (const DwMessage& aMessage);
    //. This is the assignment operator, which performs a deep copy of
    //. <var>aMessage</var>.  The parent node of the <var>DwMessage</var>
    //. object is not changed.

    virtual DwMessageComponent* Clone() const;
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. creates a new <var>DwMessage</var> on the free store that has the same
    //. value as this <var>DwMessage</var> object.  The basic idea is that of
    //. a ``virtual copy constructor.''

    static DwMessage* NewMessage(const DwString& aStr,
        DwMessageComponent* aParent);
    //. Creates a new <var>DwMessage</var> object on the free store.
    //. If the static data member <var>sNewMessage</var> is <var>NULL</var>, 
    //. this member function will create a new <var>DwMessage</var>
    //. and return it.  Otherwise, <var>NewMessage()</var> will call
    //. the user-supplied function pointed to by <var>sNewMessage</var>,
    //. which is assumed to return an object from a class derived from
    //. <var>DwMessage</var>, and return that object.

    //+ Var sNewMessage
    static DwMessage* (*sNewMessage)(const DwString&, DwMessageComponent*);
    //. If <var>sNewMessage</var> is not <var>NULL</var>, it is assumed to
    //. point to a user supplied function that returns an object from a class
    //. derived from  <var>DwMessage</var>.

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

protected:

    void _PrintDebugInfo(DW_STD ostream& aStrm) const;

};

#endif
