#pragma once

#include <string>

#include "../../fastcgi.hpp"

#include <v8.h>

#include "../conversion.hpp"

#include <iostream>


namespace runpac { namespace fcgixx { namespace policy {

template <typename Host>
class js_router
{
    typedef js_router<Host> self_type;

public:

    js_router()
    {
        v8::HandleScope scope;

        v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New();
        global->Set(v8::String::New("setRoute"), v8::FunctionTemplate::New(set_route_cb, v8::External::New(this)));

        // tmp logging
        global->Set(v8::String::New("log"), v8::FunctionTemplate::New(tmp_log, v8::External::New(this)));

        js_context = v8::Context::New(0, global);
    }


    ~js_router()
    {
        js_script.Dispose();
        js_context.Dispose();
    }


    void route(FCGIRequest* request, const std::string& path_info)
    {
        this->request = request;

        v8::HandleScope scope;
        v8::Context::Scope context_scope(js_context);

        //std::cout << "test from js_router2" << std::endl;

        js_context->Global()->Set(v8::String::New("REQUEST_METHOD")
                                 ,v8::String::New(request->params["REQUEST_METHOD"].c_str()));
        js_context->Global()->Set(v8::String::New("PATH_INFO"), v8::String::New(path_info.c_str()));

        js_script->Run();
    }


    void set_js_source(const std::string& js_source)
    {
        v8::HandleScope scope;
        v8::Context::Scope context_scope(js_context);

        v8::Handle<v8::String> source = v8::String::New(js_source.c_str());
        js_script = v8::Persistent<v8::Script>::New(v8::Script::Compile(source));
    }


  static v8::Handle<v8::Value> set_route_cb(const v8::Arguments& args)
  {
        using namespace std;
        using namespace v8;

        HandleScope scope;

        Handle<External> field = Handle<External>::Cast(args.Data());
        void* ptr = field->Value();
        self_type* self = static_cast<self_type*>(ptr);

        Context::Scope context_scope(self->js_context);

        if (args.Length() != 1) {
            throw std::logic_error("router::set_route_cb : bad args count");
        }

        Local<Object> obj = args[0]->ToObject();

        const string route_name = cast::get_to<string>(args[0], "route", "error");
        self->request->params.insert(make_pair("route", route_name));

        Local<Array> params_array = Local<Array>::Cast(obj->Get(String::New("params")));
        for (uint32_t i = 0; i < params_array->Length(); ++i) {
            Local<Value> param = params_array->Get(Integer::New(i));
            const string name = cast::get_to<string>(param, "name");
            const string value = cast::get_to<string>(param, "value");
            if (name.length() && value.length()) {
                self->request->params.insert(make_pair(name, value));
            }
        }

        return Undefined();
  }

  // tmp_log
  static v8::Handle<v8::Value> tmp_log(const v8::Arguments& args)
  {
      const std::string log_msg = cast::to<std::string>(args[0]);
      std::cout << "js_router: " << log_msg << std::endl;
      return v8::Undefined();
  }

private:

    v8::Persistent<v8::Context> js_context;
    v8::Persistent<v8::Script> js_script;
    FCGIRequest* request;

};


} } } // ns


