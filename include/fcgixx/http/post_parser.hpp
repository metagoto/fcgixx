#pragma once

#include <boost/tokenizer.hpp>
#include <fcgixx/util/url_dencode.hpp>


namespace runpac { namespace fcgixx { namespace http {


struct post_parser
{

    template<typename Stream, typename T>
    static void parse(const Stream& input, T& params)
    {
        typedef typename Stream::value_type value_type;

        typedef boost::tokenizer<boost::char_separator<value_type> > toker;

        boost::char_separator<value_type> field_sep("&");
        boost::char_separator<value_type> var_sep("=");

        toker fields(input, field_sep);
        for (typename toker::iterator itf = fields.begin(); itf != fields.end(); ++itf) {
            toker vars(*itf, var_sep);
            typename toker::iterator itv = vars.begin();
            if (itv != vars.end()) {
                std::string key(util::url_decode(*itv));
                params[key] = (++itv != vars.end()) ? util::url_decode(*itv) : "";
            }
        }

    }

};

} } } // ns
