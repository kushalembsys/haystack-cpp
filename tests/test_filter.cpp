//
// Copyright (c) 2014, Radu Racariu<radur@2inn.com>, Brian Frank
// History:
//   19 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//
#include "headers.hpp"
#include "filter.hpp"
#include "bool.hpp"
#include "date.hpp"
#include "num.hpp"
#include "time.hpp"
#include "ref.hpp"
#include "str.hpp"
#include "uri.hpp"


#include "ext/catch/catch.hpp"

using namespace haystack;

///////////////////////////////////////////////////////////
// Filter
///////////////////////////////////////////////////////////

void verifyParse(const std::string& s, const Filter& expected)
{
    Filter::auto_ptr_t actual = Filter::make(s);
    CHECK(*actual == expected);
}

Val::auto_ptr_t n(double v, std::string u = "")
{
    return Num(v, u).clone();
}

TEST_CASE("Filter testcase", "[Filter]")
{
    SECTION("Filter testIdentity")
    {
        CHECK(*Filter::has("a") == *Filter::has("a"));
        CHECK_FALSE(*Filter::has("a") == *Filter::has("b"));
    }

    SECTION("Filter testParse")
    {
        // basics
        verifyParse("x", *Filter::has("x"));
        verifyParse("foo", *Filter::has("foo"));
        verifyParse("fooBar", *Filter::has("fooBar"));
        verifyParse("foo7Bar", *Filter::has("foo7Bar"));
        verifyParse("foo_bar->a", *Filter::has("foo_bar->a"));
        verifyParse("a->b->c", *Filter::has("a->b->c"));
        verifyParse("not foo", *Filter::missing("foo"));

        // verify Zinc only literals do not work
        CHECK((Filter::make("x==T", false).get() == NULL));
        CHECK((Filter::make("x==F", false).get() == NULL));
        CHECK((Filter::make("x==F", false).get() == NULL));

        // bool literals
        verifyParse("x->y==true", *Filter::eq("x->y", Bool::TRUE_VAL.clone()));
        verifyParse("x->y!=false", *Filter::ne("x->y", Bool::FALSE_VAL.clone()));

        // str literals
        verifyParse("x==\"hi\"", *Filter::eq("x", Str("hi").clone()));
        verifyParse("x!=\"\\\"hi\\\"\"", *Filter::ne("x", Str("\"hi\"").clone()));

        // uri literals
        verifyParse("ref==`http://foo/?bar`", *Filter::eq("ref", Uri("http://foo/?bar").clone()));
        verifyParse("ref->x==`file name`", *Filter::eq("ref->x", Uri("file name").clone()));
        verifyParse("ref == `foo bar`", *Filter::eq("ref", Uri("foo bar").clone()));

        // int literals
        verifyParse("num < 4", *Filter::lt("num", n(4)));
        verifyParse("num <= -99", *Filter::le("num", n(-99)));

        // float literals
        verifyParse("num < 4.0", *Filter::lt("num", n(4.0f)));
        verifyParse("num <= -9.6", *Filter::le("num", n(-9.6f)));
        verifyParse("num > 400000", *Filter::gt("num", n(4e5f)));
        verifyParse("num >= 16000", *Filter::ge("num", n(1.6e+4f)));
        verifyParse("num >= 2.16", *Filter::ge("num", n(2.16)));

        // unit literals
        verifyParse("dur < 5ns", *Filter::lt("dur", n(5, "ns")));
        verifyParse("dur < 10kg", *Filter::lt("dur", n(10, "kg")));
        verifyParse("dur < -9sec", *Filter::lt("dur", n(-9, "sec")));
        verifyParse("dur < 2.5hr", *Filter::lt("dur", n(2.5, "hr")));

        // date, time, datetime
        verifyParse("foo < 2009-10-30", *Filter::lt("foo", Date(2009, 10, 30).clone()));
        verifyParse("foo < 08:30:00", *Filter::lt("foo", Time(8, 30, 00).clone()));
        verifyParse("foo < 13:00:00", *Filter::lt("foo", Time(13, 00, 00).clone()));

        // ref literals
        verifyParse("author == @xyz", *Filter::eq("author", Ref("xyz").clone()));
        verifyParse("author==@xyz:foo.bar", *Filter::eq("author", Ref("xyz:foo.bar").clone()));

        // and
        verifyParse("a and b", *Filter::andF(Filter::has("a"), Filter::has("b")));
        //verifyParse("a and b and c == 3", HFilter.has("a").and(HFilter.has("b").and(HFilter.eq("c", n(3)))));

    }


}