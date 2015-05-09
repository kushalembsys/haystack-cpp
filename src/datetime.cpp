//
// Copyright (c) 2015, J2 Innovations
// Copyright (c) 2012 Brian Frank
// Licensed under the Academic Free License version 3.0
// History:
//   19 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++ 
//   06 Jun 2011  Brian Frank  Creation
//
#include "datetime.hpp"
#include <sstream>
#include <ctime>
#include <cmath>
#include <limits>
#include <stdio.h>

////////////////////////////////////////////////
// DateTime
////////////////////////////////////////////////
using namespace haystack;

// make from time_t
DateTime DateTime::make(const int64_t& ts, const TimeZone& tz)
{
    time_t t = ts;
    int ms = 0;
    if (ts > (time_t)std::numeric_limits<time_t>::max)
    {
        ms = ts % 1000LL;
        t = static_cast<time_t>(ts / 1000LL);
    }

    const std::tm *tm = std::localtime(&t);
    int tz_offset = 0;
    if (tz != TimeZone::DEFAULT)
    {
        tm = std::gmtime(&t);
        tz_offset = tz.offset;
    }
    else
    {
        tz_offset = 0;
    }


    int offset = 3600 * (tz.offset + tm->tm_isdst);

    return DateTime(Date(1900 + tm->tm_year, tm->tm_mon + 1, tm->tm_mday), Time(tm->tm_hour + tz_offset, tm->tm_min, tm->tm_sec, ms), tz, offset);
}

DateTime DateTime::make_time_t(const time_t& ts, const TimeZone& tz)
{
    const std::tm *tm = std::localtime(&ts);
    
    int offset_millis = 0;
    int tz_offset = 0;
    if (tz != TimeZone::DEFAULT)
    {
        tm = std::gmtime(&ts);
        tz_offset = tz.offset;
    }
    else
    {
        tz_offset = 0;
    }

    offset_millis = 3600 * (tz.offset + tm->tm_isdst);

    return DateTime(Date(1900 + tm->tm_year, tm->tm_mon + 1, tm->tm_mday), Time(tm->tm_hour + tz_offset, tm->tm_min, tm->tm_sec, 0), tz, offset_millis);
}

DateTime DateTime::now(const TimeZone& tz)
{
    return DateTime::make_time_t(std::time(NULL), tz);
}

////////////////////////////////////////////////
// to zinc
////////////////////////////////////////////////

// Encode as "YYYY-MM-DD'T'hh:mm:ss.FFFz zzzz"
const std::string DateTime::to_zinc() const
{
    std::stringstream os;

    os << date.to_zinc();
    os << 'T';
    os << time.to_zinc();
    if (tz_offset == 0) os << 'Z';
    else
    {
        int offset = tz_offset;
        if (offset < 0) 
        { 
            os << '-';
            offset = -offset;
        }
        else 
        { 
            os << '+';
        }
        int zh = offset / 3600;
        int zm = (offset % 3600) / 60;
        if (zh < 10)  os << '0';  os << zh << ':';
        if (zm < 10)  os << '0';  os << zm;
    }
    os << ' ' << tz.name;

    return os.str();
}

////////////////////////////////////////////////
// Equal
////////////////////////////////////////////////
bool DateTime::operator ==(const DateTime &other) const
{
    return (date == other.date && time == other.time && tz == other.tz && tz_offset == other.tz_offset);
}

bool DateTime::operator==(const Val &other) const
{
    if (type() != other.type())
        return false;
    return *this == static_cast<const DateTime&>(other);
}

////////////////////////////////////////////////
// Comparators
////////////////////////////////////////////////
bool DateTime::operator <(const Val &other) const
{
    return type() == other.type() && millis() < ((DateTime&)other).millis();
}

bool DateTime::operator >(const Val &other) const
{
    return type() == other.type() && millis() > ((DateTime&)other).millis();
}

const int64_t DateTime::millis() const
{
    if (m_millis > 0)
        return m_millis;
    // lazy init millis
    std::tm tm = { time.sec /*sec*/, time.minutes /*min*/, time.hour + TimeZone::DEFAULT.offset /*hr*/, date.day, date.month - 1, date.year - 1900 };
    std::time_t t = std::mktime(&tm);
    const_cast<DateTime*>(this)->m_millis = static_cast<int64_t>((t + std::abs(tz_offset)) * 1000ULL + time.ms);
    return m_millis;
}

DateTime::auto_ptr_t DateTime::clone() const
{
    return auto_ptr_t(new DateTime(*this));
}