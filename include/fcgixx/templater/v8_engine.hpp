#pragma once

#include <v8.h>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_confix.hpp>

#include "detail/js_template_grammar.hpp"
#include "detail/js_template_compiler.hpp"
#include "../conversion.hpp"


namespace runpac { namespace fcgixx {


template<typename Comp>
struct v8_engine
{

    typedef v8_engine self_type;

    typedef v8::Persistent<v8::Script> compiled_type;

    typedef boost::function<std::string (std::string::iterator, std::string::iterator)> modifier_f;
    typedef boost::unordered_map<std::string, modifier_f> modifiers_type;


    v8_engine()
    {
        clear_context();
    }

    ~v8_engine()
    {
        if (!js_context.IsEmpty()) {
            js_context.Dispose();
        }
    }


    void clear_context()
    {
        if (!js_context.IsEmpty()) {
            js_context.Dispose();
        }
        init_context();
    }

    //private
    void init_context()
    {
        v8::HandleScope scope;

        v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New();

        global->Set(v8::String::New("__pr"), v8::FunctionTemplate::New(print_cb, v8::External::New(this)));
        global->Set(v8::String::New("__p"), v8::FunctionTemplate::New(print_html_cb, v8::External::New(this)));

        js_context = v8::Context::New(0, global);
        /////////
        //modifiers["h"] = html_esc();
    }


    template<typename T>
    void assign(const char* name, const T& value)
    {
        v8::HandleScope scope;
        v8::Context::Scope context_scope(js_context);

        js_context->Global()->ForceSet(v8::String::New(name), cast::to(value));
    }

    void assign(const char* name, const char* value)
    {
        v8::HandleScope scope;
        v8::Context::Scope context_scope(js_context);

        js_context->Global()->ForceSet(v8::String::New(name), cast::to(value));
    }

    std::string render(const compiled_type& script)
    {
        v8::HandleScope scope;
        v8::Context::Scope context_scope(js_context);

        buf.clear();
        script->Run();
        return buf;
    }

    compiled_type compile(const std::string& source_str)
    {
        using namespace runpac::fcgixx::detail;

        js_template_compiler tpl_compiler;

        js_template_grammar::action_f raw_f      (bind(&js_template_compiler::raw,  &tpl_compiler, _1,_2));
        js_template_grammar::action_f directive_f(bind(&js_template_compiler::directive, &tpl_compiler, _1,_2));
        js_template_grammar::action_f echo_f     (bind(&js_template_compiler::echo, &tpl_compiler, _1,_2));
        js_template_grammar::action_f close_f    (bind(&js_template_compiler::close,&tpl_compiler, _1,_2));
        js_template_grammar::action_f statement_start_f(bind(&js_template_compiler::statement_start,&tpl_compiler, _1,_2));
        js_template_grammar::action_f else_f     (bind(&js_template_compiler::else_,&tpl_compiler, _1,_2));
        js_template_grammar::action_f raw_escape_mode_f(bind(&js_template_compiler::raw_escape_mode,&tpl_compiler, _1,_2));
        js_template_grammar::action_f default_modifier_f(bind(&js_template_compiler::default_modifier,&tpl_compiler, _1,_2));

        js_template_grammar parser(raw_f, directive_f, echo_f, close_f
                                  ,statement_start_f, else_f, raw_escape_mode_f, default_modifier_f);

        std::string tpl_js_source;
        boost::spirit::classic::parse_info<> info =
            boost::spirit::classic::parse(source_str.c_str(), parser);

        if (info.full) {
            tpl_compiler.flush();
            tpl_js_source = tpl_compiler.out.str();
        }
        else {
            // throw...
        }

        v8::HandleScope scope;
        v8::Context::Scope context_scope(js_context);

        v8::Handle<v8::String> source = v8::String::New(tpl_js_source.c_str());

        return v8::Persistent<v8::Script>::New(v8::Script::New(source));
    }

    static v8::Handle<v8::Value> print_cb(const v8::Arguments& args)
    {
        v8::HandleScope scope;

        if (args.Length() != 1) {
            throw std::logic_error("print_cb : bad args count");
        }

        self_type* self = self_ptr_from(args);
        v8::Context::Scope context_scope(self->js_context);

        self->buf.append(cast::to<std::string>(args[0]));
        return v8::Undefined();
    }

    static v8::Handle<v8::Value> print_html_cb(const v8::Arguments& args)
    {
        v8::HandleScope scope;

        if (args.Length() < 1) {
            throw std::logic_error("print_cb : bad args count");
        }

        self_type* self = self_ptr_from(args);
        v8::Context::Scope context_scope(self->js_context);

        std::string str(cast::to<std::string>(args[0]));
        if (args[1]->IsArray()) {
            v8::Handle<v8::Array> arr = v8::Handle<v8::Array>::Cast(args[1]);
            for (unsigned int i = 0; i < arr->Length(); ++i) {
                std::string key(cast::to<std::string>(arr->Get(v8::Integer::New(i))));
                modifiers_type::const_iterator it = self->modifiers.find(key);
                if (it != self->modifiers.end()) {
                    str = it->second(str.begin(), str.end());
                }
            }
        }

        self->buf.append(str);
        return v8::Undefined();
    }

    static void release(compiled_type& item)
    {
        item.Dispose();
    }


    void register_modifier(const std::string& name, const modifier_f& modifier)
    {
        modifiers[name] = modifier;
    }


private:

    static self_type* self_ptr_from(const v8::Arguments& args)
    {
        v8::Handle<v8::External> field = v8::Handle<v8::External>::Cast(args.Data());
        return static_cast<self_type*>(field->Value());
    }


    std::string buf;

    v8::Persistent<v8::Context> js_context;

    boost::unordered_map<std::string, modifier_f> modifiers;

};


} } // ns


