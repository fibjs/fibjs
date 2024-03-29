/*
 * HttpCollection.h
 *
 *  Created on: Aug 9, 2012
 *      Author: lion
 */

#pragma once

#include "ifs/HttpCollection.h"
#include "QuickArray.h"

namespace fibjs {

class HttpCollection : public HttpCollection_base {
public:
    HttpCollection()
        : m_count(0)
    {
        m_map.resize(16);
    }

public:
    // HttpCollection_base
    virtual result_t clear();
    virtual result_t has(exlib::string name, bool& retVal);
    virtual result_t first(exlib::string name, Variant& retVal);
    virtual result_t get(exlib::string name, Variant& retVal);
    virtual result_t all(exlib::string name, obj_ptr<NObject>& retVal);
    virtual result_t add(v8::Local<v8::Object> map);
    virtual result_t add(exlib::string name, v8::Local<v8::Array> values);
    virtual result_t add(exlib::string name, Variant value);
    virtual result_t set(v8::Local<v8::Object> map);
    virtual result_t set(exlib::string name, v8::Local<v8::Array> values);
    virtual result_t set(exlib::string name, Variant value);
    virtual result_t remove(exlib::string name);
    virtual result_t _delete(exlib::string name);
    virtual result_t sort();
    virtual result_t keys(obj_ptr<NArray>& retVal);
    virtual result_t values(obj_ptr<NArray>& retVal);
    virtual result_t _named_getter(exlib::string property, Variant& retVal);
    virtual result_t _named_enumerator(v8::Local<v8::Array>& retVal);
    virtual result_t _named_setter(exlib::string property, Variant newVal);
    virtual result_t _named_deleter(exlib::string property, v8::Local<v8::Boolean>& retVal);

public:
    void add(const char* name, int32_t szName, const char* value, int32_t szValue)
    {
        add(exlib::string(name, szName), exlib::string(value, szValue));
    }

    result_t add(exlib::string& name, exlib::string value)
    {
        if (m_map.size() < m_count + 1)
            m_map.resize(m_count + 1);

        m_map[m_count] = pair(name, value);
        m_count++;

        return 0;
    }

    result_t first(exlib::string name, exlib::string& retVal)
    {
        size_t i;

        for (i = 0; i < m_count; i++) {
            pair& _pair = m_map[i];

            if (!qstricmp(_pair.first.c_str(), name.c_str())) {
                retVal = _pair.second;
                return 0;
            }
        }

        return CALL_RETURN_NULL;
    }

    result_t all(exlib::string name, obj_ptr<NArray>& retVal)
    {
        obj_ptr<NArray> list = new NArray();
        size_t i;

        for (i = 0; i < m_count; i++) {
            pair& _pair = m_map[i];

            if (!qstricmp(_pair.first.c_str(), name.c_str()))
                list->append(_pair.second);
        }

        retVal = list;
        return 0;
    }

    result_t all(obj_ptr<NObject>& retVal)
    {
        obj_ptr<NObject> map = new NObject();
        size_t i;

        map->enable_multi_value();

        for (i = 0; i < m_count; i++) {
            pair& _pair = m_map[i];
            map->add(_pair.first, _pair.second);
        }

        retVal = map;
        return 0;
    }

    size_t size();
    size_t getData(char* buf, size_t sz);

    result_t parse(exlib::string& str, const char* sep = "&", const char* eq = "=");
    result_t parseCookie(exlib::string& str);

private:
    typedef std::pair<exlib::string, exlib::string> pair;
    std::vector<pair> m_map;
    size_t m_count;
};

} /* namespace fibjs */
