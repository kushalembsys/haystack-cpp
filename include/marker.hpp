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
    /**
     Marker is the singleton value for a marker tag.

     @see <a href='http://project-haystack.org/doc/TagModel#tagKinds'>Project Haystack</a>

     */
    class Marker : public Val
    {
        // disable copy ctor
        Marker(const Marker&);
        // disable assignment
        Marker& operator=(const Marker &other);
    public:
        Marker() {};
        const Type type() const { return MARKER_TYPE; }

        /**
        default marker value
        */
        static const Marker& VAL;

        /**
        Encode as "marker"
        */
        const std::string to_string() const;

        /**
        Encode as "M"
        */
        const std::string to_zinc() const;

        /**
        Equality
        */
        bool operator ==(const Marker &b) const;
        bool operator==(const Val &other) const;
        bool operator > (const Val &other) const;
        bool operator < (const Val &other) const;

        auto_ptr_t clone() const;
    };
};