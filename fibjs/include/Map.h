/*
 * Map.h
 *
 *  Created on: Aug 23, 2012
 *      Author: lion
 */

#ifndef MAP_H_
#define MAP_H_

#include "ifs/Map.h"
#include "map"

namespace fibjs {

class Map : public Map_base {
public:
    // Map_base
    virtual result_t get_size(int32_t& retVal);
    virtual result_t clear();
    virtual result_t has(exlib::string name, bool& retVal);
    virtual result_t get(exlib::string name, Variant& retVal);
    virtual result_t put(v8::Local<v8::Object> map);
    virtual result_t put(exlib::string name, Variant value);
    virtual result_t set(v8::Local<v8::Object> map);
    virtual result_t set(exlib::string name, Variant value);
    virtual result_t remove(exlib::string name);
    virtual result_t isEmpty(bool& retVal);
    virtual result_t _named_getter(exlib::string property, Variant& retVal);
    virtual result_t _named_enumerator(v8::Local<v8::Array>& retVal);
    virtual result_t _named_setter(exlib::string property, Variant newVal);
    virtual result_t _named_deleter(exlib::string property, v8::Local<v8::Boolean>& retVal);

public:
    template <typename T>
    inline result_t map(T* o, result_t (T::*fn)(exlib::string name, Variant value))
    {
        std::map<exlib::string, VariantEx>::iterator iter;
        result_t hr;

        for (iter = m_datas.begin(); iter != m_datas.end(); iter++) {
            hr = (o->*fn)(iter->first, iter->second);
            if (hr < 0)
                return hr;
        }

        return 0;
    }

private:
    std::map<exlib::string, VariantEx> m_datas;
};

} /* namespace fibjs */
#endif /* MAP_H_ */
