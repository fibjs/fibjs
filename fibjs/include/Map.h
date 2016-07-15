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

namespace fibjs
{

class Map: public Map_base
{
public:
    // Map_base
    virtual result_t get_size(int32_t &retVal);
    virtual result_t clear();
    virtual result_t has(const char *name, bool &retVal);
    virtual result_t get(const char *name, Variant &retVal);
    virtual result_t put(v8::Local<v8::Object> map);
    virtual result_t put(const char *name, Variant value);
    virtual result_t remove(const char *name);
    virtual result_t isEmpty(bool &retVal);
    virtual result_t _named_getter(const char *property, Variant &retVal);
    virtual result_t _named_enumerator(v8::Local<v8::Array> &retVal);
    virtual result_t _named_setter(const char *property, Variant newVal);
    virtual result_t _named_deleter(const char *property, v8::Local<v8::Boolean> &retVal);

public:
    template<typename T>
    inline result_t map(T *o, result_t (T::*fn)(const char *name, Variant value))
    {
        std::map<exlib::string, VariantEx>::iterator iter;
        result_t hr;

        for (iter = m_datas.begin(); iter != m_datas.end(); iter++)
        {
            hr = (o->*fn)(iter->first.c_str(), iter->second);
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
