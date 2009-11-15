#pragma once

#include <string>

#include <boost/unordered_map.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>


namespace runpac { namespace fcgixx { namespace dispatcher {

template <typename Host>
class simple
{

public:
    typedef simple<Host> self_type;

    typedef boost::function<bool ()> callback_t;

    typedef boost::unordered_map<std::string, callback_t> routes_def_t;

    simple()
    {

    }


    ~simple()
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

    void set_fatal(char const* route)
    {
        fatal_route = route;
    }


    bool dispatch(std::string const& route)
    {
        try {
            routes_def_t::const_iterator itr = routes_def.find(route);
            if (itr != routes_def.end()) {
                return itr->second();
            }
            return false;
        }
        catch(const std::exception& e)  {
            std::cout << "dispatcher exception: " << e.what() << std::endl;
            return fallback();
        }
        catch(...) {
            std::cout << "dispatcher unknown exception" << std::endl;
            return fallback();
        }
    }

    bool fallback()
    {
        if (fatal_route.empty()) return false;

        routes_def_t::const_iterator itr = routes_def.find(fatal_route);
        if (itr != routes_def.end()) {
            return itr->second();
        }
        return false;
    }

    routes_def_t routes_def;
    std::string fatal_route;


};


} } } // ns


