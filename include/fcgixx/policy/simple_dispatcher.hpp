#pragma once

#include <string>

#include <boost/unordered_map.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>


namespace runpac { namespace fcgixx { namespace policy {

template <typename Host>
class simple_dispatcher
{

public:
    typedef simple_dispatcher<Host> self_type;

    typedef boost::function<bool ()> callback_t;

    typedef boost::unordered_map<std::string, callback_t> routes_def_t;

    simple_dispatcher()
    {

    }


    ~simple_dispatcher()
    {

    }


    template<typename T>
    void bind(char const* route, T const& callback)
    {
        routes_def.insert(std::make_pair(route, boost::bind(callback, static_cast<Host*>(this))));
    }

    void bind(char const* route, callback_t const& callback)
    {
        routes_def.insert(std::make_pair(route, callback));
    }


    bool dispatch(std::string const& route)
    {
        try {
            routes_def_t::const_iterator itr = routes_def.find(route);
            if (itr != routes_def.end()) {
                itr->second();
                return true;
            }
            return false;
        }
        catch(const std::exception& e)  {
            std::cout << "bool dispatch(std::string const& route) : exception: " << e.what() << std::endl;

            routes_def_t::const_iterator itr = routes_def.find("__500__");
            if (itr != routes_def.end()) {
                itr->second();
                return true;
            }
            return false;
        }
        catch(...) {
            std::cout << "bool dispatch(std::string const& route) : unknown exception" << std::endl;

            routes_def_t::const_iterator itr = routes_def.find("__500__");
            if (itr != routes_def.end()) {
                itr->second();
                return true;
            }
            return false;
        }
    }


    routes_def_t routes_def;

};


} } } // ns


