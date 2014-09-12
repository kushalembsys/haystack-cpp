//
// Copyright (c) 2014, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   19 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//
#include "timezone.hpp"
#include <ctime>
#include <sstream>
#include <algorithm>
#include <functional>
////////////////////////////////////////////////
// TimeZone
////////////////////////////////////////////////
using namespace haystack;


////////////////////////////////////////////////
// Equal
////////////////////////////////////////////////
bool TimeZone::operator ==(const TimeZone &other) const
{
    return name == other.name && offset == other.offset;
}

bool TimeZone::operator !=(const TimeZone &other) const
{
    return !(*this == other);
}


char upp(char input)
{
    return (char)toupper(input);
}

int TimeZone::detect_gmt_offset(std::string name)
{
    std::transform(name.begin(), name.end(), name.begin(), std::ptr_fun <char, char>(upp));
    size_t pos = name.find("GMT");
    int offset = 0;
    if (pos != std::string::npos)
    {
        offset = std::strtol(name.substr(pos + 3).c_str(), NULL, 10);
    }

    return offset;
}

////////////////////////////////////////////////
// Static
////////////////////////////////////////////////

static const TimeZone currentTZ()
{
    time_t time = std::time(NULL);
    const tm *l = std::localtime(&time);
    const int local = l->tm_hour;
    const int dst = l->tm_isdst;
    const int gmt = std::gmtime(&time)->tm_hour;

    int offset = local - gmt - dst;

    std::stringstream os;
    os << "GMT";
    if (offset >= 0)
        os << '+';
    else
        os << '-';
    os << offset;

    return TimeZone(os.str(), offset);
}

const TimeZone TimeZone::UTC = TimeZone("UTC", 0);

const TimeZone TimeZone::DEFAULT = TimeZone(currentTZ());

