#pragma once

#include <iostream>
#include <sstream>

#include <boost/unordered_map.hpp>

#include "header.hpp"


namespace runpac { namespace fcgixx {


struct http_response
{

    http_response()
    : buf("")
    , os(&buf)
    {
    }

    void clear()
    {
        clear_headers();
        clear_body();
    }

    void clear_headers()
    {
        headers.clear();
    }

    void clear_body()
    {
        buf.str("");
        os.clear();
    }

    std::string formated_headers()
    {
        std::string buf;
        headers_t::const_iterator it = headers.begin();
        for ( ; it != headers.end(); ++it) {
            buf += it->first + ": " + it->second + "\r\n";
        }
        return buf + "\r\n";
    }

    std::stringbuf buf;
    std::ostream os;

    typedef boost::unordered_map<std::string, std::string> headers_t;
    headers_t headers;

};


template<typename T>
inline http_response& operator<< (http_response& res, const T& data)
{
    res.os << data;
    return res;
}

inline http_response& operator<< (http_response& res, const header& head)
{
    res.headers[head.name] = head.value;
    return res;
}



} } // ns

