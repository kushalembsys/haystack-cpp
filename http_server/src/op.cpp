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
#include "zincreader.hpp"
#include "zincwriter.hpp"
#include "server.hpp"

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
        w.write_grid(*on_service(db, *reqGrid));
        reset();
    }
    catch (std::runtime_error& e)
    {
        w.write_grid(*Grid::make_err(e));
    }
}

// Service the request and return response.
Grid::auto_ptr_t Op::on_service(const Server& db, const Grid& req)
{
    throw std::runtime_error("Not implemented Op::on_service(const Server& db, const Grid& req)");
    return  Grid::auto_ptr_t();
}

// reset current Op state
void Op::reset()
{

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
        const std::string& valStr = it->second;
        Val::auto_ptr_t val;
        try
        {
            val = ZincReader::make(valStr)->read_scalar();
        }
        catch (std::exception&)
        {
            val = Str(valStr).clone();
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

    Grid::auto_ptr_t on_service(const Server& db, const Grid& req)
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

    Grid::auto_ptr_t on_service(const Server& db, const Grid& req)
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

    Grid::auto_ptr_t on_service(const Server& db, const Grid& req)
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

    Grid::auto_ptr_t on_service(const Server& db, const Grid& req)
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

// List the registered operations.
const Op& StdOps::about = *new AboutOp();
// List the registered grid formats.
const Op& StdOps::formats = *new FormatsOp();
// Read entity records in database.
const Op& StdOps::read = *new ReadOp();

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
    m_ops_map->insert(std::pair<std::string, const Op* const>(StdOps::ops.name(), &StdOps::ops));

    return *m_ops_map;
}


StdOps::ops_map_t* StdOps::m_ops_map = NULL;
