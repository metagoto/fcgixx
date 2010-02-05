#pragma once

#include <fcgixx/session/basic_session.hpp>
#include <fcgixx/session/identifier/cookie.hpp>
#include <fcgixx/session/storage/memory.hpp>

//#include <boost/uuid/uuid.hpp>
#include <fcgixx/session/uuid/standard_generator.hpp>
#include <fcgixx/session/uuid/standard_converter.hpp>


namespace runpac { namespace fcgixx { namespace session {

/* // waiting for c++0x template aliases
template<typename Request, typename Response>
using mem_cook =
        basic_session<
            identifier::cookie<Request, Response>
           ,storage::memory
        >;
*/

template<typename Request, typename Response>
struct mem_cook
{

    typedef basic_session<
            identifier::cookie<Request, Response, boost::uuids::uuid>
               ,storage::memory
            > type;
};


} } } // ns
