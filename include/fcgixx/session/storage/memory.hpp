#pragma once

#include <boost/unordered_map.hpp>


namespace runpac { namespace fcgixx { namespace session { namespace storage {

template<typename Id>
struct memory
{

    typedef Id id_type;

    struct node
    {

        typedef const char* const key_type;
        typedef std::string mapped_type;

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
            params.insert(std::make_pair(name, boost::lexical_cast<mapped_type>(value)));
        }


        typedef boost::unordered_map<key_type, mapped_type> params_type;
        params_type params;

    };


    void add(const id_type& id)
    {
        if (nodes.find(id) == nodes.end()) {
            nodes.insert(std::make_pair(id, node()));
        }
    }

    void ensure(const id_type& id)
    {
        if (nodes.find(id) == nodes.end()) {
            nodes.insert(std::make_pair(id, node()));
        }
    }

    template<typename T = std::string>
    T get(const id_type& id, const char* name, const T& def = T())
    {
        typename nodes_type::const_iterator i = nodes.find(id);
        if (i != nodes.end()) {
            return i->second.get<T>(name, def);
        }
        return def;
    }

    template<typename T = std::string>
    void set(const id_type& id, const char* name, const T& value)
    {
        typename nodes_type::iterator i = nodes.find(id);
        if (i != nodes.end()) {
            i->second.set(name, value);
        }
    }

    /// tmp
    std::string check()
    {
        std::string s;
        for (typename nodes_type::const_iterator i(nodes.begin()), e(nodes.end()); i!=e; ++i) {
            s += i->first + " - ";
        }
        return s + "\n";
    }
    ///

    typedef boost::unordered_map<id_type, node> nodes_type;
    nodes_type nodes;

};


} } } } //ns
