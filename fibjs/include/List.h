/*
 * List.h
 *
 *  Created on: Apr 11, 2012
 *      Author: lion
 */

#include "ifs/List.h"
#include <vector>

#ifndef VARIANTLIST_H_
#define VARIANTLIST_H_

namespace fibjs
{

class List: public List_base
{
public:
    List(): m_freeze(false)
    {}

public:
    // object_base
    virtual result_t toJSON(exlib::string key, v8::Local<v8::Value> &retVal);

public:
    // List_base
    virtual result_t _indexed_getter(uint32_t index, Variant &retVal);
    virtual result_t _indexed_setter(uint32_t index, Variant newVal);
    virtual result_t freeze();
    virtual result_t get_length(int32_t &retVal);
    virtual result_t resize(int32_t sz);
    virtual result_t push(Variant v);
    virtual result_t push(const v8::FunctionCallbackInfo<v8::Value> &args);
    virtual result_t pushArray(v8::Local<v8::Array> data);
    virtual result_t pop(Variant &retVal);
    virtual result_t slice(int32_t start, int32_t end, obj_ptr<List_base> &retVal);
    virtual result_t concat(const v8::FunctionCallbackInfo<v8::Value> &args, obj_ptr<List_base> &retVal);
    virtual result_t every(v8::Local<v8::Function> func, v8::Local<v8::Value> thisArg, bool &retVal);
    virtual result_t some(v8::Local<v8::Function> func, v8::Local<v8::Value> thisArg, bool &retVal);
    virtual result_t filter(v8::Local<v8::Function> func, v8::Local<v8::Value> thisArg, obj_ptr<List_base> &retVal);
    virtual result_t forEach(v8::Local<v8::Function> func, v8::Local<v8::Value> thisArg);
    virtual result_t map(v8::Local<v8::Function> func, v8::Local<v8::Value> thisArg, obj_ptr<List_base> &retVal);
    virtual result_t sort(v8::Local<v8::Function> func, obj_ptr<List_base>& retVal);
    virtual result_t sort(obj_ptr<List_base>& retVal);
    virtual result_t toArray(v8::Local<v8::Array> &retVal);


public:
    void append(object_base *newVal)
    {
        Variant v;
        v = newVal;
        push(v);
    }

    void append(Variant newVal)
    {
        push(newVal);
    }

private:
    v8::Local<v8::Value> _call(v8::Local<v8::Function> func,
                               v8::Local<v8::Value> thisArg, int32_t i);

private:
    std::vector<VariantEx> m_array;
    bool m_freeze;
};

} /* namespace fibjs */
#endif /* VARIANTLIST_H_ */
