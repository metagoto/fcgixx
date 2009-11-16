#pragma once

#include <string>

namespace runpac { namespace fcgixx { namespace session {

template< typename Identifier
        , template<typename> class Storage
        >
struct basic_session : public Identifier
                     , public Storage<typename Identifier::id_type>
{

    typedef Identifier identifier;
    typedef typename Identifier::id_type id_type;
    typedef Storage<id_type> storage;


    basic_session(typename identifier::request_type& request
                 ,typename identifier::response_type& response)
        : identifier(request, response)
    {
    }


    void start() // tmp
    {
        id_type id;
        if (identifier::has_id(id)) {
            storage::ensure(id);
        }
        else {
            storage::add(identifier::new_id());
        }
    }


    template<typename T = std::string>
    T get(const char* name, const T& def = T())
    {
        id_type id;
        if (identifier::has_id(id)) {
            return storage::template get<T>(id, name, def);
        }
        return def;
    }


    template<typename T = std::string>
    void set(const char* name, const T& value)
    {
        id_type id;
        if (identifier::has_id(id)) {
            storage::set(id, name, value);
        }
    }

};


} } } // ns
