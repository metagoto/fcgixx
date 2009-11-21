#pragma once

#include <string>
#include <boost/function.hpp>
#include <boost/bind.hpp>


namespace runpac { namespace fcgixx { namespace dispatcher {

template <typename Host>
struct dummy
{

    typedef bool (Host::*callback_t)();


    dummy() : fun(0)
    { }


    void bind(callback_t const& callback)
    {
        fun = callback;
    }


    bool dispatch(std::string const& route)
    {
        if (fun) return (static_cast<Host*>(this)->*fun)();
        return false;
    }


private:
    callback_t fun;

};

/*
template <typename Host>
struct dummy
{

    typedef boost::function<bool ()> callback_t;


    template<typename T>
    void bind(T const& callback)
    {
        fun = boost::bind(callback, static_cast<Host*>(this));
    }


    void bind(callback_t const& callback)
    {
        fun = callback;
    }


    bool dispatch(std::string const& route)
    {
        return fun();
    }


private:
    callback_t fun;


};
*/

} } } // ns


