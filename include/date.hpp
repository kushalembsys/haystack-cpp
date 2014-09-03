#pragma once
#include "val.hpp"

//
// Copyright (c) 2014, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   19 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

namespace haystack {
    /*
     Date models a date (day in year) tag value.

     @see <a href='http://project-haystack.org/doc/TagModel#tagKinds'>Project Haystack</a>

     */
    class Date : public Val
    {
        // disable construction
        Date();
        // disable assignment
        Date& operator=(const Date &other);
        friend class DateTime;
        Date(const Date &other) : year(other.year), month(other.month), day(other.day) {};
    public:
        const char type() const { return DATE_TYPE; }

        // Four digit year such as 2014
        const int year;

        // Month as 1-12 (Jan is 1, Dec is 12)
        const int month;

        // Day of month as 1-31
        const int day;

        // ctors
        Date(int year, int month, int day) : year(year), month(month), day(day) {};

        // Encode as "YYYY-MM-DD"
        const std::string to_zinc() const;

        // Return date in future given number of days
        Date inc_days(int) const;
        // Return date in past given number of days
        Date dec_days(int) const;

        // Return day of week: Sunday is 1, Saturday is 7
        int weekday() const;

        // Return if given year a leap year
        static bool is_leap_year(int year);
        // Return number of days in given year (xxxx) and month (1-12)
        static int days_in_month(int year, int mon);

        // Equality
        bool operator ==(const Date &b) const;
        bool operator !=(const Date &b) const;

        // Comparator
        bool operator <(const Val &b) const;
        bool operator >(const Val &b) const;

        bool operator==(const Val &other) const;

        auto_ptr_t clone() const;

        // utils
    private:
        int compareTo(const Date &that) const;

    };
};