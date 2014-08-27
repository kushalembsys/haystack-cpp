//
// Copyright (c) 2014, Radu Racariu, Brian Frank
// History:
//   06 Jun 2011  Brian Frank  Creation
//   19 Aug 2014  Radu Racariu Ported to C++
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
    if (hour < 10) os << '0'; os <<  hour << ':';
    if (min  < 10) os << '0'; os << min << ':';
    if (sec  < 10) os << '0'; os << sec;
    if (ms != 0)
    {
        os << '.';
        if (ms < 10) os << '0';
        if (ms < 100) os << '0';
        os << ms;
    }

    return os.str();
}

////////////////////////////////////////////////
// Equal
////////////////////////////////////////////////
bool Time::operator ==(const Time &other) const
{
    return (hour == other.hour && min == other.min && sec == other.sec && ms == other.ms);
}

bool Time::operator !=(const Time &other) const
{
    return !(*this == other);
}

////////////////////////////////////////////////
// Comparators
////////////////////////////////////////////////
bool Time::operator <(const Time &other) const
{
    return compareTo(other) < 0;
}

bool Time::operator >(const Time &other) const
{
    return compareTo(other) > 0;
}

int Time::compareTo(const Time &other) const
{
    if (hour < other.hour) return -1; else if (hour > other.hour) return 1;
    if (min < other.min)   return -1; else if (min > other.min)   return 1;
    if (sec < other.sec)   return -1; else if (sec > other.sec)   return 1;
    if (ms < other.ms)     return -1; else if (ms > other.ms)     return 1;

    return 0;
}
