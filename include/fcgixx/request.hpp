#pragma once

#include <fastcgi.hpp>

namespace runpac { namespace fcgixx {


struct request
{

    typedef std::map<std::string,std::string> params_type;

    request(FCGIRequest* request);
    ~request();


    FCGIRequest* fcgi_request;

};


} } // ns

