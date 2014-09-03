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
     Uri models a URI as a string.

     @see <a href='http://project-haystack.org/doc/TagModel#tagKinds'>Project Haystack</a>
     */
    class Uri : public Val
    {
        Uri();
        // disable assigment
        Uri& operator=(const Uri &other);
        Uri(const Uri &other) : value(other.value) {};
    public:
        const char type() const { return URI_TYPE; }
        // This value
        const std::string value;

        Uri(const std::string &val) : value(val) {};

        static const Uri EMPTY;

        // Return value string.
        const std::string to_string() const;

        // Encode using double quotes and back slash escapes
        const std::string to_zinc() const;

        // Equality is value based
        bool operator ==(const Uri &other) const;
        bool operator !=(const Uri &other) const;
        bool operator < (const Val &other) const;
        bool operator > (const Val &other) const;

        bool operator ==(const std::string &other) const;

        bool operator==(const Val &other) const;

        auto_ptr_t clone() const;

    };
};