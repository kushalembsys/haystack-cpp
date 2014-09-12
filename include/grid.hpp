#pragma once
#include "headers.hpp"
#include "dict.hpp"
#include "row.hpp"
#include "col.hpp"
#include <stdexcept>
#include <boost/ptr_container/ptr_vector.hpp>
//
// Copyright (c) 2014, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   28 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

namespace haystack {
    /*
     Grid a two dimension data structure of cols and rows.

     @see <a href='http://project-haystack.org/doc/TagModel#tagKinds'>Project Haystack</a>
     */
    class Grid : boost::noncopyable
    {
    public:

        typedef boost::ptr_vector<Row> row_vec_t;
        typedef boost::ptr_vector<Col> col_vec_t;
        typedef std::map<std::string, size_t> name_col_map_t;

        // really it is a const iterator
        typedef row_vec_t::const_iterator iterator;
        typedef row_vec_t::const_iterator const_iterator;

        typedef std::auto_ptr<Grid> auto_ptr_t;

        //////////////////////////////////////////////////////////////////////////
        // Access
        //////////////////////////////////////////////////////////////////////////

        // Return grid level meta
        Dict& meta();
        const Dict& meta() const;

        // Error grid have the meta.err marker tag
        const bool is_err() const;

        // Return if number of rows is zero
        const bool is_empty() const;

        // Return number of rows
        const size_t num_rows() const;

        // Get a row by its zero based index
        const Row& row(size_t row) const;

        // Get number of columns
        const size_t num_cols() const;

        // Get a column by its index
        const Col& col(size_t index) const;

        // Convenience for "col(name, true)"
        const Col* const col(const std::string& name) const;

        // Get a column by name.  If not found and checked if false then
        // return null, otherwise throw exception
        const Col* const col(const std::string& name, bool checked) const;

        //////////////////////////////////////////////////////////////////////////
        // Iterator
        //////////////////////////////////////////////////////////////////////////

        // Return rows iterator
        const_iterator begin() const;
        const_iterator end() const;

        //////////////////////////////////////////////////////////////////////////
        // Construction
        //////////////////////////////////////////////////////////////////////////

        // Add new column and return builder for column metadata.
        //Columns cannot be added after adding the first row. */
        Dict& add_col(const std::string& name);

        // Add new row with array of cells which correspond to column
        // order.  Return this.
        Grid& add_row(Val *[], size_t count);

        // Tell grid to allocate space for this abount of rows entries.
        void reserve_rows(size_t count);

        // Constructs an err grid
        static Grid::auto_ptr_t make_err(const std::runtime_error&);
        // Constructs grid from Dict
        static Grid::auto_ptr_t make(const Dict&);
        // Constructs grid from Dicts vector
        static Grid::auto_ptr_t make(const std::vector<const Dict*>&);
        static Grid::auto_ptr_t make(const boost::ptr_vector<Dict>&);

    private:
        // Add new row with values from the Dict; Dict column
        // should match this grid columns.
        // Return this.
        Grid& add_row(const Dict&);

        //////////////////////////////////////////////////////////////////////////
        // Rows
        //////////////////////////////////////////////////////////////////////////
        friend class const_grid_iterator;
        row_vec_t m_rows;
        col_vec_t m_cols;
        name_col_map_t m_cols_by_name;
        Dict m_meta;
    };
};