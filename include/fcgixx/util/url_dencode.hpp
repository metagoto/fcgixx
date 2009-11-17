#pragma once

#include <boost/unordered_map.hpp>
#include <boost/tokenizer.hpp>


namespace runpac { namespace fcgixx { namespace util {


//TODO: credit original contributor
inline char hex_to_char(char const& c1, char const& c2)
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


inline std::string url_decode( const std::string& str )
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


} } } // ns
