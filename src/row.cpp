//
// Copyright (c) 2014, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   28 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//
#include "grid.hpp"
#include "num.hpp"
#include "str.hpp"

////////////////////////////////////////////////
// Row
////////////////////////////////////////////////
using namespace haystack;

// Get the grid associated with this row
const Grid& Row::grid() const { return m_grid; }

// Number of columns in grid (which may map to empty cells)
const size_t Row::size() const { return m_grid.num_cols(); }

// Get a cell by column name. 
const Val& Row::get(const std::string& name, bool checked) const
{
    const Col* col = m_grid.col(name, false);
    if (col != NULL && !m_cells->is_null(col->_index))
    {
        const Val& val = ((*m_cells)[col->_index]);
        if (!val.is_empty()) return val;
    }
    if (checked)throw std::runtime_error("Column not found: " + name);
    return EmptyVal::DEF;
}

// Get a string by column name. 
const std::string Row::get_string(const std::string& name) const
{
    const Col* col = m_grid.col(name);
    if (col != NULL && !m_cells->is_null(col->_index))
    {
        const Str* val = (Str*)&(*m_cells)[col->_index];
        if (!val->is_empty() && val->type() == STR_TYPE) 
            return val->value;
    }
    throw std::runtime_error("Column not found: " + name);
}

// Get a int by column name. 
const double Row::get_double(const std::string& name) const
{
    const Col* col = m_grid.col(name);
    if (col != NULL && !m_cells->is_null(col->_index))
    {
        const Num* val = (Num*)&(*m_cells)[col->_index];
        if (!val->is_empty() && val->type() == NUM_TYPE) return val->value;
    }
    throw std::runtime_error("Column not found: " + name);
}

// Get a cell by column.
const Val& Row::get(const Col& col) const
{
    if (col._index < m_cells->size() && !m_cells->is_null(col._index))
        return ((*m_cells)[col._index]);

    return EmptyVal::DEF;
}

// Get start it
Row::const_iterator Row::begin() const
{
    return const_row_iterator(*this);
}

// Get end it
Row::const_iterator Row::end() const
{
    return const_row_iterator(*this, m_cells->size());
}

//////////////////////////////////////////////////////////////////////////
// RowIterator
//////////////////////////////////////////////////////////////////////////
const_row_iterator::const_row_iterator(const Row& r, size_t pos)
    : m_row(r), m_pos(pos), m_grid(r.grid()) {}

void const_row_iterator::increment() { m_pos++; }

bool const_row_iterator::equal(const_row_iterator const& other) const
{
    return m_pos == other.m_pos && &m_row == &other.m_row;
}

const Val& const_row_iterator::dereference() const
{
    return m_row.get(m_grid.col(m_pos));
}