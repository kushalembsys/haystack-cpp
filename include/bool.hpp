#pragma once
//
// Copyright (c) 2015, J2 Innovations
// Copyright (c) 2012 Brian Frank
// Licensed under the Academic Free License version 3.0
// History:
//   19 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

#include "val.hpp"

namespace haystack {
    /**
     Bool defines true/false tag values.

     @see <a href='http://project-haystack.org/doc/TagModel#tagKinds'>Project Haystack</a>
     */
    class Bool : public Val
    {
        Bool();
        // disable assignment
        Bool& operator = (const Bool &other);
        Bool(const Bool &other) : value(other.value){};
    public:
        const Type type() const { return BOOL_TYPE; }

        /**
        This bool value
        */
        const bool value;

        Bool(bool val);

        /**
        Singleton value for true
        */
        static const Bool& TRUE_VAL;

        /**
        Singleton value for false
        */
        static const Bool& FALSE_VAL;

        /**
        Encode as "true" or "false"
        */
        const std::string to_string() const;

        /**
        Encode as "T" or "F"
        */
        const std::string to_zinc() const;

        /**
        Equality
        */
        bool operator == (const Bool &other) const;
        bool operator == (const Val &other) const;

        bool operator > (const Val &other) const;
        bool operator < (const Val &other) const;

        auto_ptr_t clone() const;
    };
};