//
// haystack_server.cpp
// 
// Copyright (c) 2015, J2 Innovations
// Author: Radu Racariu <radur@j2inn.com>
// 
// Based on (original notes):
// $Id: //poco/1.4/Net/samples/HTTPTimeServer/src/HTTPTimeServer.cpp#1 $
//
// This sample demonstrates the HTTPTimeServer and related classes.
//
// Copyright (c) 2005-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/AbstractHTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/NumberFormatter.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/URI.h"

#include <iostream>

#include <boost/algorithm/string.hpp>

// haystack includes
#include "testproj.hpp"
#include "op.hpp"


using Poco::Timestamp;
using Poco::DateTimeFormatter;
using Poco::DateTimeFormat;
using Poco::ThreadPool;
using Poco::Net::ServerSocket;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTTPServerParams;
using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;


class HaystackRequestHandler : public HTTPRequestHandler
    /// Return the Zinc encoded grid for the invoked Haystack Op.
{
public:
    HaystackRequestHandler(haystack::TestProj& proj) : _proj(proj)
    {
    }

    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
    {
        Application& app = Application::instance();

        const std::string path = Poco::URI(request.getURI()).getPath();
        app.logger().information("Request: [" + request.getMethod() + "] " + path + " from: " + request.clientAddress().toString());

        if (path == "" || path == "/")
        {
            response.redirect("/about");
            return;
        }

        size_t slash = path.find('/', 1);
        if (slash == path.npos)
            slash = path.size();

        response.setChunkedTransferEncoding(true);
        
        std::string op_name = path.substr(1, slash);

        haystack::Op* op = (haystack::Op*)_proj.op(op_name, false);
        if (op == NULL)
        {
            response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
            response.send();
            return;
        }

        //Timestamp now;
        op->on_service(_proj, request, response);
    }

private:
    haystack::TestProj& _proj;
};

class AuthRequestHandler : public Poco::Net::AbstractHTTPRequestHandler
    /// Handle the /auth request.
    /// A HTTP Basic Auth sample.
{
public:
    AuthRequestHandler(haystack::TestProj& proj) :
        m_proj(proj){}

    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
    {
        AbstractHTTPRequestHandler::handleRequest(request, response);
    }

    void run()
    {
        Application& app = Application::instance();

        std::string path = Poco::URI(request().getURI()).getPath();
        app.logger().information("Request: [" + request().getMethod() + "] " + path + " from: " + request().clientAddress().toString());

        if (path == "/auth" || path == "/auth/")
        {
            response().redirect("/auth/about");
            return;
        }

        path = path.substr(path.find("/auth/") + 6 );

        size_t slash = path.find('/');
        if (slash == path.npos)
            slash = path.size();

        response().setChunkedTransferEncoding(true);
        std::string op_name = path.substr(0, slash);

        haystack::Op* op = (haystack::Op*)m_proj.op(op_name, false);
        if (op == NULL)
        {
            this->sendErrorResponse(Poco::Net::HTTPResponse::HTTP_NOT_FOUND, "Operation '" + op_name + "' not defined");
            return;
        }

        //Timestamp now;
        op->on_service(m_proj, request(), response());
    }

    bool authenticate()
    {
        // basic example
        if (request().hasCredentials())
        {
            std::string scheme;
            std::string authinfo;
            request().getCredentials(scheme, authinfo);
            // u: demo
            // p: demo
            if (scheme == "Basic" && authinfo == "ZGVtbzpkZW1v")
            {
                return true;
            }
            else
            {
                response().requireAuthentication("/auth");
                return false;
            }
        }
        response().requireAuthentication("/auth");
        return false;
    }

private:
    haystack::TestProj& m_proj;
};


class HaystackRequestHandlerFactory : public HTTPRequestHandlerFactory
{
public:
    HaystackRequestHandlerFactory(haystack::TestProj& proj) :
        _proj(proj)
    {
    }

    HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request)
    {
        const std::string path = Poco::URI(request.getURI()).getPath();

        if (boost::starts_with(path, "/auth"))
            return new AuthRequestHandler(_proj);
        else 
            return new HaystackRequestHandler(_proj);
    }

private:
    haystack::TestProj& _proj;
};


class HTTPHaystackServer : public Poco::Util::ServerApplication
    /// The main application class.
    ///
    /// This class handles command-line arguments and
    /// configuration files.
    /// Start the haystack_server executable with the help
    /// option (/help on Windows, --help on Unix) for
    /// the available command line options.
    ///
    /// To use the sample configuration file (HTTPHaystackServer.properties),
    /// copy the file to the directory where the HTTPTimeServer executable
    /// resides. If you start the debug version of the haystack_server
    /// (haystack_serverd[.exe]), you must also create a copy of the configuration
    /// file named haystack_serverd.properties. In the configuration file, you
    /// can specify the port on which the server is listening (default
    /// 8085) and the format of the date/time string sent back to the client.
    ///
    /// To test the haystack_server you can use any web browser (http://localhost:8085/).
{
public:
    HTTPHaystackServer() : _helpRequested(false)
    {
    }

    ~HTTPHaystackServer()
    {
    }

protected:
    void initialize(Application& self)
    {
        loadConfiguration(); // load default configuration files, if present
        ServerApplication::initialize(self);
    }

    void uninitialize()
    {
        ServerApplication::uninitialize();
    }

    void defineOptions(OptionSet& options)
    {
        ServerApplication::defineOptions(options);

        options.addOption(
            Option("help", "h", "display help information on command line arguments")
            .required(false)
            .repeatable(false));
    }

    void handleOption(const std::string& name, const std::string& value)
    {
        ServerApplication::handleOption(name, value);

        if (name == "help")
            _helpRequested = true;
    }

    void displayHelp()
    {
        HelpFormatter helpFormatter(options());
        helpFormatter.setCommand(commandName());
        helpFormatter.setUsage("OPTIONS");
        helpFormatter.setHeader("A sample Haystack Server implementation.");
        helpFormatter.format(std::cout);
    }

    int main(const std::vector<std::string>& args)
    {
        if (_helpRequested)
        {
            displayHelp();
        }
        else
        {
            // get parameters from configuration file
            unsigned short port = (unsigned short)config().getInt("haystack_server.port", 8085);
            int maxQueued = config().getInt("haystack_server.maxQueued", 100);
            int maxThreads = config().getInt("haystack_server.maxThreads", 16);
            ThreadPool::defaultPool().addCapacity(maxThreads);

            HTTPServerParams* pParams = new HTTPServerParams;
            pParams->setMaxQueued(maxQueued);
            pParams->setMaxThreads(maxThreads);
            pParams->setKeepAlive(true);

            // set-up a server socket
            ServerSocket svs(port);
            haystack::TestProj proj;
            // set-up a HTTPServer instance
            HTTPServer srv(new HaystackRequestHandlerFactory(proj), svs, pParams);
            // start the HTTPServer
            srv.start();
            std::cout << "Haystack++ HTTP Demo Server running... \n";
            // wait for CTRL-C or kill
            waitForTerminationRequest();
            // Stop the HTTPServer
            srv.stop();
        }
        return Application::EXIT_OK;
    }

private:
    bool _helpRequested;
};

int main(int argc, char** argv)
{
    HTTPHaystackServer app;
    return app.run(argc, argv);
}
