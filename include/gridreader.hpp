#pragma once
//
// Copyright (c) 2015, J2 Innovations
// Copyright (c) 2012 Brian Frank
// Licensed under the Academic Free License version 3.0
// History:
//   29 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

#include "headers.hpp"

namespace haystack {

    class Grid;
    /**
     GridReader is base class for reading grids from an input stream.

     @see <a href='http://project-haystack.org/doc/TagModel#tagKinds'>Project Haystack</a>

     */
    class GridReader
    {
    public:
        /**
        Read a grid
        */
        virtual std::auto_ptr<Grid> read_grid() = 0;
    };
};