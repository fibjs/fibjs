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

namespace fibjs
{

class HttpCollection: public HttpCollection_base
{
public:
    HttpCollection() :
        m_count(0)
    {
    }

public:
    // HttpCollection_base
    virtual result_t clear();
    virtual result_t has(exlib::string name, bool &retVal);
    virtual result_t first(exlib::string name, Variant &retVal);
    virtual result_t all(exlib::string name, obj_ptr<List_base> &retVal);
    virtual result_t add(Map_base* map);
    virtual result_t add(exlib::string name, Variant value);
    virtual result_t set(Map_base* map);
    virtual result_t set(exlib::string name, Variant value);
    virtual result_t remove(exlib::string name);
    virtual result_t _named_getter(const char *property, Variant &retVal);
    virtual result_t _named_enumerator(v8::Local<v8::Array> &retVal);
    virtual result_t _named_setter(const char *property, Variant newVal);
    virtual result_t _named_deleter(const char *property, v8::Local<v8::Boolean> &retVal);

public:
    void add(const char *name, int32_t szName, const char *value, int32_t szValue)
    {
        m_names[m_count].assign(name, szName);
        m_values[m_count].assign(value, szValue);
        m_count++;
    }

    void add(exlib::string &name, exlib::string &value)
    {
        m_names[m_count].assign(name);
        m_values[m_count].assign(value);
        m_count++;
    }

    size_t size();
    size_t getData(char *buf, size_t sz);

    result_t parse(exlib::string &str, const char* sep = "&", const char* eq = "=");
    result_t parseCookie(exlib::string &str);

private:
    QuickArray<exlib::string> m_names;
    QuickArray<exlib::string> m_values;
    int32_t m_count;
};

} /* namespace fibjs */
#endif /* HTTPCOLLECTION_H_ */
