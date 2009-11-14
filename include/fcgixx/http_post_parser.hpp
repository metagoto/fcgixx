#pragma once

//TODO: credit original contributor

#include <boost/unordered_map.hpp>
#include <boost/tokenizer.hpp>


namespace runpac { namespace fcgixx {


struct http_post_parser
{

    static char hex_to_char(char const& c1, char const& c2)
    {
        int ret ((std::isalpha(c1)
                    ? ((c1 & 0xdf) - 'A') + 10
                    : (c1 - '0')
                 ) << 4
                );

        ret +=  (std::isalpha(c2)
                    ? ((c2 & 0xdf) - 'A') + 10
                    : (c2 - '0')
                );

         return static_cast<char>(ret);
    }


    static std::string url_decode( const std::string& str )
    {
        typedef std::string string_type;
        string_type result;

        for( string_type::const_iterator iter (str.begin()), end (str.end())
           ; iter != end; ++iter )
        {
            switch( *iter )
            {
                case ' ':
                    break;
                case '+':
                    result.append(1, ' ');
                    break;
                case '%':
                    if (std::distance(iter, end) <= 2
                        || !std::isxdigit(*(iter+1))
                        || !std::isxdigit(*(iter+2)))
                    {
                        result.append(1, '%');
                    }
                    else {
                        char ch = *++iter;
                        result.append(1, hex_to_char(ch, *++iter));
                    }
                    break;
                default:
                    result.append(1, *iter);
            }
        }

        return result;
    }


    static void parse_http_params(const std::string& input
                                 ,boost::unordered_map<std::string, std::string>& params)
    {
        typedef boost::tokenizer<boost::char_separator<char> > toker;

        boost::char_separator<char> field_sep("&");
        boost::char_separator<char> var_sep("=");

        toker fields(input, field_sep);
        for (/*typename */toker::iterator itf = fields.begin(); itf != fields.end(); ++itf) {
            toker vars(*itf, var_sep);
            /*typename*/ toker::iterator itv = vars.begin();
            if (itv != vars.end()) {
                std::string key(url_decode(*itv));
                params[key] = (++itv != vars.end()) ? url_decode(*itv) : "";
            }
        }

    }

};

} } // ns
