//=============================================================================
// File:       disptype.h
// Contents:   Declarations for DwDispositionType
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

#ifndef DW_DISPTYPE_H
#define DW_DISPTYPE_H

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
//+ Name DwDispositionType -- Class representing a MIME content-disposition field body
//+ Description
//. <var>DwDispositionType</var> represents a field body for the 
//. Content-Disposition header field as described in RFC-1806.  This header
//. field specifies whether the content of a message or body part should
//. be displayed automatically to a user.  A disposition-type of inline
//. indicates that the content should be displayed; a disposition-type
//. of attachment indicates that it should not be.  RFC-1806 specifies
//. that a filename parameter may be optionally included in the field
//. body; the filename parameter suggests a file name for saving the
//. message or body part's content.
//.
//. <var>DwDispositionType</var> provides convenience functions that allow you
//. to set or get the disposition-type as an enumerated value, to set or
//. get the filename parameter, or to manage a list of parameters.
//.
//. RFC-1806 specifically states that the Content-Disposition header field
//. is experimental and not a proposed standard.
//=============================================================================
// Last modified 1998-04-28
//+ Noentry ~DwDispositionType EnumToStr StrToEnum mDispositionType
//+ Noentry mDispositionTypeStr mFilenameStr PrintDebugInfo
//+ Noentry _PrintDebugInfo CheckInvariants


class DW_CORE_EXPORT DwDispositionType : public DwFieldBody {

public:

    DwDispositionType();
    DwDispositionType(const DwDispositionType& aDispType);
    DwDispositionType(const DwString& aStr, DwMessageComponent* aParent=0);
    //. The first constructor is the default constructor, which sets the
    //. <var>DwDispositionType</var> object's string representation to the
    //. empty string and sets its parent to <var>NULL</var>.
    //.
    //. The second constructor is the copy constructor, which performs
    //. deep copy of <var>aDispType</var>.
    //. The parent of the new <var>DwDispositionType</var> object is set to
    //. <var>NULL</var>.
    //.
    //. The third constructor copies <var>aStr</var> to the
    //. <var>DwDispositionType</var> object's string representation and
    //. sets <var>aParent</var> as its parent. The virtual member function
    //. <var>Parse()</var> should be called immediately after this
    //. constructor in order to parse the string representation. Unless it
    //. is <var>NULL</var>, <var>aParent</var> should point to an object of
    //. a class derived from <var>DwField</var>.

    virtual ~DwDispositionType();

    const DwDispositionType& operator = (const DwDispositionType& aDispType);
    //. This is the assignment operator, which performs a deep copy of
    //. <var>aDispType</var>.  The parent node of the <var>DwDipositionType
    //. </var> object is not changed.

    virtual void Parse();
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. executes the parse method for <var>DwDispositionType</var> objects.
    //. It should be called immediately after the string representation
    //. is modified and before the parts of the broken-down
    //. representation are accessed.
    //.
    //. This function clears the is-modified flag.

    virtual void Assemble();
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. executes the assemble method for <var>DwDispositionType</var> objects.
    //. It should be called whenever one of the object's attributes
    //. is changed in order to assemble the string representation from
    //. its broken-down representation.  It will be called
    //. automatically for this object by the parent object's
    //. <var>Assemble()</var> member function if the is-modified flag is set.
    //.
    //. This function clears the is-modified flag.

    virtual DwMessageComponent* Clone() const;
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. creates a new <var>DwDispositionType</var> object on the free store
    //. that has the same value as this <var>DwDispositionType</var> object.
    //. The basic idea is that of a virtual copy constructor.

    int DispositionType() const;
    //. Returns the disposition-type as an enumerated value. Valid
    //. enumerated types, which are defined in enum.h, include
    //. <var>DwMime::kDispTypeNull</var>, <var>DwMime::kDispTypeUnknown</var>,
    //. <var>DwMime::kDispTypeInline</var>, and
    //. <var>DwMime::kDispTypeAttachment</var>.

    void SetDispositionType(int aType);
    //. Sets the disposition-type from the enumerated value <var>aType</var>.
    //. Valid enumerated types, which are defined in enum.h, include
    //. <var>DwMime::kDispTypeNull</var>, <var>DwMime::kDispTypeUnknown</var>,
    //. <var>DwMime::kDispTypeInline</var>, and
    //. <var>DwMime::kDispTypeAttachment</var>.

    const DwString& DispositionTypeStr() const;
    //. Returns the disposition-type as a string.

    void SetDispositionTypeStr(const DwString& aStr);
    //. Sets the disposition-type from a string.

    const DwString& Filename() const;
    //. This convenience function returns the value from the filename
    //. parameter, if present.  If no filename parameter is present,
    //. an empty string is returned.

    void SetFilename(const DwString& aStr);
    //. This convenience function sets the value of the filename parameter
    //. to <var>aStr</var>.

    int NumParameters() const;
    //. Returns the number of <var>DwParameter</var> objects contained in this
    //. <var>DwDispositionType</var> object.

    void AddParameter(DwParameter* aParam);
    //. Appends a <var>DwParameter</var> object to the list managed by
    //. this <var>DwDispositionType</var> object.
    //. 
    //. Any <var>DwParameter</var> objects contained in a
    //. <var>DwDispositionType</var> object's list will be deleted by the
    //. <var>DwDispositionType</var> object's destructor.

    void DeleteAllParameters();
    //. Removes and deletes all <var>DwParameter</var> objects contained in
    //. this <var>DwDispositionType</var> object.

    DwParameter& ParameterAt(int aIndex) const;
    //. Returns the <var>DwParameter</var> object at position <var>aIndex
    //. </var> in this <var>DwDispositionType</var> object's list.
    //.
    //. If the library is compiled to throw exceptions, the function will
    //. throw a <var>DwBoundsException</var> if <var>aIndex</var> is out
    //. of range. Otherwise, (if the library is compiled <i>not</i> to throw
    //. exceptions) the behavior is undefined if <var>aIndex</var> is out of
    //. range.  Valid values for <var>aIndex</var> are 0 &lt;=
    //. <var>aIndex</var> &lt; <var>NumParameters()</var>.

    void InsertParameterAt(int aIndex, DwParameter* aParam);
    //. Inserts <var>aParam</var> into the list of <var>DwParameter</var>
    //. objects at the position given by <var>aIndex</var>.  A value of
    //. zero for <var>aIndex</var> will insert <var>aParam</var> as the
    //. first one in the list; a value of <var>NumParameters()</var> will
    //. insert it as the last one in the list.
    //. If <var>aIndex</var> is less than <var>NumParameters()</var>, the
    //. <var>DwParameter</var> objects at position <var>aIndex</var> or greater
    //. will be shifted down the list to make room to insert <var>aParam</var>.
    //. 
    //. Any <var>DwParameter</var> objects contained in a
    //. <var>DwDispositionType</var> object's list will be deleted by the
    //. <var>DwDispositionType</var> object's destructor.
    //.
    //. If the library is compiled to throw exceptions, the function will
    //. throw a <var>DwBoundsException</var> if <var>aIndex</var> is out
    //. of range. Otherwise, (if the library is compiled <i>not</i> to throw
    //. exceptions) the behavior is undefined if <var>aIndex</var> is out of
    //. range.  Valid values for <var>aIndex</var> are 0 &lt;=
    //. <var>aIndex</var> &lt;= <var>NumParameters()</var>.

    DwParameter* RemoveParameterAt(int aIndex);
    //. Removes the <var>DwParameter</var> object at position <var>aIndex</var>
    //. from the list and returns it.
    //. If <var>aIndex</var> is less than <var>NumParameters()-1</var>,
    //. then any <var>DwParameter</var> objects at a position greater than
    //. <var>aIndex</var> will be shifted up in the list after the requested
    //. <var>DwParameter</var> is removed.
    //.
    //. If the library is compiled to throw exceptions, the function will
    //. throw a <var>DwBoundsException</var> if <var>aIndex</var> is out
    //. of range. Otherwise, (if the library is compiled <i>not</i> to throw
    //. exceptions) the behavior is undefined if <var>aIndex</var> is out of
    //. range.  Valid values for <var>aIndex</var> are 0 &lt;=
    //. <var>aIndex</var> &lt; <var>Numparameters()</var>.

    static DwDispositionType* NewDispositionType(const DwString& aStr,
        DwMessageComponent* aParent);
    //. Creates a new <var>DwDispositionType</var> object on the free store.
    //. If the static data member <var>sNewDispositionType</var> is
    //. <var>NULL</var>,  this member function will create a new
    //. <var>DwDispositionType</var> and return it.  Otherwise,
    //. <var>NewDispositionType()</var> will call the user-supplied function
    //. pointed to by <var>sNewDispositionType</var>, which is assumed to
    //. return an object from a class derived from <var>DwDispositionType
    //. </var>, and return that object.

    //+ Var sNewDispositionType
    static DwDispositionType* (*sNewDispositionType)(const DwString&,
        DwMessageComponent*);
    //. If <var>sNewDispositionType</var> is not <var>NULL</var>, it is
    //. assumed to point to a  user-supplied function that returns an 
    //.object from a class derived from <var>DwDispositionType</var>.

protected:

    int mDispositionType;
    DwString mDispositionTypeStr;
    DwString mFilenameStr;

    virtual void EnumToStr();
    virtual void StrToEnum();

    void _CopyParameters(const DwDispositionType& aType);
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

    void DwDispositionType_Init();

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
