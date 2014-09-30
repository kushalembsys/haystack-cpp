//
// Copyright (c) 2014, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   08 Sep 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

#include "op.hpp"
#include "bool.hpp"
#include "marker.hpp"
#include "num.hpp"
#include "str.hpp"
#include "uri.hpp"
#include "zincreader.hpp"
#include "zincwriter.hpp"
#include "datetimerange.hpp"
#include "hisitem.hpp"
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
    if (method == "GET")
    {
        reqGrid = get_to_grid(req);
    }
    else if (method == "POST")
    {
        reqGrid = post_to_grid(req, res);
        if (reqGrid.get() == NULL)
            return;
    }
    else
    {
        // unhandeld request type
        return;
    }
    // send response
    res.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
    res.setContentType("text/zinc; charset=utf-8");

    std::ostream& ostr = res.send();
    ZincWriter w(ostr);

    // route to on_service(Server& db, const Grid& req)
    try
    {
        Grid::auto_ptr_t g;

        if (reqGrid.get() != NULL)
            g = on_service(const_cast<Server&>(db), *reqGrid);
        else
            g = on_service(const_cast<Server&>(db), Grid::EMPTY);

        if (g.get() != NULL)
            w.write_grid(*g);
        else
            w.write_grid(Grid::EMPTY);
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

Op::refs_t Op::grid_to_ids(const Server& db, const Grid& grid) const
{
    refs_t ids(grid.num_rows());

    for (Grid::const_iterator it = grid.begin(), e = grid.end(); it != e; ++it)
    {
        const Ref& val = it->get_ref("id");
        ids.push_back((Ref*)val_to_id(db, val).release());
    }
    return ids;
}

Val::auto_ptr_t Op::val_to_id(const Server& db, const Val& val) const
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
        return Grid::auto_ptr_t();

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

        d.add(name, val);
    }

    return Grid::make(d);
}

// Map the POST body to grid
Grid::auto_ptr_t Op::post_to_grid(HTTPServerRequest& req, HTTPServerResponse& res)
{
    const std::string& mime = req.getContentType();
    if (mime.find("text/zinc") == mime.npos && mime.find("text/plain") == mime.npos)
    {
        res.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_NOT_ACCEPTABLE, mime);
        res.send();
        return Grid::auto_ptr_t();
    }

    return ZincReader(req.stream()).read_grid();
}

//////////////////////////////////////////////////////////////////////////
// AboutOp
//////////////////////////////////////////////////////////////////////////
class AboutOp : public Op
{
public:
    const std::string name() const { return "about"; }
    const std::string summary() const { return "Summary information for server"; }

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
    const std::string name() const { return "ops"; }
    const std::string summary() const { return "Operations supported by this server"; }

    Grid::auto_ptr_t on_service(Server& db, const Grid& req)
    {
        Grid::auto_ptr_t g(new Grid);

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
    const std::string name() const { return "formats"; }
    const std::string summary() const { return "Grid data formats supported by this server"; }

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
    const std::string name() const { return "read"; }
    const std::string summary() const { return "Read entity records in database"; }

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
        else if (row.has("id"))
        {
            boost::ptr_vector<Ref> v;

            Grid::const_iterator it = req.begin();
            for (; it != req.end(); ++it)
            {
                if (it->has("id"))
                    v.push_back(new_clone((Ref&)it->get("id")));
            }

            return db.read_by_ids(v);
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
    const std::string name() const { return "nav"; }
    const std::string summary() const { return "Navigate record tree"; }
    Grid::auto_ptr_t on_service(Server& db, const Grid& req)
    {
        // ensure we have one row
        std::string nav_id;
        if (!req.is_empty())
        {
            const Val& val = req.row(0).get("navId", false);
            if (val.type() == Val::STR_TYPE)
                nav_id = val.as<Str>().value;
            else if (val.type() == Val::URI_TYPE)
                nav_id = val.as<Uri>().value;
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
    const std::string name() const { return "watchSub"; }
    const std::string summary() const { return "Watch subscription"; }
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

        if (watch.get() == NULL)
            return Grid::make_err(std::runtime_error("Watch not found."));

        // map grid to ids
        const Op::refs_t& ids = grid_to_ids(db, req);

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
    const std::string name() const { return "watchUnsub"; }
    const std::string summary() const { return "Watch unsubscription"; }
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
    const std::string name() const  { return "watchPoll"; }
    const std::string summary() const { return "Watch poll cov or refresh"; }

    Grid::auto_ptr_t on_service(Server& db, const Grid& req)
    {
        // lookup watch
        const std::string& watchId = req.meta().get_str("watchId");
        Watch::shared_ptr watch = db.watch(watchId);

        // check for close or unsub
        if (watch.get() != NULL)
        {
            // poll cov or refresh
            if (req.meta().has("refresh"))
                return watch->poll_refresh();
            else
                return watch->poll_changes();
        }
        Grid::auto_ptr_t g = Grid::make_err(std::runtime_error("Watch not found. " + watchId));
        g->meta().add("watchId", watchId);
        return g;
    }
};

//////////////////////////////////////////////////////////////////////////
// List all watches op
//////////////////////////////////////////////////////////////////////////
class WatchListOp : public Op
{
public:
    const std::string name() const  { return "watchList"; }
    const std::string summary() const { return "List all watches registered"; }

    Grid::auto_ptr_t on_service(Server& db, const Grid& req)
    {
        boost::ptr_vector<Dict> rows;
        const std::vector<Watch::shared_ptr>& ws = db.watches();
        for (std::vector<Watch::shared_ptr>::const_iterator it = ws.begin(),
            end = ws.end();
            it != end;
        ++it)
        {
            Dict::auto_ptr_t d(new Dict);
            d->add("id", (**it).id());
            d->add("dis", (**it).dis());
            d->add("lease", (**it).lease(), "sec");
            d->add("isOpen", Bool((**it).is_open()));
            rows.push_back(d);
        }

        return Grid::make(rows);
    }
};

//////////////////////////////////////////////////////////////////////////
// PointWriteOp
//////////////////////////////////////////////////////////////////////////
class PointWriteOp : public Op
{
public:
    const std::string name() const { return "pointWrite"; }
    const std::string summary() const { return "Read/write writable point priority array"; }

    Grid::auto_ptr_t on_service(Server& db, const Grid& req)
    {
        // get required point id
        if (req.is_empty()) throw std::runtime_error("Request has no rows");
        const Row& row = req.row(0);
        Val::auto_ptr_t id = val_to_id(db, row.get("id"));

        // check for write
        if (row.has("level"))
        {
            int level = static_cast<int>(row.get_int("level"));
            const std::string& who = row.get_str("who"); // be nice to have user fallback
            const Val& val = row.get("val", false);
            const Num& dur = (Num&)row.get("duration", false);
            db.point_write(id->as<Ref>(), level, val, who, dur);
        }

        return db.point_write_array(id->as<Ref>());
    }
};

//////////////////////////////////////////////////////////////////////////
// HisReadOp
//////////////////////////////////////////////////////////////////////////
class HisReadOp : public Op
{
public:
    const std::string name() const  { return "hisRead"; }
    const std::string summary() const { return "Read time series from historian"; }

    Grid::auto_ptr_t on_service(Server& db, const Grid& req)
    {
        if (req.is_empty()) throw std::runtime_error("Request has no rows");
        const Row& row = req.row(0);
        Val::auto_ptr_t id = val_to_id(db, row.get("id"));

        const std::string& r = row.get_str("range");
        return db.his_read((Ref&)*id, r);
    }
};

//////////////////////////////////////////////////////////////////////////
// HisWriteOp
//////////////////////////////////////////////////////////////////////////
class HisWriteOp : public Op
{
public:
    const std::string name() const  { return "hisWrite"; }
    const std::string summary() const  { return "Write time series data to historian"; }

    Grid::auto_ptr_t on_service(Server& db, const Grid& req)
    {
        if (req.is_empty()) throw std::runtime_error("Request has no rows");
        Val::auto_ptr_t id = val_to_id(db, req.meta().get("id"));

        std::vector<HisItem> items = HisItem::grid_to_items(req);
        db.his_write(id->as<Ref>(), items);

        return Grid::auto_ptr_t();
    }
};

//////////////////////////////////////////////////////////////////////////
// InvokeActionOp
//////////////////////////////////////////////////////////////////////////
class InvokeActionOp : public Op
{
public:
    const std::string name() const { return "invokeAction"; }
    const std::string summary() const { return "Invoke action on target entity"; }

    Grid::auto_ptr_t on_service(Server& db, const Grid& req)
    {
        Val::auto_ptr_t id = val_to_id(db, req.meta().get("id"));

        const std::string& action = req.meta().get_str("action");

        if (req.num_rows() > 0)
            return db.invoke_action(id->as<Ref>(), action, req.row(0));
        else
            return db.invoke_action(id->as<Ref>(), action, Dict::EMPTY);
    }
};

// List the registered operations.
const Op& StdOps::about = AboutOp();
// List the registered grid formats.
const Op& StdOps::formats = FormatsOp();
// Read entity records in database.
const Op& StdOps::read = ReadOp();
// Navigate tree structure of database.
const Op& StdOps::nav = NavOp();
// Watch subscription.
const Op& StdOps::watch_sub = WatchSubOp();
// Watch unsubscription.
const Op& StdOps::watch_unsub = WatchUnsubOp();
// Watch poll cov or refresh.
const Op& StdOps::watch_poll = WatchPollOp();
// List all Watches.
const Op& StdOps::watch_list = WatchListOp();
// Read/write writable point priority array.
const Op& StdOps::point_write = PointWriteOp();
// Read time series history data.
const Op& StdOps::his_read = HisReadOp();
// Write time series history data.
const Op& StdOps::his_write = HisWriteOp();
// Invoke action.
const Op& StdOps::invoke_action = InvokeActionOp();

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
    m_ops_map->insert(std::pair<std::string, const Op* const>(StdOps::watch_sub.name(), &StdOps::watch_sub));
    m_ops_map->insert(std::pair<std::string, const Op* const>(StdOps::watch_unsub.name(), &StdOps::watch_unsub));
    m_ops_map->insert(std::pair<std::string, const Op* const>(StdOps::watch_poll.name(), &StdOps::watch_poll));
    m_ops_map->insert(std::pair<std::string, const Op* const>(StdOps::watch_list.name(), &StdOps::watch_list));
    m_ops_map->insert(std::pair<std::string, const Op* const>(StdOps::point_write.name(), &StdOps::point_write));
    m_ops_map->insert(std::pair<std::string, const Op* const>(StdOps::his_read.name(), &StdOps::his_read));
    m_ops_map->insert(std::pair<std::string, const Op* const>(StdOps::his_write.name(), &StdOps::his_write));
    m_ops_map->insert(std::pair<std::string, const Op* const>(StdOps::invoke_action.name(), &StdOps::invoke_action));
    m_ops_map->insert(std::pair<std::string, const Op* const>(StdOps::ops.name(), &StdOps::ops));

    return *m_ops_map;
}


StdOps::ops_map_t* StdOps::m_ops_map = NULL;
