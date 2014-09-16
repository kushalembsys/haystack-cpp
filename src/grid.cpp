//
// Copyright (c) 2014, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   28 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//
#include "grid.hpp"
#include "str.hpp"
#include "marker.hpp"
#include <boost/scoped_ptr.hpp>

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
const Col& Grid::col(size_t index) const
{
    if (index < m_cols.size())
        return m_cols[index];
    else
        throw std::runtime_error("Col index out of bouds.");
}

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
Dict& Grid::add_col(const std::string& name)
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
Grid& Grid::add_row(Val * valp[], size_t count)
{
    std::auto_ptr<Row::val_vec_t> v(new Row::val_vec_t(count));

    for (size_t i = 0; i < count; i++)
    {
        v->push_back(valp[i]);
    }

    m_rows.push_back(new Row(*this, v));

    return *this;
}

// Tell grid to allocate space for this abount of rows entries.
void Grid::reserve_rows(size_t count)
{
    m_rows.reserve(count);
}

// Add new row with array of cells which correspond to column
// order.  Return this.
Grid& Grid::add_row(const Dict& d)
{
    if (d.is_empty())
        return *this;

    // preallocate a fixed Val* vector
    std::auto_ptr<Row::val_vec_t> v(new Row::val_vec_t(m_cols_by_name.size()));
    v->resize(m_cols_by_name.size(), NULL);

    for (name_col_map_t::const_iterator it = m_cols_by_name.begin(), e = m_cols_by_name.end(); it != e; ++it)
    {
        const Val& val = d.get(it->first, false);
        const size_t index = it->second;

        if (!val.is_empty())
            v->replace(index, new_clone(val));
        else
            v->replace(index, NULL);
    }

    m_rows.push_back(new Row(*this, v));

    return *this;
}

Grid::auto_ptr_t Grid::make_err(const std::runtime_error& e)
{
    auto_ptr_t g(new Grid());
    g->meta().add("err")
        .add("dis", e.what())
        .add("errTrace", "");
    g->add_col("empty");

    return g;
}

// Constructs grid from Dict
Grid::auto_ptr_t Grid::make(const Dict& d)
{
    auto_ptr_t g(new Grid());

    if (d.is_empty())
        return g;

    // add cols
    for (Dict::const_iterator it = d.begin(), e = d.end(); it != e; ++it)
        g->add_col(it->first);

    g->add_row(d);

    return g;
}

// Constructs grid from Dicts vector
Grid::auto_ptr_t Grid::make(const std::vector<const Dict*>& dicts)
{
    auto_ptr_t g(new Grid());

    if (dicts.empty())
        return g;

    std::map<std::string, bool> col_names;

    // add cols
    for (std::vector<const Dict*>::const_iterator dit = dicts.begin(), e = dicts.end(); dit != e; ++dit)
    {
        for (Dict::const_iterator vit = (**dit).begin(), e = (**dit).end(); vit != e; ++vit)
        {
            const std::string& col_name = vit->first;
            if (col_names.find(col_name) == col_names.end())
            {
                col_names[col_name] = true;
                g->add_col(col_name);
            }
        }
    }

    g->reserve_rows(dicts.size());

    for (std::vector<const Dict*>::const_iterator it = dicts.begin(), e = dicts.end(); it != e; ++it)
    {
        g->add_row(**it);
    }

    return g;
}

// Constructs grid from Dicts vector
Grid::auto_ptr_t Grid::make(const boost::ptr_vector<Dict>& dicts)
{
    auto_ptr_t g(new Grid());

    if (dicts.empty())
        return g;

    std::map<std::string, bool> col_names;

    // add cols
    for (boost::ptr_vector<Dict>::const_iterator dit = dicts.begin(), e = dicts.end(); dit != e; ++dit)
    {
        for (Dict::const_iterator vit = dit->begin(), e = dit->end(); vit != e; ++vit)
        {
            const std::string& col_name = vit->first;
            if (col_names.find(col_name) == col_names.end())
            {
                col_names[col_name] = true;
                g->add_col(col_name);
            }
        }
    }

    g->reserve_rows(dicts.size());

    for (boost::ptr_vector<Dict>::const_iterator it = dicts.begin(), e = dicts.end(); it != e; ++it)
    {
        g->add_row(*it);
    }

    return g;
}

const Grid& Grid::EMPTY = *new Grid;