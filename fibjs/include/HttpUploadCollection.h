/*
 * HttpUploadCollection.h
 *
 *  Created on: Aug 28, 2012
 *      Author: lion
 */

#pragma once

#include "ifs/HttpCollection.h"
#include "QuickArray.h"

namespace fibjs {

class HttpUploadCollection : public HttpCollection_base {
public:
    HttpUploadCollection()
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
    void parse(exlib::string& str, const char* boundary);

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

private:
    typedef std::pair<exlib::string, VariantEx> pair;
    std::vector<pair> m_map;
    size_t m_count;
};

} /* namespace fibjs */
