#pragma once

#include <algorithm>
#include <vector>
#include <string>
#include <sstream>
#include <boost/algorithm/string/replace.hpp>


namespace runpac { namespace fcgixx { namespace tpl { namespace compiler {


struct simple_js
{
    simple_js()
    { }

    void raw(char const* s, char const* e)
    {
        raw_buf.append(s, e);
    }

    void statement_start(char const* s, char const* e)
    {
        flush();
    }

    void directive(char const* s, char const* e)
    {
        std::string str(s, e);
        if (*(e-1) == ':') {
          str = std::string(s, --e) + "{";
        }
        out << str;
    }

    void echo(char const* s, char const* e)
    {
        flush();
        std::string str(s, e);

        if (!modifiers.empty()) {
            std::ostringstream ss;
            ss << '[';
            for (std::vector<std::string>::iterator it(modifiers.begin()), ite(modifiers.end())
                ; it != ite ; ++it) {
                //[&ss](std::string o){ss<<'\''<<o<<"\',";});
                ss << '\'' << *it << "\',";
            }
            ss << ']';
            out << "__p(" << str << "," << ss.str() << ");";
            modifiers.clear();
        }
        else {
            out << "__p(" << str << ",['h']);";
        }
    }

    void else_(char const* s, char const* e)
    {
        out << "}else ";
    }

    void close(char const* s, char const* e)
    {
        out << "}";
    }

    void raw_escape_mode(char const* s, char const* e)
    {
        modifiers.push_back(std::string(s, e));
    }

    // not used
    void default_modifier(char const* s, char const* e)
    {
        modifiers.push_back("h"); ///
    }

    void flush()
    {
        if (raw_buf.size()) {
            boost::algorithm::replace_all(raw_buf, "\n", "\\n");
            boost::algorithm::replace_all(raw_buf, "'", "\\'");
            out << "__pr('" << raw_buf << "');";
            raw_buf.clear();
        }
    }

    std::string raw_buf;
    std::ostringstream out;

    std::vector<std::string> modifiers;

};


} } } } // ns

