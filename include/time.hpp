#pragma once
#include "val.hpp"
#include <stdexcept>
//
// Copyright (c) 2014, Radu Racariu, Brian Frank
// History:
//   19 Aug 2014  Radu Racariu Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

/**
 Time models a time of day tag value.

 @see <a href='http://project-haystack.org/doc/TagModel#tagKinds'>Project Haystack</a> 

*/
namespace haystack {
class Time : public Val
{
	// disable construction
    Time();
	// disable assignment
    Time& operator=(const Time &other);
protected:
    const char type() const { return 'T'; }
public:
    
    // Hour of day as 0-23
    const int hour;

    // Minute of hour as 0-59
    const int min;

    // Second of minute as 0-59
    const int sec;

    // Fractional seconds in milliseconds 0-999
    const int ms;

    // ctors
    Time(int hour, int min, int sec, int ms) : hour(hour), min(min), sec(sec), ms(ms) { enforceInit(); };
    Time(int hour, int min, int sec) : hour(hour), min(min), sec(sec), ms(0) { enforceInit(); };
    Time(int hour, int min) : hour(hour), min(min), sec(0), ms(0) { enforceInit(); };
    Time(const Time &other) : hour(other.hour), min(other.min), sec(other.sec), ms(other.ms)  { enforceInit(); };
	
    // Encode as "hh:mm:ss.FFF"
	const std::string to_zinc() const;

    // Equality
    bool operator ==(const Time &b) const;
    bool operator !=(const Time &b) const;

    bool operator ==(const Val &other) const
    {
        if (type() != other.type())
            return false;
        return static_cast<const Time&>(other).operator==(*this);
    }

    // Comparator
    bool operator <(const Time &b) const;
    bool operator >(const Time &b) const;
   
    // utils
private:
    int compareTo(const Time &that) const;

    void enforceInit()
    {
        if (hour < 0 || hour > 23)  throw std::runtime_error("Invalid hour");
        if (min  < 0 || min  > 59)  throw std::runtime_error("Invalid min");
        if (sec  < 0 || sec  > 59)  throw std::runtime_error("Invalid sec");
        if (ms   < 0 || ms   > 999) throw std::runtime_error("Invalid ms");
    }

};
};