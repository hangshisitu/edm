//=============================================================================
// File:       sys_un.cpp
// Contents:   Definitions for portability functions (Unix system-dependent)
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

#if !defined WIN32

#include <mimepp/config.h>
#include <mimepp/debug.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#if defined(__SUNPRO_CC)
#  include <sysent.h>
#endif // defined(__SUNPRO_CC)
#if defined(DW_MULTITHREADED)
#  include <pthread.h>
#endif

#include <mimepp/system.h>


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------


DwBool DwInitialize()
{
    return DwTrue;
}


void DwFinalize()
{
}


#if defined(DW_MULTITHREADED)

DwUint32 DwNextInteger()
{
    static pthread_mutex_t sequenceMutex = PTHREAD_MUTEX_INITIALIZER;
    static DwUint32 sequenceNumber = 0;
    pthread_mutex_lock(&sequenceMutex);
    ++sequenceNumber;
    DwUint32 n = sequenceNumber;
    pthread_mutex_unlock(&sequenceMutex);
    return n;
}

#else

DwUint32 DwNextInteger()
{
    static DwUint32 sequenceNumber = 0;
    ++sequenceNumber;
    return sequenceNumber;
}

#endif


DwUint32 DwCalendarTimeToUnixTime(time_t aCalendarTime)
{
    return (DwUint32) aCalendarTime;
}


time_t DwUnixTimeToCalendarTime(DwUint32 aUnixTime)
{
    return (time_t) aUnixTime;
}


void DwGetTimeZoneInfo(DwInt32 aUnixTime, DwInt32 *aOffset,
    char *aName, DwInt32 aNameSize)
{
    struct tm local;
    time_t t;
    DwUint32 utcScalar, localScalar;
    int offsetSecs;

    // Compute the local time as a scalar time. Use this scalar time
    // to compute the time zone offset.

    t = (time_t) aUnixTime;
#ifdef DW_MULTITHREADED
    localtime_r(&t, &local);
#else
    local = *localtime(&t);
#endif
    utcScalar = aUnixTime;
    localScalar = (DwUint32) dw_brokendown_to_scalar(&local);
    if (localScalar <= utcScalar) {
        offsetSecs = utcScalar - localScalar;
        offsetSecs = -offsetSecs;
    }
    else /* if (localScalar > utcScalar) */ {
        offsetSecs = localScalar - utcScalar;
    }
    if (aOffset != 0) {
        *aOffset = offsetSecs/60;
    }
    if (aName != 0 && aNameSize > 0) {
        strftime(aName, (size_t) aNameSize, "%Z", &local);
    }
}


void DwGetFullyQualifiedDomainName(char* aBuf, int aBufSize)
{
    DW_ASSERT(aBuf != 0);
    if (aBuf == 0) {
        return;
    }
    aBuf[0] = 0;
    int err = gethostname(aBuf, aBufSize);
    if (! err) {
        char buf[200];
        DwGetHostByName(aBuf, buf, 200, 0, 0, &err);
        if (!err && buf[0] != 0) {
            strncpy(aBuf, buf, aBufSize);
            aBuf[aBufSize-1] = 0;
        }
    }
}


DwUint32 DwGetThreadId()
{
    return (DwUint32) getpid();
}


// Note: DwGetHostByName() exists because the traditional gethostybname()
// function uses static data, which could be a problem in a multithreaded
// application.  Different UNIX OSes handle this situation differently.
// Some OSes define gethostbyname_r(), which uses no static data.  This
// function is not standard, however, and is different in different OSes
// (for example, Linux and Solaris).  Some OSes decided to implement
// gethostbyname() using thread-specific data, which makes it thread-safe
// but not reentrant.  The OSes that I know about that implement
// gethostbyname() this way include HP/UX 11.0, AIX 4.3, and Compaq
// Tru64 Unix 5.0.

#if defined(__linux__)

int DwGetHostByName(const char* aHostname, char* aCanonicalName,
    int aCanonicalNameSize, DwUint32* aAddresses, int aAddressesSize,
    int* aErr)
{
    *aErr = 0;
    int numAddresses = 0;
    if (aCanonicalNameSize > 0) {
        aCanonicalName[0] = 0;
    }
    int bufferSize = 2000;
    char* buffer = (char*) malloc((size_t) bufferSize);
    if (buffer != 0) {
        struct hostent xhostent;
        struct hostent* phostent = 0;
        gethostbyname_r(aHostname, &xhostent, buffer, bufferSize,
            &phostent, aErr);
        if (phostent != 0 && phostent->h_addrtype == AF_INET) {
            if (aCanonicalNameSize > 0) {
                strncpy(aCanonicalName, phostent->h_name,
                    aCanonicalNameSize-1);
                aCanonicalName[aCanonicalNameSize-1] = 0;
            }
            while (numAddresses < aAddressesSize
                && phostent->h_addr_list[numAddresses] != 0) {

                memcpy(&aAddresses[numAddresses],
                    phostent->h_addr_list[numAddresses], 4);
                ++numAddresses;
            }
        }
        free(buffer);
    }
    return numAddresses;
}

#elif defined(__sun__)

int DwGetHostByName(const char* aHostname, char* aCanonicalName,
    int aCanonicalNameSize, DwUint32* aAddresses, int aAddressesSize,
    int* aErr)
{
    *aErr = 0;
    int numAddresses = 0;
    if (aCanonicalNameSize > 0) {
        aCanonicalName[0] = 0;
    }
    int bufferSize = 2000;
    char* buffer = (char*) malloc((size_t) bufferSize);
    if (buffer != 0) {
        struct hostent xhostent;
        struct hostent* phostent =
            gethostbyname_r(aHostname, &xhostent, buffer, bufferSize, aErr);
        if (phostent != 0 && phostent->h_addrtype == AF_INET) {
            if (aCanonicalNameSize > 0) {
                strncpy(aCanonicalName, phostent->h_name,
                    aCanonicalNameSize-1);
                aCanonicalName[aCanonicalNameSize-1] = 0;
            }
            while (numAddresses < aAddressesSize
                && phostent->h_addr_list[numAddresses] != 0) {

                memcpy(&aAddresses[numAddresses],
                    phostent->h_addr_list[numAddresses], 4);
                ++numAddresses;
            }
        }
        free(buffer);
    }
    return numAddresses;
}

#else

int DwGetHostByName(const char* aHostname, char* aCanonicalName,
    int aCanonicalNameSize, DwUint32* aAddresses, int aAddressesSize,
    int* aErr)
{
    *aErr = 0;
    int numAddresses = 0;
    if (aCanonicalNameSize > 0) {
        aCanonicalName[0] = 0;
    }
    struct hostent* phostent = gethostbyname(aHostname);
    if (phostent != 0 && phostent->h_addrtype == AF_INET) {
        if (aCanonicalNameSize > 0) {
            strncpy(aCanonicalName, phostent->h_name, aCanonicalNameSize-1);
            aCanonicalName[aCanonicalNameSize-1] = 0;
        }
        while (numAddresses < aAddressesSize
            && phostent->h_addr_list[numAddresses] != 0) {

            memcpy(&aAddresses[numAddresses],
                phostent->h_addr_list[numAddresses], 4);
            ++numAddresses;
        }
    }
    else {
        *aErr = h_errno;
    }
    return numAddresses;
}

#endif


//-----------------------------------------------------------------------------
// Advanced functions
//-----------------------------------------------------------------------------

DwInt32 dw_ymd_to_jdn(DwInt32 aYear, DwInt32 aMonth, DwInt32 aDay)
{
    DwInt32 jdn;

    if (aYear < 0) {
        aYear++;
    }
    jdn = (aDay - 32075)
        + 1461 * (aYear + 4800 + (aMonth - 14) / 12) / 4
        + 367 * (aMonth - 2 - (aMonth - 14) / 12 * 12) / 12
        - 3 * ((aYear + 4900 + (aMonth - 14) / 12) / 100) / 4;
    return jdn;
}


void dw_jdn_to_ymd(DwInt32 aJdn, DwInt32 *aYear, DwInt32 *aMonth,
    DwInt32 *aDay)
{
    DwInt32 x, z, m, d, y;
    DwInt32 daysPer400Years = (DwInt32) 146097L;
    DwInt32 fudgedDaysPer4000Years = (DwInt32) 1460970L + 31;

    x = aJdn + (DwInt32) 68569L;
    z = 4 * x / daysPer400Years;
    x = x - (daysPer400Years * z + 3) / 4;
    y = 4000 * (x + 1) / fudgedDaysPer4000Years;
    x = x - 1461 * y / 4 + 31;
    m = 80 * x / 2447;
    d = x - 2447 * m / 80;
    x = m / 11;
    m = m + 2 - 12 * x;
    y = 100 * (z - 49) + y + x;
    if (aYear != 0) {
        *aYear = (y <= 0) ? (y - 1) : y;
    }
    if (aMonth != 0) {
        *aMonth = m;
    }
    if (aDay != 0) {
        *aDay = d;
    }
}


#define JDN_JAN_1_1970  ((DwInt32) 2440588L)

DwUint32 dw_brokendown_to_scalar(struct tm* aBrokenDownTime)
{
    DwInt32 jdn;
    DwUint32 scalarTime;

    jdn = dw_ymd_to_jdn(
        aBrokenDownTime->tm_year+1900,
        aBrokenDownTime->tm_mon+1,
        aBrokenDownTime->tm_mday);
    scalarTime = jdn - JDN_JAN_1_1970;
    scalarTime = 24*scalarTime + aBrokenDownTime->tm_hour;
    scalarTime = 60*scalarTime + aBrokenDownTime->tm_min;
    scalarTime = 60*scalarTime + aBrokenDownTime->tm_sec;
    return scalarTime;
}


void dw_scalar_to_brokendown(DwUint32 aScalarTime, struct tm *aBrokenDownTime)
{
    DwUint32 t, days, secs;
    int year, mon, day;
    DwInt32 jdn, jan1;

    if (aBrokenDownTime == 0) {
        return;
    }
    t = aScalarTime;
    days = t / (DwInt32) 86400L;
    secs = t % (DwInt32) 86400L;
    jdn = JDN_JAN_1_1970 + days;
    dw_jdn_to_ymd(jdn, &year, &mon, &day);
    jan1 = dw_ymd_to_jdn(year, 1, 1);
    aBrokenDownTime->tm_sec   = (int) ( secs         % 60);
    aBrokenDownTime->tm_min   = (int) ((secs /   60) % 60);
    aBrokenDownTime->tm_hour  = (int) ((secs / 3600) % 24);
    aBrokenDownTime->tm_mday  = day;
    aBrokenDownTime->tm_mon   = mon - 1;
    aBrokenDownTime->tm_year  = year - 1900;
    aBrokenDownTime->tm_wday  = (int) ((jdn+1)%7);
    aBrokenDownTime->tm_yday  = jdn - jan1;
}


DwUint32 dw_rand(DwUint32* aSeed)
{
    DwUint32 lo, hi, r;

    lo = *aSeed & 0xffff;
    hi = (*aSeed >> 16) & 0xffff;
    lo *= 16807;
    hi *= 16807;
    r = lo + ((hi & 0x7fff) << 16);
    if (r > 0x7fffffff) {
        r &= 0x7fffffff;
        ++r;
    }
    r += hi >> 15;
    if (r > 0x7fffffff) {
        r &= 0x7fffffff;
        ++r;
    }
    *aSeed = r;
    return r;
}

#endif
