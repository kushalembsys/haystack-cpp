#pragma once
#include "headers.hpp"
#include "dict.hpp"
#include "row.hpp"
#include "col.hpp"
#include <boost/ptr_container/ptr_vector.hpp>
#include <stdexcept>
//
// Copyright (c) 2014, Radu Racariu, Brian Frank
// History:
//   28 Aug 2014  Radu Racariu Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

/**
 Grid two dimension data structure of cols and rows.

 @see <a href='http://project-haystack.org/doc/TagModel#tagKinds'>Project Haystack</a>
*/
namespace haystack {
class Grid : boost::noncopyable
{
public:
    typedef std::map<std::string, size_t> name_col_map_t;
    typedef boost::ptr_vector<Row> row_vec_t;
    typedef boost::ptr_vector<Col> col_vec_t;

    typedef row_vec_t::const_iterator const_iterator;

    //////////////////////////////////////////////////////////////////////////
    // Rows
    //////////////////////////////////////////////////////////////////////////

    row_vec_t m_rows;
    col_vec_t m_cols;
    name_col_map_t m_colsByName;
    Dict m_meta;

public:
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

    const_iterator begin() const;
    const_iterator end() const;

    //////////////////////////////////////////////////////////////////////////
    // Construction
    //////////////////////////////////////////////////////////////////////////

    // Add new column and return builder for column metadata.
    //Columns cannot be added after adding the first row. */
    Dict& addCol(const std::string& name);

    // Add new row with array of cells which correspond to column
    // order.  Return this.
    Grid& addRow(Val * [], size_t count);
};
};