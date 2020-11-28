//=============================================================================
// File:       boyermor.h
// Contents:   Declarations for DwBoyerMoore
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

#ifndef DW_BOYERMOR_H
#define DW_BOYERMOR_H

#ifndef DW_CONFIG_H
#include <mimepp/config.h>
#endif

#ifndef DW_STRING_H
#include <mimepp/string.h>
#endif


//=============================================================================
//+ Name DwBoyerMoore -- Class for executing Boyer-Moore string search algorithm
//+ Description
//. <var>DwBoyerMoore</var> implements the Boyer-Moore algorithm for searching
//. for a string.  The Boyer-Moore algorithm is fast, but requires a bit
//. of start-up overhead compared to a brute force algorithm.
//=============================================================================
// Last modified 1998-04-28
//+ Noentry ~DwBoyerMoore


class DW_CORE_EXPORT DwBoyerMoore {

public:
    
    DwBoyerMoore(const char* aCstr);
    DwBoyerMoore(const DwString& aStr);
    //. Constructs a <var>DwBoyerMoore</var> object for searching for
    //. a particular string.

    virtual ~DwBoyerMoore();

    void Assign(const char* aCstr);
    void Assign(const DwString& aStr);
    //. Sets the string to search for.

    size_t FindIn(const DwString& aStr, size_t aPos);
    //. Searches for the search string in <var>aStr</var> starting at position
    //. <var>aPos</var>.  If found, the function returns the first position in
    //. <var>aStr</var> where the search string was found.  If not found, the
    //. function returns <var>DwString::npos</var>.

private:

    size_t mPatLen;
    char* mPat;
    unsigned char mSkipAmt[256];

    void _Assign(const char* aPat, size_t aPatLen);
};

#endif
