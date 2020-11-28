//=============================================================================
// File:       system.h
// Contents:   Declarations for portability functions
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

#ifndef DW_SYSTEM_H
#define DW_SYSTEM_H

#ifndef DW_CONFIG_H
#include <mimepp/config.h>
#endif

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*\
 * Public functions
\*--------------------------------------------------------------------------*/

/*
 * Returns next integer in sequence.  The first integer may be arbitrary,
 * although it is probably best seed by a system time scalar value.  Wrap
 * around may occur.
 */
DwUint32 DwNextInteger();

/*
 * Convert a native time_t value as returned from the ANSI C time() function
 * to the number of seconds since 1 Jan 1970 00:00:00 UTC.  On Unix systems,
 * the input and output are the same.
 */
DwUint32 DW_CORE_EXPORT DwCalendarTimeToUnixTime(time_t aCalendarTime);


/*
 * Inverse function to DwCalendarTimeToUnixTime()
 */
time_t DW_CORE_EXPORT DwUnixTimeToCalendarTime(DwUint32 aUnixTime);

/*
 * For the given value of aUnixTime, return the time zone offset and time zone
 * name.  aUnixTime specifies a particular point in time, as the number
 * of elapsed seconds since 1 Jan 1970 00:00:00 UTC.  aOffset is the number
 * of minutes offset from UTC (time zones west of the prime meridian are
 * negative).  aName is the name of the time zone, if available.  aNameSize
 * is the size of the buffer pointed to by aName.
 *
 * As an example, if aUnixTime represents a date in the summer, and the time
 * zone is that of Washington, DC, the returned values would be -240 (number
 * of minutes offset from UTC) and "EDT" (Eastern Daylight Time).
 */
void DW_CORE_EXPORT DwGetTimeZoneInfo(DwInt32 aUnixTime, DwInt32* aOffset,
    char* aName, DwInt32 aNameSize);

/*
 * Returns the thread or process id.  This function is necessary to create
 * a unique message id in a multiprocessing environment.  The unique
 * message id is created by combining the current date, time, and thread
 * id.
 *
 * On the Win32 platform, this function calls GetCurrentThreadId().
 *
 * On the Unix platform, this function calls getpid().  This may need to
 * be modified if the Unix application is to use the library in multiple
 * threads in the same process.
 */
DwUint32 DW_CORE_EXPORT DwGetThreadId();

/*
 * Returns the fully qualified domain name, e.g. frodo.xyz.com
 */
void DW_CORE_EXPORT DwGetFullyQualifiedDomainName(char* aBuf, int aBufSize);

/*
 * Gets canonical name and IP addresses for given host name.  Return
 * value is number of addresses returned.  Addresses are network byte
 * order (big endian) 32-bit integers containing IPv4 addresses.
 */
int DW_CORE_EXPORT DwGetHostByName(
    const char* aHostname,   // (input)  name of host we want
    char* aCanonicalName,    // (output) canonical name for this host
    int aCanonicalNameSize,  // (input)  size of buffer for canonical name
    DwUint32* aAddresses,    // (output) IP addresses in network byte order
    int aAddressesSize,      // (input)  length of aAddresses array
    int* aErr);              // (output) error code (zero if no error)

/*--------------------------------------------------------------------------*\
 * Advanced functions
\*--------------------------------------------------------------------------*/
    
DwInt32 DW_CORE_EXPORT dw_ymd_to_jdn(DwInt32 aYear, DwInt32 aMonth,
    DwInt32 aDay);
void DW_CORE_EXPORT dw_jdn_to_ymd(DwInt32 aJdn, DwInt32 *aYear,
    DwInt32 *aMonth, DwInt32 *aDay);
DwUint32 DW_CORE_EXPORT dw_brokendown_to_scalar(struct tm* aBrokenDownTime);
void DW_CORE_EXPORT dw_scalar_to_brokendown(DwUint32 aCalTime,
    struct tm *aBrokenDownTime);
DwUint32 dw_rand(DwUint32* aSeed);
    
#ifdef __cplusplus
}
#endif

#endif
