#pragma once

#include <string>
#include <ctime>

#include <boost/variant.hpp>
#include <boost/unordered_map.hpp>


namespace runpac { namespace fcgixx { namespace util {


typedef boost::make_recursive_variant
<
     int, double, bool, const char*, std::string, std::time_t // time_t ?
    ,boost::unordered_map<std::string, boost::recursive_variant_> // object
    ,std::vector<boost::recursive_variant_> // array

>::type json_type;

typedef boost::unordered_map<std::string, json_type> json_object;
typedef std::vector<json_type> json_array;


} } } // ns

