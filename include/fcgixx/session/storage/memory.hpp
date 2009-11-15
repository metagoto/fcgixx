#pragma once

#include <boost/unordered_map.hpp>


namespace runpac { namespace fcgixx { namespace session { namespace storage {


struct memory
{

    struct node
    {

        template<typename T = std::string>
        T get(const char* name, const T& def = T()) const
        {
            params_type::const_iterator i = params.find(name);
            if (i != params.end()) {
                return boost::lexical_cast<T>(i->second);
            }
            return def;
        }


        template<typename T = std::string>
        void set(const char* name, const T& value)
        {
            params[name] = value;
        }


        typedef boost::unordered_map<std::string, std::string> params_type;
        params_type params;

    };


    void add(const std::string& id)
    {
        if (nodes.find(id) == nodes.end()) {
            nodes[id] = node();
        }
    }

    void ensure(const std::string& id)
    {
        if (nodes.find(id) == nodes.end()) {
            nodes[id] = node();
        }
    }

    template<typename T = std::string, typename U = std::string>
    U get(const T& id, const char* name, const U& def = T())
    {
        nodes_type::const_iterator i = nodes.find(id);
        if (i != nodes.end()) {
            return i->second.get<U>(name, def);
        }
        return def;
    }

    template<typename T = std::string, typename U = std::string>
    void set(const T& id, const char* name, const U& value)
    {
        nodes_type::iterator i = nodes.find(id);
        if (i != nodes.end()) {
            i->second.set(name, value);
        }
    }

    ///
    std::string check()
    {
        std::string s;
        for (nodes_type::const_iterator i(nodes.begin()), e(nodes.end()); i!=e; ++i) {
            s += i->first + " - ";
        }
        return s + "\n";
    }
    ///

    typedef boost::unordered_map<std::string, node> nodes_type;
    nodes_type nodes;

};

} } } } //ns
