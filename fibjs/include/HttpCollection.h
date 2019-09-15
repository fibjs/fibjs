/*
 * HttpCollection.h
 *
 *  Created on: Aug 9, 2012
 *      Author: lion
 */

#include "ifs/HttpCollection.h"
#include "QuickArray.h"

#ifndef HTTPCOLLECTION_H_
#define HTTPCOLLECTION_H_

namespace fibjs {

class HttpCollection : public HttpCollection_base {
public:
    HttpCollection()
        : m_count(0)
    {
    }

public:
    // HttpCollection_base
    virtual result_t clear();
    virtual result_t has(exlib::string name, bool& retVal);
    virtual result_t first(exlib::string name, Variant& retVal);
    virtual result_t all(exlib::string name, obj_ptr<NObject>& retVal);
    virtual result_t add(v8::Local<v8::Object> map);
    virtual result_t add(exlib::string name, v8::Local<v8::Array> values);
    virtual result_t add(exlib::string name, Variant value);
    virtual result_t set(v8::Local<v8::Object> map);
    virtual result_t set(exlib::string name, v8::Local<v8::Array> values);
    virtual result_t set(exlib::string name, Variant value);
    virtual result_t remove(exlib::string name);
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
        m_map[m_count] = pair(name, value);
        m_count++;

        return 0;
    }

    result_t first(exlib::string name, exlib::string& retVal)
    {
        int32_t i;

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
        int32_t i;

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
        int32_t i;

        for (i = 0; i < m_count; i++) {
            pair& _pair = m_map[i];
            std::map<exlib::string, Variant>::iterator it = map->m_datas.find(_pair.first);

            if (it == map->m_datas.end())
                map->m_datas.insert(std::pair<exlib::string, VariantEx>(_pair.first, _pair.second));
            else {
                obj_ptr<NArray> list;

                if (it->second.type() != Variant::VT_Object) {
                    list = new NArray();
                    list->append(it->second);
                    it->second = list;
                } else
                    list = (NArray*)it->second.object();

                list->append(_pair.second);
            }
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
    QuickArray<pair> m_map;
    int32_t m_count;
};

} /* namespace fibjs */
#endif /* HTTPCOLLECTION_H_ */
