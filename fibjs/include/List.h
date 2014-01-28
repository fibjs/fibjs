/*
 * List.h
 *
 *  Created on: Apr 11, 2012
 *      Author: lion
 */

#include "ifs/List.h"
#include "QuickArray.h"

#ifndef VARIANTLIST_H_
#define VARIANTLIST_H_

namespace fibjs
{

class List: public List_base
{
public:
    // object_base
    virtual result_t toJSON(const char *key, v8::Local<v8::Value> &retVal);

public:
    // ObjectArray_base
    virtual result_t _indexed_getter(uint32_t index, Variant &retVal);
    virtual result_t _indexed_setter(uint32_t index, Variant newVal);
    virtual result_t get_length(int32_t &retVal);
    virtual result_t resize(int32_t sz);
    virtual result_t push(Variant v);
    virtual result_t push(const v8::FunctionCallbackInfo<v8::Value> &args);
    virtual result_t pop(Variant &retVal);
    virtual result_t slice(int32_t start, int32_t end, obj_ptr<List_base> &retVal);
    virtual result_t concat(const v8::FunctionCallbackInfo<v8::Value> &args, obj_ptr<List_base> &retVal);
    virtual result_t every(v8::Local<v8::Function> func, v8::Local<v8::Object> thisp, bool &retVal);
    virtual result_t filter(v8::Local<v8::Function> func, v8::Local<v8::Object> thisp, obj_ptr<List_base> &retVal);
    virtual result_t forEach(v8::Local<v8::Function> func, v8::Local<v8::Object> thisp);
    virtual result_t map(v8::Local<v8::Function> func, v8::Local<v8::Object> thisp, obj_ptr<List_base> &retVal);
    virtual result_t toArray(v8::Local<v8::Array> &retVal);

public:
    class array
    {
    public:
        result_t _indexed_getter(uint32_t index, Variant &retVal);
        result_t _indexed_setter(uint32_t index, Variant newVal);
        result_t get_length(int32_t &retVal);
        result_t resize(int32_t sz);
        result_t push(Variant v);
        result_t push(const v8::FunctionCallbackInfo<v8::Value> &args);
        result_t pop(Variant &retVal);
        result_t slice(int32_t start, int32_t end, obj_ptr<List_base> &retVal);
        result_t concat(const v8::FunctionCallbackInfo<v8::Value> &args, obj_ptr<List_base> &retVal);
        result_t every(v8::Local<v8::Function> func, v8::Local<v8::Object> thisp, bool &retVal);
        result_t filter(v8::Local<v8::Function> func, v8::Local<v8::Object> thisp, obj_ptr<List_base> &retVal);
        result_t forEach(v8::Local<v8::Function> func, v8::Local<v8::Object> thisp);
        result_t map(v8::Local<v8::Function> func, v8::Local<v8::Object> thisp, obj_ptr<List_base> &retVal);
        result_t toArray(v8::Local<v8::Array> &retVal);
        result_t toJSON(const char *key, v8::Local<v8::Value> &retVal);

    public:
        void append(object_base *newVal)
        {
            Variant v;
            v = newVal;
            push(v);
        }

    private:
        v8::Local<v8::Value> _call(v8::Local<v8::Function> func,
                                    v8::Local<v8::Object> thisp, int i);

    private:
        QuickArray<VariantEx> m_array;
    };

public:
    void append(object_base *newVal)
    {
        m_array.append(newVal);
    }

private:
    array m_array;
};

} /* namespace fibjs */
#endif /* VARIANTLIST_H_ */
