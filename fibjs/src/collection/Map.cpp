/*
 * Map.cpp
 *
 *  Created on: Aug 23, 2012
 *      Author: lion
 */

#include "Map.h"

namespace fibjs
{

result_t Map_base::_new(obj_ptr<Map_base> &retVal, v8::Local<v8::Object> This)
{
    retVal = new Map();
    return 0;
}

result_t Map_base::_new(v8::Local<v8::Object> map, obj_ptr<Map_base>& retVal,
                        v8::Local<v8::Object> This)
{
    retVal = new Map();
    return retVal->put(map);
}

result_t Map::get_size(int32_t &retVal)
{
    retVal = (int32_t) m_datas.size();
    return 0;
}

result_t Map::clear()
{
    m_datas.clear();
    return 0;
}

result_t Map::has(const char *name, bool &retVal)
{
    retVal = m_datas.find(name) != m_datas.end();
    return 0;
}

inline result_t _map(Map *o, v8::Local<v8::Object> m,
                     result_t (Map::*fn)(const char *name, Variant value))
{
    v8::Local<v8::Array> ks = m->GetPropertyNames();
    int32_t len = ks->Length();
    int32_t i;

    for (i = 0; i < len; i++)
    {
        v8::Local<v8::Value> k = ks->Get(i);
        (o->*fn)(*v8::String::Utf8Value(k), m->Get(k));
    }

    return 0;
}

result_t Map::get(const char *name, Variant &retVal)
{
    std::map<exlib::string, VariantEx>::iterator it = m_datas.find(name);

    if (it == m_datas.end())
        return CALL_RETURN_NULL;

    retVal = it->second;
    return 0;
}

result_t Map::put(const char *name, Variant value)
{
    if (value.type() == Variant::VT_JSValue)
        setJSObject();

    m_datas.insert(std::pair<exlib::string, VariantEx>(name, value));
    return 0;
}

result_t Map::put(v8::Local<v8::Object> map)
{
    return _map(this, map, &Map::put);
}

result_t Map::remove(const char *name)
{
    m_datas.erase(name);
    return 0;
}

result_t Map::isEmpty(bool &retVal)
{
    retVal = m_datas.empty();
    return 0;
}

result_t Map::_named_getter(const char *property, Variant &retVal)
{
    return get(property, retVal);
}

result_t Map::_named_enumerator(v8::Local<v8::Array> &retVal)
{
    int32_t i = 0;
    Isolate* isolate = holder();

    retVal = v8::Array::New(isolate->m_isolate, (int32_t)m_datas.size());
    std::map<exlib::string, VariantEx>::iterator iter;

    for (iter = m_datas.begin(); iter != m_datas.end(); iter++)
        retVal->Set(i++, isolate->NewFromUtf8(iter->first));

    return 0;
}

result_t Map::_named_setter(const char *property, Variant newVal)
{
    return put(property, newVal);
}

result_t Map::_named_deleter(const char *property,
                             v8::Local<v8::Boolean> &retVal)
{
    return remove(property);
}

} /* namespace fibjs */
