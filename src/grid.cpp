//
// Copyright (c) 2014, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   28 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//
#include "grid.hpp"
#include "str.hpp"
#include <stdexcept>

////////////////////////////////////////////////
// Grid
////////////////////////////////////////////////
using namespace haystack;

//public:
//////////////////////////////////////////////////////////////////////////
// Access
//////////////////////////////////////////////////////////////////////////

// Return grid level meta
Dict& Grid::meta() { return m_meta; }
const Dict& Grid::meta() const { return m_meta; }

// Error grid have the meta.err marker tag
const bool Grid::is_err() const { return m_meta.has("err"); }

// Return if number of rows is zero
const bool Grid::is_empty() const { return num_rows() == 0; }

// Return number of rows
const size_t Grid::num_rows() const { return m_rows.size(); }

// Get a row by its zero based index
const Row& Grid::row(size_t row) const { return m_rows[row]; }

// Get number of columns
const size_t Grid::num_cols() const { return m_cols.size(); }

// Get a column by its index
const Col& Grid::col(size_t index) const { return m_cols[index]; }

// Convenience for "col(name, true)"
const Col* const Grid::col(const std::string& name) const { return col(name, true); }

// Get a column by name.  If not found and checked if false then
// return null, otherwise throw exception
const Col* const Grid::col(const std::string& name, bool checked) const
{
    name_col_map_t::const_iterator it = m_cols_by_name.find(name);
    if (it != m_cols_by_name.end()) return &m_cols[it->second];
    if (checked) throw std::runtime_error(name);
    return NULL;
}

//////////////////////////////////////////////////////////////////////////
// Iterator
//////////////////////////////////////////////////////////////////////////

Grid::const_iterator Grid::begin() const  { return m_rows.begin(); }
Grid::const_iterator Grid::end() const { return m_rows.end(); }

//////////////////////////////////////////////////////////////////////////
// Construction
//////////////////////////////////////////////////////////////////////////

// Add new column and return builder for column metadata.
//Columns cannot be added after adding the first row.
Dict& Grid::addCol(const std::string& name)
{
    if (m_rows.size() > 0)
        throw std::runtime_error("Cannot add cols after rows have been added");
    if (!Dict::is_tag_name(name))
        throw  std::runtime_error("Invalid column name: ");

    if (m_cols_by_name.find(name) != m_cols_by_name.end())
        throw std::runtime_error("Duplicate col name: ");

    size_t index = m_cols.size();

    // col is owned by m_cols vector
    Col* col = new Col(index, name, std::auto_ptr<Dict>(new Dict()));
    m_cols.push_back(col);

    m_cols_by_name.insert(std::pair<std::string, size_t>(name, index));

    return (Dict&)col->meta();
}

// Add new row with array of cells which correspond to column
// order.  Return this.
Grid& Grid::addRow(Val * valp[], size_t count)
{
    std::auto_ptr<Row::val_vec_t> v(new Row::val_vec_t());

    for (size_t i = 0; i < count; i++)
    {
        v->push_back(valp[i]);
    }

    m_rows.push_back(new Row(*this, v));

    return *this;
}
