//
// Copyright (c) 2014, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   19 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//
#include "date.hpp"
#include <sstream>
#include <ctime>

//
// Static
//
static const int daysInMon[] = { -1, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
static const int daysInMonLeap[] = { -1, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

////////////////////////////////////////////////
// Date
////////////////////////////////////////////////
using namespace haystack;

////////////////////////////////////////////////
// to zinc
////////////////////////////////////////////////

// Encode as "YYYY-MM-DD"
const std::string Date::to_zinc() const
{
    std::stringstream os;
    os << year << '-';
    if (month < 10) 
        os << '0'; 
    os << month << '-';
    if (day < 10)
        os << '0';
    os<< day;

    return os.str();
}

// Return date in future given number of days
Date Date::inc_days(int numDays) const
{
    if (numDays == 0) return *this;
    if (numDays < 0) return dec_days(-numDays);
    int year    = this->year;
    int month   = this->month;
    int day     = this->day;
    for (; numDays > 0; --numDays)
    {
        day++;
        if (day > days_in_month(year, month))
        {
            day = 1;
            month++;
            if (month > 12) { month = 1; year++; }
        }
    }
    return Date(year, month, day);
}
// Return date in past given number of days
Date Date::dec_days(int numDays) const
{
    if (numDays == 0) return *this;
    if (numDays < 0) return inc_days(-numDays);
    int year    = this->year;
    int month   = this->month;
    int day     = this->day;
    for (; numDays > 0; --numDays)
    {
        day--;
        if (day <= 0)
        {
            month--;
            if (month < 1) { month = 12; year--; }
            day = days_in_month(year, month);
        }
    }
    return Date(year, month, day);
}

int Date::weekday() const
{
    std::tm tm = { 0 /*sec*/, 0 /*min*/, 0 /*hr*/, day, month - 1, year - 1900 };
    std::time_t time = std::mktime(&tm);
    std::tm const *loc_time = std::localtime(&time);
    
    return loc_time->tm_wday + 1;
}

bool Date::is_leap_year(int year)
{
    if ((year & 3) != 0) return false;
    return (year % 100 != 0) || (year % 400 == 0);
}

int Date::days_in_month(int year, int mon)
{
    return is_leap_year(year) ? daysInMonLeap[mon] : daysInMon[mon];
}

////////////////////////////////////////////////
// Equal
////////////////////////////////////////////////
bool Date::operator ==(const Date &other) const
{
    return (year == other.year && month == other.month && day == other.day);
}

bool Date::operator==(const Val &other) const
{
    if (type() != other.type())
        return false;
    return static_cast<const Date&>(other).operator==(*this);
}

bool Date::operator !=(const Date &other) const
{
    return !(*this == other);
}

////////////////////////////////////////////////
// Comparators
////////////////////////////////////////////////
bool Date::operator <(const Date &other) const
{
    return compareTo(other) < 0;
}

bool Date::operator >(const Date &other) const
{
    return compareTo(other) > 0;
}

int Date::compareTo(const Date &other) const
{
    if (year < other.year)   return -1; else if (year > other.year)   return 1;
    if (month < other.month) return -1; else if (month > other.month) return 1;
    if (day < other.day)     return -1; else if (day > other.day)     return 1;
    return 0;
}

Date::auto_ptr_t Date::clone() const
{
    return auto_ptr_t(new Date(*this));
}