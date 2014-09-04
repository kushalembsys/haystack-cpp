//
// Copyright (c) 2014, Radu Racariu<radur@2inn.com>, Brian Frank
// History:
//   19 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//
#include "headers.hpp"
#include "dict.hpp"
#include "bin.hpp"
#include "bool.hpp"
#include "date.hpp"
#include "datetime.hpp"
#include "marker.hpp"
#include "num.hpp"
#include "ref.hpp"
#include "str.hpp"
#include "time.hpp"
#include "uri.hpp"
#include "zincreader.hpp"
#include <iostream>


#include "ext/catch/catch.hpp"

using namespace haystack;

///////////////////////////////////////////////////////////
// Dict
///////////////////////////////////////////////////////////

#define verifyZinc(str, tags) { std::istringstream iss(str); std::auto_ptr<Dict> x = ZincReader(iss).read_dict(); if (tags.size() <= 1){ CHECK(tags.to_zinc() == str);} CHECK(*x == tags); }

TEST_CASE("Dict testcase", "[Dict]")
{
    SECTION("Dict testEmpty")
    {
        const Dict& d = Dict::EMPTY;
        const Dict& tags = Dict();
        CHECK(tags == Dict::EMPTY);
        CHECK(tags == Dict::EMPTY);

        // size
        CHECK(tags.size() == 0);
        CHECK(tags.is_empty());

        // missing tag
        CHECK(!tags.has("foo"));
        CHECK(tags.missing("foo"));
        CHECK(tags.get("foo").is_empty());
    }

    SECTION("Dict is_tag_name")
    {
        CHECK(Dict::is_tag_name("") == false);
        CHECK(Dict::is_tag_name("A") == false);
        CHECK(Dict::is_tag_name(" ") == false);
        CHECK(Dict::is_tag_name("a") == true);
        CHECK(Dict::is_tag_name("a_B_19") == true);
        CHECK(Dict::is_tag_name("a b") == false);
        CHECK(Dict::is_tag_name("a\x7f") == false);
        CHECK(Dict::is_tag_name("a\x81") == false);
        CHECK(Dict::is_tag_name("a\xab\xcd") == false);
    }

    SECTION("Dict testBasics")
    {
        Dict tags;
        tags.add("id", new Ref("aaaa-bbbb")).add("site")
            .add("geoAddr", "Richmond, Va")
            .add("area", 1200, "ft")
            .add("date", new Date(2000, 12, 3));

        // size
        CHECK(tags.size() == 5);
        CHECK(!tags.is_empty());

        // configured tags
        CHECK(tags.get("id") == Ref("aaaa-bbbb"));
        CHECK(tags.get("site") == Marker::VAL);
        CHECK(tags.get("geoAddr") == Str("Richmond, Va"));
        CHECK(tags.get("area") == Num(1200, "ft"));
        CHECK(tags.get("date") == Date(2000, 12, 3));

        // missing tag
        CHECK(!tags.has("foo"));
        CHECK(tags.missing("foo"));
        CHECK(tags.get("foo") == EmptyVal::DEF);
    }

    SECTION("Dict testEquality")
    {
        Dict a;
        a.add("x");
        CHECK(a == Dict().add("x"));
        CHECK(a != Dict().add("x", 3));
        CHECK(a != Dict().add("y"));
        CHECK(a != Dict().add("x").add("y"));

        Dict b;
        b.add("x").add("y", "str");
        CHECK(b == Dict().add("x").add("y", "str"));
        CHECK(b == Dict().add("y", "str").add("x"));

        CHECK(b != Dict().add("x", "str").add("y", "str"));
        CHECK(b != Dict().add("x").add("y", "strx"));
        CHECK(b != Dict().add("y", "str"));
        CHECK(b != Dict().add("x"));
        CHECK(b != Dict().add("x").add("yy", "str"));
    }

    SECTION("Dict to_zinc")
    {
        CHECK(Dict().to_zinc() == "");

        CHECK(Dict().add("foo_12").to_zinc() == "foo_12");
        CHECK(Dict().add("fooBar", 123, "ft").to_zinc() == "fooBar:123ft");

        verifyZinc("dis:\"Bob\" bday:1970-06-03 marker",
            Dict().add("bday", new Date(1970, 6, 3)).add("dis", "Bob").add("marker"));
        verifyZinc("dis  :  \"Bob\"  bday : 1970-06-03  marker",
            Dict().add("dis", "Bob").add("bday", new Date(1970, 6, 3)).add("marker"));
    }

    SECTION("Dict testDis")
    {
        CHECK(Dict().add("id", new Ref("a")).dis() == "a");
        CHECK(Dict().add("id", new Ref("a", "b")).dis() == "b");
        CHECK(Dict().add("id", new Ref("a")).add("dis", "d").dis() == "d");
    }

}