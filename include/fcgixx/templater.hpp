#pragma once

#include <boost/noncopyable.hpp>
#include "templater/stub_cache.hpp"


namespace runpac { namespace fcgixx {


struct compiler_t { }; // todo move spirit stuff outside tpl engine


template< typename Loader
        , typename Engine
        , typename Cache // = stub_cache
        >
struct templater : public Loader
                 , public Engine
                 , public Cache
                 , private boost::noncopyable
{

    typedef Loader loader;
    typedef Engine engine;
    typedef Cache  cache;

    templater()
        : loader()
        , engine()
        , cache()
    {
    }

    ~templater()
    {
    }

    std::string render(const std::string& id)
    {
        typename cache::const_iterator it = cache::get(id);
        if (it == cache::end()) {
            const std::string& src = loader::load(id);
            const typename engine::compiled_type& item = engine::compile(src);
            it = cache::set(id, item);
        }
        //else { } // todo pour stub
        return engine::render(it->second);
    }

};


} } // ns


