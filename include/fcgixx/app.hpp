#pragma once

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>


namespace runpac { namespace fcgixx {

template< typename T
        , typename Request
        , typename Response
        , template <typename, typename> class Router
        , template <typename> class Dispatcher
        >
struct app : public Router<T, Request>
           , public Dispatcher<T>
           , boost::noncopyable
{


    typedef Request request_type;
    typedef Response response_type;
    typedef typename request_type::params_type params_type;
    typedef Router<T, Request> router;
    typedef Dispatcher<T> dispatcher;


    void process(typename request_type::raw_type raw_request)
    {
        request.process(raw_request);
        response.clear();
        route();
        dispatch();
        //if (!dispatch()) { // temp!
        //    request.write("Content-type: text/html\r\n\r\nunmatched");
        //}
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

    response_type response;

};


} } // ns

