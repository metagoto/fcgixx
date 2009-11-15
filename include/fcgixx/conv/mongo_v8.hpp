#pragma once

#include <fcgixx/conv/json_v8.hpp>
#include <mongo/client/dbclient.h>


namespace runpac { namespace fcgixx { namespace conv {

template<>
inline v8::Handle<v8::Value> to<mongo::BSONElement>(const mongo::BSONElement& elem)
{
   switch (elem.type()) {
       case mongo::Undefined:
       case mongo::jstNULL:
           return v8::Null();
       case mongo::Bool:
           return v8::Boolean::New(elem.boolean());
       case mongo::NumberInt:
           return v8::Int32::New(elem.numberInt());
       case mongo::NumberDouble:
           return v8::Number::New(elem.numberDouble());
       case mongo::NumberLong:
           return v8::Integer::New(elem.numberInt()); //!
       case mongo::String:
           return v8::String::New(elem.valuestr());
       case mongo::Date:
           return v8::Date::New(elem.date());

       case mongo::Object: {
           v8::Local<v8::Object> obj = v8::Object::New();
           const mongo::BSONObj& embedded = elem.embeddedObject();
           for (mongo::BSONObjIterator it(embedded); it.more(); ) {
               const mongo::BSONElement& e = it.next();
               v8::Local<v8::Value> key = v8::String::New(e.fieldName());
               obj->ForceSet(key, to(e));
           }
           return obj;
       }

       case mongo::Array: {
           const mongo::BSONObj& embedded_arr = elem.embeddedObject();
           const int len = embedded_arr.nFields();
           v8::Local<v8::Array> arr = v8::Array::New(len);
           for (int i = 0; i < len; ++i) {
               arr->Set(v8::Integer::New(i), to(embedded_arr[i]));
           }
           return arr;
       }

       default:
           break;
    }
   return v8::Null();
}


template<>
inline v8::Handle<v8::Value> to<mongo::BSONObj>(const mongo::BSONObj& val)
{
    v8::Local<v8::Object> obj = v8::Object::New();
    for (mongo::BSONObjIterator it(val); it.more(); ) {
        const mongo::BSONElement& elem = it.next();
        obj->ForceSet(v8::String::New(elem.fieldName()), to(elem));
    }
    return obj;
}



} } } // ns
