#pragma once


#include <fcgixx/http/cookie.hpp>
#include <fcgixx/session/uuid/generator.hpp>
#include <fcgixx/session/uuid/converter.hpp>


namespace runpac { namespace fcgixx { namespace session { namespace identifier {


static const char* ident = "uuid";



template<typename Request
        ,typename Response
        ,typename Id>
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
        using fcgixx::session::uuid::to_uuid;

        const std::string& uuid = request.get_cookie(ident);
        if (!uuid.empty()) {
            id = to_uuid<id_type, std::string>(uuid);
            return true;
        }
        return false;
    }


    id_type new_id()
    {
        using fcgixx::session::uuid::from_uuid;

        const id_type& id = id_generator();
        const std::string& id_str =
            from_uuid<id_type, std::string>(id);
        response << http::cookie(ident, id_str.c_str());
        return id;
    }

    void delete_id(const id_type& id)
    {
        response << http::cookie(ident);
    }


private:
    request_type& request;
    response_type& response;

    fcgixx::session::uuid::generator<id_type> id_generator;


};



} } } } //ns
