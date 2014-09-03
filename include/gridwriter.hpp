#pragma once
#include "val.hpp"

//
// Copyright (c) 2014, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   01 Sep 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

namespace haystack {

    class Grid;
    /*
     GridWriter is base class for writing grids to an output stream.

     @see <a href='http://project-haystack.org/doc/TagModel#tagKinds'>Project Haystack</a>

     */
    class GridWriter
    {
    public:
        // Write a grid
        virtual void write_grid(const Grid& grid) = 0;
    };
};