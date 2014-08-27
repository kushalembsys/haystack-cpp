//
// Copyright (c) 2014, Radu Racariu, Brian Frank
// History:
//   19 Aug 2014  Radu Racariu Ported to C++
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
#include <iostream>
#include "ext/catch/catch.hpp"

using namespace haystack;

///////////////////////////////////////////////////////////
// Dict
///////////////////////////////////////////////////////////
TEST_CASE("Dict testcase", "[Dict]")
{
    const Dict& d = Dict::EMPTY;
    const Dict& tags = DictBuilder().to_dict();
    CHECK(tags == Dict::EMPTY);
    CHECK(tags == Dict::EMPTY);

    CHECK(d.has("ffo") == false);

    DictBuilder db;
    db.add("foo", Marker::VAL)
        .add("bar", Num(45.6));

    const Dict& a = db.to_dict();
    CHECK(a.has("foo"));
    //CHECK(a.get("foo") == Marker::VAL);

}
