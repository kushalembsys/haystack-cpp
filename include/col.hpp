#pragma once
//
// Copyright (c) 2015, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   28 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

#include "headers.hpp"
#include "boost/noncopyable.hpp"

namespace haystack {
    class Dict;
    /**
     Col is a column in a Grid.

     @see <a href='http://project-haystack.org/doc/TagModel#tagKinds'>Project Haystack</a>
     */
    class Col : boost::noncopyable
    {
        friend class Grid;
        friend class Row;

        //////////////////////////////////////////////////////////////////////////
        // Constructor
        //////////////////////////////////////////////////////////////////////////

        // Private constructor
        Col(size_t index, const std::string& name, std::auto_ptr<Dict> meta) :
            m_index(index), m_name(name), m_meta(meta){}

    public:
        //////////////////////////////////////////////////////////////////////////
        // Access
        //////////////////////////////////////////////////////////////////////////

        /**
        Return programatic name of column
        */
        const std::string name() const;

        /**
        Return display name of column which is meta.dis or name
        */
        const std::string dis() const;

        /**
        Column meta-data tags
        */
        const Dict& meta() const;

        /**
        Equality is name and meta
        */
        bool operator == (const Col& that);
        bool operator != (const Col& that);

        //////////////////////////////////////////////////////////////////////////
        // Fields
        //////////////////////////////////////////////////////////////////////////
        const size_t m_index;
        const std::string m_name;
        const std::auto_ptr<Dict> m_meta;
    };
};