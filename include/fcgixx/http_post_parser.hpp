#pragma once


#include <boost/unordered_map.hpp>
#include <boost/tokenizer.hpp>
#include "url_dencode.hpp"

namespace runpac { namespace fcgixx {


struct http_post_parser
{

    template<typename T>
    static void parse(const std::string& input, T& params)
    {
        typedef boost::tokenizer<boost::char_separator<char> > toker;

        boost::char_separator<char> field_sep("&");
        boost::char_separator<char> var_sep("=");

        toker fields(input, field_sep);
        for (toker::iterator itf = fields.begin(); itf != fields.end(); ++itf) {
            toker vars(*itf, var_sep);
            toker::iterator itv = vars.begin();
            if (itv != vars.end()) {
                std::string key(url_decode(*itv));
                params[key] = (++itv != vars.end()) ? url_decode(*itv) : "";
            }
        }

    }

};

} } // ns
