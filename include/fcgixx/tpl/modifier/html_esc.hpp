#pragma once

#include <string>

namespace runpac { namespace fcgixx { namespace tpl { namespace modifier {


struct html_esc
{
    std::string operator()(std::string::iterator s, std::string::iterator e) const
    {
        std::string res;
        for ( ; s != e; ++s) {
            switch (*s) {
                case '&': res += "&amp;"; break;
                case '"': res += "&quot;"; break;
                case '<': res += "&lt;"; break;
                case '>': res += "&gt;"; break;
                default: res += *s;
            }
        }
        return res;
    }
};



} } } } // ns
