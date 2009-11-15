#pragma once

#include <string>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <fcgixx/http_response.hpp>


namespace runpac { namespace fcgixx {

template< typename T
        , typename Request
        , typename Response
        , template <typename, typename> class Router
        , template <typename> class Dispatcher
        >
struct application : public Router<T, Request>
                   , public Dispatcher<T>
{


    typedef Request request_type;
    typedef Response response_type;
    typedef typename request_type::params_type params_type;
    typedef Router<T, Request> router;
    typedef Dispatcher<T> dispatcher;

    application()
    {
    }


    virtual ~application()
    {
    }


    void process(typename request_type::raw_type raw_request)
    {
        request.init(raw_request);

        response.clear();

        route();

        if (!dispatch()) { // temp!
            request.write("Content-type: text/html\r\n\r\nunmatched");
        }
        request.end();
    }

    void route()
    {
        typename params_type::const_iterator it = request.params().find("SCRIPT_NAME");
        if (it != request.params().end()) {
            router::route(request, it->second);
        }
    }


    bool dispatch()
    {
       typename params_type::const_iterator it = request.params().find("route"); // tmp
        if (it != request.params().end()) {
            if (dispatcher::dispatch(it->second)) {
                request.write(response.formated_headers()); //.c_str());
                request.write(response.buf.str().c_str(), response.buf.str().size());
                return true;
            }
        }
        return false; //else //??
    }

    request_type request;

    //http_response response;
    response_type response;

};


} } // ns

