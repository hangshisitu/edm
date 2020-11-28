//=============================================================================
// File:       string.h
// Contents:   Declarations for DwString
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

#ifndef DW_STRING_H
#define DW_STRING_H

#ifndef DW_CONFIG_H
#include <mimepp/config.h>
#endif

#include <stddef.h>
#ifdef DW_USE_ANSI_IOSTREAM
#  include <iostream>
#else
#  include <iostream.h>
#endif

#if defined(DW_MULTITHREADED) && defined(DW_UNIX)
#include <pthread.h>
#endif

//=============================================================================
// DwStringRep is an implementation class that should not be used externally.
//=============================================================================

class DW_CORE_EXPORT DwStringRep {
public:
    DwStringRep();
    DwStringRep(char* aBuf, size_t aSize);
    ~DwStringRep();
    void CheckInvariants() const;
    size_t mSize;
    char* mBuffer;
    long mRefCount;
#if defined(DW_MULTITHREADED) && defined(DW_UNIX)
    pthread_mutex_t mMutex;
#endif
};


//=============================================================================
//+ Name DwString -- String class
//+ Description
//. <var>DwString</var> is the workhorse of the MIME++ library.  Creating,
//. parsing, or otherwise manipulating MIME messages is basically a matter of
//. manipulating strings.  <var>DwString</var> provides all the basic
//. functionality required of a string object, including copying, comparing,
//. concatenating, and so on.
//.
//. <var>DwString</var> is similar to the <var>string</var> class that is
//. part of  the proposed ANSI standard C++ library.  Some of the member
//. functions present in the ANSI <var>string</var> are not present in
//. <var>DwString</var>: mostly these are the functions that deal with
//. iterators.  <var>DwString</var> also includes some member functions
//. and class utility functions that are not a part of the ANSI
//. <var>string</var> class.  These non-ANSI functions are easy to
//. distinguish: they all begin with upper-case letters, and all ANSI
//. functions begin with lower-case letters.  The library classes themselves
//. use only the ANSI <var>string</var> functions. At some point in the
//. future, MIME++ will probably allow the option to substitute the ANSI
//. <var>string</var> class for <var>DwString</var>.
//.
//. <var>DwString</var> makes extensive use of copy-on-write, even when
//. extracting substrings.  It is this feature that distiguishes
//. <var>DwString</var> from most other string classes.  <var>DwString</var>
//. also handles binary data, which can contain embedded NUL characters.
//=============================================================================
// Last modified 1998-04-28
//+ Noentry ~DwString _copy _replace
//+ Noentry mRep mStart mLength


class DW_CORE_EXPORT DwString {

public:
	typedef	char	value_type;	// added by WELKIN to support template compile
    static const size_t npos;
	typedef char *	iterator;
	typedef const char *	const_iterator;

    //. <var>npos</var> is assigned the value (size_t)-1.

    DwString();
    DwString(const DwString& aStr, size_t aPos=0, size_t aLen=npos);
    DwString(const char* aBuf, size_t aLen);
    DwString(const char* aCstr);
    DwString(size_t aLen, char aChar);
    //. The first constructor is the default constructor, which sets the
    //. <var>DwString</var> object's contents to be empty.
    //.
    //. The second constructor is the copy constructor, which copies at most
    //. <var>aLen</var> characters beginning at position
    //. <var>aPos</var> from <var>aStr</var> to the new <var>DwString</var>
    //. object.  It will not copy more characters than what are available in
    //. <var>aStr</var>. <var>aPos</var> must be less than or equal to
    //. <var>aStr.size()</var>.
    //.
    //. The third constructor copies <var>aLen</var> characters from the buffer
    //. <var>aBuf</var> into the new <var>DwString</var> object.
    //. <var>aBuf</var> need not be NUL-terminated and may contain NUL
    //. characters.
    //.
    //. The fourth constructor copies the contents of the NUL-terminated string
    //. <var>aCstr</var> into the new <var>DwString</var> object.
    //.
    //. The fifth constructor sets the contents of the new <var>DwString</var>
    //. object to be the character <var>aChar</var> repeated <var>aLen</var>
    //. times.

    virtual ~DwString();

    DwString& operator = (const DwString& aStr);
    DwString& operator = (const char* aCstr);
    DwString& operator = (char aChar);
    //. Assigns the contents of the operand to this string.
    //. <var>aCstr</var> must point to a NUL-terminated array of characters
    //. (a C string).
    //. Returns <var>*this</var>.

    size_t size() const;
    //. Returns the number of characters in this string's contents.  This
    //. member function is identical to <var>length()</var>

    size_t length() const;
    //. Returns the number of characters in this string's contents.  This
    //. member function is identical to <var>size()</var>

    size_t max_size() const;
    //. Returns the maximum length that this string can ever attain.

    void resize(size_t aLen, char aChar);
    void resize(size_t aLen);
    //. Changes the length of this string. If the string is shortened, the
    //. final characters are truncated. If the string is expanded, the added
    //. characters will be NULs or the character specified by <var>aChar</var>.

    size_t capacity() const;
    //. Returns the size of the internal buffer used for this string, which
    //. will always be greater than or equal to the length of the string.

    void reserve(size_t aSize);
    //. If <var>aSize</var> is greater than the current capacity of this
    //. string, this member function will increase the capacity to be at least
    //. <var>aSize</var>.

    void clear();
    //. Sets this string's contents to be empty.

    DwBool empty() const;
    //. Returns a true value if and only if the contents of this string
    //. are empty.

    const char& operator [] (size_t aPos) const;
    char& operator [] (size_t aPos);
    //. Returns <var>DwString::at(aPos) const</var> or
    //. <var>DwString::at(aPos)</var>.
    //. Note that the non-const version always assumes that the contents
    //. will be modified and therefore always copies a shared internal
    //. buffer before it returns.

    const char& at(size_t aPos) const;
    char& at(size_t aPos);
    //. Returns the character at position <var>aPos</var> in the string's
    //. contents. The non-const version returns an lvalue that may be
    //. assigned to. Note that the non-const version always assumes that the
    //. contents will be modified and therefore always copies a shared
    //. internal buffer before it returns.

    DwString& operator += (const DwString& aStr);
    DwString& operator += (const char* aCstr);
    DwString& operator += (char aChar);
    //. Appends the contents of the operand to this string.
    //. <var>aCstr</var> must point to a NUL-terminated array of characters
    //. (a C string).
    //. Returns <var>*this</var>.

    DwString& append(const DwString& aStr);
    DwString& append(const DwString& aStr, size_t aPos, size_t aLen);
    DwString& append(const char* aBuf, size_t aLen);
    DwString& append(const char* aCstr);
    DwString& append(size_t aLen, char aChar);
    //. Appends characters to (the end of) this string.
    //. Returns <var>*this</var>.
    //.
    //. The first version appends all of the characters from <var>aStr</var>.
    //.
    //. The second version appends at most <var>aLen</var> characters from
    //. <var>aStr</var> beginning at position <var>aPos</var>.
    //. <var>aPos</var> must be less than or equal to <var>aStr.size()</var>.
    //. The function will not append more characters than what are available
    //. in <var>aStr</var>.
    //.
    //. The third version appends <var>aLen</var> characters from
    //. <var>aBuf</var>, which is not assumed to be NUL-terminated and can
    //. contain embedded NULs.
    //. 
    //. The fourth version appends characters from the NUL-terminated
    //. string <var>aCstr</var>.
    //.
    //. The fifth version appends <var>aChar</var> repeated <var>aLen</var>
    //. times.

    DwString& assign(const DwString& aStr);
    DwString& assign(const DwString& aStr, size_t aPos, size_t aLen);
    DwString& assign(const char* aBuf, size_t aLen);
    DwString& assign(const char* aCstr);
    DwString& assign(size_t aLen, char aChar);
    //. Assigns characters to this string.
    //. Returns <var>*this</var>.
    //.
    //. The first version assigns all of the characters from <var>aStr</var>.
    //.
    //. The second version assigns at most <var>aLen</var> characters from
    //. <var>aStr</var> beginning at position <var>aPos</var>.
    //. <var>aPos</var> must be less than or equal to <var>aStr.size()</var>.
    //. The function will not assign more characters than what are available
    //. in <var>aStr</var>.
    //.
    //. The third version assigns <var>aLen</var> characters from
    //. <var>aBuf</var>, which is not assumed to be NUL-terminated and can
    //. contain embedded NULs.
    //. 
    //. The fourth version assigns characters from the NUL-terminated
    //. string <var>aCstr</var>.
    //.
    //. The fifth version assigns <var>aChar</var> repeated <var>aLen</var>
    //. times.

    DwString& insert(size_t aPos1, const DwString& aStr);
    DwString& insert(size_t aPos1, const DwString& aStr, size_t aPos2,
        size_t aLen2);
    DwString& insert(size_t aPos1, const char* aBuf, size_t aLen2);
    DwString& insert(size_t aPos1, const char* aCstr);
    DwString& insert(size_t aPos1, size_t aLen2, char aChar);
    //. Inserts characters into this string beginning at position
    //. <var>aPos1</var>. Returns <var>*this</var>.
    //.
    //. The first version inserts all of the characters from <var>aStr</var>.
    //.
    //. The second version inserts at most <var>aLen2</var> characters from
    //. <var>aStr</var> beginning at position <var>aPos2</var>.
    //. <var>aPos2</var> must be less than or equal to <var>aStr.size()</var>.
    //. The function will not assign more characters than what are available
    //. in <var>aStr</var>.
    //.
    //. The third version inserts <var>aLen2</var> characters from
    //. <var>aBuf</var>, which is not assumed to be NUL-terminated and can
    //. contain embedded NULs.
    //. 
    //. The fourth version inserts characters from the NUL-terminated
    //. string <var>aCstr</var>.
    //.
    //. The fifth version inserts <var>aChar</var> repeated <var>aLen2</var>
    //. times.
  
    DwString& erase(size_t aPos=0, size_t aLen=npos);
    //. Erases (removes) at most <var>aLen</var> characters beginning at
    //. position <var>aPos</var> from this string. The function will not
    //. erase more characters than what are available.
    //. Returns <var>*this</var>.

    DwString& replace(size_t aPos1, size_t aLen1, const DwString& aStr);
    DwString& replace(size_t aPos1, size_t aLen1, const DwString& aStr,
        size_t aPos2, size_t aLen2);
    DwString& replace(size_t aPos1, size_t aLen1, const char* aBuf,
        size_t aLen2);
    DwString& replace(size_t aPos1, size_t aLen1, const char* aCstr);
    DwString& replace(size_t aPos1, size_t aLen1, size_t aLen2, char aChar);
    //. Removes <var>aLen1</var> characters beginning at position
    //. <var>aPos1</var> and inserts other characters.
    //. Returns <var>*this</var>.
    //.
    //. The first version inserts all of the characters from <var>aStr</var>.
    //.
    //. The second version inserts at most <var>aLen2</var> characters from
    //. <var>aStr</var> beginning at position <var>aPos2</var>.
    //. <var>aPos2</var> must be less than or equal to <var>aStr.size()</var>.
    //. The function will not assign more characters than what are available
    //. in <var>aStr</var>.
    //.
    //. The third version inserts <var>aLen2</var> characters from
    //. <var>aBuf</var>, which is not assumed to be NUL-terminated and can
    //. contain embedded NULs.
    //. 
    //. The fourth version inserts characters from the NUL-terminated
    //. string <var>aCstr</var>.
    //.
    //. The fifth version inserts <var>aChar</var> repeated <var>aLen2</var>
    //. times.

    size_t copy(char* aBuf, size_t aLen, size_t aPos=0) const;
    //. Copies at most <var>aLen</var> characters beginning at position
    //. <var>aPos</var> from this string to the buffer pointed to by
    //. <var>aBuf</var>. Returns the number of characters copied.

    void swap(DwString& aStr);
    //. Swaps the contents of this string and <var>aStr</var>.

    const char* c_str() const;
    const char* data() const;
    //. These member functions permit access to the internal buffer used
    //. by the <var>DwString</var> object.  <var>c_str()</var> returns a
    //. NUL-terminated string suitable for use in C library functions.
    //. <var>data()</var> returns a pointer to the internal buffer, which
    //. may not be NUL-terminated.
    //.
    //. <var>c_str()</var> may copy the internal buffer in order to place the
    //. terminating NUL.  This is not a violation of the const declaration:
    //. it is a logical const, not a bit-representation const.  It could
    //. have the side effect of invalidating a pointer previously returned
    //. by <var>c_str()</var> or <var>data()</var>.
    //.
    //. The characters in the returned string should not be modified, and
    //. should be considered invalid after any call to a non-const member
    //. function or another call to <var>c_str()</var>.

	// 加入支持STL的调用
	const char * begin(void) const;
	char * begin(void);
	const char * end(void) const;
	char * end(void);

    size_t find(const DwString& aStr, size_t aPos=0) const;
    size_t find(const char* aBuf, size_t aPos, size_t aLen) const;
    size_t find(const char* aCstr, size_t aPos=0) const;
    size_t find(char aChar, size_t aPos=0) const;
    //. Performs a forward search for a sequence of characters in the
    //. <var>DwString</var> object.  The return value is the position of the
    //. sequence in the string if found, or <var>DwString::npos</var> if not
    //. found.
    //.
    //. The first version searches beginning at position <var>aPos</var> for
    //. the sequence of characters in <var>aStr</var>.
    //.
    //. The second version searches beginning at position <var>aPos</var> for
    //. the sequence of <var>aLen</var> characters in <var>aBuf</var>, which
    //. need not be NUL-terminated and can contain embedded NULs.
    //.
    //. The third version searches beginning at position <var>aPos</var> for
    //. the sequence of characters in the NUL-terminated string
    //. <var>aCstr</var>.
    //.
    //. The fourth version searches beginning at position <var>aPos</var> for
    //. the character <var>aChar</var>.

    size_t rfind(const DwString& aStr, size_t aPos=npos) const;
    size_t rfind(const char* aBuf, size_t aPos, size_t aLen) const;
    size_t rfind(const char* aCstr, size_t aPos=npos) const;
    size_t rfind(char aChar, size_t aPos=npos) const;
    //. Performs a reverse search for a sequence of characters in the
    //. <var>DwString</var> object.  The return value is the position of the
    //. sequence in the string if found, or <var>DwString::npos</var> if not
    //. found.
    //.
    //. The first version searches beginning at position <var>aPos</var> for
    //. the sequence of characters in <var>aStr</var>.
    //.
    //. The second version searches beginning at position <var>aPos</var> for
    //. the sequence of <var>aLen</var> characters in <var>aBuf</var>, which
    //. need not be NUL-terminated and can contain embedded NULs.
    //.
    //. The third version searches beginning at position <var>aPos</var> for
    //. the sequence of characters in the NUL-terminated string
    //. <var>aCstr</var>.
    //.
    //. The fourth version searches beginning at position <var>aPos</var> for
    //. the character <var>aChar</var>.

    size_t find_first_of(const DwString& aStr, size_t aPos=0) const;
    size_t find_first_of(const char* aBuf, size_t aPos, size_t aLen) const;
    size_t find_first_of(const char* aCstr, size_t aPos=0) const;
    //. Performs a forward search beginning at position <var>aPos</var> for
    //. the first occurrence of any character from a specified set of
    //. characters.  The return value is the position of the character
    //. if found, or <var>DwString::npos</var> if not found.
    //.
    //. The first version searches for any character in the string
    //. <var>aStr</var>.
    //.
    //. The second version searches for any of the <var>aLen</var> characters
    //. in <var>aBuf</var>.
    //.
    //. The third version searches for any character in the NUL-terminated
    //. string <var>aCstr</var>.

    size_t find_last_of(const DwString& aStr, size_t aPos=npos) const;
    size_t find_last_of(const char* aBuf, size_t aPos, size_t aLen) const;
    size_t find_last_of(const char* aCstr, size_t aPos=npos) const;
    //. Performs a reverse search beginning at position <var>aPos</var> for
    //. the first occurrence of any character from a specified set of
    //. characters.  If <var>aPos</var> is greater than or equal to the number
    //. of characters in the string, then the search starts at the end
    //. of the string.  The return value is the position of the character
    //. if found, or <var>DwString::npos</var> if not found.
    //.
    //. The first version searches for any character in the string
    //. <var>aStr</var>.
    //.
    //. The second version searches for any of the <var>aLen</var> characters
    //. in <var>aBuf</var>.
    //.
    //. The third version searches for any character in the NUL-terminated
    //. string <var>aCstr</var>.

    size_t find_first_not_of(const DwString& aStr, size_t aPos=0) const;
    size_t find_first_not_of(const char* aBuf, size_t aPos, size_t aLen) const;
    size_t find_first_not_of(const char* aCstr, size_t aPos=0) const;
    //. Performs a forward search beginning at position <var>aPos</var> for
    //. the first occurrence of any character <i>not</i> in a specified set
    //. of characters.  The return value is the position of the character
    //. if found, or <var>DwString::npos</var> if not found.
    //.
    //. The first version searches for any character not in the string
    //. <var>aStr</var>.
    //.
    //. The second version searches for any character not among the
    //. <var>aLen</var> characters in <var>aBuf</var>.
    //.
    //. The third version searches for any character not in the NUL-terminated
    //. string <var>aCstr</var>.

    size_t find_last_not_of(const DwString& aStr, size_t aPos=npos) const;
    size_t find_last_not_of(const char* aBuf, size_t aPos, size_t aLen) const;
    size_t find_last_not_of(const char* aCstr, size_t aPos=npos) const;
    //. Performs a reverse search beginning at position <var>aPos</var> for
    //. the first occurrence of any character <i>not</i> in a specified set
    //. of characters.  If <var>aPos</var> is greater than or equal to the
    //. number of characters in the string, then the search starts at the end
    //. of the string.  The return value is the position of the character
    //. if found, or <var>DwString::npos</var> if not found.
    //.
    //. The first version searches for any character not in the string
    //. <var>aStr</var>.
    //.
    //. The second version searches for any character not among the
    //. <var>aLen</var> characters in <var>aBuf</var>.
    //.
    //. The third version searches for any character not in the NUL-terminated
    //. string <var>aCstr</var>.

    DwString substr(size_t aPos=0, size_t aLen=npos) const;
    //. Returns a string that contains at most <var>aLen</var> characters from
    //. the <var>DwString</var> object beginning at position <var>aPos</var>.
    //. The returned substring will not contain more characters than what are
    //. available in the superstring.

    int compare(const DwString& aStr) const;
    int compare(size_t aPos1, size_t aLen1, const DwString& aStr) const;
    int compare(size_t aPos1, size_t aLen1, const DwString& aStr,
        size_t aPos2, size_t aLen2) const;
    int compare(const char* aCstr) const;
    int compare(size_t aPos1, size_t aLen1, const char* aBuf, 
        size_t aLen2=npos) const;
    //. These member functions compare a sequence of characters to this
    //. <var>DwString</var> object, or a segment of this <var>DwString</var>
    //. object. They return -1, 0, or 1, depending on whether this
    //. <var>DwString</var> object is less than, equal to, or greater than
    //. the compared sequence of characters, respectively.
    //.
    //. The first version compares <var>aStr</var> to this string.
    //.
    //. The second version compares <var>aStr</var> to the segment of this
    //. string of length <var>aLen</var> beginning at position <var>aPos</var>.
    //.
    //. The third version compares the <var>aLen2</var> characters beginning at
    //. position <var>aPos2</var> in <var>aStr</var> with the <var>aLen1</var>
    //. characters beginning at position <var>aPos1</var> in this
    //. <var>DwString</var> object.
    //.
    //. The fourth version compares the NUL-terminated string <var>aCstr</var>
    //. to this <var>DwString</var>.
    //.
    //. The fifth version compares the <var>aLen2</var> characters in
    //. <var>aBuf</var> with this <var>DwString</var>.

    // Non-ANSI member functions

    virtual const char* ClassName() const;
    //. This virtual function returns the name of the class as a NUL-terminated
    //. char string.

    int ObjectId() const;
    //. Returns the unique object id for this <var>DwString</var>.

    void ConvertToLowerCase();
    void ConvertToUpperCase();
    //. Converts this <var>DwString</var> object's characters to all lower
    //. case or all upper case.
    
    void Trim();
    //. Removes all white space from the beginning and the end of this
    //. <var>DwString</var> object.  White space characters include ASCII HT,
    //. LF, and SPACE.

    void WriteTo(DW_STD ostream& aStrm) const;
    //. Writes the contents of this <var>DwString</var> object to the stream
    //. <var>aStrm</var>.

private:

    DwStringRep* mRep;
    size_t  mStart;
    size_t  mLength;

    void _copy();
    void _replace(size_t aPos1, size_t aLen1, const char* aBuf, size_t aLen2);
    void _replace(size_t aPos1, size_t aLen1, size_t aLen2, char aChar);

public:

    virtual void PrintDebugInfo(DW_STD ostream& aStrm) const;
    //. Prints debugging information about the object to <var>aStrm</var>.
    //.
    //. This member function is available only in the debug version of
    //. the library.

    virtual void CheckInvariants() const;
    //. Aborts if one of the invariants of the object fails.  Use this
    //. member function to track down bugs.
    //.
    //. This member function is available only in the debug version of
    //. the library.

};

DW_CORE_EXPORT DwString operator + (const DwString& aStr1, const DwString& aStr2);
DW_CORE_EXPORT DwString operator + (const char* aCstr, const DwString& aStr2);
DW_CORE_EXPORT DwString operator + (char aChar, const DwString& aStr2);
DW_CORE_EXPORT DwString operator + (const DwString& aStr1, const char* aCstr);
DW_CORE_EXPORT DwString operator + (const DwString& aStr1, char aChar);

DW_CORE_EXPORT DwBool operator == (const DwString& aStr1, const DwString& aStr2);
DW_CORE_EXPORT DwBool operator == (const DwString& aStr1, const char* aCstr);
DW_CORE_EXPORT DwBool operator == (const char* aCstr, const DwString& aStr2);

DW_CORE_EXPORT DwBool operator != (const DwString& aStr1, const DwString& aStr2);
DW_CORE_EXPORT DwBool operator != (const DwString& aStr1, const char* aCstr);
DW_CORE_EXPORT DwBool operator != (const char* aCstr, const DwString& aStr2);

DW_CORE_EXPORT DwBool operator < (const DwString& aStr1, const DwString& aStr2);
DW_CORE_EXPORT DwBool operator < (const DwString& aStr1, const char* aCstr);
DW_CORE_EXPORT DwBool operator < (const char* aCstr, const DwString& aStr2);

DW_CORE_EXPORT DwBool operator > (const DwString& aStr1, const DwString& aStr2);
DW_CORE_EXPORT DwBool operator > (const DwString& aStr1, const char* aCstr);
DW_CORE_EXPORT DwBool operator > (const char* aCstr, const DwString& aStr2);

DW_CORE_EXPORT DwBool operator <= (const DwString& aStr1, const DwString& aStr2);
DW_CORE_EXPORT DwBool operator <= (const DwString& aStr1, const char* aCstr);
DW_CORE_EXPORT DwBool operator <= (const char* aCstr, const DwString& aStr2);

DW_CORE_EXPORT DwBool operator >= (const DwString& aStr1, const DwString& aStr2);
DW_CORE_EXPORT DwBool operator >= (const DwString& aStr1, const char* aCstr);
DW_CORE_EXPORT DwBool operator >= (const char* aCstr, const DwString& aStr2);

DW_CORE_EXPORT DW_STD ostream& operator << (DW_STD ostream& aOstrm, const DwString& aStr);
//. Writes the contents of <var>aStr</var> to the stream <var>aOstrm</var>.

DW_CORE_EXPORT DW_STD istream& getline (DW_STD istream& aIstrm, DwString& aStr, char aDelim);
DW_CORE_EXPORT DW_STD istream& getline (DW_STD istream& aIstrm, DwString& aStr);

DW_CORE_EXPORT int DwStrcasecmp(const DwString& aStr1, const DwString& aStr2);
DW_CORE_EXPORT int DwStrcasecmp(const DwString& aStr1, const char* aCstr);
DW_CORE_EXPORT int DwStrcasecmp(const char* aCstr, const DwString& aStr2);

DW_CORE_EXPORT int DwStrncasecmp(const DwString& aStr1, const DwString& aStr2,
    size_t aLen);
DW_CORE_EXPORT int DwStrncasecmp(const DwString& aStr, const char* aCstr, size_t aLen);
DW_CORE_EXPORT int DwStrncasecmp(const char* aCstr, const DwString& aStr, size_t aLen);

DW_CORE_EXPORT int DwStrcmp(const DwString& aStr1, const DwString& aStr2);
DW_CORE_EXPORT int DwStrcmp(const DwString& aStr, const char* aCstr);
DW_CORE_EXPORT int DwStrcmp(const char* aCstr, const DwString& aStr);

DW_CORE_EXPORT int DwStrncmp(const DwString& aStr1, const DwString& aStr2, size_t aLen);
DW_CORE_EXPORT int DwStrncmp(const DwString& aStr, const char* aCstr, size_t aLen);
DW_CORE_EXPORT int DwStrncmp(const char* aCstr, const DwString& aStr, size_t aLen);

DW_CORE_EXPORT void DwStrcpy(DwString& aStrDest, const DwString& aStrSrc);
DW_CORE_EXPORT void DwStrcpy(DwString& aStrDest, const char* aCstrSrc);
DW_CORE_EXPORT void DwStrcpy(char* aCstrDest, const DwString& aStrSrc);

DW_CORE_EXPORT void DwStrncpy(DwString& aStrDest, const DwString& aStrSrc, size_t aLen);
DW_CORE_EXPORT void DwStrncpy(DwString& aStrDest, const char* aCstrSrc, size_t aLen);
DW_CORE_EXPORT void DwStrncpy(char* aCstrDest, const DwString& aStrSrc, size_t aLen);

DW_CORE_EXPORT char* DwStrdup(const DwString& aStr);

#endif

