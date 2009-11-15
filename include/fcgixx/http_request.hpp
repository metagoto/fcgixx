#pragma once

#include "../fastcgi.hpp"

#include <boost/unordered_map.hpp>
#include <boost/noncopyable.hpp>
#include <boost/lexical_cast.hpp>

#include "request_base.hpp"
#include "http_post_parser.hpp"
#include "http_cookie_parser.hpp"

namespace runpac { namespace fcgixx {


struct http_request : public request_base
                    , private boost::noncopyable
{

    typedef request_base::raw_type raw_request;
    typedef request_base::params_type params_type;


    http_request()
    : post_params_parsed(false)
    , cookie_params_parsed(false)
    {
    }


    ~http_request()
    {
    }


    void init(request_base::raw_type raw_request)
    {
        request_base::init(raw_request);
        post_params.clear();
        cookie_params.clear();
        post_params_parsed = false;
        cookie_params_parsed = false;
    }


    template<typename T = std::string>
    T get_param(const char* name, const T& def = T())
    {
        params_type::const_iterator it = params().find(name);
        if (it != params().end()) {
            return boost::lexical_cast<T>(it->second);
        }
        return def;
    }


    template<typename T = std::string>
    T get_post_param(const char* name, const T& def = T())
    {
        parse_post_params();

        params_type::const_iterator it = post_params.find(name);
        if (it != post_params.end()) {
            return boost::lexical_cast<T>(it->second);
        }
        return def;
    }


    template<typename T = std::string>
    T get_cookie(const char* name, const T& def = T())
    {
        parse_cookie_params();

        params_type::const_iterator it = cookie_params.find(name);
        if (it != cookie_params.end()) {
            return boost::lexical_cast<T>(it->second);
        }
        return def;
    }


    const params_type& get_params()
    {
        return params();
    }


    const params_type& get_post_params()
    {
        parse_post_params();
        return post_params;
    }

    const params_type& get_cookie_params()
    {
        parse_cookie_params();
        return cookie_params;
    }



private:

    void parse_post_params()
    {
        if (!post_params_parsed) {
            http_post_parser::parse(stdin(), post_params);
            post_params_parsed = true;
        }
    }

    void parse_cookie_params()
    {
        if (!cookie_params_parsed) {
            http_cookie_parser::parse(get_param("HTTP_COOKIE"), cookie_params);
            cookie_params_parsed = true;
        }
    }


    params_type post_params;
    bool post_params_parsed;

    params_type cookie_params;
    bool cookie_params_parsed;

};


} } // ns

