#pragma once

#include <string>
#include <map>

#include <boost/function.hpp>
#include <boost/bind.hpp>

#include "http_request.hpp"
#include "http_response.hpp"


namespace runpac { namespace fcgixx {

template< typename T
        , template <typename> class Router
        , template <typename> class Dispatcher
        >
struct application : public Router<T>
                   , public Dispatcher<T>
{

    typedef Router<T>     router;
    typedef Dispatcher<T> dispatcher;

    application()
    {
    }


    virtual ~application()
    {
    }


    void process(FCGIRequest* fcgi_request)
    {
        request.reset(fcgi_request);

        response.clear();

        route();

        if (!dispatch()) { // temp!
            fcgi_request->write("Content-type: text/html\r\n\r\nunmatched");
        }
        fcgi_request->end_request(0, FCGIRequest::REQUEST_COMPLETE);
    }


    void route()
    {
        FCGIRequest* fcgi_request = request.get_raw_request();

        params_t::const_iterator it = fcgi_request->params.find("SCRIPT_NAME");
        if (it != fcgi_request->params.end()) {
            router::route(fcgi_request, it->second);
        }
    }


    bool dispatch()
    {
        FCGIRequest* fcgi_request = request.get_raw_request();

        params_t::const_iterator it = fcgi_request->params.find("route");
        if (it != fcgi_request->params.end()) {
            if (dispatcher::dispatch(it->second)) {
                fcgi_request->write(response.formated_headers()); // inclu le "\r\n"
                fcgi_request->write(response.buf.str().c_str(), response.buf.str().size());
                return true;
            }
        }
        return false; //else //??
    }

    http_request request;

    http_response response;

    typedef std::map<std::string, std::string> params_t;


};


} } // ns

