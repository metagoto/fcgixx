#pragma once

#include <string>

namespace runpac { namespace fcgixx {

static const char* past_date = "Wednesday, 09-Nov-1999 23:12:40 GMT";

struct cookie
{

    cookie(const char* name, const char* value, const char* expires = "")
        : name(name)
        , value(value)
        , expires(expires)
    {
    }

    cookie(const char* name)
        : name(name)
        , value()
        , expires(past_date)
    {
    }

    std::string to_string() const
    {
        std::string str(name + "=" + value);
        if (!expires.empty()) str += ("; expires=" + expires);
        //if (!path.empty()   ) str += ("; path="    + path);
        //if (!domain.empty() ) str += ("; domain="  + domain);
        //if ( secure         ) str += "; secure";
        //if ( http_only      ) str += "; HttpOnly";
        return str;
    }

    std::string name;
    std::string value;
    std::string expires;


};




} } //ns

