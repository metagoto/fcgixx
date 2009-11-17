#pragma once

#include <fcgixx/request/basic_request.hpp>
#include <fcgixx/request/adapter/fcgi.hpp>
#include <fcgixx/request/handler/simple.hpp>


namespace runpac { namespace fcgixx { namespace request {


typedef basic_request<adapter::fcgi, handler::simple> http_simple;


} } } // ns
