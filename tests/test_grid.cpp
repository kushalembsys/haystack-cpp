//
// Copyright (c) 2014, Radu Racariu, Brian Frank
// History:
//   28 Aug 2014  Radu Racariu Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//
#include "headers.hpp"
#include "grid.hpp"
#include "str.hpp"
#include "ref.hpp"
#include "num.hpp"
#include <iostream>
#include "ext/catch/catch.hpp"

using namespace haystack;

///////////////////////////////////////////////////////////
// Grid
///////////////////////////////////////////////////////////

const Col& verifyCol(const Grid& g, size_t i, const std::string n)
{
    const Col& col = g.col(i);
    CHECK(g.col(n) == &g.col(i));
    CHECK(col.name() == n);
    return col;
}

TEST_CASE("Grid testcase", "[Grid]")
{
    SECTION("Grid verifyCol")
    {
        Grid g;
        CHECK(g.meta() == Dict::EMPTY);
        CHECK(g.num_rows() == 0);
        CHECK(g.is_empty() == true);
        CHECK(!(g.col("foo", false) !=  NULL));
        CHECK_THROWS(g.col("foo"));
    }

    SECTION("Grid testNoRows")
    {
        Grid  g;
        g.meta().add("dis", "Title");
        g.addCol("a").add("dis", "Alpha");
        g.addCol("b");

        // meta
        CHECK(g.meta().size() == 1);
        CHECK(g.meta().get("dis") == Str("Title"));

        // cols
        CHECK(g.num_cols() == 2);
        
        const Col& c = verifyCol(g, 0, "a");
        CHECK(c.dis() == "Alpha");
        CHECK(c.meta().size() == 1);
        CHECK(c.meta().get("dis") == Str("Alpha"));

        // rows
        CHECK(g.num_rows() == 0);
        CHECK(g.is_empty() == true);

    }
    SECTION("Grid testSimple")
    {
        Grid g;
        g.addCol("id");
        g.addCol("dis");
        g.addCol("area");
        Val* r1[3] = { new Ref("a"), new Str("Alpha"), new Num(1200) };
        g.addRow(r1, sizeof(r1) / sizeof(Val*));
        
        Val* r2[3] = { new Ref("b"), new Str("Beta"), (Val*)&EmptyVal::DEF };
        g.addRow(r2, sizeof(r2) / sizeof(Val*));
        
        // meta
        CHECK(g.meta().size() == 0);

        // cols
        CHECK(g.num_cols() == 3);
        verifyCol(g, 0, "id");
        verifyCol(g, 1, "dis");
        verifyCol(g, 2, "area");

        // rows
        CHECK(g.num_rows() == 2);
        CHECK(!g.is_empty());
        const Row& gr1 = g.row(0);
        CHECK(gr1.get("id") == Ref("a"));
        CHECK(gr1.get("dis") == Str("Alpha"));
        CHECK(gr1.get("area") == Num(1200));

        const Row& gr2 = g.row(1);
        CHECK(gr2.get("id") == Ref("b"));
        CHECK(gr2.get("dis") == Str("Beta"));
        CHECK(gr2.get("area") == EmptyVal::DEF);
        
        // row it
        Row::const_iterator it = gr1.begin();
        
        CHECK(*it == Ref("a"));
        CHECK(*++it == Str("Alpha"));
        CHECK(*++it == Num(1200));

        int i = 0;
        for (Row::const_iterator it = gr1.begin(), end = gr1.end(); it != end; ++it, i++)
            CHECK(*it == *r1[i]);
        i = 0;
        for (Row::const_iterator it = gr2.begin(), end = gr2.end(); it != end; ++it, i++)
            CHECK(*it == *r2[i]);

    }
}