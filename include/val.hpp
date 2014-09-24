#pragma once
#include "headers.hpp"
#include<assert.h>

//
// Copyright (c) 2014, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   19 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

namespace haystack {

    /**
     Val is the base class for representing haystack tag
     scalar values as an immutable class.

     @see <a href='http://project-haystack.org/doc/TagModel#tagKinds'>Project Haystack</a>
     */
    class Val : boost::noncopyable
    {
        // disable copy ctor
    public:
        // Define Val type enumeration for all types
        enum Type
        {
            BOOL_TYPE = 'B',
            BIN_TYPE = 'b',
            COORD_TYPE = 'C',
            DATE_TIME_TYPE = 'd',
            DATE_TYPE = 'D',
            MARKER_TYPE = 'M',
            NUM_TYPE = 'N',
            REF_TYPE = 'R',
            STR_TYPE = 'S',
            TIME_TYPE = 'T',
            URI_TYPE = 'U',
            EMPTY_TYPE = '|'
        };

        typedef std::auto_ptr<Val> auto_ptr_t;

        virtual ~Val() {};
        /**
        String format is for human consumption only
        */
        virtual const std::string to_string() const { return to_zinc(); }

        /**
        Encode value to zinc format
        */
        virtual const std::string to_zinc() const = 0;
        /**
        Return this Val type
        */
        virtual const Type type() const = 0;

        virtual bool operator==(const Val &other) const = 0;
        virtual bool operator > (const Val &other) const = 0;
        virtual bool operator < (const Val &other) const = 0;

        /**
        Converts to a concrete type
        */
        template<class Type>
        inline const Type& as() const { return static_cast<const Type&>(*this); }

        /**
        creates an auto_ptr pointer to the cloned Val
        */
        virtual auto_ptr_t clone() const = 0;
        /**
        Check if this Val's Type is EMPTY_TYPE
        */
        const bool is_empty() const { return type() == EMPTY_TYPE; }
    };

    /**
    This class is the "empty" value for all types of Val
    */
    class EmptyVal : public Val
    {
    private:
        EmptyVal() {}
    public:
        const std::string to_string() const;

        const std::string to_zinc() const;

        static const EmptyVal &DEF;

        const Type type() const;

        bool operator==(const Val &other) const;
        bool operator > (const Val &other) const;
        bool operator < (const Val &other) const;

        auto_ptr_t clone() const;
    };

    /**
    generic clone function
    */
    inline Val* new_clone(const Val& other)
    {
        return (Val*)other.clone().release();
    }
}