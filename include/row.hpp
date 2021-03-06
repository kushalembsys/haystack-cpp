#pragma once
//
// Copyright (c) 2015, J2 Innovations
// Copyright (c) 2012 Brian Frank
// Licensed under the Academic Free License version 3.0
// History:
//   28 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

#include "headers.hpp"
#include "dict.hpp"
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/iterator/iterator_facade.hpp>

namespace haystack {

    // fwd dec
    class Grid;
    class Col;
    class Row;

    //////////////////////////////////////////////////////////////////////////
    // RowIterator
    //////////////////////////////////////////////////////////////////////////
    class const_row_iterator;
    class const_row_iterator
        : public boost::iterator_facade <
        const_row_iterator
        , Val const
        , boost::forward_traversal_tag
        >
    {
        friend class Row;
        explicit const_row_iterator(const Row& p, size_t pos = 0);

        friend class boost::iterator_core_access;

        void increment();

        bool equal(const_row_iterator const& other) const;

        const Val& dereference() const;

        const Row& m_row;
        size_t m_pos;
        const Grid& m_grid;
    };

    /**
     Row is a row in a Grid.  It implements the Dict interface also.

     @see <a href='http://project-haystack.org/doc/TagModel#tagKinds'>Project Haystack</a>
     */
    class Row : public Dict
    {
    public:
        typedef const_row_iterator const_iterator;

        typedef boost::ptr_vector< boost::nullable<Val> > val_vec_t;
    private:
        friend class Grid;
        // Private constructor
        Row(const Grid& grid, std::auto_ptr<val_vec_t> cells) : m_grid(grid), m_cells(cells) {}
    public:
        /**
        Get the grid associated with this row
        */
        const Grid& grid() const;

        /**
        Number of columns in grid (which may map to null cells)
        */
        const size_t size() const;

        /**
        Get a cell by column name.
        */
        const Val& get(const std::string& name, bool checked = true) const;

        /**
        Get a string by column name.
        */
        const std::string get_string(const std::string& name) const;

        /**
        Get a int by column name.
        */
        const double get_double(const std::string& name) const;

        /**
        Get a cell by column.
        */
        const Val& get(const Col& col) const;

        /**
        Encode values to zinc format
        */
        const std::string to_zinc() const;

        /**
        Get new Dict for this Row.
        */
        Dict::auto_ptr_t to_dict() const;

        /**
        Get start it
        */
        const_iterator begin() const;

        /**
        Get end it
        */
        const_iterator end() const;
        
        /**
        Equality
        */
        bool operator == (const Dict &b) const;

    private:
        // hide from base class

        Dict& add(std::string name, Val::auto_ptr_t val);
        Dict& add(std::string name, const Val* val);
        Dict& add(std::string name, const Val& val);
        Dict& add(std::string name);
        Dict& add(std::string name, const std::string& val);
        Dict& add(std::string name, double val, const std::string &unit = "");
        Dict& add(const Dict& other);
        auto_ptr_t clone() { return Dict::clone(); }

        //////////////////////////////////////////////////////////////////////////
        // Fields
        //////////////////////////////////////////////////////////////////////////

        const Grid& m_grid;
        const std::auto_ptr<val_vec_t> m_cells;
    };

};