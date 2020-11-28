//=============================================================================
// File:       fieldbdy.h
// Contents:   Declarations for DwFieldBody
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

#ifndef DW_FIELDBDY_H
#define DW_FIELDBDY_H

#ifndef DW_CONFIG_H
#include <mimepp/config.h>
#endif

#ifndef DW_STRING_H
#include <mimepp/string.h>
#endif

#ifndef DW_MSGCMP_H
#include <mimepp/msgcmp.h>
#endif

//=============================================================================
//+ Name DwFieldBody -- Class representing a MIME header field body
//+ Description
//. <var>DwFieldBody</var> represents the field-body element in the BNF grammar
//. specified by RFC-822.  It is an abstract base class that defines the
//. interface common to all structured field bodies.
//.
//. In the tree (broken-down) representation of a message, a
//. <var>DwFieldBody</var> object may be either a leaf node, having a
//. parent but no child nodes, or an intermediate node, having a parent
//. and one or more child nodes.  The parent node is the <var>DwField</var>
//. object that contains it.  Child nodes, if present, depend on the
//. particular subclass of <var>DwFieldBody</var> that is instantiated.
//. A <var>DwAddressList</var> object, for example, has <var>DwAddress</var>
//. objects as its child nodes.
//.
//. Since <var>DwFieldBody</var> is an abstract base class, you cannot create
//. instances of it directly.  Normally, objects of classes derived from
//. <var>DwFieldBody</var> are obtained by calling convenience member functions
//. in the class <var>DwHeaders</var>.
//.
//. Some MIME parsers are broken in that they do not handle the folding of
//. some fields properly.  <var>DwFieldBody</var> folds its string
//. representation by default.  You can disable folding, however, by calling
//. the <var>SetFolding()</var> member function.  To determine if folding is
//. enabled, call <var>IsFolding()</var>.
//=============================================================================
// Last modified 1998-04-28
//+ Noentry ~DwFieldBody mLineOffset mDoFolding _PrintDebugInfo


class DW_CORE_EXPORT DwFieldBody : public DwMessageComponent {

public:

    DwFieldBody();
    DwFieldBody(const DwFieldBody& aFieldBody);
    DwFieldBody(const DwString& aStr, DwMessageComponent* aParent=0);
    //. The first constructor is the default constructor, which sets the
    //. <var>DwFieldBody</var> object's string representation to the empty
    //. string and sets its parent to <var>NULL</var>.
    //.
    //. The second constructor is the copy constructor, which performs a
    //. deep copy of <var>aFieldBody</var>.
    //. The parent of the new <var>DwFieldBody</var> object is set to
    //. <var>NULL</var>.
    //.
    //. The third constructor copies <var>aStr</var> to the
    //. <var>DwFieldBody</var> object's string representation and sets
    //. <var>aParent</var> as its parent. The virtual member function
    //. <var>Parse()</var> should be called immediately after this
    //. constructor in order to parse the string representation. Unless
    //. it is <var>NULL</var>, <var>aParent</var> should point to an object
    //. of a class derived from <var>DwField</var>.

    virtual ~DwFieldBody();

    const DwFieldBody& operator = (const DwFieldBody& aFieldBody);
    //. This is the assignment operator, which performs a deep copy of
    //. <var>aFieldBody</var>.  The parent node of the <var>DwFieldBody</var>
    //. object is not changed.

    void SetOffset(int aOffset);
    //. Sets the offset to <var>aOffset</var>.  The offset is used when folding
    //. lines.  It indicates how much the first line should be offset to
    //. account for the field name, colon, and initial white space.

    void SetFolding(DwBool aTrueOrFalse);
    //. Enables (<var>aTrueOrFalse = DwTrue</var>) or disables
    //. (<var>aTrueOrFalse = DwFalse</var>) the folding of fields.  The default
    //. is to fold fields.  Unfortunately, some parsers are broke and
    //. do not handle folded lines properly.  This function allows a kludge
    //. to deal with these broken parsers.

    DwBool IsFolding() const;
    //. Returns a boolean indicating if folding of fields is enabled.

protected:

    int mLineOffset;
    DwBool mDoFolding;

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
