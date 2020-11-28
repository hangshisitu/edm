//=============================================================================
// File:       except.cpp
// Contents:   Definitions for DwException
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
#include <mimepp/except.h>


DwException::DwException()
{
    mMessage = "Unspecified error";
}


DwException::DwException(const DwException& aException)
{
    mMessage = aException.mMessage;
}


DwException::~DwException()
{
}


DwException& DwException::operator = (const DwException& aException)
{
    if (this == &aException) return *this;
    mMessage = aException.mMessage;
    return *this;
}


const char* DwException::Message() const
{
    return mMessage;
}
