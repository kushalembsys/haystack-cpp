//
// Copyright (c) 2014, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   06 Jun 2011  Brian Frank  Creation
//   19 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++
//
#include "time.hpp"
#include <sstream>
#include <ctime>


////////////////////////////////////////////////
// Time
////////////////////////////////////////////////
using namespace haystack;

////////////////////////////////////////////////
// to zinc
////////////////////////////////////////////////

// Encode as "hh:mm:ss.FFF"
const std::string Time::to_zinc() const
{
    std::stringstream os;
    if (hour < 10) os << '0'; os << hour << ':';
    if (minutes < 10) os << '0'; os << minutes << ':';
    if (sec < 10) os << '0'; os << sec;
    if (ms != 0)
    {
        os << '.';
        if (ms < 10) os << '0';
        if (ms < 100) os << '0';
        os << ms;
    }

    return os.str();
}

const Time& Time::MIDNIGHT = *new Time(0, 0, 0);

////////////////////////////////////////////////
// Equal
////////////////////////////////////////////////
bool Time::operator ==(const Time &other) const
{
    return (hour == other.hour && minutes == other.minutes && sec == other.sec && ms == other.ms);
}

bool Time::operator==(const Val &other) const
{
    if (type() != other.type())
        return false;
    return static_cast<const Time&>(other).operator==(*this);
}

bool Time::operator !=(const Time &other) const
{
    return !(*this == other);
}

////////////////////////////////////////////////
// Comparators
////////////////////////////////////////////////
bool Time::operator <(const Val &other) const
{
    return type() == other.type() && compareTo(((Time&)other)) < 0;
}

bool Time::operator >(const Val &other) const
{
    return type() == other.type() && compareTo(((Time&)other)) > 0;
}

Time::auto_ptr_t Time::clone() const
{
    return auto_ptr_t(new Time(*this));
}

int Time::compareTo(const Time &other) const
{
    if (hour < other.hour) return -1; else if (hour > other.hour) return 1;
    if (minutes < other.minutes)   return -1; else if (minutes > other.minutes)   return 1;
    if (sec < other.sec)   return -1; else if (sec > other.sec)   return 1;
    if (ms < other.ms)     return -1; else if (ms > other.ms)     return 1;

    return 0;
}
