#pragma once
//
// Copyright (c) 2014, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   19 Sep 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//
#include "proj.hpp"
#include <Poco/URI.h>
#include <Poco/Net/HTTPClientSession.h>

namespace haystack
{
    class Client : public Proj
    {

    public:
        typedef std::auto_ptr<Client> auto_ptr_t;
        //////////////////////////////////////////////////////////////////////////
        // Construction
        //////////////////////////////////////////////////////////////////////////

        // Convenience for construction and call to open().
        static Client::auto_ptr_t open(const std::string& uri, const std::string& user, const std::string& pass);

        // Constructor with URI to server's API and authentication credentials.
        Client(const std::string& uri, const std::string& user, const std::string& pass);

        //////////////////////////////////////////////////////////////////////////
        // Operations
        //////////////////////////////////////////////////////////////////////////

        // Authenticate the client and return this.
        Client& open();

        Dict::auto_ptr_t about() const;
        Grid::auto_ptr_t call(const std::string& op, const Grid& req) const;

        /*
        Dict::auto_ptr_t read_by_id(const Ref&) const;
        Dict::auto_ptr_t read_by_id(const Ref&, bool)const;
        Grid::auto_ptr_t read_by_ids(const std::vector<Ref>&)const;
        Grid::auto_ptr_t read_by_ids(const std::vector<Ref>&, bool)const;
        Dict::auto_ptr_t read(const std::string&)const;
        Dict::auto_ptr_t read(const std::string&, bool)const;
        Grid::auto_ptr_t read_all(const std::string&)const;
        Grid::auto_ptr_t read_all(const std::string&, unsigned int)const;*/

    private:

        Poco::Net::HTTPClientSession&  session() const { return *m_session; }
        void authenticate() const;
        Grid::auto_ptr_t post_grid(const std::string& op, const Grid& req) const;
        const std::string post_string(const std::string& op, const std::string& req);


        // stubs
        Dict::auto_ptr_t on_read_by_id(const haystack::Ref &) const;
        Grid::auto_ptr_t on_read_by_ids(const std::vector<haystack::Ref> &) const;
        Grid::auto_ptr_t on_read_all(const std::string &, size_t) const;

        //////////////////////////////////////////////////////////////////////////
        // Fields
        //////////////////////////////////////////////////////////////////////////
        const Poco::URI  m_uri;
        const std::string m_user;
        const std::string m_pass;
        std::string m_auth;
        std::auto_ptr<Poco::Net::HTTPClientSession> m_session;
    };
}