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
     Bin models a binary file with a MIME type.

     @see <a href='http://project-haystack.org/doc/TagModel#tagKinds'>Project Haystack</a>

     */
    class Bin : public Val
    {
        Bin();
        // disable assignment
        Bin& operator=(const Bin &other);
        Bin(const Bin& other) : value(other.value) {};
    public:
        const char type() const { return BIN_TYPE; }

        // This string value
        const std::string value;
        // Construct from std::string
        Bin(const std::string &val);

        // MIME type for binary file
        const std::string to_string() const;

        // Encode as "Bin(<mime>)" 
        const std::string to_zinc() const;

        // Equality is value based
        bool operator ==(const Bin &b) const;
        bool operator !=(const Bin &b) const;
        bool operator ==(const std::string &other) const;
        bool operator==(const Val &other) const;

        bool operator > (const Val &other) const;
        bool operator < (const Val &other) const;

        auto_ptr_t clone() const;
    };
};