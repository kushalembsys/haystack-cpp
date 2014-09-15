//
// Copyright (c) 2014, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   15 Sep 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//
#include "datetimerange.hpp"
#include "date.hpp"
#include "zincreader.hpp"

#include <boost/algorithm/string.hpp>

////////////////////////////////////////////////
// DateTimeRange
////////////////////////////////////////////////
using namespace haystack;

DateTimeRange::DateTimeRange(const Date& date, const TimeZone& tz) :
m_start(*date.midnight(tz)),
m_end(*date.inc_days(1).midnight(tz))
{}

DateTimeRange::DateTimeRange(const Date& s, const Date& e, const TimeZone& tz) : m_start(*s.midnight(tz)), m_end(*e.inc_days(1).midnight(tz)) {}

DateTimeRange::DateTimeRange(const DateTime& s, const DateTime& e) : m_start(s), m_end(e)
{
    if (start().tz != end().tz) throw std::runtime_error("start.tz != end.tz");
}

DateTimeRange::auto_ptr_t  DateTimeRange::this_week(const TimeZone& tz)
{
    const Date& today = Date::today();
    const Date& sun = today.dec_days(today.weekday() - SUNDAY);
    const Date& sat = today.inc_days(SATURDAY - today.weekday());
    return auto_ptr_t(new DateTimeRange(sun, sat, tz));
}

DateTimeRange::auto_ptr_t DateTimeRange::this_month(const TimeZone& tz)
{
    const Date& today = Date::today();
    Date first(today.year, today.month, 1);
    Date last(today.year, today.month, Date::days_in_month(today.year, today.month));
    return auto_ptr_t(new DateTimeRange(first, last, tz));
}

DateTimeRange::auto_ptr_t DateTimeRange::this_year(const TimeZone& tz)
{
    const Date& today = Date::today();
    Date first(today.year, 1, 1);
    Date last(today.year, 12, 31);
    return auto_ptr_t(new DateTimeRange(first, last, tz));
}

DateTimeRange::auto_ptr_t DateTimeRange::last_week(const TimeZone& tz)
{
    const Date& today = Date::today();
    const Date& prev = today.dec_days(7);
    const Date& sun = prev.dec_days(prev.weekday() - SUNDAY);
    const Date& sat = prev.inc_days(SATURDAY - prev.weekday());
    return auto_ptr_t(new DateTimeRange(sun, sat, tz));
}

DateTimeRange::auto_ptr_t DateTimeRange::last_month(const TimeZone& tz)
{
    const Date& today = Date::today();
    int year = today.year;
    int month = today.month;

    if (month == 1)
    {
        year--;
        month = 12;
    }
    else
    {
        month--;
    }
    Date first(year, month, 1);
    Date last(year, month, Date::days_in_month(year, month));
    return auto_ptr_t(new DateTimeRange(first, last, tz));
}

DateTimeRange::auto_ptr_t DateTimeRange::last_year(const TimeZone& tz)
{
    const Date& today = Date::today();
    Date first(today.year - 1, 1, 1);
    Date last(today.year - 1, 12, 31);
    return auto_ptr_t(new DateTimeRange(first, last, tz));
}

DateTimeRange::auto_ptr_t DateTimeRange::make(std::string str, const TimeZone& tz)
{
    // handle keywords

    boost::algorithm::trim(str);

    if (str == "today")     return auto_ptr_t(new DateTimeRange(Date::today(), tz));
    if (str == "yesterday") return auto_ptr_t(new DateTimeRange(Date::today().dec_days(1), tz));

    // parse scalars
    size_t comma = str.find(',');
    Val::auto_ptr_t start, end;
    if (comma == str.npos)
    {
        start = ZincReader::make(str)->read_scalar();
    }
    else
    {
        start = ZincReader::make(str.substr(0, comma))->read_scalar();
        end = ZincReader::make(str.substr(comma + 1))->read_scalar();
    }

    // figure out what we parsed for start,end
    if (start->type() == Val::DATE_TYPE)
    {
        if (end.get() == NULL) return auto_ptr_t(new DateTimeRange((Date&)*start, tz));
        if (end->type() == Val::DATE_TYPE) return auto_ptr_t(new DateTimeRange((Date&)*start, (Date&)*end, tz));
    }
    else if (start->type() == Val::DATE_TIME_TYPE)
    {
        if (end.get() == NULL) return auto_ptr_t(new DateTimeRange((DateTime&)*start, DateTime::now(tz)));
        if (end->type() == Val::DATE_TIME_TYPE) return auto_ptr_t(new DateTimeRange((DateTime&)*start, (DateTime&)*end));
    }

    throw std::runtime_error("Invalid HDateTimeRange: " + str);
}

////////////////////////////////////////////////
// to string
////////////////////////////////////////////////

const std::string DateTimeRange::to_string() const
{
    return start().to_string() + end().to_string();
}
