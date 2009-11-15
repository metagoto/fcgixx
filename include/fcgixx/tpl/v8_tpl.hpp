#pragma once

#include <fcgixx/tpl/basic_tpl.hpp>
#include <fcgixx/tpl/engine/simple_v8.hpp>
#include <fcgixx/tpl/cache/simple.hpp>
#include <fcgixx/tpl/loader/filesys.hpp>


namespace runpac { namespace fcgixx { namespace tpl {


typedef loader::filesys loader_t;

typedef engine::simple_v8<fcgixx::tpl::compiler_t> engine_t;

typedef cache::simple<engine_t::compiled_type
                     ,engine_t::release> cache_t;

typedef basic_tpl<loader_t
                 ,engine_t
                 ,cache_t> v8_tpl;


} } } // ns
