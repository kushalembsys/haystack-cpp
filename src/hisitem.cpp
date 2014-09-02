//
// Copyright (c) 2014, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   19 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//
#include "hisitem.hpp"
#include "grid.hpp"
#include "bool.hpp"
#include "datetime.hpp"

////////////////////////////////////////////////
// HisItem
////////////////////////////////////////////////
using namespace haystack;

HisItem::HisItem(const HisItem& other) : ts(other.ts), val(other.val) {}
HisItem::HisItem(boost::shared_ptr<const DateTime> ts, boost::shared_ptr<const Val> val) : ts(ts), val(val) {}


// Map Grid to HisItems.  Grid must have ts and val columns.
const std::vector<HisItem> HisItem::grid_to_items(const Grid& grid)
{
    const Col* tsCol = grid.col("ts");
    const Col* valCol = grid.col("val");

    std::vector<HisItem> items;

    if (tsCol == NULL || valCol == NULL)
        return items;
    
    for (size_t i = 0; i < grid.num_rows(); ++i)
    {
        const Row& row = grid.row(i);

        DateTime::auto_ptr_t t =  row.get(*tsCol).clone();
        boost::shared_ptr<const DateTime> ts((DateTime*)t.get());
        t.release();
        Val::auto_ptr_t v = row.get(*valCol).clone();
        boost::shared_ptr<const Val> val(v.get());
        v.release();

        items.push_back(HisItem(ts, val));
    }
    return items;
}