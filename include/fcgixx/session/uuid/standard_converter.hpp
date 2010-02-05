#pragma once


#include <fcgixx/session/uuid/converter.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>


namespace runpac { namespace fcgixx { namespace session { namespace uuid {


template<>
inline boost::uuids::uuid to_uuid<boost::uuids::uuid, std::string>(const std::string& cookie)
{
    boost::uuids::string_generator gen;
    return gen(cookie);
}


template<>
inline std::string from_uuid<boost::uuids::uuid, std::string>(const boost::uuids::uuid& uuid)
{
    std::stringstream ss;
    ss << uuid;
    return ss.str();
}


} } } } //ns
