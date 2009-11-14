#pragma once

#include "../fastcgi.hpp"

#include <boost/unordered_map.hpp>
#include <boost/noncopyable.hpp>
#include <boost/lexical_cast.hpp>

#include "request.hpp"
#include "http_post_parser.hpp"

namespace runpac { namespace fcgixx {


struct http_request : boost::noncopyable
{
    //friend class application;

    typedef boost::unordered_map<std::string, std::string> params_type;

    http_request(FCGIRequest* fcgi_request = 0)
    : fcgi_request(fcgi_request)
    , post_params_parsed(false)
    {
    }


    ~http_request()
    {
    }


    void reset(FCGIRequest* request)
    {
        fcgi_request = request;
        post_params.clear();
        post_params_parsed = false;
    }


    template<typename T = std::string>
    T get_param(const char* name, const T& def = T())
    {
        if (!fcgi_request) return def;

        typename request::params_type::const_iterator it = fcgi_request->params.find(name);
        if (it != fcgi_request->params.end()) {
            return boost::lexical_cast<T>(it->second);
        }
        return def;
    }

    /*template<typename T = std::string>
    T get_param(const char* name)
    {
        return get_param(name, T());
    }*/


    template<typename T = std::string>
    T get_post_param(const char* name, const T& def = T())
    {
        if (!fcgi_request) return def;

        parse_post_params();

        params_type::const_iterator it = post_params.find(name);
        if (it != post_params.end()) {
            return boost::lexical_cast<T>(it->second);
        }
        return def;
    }

    /*template<typename T = std::string>
    T get_post_param(const char* name)
    {
        return get_post_param(name, T());
    }*/


    const std::map<std::string, std::string>& get_params() //TODO: make it a std::unordered_map
    {
        return fcgi_request->params;
    }


    const params_type& get_post_params()
    {
        parse_post_params();
        return post_params;
    }


    FCGIRequest* get_raw_request()
    {
        return fcgi_request;
    }


private:

    void parse_post_params()
    {
        if (!post_params_parsed) {
            http_post_parser::parse_http_params(fcgi_request->stdin_stream, post_params);
            post_params_parsed = true;
        }
    }


    FCGIRequest* fcgi_request; // utiliser fcgixx::request
    params_type post_params;
    bool post_params_parsed;

};


} } // ns

