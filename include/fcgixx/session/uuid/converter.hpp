#pragma once


#include <boost/lexical_cast.hpp>


namespace runpac { namespace fcgixx { namespace session { namespace uuid {


template<typename Uuid, typename Cookie>
inline Uuid to_uuid(const Cookie& cookie)
{
    return boost::lexical_cast<Uuid>(cookie);
}


template<typename Uuid, typename Cookie>
inline Cookie from_uuid(const Uuid& uuid)
{
    return boost::lexical_cast<Cookie>(uuid);
}


} } } } //ns
