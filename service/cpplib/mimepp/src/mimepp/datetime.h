//=============================================================================
// File:       datetime.h
// Contents:   Declarations for DwDateTime
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

#ifndef DW_DATETIME_H
#define DW_DATETIME_H

#ifndef DW_CONFIG_H
#include <mimepp/config.h>
#endif

#ifndef DW_FIELDBDY_H
#include <mimepp/fieldbdy.h>
#endif

#include <time.h>

//=============================================================================
//+ Name DwDateTime -- Class representing an RFC-822 date-time
//+ Description
//. <var>DwDatetime</var> represents an Internet <i>date-time</i>
//. string as described in RFC-822 and RFC-1123. The parse method for
//. <var>DwDateTime</var> parses the string representation to extract the
//. year, month, day, hour, minute, second, and time zone.
//. <var>DwDateTime</var> provides member functions to set or get the
//. individual elements of the date-time.
//.
//. The primary purpose of <var>DwDateTime</var> is to provide access to the
//. elements of a date-time string (the year, month, day, and so on), and
//. not to provide an abstract representation of time. Thus, it may be
//. helpful to think of <var>DwDateTime</var> as you would a <var>struct</var>
//. <var>tm</var> structure from the standard library. This is important, for
//. example, when you use the default constructor to create a new
//. <var>DwDateTime</var> object. In this situation, you can specify an
//. optional parameter to create the object in UTC time, or you can accept
//. the default to create the object in local time. It is then the concrete
//. representation of date and time, with respect to a particular time zone,
//. that is maintained internally, and not some more abstract notion of time.
//.
//. The use of <var>DwDateTime</var> is best explained by considering the
//. following use cases.
//.
//. <var>DwDateTime</var> parses Internet date-time strings. When a
//. MIME message is received, perhaps from a remote source, you can use
//. the inherited member function <var>DwMessageComponent::FromString()</var>,
//. or the <var>DwDateTime(const DwString&)</var> constructor, to set the
//. string representation of the <var>DwDateTime</var> object. Then call
//. the virtual function <var>Parse()</var> to parse the string
//. representation. Finally, call the member functions <var>Year()</var>,
//. <var>Month()</var>, and so on, to access the parsed elements of the
//. date-time string.
//.
//. <var>DwDateTime</var> assembles Internet date-time strings. Use the
//. default constructor to create a new <var>DwDateTime</var> object with
//. the current date and time. Call one of the member functions
//. <var>SetValuesLocal()</var> or <var>SetValuesLiteral()</var> to set
//. the year, month, and so on, from actual values. Then call the virtual
//. function <var>Assemble()</var> to create the date-time string. Finally,
//. call the inherited member function
//. <var>DwMessageComponent::AsString()</var> to retrieve the date-time
//. string.
//.
//. <var>DwDateTime</var> converts to and from a scalar date and time.
//. On a Unix system, or for portability with other Unix software, call
//. the member functions <var>FromUnixTime()</var> or <var>AsUnixTime()</var>
//. to convert from or to a ``Unix time,'' defined as the number of seconds
//. elapsed since 1 Jan 1970 00:00:00 UTC. Alternatively, or for portability
//. at the source code level, call the member functions
//. <var>FromCalendarTime()</var> or <var>AsCalendarTime()</var> to convert
//. from or to a ``calendar time,'' a value that is commensurate with the
//. output from the <var>time()</var> function of the standard library.
//. By converting to a scalar representation of the date and time,
//. MIME++ makes it easy for you to compare two different dates.
//.
//. Note: It is possible that <var>DwDateTime</var> will encounter an error
//. while parsing a date-time string. When this happens, throwing an exception
//. is not a wise way to proceed, since in most cases higher level operations
//. would also have to terminate before the library user's code could deal
//. with the exception. What actually happens, is that <var>DwDateTime</var>
//. ``recovers'' from the error by setting the date and time to
//. 1 Jan 1970 00:00:00 UTC. To easily check whether a parse operation
//. completed successfully, you can call <var>AsUnixTime()</var> and examine
//. the return value. A value of zero indicates that the parsing operation
//. failed; any other value indicates it succeeded.
//=============================================================================
// Last modified 1998-04-29
//+ Noentry ~DwDateTime mYear mMonth mDay mHour mMinute mSecond mZone mZoneName
//+ Noentry sDefaultZone sIsDefaultZoneSet _PrintDebugInfo


class DW_CORE_EXPORT DwDateTime : public DwFieldBody {

public:

    DwDateTime(DwBool aConvertToLocal=DwTrue);
    DwDateTime(const DwDateTime& aDateTime);
    DwDateTime(const DwString& aStr, DwMessageComponent* aParent=0);
    //. The first constructor is the default constructor, which assigns
    //. the current date and time as reported by the operating system.
    //. If <var>aConvertToLocal</var> is true, then the <var>DwDateTime</var>
    //. object creates its internal representation in local time; otherwise,
    //. it creates it in UTC time.
    //.
    //. The second constructor is the copy constructor.  The parent of
    //. the new <var>DwDateTime</var> object is set to <var>NULL</var>.
    //.
    //. The third constructor sets <var>aStr</var> as the <var>DwDateTime</var>
    //. object's string representation and sets <var>aParent</var> as its
    //. parent. The virtual member function <var>Parse()</var> should be
    //. called after this constructor to extract the date and time information
    //. from the string representation.  Unless it is <var>NULL</var>,
    //. <var>aParent</var> should point to an object of a class derived
    //. from <var>DwField</var>.

    virtual ~DwDateTime();

    const DwDateTime& operator = (const DwDateTime& aDateTime);
    //. This is the assignment operator, which sets this <var>DwDateTime</var>
    //. object to the same value as <var>aDateTime</var>.

    virtual void Parse();
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. executes the parse method for <var>DwDateTime</var> objects. The parse
    //. method creates or updates the broken-down representation from the
    //. string representation.  For <var>DwDateTime</var> objects, the parse
    //. method parses the string representation to extract the year,
    //. month, day, hour, minute, second, and time zone.
    //.
    //. This function clears the is-modified flag.

    virtual void Assemble();
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. executes the assemble method for <var>DwDateTime</var> objects.
    //. It should be called whenever one of the object's attributes
    //. is changed in order to assemble the string representation from
    //. its broken-down representation.  It will be called
    //. automatically for this object by the parent object's
    //. <var>Assemble()</var> member function if the is-modified flag is set.
    //.
    //. This function clears the is-modified flag.

    virtual DwMessageComponent* Clone() const;
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. creates a new <var>DwDateTime</var> on the free store that has the same
    //. value as this <var>DwDateTime</var> object. The basic idea is that of
    //. a virtual copy constructor.

    DwUint32 AsUnixTime() const;
    //. Returns the date and time as a UNIX (POSIX) time, defined as the
    //. number of seconds elapsed since 1 Jan 1970 00:00:00 UTC.

    void FromUnixTime(DwUint32 aTime, DwBool aConvertToLocal=DwTrue);
    //. Sets the date and time from <var>aTime</var>, interpreted as the
    //. number of of seconds elapsed since 1 Jan 1970 00:00:00 UTC.
    //.
    //. If <var>aConvertToLocal</var> is true, then the <var>DwDateTime</var>
    //. object creates its internal representation in local time; otherwise,
    //. it creates it in UTC time.

    time_t AsCalendarTime() const;
    //. Returns the date and time as a value of type <var>time_t</var> that
    //. conforms to the native format returned by the <var>time()</var> ANSI
    //. C function.  On most UNIX systems, this function returns the same
    //. value as <var>AsUnixTime()</var>.

    void FromCalendarTime(time_t aTime, DwBool aConvertToLocal=DwTrue);
    //. Sets the date and time from <var>aTime</var>, which is assumed to be
    //. in a format compatible with the native <var>time()</var> ANSI C
    //. function. For most UNIX systems, this function is the same as the
    //. function <var>FromUnixTime()</var>.
    //.
    //. If <var>aConvertToLocal</var> is true, then the <var>DwDateTime</var>
    //. object creates its internal representation in local time; otherwise,
    //. it creates it in UTC time.

    void SetValuesLocal(int aYear, int aMonth, int aDay, int aHour,
        int aMinute, int aSecond);
    //. Sets the values of the <var>DwDateTime</var> object as specified.
    //. Time zone information will be obtained from the system, based on the
    //. values specified.
    //.
    //. See also <var>SetValuesLiteral()</var>.

    void SetValuesLiteral(int aYear, int aMonth, int aDay, int aHour,
        int aMinute, int aSecond, int aZoneOffset, const char* aZoneName=0);
    //. Sets the values of the <var>DwDateTime</var> object as specified.
    //. Unlike <var>SetValuesLocal()</var>, this function will not obtain
    //. any time zone information from the system, nor will it do any
    //. conversion.  In other words, all values are taken to be literal,
    //. including the time zone offset.
    //.
    //. <var>aZoneOffset</var> is the offset in minutes from Coordinated
    //. Universal Time (UTC or GMT). For example, the correct value for US
    //. Eastern Standard Time is -300, since 12:00 UTC equals 7:00 EST, a
    //. difference of 300 minutes. <var>aZoneName</var>, if it is not NULL,
    //. is the name of the time zone (for example, EST).
    //.
    //. The allowed values for the parameters are as listed below:
    //.
    //. <pre>
    //.     aYear      1970 -
    //.     aMonth        1 -  12
    //.     aDay          1 -  31
    //.     aHour         0 -  23
    //.     aMinute       0 -  59
    //.     aSecond       0 -  59
    //.     aZone      -720 - 720
    //. </pre>

    int Year() const;
    //. Returns the four digit year, e.g. 1997.

    int Month() const;
    //. Returns the month. Values range from 1 to 12.

    int Day() const;
    //. Returns the day of the month. Values range from 1 to 31.

    int Hour() const;
    //. Returns the hour according to the 24 hour clock.
    //. Values range from 0 to 23.

    int Minute() const;
    //. Returns the minute. Values range from 0 to 59.

    int Second() const;
    //. Returns the second. Values range from 0 to 59.

    int Zone() const;
    //. Returns the time zone as the difference in minutes between local time
    //. and Coordinated Universal Time (UTC or GMT).

    const DwString& ZoneName() const;
    //. Returns the name of the time zone, if it's available; otherwise,
    //. returns an empty string.  The time zone name is not part of the
    //. RFC-822 <i>date-time</i> string.  Some implementations, however,
    //. include the time zone name in a comment, like so:
    //.
    //. <pre>
    //.     Sat, 28 Feb 1998 10:00:00 -0500 (EST)
    //. </pre>
    //.
    //. The above date is perfectly valid under RFC-822/RFC-1123; including
    //. the time zone name is slightly more readable to humans.
    //.
    //. MIME++ will include the time zone name in a comment if the time zone
    //. name is set in the <var>DwDateTime</var> object or if it can be
    //. obtained from the system. It will infer a time zone name if a
    //. <i>date-time</i> string that it parses has a comment at the end.

    int DayOfTheWeek() const;
    //. Returns an integer indicating the day of the week. (Sunday
    //. is 0. Saturday is 6.) This value is always computed from the other
    //. values. The day of the week is a read-only value that cannot be set.

    static DwDateTime* NewDateTime(const DwString&, DwMessageComponent*);
    //. Creates a new <var>DwDateTime</var> object on the free store.
    //. If the static data member <var>sNewDateTime</var> is <var>NULL</var>,
    //. this member function will create a new <var>DwDateTime</var>
    //. and return it. Otherwise, <var>NewDateTime()</var> will call
    //. the user-supplied function pointed to by <var>sNewDateTime</var>,
    //. which is assumed to return an object from a class derived from
    //. <var>DwDateTime</var>, and return that object.

    //+ Var sNewDateTime
    static DwDateTime* (*sNewDateTime)(const DwString&, DwMessageComponent*);
    //. If <var>sNewDateTime</var> is not <var>NULL</var>, it is assumed to
    //. point to a  user-supplied function that returns an object from a
    //. class derived from <var>DwDateTime</var>.

protected:

    void _FromUnixTime(DwUint32 aTime, DwBool aConvertToLocal);
    //. Like <var>FromUnixTime()</var>, but doesn't set the is-modified flag.

    void _FromCalendarTime(time_t aTime, DwBool aConvertToLocal);
    //. Like <var>FromCalendarTime()</var>, but doesn't set the is-modified
    //. flag.

    int      mYear;
    int      mMonth;
    int      mDay;
    int      mHour;
    int      mMinute;
    int      mSecond;
    int      mZone;
    DwString mZoneName;

private:

    static const char* const sClassName;

    void Init();
    //. Initialization code common to all constructors.

public:

    virtual void PrintDebugInfo(DW_STD ostream& aStrm, int aDepth=0) const;
    //. This virtual function, inherited from <var>DwMessageComponent</var>,
    //. prints debugging information about this object to <var>aStrm</var>.
    //.
    //. This member function is available only in the debug version of
    //. the library.

    virtual void CheckInvariants() const;
    //. Aborts if one of the invariants of the object fails. Use this
    //. member function to track down bugs.
    //.
    //. This member function is available only in the debug version of
    //. the library.

protected:

    void _PrintDebugInfo(DW_STD ostream& aStrm) const;

};

#endif
