//
// Copyright (c) 2014, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   19 Sep 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

#include "client.hpp"

#include <string>
#include <sstream>
#include <iostream>

#include <Poco/Base64Encoder.h>
#include <Poco/Exception.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Path.h>
#include <Poco/URI.h>

#include "zincreader.hpp"
#include "zincwriter.hpp"

using namespace haystack;
using namespace Poco;
using namespace Poco::Net;

Client::Client(const std::string& uri, const std::string& user, const std::string& pass) :
m_uri(uri),
m_user(user),
m_pass(pass)
{
    // Initialize session
    m_session.reset(new HTTPClientSession(m_uri.getHost(), m_uri.getPort()));
    session().setKeepAlive(true);
    std::stringstream ss;
    Poco::Base64Encoder enc(ss);
    enc << m_user << ':' << m_pass;
    m_auth = ss.str();
}

Client::auto_ptr_t Client::open(const std::string& uri, const std::string& user, const std::string& pass)
{
    Client::auto_ptr_t c(new Client(uri, user, pass));
    c->open();
    return c;
}

Client& Client::open()
{
    authenticate();
    return *this;
}

void Client::authenticate() const
{
    HTTPRequest req(HTTPRequest::HTTP_GET, m_uri.getPathAndQuery(), HTTPMessage::HTTP_1_1);
    req.setKeepAlive(true);
    req.setCredentials("Basic", m_auth);

    session().sendRequest(req);

    HTTPResponse res;
    session().receiveResponse(res);

    if (res.getStatus() != HTTPResponse::HTTP_OK && res.getStatus() != HTTPResponse::HTTP_FOUND)
        throw std::runtime_error("Authentication failed");
}

Dict::auto_ptr_t Client::about() const
{
    Grid::auto_ptr_t g = call("about", Grid::EMPTY);
    const Row& r = g->row(0);
    Dict::auto_ptr_t d(new Dict);

    const size_t n_cols = g->num_cols();

    for (size_t i = 0; i < n_cols; i++)
    {
        const Col& c = g->col(i);
        d->add(c.name(), r.get(c));
    }

    return d;
}

Grid::auto_ptr_t Client::call(const std::string& op, const Grid& req) const
{
    Grid::auto_ptr_t res = post_grid(op, req);
    if (res->is_err()) throw std::runtime_error("Call error: " + res->meta().get_str("dis"));
    return res;
}

// impl

Grid::auto_ptr_t Client::post_grid(const std::string& op, const Grid& req) const
{
    const std::string& reqStr = ZincWriter::grid_to_string(req);
    const std::string& resStr = const_cast<Client&>(*this).post_string(op, reqStr);
    return ZincReader::make(resStr)->read_grid();
}

const std::string Client::post_string(const std::string& op, const std::string& req)
{
    HTTPClientSession session(m_uri.getHost(), m_uri.getPort());
    HTTPRequest post(HTTPRequest::HTTP_POST, m_uri.getPathAndQuery() + op, HTTPMessage::HTTP_1_1);
    post.setKeepAlive(true);

    post.setCredentials("Basic", m_auth);
    post.setContentLength(req.length());
    post.setContentType("text/zinc; charset=utf-8");
    session.sendRequest(post) << req;
    
    HTTPResponse res;
    std::stringstream ss;
    std::istream& rs = session.receiveResponse(res);
    if (res.getStatus() != HTTPResponse::HTTP_OK && res.getStatus() != HTTPResponse::HTTP_FOUND)
        throw std::runtime_error("POST failed");

    std::istreambuf_iterator<char> eos;
    std::string result(std::istreambuf_iterator<char>(rs), eos);
    return result;
}


// stubs

Dict::auto_ptr_t Client::on_read_by_id(const haystack::Ref &) const
{
    return Dict::auto_ptr_t();
}

Grid::auto_ptr_t Client::on_read_by_ids(const std::vector<haystack::Ref> &) const
{
    return Grid::auto_ptr_t();
}

Grid::auto_ptr_t Client::on_read_all(const std::string &, size_t) const
{
    return Grid::auto_ptr_t();
}


// stubs from Proj

Dict::auto_ptr_t  Proj::read_by_id(const Ref&)const
{
    return Dict::auto_ptr_t();
}
Dict::auto_ptr_t  Proj::read_by_id(const Ref&, bool)const
{
    return Dict::auto_ptr_t();
}
Grid::auto_ptr_t  Proj::read_by_ids(const std::vector<Ref>&)const
{
    return Grid::auto_ptr_t();
}
Grid::auto_ptr_t  Proj::read_by_ids(const std::vector<Ref>&, bool)const
{
    return Grid::auto_ptr_t();
}
Dict::auto_ptr_t  Proj::read(const std::string&)const
{
    return Dict::auto_ptr_t();
}
Dict::auto_ptr_t  Proj::read(const std::string&, bool)const
{
    return Dict::auto_ptr_t();
}
Grid::auto_ptr_t  Proj::read_all(const std::string&) const
{
    return Grid::auto_ptr_t();
}
Grid::auto_ptr_t  Proj::read_all(const std::string&, size_t) const
{
    return Grid::auto_ptr_t();
}

/// stuff

int main(int argc, char*argv[])
{
    try
    {
        Client c("http://localhost:8085/auth/", "demo", "demo");

        std::cout << c.open().about()->get_str("serverName") << "\n";
    }
    catch (std::exception& e)
    {
        std::cout << "Error: " << e.what() << "\n";
    }
}