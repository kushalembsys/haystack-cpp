//
// Copyright (c) 2015, J2 Innovations
// Copyright (c) 2012 Brian Frank
// Licensed under the Academic Free License version 3.0
// History:
//   28 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//
#include "grid.hpp"
#include "num.hpp"
#include "marker.hpp"
#include "str.hpp"
#include <sstream>

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
    if (col != NULL && !m_cells->is_null(col->m_index))
    {
        const Val& val = ((*m_cells)[col->m_index]);
        if (!val.is_empty()) return val;
    }
    if (checked)throw std::runtime_error("Column not found: " + name);
    return EmptyVal::DEF;
}

// Get a string by column name. 
const std::string Row::get_string(const std::string& name) const
{
    const Col* col = m_grid.col(name);
    if (col != NULL && !m_cells->is_null(col->m_index))
    {
        const Str* val = (Str*)&(*m_cells)[col->m_index];
        if (!val->is_empty() && val->type() == Val::STR_TYPE)
            return val->value;
    }
    throw std::runtime_error("Column not found: " + name);
}

// Get a int by column name. 
const double Row::get_double(const std::string& name) const
{
    const Col* col = m_grid.col(name);
    if (col != NULL && !m_cells->is_null(col->m_index))
    {
        const Num* val = (Num*)&(*m_cells)[col->m_index];
        if (!val->is_empty() && val->type() == Val::NUM_TYPE) return val->value;
    }
    throw std::runtime_error("Column not found: " + name);
}

// Get a cell by column.
const Val& Row::get(const Col& col) const
{
    if (col.m_index < m_cells->size() && !m_cells->is_null(col.m_index))
        return ((*m_cells)[col.m_index]);

    return EmptyVal::DEF;
}

//Encode values to zinc format
const std::string Row::to_zinc() const
{
    const size_t n_cols = m_grid.num_cols();
    std::stringstream os;
    bool first = true;

    for (size_t i = 0; i < n_cols; i++)
    {
        const Col& c = m_grid.col(i);
        const std::string& name = c.name();
        const Val& val = get(c);
        if (first)
            first = false;
        else os << ' ';

        os << name;
        if (val != Marker::VAL)
            os << ':' << val.to_zinc();
    }
    return os.str();
}

// Get new Dict from this Row.
Dict::auto_ptr_t Row::to_dict() const
{
    Dict::auto_ptr_t d(new Dict);

    const size_t n_cols = m_grid.num_cols();

    for (size_t i = 0; i < n_cols; i++)
    {
        const Col& c = m_grid.col(i);
        d->add(c.name(), get(c));
    }

    return d;
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

// Equality
bool Row::operator == (const Dict &other) const
{
    if (size() != other.size()) return false;

    const size_t n_cols = m_grid.num_cols();

    for (size_t i = 0; i < n_cols; i++)
    {
        const Col& c = m_grid.col(i);
        const std::string& name = c.name();
        const Val& val = get(c);
        if (val != other.get(name, false))
            return false;
    }
    return true;
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