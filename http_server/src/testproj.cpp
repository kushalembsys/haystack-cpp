#include "testproj.hpp"
#include "dict.hpp"
#include "marker.hpp"
#include "num.hpp"
#include "ref.hpp"
#include "uri.hpp"
#include "op.hpp"
#include <boost/scoped_ptr.hpp>
//
// Copyright (c) 2014, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   09 Sep 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//
#include "Poco/StringTokenizer.h"
#include "Poco/Net/DNS.h"

#include <stdio.h>

using namespace haystack;

TestProj::TestProj()
{
    add_site("A", "Richmond", "VA", 1000);
    add_site("B", "Richmond", "VA", 2000);
    add_site("C", "Washington", "DC", 3000);
    add_site("D", "Boston", "MA", 4000);
}

const std::vector<const Op*>& TestProj::ops()
{

    if (m_ops != NULL)
        return (std::vector<const Op*>&)*m_ops;

    std::vector<const Op*>* v = new std::vector<const Op*>();

    StdOps::ops_map_t::const_iterator it = StdOps::ops_map().begin(), end = StdOps::ops_map().end();

    for (; it != end; ++it)
    {
        v->push_back(it->second);
    }

    return (std::vector<const Op*>&)*m_ops;
}

const Op* const TestProj::op(const std::string& name, bool checked) const
{
    std::map<std::string, const Op* const>::const_iterator it = StdOps::ops_map().find(name);

    if (checked && it == StdOps::ops_map().end())
        throw std::runtime_error("Unknown Op Name");
    else
        return it == StdOps::ops_map().end() ? NULL : it->second;
}

const Dict& TestProj::on_about() const 
{
    if (m_about != NULL)
        return *m_about; 
    
    m_about = new Dict();
    m_about->add("serverName", Poco::Net::DNS::hostName())
        .add("vendorName", "Haystack C++ Toolkit")
        .add("vendorUri", Uri("http://project-haystack.org/"))
        .add("productName", "Haystack C++ Toolkit")
        .add("productVersion", "2.0.0")
        .add("productUri", Uri("http://project-haystack.org/"));

    return *m_about;
}

Dict::auto_ptr_t TestProj::on_read_by_id(const Ref& id) const
{
    recs_t::const_iterator it = m_recs.find(id.value);

    if (it != m_recs.end())
        return ((Dict*)it->second)->clone();
    else
        return ((Dict&)Dict::EMPTY).clone();
}

//////////////////////////////////////////////////////////////////////////
// Navigation
//////////////////////////////////////////////////////////////////////////

Grid::auto_ptr_t TestProj::on_nav(const std::string& nav_id) const
{
    // test database navId is record id
    Dict::auto_ptr_t base;
    if (!nav_id.empty()) base = read_by_id(Ref(nav_id));

    // map base record to site, equip, or point
    std::string filter = "site";
    if (base.get() != NULL)
    {
        if (base->has("site")) filter = "equip and siteRef==" + base->id().to_code();
        else if (base->has("equip")) filter = "point and equipRef==" + base->id().to_code();
        else filter = "navNoChildren";
    }

    // read children of base record
    Grid::auto_ptr_t read = read_all(filter);

    Grid::auto_ptr_t res(new Grid);
    res->reserve_rows(read->num_rows());

    // copy columns
    for (size_t i = 0; i < read->num_cols(); i++)
    {
        const Col& srcCol = read->col(i);
        res->add_col(srcCol.name()).add(srcCol.meta());
    }
    // add a new colum
    res->add_col("navId");
    // copy rows
    for (Grid::const_iterator row = read->begin(), e = read->end(); row != e; ++row)
    {
        const std::string& id = row->id().value;
        boost::scoped_ptr<Val*> v(new Val*[res->num_cols()]);
        // copy each Val from the row
        for (size_t i = 0; i < res->num_cols() - 1; i++)
        {
            (v.get())[i] = (Val*)row->get(res->col(i).name(), false).clone().release();
        }
        // add the new 'navId' tag
        (v.get())[res->num_cols() - 1] = (Val*) Str(row->id().value).clone().release();
        // add the new row to the result grid
        res->add_row(v.get(), res->num_cols());
    }

    return res;
}

Dict::auto_ptr_t TestProj::on_nav_read_by_uri(const Uri& uri) const
{
    return Dict::auto_ptr_t();
}

//////////////////////////////////////////////////////////////////////////
// Impl
//////////////////////////////////////////////////////////////////////////

void TestProj::add_site(const std::string& dis, const std::string& geoCity, const std::string& geoState, int area)
{
    Dict::auto_ptr_t site(new Dict);
    site->add("id", Ref(dis))
        .add("dis", dis)
        .add("site", Marker::VAL)
        .add("geoCity", geoCity)
        .add("geoState", geoState)
        .add("geoAddr", "" + geoCity + "," + geoState)
        .add("tz", "New_York")
        .add("area", Num(area, "ft\x00B2"));


    add_meter(*site, dis + "-Meter");
    add_ahu(*site, dis + "-AHU1");
    add_ahu(*site, dis + "-AHU2");

    std::string k = dis;
    m_recs.insert(k, site);
}

void TestProj::add_meter(Dict& site, const std::string& dis)
{
    Dict::auto_ptr_t equip(new Dict);
    equip->add("id", Ref(dis))
        .add("dis", dis)
        .add("equip", Marker::VAL)
        .add("elecMeter", Marker::VAL)
        .add("siteMeter", Marker::VAL)
        .add("siteRef", site.get("id"));

    add_point(*equip, dis + "-KW", "kW", "elecKw");
    add_point(*equip, dis + "-KWH", "kWh", "elecKwh");

    std::string k = dis;
    m_recs.insert(k, equip);
}

void TestProj::add_ahu(Dict& site, const std::string& dis)
{
    Dict::auto_ptr_t equip(new Dict);
    equip->add("id", Ref(dis))
        .add("dis", dis)
        .add("equip", Marker::VAL)
        .add("ahu", Marker::VAL)
        .add("siteRef", site.get("id"));

    add_point(*equip, dis + "-Fan", "", "discharge air fan cmd");
    add_point(*equip, dis + "-Cool", "", "cool cmd");
    add_point(*equip, dis + "-Heat", "", "heat cmd");
    add_point(*equip, dis + "-DTemp", "\u00B0F", "discharge air temp sensor");
    add_point(*equip, dis + "-RTemp", "\u00B0F", "return air temp sensor");
    add_point(*equip, dis + "-ZoneSP", "\u00B0F", "zone air temp sp writable");

    std::string k = dis;
    m_recs.insert(k, equip);
}

void TestProj::add_point(Dict& equip, const std::string& dis, const std::string& unit, const std::string& markers)
{
    Dict::auto_ptr_t d(new Dict);

    d->add("id", Ref(dis))
        .add("dis", dis)
        .add("point", Marker::VAL)
        .add("his", Marker::VAL)
        .add("siteRef", equip.get("siteRef"))
        .add("equipRef", equip.get("id"))
        .add("kind", unit.empty() ? "Bool" : "Number")
        .add("tz", "New_York");
    if (!unit.empty()) d->add("unit", unit);

    Poco::StringTokenizer st(markers, " ");

    for (Poco::StringTokenizer::Iterator it = st.begin(), end = st.end(); it != end; ++it)
        d->add(*it);

    std::string k = dis;
    m_recs.insert(k, d.release());
}

Dict* TestProj::m_about = NULL;
std::vector<const Op*>* TestProj::m_ops = NULL;

//////////////////////////////////////////////////////////////////////////
// Reads
//////////////////////////////////////////////////////////////////////////

Server::const_iterator TestProj::begin() const { return const_iterator(m_recs.begin()); }
Server::const_iterator TestProj::end() const { return const_iterator(m_recs.end()); }