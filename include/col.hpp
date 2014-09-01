#pragma once
#include "headers.hpp"
#include "boost/noncopyable.hpp"

//
// Copyright (c) 2014, Radu Racariu, Brian Frank
// History:
//   28 Aug 2014  Radu Racariu Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

/**
 Col is a column in a Grid.

 @see <a href='http://project-haystack.org/doc/TagModel#tagKinds'>Project Haystack</a>
*/
namespace haystack {
class Dict;
class Col : boost::noncopyable
{
    friend class Grid;
    friend class Row;
    //////////////////////////////////////////////////////////////////////////
    // Fields
    //////////////////////////////////////////////////////////////////////////
    const size_t _index;
    const std::string _name;
    const Dict& _meta;

    //////////////////////////////////////////////////////////////////////////
    // Constructor
    //////////////////////////////////////////////////////////////////////////

    // Private constructor
    Col(size_t index, const std::string& name, const Dict& meta) : _index(index), _name(name), _meta(meta){}

public:
    //////////////////////////////////////////////////////////////////////////
    // Access
    //////////////////////////////////////////////////////////////////////////

    // Return programatic name of column
    const std::string name() const;

    // Return display name of column which is meta.dis or name
    const std::string dis() const;

    // Column meta-data tags
    const Dict& meta() const;

    // Equality is name and meta
    bool operator== (const Col& that);
    bool operator!= (const Col& that);
};
};