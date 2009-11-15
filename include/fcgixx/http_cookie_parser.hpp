#pragma once


#include <boost/unordered_map.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>


namespace runpac { namespace fcgixx {


struct http_cookie_parser
{

    typedef boost::unordered_map<std::string, std::string> params_type;

    static void parse(const std::string& input, params_type& params)
    {
        typedef boost::tokenizer<boost::char_separator<char> > toker;

        boost::char_separator<char> field_sep(";");
        boost::char_separator<char> var_sep("=");

        toker fields(input, field_sep);
        for (toker::iterator i(fields.begin()), e(fields.end()); i != e; ++i) {
            toker vars(*i, var_sep);
            toker::iterator iv = vars.begin();
            if (iv != vars.end()) {
                std::string key(/*url_decode(*/*iv/*)*/);
                boost::trim(key);
                params[key] = (++iv != vars.end()) ? /*url_decode(*/*iv/*)*/ : "";
            }
        }

    }

};

} } // ns
