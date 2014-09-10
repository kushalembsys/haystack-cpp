//
// Copyright (c) 2014, Radu Racariu<radur@2inn.com>, Brian Frank
// History:
//   28 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//
#include "headers.hpp"
#include "zincreader.hpp"
#include "zincwriter.hpp"
#include "bin.hpp"
#include "bool.hpp"
#include "coord.hpp"
#include "datetime.hpp"
#include "marker.hpp"
#include "num.hpp"
#include "ref.hpp"
#include "str.hpp"
#include "uri.hpp"
#include "grid.hpp"
#include <sstream>
#include <iostream>
#include <stdio.h>
#include "ext/catch/catch.hpp"

using namespace haystack;

///////////////////////////////////////////////////////////
// ZincReader
///////////////////////////////////////////////////////////

void verifyGridEq(const Grid& grid, const Grid& expected)
{
    // meta
    CHECK(grid.meta() == expected.meta());

    // cols
    CHECK(grid.num_cols() == expected.num_cols());

    for (size_t i = 0; i < grid.num_cols(); ++i)
    {
        CHECK(grid.col(i).name() == expected.col(i).name());
        CHECK(grid.col(i).meta() == expected.col(i).meta());
    }

    // rows
    CHECK(grid.num_rows() == expected.num_rows());

    size_t i = 0;
    for (Grid::const_iterator it = expected.begin(), end = expected.end(); it != end; ++it, i++)
    {
        CHECK(grid.row(i) == *it);
    }
}

void verifyGrid(const std::string& s, const Grid& e)
{
    // docode and compare
    std::istringstream iss(s);
    ZincReader r(iss);
    Grid::auto_ptr_t g = r.read_grid();
    verifyGridEq(*g, e);

    // encode to string then decode and compare again
    std::string out = ZincWriter::grid_to_string(*g);
    std::istringstream iss2(out);
    ZincReader r2(iss2);
    Grid::auto_ptr_t g1 = r2.read_grid();
    verifyGridEq(*g1, *g);
}

TEST_CASE("ZincReader", "[ZincReader]")
{
    SECTION("Grid verifyCol")
    {
        Grid e;
        e.add_col("fooBar33");

        verifyGrid("ver:\"2.0\"\nfooBar33\n\n", e);

    }

    SECTION("Grid verifyGrid1")
    {

        Grid e;
        e.meta().add("tag").add("foo", "bar");
        e.add_col("xyz");
        Val* r[1] = { new Str("val") };
        e.add_row(r, sizeof(r) / sizeof(Val*));

        verifyGrid("ver:\"2.0\" tag foo:\"bar\"\nxyz\n\"val\"\n\n", e);

    }

    SECTION("Grid verifyGrid2")
    {
        Grid e;
        e.add_col("val");
        Val* r[1] = { NULL };
        e.add_row(r, 1);

        verifyGrid("ver:\"2.0\"\nval\nN\n\n", e);
    }

    SECTION("Grid verifyGrid3")
    {
        Grid e;
        e.add_col("a");
        e.add_col("b");
        Val* r1[2] = { new Num(1.0), new Num(2.0) };
        Val* r2[2] = { new Num(3.0), new Num(4.0) };
        e.add_row(r1, sizeof(r1) / sizeof(Val*));
        e.add_row(r2, sizeof(r2) / sizeof(Val*));

        verifyGrid("ver:\"2.0\"\na,b\n1,2\n3,4\n\n", e);
    }

    SECTION("Grid verifyGrid4")
    {
        Grid e;

        e.meta().add("bg", new Bin("image/jpeg")).add("mark");

        e.add_col("file1").add("icon", new Bin("image/gif")).add("dis", "F1");
        e.add_col("file2").add("icon", new Bin("image/jpg"));
        Val* r1[2] = { new Bin("text/plain"), NULL };
        Val* r2[2] = { new Num(4.0), new Bin("image/png") };
        Val* r3[2] = { new Bin("text/html; a=foo; bar=\"sep\""), new Bin("text/html; charset=utf8") };
        e.add_row(r1, 2);
        e.add_row(r2, 2);
        e.add_row(r3, 2);

        verifyGrid("ver:\"2.0\" bg: Bin(image/jpeg) mark\n"
            "file1 dis:\"F1\" icon: Bin(image/gif),file2 icon: Bin(image/jpg)\n"
            "Bin(text/plain),N\n"
            "4,Bin(image/png)\n"
            "Bin(text/html; a=foo; bar=\"sep\"),Bin(text/html; charset=utf8)\n", e);
    }

    SECTION("Grid verifyGrid5")
    {
        Grid e;

        e.add_col("a");
        e.add_col("b");
        e.add_col("c");
        e.add_col("d");

        Val* r1[4] = { new Bool(true), new Bool(false), NULL, new Num(-99.0) };
        Val* r2[4] = { new Num(2.3), new Num(-5.0E-10), new Num(2.4E20), new Num(1.23E-8) };
        Val* r3[4] = { new Str(""), new Str("a"), new Str("\" \\ \t \n \r"), new Str("\x0a") };
        Val* r4[4] = { new Uri("path"), new Ref("12cbb082-0c02ae73"), new Num(4.0, "s"), new Num(-2.5, "min") };
        Val* r5[4] = { (Val*)new Marker(), new Str("_remove_"), new Bin("image/png"), new Bin("image/png") };
        Val* r6[4] = { new Date(2009, 12, 31), new Time(23, 59, 1, 0), new Time(1, 2, 3, 123), new DateTime(Date(2009, 2, 3), Time(4, 5, 6, 0), TimeZone("UTC")) };
        Val* r7[4] = { new Num(Num::POS_INF.value), new Num(Num::NEG_INF.value), new Str(""), new Num(Num::NaN.value) };
        Val* r8[4] = { new Coord(12.0, -34.0), new Coord(0.123, -0.789), new Coord(84.5, -77.45), new Coord(-90.0, 180.0) };

        e.add_row(r1, 4);
        e.add_row(r2, 4);
        e.add_row(r3, 4);
        e.add_row(r4, 4);
        e.add_row(r5, 4);
        e.add_row(r6, 4);
        e.add_row(r7, 4);
        e.add_row(r8, 4);

        verifyGrid("ver:\"2.0\"\n"
            "a,    b,      c,      d\n"
            "T,    F,      N,   -99\n"
            "2.3,  -5e-10, 2.4e20, 123e-10\n"
            "\"\",   \"a\",   \"\\\" \\\\ \\t \\n \\r\", \"\\uabcd\"\n"
            "`path`, @12cbb082-0c02ae73, 4s, -2.5min\n"
            "M,R,Bin(image/png),Bin(image/png)\n"
            "2009-12-31, 23:59:01, 01:02:03.123, 2009-02-03T04:05:06Z\n"
            "INF, -INF, \"\", NaN\n"
            "C(12,-34),C(0.123,-.789),C(84.5,-77.45),C(-90,180)\n"
            "\n", e);
    }

    SECTION("Grid verifyGrid6")
    {
        Grid e;
        e.add_col("foo");

        Val* r1[1] = { new Uri("foo$20bar") };
        Val* r2[1] = { new Uri("foo`bar") };
        Val* r3[1] = { new Uri("file \\#2") };
        Val* r4[1] = { new Str("$15") };

        e.add_row(r1, 1);
        e.add_row(r2, 1);
        e.add_row(r3, 1);
        e.add_row(r4, 1);

        verifyGrid("ver:\"2.0\"\n"
            "foo\n"
            "`foo$20bar`\n"
            "`foo\\`bar`\n"
            "`file \\#2`\n"
            "\"$15\"\n", e);
    }

    SECTION("Grid verifyGrid7")
    {
        Grid e;
        e.add_col("a");
        e.add_col("b");

        Val* r1[2] = { new Num(-3.1, "kg"), new Num(4.0, "kg") };
        Val* r2[2] = { new Num(5.0, "%"), new Num(3.2, "%") };
        Val* r3[2] = { new Num(5.0, "kWh/ft\xb2"), new Num(-15.0, "kWh/m\xb2") };
        Val* r4[2] = { new Num(1.23E14, "kJ/kg_dry"), new Num(74.0, "\xb2\xb2") };

        e.add_row(r1, 2);
        e.add_row(r2, 2);
        e.add_row(r3, 2);
        e.add_row(r4, 2);

        verifyGrid("ver:\"2.0\"\n"
            "a, b\n"
            "-3.1kg,4kg\n"
            "5%,3.2%\n"
            "5kWh/ft\xb2,-15kWh/m\xb2\n"
            "123e+12kJ/kg_dry,74\xb2\xb2\n", e);
    }

    SECTION("Grid verifyGrid8")
    {
        Grid e;
        e.add_col("a");
        e.add_col("b");
        e.add_col("c");

        Val* r1[3] = { NULL, new Num(1.0), new Num(2.0) };
        Val* r2[3] = { new Num(3.0), NULL, new Num(5.0) };
        Val* r3[3] = { new Num(6.0), new Num(7000.0), NULL };
        Val* r4[3] = { NULL, NULL, new Num(10.0) };
        Val* r5[3] = { NULL, NULL, NULL };
        Val* r6[3] = { new Num(14.0), NULL, NULL };

        e.add_row(r1, 3);
        e.add_row(r2, 3);
        e.add_row(r3, 3);
        e.add_row(r4, 3);
        e.add_row(r5, 3);
        e.add_row(r6, 3);

        verifyGrid("ver:\"2.0\"\n"
            "a, b, c\n"
            ", 1, 2\n"
            "3, , 5\n"
            "6, 7_000,\n"
            ",,10\n"
            ",,\n"
            "14,,\n"
            "\n", e);
    }

    SECTION("Grid verifyGrid9")
    {
        Grid e;
        e.add_col("a");
        e.add_col("b");

        Val* r1[2] = { new DateTime(Date(2010, 3, 1), Time(23, 55, 0, 13), TimeZone("GMT+5")), new DateTime(Date(2010, 3, 1), Time(23, 55, 0, 13), TimeZone("GMT-10")) };

        e.add_row(r1, 2);

        verifyGrid("ver:\"2.0\"\n"
            "a,b\n"
            "2010-03-01T23:55:00.013-05:00 GMT+5,2010-03-01T23:55:00.013+10:00 GMT-10\n", e);
    }

    SECTION("Grid verifyGrid10")
    {
        Grid e;
        e.meta().add("b", new DateTime(Date(2010, 2, 3), Time(4, 5, 6, 0), TimeZone("UTC")))
            .add("baz", new Marker())
            .add("c", new DateTime(Date(2009, 12, 3), Time(4, 5, 6, 0), TimeZone("London")))
            .add("a", new DateTime(Date(2009, 2, 3), Time(4, 5, 6, 0), TimeZone("UTC")))
            .add("foo", new Marker())
            .add("bar", new Marker());

        e.add_col("a");

        Val* r1[1] = { new Num(3.814697265625E-6) };
        Val* r2[1] = { new DateTime(Date(2010, 12, 18), Time(14, 11, 30, 924), TimeZone("UTC")) };
        Val* r3[1] = { new DateTime(Date(2010, 12, 18), Time(14, 11, 30, 925), TimeZone("UTC")) };
        Val* r4[1] = { new DateTime(Date(2010, 12, 18), Time(14, 11, 30, 925), TimeZone("London")) };
        Val* r5[1] = { new Num(45.0, "$") };
        Val* r6[1] = { new Num(33.0, "\xa3") };
        Val* r7[1] = { new Ref("12cbb08e-0c02ae73") };
        Val* r8[1] = { new Num(7.15625E-4, "kWh/ft\xb2") };

        e.add_row(r1, 1);
        e.add_row(r2, 1);
        e.add_row(r3, 1);
        e.add_row(r4, 1);
        e.add_row(r5, 1);
        e.add_row(r6, 1);
        e.add_row(r7, 1);
        e.add_row(r8, 1);

        verifyGrid("ver:\"2.0\" a: 2009-02-03T04:05:06Z foo b: 2010-02-03T04:05:06Z UTC bar c: 2009-12-03T04:05:06Z London baz\n"
            "a\n"
            "3.814697265625E-6\n"
            "2010-12-18T14:11:30.924Z\n"
            "2010-12-18T14:11:30.925Z UTC\n"
            "2010-12-18T14:11:30.925Z London\n"
            "45$\n"
            "33\xa3\n"
            "@12cbb08e-0c02ae73\n"
            "7.15625E-4kWh/ft\xb2\n", e);
    }

}