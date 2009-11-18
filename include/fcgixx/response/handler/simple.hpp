#pragma once


#include <string>
#include <boost/unordered_map.hpp>
#include <fcgixx/http/header.hpp>
#include <fcgixx/http/cookie.hpp>

namespace runpac { namespace fcgixx { namespace response { namespace handler {


struct simple
{

    simple()
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
inline simple& operator<< (simple& res, const T& data)
{
    res.os << data;
    return res;
}

inline simple& operator<< (simple& res, const http::header& head)
{
    res.headers[head.name] = head.value;
    return res;
}

inline simple& operator<< (simple& res, const http::cookie& cookie)
{
    res.cookies.push_back(cookie.to_string());
    return res;
}


} } } } // ns
