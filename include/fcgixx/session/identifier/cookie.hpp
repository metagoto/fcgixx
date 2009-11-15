#pragma once

#include <boost/lexical_cast.hpp>


namespace runpac { namespace fcgixx { namespace session { namespace identifier {


static const char* ident = "uuid"; // tmp

template<typename Request, typename Response>
struct cookie
{

    typedef Request request_type;
    typedef Response response_type;

    cookie(request_type& request, response_type& response)
        : request(request)
        , response(response)
    {
    }


    template<typename T = std::string>
    bool has_id(T& id)
    {
        const std::string& uuid = request.get_cookie(ident);
        if (!uuid.empty()) {
            id = boost::lexical_cast<T>(uuid);
            return true;
        }
        return false;
    }


    template<typename T = std::string>
    T new_id()
    {
        const std::string& new_uuid = boost::lexical_cast<std::string>(time(0)); /// tmp!! need proper uuid
        response << fcgixx::cookie(ident, new_uuid.c_str());
        return boost::lexical_cast<T>(new_uuid);
    }


private:
    request_type& request;
    response_type& response;


};

} } } } //ns
