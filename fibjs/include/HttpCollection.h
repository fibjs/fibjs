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
    virtual result_t has(const char *name, bool &retVal);
    virtual result_t first(const char *name, Variant &retVal);
    virtual result_t all(const char *name, v8::Local<v8::Array> &retVal);
    virtual result_t add(v8::Local<v8::Object> map);
    virtual result_t add(const char *name, Variant value);
    virtual result_t set(v8::Local<v8::Object> map);
    virtual result_t set(const char *name, Variant value);
    virtual result_t remove(const char *name);
    virtual result_t _named_getter(const char *property, Variant &retVal);
    virtual result_t _named_enumerator(v8::Local<v8::Array> &retVal);
    virtual result_t _named_setter(const char *property, Variant newVal);
    virtual result_t _named_deleter(const char *property, v8::Local<v8::Boolean> &retVal);

public:
    void add(const char *name, int szName, const char *value, int szValue)
    {
        m_names[m_count].assign(name, szName);
        m_values[m_count].assign(value, szValue);
        m_count++;
    }

    void add(std::string &name, std::string &value)
    {
        m_names[m_count].assign(name);
        m_values[m_count].assign(value);
        m_count++;
    }

    size_t size();
    size_t getData(char *buf, size_t sz);

private:
    QuickArray<std::string> m_names;
    QuickArray<std::string> m_values;
    int32_t m_count;
};

} /* namespace fibjs */
#endif /* HTTPCOLLECTION_H_ */
