#pragma once

#include <string>
#include <boost/function.hpp>
#include <boost/bind.hpp>


namespace runpac { namespace fcgixx { namespace dispatcher {

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


private:    \
    callback_t fun;


};


} } } // ns


