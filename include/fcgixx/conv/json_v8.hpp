#pragma once

#include <string>
#include <map>
#include <vector>
#include <boost/variant/static_visitor.hpp>
#include <v8.h>
#include <fcgixx/json_type.hpp>


namespace runpac { namespace fcgixx { namespace conv {


// V8 -> NATIVE
template<typename T>
inline T to(const v8::Handle<v8::Value>& val)
{
}

template<>
inline std::string to<std::string>(const v8::Handle<v8::Value>& val)
{
    //return *v8::String::AsciiValue(val->ToString());
    return *v8::String::Utf8Value(val->ToString());
}

template<>
inline int to<int>(const v8::Handle<v8::Value>& val)
{
    return val->Int32Value();
}

template<>
inline double to<double>(const v8::Handle<v8::Value>& val)
{
    return val->NumberValue();
}

template<>
inline std::time_t to<std::time_t>(const v8::Handle<v8::Value>& val)
{
    return val->IntegerValue();
}

template<>
inline bool to<bool>(const v8::Handle<v8::Value>& val)
{
    return val->BooleanValue();
}


// NATIVE -> V8

template<typename F>
inline v8::Handle<v8::Value> to(const F& val)
{
    //return v8::Undefined();
    return v8::Null();
}

template<>
inline v8::Handle<v8::Value> to<std::string>(const std::string& val)
{
    return v8::String::New(val.c_str());
}

v8::Handle<v8::Value>
inline to(const char* val)
{
    return v8::String::New(val);
}

template<>
inline v8::Handle<v8::Value> to<int>(const int& val)
{
    return v8::Int32::New(val);
}

template<>
inline v8::Handle<v8::Value> to<double>(const double& val)
{
    return v8::Number::New(val);
}

template<>
inline v8::Handle<v8::Value> to<std::time_t>(const std::time_t& val)
{
    return v8::Integer::New(static_cast<uint32_t>(val));
}

template<>
inline v8::Handle<v8::Value> to<bool>(const bool& val)
{
    return v8::Boolean::New(val);
}


/////////
struct json_type_converter_visitor : public boost::static_visitor<v8::Handle<v8::Value> >
{
    template<typename T>
    v8::Handle<v8::Value> operator()(const T& val) const
    {
        return to(val);
    }
};

template<>
inline v8::Handle<v8::Value> to<json_type>(const json_type& val)
{
    return boost::apply_visitor(json_type_converter_visitor(), val);
}
/////////////


template<typename T>
inline v8::Handle<v8::Value> to(const std::vector<T>& val)
{
    v8::Local<v8::Array> arr = v8::Array::New(val.size());
    typename std::vector<T>::const_iterator i(val.begin()), e(val.end());
    for (int k = 0; i != e; ++i) {
        arr->Set(v8::Integer::New(k++), to(*i));
    }
    return arr;
}

template<typename T>
inline v8::Handle<v8::Value> to(const std::map<std::string, T>& val)
{
    v8::Local<v8::Object> obj = v8::Object::New();
    for (typename std::map<std::string, T>::const_iterator i(val.begin()), e(val.end()); i != e; ++i) {
        obj->ForceSet(v8::String::New(i->first.c_str()), to(i->second));
    }
    return obj;
}


template<>
inline v8::Handle<v8::Value> to<json_array>(const json_array& val)
{
    v8::Local<v8::Array> arr = v8::Array::New(val.size());
    json_array::const_iterator i(val.begin()), e(val.end());
    for (int k = 0; i != e; ++i) {
        arr->Set(v8::Integer::New(k++), to(*i));
    }
    return arr;
}


template<>
inline v8::Handle<v8::Value> to<json_object>(const json_object& val)
{
    v8::Local<v8::Object> obj = v8::Object::New();
    json_object::const_iterator i = val.begin();
    for (json_object::const_iterator i(val.begin()), e(val.end()); i != e; ++i) {
        obj->ForceSet(v8::String::New(i->first.c_str()), to(i->second));
    }
    return obj;
}


/// V8 ACCESSOR HELPERS

template<typename T>
inline T get_to(const v8::Handle<v8::Value>& val, const char* name)
{
}

template<typename T>
inline T get_to(const v8::Handle<v8::Value>& val, const char* name, const T& def_val)
{
}

template<>
inline std::string get_to(const v8::Handle<v8::Value>& val, const char* name, const std::string& def_val)
{
    v8::HandleScope scope;
    v8::Handle<v8::Object> obj = val->ToObject();
    v8::Handle<v8::String> key = v8::String::New(name);
    if (obj->Has(key) && !obj->Get(key)->IsNull() && !obj->Get(key)->IsUndefined()) {
        return to<std::string>(obj->Get(key));
    }
    return def_val;
}

template<>
inline std::string get_to(const v8::Handle<v8::Value>& val, const char* name)
{
    return get_to<std::string>(val, name, "");
}



} } } // ns

