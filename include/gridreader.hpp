#pragma once
#include "val.hpp"

//
// Copyright (c) 2014, Radu Racariu, Brian Frank
// History:
//   29 Aug 2014  Radu Racariu Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

/**
 GridReader is base class for reading grids from an input stream.

 @see <a href='http://project-haystack.org/doc/TagModel#tagKinds'>Project Haystack</a> 

*/
namespace haystack {

class Grid;
class GridReader
{
public:
    // Read a grid
    virtual const Grid& read_grid() = 0;
};
};