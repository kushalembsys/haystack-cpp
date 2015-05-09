//
// Copyright (c) 2015, J2 Innovations
// History:
//   19 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//
#include "headers.hpp"
#include "filter.hpp"
#include "bool.hpp"
#include "date.hpp"
#include "dict.hpp"
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


#define verifyParse(str, expected)  { Filter::shared_ptr_t actual = Filter::make(str); CHECK(*actual == expected); }

Val::auto_ptr_t n(double v, std::string u = "")
{
    return Num(v, u).clone();
}

class PathTest : public Pather
{
public:
    PathTest(const boost::ptr_map<std::string, Dict>& m) : m_map(m) {}
    const Dict& find(const std::string& ref) const
    {
        boost::ptr_map<std::string, Dict>::const_iterator it = m_map.find(ref);
        if (it != m_map.end())
            return *it->second;

        return Dict::EMPTY;
    }
private:
    const boost::ptr_map<std::string, Dict>& m_map;
};

void verifyInclude(const boost::ptr_map<std::string, Dict>& map, const std::string& query, const std::string& expected)
{
    PathTest db(map);

    Filter::shared_ptr_t q = Filter::make(query);

    std::stringstream actual;
    for (int c = 'a'; c <= 'c'; ++c)
    {
        std::stringstream id;
        id << (char)c;

        if (q->include(db.find(id.str()), db))
            actual << (actual.tellp() > 0 ? "," + id.str() : id.str());
    }
    CHECK(expected == actual.str());
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
        verifyParse("a and b", *Filter::has("a")->AND(Filter::has("b")));
        verifyParse("a and b and c == 3", *Filter::has("a")->AND(Filter::has("b")->AND(Filter::eq("c", n(3)))));

        // or
        verifyParse("a or b", *Filter::has("a")->OR(Filter::has("b")));
        verifyParse("a or b or c == 3", *Filter::has("a")->OR(Filter::has("b")->OR(Filter::eq("c", n(3)))));

        // parens
        verifyParse("(a)", *Filter::has("a"));
        verifyParse("(a) and (b)", *Filter::has("a")->AND(Filter::has("b")));
        verifyParse("( a )  and  ( b ) ", *Filter::has("a")->AND(Filter::has("b")));
        verifyParse("(a or b) or (c == 3)", *Filter::has("a")->OR(Filter::has("b"))->OR(Filter::eq("c", n(3))));

        // combo
        Filter::shared_ptr_t isA = Filter::has("a");
        Filter::shared_ptr_t isB = Filter::has("b");
        Filter::shared_ptr_t isC = Filter::has("c");
        Filter::shared_ptr_t isD = Filter::has("d");

        verifyParse("a and b or c", *isA->AND(isB)->OR(isC));
        verifyParse("a or b and c", *isA->OR(isB->AND(isC)));
        verifyParse("a and b or c and d", *(isA->AND(isB))->OR(isC->AND(isD)));
        verifyParse("(a and (b or c)) and d", *isA->AND(isB->OR(isC))->AND(isD));
        verifyParse("(a or (b and c)) or d", *isA->OR(isB->AND(isC))->OR(isD));

    }

    SECTION("Filter testInclude")
    {

        Dict*  a = new Dict();
        a->add("dis", "a")
            .add("num", 100)
            .add("foo", 99)
            .add("date", new Date(2011, 10, 5));

        Dict*  b = new Dict();
        b->add("dis", "b")
            .add("num", 200)
            .add("foo", 88)
            .add("date", new Date(2011, 10, 20))
            .add("bar")
            .add("ref", new Ref("a"));

        Dict*  c = new Dict();
        c->add("dis", "c")
            .add("num", 300)
            .add("ref", new Ref("b"))
            .add("bar");

        boost::ptr_map<std::string, Dict> db;
        std::string ak("a");
        db.insert(ak, a);
        std::string bk("b");
        db.insert(bk, b);
        std::string ck("c");
        db.insert(ck, c);

        verifyInclude(db, "dis", "a,b,c");
        verifyInclude(db, "dis == \"b\"", "b");
        verifyInclude(db, "dis != \"b\"", "a,c");
        verifyInclude(db, "dis >  \"b\"", "c");
        verifyInclude(db, "dis <= \"b\"", "a,b");
        verifyInclude(db, "num < 200", "a");
        verifyInclude(db, "num <= 200", "a,b");
        verifyInclude(db, "num > 200", "c");
        verifyInclude(db, "num >= 200", "b,c");
        verifyInclude(db, "date", "a,b");
        verifyInclude(db, "date == 2011-10-20", "b");
        verifyInclude(db, "date < 2011-10-10", "a");
        verifyInclude(db, "foo", "a,b");
        verifyInclude(db, "not foo", "c");
        verifyInclude(db, "foo == 88", "b");
        verifyInclude(db, "foo != 88", "a");
        verifyInclude(db, "foo == \"x\"", "");
        verifyInclude(db, "ref", "b,c");
        verifyInclude(db, "ref->dis", "b,c");
        verifyInclude(db, "ref->dis == \"a\"", "b");
        verifyInclude(db, "ref->bar", "c");
        verifyInclude(db, "not ref->bar", "a,b");
        verifyInclude(db, "foo and bar", "b");
        verifyInclude(db, "foo or bar", "a,b,c");
        verifyInclude(db, "(foo and bar) or num==300", "b,c");
        verifyInclude(db, "foo and bar and num==300", "");

    }


}