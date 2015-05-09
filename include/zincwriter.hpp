#pragma once
//
// Copyright (c) 2015, J2 Innovations
// Copyright (c) 2012 Brian Frank
// Licensed under the Academic Free License version 3.0
// History:
//   01 Sep 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

#include "gridwriter.hpp"
#include <ostream>
#include <stdint.h>

namespace haystack {
    class Dict;
    class Col;
    class Row;
    /**
     ZincWriter is used to write grids in the Zinc format.

     @see <a href='http://project-haystack.org/doc/TagModel#tagKinds'>Project Haystack</a>

     */
    class ZincWriter : public GridWriter
    {
    public:

        ZincWriter(std::ostream& os);

        /**
        Write a grid
        */
        void write_grid(const Grid& grid);

        /**
        Write a grid to string
        */
        static const std::string grid_to_string(const Grid& grid);

    private:
        //////////////////////////////////////////////////////////////////////////
        // Implementation
        //////////////////////////////////////////////////////////////////////////
        void write_meta(const Dict& meta);
        void write_col(const Col& col);
        void write_row(const Grid& grid, const Row& row);

        //////////////////////////////////////////////////////////////////////////
        // Fields
        //////////////////////////////////////////////////////////////////////////

        std::ostream& m_os;
    };
};