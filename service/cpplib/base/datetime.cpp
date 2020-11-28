/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "base/datetime.h"
#include "base/formatstream.h"
#include <time.h>
#include <sys/time.h>

RFC_NAMESPACE_BEGIN

class AutoTimeZone
{
public:
	AutoTimeZone(void) { resetTimeZone(); }
	time_t				getTimeZoneSecond(void) const { return m_nTimeZoneSecond; }
	const stdstring &	getTimeZoneName(void) const { return m_strTimeZoneName; }

protected:
	void				resetTimeZone(void);

private:
	time_t				m_nTimeZoneSecond;
	stdstring			m_strTimeZoneName;
};

void AutoTimeZone::resetTimeZone(void)
{
	tzset();
	m_nTimeZoneSecond = timezone;
	m_strTimeZoneName = tzname[0];
}

static const AutoTimeZone g_TimeZone;
static const rfc_int_64 g_nSecondBefore1970 = static_cast<rfc_int_64>(62135769600LL);

template<typename typeParent, typename typeChild>
static void smartAdjustTime(typeParent & nParent, typeChild & nChild, int nUnitMutilple)
{
	if ( nChild > nUnitMutilple )
	{
		nParent += nChild / nUnitMutilple;
		nChild %= nUnitMutilple;
	}
	else if ( nChild < 0 )
	{
		int nMutilple = (-nChild) / nUnitMutilple + 1;
		nParent -= nMutilple;
		nChild += nMutilple * nUnitMutilple;
	}
}

static void smartAdjustDate(int & nYear, int & nMonth, int & nDay)
{
	smartAdjustTime(nYear, nMonth, MONTH_PER_YEAR);

	while( true )
	{
		int nDayOfMonth = DateTime::getDayOfMonth( DateTime::isLeapYear(nYear), nMonth );
		if ( nDay > nDayOfMonth )
		{
			nDay -= nDayOfMonth;
			++nMonth;
			if ( nMonth > 12 )
			{
				++nYear;
				nMonth -= 12;
			}
		} // if ( nDay > nDayOfMonth )
		else if ( nDay < 1 )
		{
			if ( nMonth == 1 )
			{
				--nYear;
				nMonth += 12;
			}
			--nMonth;
			nDay += DateTime::getDayOfMonth( DateTime::isLeapYear(nYear), nMonth );
		}
		else
			break;
	} // while( true )
}

static void convertFrom(rfc_int_64 nVal, TimeValueConstant nSecondToUnit, TimeValueConstant nUnitToMicro, time_t & nSecond, long & nMicroSecond)
{
	nSecond = nVal / nSecondToUnit;
	nMicroSecond = (nVal % nSecondToUnit) * nUnitToMicro;
	smartAdjustTime(nSecond, nMicroSecond, MICRO_PER_SECOND);
}

static rfc_int_64 convertTo(time_t nSecond, long nMicroSecond, TimeValueConstant nSecondToUnit, TimeValueConstant nUnitToMicro)
{
	return static_cast<rfc_int_64>(nSecond) * nSecondToUnit + static_cast<rfc_int_64>(nMicroSecond) / nUnitToMicro;
}

//////////////////////////////////////////////////////////////////////////

const TimeValue TimeValue::g_tvZeroTime(0, 0);
const TimeValue TimeValue::g_tvInfiniteTime(static_cast<time_t>(GlobalConstant::g_nMaxInt32), 0);

TimeValue TimeValue::getGMTPresentTime(void)
{
	TimeValue tv;
	tv.getPresentTime();
	return tv;
}

void TimeValue::getPresentTime(void)
{
	timeval tvPresentTime;
	gettimeofday( &tvPresentTime, NULL );
	setTimeValue(tvPresentTime.tv_sec, tvPresentTime.tv_usec);
}

TimeValue TimeValue::getTimeUntil(const TimeValue & tvTimeWait)
{
	if ( tvTimeWait >= TimeValue::g_tvInfiniteTime )
		return TimeValue::g_tvInfiniteTime;

	TimeValue tvTimeUntil = TimeValue::getGMTPresentTime();
	tvTimeUntil += tvTimeWait;
	return tvTimeUntil;
}

void TimeValue::setTimeValue(time_t nSecond, long nMicroSecond)
{
	m_nSecond = nSecond;
	m_nMicroSecond = nMicroSecond;
	smartAdjustTime(m_nSecond, m_nMicroSecond, MICRO_PER_SECOND);
}

void TimeValue::setTotalMilliSecond(rfc_int_64 nTotalMilliSecond)
{
	convertFrom(nTotalMilliSecond, MILLI_PER_SECOND, MICRO_PER_MILLI, m_nSecond, m_nMicroSecond);
}

rfc_int_64 TimeValue::getTotalMilliSecond(void) const
{
	return convertTo(m_nSecond, m_nMicroSecond, MILLI_PER_SECOND, MICRO_PER_MILLI);
}

void TimeValue::setTotalMicroSecond(rfc_int_64 nTotalMilliSecond)
{
	convertFrom(nTotalMilliSecond, MICRO_PER_SECOND, MICRO_PER_MICRO, m_nSecond, m_nMicroSecond);
}

rfc_int_64 TimeValue::getTotalMicroSecond(void) const
{
	return convertTo(m_nSecond, m_nMicroSecond, MICRO_PER_SECOND, MICRO_PER_MICRO);
}

TimeValue & TimeValue::operator = (time_t nSecond)
{
	m_nSecond = nSecond;
	m_nMicroSecond = 0;
	return *this;
}

const TimeValue & TimeValue::operator += (const TimeValue & tv)
{
	m_nSecond += tv.m_nSecond;
	m_nMicroSecond += tv.m_nMicroSecond;
	smartAdjustTime(m_nSecond, m_nMicroSecond, MICRO_PER_SECOND);
	return *this;
}

const TimeValue & TimeValue::operator -= (const TimeValue & tv)
{
	m_nSecond -= tv.m_nSecond;
	m_nMicroSecond -= tv.m_nMicroSecond;
	smartAdjustTime(m_nSecond, m_nMicroSecond, MICRO_PER_SECOND);
	return *this;
}

int TimeValue::compare(const TimeValue & tvOther) const
{
	if ( m_nSecond != tvOther.m_nSecond )	//compare second first
		return (m_nSecond > tvOther.m_nSecond ) ? 1 : -1;

	if ( m_nMicroSecond == tvOther.m_nMicroSecond )	//second equal, compare nanosecond
		return 0;
	return ( m_nMicroSecond > tvOther.m_nMicroSecond ) ? 1 : -1;
}

//////////////////////////////////////////////////////////////////////////

DateTime DateTime::getLocalZonePresentTime(void)
{
	DateTime dt;
	dt.getPresentTime();
	return dt;
}

time_t DateTime::getTimeZoneSecond(void)
{
	return g_TimeZone.getTimeZoneSecond();
}

const stdstring & DateTime::getTimeZoneName(void)
{
	return g_TimeZone.getTimeZoneName();
}

bool DateTime::isLeapYear(int nYear)
{
	if ( nYear < 1752 )
		return ( (nYear & 3) == 0 );		// nYear % 4
	return ( ((nYear & 3) == 0 && (nYear % 100) != 0) || (nYear % 400) == 0 );
}

int	DateTime::getDayOfMonth(bool bLeapYear, int nMonth)
{
	static const int g_arDayOfMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };	// Non-leap year
	if ( nMonth < 1 || nMonth > 12 )
		throw DateTimeException( FormatString("Invalid month on call DateTime::getDayOfMonth:%").arg(nMonth).str() );
	if ( bLeapYear && nMonth == 2 )
		return 29;
	return g_arDayOfMonth[nMonth];
}

int DateTime::getPassedMonthDayOfYear(bool bLeapYear, int nMonth)
{
	static const int g_arDayOfYear[] ={ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };	// Non-leap year
	if ( nMonth < 1 || nMonth > 12 )
		throw DateTimeException( FormatString("Invalid month on call DateTime::getDayOfMonth:%").arg(nMonth).str() );
	if ( bLeapYear && nMonth > 2 )
		return g_arDayOfYear[nMonth - 1] + 1;
	return g_arDayOfYear[nMonth - 1];
}

int DateTime::getDayOfWeek(int nYear, int nMonth, int nDay)
{
	if ( nMonth <= 2 )
	{
		nMonth += 12;
		--nYear;
	}
	int nWeek = (nDay + 2 * nMonth + 3 * (nMonth + 1) / 5 + nYear + nYear / 4 - nYear / 100 + nYear / 400 + 1) % 7; //Zeller
	if ( nWeek < 0 )
		nWeek += 7;
	return nWeek;
}

bool DateTime::isValidDateTime(int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond, int nMilli, int nMicro)
{
	return ( isValidDate(nYear, nMonth, nDay) && isValidTime(nHour, nMinute, nSecond) && isValidFractionTime(nMilli, nMicro) );
}

bool DateTime::isValidDate(int nYear, int nMonth, int nDay)
{
	if ( nYear < 1970 || nYear > 2038 || nMonth < 1 || nMonth > 12 || nDay < 1 )
		return false;
	return ( nDay <= getDayOfMonth(isLeapYear(nYear), nMonth) );
}

bool DateTime::isValidTime(int nHour, int nMinute, int nSecond)
{
	return ( nHour >= 0 && nHour < 24 && nMinute >= 0 && nMinute < 60 && nSecond >= 0 && nSecond < 60 );
}

bool DateTime::isValidFractionTime(int nMilli, int nMicro)
{
	return ( nMilli >= 0 && nMilli < 1000 && nMicro >= 0 && nMicro < 1000 );
}

void DateTime::getPresentTime(void)
{
	timeval tvPresentTime;
	gettimeofday( &tvPresentTime, NULL );
	setStandardTime(tvPresentTime.tv_sec, tvPresentTime.tv_usec);
}

void DateTime::setStandardTime(time_t nSecond, long nMicroSecond)
{
	setStandardTime(nSecond);
	m_nLocalZoneMicroSecond = nMicroSecond;
	smartAdjustTime(m_nLocalZoneSecond, m_nLocalZoneMicroSecond, MICRO_PER_SECOND);
}

void DateTime::getStandardTime(time_t & nSecond, long & nMicroSecond) const
{
	nSecond = getStandardTime();
	nMicroSecond = m_nLocalZoneMicroSecond;
}

void DateTime::setLocalZoneTime(time_t nSecond, long nMicroSecond)
{
	m_nLocalZoneSecond = nSecond;
	m_nLocalZoneMicroSecond = nMicroSecond;
	smartAdjustTime(m_nLocalZoneSecond, m_nLocalZoneMicroSecond, MICRO_PER_SECOND);
}

void DateTime::getLocalZoneTime(time_t & nSecond, long & nMicroSecond) const
{
	nSecond = m_nLocalZoneSecond;
	nMicroSecond = m_nLocalZoneMicroSecond;
}

void DateTime::setStandardTime(time_t tDateTime)
{
	m_nLocalZoneSecond = tDateTime - g_TimeZone.getTimeZoneSecond();
}

time_t DateTime::getStandardTime(void) const
{
	return m_nLocalZoneSecond + g_TimeZone.getTimeZoneSecond();
}

void DateTime::setStandardTimeTotalMicroSecond(rfc_int_64 tDateTime)
{
	convertFrom(tDateTime, MICRO_PER_SECOND, MICRO_PER_MICRO, m_nLocalZoneSecond, m_nLocalZoneMicroSecond);
	setStandardTime(m_nLocalZoneSecond);
}

rfc_int_64 DateTime::getStandardTimeTotalMicroSecond(void) const
{
	return convertTo(getStandardTime(), m_nLocalZoneMicroSecond, MICRO_PER_SECOND, MICRO_PER_MICRO);
}

void DateTime::setLocalZoneTimeTotalMicroSecond(rfc_int_64 tDateTime)
{
	convertFrom(tDateTime, MICRO_PER_SECOND, MICRO_PER_MICRO, m_nLocalZoneSecond, m_nLocalZoneMicroSecond);
}

rfc_int_64 DateTime::getLocalZoneTimeTotalMicroSecond(void) const
{
	return convertTo(m_nLocalZoneSecond, m_nLocalZoneMicroSecond, MICRO_PER_SECOND, MICRO_PER_MICRO);
}

void DateTime::setDateTime(int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond, int nMilli, int nMicro)
{
	if ( !isValidDateTime(nYear, nMonth, nDay, nHour, nMinute, nSecond, nMilli, nMicro) )
		throw DateTimeException( FormatString("Invalid date time on call DateTime::setDateTime [%-%-% %:%:% (%:%)]")
		.arg(nYear).arg(nMonth).arg(nDay).arg(nHour).arg(nMinute).arg(nSecond).arg(nMilli).arg(nMicro).str() );
	/*time_t nPassedDay = (nYear - 1970) * 365 + nYear / 4 - nYear / 100 + nYear / 400;		//get passed day of year
	nPassedDay += getPassedMonthDayOfYear(isLeapYear(nYear), nMonth) + nDay - 1;			//add passed day of month and day
	m_nLocalZoneSecond = nPassedDay * SECOND_PER_DAY + nHour * SECOND_PER_HOUR + nMinute * SECOND_PER_MINUTE + nSecond;	// get total second
	*/

	struct tm tmTime;
	memset(&tmTime, 0, sizeof(struct tm));
	tmTime.tm_year = nYear - 1900;
	tmTime.tm_mon = nMonth - 1;
	tmTime.tm_mday = nDay;
	tmTime.tm_hour = nHour;
	tmTime.tm_min = nMinute;
	tmTime.tm_sec = nSecond;
	tmTime.tm_isdst = -1;
	m_nLocalZoneSecond = mktime(&tmTime);
	setStandardTime(m_nLocalZoneSecond);

	setFractionTime(nMilli, nMicro);
}

void DateTime::getDateTime(int & nYear, int & nMonth, int & nDay, int & nHour, int & nMinute, int & nSecond, int & nMilli, int & nMicro) const
{
	time_t tStandardTime = getStandardTime();
	struct tm tmTime;
	struct tm * pTmTime = localtime_r(&tStandardTime, &tmTime);
	nYear = pTmTime->tm_year + 1900;
	nMonth = pTmTime->tm_mon + 1;
	nDay = pTmTime->tm_mday;
	nHour = pTmTime->tm_hour;
	nMinute = pTmTime->tm_min;
	nSecond = pTmTime->tm_sec;
	getFractionTime(nMilli, nMicro);
}

void DateTime::setDate(int nYear, int nMonth, int nDay)
{
	int nMyYear = 0, nMyMonth = 0, nMyDay = 0, nHour = 0, nMinute = 0, nSecond = 0, nMilli = 0, nMicro = 0;
	getDateTime(nMyYear, nMyMonth, nMyDay, nHour, nMinute, nSecond, nMilli, nMicro);
	setDateTime(nYear, nMonth, nDay, nHour, nMinute, nSecond, nMilli, nMicro);

	/*if ( !isValidDate(nYear, nMonth, nDay) )
		throw DateTimeException( FormatString("Invalid date on call DateTime::setDateTime [%-%-%]").arg(nYear).arg(nMonth).arg(nDay).str() );

	time_t nPassedDay = (nYear - 1970) * 365 + nYear / 4 - nYear / 100 + nYear / 400;	//get passed day of year
	nPassedDay += getPassedMonthDayOfYear(isLeapYear(nYear), nMonth) + nDay - 1;		//add passed day of month and day
	m_nLocalZoneSecond = ( nPassedDay * SECOND_PER_DAY ) + ( m_nLocalZoneSecond % SECOND_PER_DAY );*/
}

void DateTime::getDate(int & nYear, int & nMonth, int & nDay) const
{
	int nHour = 0, nMinute = 0, nSecond = 0, nMilli = 0, nMicro = 0;
	getDateTime(nYear, nMonth, nDay, nHour, nMinute, nSecond, nMilli, nMicro);
}

void DateTime::setTime(int nHour, int nMinute, int nSecond)
{
	if ( !isValidTime(nHour, nMinute, nSecond) )
		throw DateTimeException( FormatString("Invalid time on call DateTime::setTime [%:%:%]").arg(nHour).arg(nMinute).arg(nSecond).str() );
	time_t nPassedDay = m_nLocalZoneSecond / SECOND_PER_DAY;
	m_nLocalZoneSecond = nPassedDay * SECOND_PER_DAY + nHour * SECOND_PER_HOUR + nMinute * SECOND_PER_MINUTE + nSecond;	// get total second
}

void DateTime::getTime(int & nHour, int & nMinute, int & nSecond) const
{
	time_t tSave = m_nLocalZoneSecond;
	nSecond = tSave % SECOND_PER_MINUTE;
	tSave /= SECOND_PER_MINUTE;						// save as minute
	nMinute = tSave % MINUTE_PER_HOUR;
	tSave /= MINUTE_PER_HOUR;						// save as hour
	nHour = tSave % HOUR_PER_DAY;
}

void DateTime::setFractionTime(int nMilli, int nMicro)
{
	if ( !isValidFractionTime(nMilli, nMicro) )
		throw DateTimeException( FormatString("Invalid fraction time on call DateTime::setFractionTime [%-%]").arg(nMilli).arg(nMicro).str() );
	m_nLocalZoneMicroSecond = nMilli * MICRO_PER_MILLI + nMicro;
}

void DateTime::getFractionTime(int & nMilli, int & nMicro) const
{
	nMicro = m_nLocalZoneMicroSecond % MICRO_PER_MILLI;
	nMilli = m_nLocalZoneMicroSecond / MICRO_PER_MILLI;
}

void DateTime::setHour(int nHour)
{
	if ( nHour < 0 || nHour >= 24 )
		throw DateTimeException( FormatString("Invalid time on call DateTime::setHour: %").arg(nHour).str() );
	nHour -= m_nLocalZoneSecond % HOUR_PER_DAY;
	m_nLocalZoneSecond += nHour * SECOND_PER_HOUR;
}

void DateTime::setMinute(int nMinute)
{
	if ( nMinute < 0 || nMinute >= 60 )
		throw DateTimeException( FormatString("Invalid time on call DateTime::setMinute: %").arg(nMinute).str() );
	nMinute -= m_nLocalZoneSecond % MINUTE_PER_HOUR;
	m_nLocalZoneSecond += nMinute * SECOND_PER_MINUTE;
}

void DateTime::setSecond(int nSecond)
{
	if ( nSecond < 0 || nSecond >= 60 )
		throw DateTimeException( FormatString("Invalid time on call DateTime::setSecond: %").arg(nSecond).str() );
	nSecond -= m_nLocalZoneSecond % SECOND_PER_MINUTE;
	m_nLocalZoneSecond += nSecond;
}

void DateTime::setMilliSecond(int nMilliSecond)
{
	if ( nMilliSecond < 0 || nMilliSecond >= 1000 )
		throw DateTimeException( FormatString("Invalid time on call DateTime::setMilliSecond: %").arg(nMilliSecond).str() );
	m_nLocalZoneMicroSecond = (nMilliSecond * MICRO_PER_MILLI) + (m_nLocalZoneMicroSecond % MICRO_PER_MILLI);
}

void DateTime::setMicroSecond(int nMicroSecond)
{
	if ( nMicroSecond < 0 || nMicroSecond >= 1000 )
		throw DateTimeException( FormatString("Invalid time on call DateTime::setMicroSecond: %").arg(nMicroSecond).str() );
	m_nLocalZoneMicroSecond = m_nLocalZoneMicroSecond + nMicroSecond - (m_nLocalZoneMicroSecond % MICRO_PER_MILLI);
}

void DateTime::stepDateTime(time_t nSecond, long nMicroSecond)
{
	m_nLocalZoneSecond += nSecond;
	m_nLocalZoneMicroSecond += nMicroSecond;
	smartAdjustTime(m_nLocalZoneSecond, m_nLocalZoneMicroSecond, MICRO_PER_SECOND);
}

void DateTime::stepDateTime(int nDay, int nHour, int nMinute, int nSecond, int nMilli, int nMicro)
{
	time_t nStepSecond = static_cast<time_t>(nDay) * SECOND_PER_DAY + nHour * SECOND_PER_HOUR + nMinute * SECOND_PER_MINUTE + nSecond;
	long nStepMicro = static_cast<long>(nMilli) * MICRO_PER_MILLI + nMicro;
	stepDateTime(nStepSecond, nStepMicro);
}

void DateTime::stepDateTime(int nYear, int nMonth, int nDay)
{
	stepDateTime(nDay, 0, 0, 0);
	if ( nYear == 0 && nMonth == 0 )
		return;

	int nMyYear = 0, nMyMonth = 0, nMyDay = 0;
	getDate(nMyYear, nMyMonth, nMyDay);
	nMyYear			+= nYear;
	nMyMonth		+= nMonth;
	smartAdjustDate(nMyYear, nMyMonth, nMyDay);
	setDate(nMyYear, nMyMonth, nMyDay);
}

bool DateTime::isSameDay(const DateTime & dt) const
{
	int nMyYear = 0, nMyMonth = 0, nMyDay = 0;
	getDate(nMyYear, nMyMonth, nMyDay);
	int nYear = 0, nMonth = 0, nDay = 0;
	dt.getDate(nYear, nMonth, nDay);
	return ( nMyYear == nYear && nMyMonth == nMonth && nMyDay == nDay );
}

int DateTime::getYear(void) const
{
	int nYear = 0, nMonth = 0, nDay = 0;
	getDate(nYear, nMonth, nDay);
	return nYear;
}

int DateTime::getMonthOfYear(void) const
{
	int nYear = 0, nMonth = 0, nDay = 0;
	getDate(nYear, nMonth, nDay);
	return nMonth;
}

int DateTime::getDayOfYear(void) const
{
	int nYear = 0, nMonth = 0, nDay = 0;
	getDate(nYear, nMonth, nDay);
	return getPassedMonthDayOfYear(isLeapYear(nYear), nMonth) + nDay;
}

int DateTime::getDayOfMonth(void) const
{
	int nYear = 0, nMonth = 0, nDay = 0;
	getDate(nYear, nMonth, nDay);
	return nDay;
}

int DateTime::getDayOfWeek(void) const
{
	rfc_int_64 nSecondSince1900 = g_nSecondBefore1970 + m_nLocalZoneSecond;
	return ( (static_cast<int>(nSecondSince1900 / SECOND_PER_DAY) + 6) % 7 );
}

int DateTime::getHourOfDay(void) const
{
	int nHour = 0, nMinute = 0, nSecond = 0;
	getTime(nHour, nMinute, nSecond);
	return nHour;
}

int DateTime::getMinuteOfHour(void) const
{
	int nHour = 0, nMinute = 0, nSecond = 0;
	getTime(nHour, nMinute, nSecond);
	return nMinute;
}

int DateTime::getSecondOfMinute(void) const
{
	int nHour = 0, nMinute = 0, nSecond = 0;
	getTime(nHour, nMinute, nSecond);
	return nSecond;
}

int DateTime::getFractionMilli(void) const
{
	return static_cast<int>(m_nLocalZoneMicroSecond) / MICRO_PER_MILLI;
}

int DateTime::getFractionMicro(void) const
{
	return static_cast<int>(m_nLocalZoneMicroSecond) % MICRO_PER_MILLI;
}

stdstring DateTime::formatDate(void) const
{
	int nYear = 0, nMonth = 0, nDay = 0;
	getDate(nYear, nMonth, nDay);
	return FormatString("%-%-%").arg(nYear)
		.arg(StringFunc::intToString(nMonth, 10, true, 2, '0'))
		.arg(StringFunc::intToString(nDay, 10, true, 2, '0')).str();
}

stdstring DateTime::formatTime(void) const
{
	int nHour = 0, nMinute = 0, nSecond = 0;
	getTime(nHour, nMinute, nSecond);
	return FormatString("%:%:%").arg(StringFunc::intToString(nHour, 10, true, 2, '0'))
		.arg(StringFunc::intToString(nMinute, 10, true, 2, '0'))
		.arg(StringFunc::intToString(nSecond, 10, true, 2, '0')).str();
}

stdstring DateTime::formatDateTime(const char * lpszFormat) const
{
	if ( lpszFormat != NULL )
	{
		time_t tStandardTime = getStandardTime();
		struct tm tmTime;
		struct tm * pTmTime = localtime_r(&tStandardTime, &tmTime);
		char szTimeFormat[1024];
		memset(szTimeFormat, 0, sizeof(szTimeFormat));
		::strftime(szTimeFormat, sizeof(szTimeFormat) - 1, lpszFormat, pTmTime);
		return stdstring(szTimeFormat);
	}

	int nYear = 0, nMonth = 0, nDay = 0, nHour = 0, nMinute = 0, nSecond = 0, nMilli = 0, nMicro = 0;
	getDateTime(nYear, nMonth, nDay, nHour, nMinute, nSecond, nMilli, nMicro);
	return FormatString("%-%-% %:%:%").arg(nYear)
		.arg(StringFunc::intToString(nMonth, 10, true, 2, '0'))
		.arg(StringFunc::intToString(nDay, 10, true, 2, '0'))
		.arg(StringFunc::intToString(nHour, 10, true, 2, '0'))
		.arg(StringFunc::intToString(nMinute, 10, true, 2, '0'))
		.arg(StringFunc::intToString(nSecond, 10, true, 2, '0')).str();
}

bool DateTime::parseDateTime(const char * lpszFormat, const char * pDateTime, size_t nLen)
{
	int nYear = 0, nMonth = 0, nDay = 0, nHour = 0, nMinute = 0, nSecond = 0, nMilli = 0, nMicro = 0;
	getDateTime(nYear, nMonth, nDay, nHour, nMinute, nSecond, nMilli, nMicro);

	const char * pDateTimeEnd = pDateTime + nLen;
	while ( *lpszFormat != RFC_CHAR_NULL )
	{
		char ch = *lpszFormat++;
		if ( ch != '%' )
		{
			if ( *pDateTime == RFC_CHAR_NULL || pDateTime >= pDateTimeEnd || *pDateTime != ch )
				return false;
			++pDateTime;
			continue;
		}

		ch = *lpszFormat++;
		if ( ch == 'Y' )
			nYear = StringFunc::smartConvertDigit<int>(pDateTime, 4);
		else if ( ch == 'y' )
			nYear = 2000 + StringFunc::smartConvertDigit<int>(pDateTime, 2);
		else if ( ch == 'm' )
			nMonth = StringFunc::smartConvertDigit<int>(pDateTime, 2);
		else if ( ch == 'd' || ch == 'e' )
			nDay = StringFunc::smartConvertDigit<int>(pDateTime, 2);
		else if ( ch == 'H' ) //|| ch == 'I' || ch == 'k' || ch == 'l')
			nHour = StringFunc::smartConvertDigit<int>(pDateTime, 2);
		else if ( ch == 'M' )
			nMinute = StringFunc::smartConvertDigit<int>(pDateTime, 2);
		else if ( ch == 'S' )
			nSecond = StringFunc::smartConvertDigit<int>(pDateTime, 2);
		//else	// ÉÐÎ´Ö§³Ö
		//	return false;
	} // for ( ; *lpszFormat != RFC_CHAR_NULL; ++lpszFormat )

	if ( !isValidDateTime(nYear, nMonth, nDay, nHour, nMinute, nSecond, nMilli, nMicro) )
		return false;
	setDateTime(nYear, nMonth, nDay, nHour, nMinute, nSecond, nMilli, nMicro);
	return true;
}

DateTime & DateTime::operator = (time_t tStandartTime)
{
	m_nLocalZoneSecond = tStandartTime;
	m_nLocalZoneMicroSecond = 0;
	return *this;
}

DateTime & DateTime::operator = (const DateTime & dt)
{
	m_nLocalZoneSecond = dt.m_nLocalZoneSecond;
	m_nLocalZoneMicroSecond = dt.m_nLocalZoneMicroSecond;
	return *this;
}

int DateTime::compare(const DateTime & dt) const
{
	if ( m_nLocalZoneSecond != dt.m_nLocalZoneSecond )	//compare second first
		return ( m_nLocalZoneSecond > dt.m_nLocalZoneSecond ) ? 1 : -1;

	if ( m_nLocalZoneMicroSecond == dt.m_nLocalZoneMicroSecond )	//second equal, compare nanosecond
		return 0;
	return ( m_nLocalZoneMicroSecond > dt.m_nLocalZoneMicroSecond ) ? 1 : -1;
}

rfc_int_64 MicroSecondTimer::elapsedMicroSecond(void) const
{
	if ( m_tStartTime == TimeValue::g_tvZeroTime )
		return 0;
	TimeValue tNow;
	tNow.getPresentTime();
	tNow -= m_tStartTime;
	return tNow.getTotalMicroSecond();
}

void SecondTimer::restart(void)
{
	m_tStartTime = time(NULL);
}

time_t SecondTimer::elapsedSecond(void) const
{
	time_t tNow = time(NULL);
	tNow -= m_tStartTime;
	return tNow;
}

RFC_NAMESPACE_END
