//=============================================================================
// File:       string.cpp
// Contents:   Definitions for DwString
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

#define DW_CORE_IMPLEMENTATION

#include <mimepp/config.h>
#include <mimepp/debug.h>

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#ifdef DW_USE_ANSI_IOSTREAM
#  include <iostream>
   using namespace std;
#else
#  include <iostream.h>
#endif
#if defined(DW_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <mimepp/string.h>

extern const char* dw_char_map;

#define ISSPACE(c) (dw_char_map[(c)&0xff]&0x02)
#define DW_MIN(a,b) ((a) <= (b) ? (a) : (b))
#define DW_MAX(a,b) ((a) >= (b) ? (a) : (b))


static int dw_strcasecmp(const char* s1, size_t len1, const char* s2,
    size_t len2)
{
    DW_ASSERT(s1 != 0);
    DW_ASSERT(s2 != 0);
    size_t len = DW_MIN(len1, len2);
    for (size_t i=0; i < len; ++i) {
		int c1 = ::tolower(s1[i]);
		int c2 = ::tolower(s2[i]);
        if (c1 < c2) {
            return -1;
        }
        else if (c1 > c2) {
            return 1;
        }
    }
    if (len1 < len2) {
        return -1;
    }
    else if (len1 > len2) {
        return 1;
    }
    return 0;
}


static int dw_strcmp(const char* s1, size_t len1, const char* s2, size_t len2)
{
    DW_ASSERT(s1 != 0);
    DW_ASSERT(s2 != 0);
    size_t len = DW_MIN(len1, len2);
    for (size_t i=0; i < len; ++i) {
        if (s1[i] < s2[i]) {
            return -1;
        }
        else if (s1[i] > s2[i]) {
            return 1;
        }
    }
    if (len1 < len2) {
        return -1;
    }
    else if (len1 > len2) {
        return 1;
    }
    return 0;
}


// Copy

inline void mem_copy(const char* src, size_t n, char* dest)
{
    DW_ASSERT(src != 0);
    DW_ASSERT(dest != 0);
    DW_ASSERT(src != dest);
    if (n == 0 || src == dest || !src || !dest) return;
    memmove(dest, src, n);
}


// Allocate buffer whose size is a power of 2

static char* mem_alloc(size_t* aSize)
{
    DW_ASSERT(aSize != 0);
    // minimum size is 16
    size_t size = 16;
    while (size < *aSize) {
        size <<= 1;
    }
    *aSize = 0;
    char* buf = new char[size];
    if (buf != 0) {
        *aSize = size;
    }
    return buf;
}


// Free buffer

inline void mem_free(char* buf)
{
    DW_ASSERT(buf != 0);
    if (buf != 0) {
        delete [] buf;
    }
}


//--------------------------------------------------------------------------
// Win32 multithreaded section
//--------------------------------------------------------------------------

#if defined(DW_MULTITHREADED) && defined(DW_WIN32)

inline DwBool is_shared(DwStringRep* rep)
{
    DwBool isShared;
    if (rep->mRefCount > 1) {
        isShared = DwTrue;
    }
    else {
        isShared = DwFalse;
    }
    return isShared;
}

inline DwStringRep* new_rep_reference(DwStringRep* rep)
{
    DW_ASSERT(rep != 0);
    if (rep != 0) {
        InterlockedIncrement(&rep->mRefCount);
    }
    return rep;
}

inline void delete_rep_safely(DwStringRep* rep)
{
    DW_ASSERT(rep != 0);
    if (rep == 0) {
        return;
    }
#if defined(DW_DEBUG_VERSION) || defined(DW_DEVELOPMENT_VERSION)
    if (rep->mRefCount <= 0) {
        DW_STD cerr << "Error: attempt to delete a DwStringRep "
            "with ref count <= 0\n";
        DW_STD cerr << "(Possibly 'delete' was called twice for same "
            "object)\n";
        abort();
    }
#endif // defined(DW_DEBUG_VERSION) || defined(DW_DEVELOPMENT_VERSION)
    LONG result = InterlockedDecrement(&rep->mRefCount);
    if (result == 0) {
        delete rep;
    }
}

#endif

//--------------------------------------------------------------------------
// Unix multithreaded section (POSIX threads)
//--------------------------------------------------------------------------

#if defined(DW_MULTITHREADED) && defined(DW_UNIX)

// Rationale for is_shared() implementation:
//
// It is allowed, under the documented contract, to share a
// DwStringRep between two threads, but a DwString cannot be shared
// between two threads.  This assumption is important.  (Actually, we
// mean that concurrent access to a DwString is not allowed.  It is
// possible to allow serialized access to a DwString between multiple
// threads.)
//
// It is clear from the spec for POSIX threads that we must lock the
// mutex to be guaranteed that we see the correct value for the ref
// count.
//
// It is entirely possible that the ref count will be > 1 when we
// access it, and that it will be == 1 before we branch on the
// is_shared() result.  This is okay.  It will cause the buffer to be
// unnecessarily copied -- but no invariants will be broken.
//
// It is not possible for the ref count to be == 1 when it is
// accessed, and then become > 1 before we branch on the is_shared()
// result.  Here is where we depend heavily on the assumption stated
// above.  If the ref count == 1, then the rep is referenced by only
// one DwString, and all access to that string must be serialized.

inline DwBool is_shared(DwStringRep* rep)
{
    DwBool isShared;
    pthread_mutex_lock(&rep->mMutex);
    if (rep->mRefCount > 1) {
        isShared = DwTrue;
    }
    else {
        isShared = DwFalse;
    }
    pthread_mutex_unlock(&rep->mMutex);
    return isShared;
}

inline DwStringRep* new_rep_reference(DwStringRep* rep)
{
    DW_ASSERT(rep != 0);
    if (rep != 0) {
        pthread_mutex_lock(&rep->mMutex);
        ++rep->mRefCount;
        pthread_mutex_unlock(&rep->mMutex);
    }
    return rep;
}

inline void delete_rep_safely(DwStringRep* rep)
{
    DW_ASSERT(rep != 0);
    if (rep == 0) {
        return;
    }
    int refCount;
    pthread_mutex_lock(&rep->mMutex);
    --rep->mRefCount;
    refCount = rep->mRefCount;
    pthread_mutex_unlock(&rep->mMutex);
    if (refCount == 0) {
        delete rep;
    }
}

#endif

//--------------------------------------------------------------------------
// Single threaded section
//--------------------------------------------------------------------------

#if !defined(DW_MULTITHREADED)

inline DwBool is_shared(DwStringRep* rep)
{
    DwBool isShared;
    if (rep->mRefCount > 1) {
        isShared = DwTrue;
    }
    else {
        isShared = DwFalse;
    }
    return isShared;
}

inline DwStringRep* new_rep_reference(DwStringRep* rep)
{
    DW_ASSERT(rep != 0);
    if (rep != 0) {
        ++rep->mRefCount;
    }
    return rep;
}

inline void delete_rep_safely(DwStringRep* rep)
{
    DW_ASSERT(rep != 0);
    if (rep == 0) {
        return;
    }
#if defined(DW_DEBUG_VERSION) || defined(DW_DEVELOPMENT_VERSION)
    if (rep->mRefCount <= 0) {
        DW_STD cerr << "Error: attempt to delete a DwStringRep "
            "with ref count <= 0\n";
        DW_STD cerr << "(Possibly 'delete' was called twice for same "
            "object)\n";
        abort();
    }
#endif // defined(DW_DEBUG_VERSION) || defined(DW_DEVELOPMENT_VERSION)
    --rep->mRefCount;
    if (rep->mRefCount == 0) {
        delete rep;
    }
}

#endif


//--------------------------------------------------------------------------


DwStringRep::DwStringRep()
{
    mSize = 0;
    mBuffer = 0;
    mRefCount = 1;
#if defined(DW_MULTITHREADED) && defined(DW_UNIX)
    pthread_mutex_init(&mMutex, 0);
#endif
    size_t size = 1;
    char* buffer = mem_alloc(&size);
    if (buffer != 0) {
        mSize = size;
        mBuffer = buffer;
    }
}


// DwStringRep takes ownership of the buffer passed as an argument

DwStringRep::DwStringRep(char* aBuf, size_t aSize)
{
    DW_ASSERT(aBuf != 0);
    mSize = aSize;
    mBuffer = aBuf;
    mRefCount = 1;
#if defined(DW_MULTITHREADED) && defined(DW_UNIX)
    pthread_mutex_init(&mMutex, 0);
#endif
}


DwStringRep::~DwStringRep()
{
#if defined (DW_DEBUG_VERSION) || defined (DW_DEVELOPMENT_VERSION)
    if (mBuffer == 0) {
        DW_STD cerr << "DwStringRep destructor called for bad DwStringRep "
            "object\n";
        DW_STD cerr << "(Possibly 'delete' was called twice for same "
            "object)\n";
        abort();
    }
#endif // defined (DW_DEBUG_VERSION) || defined (DW_DEVELOPMENT_VERSION)
    mem_free(mBuffer);
    mBuffer = 0;
#if defined(DW_MULTITHREADED) && defined(DW_UNIX)
    pthread_mutex_destroy(&mMutex);
#endif
}


void DwStringRep::CheckInvariants() const
{
#if defined (DW_DEBUG_VERSION)
    DW_ASSERT(mBuffer != 0);
    DW_ASSERT(mSize > 0);
    DW_ASSERT(mRefCount > 0);
#endif // defined (DW_DEBUG_VERSION)
}


//--------------------------------------------------------------------------

const size_t DwString::npos = (size_t) -1;


DwString::DwString()
{
    // Set valid values, in case an exception is thrown
    mStart = 0;
    mLength = 0;
    mRep = 0;
    mRep = new DwStringRep;
}


DwString::DwString(const DwString& aStr, size_t aPos, size_t aLen)
{
    DW_ASSERT(aPos <= aStr.mLength);
    DBG_STMT(aStr.CheckInvariants())
    size_t pos = DW_MIN(aPos, aStr.mLength);
    size_t len = DW_MIN(aLen, aStr.mLength - pos);
    if (len > 0) {
        mStart = aStr.mStart + pos;
        mLength = len;
        mRep = new_rep_reference(aStr.mRep);
    }
    else /* if (len == 0) */ {
        // Set valid values, in case an exception is thrown
        mStart = 0;
        mLength = 0;
        mRep = 0;
        mRep = new DwStringRep;
    }
}


DwString::DwString(const char* aBuf, size_t aLen)
{
    DW_ASSERT(aBuf != 0);
    DW_ASSERT(aLen != (size_t)-1);
    // Set valid values, in case an exception is thrown
    mStart = 0;
    mLength = 0;
    mRep = 0;
    mRep = new DwStringRep;
    if (aLen > 0) {
        _replace(0, mLength, aBuf, aLen);
    }
}


DwString::DwString(const char* aCstr)
{
    DW_ASSERT(aCstr != 0);
    // Set valid values, in case an exception is thrown
    mStart = 0;
    mLength = 0;
    mRep = 0;
    mRep = new DwStringRep;
    size_t len = (aCstr) ? strlen(aCstr) : 0;
    if (len > 0) {
        _replace(0, mLength, aCstr, len);
    }
}


DwString::DwString(size_t aLen, char aChar)
{
    DW_ASSERT(aLen != (size_t)-1);
    // Set valid values, in case an exception is thrown
    mStart = 0;
    mLength = 0;
    mRep = 0;
    mRep = new DwStringRep;
    if (aLen > 0) {
        _replace(0, mLength, aLen, aChar);
    }
}


DwString::~DwString()
{
    DW_ASSERT(mRep != 0);
    delete_rep_safely(mRep);
    mRep = 0;
}


DwString& DwString::operator = (const DwString& aStr)
{
    return assign(aStr);
}


DwString& DwString::operator = (const char* aCstr)
{
    return assign(aCstr);
}


DwString& DwString::operator = (char aChar)
{
    return assign(1, aChar);
}


size_t DwString::size() const
{
    return mLength;
}


size_t DwString::length() const
{
    return mLength;
}


size_t DwString::max_size() const
{
    return ((size_t)-1) - 1;
}


void DwString::resize(size_t aLen, char aChar)
{
    // making string shorter?
    if (aLen < mLength) {
        mLength = aLen;
    }
    // expanding string
    else if (aLen > mLength) {
        _replace(mLength, 0, aLen-mLength, aChar);
    }
}


void DwString::resize(size_t aLen)
{
    resize(aLen, 0);
}


size_t DwString::capacity() const
{
    return (mRep != 0) ? mRep->mSize - 1 : 0;
}


void DwString::reserve(size_t aSize)
{
    if (mRep == 0) {
        return;
    }
    if (! is_shared(mRep) && aSize < mRep->mSize) {
        return;
    }
    size_t size = aSize + 1;
    char* newBuf = mem_alloc(&size);
    DW_ASSERT(newBuf != 0);
    if (newBuf != 0) {
        char* to = newBuf;
        const char* from = mRep->mBuffer + mStart;
        mem_copy(from, mLength, to);
        to[mLength] = 0;
        DwStringRep* rep = new DwStringRep(newBuf, size);
        DW_ASSERT(rep != 0);
        if (rep != 0) {
            delete_rep_safely(mRep);
            mRep = rep;
            mStart = 0;
        }
        else {
             mem_free(newBuf);
        }
    }
}


void DwString::clear()
{
    assign("");
}


DwBool DwString::empty() const
{
    return mLength == 0;
}


const char& DwString::operator [] (size_t aPos) const
{
    return at(aPos);
}


char& DwString::operator [] (size_t aPos)
{
    return at(aPos);
}


const char& DwString::at(size_t aPos) const
{
    DW_ASSERT(aPos <= mLength);
    static const char c = 0;
    if (aPos < mLength) {
        return data()[aPos];
    }
    else if (aPos == mLength) {
        return c;
    }
    else {
        // This is "undefined behavior."
        // Normally, this will not occur.  The DW_ASSERT() macro will catch it,
        // or at some point we may throw an exception.
        return c;
    }
}


char& DwString::at(size_t aPos)
{
    DW_ASSERT(aPos < mLength);
    if (aPos < mLength) {
        return (char&) c_str()[aPos];
    }
    else {
        // This is "undefined behavior"
        DW_ASSERT(0);
        return (char&) c_str()[0];
    }
}


DwString& DwString::operator += (const DwString& aStr)
{
    return append(aStr);
}


DwString& DwString::operator += (const char* aCstr)
{
    return append(aCstr);
}


DwString& DwString::operator += (char aChar)
{
    return append(1, aChar);
}


DwString& DwString::append(const DwString& aStr)
{
    return append(aStr, 0, aStr.mLength);
}


DwString& DwString::append(const DwString& aStr, size_t aPos, size_t aLen)
{
    DW_ASSERT(aPos <= aStr.mLength);
    DW_ASSERT(mRep != 0 && aStr.mRep != 0);
    if (mRep == 0 || aStr.mRep == 0) {
        return *this;
    }
    size_t pos = DW_MIN(aPos, aStr.mLength);
    size_t len = DW_MIN(aLen, aStr.mLength - pos);
    if (&aStr == this) {
        DwString temp(aStr);
        _replace(mLength, 0, &temp.mRep->mBuffer[temp.mStart+pos], len);
    }
    else {
        _replace(mLength, 0, &aStr.mRep->mBuffer[aStr.mStart+pos], len);
    }
    return *this;
}


DwString& DwString::append(const char* aBuf, size_t aLen)
{
    DW_ASSERT(aBuf != 0);
    if (aBuf != 0) {
        _replace(mLength, 0, aBuf, aLen);
    }
    return *this;
}


DwString& DwString::append(const char* aCstr)
{
    DW_ASSERT(aCstr != 0);
    size_t len = (aCstr) ? strlen(aCstr) : 0;
    _replace(mLength, 0, aCstr, len);
    return *this;
}


DwString& DwString::append(size_t aLen, char aChar)
{
    _replace(mLength, 0, aLen, aChar);
    return *this;
}


DwString& DwString::assign(const DwString& aStr)
{
    if (this != &aStr) {
        assign(aStr, 0, aStr.mLength);
    }
    return *this;
}


DwString& DwString::assign(const DwString& aStr, size_t aPos, size_t aLen)
{
    DW_ASSERT(aPos <= aStr.mLength);
    size_t pos = DW_MIN(aPos, aStr.mLength);
    size_t len = DW_MIN(aLen, aStr.mLength - pos);
    if (mRep == aStr.mRep) {
        mStart = aStr.mStart + pos;
        mLength = len;
    }
    else {
        delete_rep_safely(mRep);
        mRep = new_rep_reference(aStr.mRep);
        mStart = aStr.mStart + pos;
        mLength = len;
    }
    return *this;
}


DwString& DwString::assign(const char* aBuf, size_t aLen)
{
    DW_ASSERT(aBuf != 0);
    DW_ASSERT(aLen != (size_t)-1);
    _replace(0, mLength, aBuf, aLen);
    return *this;
}


DwString& DwString::assign(const char* aCstr)
{
    DW_ASSERT(aCstr != 0);
    size_t len = (aCstr) ? strlen(aCstr) : 0;
    _replace(0, mLength, aCstr, len);
    return *this;
}


DwString& DwString::assign(size_t aLen, char aChar)
{
    DW_ASSERT(aLen != (size_t)-1);
    _replace(0, mLength, aLen, aChar);
    return *this;
}


DwString& DwString::insert(size_t aPos, const DwString& aStr)
{
    return insert(aPos, aStr, 0, aStr.mLength);
}


DwString& DwString::insert(size_t aPos1, const DwString& aStr,
    size_t aPos2, size_t aLen2)
{
    DW_ASSERT(aPos1 <= mLength);
    DW_ASSERT(aPos2 <= aStr.mLength);
    size_t pos2 = DW_MIN(aPos2, aStr.mLength);
    size_t len2 = DW_MIN(aLen2, aStr.mLength - pos2);
    if (&aStr == this) {
        DwString temp(aStr);
        _replace(aPos1, 0, &temp.mRep->mBuffer[temp.mStart+pos2], len2);
    }
    else {
        _replace(aPos1, 0, &aStr.mRep->mBuffer[aStr.mStart+pos2], len2);
    }
    return *this;
}


DwString& DwString::insert(size_t aPos, const char* aBuf, size_t aLen)
{
    DW_ASSERT(aBuf != 0);
    _replace(aPos, 0, aBuf, aLen);
    return *this;
}


DwString& DwString::insert(size_t aPos, const char* aCstr)
{
    DW_ASSERT(aCstr != 0);
    size_t len = (aCstr) ? strlen(aCstr) : 0;
    _replace(aPos, 0, aCstr, len);
    return *this;
}


DwString& DwString::insert(size_t aPos, size_t aLen, char aChar)
{
    _replace(aPos, 0, aLen, aChar);
    return *this;
}


DwString& DwString::erase(size_t aPos, size_t aLen)
{
    DW_ASSERT(aPos <= mLength);
    size_t pos = DW_MIN(aPos, mLength);
    size_t len = DW_MIN(aLen, mLength - pos);
    _replace(pos, len, "", 0);
    return *this;
}


DwString& DwString::replace(size_t aPos1, size_t aLen1, const DwString& aStr)
{
    return replace(aPos1, aLen1, aStr, 0, aStr.mLength);
}


DwString& DwString::replace(size_t aPos1, size_t aLen1, const DwString& aStr,
    size_t aPos2, size_t aLen2)
{
    DW_ASSERT(aPos2 <= aStr.mLength);
    size_t pos2 = DW_MIN(aPos2, aStr.mLength);
    size_t len2 = DW_MIN(aLen2, aStr.mLength - pos2);
    if (&aStr == this) {
        DwString temp(aStr);
        _replace(aPos1, aLen1, &temp.mRep->mBuffer[temp.mStart+pos2], len2);
    }
    else {
        _replace(aPos1, aLen1, &aStr.mRep->mBuffer[aStr.mStart+pos2], len2);
    }
    return *this;
}


DwString& DwString::replace(size_t aPos1, size_t aLen1, const char* aBuf,
    size_t aLen2)
{
    _replace(aPos1, aLen1, aBuf, aLen2);
    return *this;
}


DwString& DwString::replace(size_t aPos1, size_t aLen1, const char* aCstr)
{
    size_t len2 = (aCstr) ? strlen(aCstr) : 0;
    _replace(aPos1, aLen1, aCstr, len2);
    return *this;
}


DwString& DwString::replace(size_t aPos1, size_t aLen1, size_t aLen2,
    char aChar)
{
    _replace(aPos1, aLen1, aLen2, aChar);
    return *this;
}


size_t DwString::copy(char* aBuf, size_t aLen, size_t aPos) const
{
    DW_ASSERT(aPos <= mLength);
    DW_ASSERT(aBuf != 0);
    size_t pos = DW_MIN(aPos, mLength);
    size_t len = DW_MIN(aLen, mLength - pos);
    char* to = aBuf;
    const char* from = mRep->mBuffer + mStart + pos;
    mem_copy(from, len, to);
    return len;
}


void DwString::swap(DwString& aStr)
{
    DwStringRep* rep = mRep;
    mRep = aStr.mRep;
    aStr.mRep = rep;
    size_t n = mStart;
    mStart = aStr.mStart;
    aStr.mStart = n;
    n = mLength;
    mLength = aStr.mLength;
    aStr.mLength = n;
}


const char* DwString::c_str() const
{ 
    if (is_shared(mRep)) {
        DwString* xthis = (DwString*) this;
        xthis->_copy();
    }
    else {
        mRep->mBuffer[mStart+mLength] = 0;
    }
    return &mRep->mBuffer[mStart];
}


const char* DwString::data() const
{
    return &mRep->mBuffer[mStart];
}

const char * DwString::begin(void) const
{
	return &mRep->mBuffer[mStart];
}

char * DwString::begin(void)
{
	return &mRep->mBuffer[mStart];
}

const char * DwString::end(void) const
{
	return begin() + mLength;
}

char * DwString::end(void)
{
	return begin() + mLength;
}

size_t DwString::find(const DwString& aStr, size_t aPos) const
{
    return find(&aStr.mRep->mBuffer[aStr.mStart], aPos, aStr.mLength);
}


size_t DwString::find(const char* aBuf, size_t aPos, size_t aLen) const
{
    DW_ASSERT(aBuf != 0);
    if (aBuf == 0) return (size_t)-1;
    if (aLen > mLength) return (size_t)-1;
    if (aPos > mLength-aLen) return (size_t)-1;
    if (aLen == 0) return aPos;
    const char* buf = mRep->mBuffer + mStart;
    for (size_t i=aPos; i <= mLength-aLen; ++i) {
        size_t k = i;
        size_t j = 0;
        while (j < aLen && aBuf[j] == buf[k]) {
            ++j; ++k;
        }
        if (j == aLen) return i;
    }
    return (size_t)-1;
}


size_t DwString::find(const char* aCstr, size_t aPos) const
{
    DW_ASSERT(aCstr != 0);
    if (aCstr == 0) return (size_t)-1;
    size_t len = strlen(aCstr);
    return find(aCstr, aPos, len);
}


size_t DwString::find(char aChar, size_t aPos) const
{
    if (aPos >= mLength) return (size_t)-1;
    const char* buf = mRep->mBuffer + mStart;
    for (size_t i=aPos; i < mLength; ++i) {
        if (buf[i] == aChar) return i;
    }
    return (size_t)-1;
}


size_t DwString::rfind(const DwString& aStr, size_t aPos) const
{
    return rfind(&aStr.mRep->mBuffer[aStr.mStart], aPos, aStr.mLength);
}


size_t DwString::rfind(const char* aBuf, size_t aPos, size_t aLen) const
{
    DW_ASSERT(aBuf != 0);
    if (aBuf == 0) return (size_t)-1;
    if (aLen > mLength) return (size_t)-1;
    size_t pos = DW_MIN(aPos, mLength - aLen);
    if (aLen == 0) return pos;
    const char* buf = mRep->mBuffer + mStart;
    for (size_t i=0; i <= pos; ++i) {
        size_t k = pos - i;
        size_t j = 0;
        while (j < aLen && aBuf[j] == buf[k]) {
            ++j; ++k;
        }
        if (j == aLen) return pos - i;
    }
    return (size_t)-1;
}


size_t DwString::rfind(const char* aCstr, size_t aPos) const
{
    DW_ASSERT(aCstr != 0);
    size_t len = (aCstr) ? strlen(aCstr) : 0;
    return rfind(aCstr, aPos, len);
}


size_t DwString::rfind(char aChar, size_t aPos) const
{
    if (mLength > 0) {
        size_t pos = DW_MIN(aPos, mLength - 1);
        const char* buf = mRep->mBuffer + mStart;
        for (size_t i=0; i <= pos; ++i) {
            size_t k = pos - i;
            if (buf[k] == aChar) return k;
        }
    }
    return (size_t)-1;
}


size_t DwString::find_first_of(const DwString& aStr, size_t aPos) const
{
    return find_first_of(&aStr.mRep->mBuffer[aStr.mStart], aPos, aStr.mLength);
}


size_t DwString::find_first_of(const char* aBuf, size_t aPos,
    size_t aLen) const
{
    DW_ASSERT(aBuf != 0);
    if (aBuf == 0) return (size_t)-1;
    if (aPos >= mLength) return (size_t)-1;
    if (aLen == 0) return aPos;
    char table[256];
    memset(table, 0, sizeof(table));
    for (size_t j=0; j < aLen; ++j) {
        table[aBuf[j]&0xff] = 1;
    }
    const char* buf = mRep->mBuffer + mStart;
    for (size_t i=aPos; i < mLength; ++i) {
        if (table[buf[i]&0xff]) return i;
    }
    return (size_t)-1;
}


size_t DwString::find_first_of(const char* aCstr, size_t aPos) const
{
    DW_ASSERT(aCstr != 0);
    if (aCstr == 0) return (size_t)-1;
    size_t len = strlen(aCstr);
    return find_first_of(aCstr, aPos, len);
}


size_t DwString::find_last_of(const DwString& aStr, size_t aPos) const
{
    return find_last_of(&aStr.mRep->mBuffer[aStr.mStart], aPos, aStr.mLength);
}


size_t DwString::find_last_of(const char* aBuf, size_t aPos, size_t aLen) const
{
    DW_ASSERT(aBuf != 0);
    if (aBuf == 0) return (size_t)-1;
    if (mLength == 0) return (size_t)-1;
    size_t pos = DW_MIN(aPos, mLength - 1);
    if (aLen == 0) return pos;
    char table[256];
    memset(table, 0, sizeof(table));
    for (size_t j=0; j < aLen; ++j) {
        table[aBuf[j]&0xff] = 1;
    }
    const char* buf = mRep->mBuffer + mStart;
    for (size_t k=0; k <= pos; ++k) {
        size_t i = pos - k;
        if (table[buf[i]&0xff]) return i;
    }
    return (size_t)-1;
}


size_t DwString::find_last_of(const char* aCstr, size_t aPos) const
{
    DW_ASSERT(aCstr != 0);
    if (aCstr == 0) return (size_t)-1;
    size_t len = strlen(aCstr);
    return find_last_of(aCstr, aPos, len);
}


size_t DwString::find_first_not_of(const DwString& aStr, size_t aPos) const
{
    return find_first_not_of(&aStr.mRep->mBuffer[aStr.mStart], aPos,
        aStr.mLength);
}


size_t DwString::find_first_not_of(const char* aBuf, size_t aPos,
    size_t aLen) const
{
    DW_ASSERT(aBuf != 0);
    if (aBuf == 0) return (size_t)-1;
    if (aPos >= mLength) return (size_t)-1;
    if (aLen == 0) return (size_t)-1;
    char table[256];
    memset(table, 1, sizeof(table));
    for (size_t j=0; j < aLen; ++j) {
        table[aBuf[j]&0xff] = 0;
    }
    const char* buf = mRep->mBuffer + mStart;
    for (size_t i=aPos; i < mLength; ++i) {
        if (table[buf[i]&0xff]) return i;
    }
    return (size_t)-1;
}


size_t DwString::find_first_not_of(const char* aCstr, size_t aPos) const
{
    DW_ASSERT(aCstr != 0);
    if (aCstr == 0) return (size_t)-1;
    size_t len = strlen(aCstr);
    return find_first_not_of(aCstr, aPos, len);
}


size_t DwString::find_last_not_of(const DwString& aStr, size_t aPos) const
{
    return find_last_not_of(&aStr.mRep->mBuffer[aStr.mStart], aPos,
        aStr.mLength);
}


size_t DwString::find_last_not_of(const char* aBuf, size_t aPos,
    size_t aLen) const
{
    DW_ASSERT(aBuf != 0);
    if (aBuf == 0) return (size_t)-1;
    if (mLength == 0) return (size_t)-1;
    size_t pos = DW_MIN(aPos, mLength - 1);
    if (aLen == 0) return (size_t)-1;
    char table[256];
    memset(table, 1, sizeof(table));
    for (size_t j=0; j < aLen; ++j) {
        table[aBuf[j]&0xff] = 0;
    }
    const char* buf = mRep->mBuffer + mStart;
    for (size_t k=0; k <= pos; ++k) {
        size_t i = pos - k;
        if (table[buf[i]&0xff]) return i;
    }
    return (size_t)-1;
}


size_t DwString::find_last_not_of(const char* aCstr, size_t aPos) const
{
    DW_ASSERT(aCstr != 0);
    if (aCstr == 0) return (size_t)-1;
    size_t len = strlen(aCstr);
    return find_last_not_of(aCstr, aPos, len);
}


DwString DwString::substr(size_t aPos, size_t aLen) const
{
    DW_ASSERT(aPos <= mLength);
    size_t pos = DW_MIN(aPos, mLength);
    size_t len = DW_MIN(aLen, mLength - pos);
    return DwString(*this, pos, len);
}


int DwString::compare(const DwString& aStr) const
{
    return compare(0, mLength, aStr, 0, aStr.mLength);
}


int DwString::compare(size_t aPos1, size_t aLen1, const DwString& aStr) const
{
    return compare(aPos1, aLen1, aStr, 0, aStr.mLength);
}


int DwString::compare(size_t aPos1, size_t aLen1, const DwString& aStr,
        size_t aPos2, size_t aLen2) const
{
    DW_ASSERT(aPos1 <= mLength);
    DW_ASSERT(aPos2 <= aStr.mLength);
    size_t pos1 = DW_MIN(aPos1, mLength);
    const char* buf1 = mRep->mBuffer + mStart + pos1;
    size_t len1 = DW_MIN(aLen1, mLength - pos1);
    size_t pos2 = DW_MIN(aPos2, aStr.mLength);
    const char* buf2 = aStr.mRep->mBuffer + aStr.mStart + pos2;
    size_t len2 = DW_MIN(aLen2, aStr.mLength - pos2);
    size_t len = DW_MIN(len1, len2);
    int r = strncmp(buf1, buf2, len);
    if (r == 0) {
        if (len1 < len2)
            r = -1;
        else if (len1 > len2) {
            r = 1;
        }
    }
    return r;
}


int DwString::compare(const char* aCstr) const
{
    DW_ASSERT(aCstr != 0);
    size_t len = (aCstr) ? strlen(aCstr) : 0;
    return compare(0, mLength, aCstr, len);
}


int DwString::compare(size_t aPos1, size_t aLen1, const char* aBuf,
    size_t aLen2) const
{
    DW_ASSERT(aBuf != 0);
    DW_ASSERT(aPos1 <= mLength);
    if (aBuf == 0) {
        return (aLen1 > 0) ? 1 : 0;
    }
    size_t pos1 = DW_MIN(aPos1, mLength);
    const char* buf1 = mRep->mBuffer + mStart + pos1;
    size_t len1 = DW_MIN(aLen1, mLength - pos1);
    const char* buf2 = aBuf;
    size_t len2 = aLen2;
    size_t len = DW_MIN(len1, len2);
    int r = strncmp(buf1, buf2, len);
    if (r == 0) {
        if (len1 < len2)
            r = -1;
        else if (len1 > len2) {
            r = 1;
        }
    }
    return r;
}


const char* DwString::ClassName() const
{
    return "DwString";
}


int DwString::ObjectId() const
{
    return (int) (long) this;
}


void DwString::ConvertToLowerCase()
{
    if (is_shared(mRep)) {
        _copy();
    }
    char* buf = mRep->mBuffer + mStart;
    for (size_t i=0; i < mLength; ++i) {
		buf[i] = (char) ::tolower(buf[i]);
    }
}


void DwString::ConvertToUpperCase()
{
    if (is_shared(mRep)) {
        _copy();
    }
    char* buf = mRep->mBuffer + mStart;
    for (size_t i=0; i < mLength; ++i) {
        buf[i] = (char) toupper(buf[i]);
    }
}


void DwString::Trim()
{
    const char* buf = mRep->mBuffer + mStart;
    size_t i = 0;
    while (mLength > 0) {
        int ch = buf[i] & 0xff;
        if (ISSPACE(ch)) {
            ++mStart;
            --mLength;
            ++i;
        }
        else {
            break;
        }
    }
    buf = mRep->mBuffer + mStart;
    i = mLength - 1;
    while (mLength > 0) {
        int ch = buf[i] & 0xff;
        if (ISSPACE(ch)) {
            --mLength;
            --i;
        }
        else {
            break;
        }
    }
    if (mLength == 0) {
        assign("");
    }
}


void DwString::WriteTo(DW_STD ostream& aStrm) const
{
    const char* buf = mRep->mBuffer + mStart;
    for (size_t i=0; i < mLength; ++i) {
        aStrm.put(buf[i]);
    }
}


void DwString::_copy()
{
    if (is_shared(mRep)) {
        size_t size = mLength + 1;
        char* newBuf = mem_alloc(&size);
        DW_ASSERT(newBuf != 0);
        if (newBuf != 0) {
            char* to = newBuf;
            const char* from = mRep->mBuffer + mStart;
            mem_copy(from, mLength, to);
            to[mLength] = 0;
            DwStringRep* rep = new DwStringRep(newBuf, size);
            DW_ASSERT(rep != 0);
            if (rep != 0) {
                delete_rep_safely(mRep);
                mRep = rep;
                mStart = 0;
            }
            else /* if (rep == 0) */ {
                mem_free(newBuf);
                mLength = 0;
            }
        }
        else /* if (newBuf == 0) */ {
            mLength = 0;
        }
    }
}


void DwString::_replace(size_t aPos1, size_t aLen1, const char* aBuf,
    size_t aLen2)
{
    DW_ASSERT(aPos1 <= mLength);
    DW_ASSERT(aBuf != 0);
    size_t pos1 = DW_MIN(aPos1, mLength);
    size_t len1 = DW_MIN(aLen1, mLength - pos1);
    DW_ASSERT(mStart + mLength - len1 < ((size_t)-1) - aLen2);
    size_t len2 = DW_MIN(aLen2, ((size_t)-1) - (mStart + mLength - len1));
    size_t i;
    char* to;
    const char* from;
    size_t newLen = (mLength - len1) + len2;
    // Is new string empty?
    if (newLen == 0 || aBuf == 0) {
        size_t size = 4;
        char* newBuf = mem_alloc(&size);
        DW_ASSERT(newBuf != 0);
        if (newBuf != 0) {
            newBuf[0] = 0;
            DwStringRep* rep = new DwStringRep(newBuf, size);
            DW_ASSERT(rep != 0);
            if (rep != 0) {
                delete_rep_safely(mRep);
                mRep = rep;
                mStart = 0;
                mLength = newLen;
            }
        }
    }
    // Is buffer shared?  Is buffer too small?
    else if (is_shared(mRep) || newLen >= mRep->mSize) {
        size_t size = newLen + 1;
        char* newBuf = mem_alloc(&size);
        DW_ASSERT(newBuf != 0);
        if (newBuf != 0) {
            to = newBuf;
            from = mRep->mBuffer + mStart;
            for (i=0; i < pos1; ++i) *to++ = *from++;
            from = aBuf;
            for (i=0; i < len2; ++i) *to++ = *from++;
            from = mRep->mBuffer + mStart + pos1 + len1;
            for (i=0; i < mLength - pos1 - len1; ++i) *to++ = *from++;
            *to = 0;
            DwStringRep* rep = new DwStringRep(newBuf, size);
            DW_ASSERT(rep != 0);
            if (rep != 0) {
                delete_rep_safely(mRep);
                mRep = rep;
                mStart = 0;
                mLength = newLen;
            }
        }
    }
    // Is the replacement smaller than the replaced?
    else if (len2 < len1) {
        to = mRep->mBuffer + mStart + pos1;
        from = aBuf;
        for (i=0; i < len2; ++i) *to++ = *from++;
        from = mRep->mBuffer + mStart + pos1 + len1;
        for (i=0; i < mLength - pos1 - len1; ++i) *to++ = *from++;
        *to = 0;
        mLength = newLen;
    }
    // Is there enough room at end of buffer?
    else if (mStart + newLen < mRep->mSize) {
        to = mRep->mBuffer + mStart + newLen;
        from = mRep->mBuffer + mStart + mLength - 1;
        *to-- = 0;
        for (i=0; i < mLength-pos1-len1; ++i) *to-- = *from--;
        from = aBuf + (len2 - 1);
        for (i=0; i < len2; ++i) *to-- = *from--;
        mLength = newLen;
    }
    // Is there enough room at beginning of buffer?
    else if (len2 - len1 <= mStart) {
        to = mRep->mBuffer + mStart - (len2 - len1);
        from = mRep->mBuffer + mStart;
        for (i=0; i < pos1; ++i) *to++ = *from++;
        from = aBuf;
        for (i=0; i < len2; ++i) *to++ = *from++;
        mStart -= len2 - len1;
        mLength = newLen;
    }
    // There's enough room, but we must move characters.
    else {
        to = mRep->mBuffer + newLen;
        from = mRep->mBuffer + mStart + mLength - 1;
        *to-- = 0;
        for (i=0; i < mLength-pos1-len1; ++i) *to-- = *from--;
        to = mRep->mBuffer;
        from = mRep->mBuffer + mStart;
        for (i=0; i < pos1; ++i) *to++ = *from++;
        from = aBuf;
        for (i=0; i < len2; ++i) *to++ = *from++;
        mStart = 0;
        mLength = newLen;
    }
}


void DwString::_replace(size_t aPos1, size_t aLen1, size_t aLen2, char aChar)
{
    DW_ASSERT(aPos1 <= mLength);
    size_t pos1 = DW_MIN(aPos1, mLength);
    size_t len1 = DW_MIN(aLen1, mLength - pos1);
    DW_ASSERT(mStart + mLength - len1 < ((size_t)-1) - aLen2);
    size_t len2 = DW_MIN(aLen2, ((size_t)-1) - (mStart + mLength - len1));
    size_t i;
    char* to;
    const char* from;
    size_t newLen = mLength - len1 + len2;
    // Is new string empty?
    if (newLen == 0) {
        size_t size = 1;
        char* newBuf = mem_alloc(&size);
        DW_ASSERT(newBuf != 0);
        if (newBuf != 0) {
            newBuf[0] = 0;
            DwStringRep* rep = new DwStringRep(newBuf, size);
            DW_ASSERT(rep != 0);
            if (rep != 0) {
                delete_rep_safely(mRep);
                mRep = rep;
                mStart = 0;
                mLength = newLen;
            }
        }
    }
    // Is buffer shared?  Is buffer too small?
    else if (is_shared(mRep) || newLen >= mRep->mSize) {
        size_t size = newLen + 1;
        char* newBuf = mem_alloc(&size);
        DW_ASSERT(newBuf != 0);
        if (newBuf != 0) {
            to = newBuf;
            from = mRep->mBuffer + mStart;
            for (i=0; i < pos1; ++i) *to++ = *from++;
            for (i=0; i < len2; ++i) *to++ = aChar;
            from = mRep->mBuffer + mStart + pos1 + len1;
            for (i=0; i < mLength - pos1 - len1; ++i) *to++ = *from++;
            *to = 0;
            DwStringRep* rep = new DwStringRep(newBuf, size);
            DW_ASSERT(rep != 0);
            if (rep != 0) {
                delete_rep_safely(mRep);
                mRep = rep;
                mStart = 0;
                mLength = newLen;
            }
        }
    }
    // Is the replacement smaller than the replaced?
    else if (len2 < len1) {
        to = mRep->mBuffer + mStart + pos1;
        for (i=0; i < len2; ++i) *to++ = aChar;
        from = mRep->mBuffer + mStart + pos1 + len1;
        for (i=0; i < mLength - pos1 - len1; ++i) *to++ = *from++;
        *to = 0;
        mLength = newLen;
    }
    // Is there enough room at end of buffer?
    else if (mStart + newLen < mRep->mSize) {
        to = mRep->mBuffer + mStart + newLen;
        from = mRep->mBuffer + mStart + mLength - 1;
        *to-- = 0;
        for (i=0; i < mLength-pos1-len1; ++i) *to-- = *from--;
        for (i=0; i < len2; ++i) *to-- = aChar;
        mLength = newLen;
    }
    // Is there enough room at beginning of buffer?
    else if (len2 - len1 <= mStart) {
        to = mRep->mBuffer + mStart - (len2 - len1);
        from = mRep->mBuffer + mStart;
        for (i=0; i < pos1; ++i) *to++ = *from++;
        for (i=0; i < len2; ++i) *to++ = aChar;
        mStart -= len2 - len1;
        mLength = newLen;
    }
    // There's enough room, but we must move characters.
    else {
        to = mRep->mBuffer + newLen;
        from = mRep->mBuffer + mStart + mLength - 1;
        *to-- = 0;
        for (i=0; i < mLength-pos1-len1; ++i) *to-- = *from--;
        to = mRep->mBuffer;
        from = mRep->mBuffer + mStart;
        for (i=0; i < pos1; ++i) *to++ = *from++;
        for (i=0; i < len2; ++i) *to++ = aChar;
        mStart = 0;
        mLength = newLen;
    }
}


#if defined (DW_DEBUG_VERSION)
void DwString::PrintDebugInfo(DW_STD ostream& aStrm) const
{
    aStrm << 
    "----------------- Debug info for DwString class ----------------\n";
    aStrm << "Id:               " << ClassName() << ", " << ObjectId() << "\n";
    aStrm << "Rep:              " << (void*) mRep << "\n";
    aStrm << "Buffer:           " << (void*) mRep->mBuffer << "\n";
    aStrm << "Buffer size:      " << mRep->mSize << "\n";
    aStrm << "Start:            " << mStart << "\n";
    aStrm << "Length:           " << mLength << "\n";
    aStrm << "Contents:         ";
    for (size_t i=0; i < mLength && i < 64; ++i) {
        aStrm << mRep->mBuffer[mStart+i];
    }
    aStrm << endl;
}
#else // if !defined (DW_DEBUG_VERSION)
void DwString::PrintDebugInfo(DW_STD ostream& /*aStrm*/) const
{
}
#endif // !defined (DW_DEBUG_VERSION)


void DwString::CheckInvariants() const
{
#if defined (DW_DEBUG_VERSION)
    DW_ASSERT(mRep != 0);
    mRep->CheckInvariants();
#endif // defined (DW_DEBUG_VERSION)
}


DwString operator + (const DwString& aStr1, const DwString& aStr2)
{
    DwString str(aStr1);
    str.append(aStr2);
    return str;
}


DwString operator + (const char* aCstr, const DwString& aStr2)
{
    DwString str(aCstr);
    str.append(aStr2);
    return str;
}


DwString operator + (char aChar, const DwString& aStr2)
{
    DwString str(1, aChar);
    str.append(aStr2);
    return str;
}


DwString operator + (const DwString& aStr1, const char* aCstr)
{
    DwString str(aStr1);
    str.append(aCstr);
    return str;
}


DwString operator + (const DwString& aStr1, char aChar)
{
    DwString str(aStr1);
    str.append(1, aChar);
    return str;
}


DwBool operator == (const DwString& aStr1, const DwString& aStr2)
{
    const char* s1 = aStr1.data();
    size_t len1 = aStr1.length();
    const char* s2 = aStr2.data();
    size_t len2 = aStr2.length();
    int r = dw_strcmp(s1, len1, s2, len2);
    DwBool b = (r == 0) ? DwTrue : DwFalse;
    return b;
}


DwBool operator == (const DwString& aStr1, const char* aCstr)
{
    DW_ASSERT(aCstr != 0);
    const char* s1 = aStr1.data();
    size_t len1 = aStr1.length();
    const char* s2 = aCstr;
    size_t len2 = (aCstr) ? strlen(aCstr) : 0;
    int r = dw_strcmp(s1, len1, s2, len2);
    DwBool b = (r == 0) ? DwTrue : DwFalse;
    return b;
}


DwBool operator == (const char* aCstr, const DwString& aStr2)
{
    DW_ASSERT(aCstr != 0);
    const char* s1 = aCstr;
    size_t len1 = (aCstr) ? strlen(aCstr) : 0;
    const char* s2 = aStr2.data();
    size_t len2 = aStr2.length();
    int r = dw_strcmp(s1, len1, s2, len2);
    DwBool b = (r == 0) ? DwTrue : DwFalse;
    return b;
}


DwBool operator != (const DwString& aStr1, const DwString& aStr2)
{
    const char* s1 = aStr1.data();
    size_t len1 = aStr1.length();
    const char* s2 = aStr2.data();
    size_t len2 = aStr2.length();
    int r = dw_strcmp(s1, len1, s2, len2);
    DwBool b = (r == 0) ? DwFalse : DwTrue;
    return b;
}


DwBool operator != (const DwString& aStr1, const char* aCstr)
{
    DW_ASSERT(aCstr != 0);
    const char* s1 = aStr1.data();
    size_t len1 = aStr1.length();
    const char* s2 = aCstr;
    size_t len2 = (aCstr) ? strlen(aCstr) : 0;
    int r = dw_strcmp(s1, len1, s2, len2);
    DwBool b = (r == 0) ? DwFalse : DwTrue;
    return b;
}


DwBool operator != (const char* aCstr, const DwString& aStr2)
{
    DW_ASSERT(aCstr != 0);
    const char* s1 = aCstr;
    size_t len1 = (aCstr) ? strlen(aCstr) : 0;
    const char* s2 = aStr2.data();
    size_t len2 = aStr2.length();
    int r = dw_strcmp(s1, len1, s2, len2);
    DwBool b = (r == 0) ? DwFalse : DwTrue;
    return b;
}


DwBool operator < (const DwString& aStr1, const DwString& aStr2)
{
    const char* s1 = aStr1.data();
    size_t len1 = aStr1.length();
    const char* s2 = aStr2.data();
    size_t len2 = aStr2.length();
    int r = dw_strcmp(s1, len1, s2, len2);
    DwBool b = (r < 0) ? DwTrue : DwFalse;
    return b;
}


DwBool operator < (const DwString& aStr1, const char* aCstr)
{
    DW_ASSERT(aCstr != 0);
    const char* s1 = aStr1.data();
    size_t len1 = aStr1.length();
    const char* s2 = aCstr;
    size_t len2 = (aCstr) ? strlen(aCstr) : 0;
    int r = dw_strcmp(s1, len1, s2, len2);
    DwBool b = (r < 0) ? DwTrue : DwFalse;
    return b;
}


DwBool operator < (const char* aCstr, const DwString& aStr2)
{
    DW_ASSERT(aCstr != 0);
    const char* s1 = aCstr;
    size_t len1 = (aCstr) ? strlen(aCstr) : 0;
    const char* s2 = aStr2.data();
    size_t len2 = aStr2.length();
    int r = dw_strcmp(s1, len1, s2, len2);
    DwBool b = (r < 0) ? DwTrue : DwFalse;
    return b;
}


DwBool operator > (const DwString& aStr1, const DwString& aStr2)
{
    const char* s1 = aStr1.data();
    size_t len1 = aStr1.length();
    const char* s2 = aStr2.data();
    size_t len2 = aStr2.length();
    int r = dw_strcmp(s1, len1, s2, len2);
    DwBool b = (r > 0) ? DwTrue : DwFalse;
    return b;
}


DwBool operator > (const DwString& aStr1, const char* aCstr)
{
    DW_ASSERT(aCstr != 0);
    const char* s1 = aStr1.data();
    size_t len1 = aStr1.length();
    const char* s2 = aCstr;
    size_t len2 = (aCstr) ? strlen(aCstr) : 0;
    int r = dw_strcmp(s1, len1, s2, len2);
    DwBool b = (r > 0) ? DwTrue : DwFalse;
    return b;
}


DwBool operator > (const char* aCstr, const DwString& aStr2)
{
    DW_ASSERT(aCstr != 0);
    const char* s1 = aCstr;
    size_t len1 = (aCstr) ? strlen(aCstr) : 0;
    const char* s2 = aStr2.data();
    size_t len2 = aStr2.length();
    int r = dw_strcmp(s1, len1, s2, len2);
    DwBool b = (r > 0) ? DwTrue : DwFalse;
    return b;
}


DwBool operator <= (const DwString& aStr1, const DwString& aStr2)
{
    const char* s1 = aStr1.data();
    size_t len1 = aStr1.length();
    const char* s2 = aStr2.data();
    size_t len2 = aStr2.length();
    int r = dw_strcmp(s1, len1, s2, len2);
    DwBool b = (r <= 0) ? DwTrue : DwFalse;
    return b;
}


DwBool operator <= (const DwString& aStr1, const char* aCstr)
{
    DW_ASSERT(aCstr != 0);
    const char* s1 = aStr1.data();
    size_t len1 = aStr1.length();
    const char* s2 = aCstr;
    size_t len2 = (aCstr) ? strlen(aCstr) : 0;
    int r = dw_strcmp(s1, len1, s2, len2);
    DwBool b = (r <= 0) ? DwTrue : DwFalse;
    return b;
}


DwBool operator <= (const char* aCstr, const DwString& aStr2)
{
    DW_ASSERT(aCstr != 0);
    const char* s1 = aCstr;
    size_t len1 = (aCstr) ? strlen(aCstr) : 0;
    const char* s2 = aStr2.data();
    size_t len2 = aStr2.length();
    int r = dw_strcmp(s1, len1, s2, len2);
    DwBool b = (r <= 0) ? DwTrue : DwFalse;
    return b;
}


DwBool operator >= (const DwString& aStr1, const DwString& aStr2)
{
    const char* s1 = aStr1.data();
    size_t len1 = aStr1.length();
    const char* s2 = aStr2.data();
    size_t len2 = aStr2.length();
    int r = dw_strcmp(s1, len1, s2, len2);
    DwBool b = (r >= 0) ? DwTrue : DwFalse;
    return b;
}


DwBool operator >= (const DwString& aStr1, const char* aCstr)
{
    DW_ASSERT(aCstr != 0);
    const char* s1 = aStr1.data();
    size_t len1 = aStr1.length();
    const char* s2 = aCstr;
    size_t len2 = (aCstr) ? strlen(aCstr) : 0;
    int r = dw_strcmp(s1, len1, s2, len2);
    DwBool b = (r >= 0) ? DwTrue : DwFalse;
    return b;
}


DwBool operator >= (const char* aCstr, const DwString& aStr2)
{
    DW_ASSERT(aCstr != 0);
    const char* s1 = aCstr;
    size_t len1 = (aCstr) ? strlen(aCstr) : 0;
    const char* s2 = aStr2.data();
    size_t len2 = aStr2.length();
    int r = dw_strcmp(s1, len1, s2, len2);
    DwBool b = (r >= 0) ? DwTrue : DwFalse;
    return b;
}


DW_STD ostream& operator << (DW_STD ostream& aOstrm, const DwString& aStr)
{
    const char* buf = aStr.data();
    for (size_t i=0; i < aStr.length(); ++i) {
        aOstrm.put(buf[i]);
    }
    return aOstrm;
}


DW_STD istream& getline(DW_STD istream& aIstrm, DwString& aStr, char aDelim)
{
    aStr.clear();
    char ch;
    while (aIstrm.get(ch)) {
        if (ch == aDelim) break;
        if (aStr.length() < aStr.max_size()) {
            aStr.append(1, ch);
        }
    }
    return aIstrm;
}


DW_STD istream& getline(DW_STD istream& aIstrm, DwString& aStr)
{
    return getline(aIstrm, aStr, '\n');
}


int DwStrcasecmp(const DwString& aStr1, const DwString& aStr2)
{
    const char* s1 = aStr1.data();
    size_t len1 = aStr1.length();
    const char* s2 = aStr2.data();
    size_t len2 = aStr2.length();
    return dw_strcasecmp(s1, len1, s2, len2);
}


int DwStrcasecmp(const DwString& aStr, const char* aCstr)
{
    DW_ASSERT(aCstr != 0);
    const char* s1 = aStr.data();
    size_t len1 = aStr.length();
    const char* s2 = aCstr;
    size_t len2 = (aCstr) ? strlen(aCstr) : 0;
    return dw_strcasecmp(s1, len1, s2, len2);
}


int DwStrcasecmp(const char* aCstr, const DwString& aStr)
{
    DW_ASSERT(aCstr != 0);
    const char* s1 = aCstr;
    size_t len1 =  (aCstr) ? strlen(aCstr) : 0;
    const char* s2 = aStr.data();
    size_t len2 = aStr.length();
    return dw_strcasecmp(s1, len1, s2, len2);
}


int DwStrncasecmp(const DwString& aStr1, const DwString& aStr2, size_t n)
{
    const char* s1 = aStr1.data();
    size_t len1 = aStr1.length();
    len1 = DW_MIN(len1, n);
    const char* s2 = aStr2.data();
    size_t len2 = aStr2.length();
    len2 = DW_MIN(len2, n);
    return dw_strcasecmp(s1, len1, s2, len2);
}


int DwStrncasecmp(const DwString& aStr, const char* aCstr, size_t n)
{
    DW_ASSERT(aCstr != 0);
    const char* s1 = aStr.data();
    size_t len1 = aStr.length();
    len1 = DW_MIN(len1, n);
    const char* s2 = aCstr;
    size_t len2 = (aCstr) ? strlen(aCstr) : 0;
    len2 = DW_MIN(len2, n);
    return dw_strcasecmp(s1, len1, s2, len2);
}


int DwStrncasecmp(const char* aCstr, const DwString& aStr, size_t n)
{
    DW_ASSERT(aCstr != 0);
    const char* s1 = aCstr;
    size_t len1 = (aCstr) ? strlen(aCstr) : 0;
    len1 = DW_MIN(len1, n);
    const char* s2 = aStr.data();
    size_t len2 = aStr.length();
    len2 = DW_MIN(len2, n);
    return dw_strcasecmp(s1, len1, s2, len2);
}


int DwStrcmp(const DwString& aStr1, const DwString& aStr2)
{
    const char* s1 = aStr1.data();
    size_t len1 = aStr1.length();
    const char* s2 = aStr2.data();
    size_t len2 = aStr2.length();
    return dw_strcmp(s1, len1, s2, len2);
}


int DwStrcmp(const DwString& aStr, const char* aCstr)
{   
    DW_ASSERT(aCstr != 0);
    const char* s1 = aStr.data();
    size_t len1 = aStr.length();
    const char* s2 = aCstr;
    size_t len2 = (aCstr) ? strlen(aCstr) : 0;
    return dw_strcmp(s1, len1, s2, len2);
}


int DwStrcmp(const char* aCstr, const DwString& aStr)
{
    DW_ASSERT(aCstr != 0);
    const char* s1 = aCstr;
    size_t len1 = (aCstr) ? strlen(aCstr) : 0;
    const char* s2 = aStr.data();
    size_t len2 = aStr.length();
    return dw_strcmp(s1, len1, s2, len2);
}


int DwStrncmp(const DwString& aStr1, const DwString& aStr2, size_t n)
{
    const char* s1 = aStr1.data();
    size_t len1 = aStr1.length();
    len1 = DW_MIN(len1, n);
    const char* s2 = aStr2.data();
    size_t len2 = aStr2.length();
    len2 = DW_MIN(len2, n);
    return dw_strcmp(s1, len1, s2, len2);
}


int DwStrncmp(const DwString& aStr, const char* aCstr, size_t n)
{
    DW_ASSERT(aCstr != 0);
    const char* s1 = aStr.data();
    size_t len1 = aStr.length();
    len1 = DW_MIN(len1, n);
    const char* s2 = aCstr;
    size_t len2 = (aCstr) ? strlen(aCstr) : 0;
    len2 = DW_MIN(len2, n);
    return dw_strcmp(s1, len1, s2, len2);
}


int DwStrncmp(const char* aCstr, const DwString& aStr, size_t n)
{
    DW_ASSERT(aCstr != 0);
    const char* s1 = aCstr;
    size_t len1 = (aCstr) ? strlen(aCstr) : 0;
    len1 = DW_MIN(len1, n);
    const char* s2 = aStr.data();
    size_t len2 = aStr.length();
    len2 = DW_MIN(len2, n);
    return dw_strcmp(s1, len1, s2, len2);
}


void DwStrcpy(DwString& aStrDest, const DwString& aStrSrc)
{
    aStrDest.assign(aStrSrc);
}


void DwStrcpy(DwString& aStrDest, const char* aCstrSrc)
{
    aStrDest.assign(aCstrSrc);
}


void DwStrcpy(char* aCstrDest, const DwString& aStrSrc)
{
    DW_ASSERT(aCstrDest != 0);
    const char* buf = aStrSrc.data();
    size_t len = aStrSrc.length();
    mem_copy(buf, len, aCstrDest);
    aCstrDest[len] = 0;
}


void DwStrncpy(DwString& aStrDest, const DwString& aStrSrc, size_t n)
{
    aStrDest.assign(aStrSrc, 0, n);
}


void DwStrncpy(DwString& aStrDest, const char* aCstrSrc, size_t n)
{
    aStrDest.assign(aCstrSrc, 0, n);
}


void DwStrncpy(char* aCstrDest, const DwString& aStrSrc, size_t n)
{
    DW_ASSERT(aCstrDest != 0);
    const char* buf = aStrSrc.data();
    size_t len = aStrSrc.length();
    len = DW_MIN(len, n);
    mem_copy(buf, len, aCstrDest);
    for (size_t i=len; i < n; ++i) {
        aCstrDest[i] = 0;
    }
}


char* DwStrdup(const DwString& aStr)
{
    size_t len = aStr.length();
    char* buf = new char[len+1];
    DW_ASSERT(buf != 0);
    if (buf != 0) {
        DwStrncpy(buf, aStr, len);
        buf[len] = 0;
    }
    return buf;
}
