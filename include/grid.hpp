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
    /**
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

        /**
        Return grid level meta
        */
        Dict& meta();
        virtual const Dict& meta() const;

        /**
        Error grid have the meta.err marker tag
        */
        virtual const bool is_err() const;

        /**
        Return if number of rows is zero
        */
        virtual const bool is_empty() const;

        /**
        Return number of rows
        */
        virtual const size_t num_rows() const;

        /**
        Get a row by its zero based index
        */
        virtual const Row& row(size_t row) const;

        /**
        Get number of columns
        */
        virtual const size_t num_cols() const;

        /**
        Get a column by its index
        */
        virtual const Col& col(size_t index) const;

        /**
        Convenience for "col(name, true)"
        */
        const Col* const col(const std::string& name) const;

        /**
        Get a column by name.  If not found and checked if false then
        return null, otherwise throw exception
        */
        const Col* const col(const std::string& name, bool checked) const;

        //////////////////////////////////////////////////////////////////////////
        // Iterator
        //////////////////////////////////////////////////////////////////////////

        /**
        Return rows iterator
        */
        const_iterator begin() const;
        const_iterator end() const;

        //////////////////////////////////////////////////////////////////////////
        // Construction
        //////////////////////////////////////////////////////////////////////////

        /**
        Add new column and return builder for column metadata.
        Columns cannot be added after adding the first row.
        */
        Dict& add_col(const std::string& name);

        /**
        Add new row with array of cells which correspond to column
        order.  Return this.
        */
        Grid& add_row(Val *[], size_t count);

        /**
        Tell grid to allocate space for this number of rows.
        */
        void reserve_rows(size_t count);

        /**
        Constructs an err grid
        */
        static Grid::auto_ptr_t make_err(const std::runtime_error&);
        /**
        Constructs grid from Dict
        */
        static Grid::auto_ptr_t make(const Dict&);
        /**
        Constructs grid from Dicts* vector
        */
        static Grid::auto_ptr_t make(const std::vector<const Dict*>&);
        /**
        Constructs grid from Dicts ptr_vector
        */
        static Grid::auto_ptr_t make(const boost::ptr_vector<Dict>&);

        static const Grid& EMPTY;

        virtual ~Grid(){}

    private:
        // Add new row with values from the Dict; Dict column
        // should match this grid columns.
        // Return this.
        Grid& add_row(const Dict&);

        //////////////////////////////////////////////////////////////////////////
        // Rows
        //////////////////////////////////////////////////////////////////////////
        friend class const_grid_iterator;
        friend class GridView;
        row_vec_t m_rows;
        col_vec_t m_cols;
        name_col_map_t m_cols_by_name;
        Dict m_meta;
    };

    /**
    GridView wrapps a haystack::Grid so it's memebers can't be mutated.
    It doesn't own it's haystack::Val, they are owned by the Grid that
    was used to construct this GridView. Users must make sure that the source
    haystack::Grid outlives this GridView.
    */
    class GridView : public Grid
    {
    public:
        typedef std::vector<const Row*> row_vec_t;
        typedef std::vector<const Col*> col_vec_t;

        // really it is a const iterator
        typedef row_vec_t::const_iterator iterator;
        typedef row_vec_t::const_iterator const_iterator;

        //////////////////////////////////////////////////////////////////////////
        // Access
        //////////////////////////////////////////////////////////////////////////

        /**
        Return grid level meta
        */
        const Dict& meta() const
        {
            return m_meta;
        }

        /**
        Error grid have the meta.err marker tag
        */
        const bool is_err() const
        {
            return m_meta.has("err");
        }

        /**
        Return if number of rows is zero
        */
        const bool is_empty() const
        {
            return num_rows() == 0;
        }

        /**
        Return number of rows
        */
        const size_t num_rows() const
        {
            return m_rows.size();
        }

        /**
        Get a row by its zero based index
        */
        const Row& row(size_t row) const
        {
            return *m_rows[row];
        }

        /**
        Get number of columns
        */
        const size_t num_cols() const
        {
            return m_cols.size();
        }

        /**
        Get a column by its index
        */
        const Col& col(size_t index) const
        {
            if (index < m_cols.size())
                return *m_cols[index];
            else
                throw std::runtime_error("Col index out of bounds.");
        }

        /**
        Return rows iterator
        */
        const_iterator begin() const { return m_rows.begin(); }
        const_iterator end() const { return m_rows.end(); }

        GridView(const Grid& g) :
            m_meta(g.m_meta),
            m_cols_by_name(g.m_cols_by_name)
        {
            m_rows.reserve(g.m_rows.size());
            m_cols.reserve(g.m_cols.size());

            for (Grid::row_vec_t::const_iterator it = g.m_rows.begin(); it != g.m_rows.end(); ++it)
            {
                m_rows.push_back(&*it);
            }

            for (Grid::col_vec_t::const_iterator it = g.m_cols.begin(); it != g.m_cols.end(); ++it)
            {
                m_cols.push_back(&*it);
            }
        }

        GridView(const GridView& gv) : m_meta(gv.m_meta),
            m_rows(gv.m_rows),
            m_cols(gv.m_cols),
            m_cols_by_name(gv.m_cols_by_name) {}

    private:
        const Dict& m_meta;
        row_vec_t m_rows;
        col_vec_t m_cols;
        const name_col_map_t& m_cols_by_name;
        // hide base methods
        Dict& add_col(const std::string& name);
        Grid& add_row(Val *[], size_t count);
        void reserve_rows(size_t count);
        
        static Grid::auto_ptr_t make_err(const std::runtime_error&);
        static Grid::auto_ptr_t make(const Dict&);
        static Grid::auto_ptr_t make(const std::vector<const Dict*>&);
        static Grid::auto_ptr_t make(const boost::ptr_vector<Dict>&);
    };
};