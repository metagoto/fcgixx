#pragma once

#include <string>
#include <boost/unordered_map.hpp>


namespace runpac { namespace fcgixx { namespace tpl { namespace cache {

template<typename Compiled_Type
        ,void (*F)(Compiled_Type&)>
struct simple
{
    typedef Compiled_Type compiled_type;

    typedef boost::unordered_map<std::string, compiled_type> container_type;

    typedef typename container_type::const_iterator const_iterator;


    ~simple()
    {
        clear();
    }

    void clear()
    {
        for (typename container_type::iterator i(container.begin()), e(container.end()); i != e; ++i) {
            (*F)(i->second);
        }
        container.clear();
    }

    const_iterator get(const std::string& id)
    {
        return container.find(id);
    }

    const_iterator set(const std::string& id, const compiled_type& item)
    {
        return container.insert(container.end(), std::make_pair(id, item));
    }

    const_iterator end()
    {
        return container.end();
    }

private:
    container_type container;
};


} } } } // ns


