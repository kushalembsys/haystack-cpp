//
// Copyright (c) 2014, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   08 Sep 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

#include "op.hpp"
#include "str.hpp"
#include "marker.hpp"
#include "num.hpp"
#include "zincreader.hpp"
#include "zincwriter.hpp"
#include "server.hpp"
#include "watch.hpp"

// std
#include <sstream>
#include <stdio.h>
// poco
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/NameValueCollection.h"

using namespace haystack;

// Service the request and return response.
// This method routes to "on_service(const Server& db, HTTPServerRequest& req, HTTPServerResponse& res)".
void Op::on_service(const Server& db, HTTPServerRequest& req, HTTPServerResponse& res)
{
    // parse GET query parameters or POST body into grid
    Grid::auto_ptr_t reqGrid;
    const std::string& method = req.getMethod();
    if (method == "GET")  reqGrid = get_to_grid(req);
    if (method == "POST") reqGrid = post_to_grid(req, res);
    // unhandeld request type
    if (reqGrid.get() == NULL)
        return;

    // send response
    res.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
    res.setContentType("text/zinc; charset=utf-8");

    std::ostream& ostr = res.send();
    ZincWriter w(ostr);

    // route to onService(HServer, HGrid)
    try
    {
        w.write_grid(*on_service(const_cast<Server&>(db), *reqGrid));
    }
    catch (std::runtime_error& e)
    {
        w.write_grid(*Grid::make_err(e));
    }
}

// Service the request and return response.
Grid::auto_ptr_t Op::on_service(Server& db, const Grid& req)
{
    throw std::runtime_error("Not implemented Op::on_service(const Server& db, const Grid& req)");
    return  Grid::auto_ptr_t();
}

Row::val_vec_t Op::grid_to_ids(const Server& db, const Grid& grid) const
{
    Row::val_vec_t ids(grid.num_rows());

    for (Grid::const_iterator it = grid.begin(), e = grid.end(); it != e; ++it)
    {
        const Val& val = it->get("id");
        ids.push_back((Val*)val_to_id(db, val).release());
    }
    return ids;
}

Ref::auto_ptr_t Op::val_to_id(const Server& db, const Val& val) const
{
    if (val.type() == Val::URI_TYPE)
    {
        Dict::auto_ptr_t rec = db.nav_read_by_uri((Uri&)val, false);
        return rec.get() == NULL ? Ref("").clone() : rec->id().clone();
    }
    else
    {
        return val.clone();
    }
}

// Map the GET query parameters to grid with one row
Grid::auto_ptr_t  Op::get_to_grid(HTTPServerRequest& req)
{
    using namespace Poco::Net;
    HTMLForm form(req);

    if (form.empty())
        return Grid::auto_ptr_t(new Grid);

    NameValueCollection::ConstIterator it = form.begin();
    NameValueCollection::ConstIterator end = form.end();

    Dict d;

    for (; it != end; ++it)
    {
        const std::string& name = it->first;
        const std::string& val_str = it->second;
        Val::auto_ptr_t val;
        try
        {
            val = ZincReader::make(val_str)->read_scalar();
        }
        catch (std::exception&)
        {
            val = Str(val_str).clone();
        }

        d.add(name, val.release());
    }

    return Grid::make(d);
}

// Map the POST body to grid
Grid::auto_ptr_t Op::post_to_grid(HTTPServerRequest& req, HTTPServerResponse& res)
{
    const std::string& mime = req.getContentType();
    if (mime != "text/zinc")
        res.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_NOT_ACCEPTABLE, mime);

    ZincReader r(req.stream());

    return r.read_grid();
}

//////////////////////////////////////////////////////////////////////////
// AboutOp
//////////////////////////////////////////////////////////////////////////
class AboutOp : public Op
{
public:
    std::string name() const { return "about"; }
    std::string summary() const { return "Summary information for server"; }

    Grid::auto_ptr_t on_service(Server& db, const Grid& req)
    {
        return Grid::make(*db.about());
    }
};

//////////////////////////////////////////////////////////////////////////
// OpsOp
//////////////////////////////////////////////////////////////////////////
class OpsOp : public Op
{
public:
    std::string name() const { return "ops"; }
    std::string summary() const { return "Operations supported by this server"; }

    Grid::auto_ptr_t on_service(Server& db, const Grid& req)
    {
        Grid::auto_ptr_t g(new Grid);
        // lazy init the response grid
        g->add_col("name");
        g->add_col("summary");

        StdOps::ops_map_t::const_iterator it = StdOps::ops_map().begin();
        StdOps::ops_map_t::const_iterator end = StdOps::ops_map().end();

        for (; it != end; ++it)
        {
            Val* vals[2] = { new Str(it->second->name()), new Str(it->second->summary()) };
            g->add_row(vals, sizeof(vals) / sizeof(Val*));
        }

        return g;
    }

};

//////////////////////////////////////////////////////////////////////////
// FormatsOp
//////////////////////////////////////////////////////////////////////////
class FormatsOp : public Op
{
public:
    std::string name() const { return "formats"; }
    std::string summary() const { return "Grid data formats supported by this server"; }

    Grid::auto_ptr_t on_service(Server& db, const Grid& req)
    {
        Grid::auto_ptr_t g(new Grid);

        // init the response grid
        g->add_col("mime");
        g->add_col("read");
        g->add_col("write");

        Val* v[3] = {
            new Str("text/zinc"),
            new Marker(),
            new Marker() };
        g->add_row(v, 3);

        return g;
    }

};

//////////////////////////////////////////////////////////////////////////
// ReadOp
//////////////////////////////////////////////////////////////////////////
class ReadOp : public Op
{
public:
    std::string name() const { return "read"; }
    std::string summary() const { return "Read entity records in database"; }

    Grid::auto_ptr_t on_service(Server& db, const Grid& req)
    {
        // ensure we have one row
        if (req.is_empty())
            throw std::runtime_error("Request has no rows");

        // perform filter or id read
        const Row& row = req.row(0);

        if (row.has("filter"))
        {
            // filter read
            const std::string& filter = row.get_string("filter");
            size_t limit = static_cast<size_t>(row.has("limit") ? row.get_double("limit") : (size_t)-1);
            return db.read_all(filter, limit);
        }

        return  Grid::auto_ptr_t();
    }
};

//////////////////////////////////////////////////////////////////////////
// NavOp
//////////////////////////////////////////////////////////////////////////
class NavOp : public Op
{
public:
    std::string name() const { return "nav"; }
    std::string summary() const { return "Navigate record tree"; }
    Grid::auto_ptr_t on_service(Server& db, const Grid& req)
    {
        // ensure we have one row
        std::string nav_id;
        if (!req.is_empty())
        {
            const Val& val = req.row(0).get("navId", false);
            if (val.type() == Val::STR_TYPE) nav_id = ((Str&)val).value;
        }
        return db.nav(nav_id);
    }
};

//////////////////////////////////////////////////////////////////////////
// WatchSubOp
//////////////////////////////////////////////////////////////////////////
class WatchSubOp : public Op
{
public:
    std::string name() const { return "watchSub"; }
    std::string summary() const { return "Watch subscription"; }
    Grid::auto_ptr_t on_service(Server& db, const Grid& req)
    {
        // check for watchId or watchId
        std::string watchId;
        std::string watchDis;

        if (req.meta().has("watchId"))
            watchId = req.meta().get_str("watchId");
        else
            watchDis = req.meta().get_str("watchDis");

        // open or lookup watch
        Watch::shared_ptr watch = watchId.empty() ?
            db.watch_open(watchDis) :
            db.watch(watchId);

        // map grid to ids
        const Row::val_vec_t& ids = grid_to_ids(db, req);

        // subscribe and return resulting grid
        return watch->sub(ids);
    }
};

//////////////////////////////////////////////////////////////////////////
// WatchUnsubOp
//////////////////////////////////////////////////////////////////////////
class WatchUnsubOp : public Op
{
public:
    std::string name() const { return "watchUnsub"; }
    std::string summary() const { return "Watch unsubscription"; }
    Grid::auto_ptr_t on_service(Server& db, const Grid& req)
    {
        // lookup watch, silently ignore failure
        const std::string& watchId = req.meta().get_str("watchId");
        Watch::shared_ptr watch = db.watch(watchId, false);

        // check for close or unsub
        if (watch.get() != NULL)
        {
            if (req.meta().has("close"))
                watch->close();
            else
                watch->unsub(grid_to_ids(db, req));
        }

        // nothing to return
        return Grid::auto_ptr_t();
    }
};

//////////////////////////////////////////////////////////////////////////
// WatchPollOp
//////////////////////////////////////////////////////////////////////////
class WatchPollOp : public Op
{
public:
    std::string name() const  { return "watchPoll"; }
    std::string summary() const { return "Watch poll cov or refresh"; }
    Grid::auto_ptr_t on_service(Server& db, const Grid& req)
    {
        // lookup watch
        const std::string& watchId = req.meta().get_str("watchId");
        Watch::shared_ptr watch = db.watch(watchId);

        // poll cov or refresh
        if (req.meta().has("refresh"))
            return watch->poll_refresh();
        else
            return watch->poll_changes();
    }
};

//////////////////////////////////////////////////////////////////////////
// PointWriteOp
//////////////////////////////////////////////////////////////////////////
class PointWriteOp : public Op
{
public:
    std::string name() const { return "pointWrite"; }
    std::string summary() const { return "Read/write writable point priority array"; }
    Grid::auto_ptr_t on_service(Server& db, const Grid& req)
    {
        // get required point id
        if (req.is_empty()) throw std::runtime_error("Request has no rows");
        const Row& row = req.row(0);
        Ref::auto_ptr_t id = val_to_id(db, row.get("id"));

        // check for write
        if (row.has("level"))
        {
            int level = static_cast<int>(row.get_int("level"));
            const std::string& who = row.get_str("who"); // be nice to have user fallback
            const Val& val = row.get("val", false);
            const Num& dur = (Num&)row.get("duration", false);
            db.point_write((Ref&)*id, level, val, who, dur);
        }

        return db.point_write_array((Ref&)*id);
    }
};

// List the registered operations.
const Op& StdOps::about = *new AboutOp();
// List the registered grid formats.
const Op& StdOps::formats = *new FormatsOp();
// Read entity records in database.
const Op& StdOps::read = *new ReadOp();
// Navigate tree structure of database.
const Op& StdOps::nav = *new NavOp();
// Watch subscription.
const Op& StdOps::watchSub = *new WatchSubOp();
// Watch unsubscription.
const Op& StdOps::watchUnsub = *new WatchUnsubOp();
// Watch poll cov or refresh.
const Op& StdOps::watchPoll = *new WatchPollOp();
// Read/write writable point priority array.
const Op& StdOps::pointWrite = * new PointWriteOp();

// List the registered operations.
const Op& StdOps::ops = *new OpsOp();

const StdOps::ops_map_t& StdOps::ops_map()
{
    if (m_ops_map != NULL)
        return *m_ops_map;

    StdOps::m_ops_map = new StdOps::ops_map_t();

    m_ops_map->insert(std::pair<std::string, const Op* const>(StdOps::about.name(), &StdOps::about));
    m_ops_map->insert(std::pair<std::string, const Op* const>(StdOps::formats.name(), &StdOps::formats));
    m_ops_map->insert(std::pair<std::string, const Op* const>(StdOps::read.name(), &StdOps::read));
    m_ops_map->insert(std::pair<std::string, const Op* const>(StdOps::nav.name(), &StdOps::nav));
    m_ops_map->insert(std::pair<std::string, const Op* const>(StdOps::watchSub.name(), &StdOps::watchSub));
    m_ops_map->insert(std::pair<std::string, const Op* const>(StdOps::watchUnsub.name(), &StdOps::watchUnsub));
    m_ops_map->insert(std::pair<std::string, const Op* const>(StdOps::watchPoll.name(), &StdOps::watchPoll));
    m_ops_map->insert(std::pair<std::string, const Op* const>(StdOps::pointWrite.name(), &StdOps::pointWrite));
    m_ops_map->insert(std::pair<std::string, const Op* const>(StdOps::ops.name(), &StdOps::ops));

    return *m_ops_map;
}


StdOps::ops_map_t* StdOps::m_ops_map = NULL;
