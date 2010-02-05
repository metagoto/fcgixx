#pragma once

#include <ctime>
#include <fcgixx/session/uuid/generator.hpp>


namespace runpac { namespace fcgixx { namespace session { namespace uuid {


template<>
struct generator<std::time_t>
{
    typedef std::time_t uuid_type;

    uuid_type operator()()
    {
        return std::time(0);
    }
};


} } } } //ns
