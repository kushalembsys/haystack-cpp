#include "testproj.hpp"
#include "dict.hpp"
#include "marker.hpp"
#include "num.hpp"
#include "ref.hpp"
#include "uri.hpp"
#include "op.hpp"

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
    m_recs.insert(k, site.release());
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
    m_recs.insert(k, equip.release());
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
    m_recs.insert(k, equip.release());
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