//
// Copyright (c) 2014, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   09 Sep 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

#include "testproj.hpp"
#include "dict.hpp"
#include "bool.hpp"
#include "marker.hpp"
#include "num.hpp"
#include "ref.hpp"
#include "uri.hpp"
#include "hisitem.hpp"
#include "datetime.hpp"
#include "datetimerange.hpp"
#include "op.hpp"

#include <iostream>

#include <boost/scoped_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>

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
    // lazy init
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
    const StdOps::ops_map_t& o_map = StdOps::ops_map();
    std::map<std::string, const Op* const>::const_iterator it = o_map.find(name);

    if (checked && it == o_map.end())
        throw std::runtime_error("Unknown Op Name");
    else
        return it != o_map.end() ? it->second : NULL;
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
        return Dict::auto_ptr_t();
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
        boost::scoped_ptr<Val*> v(new Val*[res->num_cols()]);
        // copy each Val from the row
        for (size_t i = 0; i < res->num_cols() - 1; i++)
        {
            (v.get())[i] = new_clone(row->get(res->col(i).name(), false));
        }
        // add the new 'navId' tag
        (v.get())[res->num_cols() - 1] = new_clone(Str(row->id().value));
        // add the new row to the result grid
        res->add_row(v.get(), res->num_cols());
    }

    return res;
}

//////////////////////////////////////////////////////////////////////////
// Watches
//////////////////////////////////////////////////////////////////////////


Watch::shared_ptr TestProj::on_watch_open(const std::string& dis)
{
    Watch::shared_ptr w = boost::make_shared<TestWatch>(*this, dis);

    m_watches[w->id()] = w;

    return w;
}

const std::vector<Watch::shared_ptr> TestProj::on_watches()
{
    std::vector<Watch::shared_ptr> v;
    for (watches_t::const_iterator it = m_watches.begin(), e = m_watches.end(); it != e; ++it)
    {
        v.push_back(it->second);
    }

    return v;
}

Watch::shared_ptr TestProj::on_watch(const std::string& id)
{
    return  m_watches[id];
}

Dict::auto_ptr_t TestProj::on_nav_read_by_uri(const Uri& uri) const
{
    return Dict::auto_ptr_t();
}

//////////////////////////////////////////////////////////////////////////
// Point Write
//////////////////////////////////////////////////////////////////////////

Grid::auto_ptr_t TestProj::on_point_write_array(const Dict& rec)
{
    Grid::auto_ptr_t g(new Grid);

    g->add_col("level");
    g->add_col("levelDis");
    g->add_col("val");
    g->add_col("who");

    for (int i = 0; i < 17; ++i)
    {
        Val* v[4] = {
            new Num(i + 1),
            new Str("" + (i + 1)),
            new Num(i + 1),
            new Str("boo"),
        };
        g->add_row(v, 4);

    }

    return g;
}

void TestProj::on_point_write(const Dict& rec, int level, const Val& val, const std::string& who, const Num& dur)
{
    std::cout << "on_point_write" << rec.dis() << " " + val.to_string() << "@" << level << " [" << who << "]\n";
}

//////////////////////////////////////////////////////////////////////////
// History
//////////////////////////////////////////////////////////////////////////

std::vector<HisItem> TestProj::on_his_read(const Dict& entity, const DateTimeRange& range)
{
    // generate dummy 15min data
    std::vector<HisItem> acc;

    DateTime::auto_ptr_t ts = range.start().clone();
    bool isBool = entity.get_str("kind") == "Bool";
    while (ts->as<DateTime>() < range.end())
    {
        Val::auto_ptr_t val = isBool ?
            Bool(acc.size() % 2 == 0).clone() :
            Num((long long)acc.size()).clone();

        HisItem item(ts->as<DateTime>(), *val);
        if (ts->as<DateTime>() != range.start())
            acc.push_back(item);
        ts = DateTime::make(ts->as<DateTime>().millis() + 15 * 60 * 1000).clone();
    }
    return acc;
}

void TestProj::on_his_write(const Dict& rec, const std::vector<HisItem>& items)
{
    throw std::runtime_error("Unsupported Op 'on_his_write'");
}

//////////////////////////////////////////////////////////////////////////
// Actions
//////////////////////////////////////////////////////////////////////////

Grid::auto_ptr_t TestProj::on_invoke_action(const Dict& rec, const std::string& action, const Dict& args)
{
    std::cout << "-- invokeAction \"" << rec.dis() << "." << action << "\" " << args.to_string() << "\n";
    return Grid::auto_ptr_t();
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
        .add("area", Num(area, "ft\xc2\xb2"));


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
        .add("cur", Marker::VAL)
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

//////////////////////////////////////////////////////////////////////////
// TestWatch Impl
//////////////////////////////////////////////////////////////////////////

TestWatch::TestWatch(const TestProj& server, const std::string& dis) :
m_server(server),
m_uuid(boost::lexical_cast<std::string>(boost::uuids::random_generator()())),
m_dis(dis),
m_lease((size_t)-1),
m_is_open(false){}

const std::string TestWatch::id() const
{
    return m_uuid;
}

const std::string TestWatch::dis() const
{
    return m_dis;
}

const size_t TestWatch::lease() const
{
    return m_lease;
}

Grid::auto_ptr_t TestWatch::sub(const refs_t& ids, bool checked)
{
    m_ids = ids;

    const TestProj::recs_t& recs = m_server.m_recs;

    std::vector<const Dict*> res;

    for (refs_t::iterator id = m_ids.begin(), e = m_ids.end(); id != e; ++id)
    {
        TestProj::recs_t::const_iterator rec = recs.find(id->value);

        if (rec == recs.end())
        {
            m_ids.erase(id, id);

            if (checked)
                throw std::runtime_error("Id not found: " + id->value);

            continue;
        }

        res.push_back(rec->second);
    }

    m_is_open = true;

    Grid::auto_ptr_t g = Grid::make(res);
    g->meta().add("watchId", m_uuid).add("lease", m_lease);

    return g;
}

void TestWatch::unsub(const refs_t& ids)
{
    for (refs_t::iterator id = m_ids.begin(), e = m_ids.end(); id != e; ++id)
    {
        if (std::find(ids.begin(), ids.end(), *id) != ids.end())
        {
            m_ids.erase(id, id);
        }
    }
}

Grid::auto_ptr_t TestWatch::poll_changes()
{
    return poll_refresh();
}

Grid::auto_ptr_t TestWatch::poll_refresh()
{
    const TestProj::recs_t& recs = m_server.m_recs;
    boost::ptr_vector<Dict> res;

    boost::mt19937 rng(static_cast<uint32_t>(time(NULL)));
    boost::uniform_real<> range(0.0, 100.0);
    boost::variate_generator<boost::mt19937&, boost::uniform_real<> > gen(rng, range);

    for (refs_t::iterator id = m_ids.begin(), e = m_ids.end(); id != e; ++id)
    {
        TestProj::recs_t::const_iterator rec = recs.find(id->value);
        Dict::auto_ptr_t row(new Dict);
        // clone the record
        row->add(*rec->second);
        // add the cur value
        if (rec->second->get_str("kind") == "Number")
        {
            row->add("curVal", Num(gen()));
        }
        else if (rec->second->get_str("kind") == "Bool")
        {
            row->add("curVal", Bool((((int)gen()) % 2 == 0 ? true : false)));
        }

        res.push_back(row);
    }

    Grid::auto_ptr_t g = Grid::make(res);
    g->meta().add("watchId", m_uuid).add("lease", m_lease);
    return g;
}

void TestWatch::close()
{
    m_is_open = false;
}

bool TestWatch::is_open() const
{
    return m_is_open;
}
