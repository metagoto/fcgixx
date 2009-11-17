#pragma once

#include <iostream>
#include <sstream>
#include <vector>

#include <boost/unordered_map.hpp>

#include <fcgixx/http/header.hpp>
#include <fcgixx/http/cookie.hpp>


namespace runpac { namespace fcgixx { namespace response {


struct http_response
{

    http_response()
    : buf()
    , os(&buf)
    {
    }

    void clear()
    {
        clear_headers();
        clear_cookies();
        clear_body();
    }

    void clear_headers()
    {
        headers.clear();
    }

    void clear_cookies()
    {
        cookies.clear();
    }

    void clear_body()
    {
        buf.str("");
        os.clear();
    }

    std::string formated_headers()
    {
        std::string buf;
        for (headers_t::const_iterator i(headers.begin()), e(headers.end()); i != e; ++i) {
            buf += i->first + ": " + i->second + "\r\n";
        }
        for (cookies_t::const_iterator i(cookies.begin()), e(cookies.end()); i != e; ++i) {
            buf += "Set-cookie: " + *i + "\r\n";
        }
        return buf + "\r\n";
    }

    std::stringbuf buf;
    std::ostream os;

    typedef boost::unordered_map<std::string, std::string> headers_t;
    typedef std::vector<std::string> cookies_t;
    headers_t headers;
    cookies_t cookies;

};


template<typename T>
inline http_response& operator<< (http_response& res, const T& data)
{
    res.os << data;
    return res;
}

inline http_response& operator<< (http_response& res, const http::header& head)
{
    res.headers[head.name] = head.value;
    return res;
}

inline http_response& operator<< (http_response& res, const http::cookie& cookie)
{
    res.cookies.push_back(cookie.to_string());
    return res;
}


} } } // ns

