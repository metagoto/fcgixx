#pragma once

#include <boost/lexical_cast.hpp>
#include <fcgixx/http/post_parser.hpp>
#include <fcgixx/http/cookie_parser.hpp>


namespace runpac { namespace fcgixx { namespace request { namespace handler {


template<typename Adapter>
struct simple : public Adapter
{

    typedef Adapter adapter;
    typedef typename Adapter::params_type params_type;
    typedef typename Adapter::stream_type stream_type;


    simple()
    : post_parsed(false)
    , cookie_parsed(false)
    {
    }


    void process()
    {
        post_params.clear();
        cookie_params.clear();
        post_parsed = false;
        cookie_parsed = false;
    }


    template<typename T = std::string>
    T get_param(const char* name, const T& def = T())
    {
        typename params_type::const_iterator it = adapter::params().find(name);
        if (it != adapter::params().end()) {
            return boost::lexical_cast<T>(it->second);
        }
        return def;
    }


    template<typename T = std::string>
    T get_post_param(const char* name, const T& def = T())
    {
        parse_post_params();

        typename params_type::const_iterator it = post_params.find(name);
        if (it != post_params.end()) {
            return boost::lexical_cast<T>(it->second);
        }
        return def;
    }


    template<typename T = std::string>
    T get_cookie(const char* name, const T& def = T())
    {
        parse_cookie_params();

        typename params_type::const_iterator it = cookie_params.find(name);
        if (it != cookie_params.end()) {
            return boost::lexical_cast<T>(it->second);
        }
        return def;
    }


    const params_type& get_params()
    {
        return adapter::params();
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
        if (!post_parsed) {
            http::post_parser::parse(adapter::stdin(), post_params);
            post_parsed = true;
        }
    }

    void parse_cookie_params()
    {
        if (!cookie_parsed) {
            http::cookie_parser::parse(get_param("HTTP_COOKIE"), cookie_params);
            cookie_parsed = true;
        }
    }


    params_type post_params;
    bool post_parsed;

    params_type cookie_params;
    bool cookie_parsed;

};


} } } } // ns

