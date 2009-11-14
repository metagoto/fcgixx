#pragma once

#include <string>
#include <boost/unordered_map.hpp>


namespace runpac { namespace fcgixx {

template<typename Engine>
struct stub_cache
{
    typedef typename Engine::compiled_type compiled_type;

    typedef boost::unordered_map<std::string, compiled_type> container_type;

    typedef typename container_type::const_iterator const_iterator;

    void clear()
    {
    }

    const_iterator get(const std::string& id)
    {
        return container.end();
    }

    const_iterator set(const std::string& id, const compiled_type& item)
    {
        return std::make_pair(id, item); // stub, no store // todo!!
    }

    const_iterator end()
    {
        return container.end();
    }

private:
    container_type container;
};


} } // ns


