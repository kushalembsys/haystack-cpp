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
     Str wraps a std::string as a tag value.

     @see <a href='http://project-haystack.org/doc/TagModel#tagKinds'>Project Haystack</a>
     */
    class Str : public Val
    {
        Str();
        // disable assigment
        Str& operator=(const Str &other);
        friend class Ref;
        Str(const Str &other) : value(other.value) {};
    public:
        const Type type() const { return STR_TYPE; }

        // This string value
        const std::string value;

        Str(const std::string &val);

        static const Str& EMPTY;

        // Return value string.
        const std::string to_string() const;

        // Encode using double quotes and back slash escapes
        const std::string to_zinc() const;

        // Equality is value based
        bool operator ==(const Str &other) const;
        bool operator !=(const Str &other) const;
        bool operator < (const Val &other) const;
        bool operator > (const Val &other) const;

        bool operator ==(const std::string &other) const;
        bool operator==(const Val &other) const;
        auto_ptr_t clone() const;

        // implicit conversion to std::string
        //operator const std::string() const;
    };
};