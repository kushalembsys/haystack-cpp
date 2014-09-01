//
// Copyright (c) 2014, Radu Racariu, Brian Frank
// History:
//   28 Aug 2014  Radu Racariu Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//
#include "row.hpp"
#include "col.hpp"
#include "grid.hpp"

////////////////////////////////////////////////
// Row
////////////////////////////////////////////////
using namespace haystack;

// Get the grid associated with this row
const Grid& Row::grid() const { return m_grid; }

// Number of columns in grid (which may map to null cells)
size_t Row::size() const { return m_grid.m_cols.size(); }

// Get a cell by column name. 
const Val& Row::get(const std::string& name) const 
{
    const Col* col = m_grid.col(name);
    if (col != NULL)
    {
        const Val& val = m_cells[col->_index];
        if (!(val == EmptyVal::DEF)) return val;
    }
    return EmptyVal::DEF;
}

// Get a cell by column.
const Val& Row::get(const Col& col) const
{
    const Val& val = m_cells[col._index];
    return val;
}

// Return name/value iterator which only includes
Row::const_iterator Row::iterator() const
{
    return const_row_iterator(*this);
}

    // Get start it
Row::const_iterator Row::begin() const
{
    return const_row_iterator(*this);
}

    // Get end it
Row::const_iterator Row::end() const
{
    return const_row_iterator(*this, m_cells.size());
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