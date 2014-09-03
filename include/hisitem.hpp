#pragma once
#include "headers.hpp"
#include <boost/shared_ptr.hpp>
#include <vector>
//
// Copyright (c) 2014, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   28 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

namespace haystack {
    class DateTime;
    class Val;
    class Grid;
    /*
     HisItem is used to model a timestamp/value pair.

     @see <a href='http://project-haystack.org/doc/TagModel#tagKinds'>Project Haystack</a>
     */
    class HisItem
    {
    public:
        boost::shared_ptr<const DateTime> ts;
        boost::shared_ptr<const Val> val;

        HisItem(const HisItem&);
        HisItem(boost::shared_ptr<const DateTime> ts, boost::shared_ptr<const Val> val);

        // Map Grid to HisItems.  Grid must have ts and val columns.
        static const std::vector<HisItem> grid_to_items(const Grid& grid);
    };

};