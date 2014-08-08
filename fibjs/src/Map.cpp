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
                     result_t (Map::*fn)(const char *name, v8::Local<v8::Value> value))
{
    v8::Local<v8::Array> ks = m->GetPropertyNames();
    int len = ks->Length();
    int i;

    for (i = 0; i < len; i++)
    {
        v8::Local<v8::Value> k = ks->Get(i);

        if (!k->IsNumber() && !k->IsNumberObject())
            (o->*fn)(*v8::String::Utf8Value(k), m->Get(k));
    }

    return 0;
}

result_t Map::get(const char *name, v8::Local<v8::Value> &retVal)
{
    std::map<std::string, VariantEx>::iterator it = m_datas.find(name);

    if (it == m_datas.end())
        return CALL_RETURN_NULL;

    retVal = it->second;
    return 0;
}

result_t Map::put(const char *name, v8::Local<v8::Value> value)
{
    m_datas.insert(std::pair<std::string, Variant>(name, value));
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

result_t Map::_named_getter(const char *property, v8::Local<v8::Value> &retVal)
{
    return get(property, retVal);
}

result_t Map::_named_enumerator(v8::Local<v8::Array> &retVal)
{
    int32_t i = 0;

    retVal = v8::Array::New(isolate, (int)m_datas.size());
    std::map<std::string, VariantEx>::iterator iter;

    for (iter = m_datas.begin(); iter != m_datas.end(); iter++)
        retVal->Set(i++,
                    v8::String::NewFromUtf8(isolate, iter->first.c_str(),
                                            v8::String::kNormalString,
                                            (int) iter->first.length()));

    return 0;
}

result_t Map::_named_setter(const char *property, v8::Local<v8::Value> newVal)
{
    return put(property, newVal);
}

result_t Map::_named_deleter(const char *property,
                             v8::Local<v8::Boolean> &retVal)
{
    return remove(property);
}

} /* namespace fibjs */
