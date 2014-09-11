//
// Copyright (c) 2014, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   01 Sep 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//
#include "zincwriter.hpp"
#include "marker.hpp"
#include "grid.hpp"
#include <sstream>

////////////////////////////////////////////////
// ZincWriter
////////////////////////////////////////////////
using namespace haystack;

ZincWriter::ZincWriter(std::ostream& os) : m_os(os){}


// Write a grid
void ZincWriter::write_grid(const Grid& grid)
{
    // meta
    m_os << "ver:\"2.0\"";
    write_meta(grid.meta());
    m_os << '\n';

    // cols
    for (size_t i = 0; i < grid.num_cols(); ++i)
    {
        if (i > 0) m_os << ',';
        write_col(grid.col(i));
    }
    m_os << '\n';

    // rows
    for (size_t i = 0; i < grid.num_rows(); ++i)
    {
        write_row(grid, grid.row(i));
        m_os << '\n';
    }
}

// Write a grid to string
const std::string ZincWriter::grid_to_string(const Grid& grid)
{
    std::stringstream os;
    ZincWriter w(os);
    w.write_grid(grid);

    return os.str();
}

//////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////

void ZincWriter::write_meta(const Dict& meta)
{
    if (meta.is_empty()) return;

    for (Dict::const_iterator it = meta.begin(), e = meta.end(); it != e; ++it)
    {
        const std::string& name = it->first;
        const Val& val = *it->second;
        m_os << ' ';
        m_os << name;

        if (!(val == Marker::VAL))
        {
            m_os << ':';
            m_os << val.to_zinc();
        }
    }
}

void ZincWriter::write_col(const Col& col)
{
    m_os << col.name();
    write_meta(col.meta());
}

void ZincWriter::write_row(const Grid& grid, const Row& row)
{
    for (size_t i = 0; i < grid.num_cols(); ++i)
    {
        const Val& val = row.get(grid.col(i));
        if (i > 0)
            m_os << ',';

        if (val.is_empty())
        {
            if (i == 0) m_os << 'N';
        }
        else
        {
            m_os << val.to_zinc();
        }
    }
}