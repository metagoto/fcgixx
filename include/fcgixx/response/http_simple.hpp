#pragma once

#include <fcgixx/response/basic_response.hpp>
#include <fcgixx/response/handler/simple.hpp>


namespace runpac { namespace fcgixx { namespace response {


typedef basic_response<handler::simple> http_simple;


} } } // ns
