//=============================================================================
// File:       mediatyp.h
// Contents:   Declarations for DwMediaType
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

#ifndef DW_MEDIATYP_H
#define DW_MEDIATYP_H

#ifndef DW_CONFIG_H
#include <mimepp/config.h>
#endif

#ifndef DW_STRING_H
#include <mimepp/string.h>
#endif

#ifndef DW_FIELDBDY_H
#include <mimepp/fieldbdy.h>
#endif

class DwParameter;

//=============================================================================
//+ Name DwMediaType -- Class representing a MIME media-type
//+ Description
//. <var>DwMediaType</var> represents a field body for the Content-Type header
//. field as described in RFC-2045. This field body specifies the kind of
//. data contained in the body of a message or a body part.  A media type
//. is described by two keywords: a primary type (or just <i>type</i>) and
//. a <i>subtype</i>.  RFC-2046 specifies the seven primary types text,
//. multipart, message, image, audio, video, and application.  RFC-2077
//. adds the new primary type model.
//.
//. <var>DwMediaType</var> has member functions that allow you to set or get
//. the type and subtype as either enumerated values or as strings.  It
//. also contains a list of <var>DwParameter</var> objects that represent the
//. parameters of the field body.  You can use convenience functions to
//. directly access the boundary parameter of a multipart media type, or
//. to access the name parameter that is often used with several media
//. types, such as application/octet-stream.
//.
//. Some MIME parsers have problems with folded header fields, and this
//. especially seems to be a problem with the Content-Type field.
//. To disable folding when the <var>DwMediaType</var> object is assembled,
//. call the inherited member function <var>DwFieldBody::SetFolding()</var>
//. with an argument of <var>DwFalse</var>.
//=============================================================================
// Last modified 1998-04-28
//+ Noentry ~DwMediaType TypeEnumToStr TypeStrToEnum SubtypeEnumToStr
//+ Noentry SubtypeStrToEnum mType mSubtype mTypeStr mSubtypeStr
//+ Noentry mBoundaryStr _PrintDebugInfo mNameStr


class DW_CORE_EXPORT DwMediaType : public DwFieldBody {

public:

    DwMediaType();
    DwMediaType(const DwMediaType& aMediaType);
    DwMediaType(const DwString& aStr, DwMessageComponent* aParent=0);
    //. The first constructor is the default constructor, which sets the
    //. <var>DwMediaType</var> object's string representation to the empty
    //. string and sets its parent to <var>NULL</var>.
    //.
    //. The second constructor is the copy constructor, which performs
    //. deep copy of <var>aMediaType</var>.
    //. The parent of the new <var>DwMediaType</var> object is set to
    //. <var>NULL</var>.
    //.
    //. The third constructor copies <var>aStr</var> to the
    //. <var>DwMediaType</var> object's string representation and sets
    //. <var>aParent</var> as its parent. The virtual member function
    //. <var>Parse()</var> should be called immediately after this
    //. constructor in order to parse the string representation. Unless it
    //. is <var>NULL</var>, <var>aParent</var> should point to an object of
    //. a class derived from <var>DwField</var>.

    virtual ~DwMediaType();

    const DwMediaType& operator = (const DwMediaType& aMediaType);
    //. This is the assignment operator, which performs a deep copy of
    //. <var>aMediaType</var>.  The parent node of the <var>DwMediaType</var>
    //. object is not changed.

    virtual void Parse();
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. executes the parse method for <var>DwMediaType</var> objects.
    //. It should be called immediately after the string representation
    //. is modified and before the parts of the broken-down
    //. representation are accessed.
    //.
    //. This function clears the is-modified flag.

    virtual void Assemble();
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. executes the assemble method for <var>DwMediaType</var> objects.
    //. It should be called whenever one of the object's attributes
    //. is changed in order to assemble the string representation from
    //. its broken-down representation.  It will be called
    //. automatically for this object by the parent object's
    //. <var>Assemble()</var> member function if the is-modified flag is set.
    //.
    //. This function clears the is-modified flag.

    virtual DwMessageComponent* Clone() const;
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. creates a new <var>DwMediaType</var> object on the free store that
    //. has the same value as this <var>DwMediaType</var> object.  The basic
    //. idea is that of a virtual copy constructor.

    int Type() const;
    //. Returns the primary type as an enumerated value.  Enumerated values
    //. are defined for all standard types in the file enum.h.  If the type
    //. is non-standard, <var>DwMime::kTypeUnknown</var> is returned.  The
    //. member function <var>TypeStr()</var> may be used to get the value of
    //. any type, standard or non-standard, as a string.

    void SetType(int aType);
    //. Sets the primary type from the enumerated value <var>aType</var>.
    //. Enumerated values are defined for all standard types in the file
    //. enum.h.  The member function <var>SetTypeStr()</var> may be used to
    //. set the value of any type, standard or non-standard, from a string.

    const DwString& TypeStr() const;
    //. Returns the primary type as a string.

    void SetTypeStr(const DwString& aStr);
    //. Sets the primary type from a string.

    int Subtype() const;
    //. Returns the subtype as an enumerated value.  Enumerated values
    //. are defined for all standard subtypes in the file enum.h.  If
    //. the subtype is non-standard, <var>DwMime::kSubtypeUnknown</var> is
    //. returned.  The member function <var>SubtypeStr()</var> may be used
    //. to get the value of any subtype, standard or non-standard, as
    //. a string.

    void SetSubtype(int aSubtype);
    //. Sets the subtype from the enumerated value <var>aSubtype</var>.
    //. Enumerated values are defined for all standard subtypes in the
    //. file enum.h.  The member function <var>SetSubtypeStr()</var> may be
    //. used to set the value of any subtype, standard or non-standard,
    //. from a string.

    const DwString& SubtypeStr() const;
    //. Returns the subtype as a string.

    void SetSubtypeStr(const DwString& aStr);
    //. Sets the subtype from a string.

    const DwString& Boundary() const;
    //. For the multipart type only, returns the value of the boundary
    //. parameter.  This member function is a convenience function
    //. that searches the list of <var>DwParameter</var> objects.

    void SetBoundary(const DwString& aStr);
    //. For the multipart type only, sets the value of the boundary
    //. parameter.
    //. This member function is a convenience function that accesses the
    //. list of <var>DwParameter</var> objects.

    virtual void CreateBoundary(unsigned aLevel=0);
    //. For the multipart type only, creates a boundary string.
    //. <var>aLevel</var> indicates the level of a nested multipart body
    //. part; if it is positive, it is used to form part of the created
    //. boundary string. This member function is a convenience function
    //. that accesses the list of child <var>DwParameter</var> objects.

    const DwString& Name() const;
    //. Returns the value of the "name" parameter, if such a parameter
    //. is present.  The name parameter is often found in several media
    //. types, including the application/octet-stream media type; it
    //. suggests a file name for saving to a disk file.  (The filename
    //. parameter in the Content-Disposition header field is an alternative
    //. way to indicate a file name.)  This member function is a convenience
    //. function that searches the list of <var>DwParameter</var> objects.

    void SetName(const DwString& aStr);
    //. Sets the value of the "name" parameter.  If a name parameter is
    //. not already present, it is added.  The name parameter is often
    //. found in several media types, including the application/octet-stream
    //. media type; it suggests a file name for saving to a disk file.
    //. (The filename parameter in the Content-Disposition header field
    //. is an alternative way to indicate a file name.)  This member
    //. function is a convenience function that accesses the list of
    //. <var>DwParameter</var> objects.

    int NumParameters() const;
    //. Returns the number of <var>DwParameter</var> objects contained in this
    //. <var>DwMediaType</var> object.

    void AddParameter(DwParameter* aParam);
    //. Appends a <var>DwParameter</var> object to the list managed by
    //. this <var>DwMediaType</var> object.
    //. 
    //. Any <var>DwParameter</var> objects contained in a
    //. <var>DwMediaType</var> object's list will be deleted by the
    //. <var>DwMediaType</var> object's destructor.

    void DeleteAllParameters();
    //. Removes and deletes all <var>DwParameter</var> objects contained in
    //. this <var>DwMediaType</var> object.

    DwParameter& ParameterAt(int aIndex) const;
    //. Returns the <var>DwParameter</var> object at position
    //. <var>aIndex</var> in this <var>DwMediaType</var> object's list.
    //.
    //. If the library is compiled to throw exceptions, the function will
    //. throw a <var>DwBoundsException</var> if <var>aIndex</var> is out of
    //. range. Otherwise, (if the library is compiled <i>not</i> to throw
    //. exceptions) the behavior is undefined if <var>aIndex</var> is out of
    //. range.  Valid values for <var>aIndex</var> are 0 &lt;=
    //. <var>aIndex</var> &lt; <var>NumParameters()</var>.

    void InsertParameterAt(int aIndex, DwParameter* aParam);
    //. Inserts <var>aParam</var> into the list of <var>DwParameter</var>
    //. objects at the position given by <var>aIndex</var>.  A value of zero
    //. for <var>aIndex</var> will insert <var>aParam</var> as the first one
    //. in the list; a value of <var>NumParameters()</var> will insert it as
    //. the last one in the list.
    //. If <var>aIndex</var> is less than <var>NumParameters()</var>, the
    //. <var>DwParameter</var> objects at position <var>aIndex</var> or greater
    //. will be shifted down the list to make room to insert <var>aParam</var>.
    //. 
    //. Any <var>DwParameter</var> objects contained in a
    //. <var>DwMediaType</var> object's list will be deleted by the
    //. <var>DwMediaType</var> object's destructor.
    //.
    //. If the library is compiled to throw exceptions, the function will
    //. throw a <var>DwBoundsException</var> if <var>aIndex</var> is out of
    //. range. Otherwise, (if the library is compiled <i>not</i> to throw
    //. exceptions) the behavior is undefined if <var>aIndex</var> is out of
    //. range.  Valid values for <var>aIndex</var> are 0 &lt;=
    //. <var>aIndex</var> &lt;= <var>NumParameters()</var>.

    DwParameter* RemoveParameterAt(int aIndex);
    //. Removes the <var>DwParameter</var> object at position <var>aIndex</var>
    //. from the list and returns it.
    //. If <var>aIndex</var> is less than <var>NumParameters()-1</var>, then
    //. any <var>DwParameter</var> objects at a position greater than
    //. <var>aIndex</var> will be shifted up in the list after the requested
    //. <var>DwParameter</var> is removed.
    //.
    //. If the library is compiled to throw exceptions, the function will
    //. throw a <var>DwBoundsException</var> if <var>aIndex</var> is out of
    //. range. Otherwise, (if the library is compiled <i>not</i> to throw
    //. exceptions) the behavior is undefined if <var>aIndex</var> is out of
    //. range.  Valid values for <var>aIndex</var> are 0 &lt;=
    //. <var>aIndex</var> &lt; <var>Numparameters()</var>.

    static DwMediaType* NewMediaType(const DwString& aStr,
        DwMessageComponent* aParent);
    //. Creates a new <var>DwMediaType</var> object on the free store.
    //. If the static data member <var>sNewMediaType</var> is <var>NULL</var>,
    //. this member function will create a new <var>DwMediaType</var>
    //. and return it.  Otherwise, <var>NewMediaType()</var> will call
    //. the user-supplied function pointed to by <var>sNewMediaType</var>,
    //. which is assumed to return an object from a class derived from
    //. <var>DwMediaType</var>, and return that object.

    //+ Var sNewMediaType
    static DwMediaType* (*sNewMediaType)(const DwString&,
        DwMessageComponent*);
    //. If <var>sNewMediaType</var> is not <var>NULL</var>, it is assumed to
    //. point to a  user-supplied function that returns an object from a
    //. class derived from <var>DwMediaType</var>.

protected:

    int mType;
    int mSubtype;
    DwString mTypeStr;
    DwString mSubtypeStr;
    DwString mBoundaryStr;
    DwString mNameStr;

    virtual void TypeEnumToStr();
    virtual void TypeStrToEnum();
    virtual void SubtypeEnumToStr();
    virtual void SubtypeStrToEnum();

    void _CopyParameters(const DwMediaType& aType);
    //. Copies the list of <var>DwParameter</var> objects from
    //. <var>aType</var>.

    void _AddParameter(DwParameter* aParameter);
    //. Same as <var>AddParameter()</var>, but does not set is-modified flag.

    void _InsertParameterAt(int aIndex, DwParameter* aParam);
    //. Performs all the work of adding <var>DwParameter</var> objects to the
    //. list.  This function does not set the is-modified flag.

    void _DeleteAllParameters();
    //. Deletes all parameters.  Differs from <var>DeleteAllParameters()</var>
    //. in that it does not set the is-modified flag.

private:

    static const char* const sClassName;

    int            mNumParameters;
    DwParameter**  mParameters;
    int            mParametersSize;

    void DwMediaType_Init();

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
