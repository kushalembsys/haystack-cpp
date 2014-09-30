#pragma once
#include "str.hpp"
#include <stdexcept>
//
// Copyright (c) 2014, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   19 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

namespace haystack {
    /**
     Ref wraps a string reference identifier and a optional display name.

     @see <a href='http://project-haystack.org/doc/TagModel#tagKinds'>Project Haystack</a>
     */
    class Ref : public Val
    {
        Ref();
        // disable assignment
        Ref& operator=(const Ref &other);
        Ref(const Ref &other) : value(other.value), m_dis(other.m_dis) { Ref::enforceId(); };
    public:
        const Type type() const { return REF_TYPE; }

        /**
        This string value
        */
        const std::string	value;


        Ref(const std::string& val) : value(val), m_dis("") { Ref::enforceId(); };
        Ref(const std::string& val, const std::string &unit) : value(val), m_dis(unit) { Ref::enforceId(); };

        /**
        Encode as "@id"
        */
        const std::string to_code() const { return "@" + value; }

        /**
        Return the val string
        */
        const std::string to_string() const;

        /**
        Encode value to zinc format
        */
        const std::string to_zinc() const;

        /**
        Return display string which is dis field if nont empty, val field otherwise
        */
        const std::string dis() const;

        /**
        Equality
        */
        bool operator ==(const Ref &other) const;
        bool operator==(const Val &other) const;
        bool operator > (const Val &other) const;
        bool operator < (const Val &other) const;

        auto_ptr_t clone() const;

        static bool is_id_char(int c);
        /**
        check if str is a valid id
        */
        static bool is_id(const std::string&);

        // utils
    private:
        // This Str dis
        const Str m_dis;
        void enforceId()
        {
            if (!Ref::is_id(value))
                throw std::runtime_error("invalid id");
        }
    };

    // generic clone function
    inline Ref* new_clone(const Ref& other)
    {
        return (Ref*)other.clone().release();
    }
};