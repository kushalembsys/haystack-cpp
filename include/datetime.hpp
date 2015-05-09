#pragma once
#include "date.hpp"
#include "time.hpp"
#include "timezone.hpp"
#include <stdint.h>

//
// Copyright (c) 2015, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   19 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

namespace haystack {
    /**
     DateTime models a timestamp with a specific timezone.

     @see <a href='http://project-haystack.org/doc/TagModel#tagKinds'>Project Haystack</a>

     */
    class DateTime : public Val
    {
        // disable construction
        DateTime();
        // disable assignment
        DateTime& operator = (const DateTime &other);
        friend class DateTimeRange;
        DateTime(const DateTime &other) : date(other.date), time(other.time),
            tz(other.tz), tz_offset(tz.offset * 3600),
            m_millis(other.m_millis)  {};
    public:
        const Type type() const { return DATE_TIME_TYPE; }

        /**
        Date component of the timestamp
        */
        const Date date;

        /**
        Time component of the timestamp
        */
        const Time time;

        /**
        Timezone as Olson database city name
        */
        const TimeZone tz;

        /**
        Offset in seconds from UTC including DST offset
        */
        const int tz_offset;

        // ctors
        DateTime(int year, int month, int day, int hour, int min, int sec, const TimeZone& tz, int tzOffset)
            : date(Date(year, month, day)), time(Time(hour, min, sec)), tz(tz), tz_offset(tzOffset), m_millis(-1) {};
        DateTime(int year, int month, int day, int hour, int min, const TimeZone& tz, int tzOffset)
            : date(Date(year, month, day)), time(Time(hour, min)), tz(tz), tz_offset(tzOffset), m_millis(-1) {};
        DateTime(const Date& date, const Time& time) : date(date), time(time), tz(TimeZone::DEFAULT), tz_offset(tz.offset * 3600), m_millis(-1) {};
        DateTime(const Date& date, const Time& time, const TimeZone& tz) : date(date), time(time), tz(tz), tz_offset(tz.offset * 3600), m_millis(-1) {};
        DateTime(const Date& date, const Time& time, const TimeZone& tz, int tzOffset) : date(date), time(time), tz(tz), tz_offset(tzOffset), m_millis(-1) {};

        /**
        construct from time_t
        */
        static DateTime make_time_t(const time_t& ts, const TimeZone& = TimeZone::DEFAULT);
        /**
        construct from millis
        */
        static DateTime make(const int64_t& time, const TimeZone& = TimeZone::DEFAULT);
        /**
        Get DateTime for current time in default timezone or optionaly for given timezone
        */
        static DateTime now(const TimeZone& = TimeZone::DEFAULT);

        /**
        Encode as "YYYY-MM-DD'T'hh:mm:ss.FFFz zzzz"
        */
        const std::string to_zinc() const;

        /**
        Equality
        */
        bool operator == (const DateTime &) const;
        
        bool operator == (const Val &other) const;

        /**
        Comparator
        */
        bool operator < (const Val &) const;
        bool operator > (const Val &) const;

        /**
        Get this date time as Java milliseconds since epoch
        */
        const int64_t millis() const;

        auto_ptr_t clone() const;

        // utils
    private:
        int64_t m_millis;
    };
};