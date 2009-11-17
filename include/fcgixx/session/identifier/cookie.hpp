#pragma once

#include <boost/lexical_cast.hpp>
#include <fcgixx/http/cookie.hpp>


namespace runpac { namespace fcgixx { namespace session { namespace identifier {


static const char* ident = "uuid"; // tmp

template<typename Request
        ,typename Response
        ,typename Id = std::string>
struct cookie
{

    typedef Request request_type;
    typedef Response response_type;

    typedef Id id_type;


    cookie(request_type& request, response_type& response)
        : request(request)
        , response(response)
    {
    }


    bool has_id(id_type& id)
    {
        const std::string& uuid = request.get_cookie(ident);
        if (!uuid.empty()) {
            id = boost::lexical_cast<id_type>(uuid);
            return true;
        }
        return false;
    }


    id_type new_id()
    {
        const std::string& new_uuid =
            boost::lexical_cast<std::string>(time(0)); /// tmp!! need proper uuid
        response << http::cookie(ident, new_uuid.c_str());
        return boost::lexical_cast<id_type>(new_uuid);
    }


private:
    request_type& request;
    response_type& response;


};


} } } } //ns
