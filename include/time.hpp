#pragma once
#include "val.hpp"
#include <stdexcept>
//
// Copyright (c) 2014, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   19 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

namespace haystack {
    /*
     Time models a time of day tag value.

     @see <a href='http://project-haystack.org/doc/TagModel#tagKinds'>Project Haystack</a>

     */
    class Time : public Val
    {
        // disable construction
        Time();
        // disable assignment
        Time& operator=(const Time &other);
        friend class DateTime;
        Time(const Time &other) : hour(other.hour), minutes(other.minutes), sec(other.sec), ms(other.ms)  { enforceInit(); }
    public:
        const char type() const { return TIME_TYPE; }

        // Hour of day as 0-23
        const int hour;

        // Minute of hour as 0-59
        const int minutes;

        // Second of minute as 0-59
        const int sec;

        // Fractional seconds in milliseconds 0-999
        const int ms;

        // ctors
        Time(int hour, int minutes, int sec, int ms) : hour(hour), minutes(minutes), sec(sec), ms(ms) { enforceInit(); }
        Time(int hour, int minutes, int sec) : hour(hour), minutes(minutes), sec(sec), ms(0) { enforceInit(); }
        Time(int hour, int minutes) : hour(hour), minutes(minutes), sec(0), ms(0) { enforceInit(); }
        

        // Encode as "hh:mm:ss.FFF"
        const std::string to_zinc() const;

        // Equality
        bool operator ==(const Time &b) const;
        bool operator !=(const Time &b) const;

        bool operator==(const Val &other) const;

        // Comparator
        bool operator <(const Val &b) const;
        bool operator >(const Val &b) const;

        auto_ptr_t clone() const;

        // utils
    private:
        int compareTo(const Time &that) const;

        void enforceInit()
        {
            if (hour < 0 || hour > 23)  throw std::runtime_error("Invalid hour");
            if (minutes < 0 || minutes  > 59)  throw std::runtime_error("Invalid min");
            if (sec < 0 || sec  > 59)  throw std::runtime_error("Invalid sec");
            if (ms < 0 || ms   > 999) throw std::runtime_error("Invalid ms");
        }

    };
};