//
// Copyright (c) 2015, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   19 Sep 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

#include "client.hpp"

#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>

#include <boost/algorithm/string.hpp>

#include <Poco/Base64Encoder.h>
#include <Poco/Exception.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Path.h>
#include <Poco/URI.h>

#include "num.hpp"
#include "zincreader.hpp"
#include "zincwriter.hpp"

using namespace haystack;
using namespace Poco;
using namespace Poco::Net;

Client::Client(const std::string& uri, const std::string& user, const std::string& pass) :
m_uri(boost::ends_with(uri, "/") ? uri : uri + "/"),
m_user(user),
m_pass(pass)
{
    // check uri
    if (!boost::starts_with(uri, "http://") && !boost::starts_with(uri, "https://"))
        throw std::runtime_error("Invalid uri format: " + uri);

    // sanity check arguments
    if (user.empty()) throw std::runtime_error("user cannot be empty string");

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

    return g->row(0).to_dict();
}

Grid::auto_ptr_t Client::ops()
{
    return call("ops", Grid::EMPTY);
}

Grid::auto_ptr_t Client::formats()
{
    return call("formats", Grid::EMPTY);
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

Dict::auto_ptr_t Client::on_read_by_id(const haystack::Ref &r) const
{
    boost::ptr_vector<Ref> v;
    v.push_back(new_clone(r));
    Grid::auto_ptr_t res = read_by_ids(v, false);
    if (res.get() == NULL || res->is_empty()) return  Dict::auto_ptr_t();
    const Row& rec = res->row(0);
    if (rec.missing("id")) return Dict::auto_ptr_t();
    return rec.to_dict();
}

Grid::auto_ptr_t Client::on_read_by_ids(const boost::ptr_vector<Ref> &r) const
{
    Grid req;
    req.add_col("id");
    for (boost::ptr_vector<Ref>::const_iterator it = r.begin(); it != r.end(); ++it)
    {
        Val* v[1] = { new_clone(*it) };
        req.add_row(v, 1);
    }

    return call("read", req);
}

Grid::auto_ptr_t Client::on_read_all(const std::string & filter, size_t limit) const
{
    Grid req;
    req.add_col("filter");
    req.add_col("limit");
    Val* v[2] = { new Str(filter), new Num((long long)limit) };
    req.add_row(v, 2);
    return call("read", req);
}


// stubs from Proj

Dict::auto_ptr_t  Proj::read_by_id(const Ref& id)const
{
    return read_by_id(id, true);
}
Dict::auto_ptr_t  Proj::read_by_id(const Ref& id, bool checked)const
{
    Dict::auto_ptr_t rec = on_read_by_id(id);
    if (rec.get() != NULL) return rec;
    if (checked) throw std::runtime_error("Unknown id: " + id.to_string());
    return rec;
}
Grid::auto_ptr_t  Proj::read_by_ids(const boost::ptr_vector<Ref>& ids)const
{
    return read_by_ids(ids, true);
}
Grid::auto_ptr_t  Proj::read_by_ids(const boost::ptr_vector<Ref>& ids, bool checked)const
{
    Grid::auto_ptr_t grid = on_read_by_ids(ids);
    if (checked)
    {
        for (Grid::const_iterator it = grid->begin(), end = grid->end(); it != end; ++it)
            if (it->missing("id")) throw std::runtime_error("Unknown id: " + it->to_string());
    }
    return grid;
}

Dict::auto_ptr_t  Proj::read(const std::string& filter) const
{
    return read(filter, true);
}
Dict::auto_ptr_t  Proj::read(const std::string& filter, bool checked) const
{
    Grid::auto_ptr_t grid = read_all(filter, 1);
    if (grid->num_rows() > 0) return grid->row(0).to_dict();
    if (checked) throw std::runtime_error(filter);
    return Dict::auto_ptr_t();
}
#undef max
Grid::auto_ptr_t  Proj::read_all(const std::string& filter) const
{
    return read_all(filter, (size_t)-1);
}
Grid::auto_ptr_t  Proj::read_all(const std::string& filter, size_t limit) const
{
    return on_read_all(filter, limit);
}

/// stuff

int main(int argc, char*argv[])
{
    try
    {
        Client c("http://localhost:8085/auth/", "demo", "demo");

        std::cout << c.open().about()->get_str("serverName") << "\n";

        std::cout << ZincWriter::grid_to_string(*c.ops()) << "\n";

        std::cout << ZincWriter::grid_to_string(*c.formats()) << "\n";

        std::cout << c.read_by_id(Ref("A"))->to_string() << "\n";

        std::cout << c.read("site")->to_string() << "\n";

    }
    catch (std::exception& e)
    {
        std::cout << "Error: " << e.what() << "\n";
    }
}