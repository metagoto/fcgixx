#pragma once

#include <string>

namespace runpac { namespace fcgixx { namespace router {

template<typename Host, typename Request>
struct nothing
{
    typedef Request request_type;

    void route(request_type& request, const std::string& path_info)
    {
        request.params()["route"] = path_info;
    }

};


} } } // ns
