/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_DATETIME_H_201008
#define RFC_DATETIME_H_201008

#include "base/exception.h"
#include "base/globalfunc.h"

RFC_NAMESPACE_BEGIN

DECLARE_EXCEPTION(DateTimeException, Exception)

enum TimeValueConstant
{
	//4_YEARS_DAY				= 4 * 365 + 1,								// 1461 3个平年 + 1个闰年
	//100_YEARS_DAY				= 100 * 365 + 24,							// 36524 76个平年 + 24个闰年
	//400_YEARS_DAY				= 400 * 365 + 97,							// 146097 303个平年 + 97个闰年

	MONTH_PER_YEAR				= 12,

	HOUR_PER_DAY				= 24,
	MINUTE_PER_DAY				= 24 * 60,
	SECOND_PER_DAY				= MINUTE_PER_DAY * 60,
	MILLI_PER_DAY				= SECOND_PER_DAY * 1000,
	//MICRO_PER_DAY				= MILLI_PER_DAY * 1000,

	MINUTE_PER_HOUR				= 60,
	SECOND_PER_HOUR				= 60 * 60,
	MILLI_PER_HOUR				= SECOND_PER_HOUR * 1000,
	//MICRO_PER_HOUR			= MILLI_PER_HOUR * 1000,

	SECOND_PER_MINUTE			= 60,
	MILLI_PER_MINUTE			= SECOND_PER_MINUTE * 1000,
	MICRO_PER_MINUTE			= MILLI_PER_MINUTE * 1000,

	MILLI_PER_SECOND			= 1000,
	MICRO_PER_SECOND			= MILLI_PER_SECOND * 1000,

	MICRO_PER_MILLI				= 1000,
	MICRO_PER_MICRO				= 1,
};

class DateTimeSpan
{
public:
protected:
private:
};


// 保存GMT的时间,类似linux的timeval
class TimeValue
{
public:
	TimeValue(void) : m_nSecond(0), m_nMicroSecond(0) {}
	explicit TimeValue(time_t nSecond) : m_nSecond(nSecond), m_nMicroSecond(0) {}
	TimeValue(time_t nSecond, long nMicroSecond) { setTimeValue(nSecond, nMicroSecond); }

	static const TimeValue			g_tvZeroTime;
	static const TimeValue			g_tvInfiniteTime;
	static TimeValue	getGMTPresentTime(void);
	static TimeValue	getTimeUntil(const TimeValue & tvTimeWait);

	bool				isNegative(void) const { return ( (m_nSecond < 0) || (m_nSecond == 0 && m_nMicroSecond < 0) ); }

	void				getPresentTime(void);
	void				setTimeValue(time_t nSecond, long nMicroSecond);
	void				getTimeValue(time_t & nSecond, long & nMicroSecond) const { nSecond = m_nSecond; nMicroSecond = m_nMicroSecond; }

	void				setSecond(time_t nSecond) {	m_nSecond = nSecond; }
	time_t				getSecond(void) const { return m_nSecond; }

	void				setMicroSecond(long nMicroSecond) { setTimeValue(m_nSecond, nMicroSecond); }
	long				getMicroSecond(void) const { return m_nMicroSecond; }

	void				setTotalMilliSecond(rfc_int_64 nTotalMilliSecond);
	rfc_int_64			getTotalMilliSecond(void) const;

	void				setTotalMicroSecond(rfc_int_64 nTotalMicroSecond);
	rfc_int_64			getTotalMicroSecond(void) const;

	bool operator		==(const TimeValue & tvOther) const { return compare(tvOther) == 0; }
	bool operator		!=(const TimeValue & tvOther) const { return compare(tvOther) != 0; }
	bool operator		< (const TimeValue & tvOther) const { return compare(tvOther) < 0; }
	bool operator		>=(const TimeValue & tvOther) const { return compare(tvOther) >= 0; }
	bool operator		> (const TimeValue & tvOther) const { return compare(tvOther) > 0; }
	bool operator		<=(const TimeValue & tvOther) const { return compare(tvOther) <= 0; }

	TimeValue & operator = (time_t nSecond);
	const TimeValue & operator += (const TimeValue & tv);
	const TimeValue & operator -= (const TimeValue & tv);

protected:
	int					compare(const TimeValue & tvOther) const;

private:
	time_t				m_nSecond;
	long				m_nMicroSecond;
};

//////////////////////////////////////////////////////////////////////////

// 保存本地时区的时间 保存时间值从1970-01-01 00:00:00 UTC开始计算
class DateTime
{
public:
	DateTime(void) { setNULL(); }
	DateTime(const DateTime & dt) : m_nLocalZoneSecond(dt.m_nLocalZoneSecond), m_nLocalZoneMicroSecond(dt.m_nLocalZoneMicroSecond) {}

	// Constructor with standard time, can be from time(NULL)
	explicit DateTime(time_t tDateTime) : m_nLocalZoneMicroSecond(0) { setStandardTime(tDateTime); }
	DateTime(time_t nSecond, long nMicroSecond) { setStandardTime(nSecond, nMicroSecond); }

	static DateTime		getLocalZonePresentTime(void);
	static time_t		getTimeZoneSecond(void);
	static const stdstring & getTimeZoneName(void);
	static bool			isLeapYear(int nYear);
	static int			getDayOfMonth(bool bLeapYear, int nMonth);
	// Calculate the number of days from Jan 1st to the end of last month
	static int			getPassedMonthDayOfYear(bool bLeapYear, int nMonth);
	//! Return the day of week, 0=Sun, 1=Mon, ..., 6=Sat
	static int			getDayOfWeek(int nYear, int nMonth, int nDay);

	// nYear必须大于等于1970
	static bool			isValidDateTime(int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond, int nMilli = 0, int nMicro = 0);
	static bool			isValidDate(int nYear, int nMonth, int nDay);
	static bool			isValidTime(int nHour, int nMinute, int nSecond);
	static bool			isValidFractionTime(int nMilli, int nMicro);

	void				setNULL(void) { m_nLocalZoneSecond = 0; m_nLocalZoneMicroSecond = 0; }
	bool				isNULL(void) const { return (m_nLocalZoneSecond == 0 && m_nLocalZoneMicroSecond == 0); }
	bool				isNegative(void) const { return ( (m_nLocalZoneSecond < 0) || (m_nLocalZoneSecond == 0 && m_nLocalZoneMicroSecond < 0) ); }

	void				getPresentTime(void);
	void				setStandardTime(time_t nSecond, long nMicroSecond);
	void				getStandardTime(time_t & nSecond, long & nMicroSecond) const;
	void				setLocalZoneTime(time_t nSecond, long nMicroSecond);
	void				getLocalZoneTime(time_t & nSecond, long & nMicroSecond) const;

	void				setStandardTime(time_t tDateTime);
	time_t				getStandardTime(void) const;
	void				setStandardTimeTotalMicroSecond(rfc_int_64 tDateTime);
	rfc_int_64			getStandardTimeTotalMicroSecond(void) const;

	void				setLocalZoneTime(time_t tDateTime) { m_nLocalZoneSecond = tDateTime; m_nLocalZoneMicroSecond = 0; }
	time_t				getLocalZoneTime(void) const { return m_nLocalZoneSecond; }
	void				setLocalZoneTimeTotalMicroSecond(rfc_int_64 tDateTime);
	rfc_int_64			getLocalZoneTimeTotalMicroSecond(void) const;

	void				setDateTime(int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond, int nMilli = 0, int nMicro = 0);
	void				getDateTime(int & nYear, int & nMonth, int & nDay, int & nHour, int & nMinute, int & nSecond, int & nMilli, int & nMicro) const;

	void				setDate(int nYear, int nMonth, int nDay);
	void				getDate(int & nYear, int & nMonth, int & nDay) const;

	void				setTime(int nHour, int nMinute, int nSecond);
	void				getTime(int & nHour, int & nMinute, int & nSecond) const;

	void				setFractionTime(int nMilli, int nMicro);
	void				getFractionTime(int & nMilli, int & nMicro) const;

	void				setHour(int nHour);
	void				setMinute(int nMinute);
	void				setSecond(int nSecond);
	void				setMilliSecond(int nMilliSecond);
	void				setMicroSecond(int nMicroSecond);

	void				stepDateTime(time_t nSecond, long nMicroSecond);
	void				stepDateTime(int nDay, int nHour, int nMinute, int nSecond, int nMilli = 0, int nMicro = 0);
	void				stepDateTime(int nYear, int nMonth, int nDay);

	bool				isSameDay(const DateTime & dt) const;

	//! Return the Year
	int					getYear(void) const;
	//! Return the Month of year ( 1 = Jan )
	int					getMonthOfYear(void) const;
	//! Return days since start of year, Jan 1 = 1
	int					getDayOfYear(void) const;
	//! Return the day of month (1-31)
	int					getDayOfMonth(void) const;
	//! Return the day of week, 0=Sun, 1=Mon, ..., 6=Sat
	int					getDayOfWeek(void) const;
	//! Return hour in day (0-23)
	int					getHourOfDay(void) const;
	//! Return minute in hour (0-59)
	int					getMinuteOfHour(void) const;
	//! Return second in minute (0-59)
	int					getSecondOfMinute(void) const;
	//! Return millisecond in minute (0-999)
	int					getFractionMilli(void) const;
	//! Return microsecond in minute (0-999)
	int					getFractionMicro(void) const;

	stdstring			formatDate(void) const;
	stdstring			formatTime(void) const;
	stdstring			formatDateTime(const char * lpszFormat = NULL) const;
	bool				parseDateTime(const char * lpszFormat, const char * pDateTime, size_t nLen = stdstring::npos);

	DateTime &			operator = (time_t tStandartTime);
	DateTime &			operator = (const DateTime & dt);
	//DateTime			operator + (const DateTimeSpan & dtSpan);
	//DateTime			operator - (const DateTimeSpan & dtSpan);
	//const DateTime &	operator += (const DateTimeSpan & dtSpan);
	//const DateTime &	operator -= (const DateTimeSpan & dtSpan);
	//DateTimeSpan		operator - (const DateTime & dt);
	bool operator		==(const DateTime & dt) const { return compare(dt) == 0; }
	bool operator		!=(const DateTime & dt) const { return compare(dt) != 0; }
	bool operator		< (const DateTime & dt) const { return compare(dt) < 0; }
	bool operator		>=(const DateTime & dt) const { return compare(dt) >= 0; }
	bool operator		> (const DateTime & dt) const { return compare(dt) > 0; }
	bool operator		<=(const DateTime & dt) const { return compare(dt) <= 0; }

protected:
	int					compare(const DateTime & dt) const;

private:
	time_t				m_nLocalZoneSecond;
	long				m_nLocalZoneMicroSecond;	
};

class MicroSecondTimer
{
public:
	MicroSecondTimer(void) : m_tStartTime(0) { restart(); }
	explicit MicroSecondTimer(bool bStart) : m_tStartTime(0)
	{
		if ( bStart )
			restart();
	}

	void				restart(void) { m_tStartTime.getPresentTime(); }
	rfc_int_64			elapsedMicroSecond(void) const;

private:
	TimeValue			m_tStartTime;
};

class SecondTimer
{
public:
	SecondTimer(void) : m_tStartTime(0) { restart(); }
	explicit SecondTimer(bool bStart) : m_tStartTime(0)
	{
		if ( bStart )
			restart();
	}

	void				restart(void);
	time_t				elapsedSecond(void) const;

private:
	time_t				m_tStartTime;
};

RFC_NAMESPACE_END

#endif	//RFC_DATETIME_H_201008

