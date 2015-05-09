#pragma once
//
// Copyright (c) 2015, J2 Innovations
// Copyright (c) 2012 Brian Frank
// Licensed under the Academic Free License version 3.0
// History:
//   19 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

#include "headers.hpp"
#include "str.hpp"

namespace haystack {
    /**
     TimeZone.

     @see <a href='http://project-haystack.org/doc/TagModel#tagKinds'>Project Haystack</a>
     */
    class TimeZone
    {
        TimeZone();
        // disable assignment
        TimeZone& operator = (const TimeZone &other);

    public:
        /**
        This string name
        */
        const std::string	name;
        /**
        This int offset
        */
        const int           offset;

        TimeZone(const std::string& name) : name(name), offset(detect_gmt_offset(name)) {};
        TimeZone(const std::string& name, const int offset) : name(name), offset(offset) {};
        TimeZone(const TimeZone &other) : name(other.name), offset(other.offset) {};

        /**
        UTC timezone
        */
        static const TimeZone UTC;
        /**
        Default timezone for the machine
        */
        static const TimeZone DEFAULT;
        /**
        Equality
        */
        bool operator == (const TimeZone &other) const;
        bool operator != (const TimeZone &other) const;

    private:
        int detect_gmt_offset(std::string name);

    };
};