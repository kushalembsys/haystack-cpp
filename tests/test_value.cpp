//
// Copyright (c) 2014, Radu Racariu<radur@2inn.com>, Brian Frank
// History:
//   19 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//


#include "headers.hpp"
#include "bin.hpp"
#include "bool.hpp"
#include "coord.hpp"
#include "date.hpp"
#include "datetime.hpp"
#include "marker.hpp"
#include "num.hpp"
#include "ref.hpp"
#include "str.hpp"
#include "time.hpp"
#include "uri.hpp"
#include "zincreader.hpp"
#include <assert.h>
#include <iostream>

#define CATCH_CONFIG_MAIN
#include "ext/catch/catch.hpp"


///////////////////////////////////////////////////////////
// Bin
///////////////////////////////////////////////////////////
using namespace haystack;

#define READ(str) ZincReader::make(str)->read_scalar() 

#define VERIFY_ZINC(val, str) {  CHECK(val.to_zinc() == str); CHECK( *READ(str) == val); }

TEST_CASE("Bin testcase", "[Bin]")
{
    Bin b("text/plain");
    CHECK(b.to_string() == "text/plain");
    CHECK(b.to_zinc() == "Bin(text/plain)");
    // equality
    CHECK(Bin("text/plain") == Bin("text/plain"));
    CHECK(Bin("text/plain") != Bin("text/xml"));

    // encoding
    VERIFY_ZINC(Bin("text/plain; charset=utf-8"), "Bin(text/plain; charset=utf-8)");
    VERIFY_ZINC(Bin("text/plain"), "Bin(text/plain)");

    // verify bad bins are caught on encoding
    CHECK_THROWS(Bin("text/plain; f()").to_zinc());
    CHECK_THROWS(READ("Bin()"));
    CHECK_THROWS(READ("Bin(text)"));
}

///////////////////////////////////////////////////////////
// Bool
///////////////////////////////////////////////////////////
TEST_CASE("Boolean testcase", "[Bool]")
{
    Bool  t(true);
    CHECK(t == true);
    CHECK(t == Bool(true));
    CHECK(t == Bool::TRUE_VAL);
    CHECK_FALSE(t == Bool::FALSE_VAL);
    CHECK(t.to_string() == "true");
    CHECK(t.to_zinc() == "T");

    const Bool&  f = Bool(false);
    CHECK(f == false);
    CHECK(f == haystack::Bool::FALSE_VAL);
    CHECK_FALSE(f == haystack::Bool::TRUE_VAL);
    CHECK(f.to_string() == "false");
    CHECK(f.to_zinc() == "F");

    // equality
    CHECK(Bool::TRUE_VAL == Bool::TRUE_VAL);
    CHECK(Bool::TRUE_VAL != Bool::FALSE_VAL);
    CHECK(Bool(true) == Bool::TRUE_VAL);
    CHECK(Bool(false) == Bool::FALSE_VAL);

    // compare
    CHECK(Bool::FALSE_VAL < Bool::TRUE_VAL);
    CHECK(Bool::TRUE_VAL == Bool::TRUE_VAL);

    // toString
    CHECK(Bool::TRUE_VAL.to_string() == "true");
    CHECK(Bool::FALSE_VAL.to_string() == "false");

    // zinc
    VERIFY_ZINC(Bool::TRUE_VAL, "T");
    VERIFY_ZINC(Bool::FALSE_VAL, "F");
}

void verifyCoord(double lat, double lng, std::string s)
{
    Coord c(lat, lng);
    CHECK(c.lat() == lat);
    CHECK(c.lng() == lng);
    CHECK(c.to_string() == s);
    CHECK(Coord::make(s) == c);
}

///////////////////////////////////////////////////////////
// Coord
///////////////////////////////////////////////////////////
TEST_CASE("Coord testcase", "[Coord]")
{
    verifyCoord(12, 34, "C(12.0,34.0)");

    // lat boundaries
    verifyCoord(90, 123, "C(90.0,123.0)");
    verifyCoord(-90, 123, "C(-90.0,123.0)");
    verifyCoord(89.888999, 123, "C(89.888999,123.0)");
    verifyCoord(-89.888999, 123, "C(-89.888999,123.0)");

    // lon boundaries
    verifyCoord(45, 180, "C(45.0,180.0)");
    verifyCoord(45, -180, "C(45.0,-180.0)");
    verifyCoord(45, 179.999129, "C(45.0,179.999129)");
    verifyCoord(45, -179.999129, "C(45.0,-179.999129)");

    // decimal places
    verifyCoord(9.1, -8.1, "C(9.1,-8.1)");
    verifyCoord(9.12, -8.13, "C(9.12,-8.13)");
    verifyCoord(9.123, -8.134, "C(9.123,-8.134)");
    verifyCoord(9.1234, -8.1346, "C(9.1234,-8.1346)");
    verifyCoord(9.12345, -8.13456, "C(9.12345,-8.13456)");
    verifyCoord(9.123452, -8.134567, "C(9.123452,-8.134567)");

    // zero boundaries
    verifyCoord(0, 0, "C(0.0,0.0)");
    verifyCoord(0.3, -0.3, "C(0.3,-0.3)");
    verifyCoord(0.03, -0.03, "C(0.03,-0.03)");
    verifyCoord(0.003, -0.003, "C(0.003,-0.003)");
    verifyCoord(0.0003, -0.0003, "C(0.0003,-0.0003)");
    verifyCoord(0.02003, -0.02003, "C(0.02003,-0.02003)");
    verifyCoord(0.020003, -0.020003, "C(0.020003,-0.020003)");
    verifyCoord(0.000123, -0.000123, "C(0.000123,-0.000123)");
    verifyCoord(7.000123, -7.000123, "C(7.000123,-7.000123)");

    // arg errors
    CHECK(Coord::is_lat(-91.0) == false);
    CHECK(Coord::is_lat(-90.0) == true);
    CHECK(Coord::is_lat(-89.0) == true);
    CHECK(Coord::is_lat(90.0) == true);
    CHECK(Coord::is_lat(91.0) == false);
    CHECK(Coord::is_lng(-181.0) == false);
    CHECK(Coord::is_lng(-179.99) == true);
    CHECK(Coord::is_lng(180.0) == true);
    CHECK(Coord::is_lng(181.0) == false);

    CHECK_THROWS(Coord(91.0, 12.0));
    CHECK_THROWS(Coord(-90.2, 12.0));
    CHECK_THROWS(Coord(13.0, 180.009));
    CHECK_THROWS(Coord(13.0, -181.0));

    // parse errs
    CHECK_THROWS(Coord::make("1.0,2.0"));
    CHECK_THROWS(Coord::make("(1.0,2.0)"));
    CHECK_THROWS(Coord::make("C(1.0,2.0"));
    CHECK_THROWS(Coord::make("C(x,9)"));

}

///////////////////////////////////////////////////////////
// Date
///////////////////////////////////////////////////////////
TEST_CASE("Date testcase", "[Date]")
{
    // equality
    CHECK(Date(2011, 6, 7) == Date(2011, 6, 7));
    CHECK(Date(2011, 6, 7) != Date(2011, 6, 8));
    CHECK(Date(2011, 6, 7) != Date(2011, 2, 7));
    CHECK(Date(2011, 6, 7) != Date(2009, 6, 7));

    // compare
    CHECK(Date(2011, 6, 9) < Date(2011, 6, 21));
    CHECK(Date(2011, 10, 9) > Date(2011, 3, 21));
    CHECK(Date(2010, 6, 9) > Date(2000, 9, 30));
    CHECK(Date(2010, 6, 9) == Date(2010, 6, 9));

    // plus/minus
    CHECK(Date(2011, 12, 1).dec_days(0) == Date(2011, 12, 1));
    CHECK(Date(2011, 12, 1).dec_days(1) == Date(2011, 11, 30));
    CHECK(Date(2011, 12, 1).dec_days(-2) == Date(2011, 12, 3));
    CHECK(Date(2011, 12, 1).inc_days(2) == Date(2011, 12, 3));
    CHECK(Date(2011, 12, 1).inc_days(31) == Date(2012, 1, 1));
    CHECK(Date(2008, 3, 3).dec_days(3) == Date(2008, 2, 29));
    CHECK(Date(2008, 3, 3).dec_days(4) == Date(2008, 2, 28));

    CHECK(Date(2014, 8, 19).weekday() == 3);
    CHECK(Date(2014, 8, 22).weekday() == 6);

    // encoding
    VERIFY_ZINC(Date(2011, 6, 7), "2011-06-07");
    VERIFY_ZINC(Date(2011, 10, 10), "2011-10-10");
    VERIFY_ZINC(Date(2011, 12, 31), "2011-12-31");

    CHECK_THROWS(READ("2003-xx-02"));
    CHECK_THROWS(READ("2003 - 02"));
    CHECK_THROWS(READ("2003-02-xx"));


    // leap year
    for (int y = 1900; y <= 2100; y++)
    {
        if (((y % 4) == 0) && (y != 1900) && (y != 2100))
            CHECK(Date::is_leap_year(y));
        else
            CHECK(!Date::is_leap_year(y));
    }
}

///////////////////////////////////////////////////////////
// DateTime
///////////////////////////////////////////////////////////
TEST_CASE("DateTime testcase", "[DateTime]")
{
    const DateTime& ts = DateTime::make(1307377618069LL, TimeZone("New_York", -4));
    CHECK(ts.date.to_string() == "2011-06-06");
    CHECK(ts.time.to_string() == "12:26:58.069");

    // equality
    const TimeZone& utc = TimeZone::UTC;
    const TimeZone& london = TimeZone("London", 0);

    CHECK(DateTime(2011, 1, 2, 3, 4, 5, utc, 0) == DateTime(2011, 1, 2, 3, 4, 5, utc, 0));
    CHECK(DateTime(2011, 1, 2, 3, 4, 5, utc, 0) != DateTime(2011, 1, 2, 3, 9, 5, utc, 0));
    CHECK(DateTime(2011, 1, 2, 3, 4, 5, utc, 0) != DateTime(2011, 1, 2, 3, 4, 9, utc, 0));
    CHECK(DateTime(2011, 1, 2, 3, 4, 5, utc, 0) != DateTime(2011, 1, 2, 3, 4, 5, london, 0));
    CHECK(DateTime(2011, 1, 2, 3, 4, 5, utc, 0) != DateTime(2011, 1, 2, 3, 4, 5, london, 3600));

    // compare
    CHECK(DateTime(2011, 1, 2, 3, 4, 5, utc, 0) == DateTime(2011, 1, 2, 3, 4, 5, utc, 0));
    CHECK(DateTime(2011, 1, 2, 3, 4, 5, utc, 0) < DateTime(2011, 1, 2, 3, 4, 6, utc, 0));
    CHECK(DateTime(2011, 1, 2, 3, 4, 5, utc, 0) < DateTime(2011, 1, 2, 3, 5, 5, utc, 0));
    CHECK(DateTime(2011, 1, 2, 3, 4, 5, utc, 0) < DateTime(2011, 1, 2, 4, 4, 5, utc, 0));
    CHECK(DateTime(2011, 1, 2, 3, 4, 5, utc, 0) < DateTime(2011, 1, 3, 3, 4, 5, utc, 0));
    CHECK(DateTime(2011, 1, 2, 3, 4, 5, utc, 0) < DateTime(2011, 2, 2, 3, 4, 5, utc, 0));
    CHECK(DateTime(2011, 1, 2, 3, 4, 5, utc, 0) < DateTime(2012, 1, 2, 3, 4, 5, utc, 0));
    CHECK(DateTime(2011, 1, 2, 3, 4, 5, utc, 0) > DateTime(2011, 1, 2, 3, 4, 0, utc, 0));

    // encoding
    CHECK(ts.to_zinc() == "2011-06-06T12:26:58.069-04:00 New_York");

    CHECK(ts.tz_offset == -4 * 60 * 60);
    CHECK(ts.tz.name == "New_York");
    CHECK(ts.millis() == 1307377618069ULL);

    // convert back to millis
    CHECK(DateTime(ts.date, ts.time, ts.tz, ts.tz_offset).millis() == 1307377618069L);
    // different timezones 
    CHECK(DateTime::make(949478640000LL, TimeZone("New_York", -5)).to_zinc() == "2000-02-02T03:04:00-05:00 New_York");
    CHECK(DateTime::make(949478640000LL, TimeZone("UTC")).to_zinc() == "2000-02-02T08:04:00Z UTC");
    CHECK(DateTime::make(949478640000LL, TimeZone("Taipei", 8)).to_zinc() == "2000-02-02T16:04:00+08:00 Taipei");
    CHECK(DateTime::make(949478640000LL, TimeZone("GMT+8")).to_zinc() == "2000-02-02T16:04:00+08:00 GMT+8");
    CHECK(DateTime(2011, 6, 7, 11, 3, 43, TimeZone("GMT+10"), -36000).to_zinc() == "2011-06-07T11:03:43-10:00 GMT+10");
    CHECK(DateTime(2011, 6, 7, 11, 3, 43, TimeZone("GMT+10"), -36000).to_zinc() == "2011-06-07T11:03:43-10:00 GMT+10");
    CHECK(DateTime(Date(2011, 6, 8), Time(4, 7, 33, 771), TimeZone("GMT-7"), 25200).to_zinc() == "2011-06-08T04:07:33.771+07:00 GMT-7");

    // errors
    CHECK_THROWS(READ("2000-02-02T03:04:00-0x:00 New_York"));
    CHECK_THROWS(READ("2000-02-02T03:04:00-05 New_York"));
    CHECK_THROWS(READ("2000-02-02T03:04:00-05:!0 New_York"));
    CHECK_THROWS(READ("2000-02-02T03:04:00-05:00"));
    CHECK_THROWS(READ("2000-02-02T03:04:00-05:00 @"));
}

///////////////////////////////////////////////////////////
// Marker
///////////////////////////////////////////////////////////
TEST_CASE("Marker testcase", "[Marker]")
{
    const Marker& m = Marker::VAL;
    CHECK(m == Marker::VAL);
    CHECK(m.to_string() == "marker");
    CHECK(m.to_zinc() == "M");
    // equality
    CHECK(Marker::VAL == Marker::VAL);

    // toString
    CHECK(Marker::VAL.to_string() == "marker");

    // zinc
    VERIFY_ZINC(Marker::VAL, "M");
}

///////////////////////////////////////////////////////////
// Num
///////////////////////////////////////////////////////////
TEST_CASE("Num testcase", "[Num]")
{
    // api
    Num n(42.00, "\u00b0F");
    CHECK(n.value == 42.00);
    CHECK(n.value >= 41);
    // equality
    CHECK(Num(12.8) == Num(12.8, ""));
    CHECK(Num(2) == Num(2.0, ""));
    CHECK(Num(2) != Num(2, "%"));
    CHECK(Num(2, "%") != Num(2));
    CHECK(Num(0) == Num(0.0));

    // compare
    CHECK(Num(9) < Num(11));
    CHECK(Num(-3) > Num(-4));
    CHECK(Num(-23) == Num(-23));

    // zinc
    VERIFY_ZINC(Num(123), "123");
    VERIFY_ZINC(Num(123.4, "m/s"), "123.4m/s");
    VERIFY_ZINC(Num(9.6, "m/s"), "9.6m/s");
    VERIFY_ZINC(Num(-5.2, "\u00b0F"), "-5.2\u00b0F");
    VERIFY_ZINC(Num(23, "%"), "23%");
    VERIFY_ZINC(Num(2.4e-3, "fl_oz"), "0.0024fl_oz");
    VERIFY_ZINC(Num(2.4e5, "$"), "240000$");
    CHECK(*READ("1234.56fl_oz") == Num(1234.56, "fl_oz"));
    CHECK(*READ("0.000028fl_oz") == Num(0.000028, "fl_oz"));

    // specials
    CHECK(Num(-std::numeric_limits<double>::infinity()).to_zinc() == "-INF");
    CHECK(Num(INFINITY).to_zinc() == "INF");
    CHECK(Num(NAN).to_zinc() == "NaN");

    // verify units never serialized for special values
    CHECK(Num(NAN, "ignore").to_zinc() == "NaN");
    CHECK(Num(INFINITY, "%").to_zinc() == "INF");
    CHECK(Num(-std::numeric_limits<double>::infinity(), "%").to_zinc() == "-INF");

    // verify bad unit names
    CHECK(Num::isUnitName("x_z") == true);
    CHECK(Num::isUnitName("x z") == false);

    CHECK_THROWS(Num(123.4, "foo bar"));
    CHECK_THROWS(Num(123.4, "foo,bar"));

    //std::cout << n.to_zinc() << "\n";
}

///////////////////////////////////////////////////////////
// Ref
///////////////////////////////////////////////////////////
TEST_CASE("Ref testcase", "[Ref]")
{
    // equality(ignore dis)
    CHECK(Ref("foo") == Ref("foo"));
    CHECK(Ref("foo") == Ref("foo", "foo"));
    CHECK(Ref("foo") != Ref("Foo"));

    // api
    const Ref& a = Ref("Misc.foo", "Some dis");
    CHECK(a.dis() == "Some dis");

    // encoding
    VERIFY_ZINC(Ref("1234-5678.foo:bar"), "@1234-5678.foo:bar");
    VERIFY_ZINC(Ref("1234-5678", "Foo Bar"), "@1234-5678 \"Foo Bar\"");
    VERIFY_ZINC(Ref("1234-5678", "Foo \"Bar\""), "@1234-5678 \"Foo \\\"Bar\\\"\"");

    // veify ids
    CHECK(Ref::isId("%") == false);
    CHECK(Ref::isId("a") == true);
    CHECK(Ref::isId("a-b:c") == true);
    CHECK(Ref::isId("a b") == false);

    // verify ctor
    CHECK_THROWS(Ref("@a"));
    CHECK_THROWS(Ref("a b"));
    CHECK_THROWS(Ref("a\n"));
    CHECK_THROWS(READ("@"));
}

///////////////////////////////////////////////////////////
// Str
///////////////////////////////////////////////////////////
TEST_CASE("String testcase", "[Str]")
{
    Str s("aaa");
    assert(s == "aaa");
    CHECK(s.to_string() == "aaa");
    CHECK(s.to_zinc() == "\"aaa\"");

    const Str& s1 = Str("aaa\n");
    CHECK(s1 == "aaa\n");
    CHECK(s1.to_string() == "aaa\n");
    CHECK(s1.to_zinc() == "\"aaa\\n\"");
    // api
    const Str& s3 = s1;
    CHECK(s3 == s1);

    // equality
    CHECK(Str("a") == Str("a"));
    CHECK(Str("a") != Str("b"));
    CHECK(Str("") == Str(""));

    // compare
    CHECK(Str("abc") < Str("z"));
    CHECK(Str("Foo") == Str("Foo"));

    // encoding
    VERIFY_ZINC(Str("hello"), "\"hello\"");
    VERIFY_ZINC(Str("_ \\ \" \n \r \t \x00A0 _"), "\"_ \\\\ \\\" \\n \\r \\t \\u00A0 _\"");
    VERIFY_ZINC(Str("\x00A0"), "\"\\u00A0\"");

    CHECK_THROWS(READ("\"end..."));
    CHECK_THROWS(READ("\"end...\n\""));
    CHECK_THROWS(READ("\"hi\" "));
}

///////////////////////////////////////////////////////////
// Uri
///////////////////////////////////////////////////////////
TEST_CASE("Uri testcase", "[Uri]")
{
    // equality
    CHECK(Uri("a") == Uri("a"));
    CHECK(Uri("a") != Uri("b"));
    CHECK(Uri("") == Uri(""));

    // compare
    CHECK(Uri("abc") < Uri("z"));
    CHECK(Uri("Foo") == Uri("Foo"));

    // encoding
    VERIFY_ZINC(Uri("http://foo.com/f?q"), "`http://foo.com/f?q`");
    VERIFY_ZINC(Uri("a$b"), "`a$b`");
    VERIFY_ZINC(Uri("a`b"), "`a\\`b`");
    VERIFY_ZINC(Uri("http\\:a\\?b"), "`http\\:a\\?b`");
    VERIFY_ZINC(Uri("\u00b0F.txt"), "`\u00b0F.txt`");

    // errors
    CHECK_THROWS(READ("`no end"));
    CHECK_THROWS(READ("`new\nline`"));
}

///////////////////////////////////////////////////////////
// Time
///////////////////////////////////////////////////////////
TEST_CASE("Time testcase", "[Time]")
{
    // equality
    CHECK(Time(1, 2, 3, 4) == Time(1, 2, 3, 4));
    CHECK(Time(1, 2, 3, 4) != Time(9, 2, 3, 4));
    CHECK(Time(1, 2, 3, 4) != Time(1, 9, 3, 4));
    CHECK(Time(1, 2, 3, 4) != Time(1, 2, 9, 9));

    // compare
    CHECK(Time(0, 0, 0, 0) < Time(0, 0, 0, 9));
    CHECK(Time(0, 0, 0, 0) < Time(0, 0, 1, 0));
    CHECK(Time(0, 1, 0, 0) > Time(0, 0, 0, 0));
    CHECK(Time(0, 0, 0, 0) < Time(2, 0, 0, 0));
    CHECK(Time(2, 0, 0, 0) == Time(2, 0, 0, 0));

    // encoding
    VERIFY_ZINC(Time(2, 3), "02:03:00");
    VERIFY_ZINC(Time(2, 3, 4), "02:03:04");
    VERIFY_ZINC(Time(2, 3, 4, 5), "02:03:04.005");
    VERIFY_ZINC(Time(2, 3, 4, 56), "02:03:04.056");
    VERIFY_ZINC(Time(2, 3, 4, 109), "02:03:04.109");
    VERIFY_ZINC(Time(2, 3, 10, 109), "02:03:10.109");
    VERIFY_ZINC(Time(2, 10, 59), "02:10:59");
    VERIFY_ZINC(Time(10, 59, 30), "10:59:30");
    VERIFY_ZINC(Time(23, 59, 59, 999), "23:59:59.999");

    CHECK_THROWS(READ("3:20:00"));
    CHECK_THROWS(READ("13:xx:00"));
    CHECK_THROWS(READ("13:45:0x"));
    CHECK_THROWS(READ("13:45:00.4561"));
}