#pragma once
//
// Copyright (c) 2015, J2 Innovations
// Copyright (c) 2012 Brian Frank
// Licensed under the Academic Free License version 3.0
// History:
//   08 Sep 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

#include "headers.hpp"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "grid.hpp"
#include "ref.hpp"

using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;

namespace haystack
{
    class Server;
    class Uri;
    class Ref;
    class Val;

    /**
    Op is the base class for server side operations exposed by the REST API.
    All methods on Op must be thread safe.
    @see <a href = 'http://project-haystack.org/doc/Ops'>Project Haystack< / a>
    */
    class Op : boost::noncopyable
    {
    public:
        Op() {}
        /**
        Programatic name of the operation.
        */
        virtual const std::string name() const = 0;

        /**
        Short one line summary of what the operation does.
        */
        virtual const std::string summary() const = 0;

        /**
        Service the request and return response.
        This method routes to "on_service(Server& db, const Grid& req)"
        */
        void on_service(Server& db, HTTPServerRequest& req, HTTPServerResponse& res);

        /**
        Service the request and return response.
        */
        virtual Grid::auto_ptr_t on_service(Server& db, const Grid& req);

    protected:
        typedef boost::ptr_vector<Ref> refs_t;
        refs_t grid_to_ids(const Server& db, const Grid& grid) const;

        Val::auto_ptr_t val_to_id(const Server& db, const Val& val) const;

    private:
        // Map the GET query parameters to grid with one row
        Grid::auto_ptr_t  get_to_grid(HTTPServerRequest& req);

        // Map the POST body to grid
        Grid::auto_ptr_t post_to_grid(HTTPServerRequest& req, HTTPServerResponse& res);
    };

    class StdOps
    {
    public:
        /**
        List the registered operations.
        */
        static const Op& about;
        /**
        List the registered operations.
        */
        static const Op& ops;
        /**
        List the registered grid formats.
        */
        static const Op& formats;
        /**
        Read entity records in database.
        */
        static const Op& read;
        /**
        Navigate tree structure of database.
        */
        static const Op& nav;
        /**
        Watch subscription.
        */
        static const Op& watch_sub;
        /**
        Watch unsubscription.
        */
        static const Op& watch_unsub;
        /**
        Watch poll cov or refresh.
        */
        static const Op& watch_poll;
        /**
        List all Watches.
        */
        static const Op& watch_list;
        /**
        Read/write writable point priority array.
        */
        static const Op& point_write;
        /**
        Read time series history data.
        */
        static const Op& his_read;
        /**
        Write time series history data.
        */
        static const Op& his_write;
        /**
        Invoke action.
        */
        static const Op& invoke_action;

        typedef std::map<std::string, const Op* const> ops_map_t;
        static const ops_map_t& ops_map();
    private:
        static ops_map_t* m_ops_map;
    };
}