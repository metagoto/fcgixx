#pragma once

#include <string>


namespace runpac { namespace fcgixx { namespace http {

struct header
{
    header(const char* name, const char* value)
    : name(name), value(value)
    {
    }

    std::string name;
    std::string value;
};


} } } // ns


