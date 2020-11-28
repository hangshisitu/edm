//=============================================================================
// File:       datetime.cpp
// Contents:   Definitions for DwDateTime
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
#include <stdio.h>
#include <string.h>
#ifdef DW_USE_ANSI_IOSTREAM
#  include <iostream>
#else
#  include <iostream.h>
#endif

#include <mimepp/string.h>
#include <mimepp/datetime.h>
#include <mimepp/token.h>
#include <mimepp/system.h>

static char lWeekDay[7][4]
    = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
static char lMonth[12][4]
    = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
       "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

extern "C" int ParseRfc822Date(const char *str, struct tm *tms, int *z,
    char* name, int nameSize);


const char* const DwDateTime::sClassName = "DwDateTime";


DwDateTime* (*DwDateTime::sNewDateTime)(const DwString&,
    DwMessageComponent*) = 0;


DwDateTime* DwDateTime::NewDateTime(const DwString& aStr,
    DwMessageComponent* aParent)
{
    if (sNewDateTime != 0) {
        return sNewDateTime(aStr, aParent);
    }
    else {
        return new DwDateTime(aStr, aParent);
    }
}


DwDateTime::DwDateTime(DwBool aConvertToLocal)
{
    Init();
    time_t cal = time(0);
    _FromCalendarTime(cal, aConvertToLocal);
    mIsModified = DwTrue;
}


DwDateTime::DwDateTime(const DwDateTime& aDateTime)
  : DwFieldBody(aDateTime)
{
    Init();
    mYear     = aDateTime.mYear;
    mMonth    = aDateTime.mMonth;
    mDay      = aDateTime.mDay;
    mHour     = aDateTime.mHour;
    mMinute   = aDateTime.mMinute;
    mSecond   = aDateTime.mSecond;
    mZone     = aDateTime.mZone;
    mZoneName = aDateTime.mZoneName;
}


DwDateTime::DwDateTime(const DwString& aStr, DwMessageComponent* aParent)
  : DwFieldBody(aStr, aParent)
{
    Init();
    mIsModified = DwFalse;
}


void DwDateTime::Init()
{
    mClassId = kCidDateTime;
    mClassName = DwDateTime::sClassName;
    mYear   = 1970;
    mMonth  = 1;
    mDay    = 1;
    mHour   = 0;
    mMinute = 0;
    mSecond = 0;
    mZone   = 0;
}


DwDateTime::~DwDateTime()
{
}


const DwDateTime& DwDateTime::operator = (const DwDateTime& aDateTime)
{
    if (this == &aDateTime) {
        return *this;
    }
    DwFieldBody::operator = (aDateTime);
    mYear     = aDateTime.mYear;
    mMonth    = aDateTime.mMonth;
    mDay      = aDateTime.mDay;
    mHour     = aDateTime.mHour;
    mMinute   = aDateTime.mMinute;
    mSecond   = aDateTime.mSecond;
    mZone     = aDateTime.mZone;
    mZoneName = aDateTime.mZoneName;
    return *this;
}


void DwDateTime::Parse()
{
    mIsModified = DwFalse;
    int bufferSize = 80;
    char buffer[80];
    char *str;
    DwBool mustDelete;
    // Allocate memory from heap only in rare instances where the buffer
    // is too small.
    if ((int) mString.length() >= bufferSize) {
        mustDelete = DwTrue;
        bufferSize = mString.length() + 1;
        str = new char [bufferSize];
    }
    else {
        mustDelete = DwFalse;
        str = buffer;
    }
    if (str != 0) {
        strncpy(str, mString.data(), mString.length());
        str[mString.length()] = 0;
    }
    struct tm tms;
    int zone;
    char name[20];
    int nameSize = 20;
    int err = ParseRfc822Date(str, &tms, &zone, name, nameSize);
    if (!err) {
        mYear   = tms.tm_year + 1900;
        mMonth  = tms.tm_mon+1;
        mDay    = tms.tm_mday;
        mHour   = tms.tm_hour;
        mMinute = tms.tm_min;
        mSecond = tms.tm_sec;
        mZone   = zone;
        mZoneName = name;
    }
    else /* if (err) */ {
        mYear   = 1970;
        mMonth  = 1;
        mDay    = 1;
        mHour   = 0;
        mMinute = 0;
        mSecond = 0;
        mZone   = 0;
        mZoneName = "";
    }
    if (mustDelete && str != 0) {
        delete [] str;
    }
}


void DwDateTime::Assemble()
{
    if (!mIsModified) {
        return;
    }
    // Find the day of the week
    int dow = this->DayOfTheWeek();
    char sgn = (mZone < 0) ? '-' : '+';
    int z = (mZone < 0) ? -mZone : mZone;
    char buffer[80];
    sprintf(buffer, "%s, %d %s %4d %02d:%02d:%02d %c%02d%02d",
        lWeekDay[dow], mDay, lMonth[(mMonth-1)%12], mYear,
        mHour, mMinute, mSecond, sgn, z/60%24, z%60);
    mString = buffer;
    if (mZoneName != "") {
        mString += " (";
        mString += mZoneName;
        mString += ")";
    }
    mIsModified = DwFalse;
}


DwMessageComponent* DwDateTime::Clone() const
{
    return new DwDateTime(*this);
}


DwUint32 DwDateTime::AsUnixTime() const
{
    struct tm tt;
    tt.tm_year = mYear - 1900;
    tt.tm_mon  = mMonth - 1;
    tt.tm_mday = mDay;
    tt.tm_hour = mHour;
    tt.tm_min  = mMinute;
    tt.tm_sec  = mSecond;
    DwUint32 t = dw_brokendown_to_scalar(&tt);
    t = (t == (DwUint32) -1) ? 0 : t;
    t -= mZone*60;
    return t;
}


void DwDateTime::FromUnixTime(DwUint32 aTime, DwBool aConvertToLocal)
{
    _FromUnixTime(aTime, aConvertToLocal);
    SetModified();
}


time_t DwDateTime::AsCalendarTime() const
{
    DwUint32 unixTime = AsUnixTime();
    time_t calTime = DwUnixTimeToCalendarTime(unixTime);
    return calTime;
}


void DwDateTime::FromCalendarTime(time_t aTime, DwBool aConvertToLocal)
{
    _FromCalendarTime(aTime, aConvertToLocal);
    SetModified();
}


void DwDateTime::SetValuesLocal(int aYear, int aMonth, int aDay, int aHour,
    int aMinute, int aSecond)
{
    struct tm tms;
    tms.tm_year = aYear - 1900;
    tms.tm_mon  = aMonth - 1;
    tms.tm_mday = aDay;
    tms.tm_hour = aHour;
    tms.tm_min  = aMinute;
    tms.tm_sec  = aSecond;
    DwUint32 localScalar = dw_brokendown_to_scalar(&tms);

    DwInt32 approxOffset;
    DwGetTimeZoneInfo(localScalar, &approxOffset, 0, 0);
    DwUint32 approxUtcScalar = localScalar - approxOffset;

    DwInt32 offset;
    char name[100];
    DwInt32 nameSize = (DwInt32) sizeof(name);
    DwGetTimeZoneInfo(approxUtcScalar, &offset, name, nameSize);

    mYear   = aYear;
    mMonth  = aMonth;
    mDay    = aDay;
    mHour   = aHour;
    mMinute = aMinute;
    mSecond = aSecond;
    mZone   = offset;
    mZoneName = name;
    SetModified();
}


void DwDateTime::SetValuesLiteral(int aYear, int aMonth, int aDay, int aHour,
    int aMinute, int aSecond, int aZoneOffset, const char* aZoneName)
{
    mYear   = aYear;
    mMonth  = aMonth;
    mDay    = aDay;
    mHour   = aHour;
    mMinute = aMinute;
    mSecond = aSecond;
    mZone   = aZoneOffset;
    if (aZoneName != 0) {
        mZoneName = aZoneName;
    }
    else {
        mZoneName = "";
    }
    SetModified();
}


int DwDateTime::Year() const
{
    return mYear;
}


int DwDateTime::Month() const
{
    return mMonth;
}


int DwDateTime::Day() const
{
    return mDay;
}


int DwDateTime::Hour() const
{
    return mHour;
}


int DwDateTime::Minute() const
{
    return mMinute;
}


int DwDateTime::Second() const
{ 
    return mSecond;
}


int DwDateTime::Zone() const
{
    return mZone;
}


const DwString& DwDateTime::ZoneName() const
{
    return mZoneName;
}



int DwDateTime::DayOfTheWeek() const
{
    DwInt32 jdn = dw_ymd_to_jdn(mYear, mMonth, mDay);
    int dow = (int) ((jdn+1)%7);
    return dow;
}


void DwDateTime::_FromUnixTime(DwUint32 aTime, DwBool aConvertToLocal)
{
    DwUint32 utcScalar = aTime;
    if (aConvertToLocal) {
        DwInt32 offset;
        char name[100];
        DwInt32 nameSize = 100;
        DwGetTimeZoneInfo(utcScalar, &offset, name, nameSize);
        DwUint32 localScalar = utcScalar + offset*60;
        struct tm local;
        dw_scalar_to_brokendown(localScalar, &local);
        mYear   = local.tm_year + 1900;
        mMonth  = local.tm_mon + 1;
        mDay    = local.tm_mday;
        mHour   = local.tm_hour;
        mMinute = local.tm_min;
        mSecond = local.tm_sec;
        mZone   = offset;
        mZoneName = name;
    }
    else /* if (! aConvertToLocal) */ {
        struct tm utc;
        dw_scalar_to_brokendown(utcScalar, &utc);
        mYear   = utc.tm_year + 1900;
        mMonth  = utc.tm_mon + 1;
        mDay    = utc.tm_mday;
        mHour   = utc.tm_hour;
        mMinute = utc.tm_min;
        mSecond = utc.tm_sec;
        mZone   = 0;
        mZoneName = "UTC";
    }
}


void DwDateTime::_FromCalendarTime(time_t aTime, DwBool aConvertToLocal)
{
    // Convert to UNIX time, using portable routine

    DwUint32 unixTime = DwCalendarTimeToUnixTime(aTime);

    // Set from the UNIX time

    _FromUnixTime(unixTime, aConvertToLocal);
}


#if defined (DW_DEBUG_VERSION)
void DwDateTime::PrintDebugInfo(DW_STD ostream& aStrm, int /*aDepth*/) const
{
    aStrm <<
    "---------------- Debug info for DwDateTime class ---------------\n";
    _PrintDebugInfo(aStrm);
}
#else // if !defined (DW_DEBUG_VERSION)
void DwDateTime::PrintDebugInfo(DW_STD ostream& /*aStrm*/, int /*aDepth*/) const
{
}
#endif // !defined (DW_DEBUG_VERSION)


#if defined (DW_DEBUG_VERSION)
void DwDateTime::_PrintDebugInfo(DW_STD ostream& aStrm) const
{
    DwFieldBody::_PrintDebugInfo(aStrm);
    aStrm << "Date:             "
        << mYear << '-' << mMonth << '-' << mDay << ' '
        << mHour << ':' << mMinute << ':' << mSecond << ' '
        << mZone << '\n';
}
#else // if !defined (DW_DEBUG_VERSION)
void DwDateTime::_PrintDebugInfo(DW_STD ostream& /*aStrm*/) const
{
}
#endif // !defined (DW_DEBUG_VERSION)


void DwDateTime::CheckInvariants() const
{
#if defined (DW_DEBUG_VERSION)
    DwFieldBody::CheckInvariants();
    DW_ASSERT(mYear >= 0);
    DW_ASSERT(1 <= mMonth && mMonth <= 12);
    DW_ASSERT(1 <= mDay && mDay <= 31);
    DW_ASSERT(0 <= mHour && mHour < 24);
    DW_ASSERT(0 <= mMinute && mMinute < 60);
    DW_ASSERT(0 <= mSecond && mSecond < 60);
    DW_ASSERT(-12*60 <= mZone && mZone <= 12*60);
#endif // defined (DW_DEBUG_VERSION)
}

