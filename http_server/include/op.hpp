#pragma once
#include "headers.hpp"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "grid.hpp"

//
// Copyright (c) 2014, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   08 Sep 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;

namespace haystack
{
    class Server;
    class Op : boost::noncopyable
    {
    public:

        Op() : m_grid(new Grid){}

        // Programatic name of the operation.
        virtual std::string name() const = 0;

        // Short one line summary of what the operation does.
        virtual std::string summary() const = 0;

        // Service the request and return response.
        // This method routes to "onService(HServer,HGrid)".
        void on_service(const Server& db, HTTPServerRequest& req, HTTPServerResponse& res);

        // Service the request and return response.
        virtual Grid::auto_ptr_t on_service(const Server& db, const Grid& req);

        // reset current Op state
        virtual void reset();

    private:
        // Map the GET query parameters to grid with one row
        Grid::auto_ptr_t  get_to_grid(HTTPServerRequest& req);

        // Map the POST body to grid
        Grid::auto_ptr_t post_to_grid(HTTPServerRequest& req, HTTPServerResponse& res);

        Grid::auto_ptr_t m_grid;
    };

    class StdOps
    {
    public:
        // List the registered operations.
        static const Op& about;
        // List the registered operations.
        static const Op& ops;
        // List the registered grid formats.
        static const Op& formats;
        // Read entity records in database.
        static const Op& read;

        typedef std::map<std::string, const Op* const> ops_map_t;
        static const ops_map_t& ops_map();
    private:
        static ops_map_t* m_ops_map;
    };
}