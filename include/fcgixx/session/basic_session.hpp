#pragma once

#include <string> // tmp. need proper parametric stuff

namespace runpac { namespace fcgixx { namespace session {


template< typename Identifier
        , typename Storage
        >
struct basic_session : public Identifier
                     , public Storage
{

    typedef Identifier identifier;
    typedef Storage storage;


    basic_session(typename identifier::request_type& request
                 ,typename identifier::response_type& response)
        : identifier(request, response)
    {
    }

    void start()
    {
        std::string id;
        if (identifier::has_id(id)) {
            storage::ensure(id);
        }
        else {
            id = identifier::new_id();
            storage::add(id);
        }
    }


    template<typename T = std::string>
    T get(const char* name, const T& def = T())
    {
        std::string id;
        if (identifier::has_id(id)) {
            return storage::template get<T>(id, name, def);
        }
        return def;
    }


    template<typename T = std::string>
    void set(const char* name, const T& value)
    {
        std::string id;
        if (identifier::has_id(id)) {
            storage::set(id, name, value);
        }
    }

};


} } } // ns
