#pragma once
#include "datetime.hpp"

//
// Copyright (c) 2014, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   15 Sep 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

namespace haystack {
    class Date;
    /**
    DateTimeRange models a starting and ending timestamp.

    @see <a href='http://project-haystack.org/doc/TagModel#tagKinds'>Project Haystack</a>

    */
    class DateTimeRange : boost::noncopyable
    {
    public:
        /**
        Enumerate week days starting from Sunday
        */
        enum WeekDays { SUNDAY = 1, MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURDAY };

        typedef std::auto_ptr<DateTimeRange> auto_ptr_t;

        /**
        Make for single date within given timezone
        */
        DateTimeRange(const Date& date, const TimeZone& tz);

        /**
        Make from two timestamps
        */
        DateTimeRange(const DateTime& start, const DateTime& end);

        /**
        Make for inclusive dates within given timezone
        */
        DateTimeRange(const Date& start, const Date& end, const TimeZone& tz);

        /**
        Inclusive starting timestamp
        */
        const DateTime& start() const { return m_start; }

        /**
        Inclusive ending timestamp
        */
        const DateTime& end() const { return m_end; }

        /**
        Make a range which encompasses the current week.
        The week is defined as Sunday thru Saturday. 
        */
        static DateTimeRange::auto_ptr_t this_week(const TimeZone& tz);

        /**
        Make a range which encompasses the current month.
        */
        static DateTimeRange::auto_ptr_t  this_month(const TimeZone& tz);

        /**
        Make a range which encompasses the current year.
        */
        static DateTimeRange::auto_ptr_t  this_year(const TimeZone& tz);

        /**
        Make a range which encompasses the previous week.
        The week is defined as Sunday thru Saturday.
        */
        static DateTimeRange::auto_ptr_t  last_week(const TimeZone& tz);

        /**
        Make a range which encompasses the previous month.
        */
        static DateTimeRange::auto_ptr_t  last_month(const TimeZone& tz);

        /**
        Make a range which encompasses the previous year.
        */
        static DateTimeRange::auto_ptr_t last_year(const TimeZone& tz);

        /**
        Parse from string using the given timezone as context for
        date based ranges.  The formats are:
        - "today"
        - "yesterday"
        - "{date}"
        - "{date},{date}"
        - "{dateTime},{dateTime}"
        - "{dateTime}"  // anything after given timestamp
        Throw exception is invalid string format.
        */
        static DateTimeRange::auto_ptr_t  make(std::string str, const TimeZone& tz);

        const std::string to_string() const;

    private:
        const DateTime m_start;
        const DateTime m_end;

    };
};