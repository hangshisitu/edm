//=============================================================================
// File:       sys_nt.cpp
// Contents:   Definitions for portability functions (Win32 system-dependent)
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

#if defined WIN32 

#include <mimepp/config.h>
#include <mimepp/debug.h>

#include <time.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#if defined(DW_USE_WINSOCK2)
#  include <winsock2.h>
#else
#  include <winsock.h>
#endif

#include <mimepp/system.h>


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------


static CRITICAL_SECTION gSequenceCriticalSection;


DwBool DwInitialize()
{
    // Initialize critical section for shared counter

    InitializeCriticalSection(&gSequenceCriticalSection);

    // Initialize Winsock DLL

    WORD wVersionRequested = MAKEWORD(1, 1);
    WSADATA wsaData;
    int err = WSAStartup(wVersionRequested, &wsaData);
    if (LOBYTE(wsaData.wVersion) == 1 &&
        HIBYTE(wsaData.wVersion) == 1 &&
        err == 0) {
        // okay
    }
    else {
        return DwFalse;
    }
    return DwTrue;
}


void DwFinalize()
{
    // Delete critical section for shared counter

    DeleteCriticalSection(&gSequenceCriticalSection);

    // Release Winsock DLL

    WSACleanup();
}


#ifndef DW_NO_DLL

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason,
    LPVOID lpvReserved)
{
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        DwInitialize();
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        DwFinalize();
        break;
    }
    return TRUE;
}

#endif


DwUint32 DwNextInteger()
{
    static DwUint32 sequenceNumber = 0;
    EnterCriticalSection(&gSequenceCriticalSection);
    ++sequenceNumber;
    DwUint32 n = sequenceNumber;
    LeaveCriticalSection(&gSequenceCriticalSection);
    return n;
}


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
    static int daysInMonth[] = {
        0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    // Get time zone information from operating system

    TIME_ZONE_INFORMATION zoneInfo;
    DWORD result = GetTimeZoneInformation(&zoneInfo);
    if (result == TIME_ZONE_ID_UNKNOWN) {
        *aOffset = -zoneInfo.Bias;
        if (aName != 0) {
            aName[0] = 0;
        }
        return;
    }
    else if (result != TIME_ZONE_ID_DAYLIGHT
        && result != TIME_ZONE_ID_STANDARD) {
        *aOffset = 0;
        if (aName != 0) {
            aName[0] = 0;
        }
        return;
    }

    // Convert Unicode to ANSI

    char standardName[2*sizeof(zoneInfo.StandardName)];
    char daylightName[2*sizeof(zoneInfo.DaylightName)];
    result = WideCharToMultiByte(CP_ACP, 0, zoneInfo.StandardName, -1, 
        standardName, sizeof(standardName), 0, 0);
    result = WideCharToMultiByte(CP_ACP, 0, zoneInfo.DaylightName, -1, 
        daylightName, sizeof(daylightName), 0, 0);
/*
    printf("Bias = %d\n", (int) zoneInfo.Bias);
    printf("StandardName = %s\n", standardName);
    printf("StandardDate = %d  %d/%02d/%02d  ",
        (int) zoneInfo.StandardDate.wDayOfWeek,
        (int) zoneInfo.StandardDate.wYear,
        (int) zoneInfo.StandardDate.wMonth,
        (int) zoneInfo.StandardDate.wDay);
    printf("%02d:%02d:%02d.%03d\n",
        (int) zoneInfo.StandardDate.wHour,
        (int) zoneInfo.StandardDate.wMinute,
        (int) zoneInfo.StandardDate.wSecond,
        (int) zoneInfo.StandardDate.wMilliseconds);
    printf("StandardBias = %d\n", (int) zoneInfo.StandardBias);
    printf("DaylightName = %s\n", daylightName);
    printf("DaylightDate = %d  %d/%02d/%02d  ",
        (int) zoneInfo.DaylightDate.wDayOfWeek,
        (int) zoneInfo.DaylightDate.wYear,
        (int) zoneInfo.DaylightDate.wMonth,
        (int) zoneInfo.DaylightDate.wDay);
    printf("%02d:%02d:%02d.%03d\n",
        (int) zoneInfo.DaylightDate.wHour,
        (int) zoneInfo.DaylightDate.wMinute,
        (int) zoneInfo.DaylightDate.wSecond,
        (int) zoneInfo.DaylightDate.wMilliseconds);
    printf("DaylightBias = %d\n", (int) zoneInfo.DaylightBias);
*/

    // If month is zero, then there is *no* daylight time

    if (zoneInfo.StandardDate.wMonth == 0) {
        *aOffset = -zoneInfo.Bias;
        if (aName != 0) {
            if ((size_t) aNameSize > strlen(standardName)) {
                strncpy(aName, standardName, aNameSize);
            }
            else {
                aName[0] = 0;
            }
        }
    }

    // If month is non-zero, then there *is* daylight time

    else /* if (zoneInfo.StandardDate.wMonth != 0) */ {

        // Convert test time to Y, M, D, H, M, S

        struct tm testTime;
        DwUint32 testTimeScalar = aUnixTime;
        dw_scalar_to_brokendown(testTimeScalar, &testTime);

        // Get daylight time start

        struct tm daylightStart;
        DwUint32 daylightStartScalar;

        // If year is nonzero, then change occurs on absolute date and time.

        if (zoneInfo.DaylightDate.wYear != 0) {
            daylightStart.tm_year = testTime.tm_year;
            daylightStart.tm_mon  = zoneInfo.DaylightDate.wMonth - 1;
            daylightStart.tm_mday = zoneInfo.DaylightDate.wDay;
            daylightStart.tm_hour = zoneInfo.DaylightDate.wHour;
            daylightStart.tm_min  = zoneInfo.DaylightDate.wMinute;
            daylightStart.tm_sec  = zoneInfo.DaylightDate.wSecond;
            daylightStartScalar = dw_brokendown_to_scalar(&daylightStart);
            // Convert to UTC
            daylightStartScalar += 60*(zoneInfo.Bias + zoneInfo.StandardBias);
        }

        // If year is 0, then change occurs on specific day of week.
        // Calculate exact date and time for target year.

        else /* if (zoneInfo.DaylightDate.wYear == 0) */ {
            int year = testTime.tm_year + 1900;
            int month = zoneInfo.DaylightDate.wMonth;
            int day;
            if (zoneInfo.DaylightDate.wDay < 5) {
                day = 1;
            }
            else if (zoneInfo.DaylightDate.wDay == 5) {
                day = daysInMonth[month];
                // Feb 29 in a leap year
                if (month == 2 && year%4 == 0) {
                    if (year%400 == 0) {
                        day = 29;
                    }
                    else if (year%100 == 0) {
                    }
                    else {
                        day = 29;
                    }
                }
            }
            int jdn = dw_ymd_to_jdn(year, month, day);
            int dayOfWeek = (jdn + 1) % 7;
            if (zoneInfo.DaylightDate.wDay < 5) {
                int count = 0;
                while (day <= daysInMonth[month]) {
                    if (dayOfWeek == zoneInfo.DaylightDate.wDayOfWeek) {
                        ++count;
                    }
                    if (count == zoneInfo.DaylightDate.wDay) {
                        break;
                    }
                    ++day;
                    dayOfWeek = (dayOfWeek + 1) % 7;
                }
            }
            else if (zoneInfo.DaylightDate.wDay == 5) {
                while (day > 0) {
                    if (dayOfWeek == zoneInfo.DaylightDate.wDayOfWeek) {
                        break;
                    }
                    --day;
                    dayOfWeek = (dayOfWeek + 6) % 7;
                }
            }
            daylightStart.tm_year = year - 1900;
            daylightStart.tm_mon = month - 1;
            daylightStart.tm_mday = day;
            daylightStart.tm_hour = zoneInfo.DaylightDate.wHour;
            daylightStart.tm_min = zoneInfo.DaylightDate.wMinute;
            daylightStart.tm_sec = zoneInfo.DaylightDate.wSecond;
            daylightStartScalar = dw_brokendown_to_scalar(&daylightStart);
            // Convert to UTC
            daylightStartScalar += 60*(zoneInfo.Bias + zoneInfo.StandardBias);
        }

        // Get daylight time end

        struct tm daylightEnd;
        DwUint32 daylightEndScalar;

        // If year is nonzero, then change occurs on absolute date and time.

        if (zoneInfo.StandardDate.wYear != 0) {
            daylightEnd.tm_year = testTime.tm_year;
            daylightEnd.tm_mon  = zoneInfo.DaylightDate.wMonth - 1;
            daylightEnd.tm_mday = zoneInfo.DaylightDate.wDay;
            daylightEnd.tm_hour = zoneInfo.DaylightDate.wHour;
            daylightEnd.tm_min  = zoneInfo.DaylightDate.wMinute;
            daylightEnd.tm_sec  = zoneInfo.DaylightDate.wSecond;
            daylightEndScalar = dw_brokendown_to_scalar(&daylightStart);
            // Convert to UTC
            daylightEndScalar += 60*(zoneInfo.Bias + zoneInfo.DaylightBias);
        }

        // If year is 0, then change occurs on specific day of week.
        // Calculate exact date and time for target year.

        else /* if (zoneInfo.StandardDate.wYear == 0) */ {
            int year = testTime.tm_year + 1900;
            int month = zoneInfo.StandardDate.wMonth;
            int day;
            if (zoneInfo.StandardDate.wDay < 5) {
                day = 1;
            }
            else if (zoneInfo.StandardDate.wDay == 5) {
                day = daysInMonth[month];
                // Feb 29 in a leap year
                if (month == 2 && year%4 == 0) {
                    if (year%400 == 0) {
                        day = 29;
                    }
                    else if (year%100 == 0) {
                    }
                    else {
                        day = 29;
                    }
                }
            }
            int jdn = dw_ymd_to_jdn(year, month, day);
            int dayOfWeek = (jdn + 1) % 7;
            if (zoneInfo.StandardDate.wDay < 5) {
                int count = 0;
                while (day <= daysInMonth[month]) {
                    if (dayOfWeek == zoneInfo.StandardDate.wDayOfWeek) {
                        ++count;
                    }
                    if (count == zoneInfo.StandardDate.wDay) {
                        break;
                    }
                    ++day;
                    dayOfWeek = (dayOfWeek + 1) % 7;
                }
            }
            else if (zoneInfo.StandardDate.wDay == 5) {
                while (day > 0) {
                    if (dayOfWeek == zoneInfo.StandardDate.wDayOfWeek) {
                        break;
                    }
                    --day;
                    dayOfWeek = (dayOfWeek + 6) % 7;
                }
            }
            daylightEnd.tm_year = year - 1900;
            daylightEnd.tm_mon = month - 1;
            daylightEnd.tm_mday = day;
            daylightEnd.tm_hour = zoneInfo.StandardDate.wHour;
            daylightEnd.tm_min = zoneInfo.StandardDate.wMinute;
            daylightEnd.tm_sec = zoneInfo.StandardDate.wSecond;
            daylightEndScalar = dw_brokendown_to_scalar(&daylightEnd);
            // Convert to UTC
            daylightEndScalar += 60*(zoneInfo.Bias + zoneInfo.DaylightBias);
        }

        // Northern hemisphere -- change to daylight time comes earlier in year
        // than change to standard time

        DwBool isDaylightTime;
        if (daylightStartScalar <= daylightEndScalar) {
            if (daylightStartScalar <= testTimeScalar
                && testTimeScalar < daylightEndScalar) {
                isDaylightTime = DwTrue;
            }
            else {
                isDaylightTime = DwFalse;
            }
        }

        // Southern hemisphere -- change to daylight time comes later in year
        // than change to standard time

        else /* if (daylightStartScalar > daylightEndScalar) */ {
            if (daylightEndScalar < testTimeScalar
                && testTimeScalar <= daylightStartScalar) {
                isDaylightTime = DwFalse;
            }
            else {
                isDaylightTime = DwTrue;
            }
        }

        if (isDaylightTime) {
            *aOffset = -zoneInfo.Bias - zoneInfo.DaylightBias;
            if (aName != 0) {
                if ((size_t) aNameSize > strlen(daylightName)) {
                    strcpy(aName, daylightName);
                }
                else {
                    aName[0] = 0;
                }
            }
        }
        else /* if (! isDaylightTime) */ {
            *aOffset = -zoneInfo.Bias - zoneInfo.StandardBias;
            if (aName != 0) {
                if ((size_t) aNameSize > strlen(standardName)) {
                    strcpy(aName, standardName);
                }
                else {
                    aName[0] = 0;
                }
            }
        }
    }
}


#ifdef DW_OLD_VERSION
void DwGetTimeZoneInfo(DwInt32 aUnixTime, DwInt32 *aOffset,
    char *aName, DwInt32 aNameSize)
{
    // Old version of the implementation (version 1.1.1) that uses
    // localtime() and gmtime() from the standard C library.
    //
    // Try this old implementation if you have problems with the new
    // implementation.  However, you should know that the localtime()
    // library function in VC++ 5 has a bug.  Many daylight time periods
    // begin or end on the n-th Sunday of the month.  If the first day
    // of that month is Sunday, then localtime() will get it wrong and
    // will do the change on the (n+1)-th Sunday of the month.  For example,
    // the change from EST to EDT occurs on the first Sunday of April. In
    // 1990, the date is Sun, 1 Apr, but localtime() gets it as Sun,
    // 8 Apr, which is the second Sunday in April.
    //
    // localtime() and gmtime() are supposed to be multithread safe on
    // Windows.  They are not, in general, thread safe on Unix.

    struct tm local;
    time_t t;
    DwUint32 utcScalar, localScalar;
    int offsetSecs;

    // Compute the local time as a scalar time. Use this scalar time
    // to compute the time zone offset.

    t = DwUnixTimeToCalendarTime(aUnixTime);
    local = *localtime(&t);
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
#endif // DW_OLD_VERSION


void DwGetFullyQualifiedDomainName(char* aBuf, int aBufSize)
{
    DW_ASSERT(aBuf != 0);
    if (aBuf != 0 && aBufSize > 0) {
        aBuf[0] = 0;
        DwBool gotIt = DwFalse;
        int err = gethostname(aBuf, aBufSize);
        if (! err) {
            struct hostent* he = gethostbyname(aBuf);
            if (he != 0) {
                strncpy(aBuf, he->h_name, aBufSize-1);
                aBuf[aBufSize-1] = 0;
                gotIt = DwTrue;
            }
        }
        if (! gotIt) {
            const int bufferSize = MAX_COMPUTERNAME_LENGTH+1;
            TCHAR buffer[bufferSize];
            DWORD s = bufferSize;
            BOOL ok;
            ok = GetComputerName(buffer, &s);
            if (ok) {
                strncpy(aBuf, buffer, aBufSize-1);
                aBuf[aBufSize-1] = 0;
            }
            else {
                strncpy(aBuf, "noname", aBufSize-1);
                aBuf[aBufSize-1] = 0;
            }
        }
    }
}


DwUint32 DwGetThreadId()
{
    return (DwUint32) GetCurrentThreadId();
}


int DwGetHostByName(const char* aHostname, char* aCanonicalName,
    int aCanonicalNameSize, DwUint32* aAddresses, int aAddressesSize,
    int* aErr)
{
    if (aCanonicalNameSize > 0) {
        aCanonicalName[0] = 0;
    }
    *aErr = 0;
    struct hostent* hostentp = gethostbyname(aHostname);
    // Make sure we got back a response, and that the response contains
    // IPv4 addresses
    if (hostentp == 0 || hostentp->h_addrtype != AF_INET ||
        hostentp->h_length != 4) {
        *aErr = WSAGetLastError();
        return 0;
    }
    if (aCanonicalNameSize > 0) {
        strncpy(aCanonicalName, hostentp->h_name, aCanonicalNameSize-1);
        aCanonicalName[aCanonicalNameSize-1] = 0;
    }
    int numAddresses = 0;
    while (numAddresses < aAddressesSize
           && hostentp->h_addr_list[numAddresses] != 0) {

        memcpy(&aAddresses[numAddresses],
               hostentp->h_addr_list[numAddresses], 4);
        ++numAddresses;
    }
    return numAddresses;
}


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
