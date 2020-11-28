/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "base/unittestenv.h"
#include "base/datetime.h"

RFC_NAMESPACE_BEGIN

static void onTestGetLocalZoneTime(void)
{
	DateTime dt = DateTime::getLocalZonePresentTime();
	DateTime dtNow(time(NULL));
	assertTest( !dt.isNULL() );
	assertEqual(dt.getYear(), dtNow.getYear());
	assertEqual(dt.getMonthOfYear(), dtNow.getMonthOfYear());
	assertEqual(dt.getDayOfMonth(), dtNow.getDayOfMonth());
	assertEqual(DateTime::getTimeZoneSecond(), static_cast<time_t>(-8 * SECOND_PER_HOUR));
}

static void onTestLeapYear(void)
{
	assertTest(!DateTime::isLeapYear(2100));
	assertTest(!DateTime::isLeapYear(2101));
	assertTest(!DateTime::isLeapYear(2102));
	assertTest(!DateTime::isLeapYear(2103));
	assertTest(DateTime::isLeapYear(2104));
}

static void onTestGetDayOfMonth(bool bLeapYear)
{
	int nSecondMonth = (bLeapYear ? 29 : 28);
	assertEqual(DateTime::getDayOfMonth(bLeapYear, 1), 31);
	assertEqual(DateTime::getDayOfMonth(bLeapYear, 2), nSecondMonth);
	assertEqual(DateTime::getDayOfMonth(bLeapYear, 3), 31);
	assertEqual(DateTime::getDayOfMonth(bLeapYear, 4), 30);
	assertEqual(DateTime::getDayOfMonth(bLeapYear, 5), 31);
	assertEqual(DateTime::getDayOfMonth(bLeapYear, 6), 30);
	assertEqual(DateTime::getDayOfMonth(bLeapYear, 7), 31);
	assertEqual(DateTime::getDayOfMonth(bLeapYear, 8), 31);
	assertEqual(DateTime::getDayOfMonth(bLeapYear, 9), 30);
	assertEqual(DateTime::getDayOfMonth(bLeapYear, 10), 31);
	assertEqual(DateTime::getDayOfMonth(bLeapYear, 11), 30);
	assertEqual(DateTime::getDayOfMonth(bLeapYear, 12), 31);
}

static void onTestGetPassedMonthDayOfYear(bool bLeapYear)
{
	int nSecondMonth = ( bLeapYear ? 29 : 28 );
	assertEqual(DateTime::getPassedMonthDayOfYear(bLeapYear, 1), 0);
	assertEqual(DateTime::getPassedMonthDayOfYear(bLeapYear, 2), 31);
	assertEqual(DateTime::getPassedMonthDayOfYear(bLeapYear, 3), 31 + nSecondMonth);
	assertEqual(DateTime::getPassedMonthDayOfYear(bLeapYear, 4), 31 + nSecondMonth + 31);
	assertEqual(DateTime::getPassedMonthDayOfYear(bLeapYear, 5), 31 + nSecondMonth + 31 + 30);
	assertEqual(DateTime::getPassedMonthDayOfYear(bLeapYear, 6), 31 + nSecondMonth + 31 + 30 + 31);
	assertEqual(DateTime::getPassedMonthDayOfYear(bLeapYear, 7), 31 + nSecondMonth + 31 + 30 + 31 + 30);
	assertEqual(DateTime::getPassedMonthDayOfYear(bLeapYear, 8), 31 + nSecondMonth + 31 + 30 + 31 + 30 + 31);
	assertEqual(DateTime::getPassedMonthDayOfYear(bLeapYear, 9), 31 + nSecondMonth + 31 + 30 + 31 + 30 + 31 + 31);
	assertEqual(DateTime::getPassedMonthDayOfYear(bLeapYear, 10), 31 + nSecondMonth + 31 + 30 + 31 + 30 + 31 + 31 + 30);
	assertEqual(DateTime::getPassedMonthDayOfYear(bLeapYear, 11), 31 + nSecondMonth + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31);
	assertEqual(DateTime::getPassedMonthDayOfYear(bLeapYear, 12), 31 + nSecondMonth + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30);
}

static void onTestValidDate(void)
{
	assertTest(!DateTime::isValidDate(1969, 1, 1));
	assertTest(!DateTime::isValidDate(1970, -1, 1));
	assertTest(!DateTime::isValidDate(1970, 1, -1));
	assertTest(!DateTime::isValidDate(1970, 0, 1));
	assertTest(!DateTime::isValidDate(1970, 1, 0));
	assertTest(!DateTime::isValidDate(1970, 13, 1));
	assertTest(!DateTime::isValidDate(1970, 1, 32));
	assertTest(!DateTime::isValidDate(1970, 2, 29));

	assertTest(DateTime::isValidDate(1970, 1, 1));
	assertTest(!DateTime::isValidDate(2039, 1, 1));
}

static void onTestValidTime(void)
{
	assertTest(!DateTime::isValidTime(-1, 0, 0));
	assertTest(!DateTime::isValidTime(0, -1, 0));
	assertTest(!DateTime::isValidTime(0, 0, -1));

	assertTest(!DateTime::isValidTime(24, 0, 0));
	assertTest(!DateTime::isValidTime(0, 60, 0));
	assertTest(!DateTime::isValidTime(0, 0, 60));
	
	assertTest(DateTime::isValidTime(0, 0, 0));
	assertTest(DateTime::isValidTime(23, 0, 0));
	assertTest(DateTime::isValidTime(0, 59, 0));
	assertTest(DateTime::isValidTime(0, 0, 59));
}

static void onTestValidFractionTime(void)
{
	assertTest(!DateTime::isValidFractionTime(-1, 0));
	assertTest(!DateTime::isValidFractionTime(0, -1));
	assertTest(!DateTime::isValidFractionTime(1000, 0));
	assertTest(!DateTime::isValidFractionTime(0, 1000));
	assertTest(DateTime::isValidFractionTime(0, 0));
	assertTest(DateTime::isValidFractionTime(999, 999));
}

static void onTestParseDateTime(const char * lpszFormat, const char * pDateTime, size_t nLen,
								int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond)
{
	DateTime dt;
	assertTest( dt.parseDateTime(lpszFormat, pDateTime, nLen) );
	if ( nYear >= 0 )
		assertEqual(dt.getYear(), nYear);
	if ( nMonth >= 0 )
		assertEqual(dt.getMonthOfYear(), nMonth);
	if ( nDay >= 0 )
		assertEqual(dt.getDayOfMonth(), nDay);
	if ( nHour >= 0 )
		assertEqual(dt.getHourOfDay(), nHour);
	if ( nMinute >= 0 )
		assertEqual(dt.getMinuteOfHour(), nMinute);
	if ( nSecond >= 0 )
		assertEqual(dt.getSecondOfMinute(), nSecond);
}

onUnitTest(DateTime)
{
	onTestParseDateTime("%Y-%m-%d %H:%M:%S", "2011-02-11 11:13:20", 19, 2011, 2, 11, 11, 13, 20);
	onTestParseDateTime("%Y-%m-%d", "2011-02-11", 10, 2011, 2, 11, -1, -1, -1);
	onTestParseDateTime("%H:%M:%S", "11:13:20", 8, -1, -1, -1, 11, 13, 20);
	onTestParseDateTime("%y%m%d%H%M%S", "110211111320", 12, 2011, 2, 11, 11, 13, 20);

	onTestGetLocalZoneTime();
	onTestLeapYear();
	onTestGetDayOfMonth(true);
	onTestGetDayOfMonth(false);
	onTestGetPassedMonthDayOfYear(true);
	onTestGetPassedMonthDayOfYear(false);
	onTestValidDate();
	onTestValidTime();
	onTestValidFractionTime();

	DateTime dt;
	assertTest( dt.isNULL() );
	time_t tNow = time(NULL);
	DateTime dtNow(tNow);
	dt.getPresentTime();
	assertTest( !dt.isNULL() );
	assertEqual( dt.getStandardTime(), tNow );
	assertEqual( dt.getLocalZoneTime(), tNow - DateTime::getTimeZoneSecond());
	assertEqual(dt.getYear(), dtNow.getYear());
	assertEqual(dt.getMonthOfYear(), dtNow.getMonthOfYear());
	assertEqual(dt.getDayOfMonth(), dtNow.getDayOfMonth());
	assertEqual(dt.getDayOfWeek(), dtNow.getDayOfWeek());
	assertEqual(dt.getDayOfYear(), dtNow.getDayOfYear());

	dt.stepDateTime(5, 0, 0, 0);
	assertEqual(dt.getYear(), dtNow.getYear());
	assertEqual(dt.getMonthOfYear(), dtNow.getMonthOfYear());
	assertEqual(dt.getDayOfMonth(), dtNow.getDayOfMonth() + 5);
	assertEqual(dt.getDayOfWeek(), (dtNow.getDayOfWeek() + 5) % 7);
	assertEqual(dt.getDayOfYear(), dtNow.getDayOfYear() + 5);

	dt.stepDateTime(-10, 0, 0, 0);
	assertEqual(dt.getYear(), dtNow.getYear());
	assertEqual(dt.getMonthOfYear(), dtNow.getMonthOfYear());
	assertEqual(dt.getDayOfMonth(), dtNow.getDayOfMonth() - 5);
	assertEqual(dt.getDayOfWeek(), (dtNow.getDayOfWeek() + 2) % 7);
	assertEqual(dt.getDayOfYear(), dtNow.getDayOfYear() - 5);

	dt.stepDateTime(0, -24, 0, 0);
	assertEqual(dt.getYear(), dtNow.getYear());
	assertEqual(dt.getMonthOfYear(), dtNow.getMonthOfYear());
	assertEqual(dt.getDayOfMonth(), dtNow.getDayOfMonth() - 6);
	assertEqual(dt.getDayOfWeek(), (dtNow.getDayOfWeek() + 1) % 7);
	assertEqual(dt.getDayOfYear(), dtNow.getDayOfYear() - 6);

	dt.setDateTime(2000, 10, 21, 14, 13, 12);
	assertEqual(dt.getYear(), 2000);
	assertEqual(dt.getMonthOfYear(), 10);
	assertEqual(dt.getDayOfMonth(), 21);
	assertEqual(dt.getHourOfDay(), 14);
	assertEqual(dt.getMinuteOfHour(), 13);
	assertEqual(dt.getSecondOfMinute(), 12);
	assertEqual(dt.getDayOfWeek(), 6);

	dt.setDate(1998, 5, 31);
	assertEqual(dt.getYear(), 1998);
	assertEqual(dt.getMonthOfYear(), 5);
	assertEqual(dt.getDayOfMonth(), 31);
	assertEqual(dt.getHourOfDay(), 14);
	assertEqual(dt.getMinuteOfHour(), 13);
	assertEqual(dt.getSecondOfMinute(), 12);
	assertEqual(dt.getDayOfWeek(), 0);

	dt.setTime(23, 59, 59);
	assertEqual(dt.getYear(), 1998);
	assertEqual(dt.getMonthOfYear(), 5);
	assertEqual(dt.getDayOfMonth(), 31);
	assertEqual(dt.getHourOfDay(), 23);
	assertEqual(dt.getMinuteOfHour(), 59);
	assertEqual(dt.getSecondOfMinute(), 59);
	assertEqual(dt.getDayOfWeek(), 0);

	dt.setFractionTime(159, 659);
	assertEqual(dt.getYear(), 1998);
	assertEqual(dt.getMonthOfYear(), 5);
	assertEqual(dt.getDayOfMonth(), 31);
	assertEqual(dt.getHourOfDay(), 23);
	assertEqual(dt.getMinuteOfHour(), 59);
	assertEqual(dt.getSecondOfMinute(), 59);
	assertEqual(dt.getFractionMilli(), 159);
	assertEqual(dt.getFractionMicro(), 659);
	assertEqual(dt.getDayOfWeek(), 0);

	dt.setStandardTime(tNow);
	assertEqual(dt.getYear(), dtNow.getYear());
	assertEqual(dt.getMonthOfYear(), dtNow.getMonthOfYear());
	assertEqual(dt.getDayOfMonth(), dtNow.getDayOfMonth());
	assertEqual(dt.getFractionMilli(), 159);
	assertEqual(dt.getFractionMicro(), 659);
	assertEqual(dt.getDayOfWeek(), dtNow.getDayOfWeek());
	assertEqual(dt.getDayOfYear(), dtNow.getDayOfYear());

	dt = tNow;
	assertEqual(dt.getYear(), dtNow.getYear());
	assertEqual(dt.getMonthOfYear(), dtNow.getMonthOfYear());
	assertEqual(dt.getDayOfMonth(), dtNow.getDayOfMonth());
	assertEqual(dt.getFractionMilli(), dtNow.getFractionMilli());
	assertEqual(dt.getFractionMicro(), dtNow.getFractionMilli());
	assertEqual(dt.getDayOfWeek(), dtNow.getDayOfWeek());
	assertEqual(dt.getDayOfYear(), dtNow.getDayOfYear());

	dt.setDateTime(2012, 2, 27, 14, 50, 32);
	assertEqual(dt.formatDateTime(), stdstring("2012-02-27 14:50:32"));
	assertEqual(dt.formatDateTime("%Y-%m-%d %H:%M:%S"), stdstring("2012-02-27 14:50:32"));
	assertEqual(dt.formatDateTime("%a, %d %b %Y %H:%M:%S %z (%Z)"), stdstring("Mon, 27 Feb 2012 14:50:32 +0800 (CST)"));
}

RFC_NAMESPACE_END
