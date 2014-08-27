#pragma once
#include "date.hpp"
#include "time.hpp"
#include "timezone.hpp"
#include <stdint.h>

//
// Copyright (c) 2014, Radu Racariu, Brian Frank
// History:
//   19 Aug 2014  Radu Racariu Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

/**
 DateTime models a timestamp with a specific timezone.

 @see <a href='http://project-haystack.org/doc/TagModel#tagKinds'>Project Haystack</a> 

*/
namespace haystack {
class DateTime : public Val
{
	// disable construction
    DateTime();
	// disable assignment
    DateTime& operator=(const DateTime &other);
protected:
    const char type() const { return 'X'; }
public:
    
    // Date component of the timestamp
    const Date date;

    // Time component of the timestamp
    const Time time;

    // Timezone as Olson database city name
    const TimeZone tz;

    // Offset in seconds from UTC including DST offset
    const int tz_offset;

    // ctors
    DateTime(int year, int month, int day, int hour, int min, int sec, const TimeZone& tz, int tzOffset) 
        : date(Date(year, month, day)), time(Time(hour, min, sec)), tz(tz), tz_offset(tzOffset), _millis(-1) {};
    DateTime(int year, int month, int day, int hour, int min, const TimeZone& tz, int tzOffset)
        : date(Date(year, month, day)), time(Time(hour, min)), tz(tz), tz_offset(tzOffset), _millis(-1) {};
    DateTime(const Date& date, const Time& time) : date(date), time(time), tz(TimeZone::DEFAULT), tz_offset(tz.offset * 3600), _millis(-1) {};
    DateTime(const DateTime &other) : date(other.date), time(other.time), tz(TimeZone::DEFAULT), tz_offset(tz.offset * 3600), _millis(-1)  {};
    DateTime(const Date& date, const Time& time, const TimeZone& tz) : date(date), time(time), tz(tz), tz_offset(tz.offset * 3600), _millis(-1) {};
    DateTime(const Date& date, const Time& time, const TimeZone& tz, int tzOffset) : date(date), time(time), tz(tz), tz_offset(tzOffset), _millis(-1) {};
	
    // construct from time_t
    static DateTime make(const int64_t& time, const TimeZone& = TimeZone::DEFAULT);
    // Get DateTime for current time in default timezone or optionaly for given timezone
    static DateTime now(const TimeZone& = TimeZone::DEFAULT);
    
    // Encode as "YYYY-MM-DD'T'hh:mm:ss.FFFz zzzz"
	const std::string to_zinc() const;

   // Equality
    bool operator ==(const DateTime &) const;
    bool operator !=(const DateTime &) const;
    
    bool operator ==(const Val &other) const
    {
        if (type() != other.type())
            return false;
        return static_cast<const DateTime&>(other).operator==(*this);
    }

    // Comparator
    bool operator <(const DateTime &) const;
    bool operator >(const DateTime &) const;
    // Get this date time as Java milliseconds since epoch * /
    const int64_t millis() const;
    // utils
private:
    int64_t _millis;
};
};