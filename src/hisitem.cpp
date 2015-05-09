//
// Copyright (c) 2015, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   19 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//
#include "hisitem.hpp"
#include "bool.hpp"
#include "datetime.hpp"

////////////////////////////////////////////////
// HisItem
////////////////////////////////////////////////
using namespace haystack;

HisItem::HisItem(const HisItem& other) : ts(other.ts), val(other.val) {}

HisItem::HisItem(const DateTime& t, const Val& v) : ts((DateTime*)new_clone(t)), val(new_clone(v)) {}
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

        boost::shared_ptr<const DateTime> ts((DateTime*)row.get(*tsCol).clone().release());
        boost::shared_ptr<const Val> val(row.get(*valCol).clone().release());
        
        items.push_back(HisItem(ts, val));
    }
    return items;
}

// Convenience to build grid from array of HHisItem
Grid::auto_ptr_t HisItem::his_items_to_grid(const Dict& meta, const std::vector<HisItem>& items)
{
    Grid::auto_ptr_t g(new Grid);
    g->meta().add(meta);
    g->add_col("ts");
    g->add_col("val");

    for (std::vector<HisItem>::const_iterator it = items.begin(), e = items.end(); it != e; ++it)
    {
        Val* v[2] = { (DateTime*)it->ts->clone().release(), (Val*)it->val->clone().release() };
        g->add_row(v, 2);
    }
    return g;
}