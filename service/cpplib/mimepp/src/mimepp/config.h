//=============================================================================
// File:       config.h
// Contents:   Declarations of macros for configuring the library
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

#ifndef DW_CONFIG_H
#define DW_CONFIG_H

//-----------------------------------------------------------------------------
// Platform
//
// Make sure that the following lines define either DW_UNIX or DW_WIN32.
//-----------------------------------------------------------------------------

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#   define DW_WIN32
#endif

#if defined(__unix__) || defined(__unix) || defined(_AIX)
#   define DW_UNIX
#endif

#if defined(DW_WIN32) && defined(DW_UNIX)
#   error "Both DW_WIN32 and DW_UNIX #defined"
#   include "stop_on_error.h"
#endif

#if !defined(DW_WIN32) && !defined(DW_UNIX)
#   error "Must #define DW_WIN32 or DW_UNIX"
#   include "stop_on_error.h"
#endif

//-----------------------------------------------------------------------------
// End of line characters
//
// Uncomment one of the following to indicate whether you want the library to
// use LF or CR LF as the end of line characters.
//
// I strongly recommend using LF ('\n') alone as the end of line character,
// since that is the normal end of line character for C and C++ libraries.
// Then you can do the conversion to and from the CR LF end of line
// characters at the interface to the transport system.
//-----------------------------------------------------------------------------

//#define DW_EOL_LF
#define DW_EOL_CRLF
// notice: After change this define, check the value of 'MY_EOL' in maillib\decheader.cpp!!

#if defined(DW_EOL_CRLF)
#   define DW_EOL  "\r\n"
#elif defined(DW_EOL_LF)
#   define DW_EOL  "\n"
#else
#   error "Must define DW_EOL_CRLF, DW_EOL_LF"
#   include "stop_on_error.h"
#endif

//-----------------------------------------------------------------------------
// C++ namespaces
//
// Uncomment the following line if you want the DwMime namespace to be defined
// for enums.  If the namespace is not defined, then enums are specified as
// part of a DwMime class.
//-----------------------------------------------------------------------------

//#define DW_USE_NAMESPACES


//-----------------------------------------------------------------------------
// ANSI C++ streams library
//
// Define the following if you want to use the new ANSI C++ streams.
// Don't define it if you want the old cfront-compatible streams.
//-----------------------------------------------------------------------------

#define DW_USE_ANSI_IOSTREAM

#if defined(DW_USE_ANSI_IOSTREAM)
#   define DW_STD  std::
#else
#   define DW_STD
#endif


//-----------------------------------------------------------------------------
// C++ exceptions
//
// Define the following if you want to use exceptions.  If you don't use
// exceptions, the library should recover from resource exhaustion (such as
// out of memory), library function errors (such as errors writing to streams),
// and other exceptional conditions.  However, the library may terminate
// in cases of programmer error, such as if you request the element of a list
// and your index is out of range.
//
// Note that you may need to make a change in the makefile, too, to enable
// or disable exceptions.
//-----------------------------------------------------------------------------

#define DW_USE_EXCEPTIONS


//-----------------------------------------------------------------------------
// Windows sockets (Win32 only)
//
// Define the following if you want to use Winsock 2 instead of Winsock 1.1.
//-----------------------------------------------------------------------------

#define DW_USE_WINSOCK2


//-----------------------------------------------------------------------------
// bool type
//
// Uncomment the following line if you want DwBool typedef-ed to int instead
// of bool.
//-----------------------------------------------------------------------------

#define DW_NO_BOOL

#if defined(DW_NO_BOOL)
    typedef int     DwBool;
#   define DwFalse  0
#   define DwTrue   1
#else
    typedef bool    DwBool;
#   define DwFalse  false
#   define DwTrue   true
#endif


//-----------------------------------------------------------------------------
// DLL vs static library (Win32 only)
//
// Uncomment out the following line to create a static library instead of
// a DLL.
//-----------------------------------------------------------------------------

#define DW_NO_DLL

// Note: If you want the library to be in one DLL instead of three, then
// change the following #define's to look like this:
// #if defined(DW_WIN32) && !defined(DW_NO_DLL)
// #   if defined(DW_CORE_IMPLEMENTATION)
// #      define DW_CORE_EXPORT __declspec(dllexport)
// #      define DW_UTIL_EXPORT __declspec(dllexport)
// #      define DW_NET_EXPORT __declspec(dllexport)
// #   elif defined(DW_UTIL_IMPLEMENTATION)
// #      define DW_CORE_EXPORT __declspec(dllexport)
// #      define DW_UTIL_EXPORT __declspec(dllexport)
// #      define DW_NET_EXPORT __declspec(dllexport)
// #   elif defined(DW_NET_IMPLEMENTATION)
// #      define DW_CORE_EXPORT __declspec(dllexport)
// #      define DW_UTIL_EXPORT __declspec(dllexport)
// #      define DW_NET_EXPORT __declspec(dllexport)
// #   else
// #      define DW_CORE_EXPORT __declspec(dllimport)
// #      define DW_UTIL_EXPORT __declspec(dllimport)
// #      define DW_NET_EXPORT __declspec(dllimport)
// #   endif
// #else
// ...

#if defined(DW_WIN32) && !defined(DW_NO_DLL)
#   ifdef DW_CORE_IMPLEMENTATION
#      define DW_CORE_EXPORT __declspec(dllexport)
#   else
#      define DW_CORE_EXPORT __declspec(dllimport)
#   endif
#   ifdef DW_UTIL_IMPLEMENTATION
#      define DW_UTIL_EXPORT __declspec(dllexport)
#   else
#      define DW_UTIL_EXPORT __declspec(dllimport)
#   endif
#   ifdef DW_NET_IMPLEMENTATION
#      define DW_NET_EXPORT __declspec(dllexport)
#   else
#      define DW_NET_EXPORT __declspec(dllimport)
#   endif
#else
#   define DW_CORE_EXPORT
#   define DW_UTIL_EXPORT
#   define DW_NET_EXPORT
#endif


//-----------------------------------------------------------------------------
// Thread safety -- enables mutexes and other synchronization mechanisms
//
// Please see the FAQ about multithreading issues
//-----------------------------------------------------------------------------

#define DW_MULTITHREADED


//-----------------------------------------------------------------------------
// Type definitions
//
// Make sure the following types are accurate for your machine.
//-----------------------------------------------------------------------------

#if defined(__BCPLUSPLUS__) && !defined(__WIN32__)
#   define DW_STD_16_BIT
#endif

#if defined(__alpha) || defined(__sgi)
#   define DW_STD_64_BIT
#endif

#if !defined(DW_STD_16_BIT) && !defined(DW_STD_64_BIT)
#   define DW_STD_32_BIT
#endif

typedef char           DwChar7;  // type for ASCII characters
typedef unsigned char  DwChar8;  // type for 8-bit characters
typedef unsigned char  DwUint8;  // type for 8-bit unsigned integers

#if defined(DW_STD_16_BIT)
typedef int            DwInt16;  // 16-bit signed integers
typedef unsigned int   DwUint16; // 16-bit unsigned integers
typedef long           DwInt32;  // 32-bit signed integers
typedef unsigned long  DwUint32; // 32-bit unsigned integers
#elif defined(DW_STD_32_BIT)
typedef short          DwInt16;
typedef unsigned short DwUint16;
typedef int            DwInt32;
typedef unsigned int   DwUint32;
#elif defined(DW_STD_64_BIT)
typedef short          DwInt16;
typedef unsigned short DwUint16;
typedef int            DwInt32;
typedef unsigned int   DwUint32;
#endif

#endif