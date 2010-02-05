#pragma once


#include <fcgixx/session/uuid/generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

namespace runpac { namespace fcgixx { namespace session { namespace uuid {


template<>
struct generator<boost::uuids::uuid>
{
    typedef boost::uuids::uuid uuid_type;

    uuid_type operator()()
    {
        return gen();
    }

    boost::uuids::random_generator gen;
};


} } } } //ns
