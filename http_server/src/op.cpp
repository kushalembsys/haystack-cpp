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

    // route to onService(HServer, HGrid)
    Grid::auto_ptr_t resGrid;
    try
    {
        resGrid = on_service(db, *reqGrid);
    }
    catch (std::runtime_error& e)
    {
        resGrid = Grid::make_err(e);
    }

    // send response
    res.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
    res.setContentType("text/zinc; charset=utf-8");

    std::ostream& ostr = res.send();
    ZincWriter w(ostr);
    w.write_grid(*resGrid);
}

// Service the request and return response.
Grid::auto_ptr_t Op::on_service(const Server& db, const Grid& req)
{
    throw std::runtime_error("Not implemented Op::on_service(const Server& db, const Grid& req)");
    return Grid::auto_ptr_t();
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
    const std::string mime = req.getContentType();
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
        Grid::auto_ptr_t g(new Grid());
        g->addCol("name");
        g->addCol("summary");

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
        Grid::auto_ptr_t g(new Grid());

        g->addCol("mime");
        g->addCol("read");
        g->addCol("write");

        Val* v[3] = {
            new Str("text/zinc"),
            new Marker(),
            new Marker() };
        g->addRow(v, 3);

        return g;
    }
};

// List the registered operations.
const Op& StdOps::about = *new AboutOp();
// List the registered operations.
const Op& StdOps::ops = *new OpsOp();
// List the registered grid formats.
const Op& StdOps::formats = *new FormatsOp();

const StdOps::ops_map_t& StdOps::ops_map()
{
    if (m_ops_map != NULL)
        return *m_ops_map;

    StdOps::m_ops_map = new StdOps::ops_map_t();

    m_ops_map->insert(std::pair<std::string, const Op* const>(StdOps::about.name(), &StdOps::about));
    m_ops_map->insert(std::pair<std::string, const Op* const>(StdOps::formats.name(), &StdOps::formats));
    m_ops_map->insert(std::pair<std::string, const Op* const>(StdOps::ops.name(), &StdOps::ops));

    return *m_ops_map;
}


StdOps::ops_map_t* StdOps::m_ops_map = NULL;
