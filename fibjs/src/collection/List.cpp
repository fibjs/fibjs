/*
 * List.cpp
 *
 *  Created on: Apr 11, 2012
 *      Author: lion
 */

#include "List.h"

namespace fibjs
{

result_t List_base::_new(obj_ptr<List_base> &retVal, v8::Local<v8::Object> This)
{
    retVal = new List();
    return 0;
}

result_t List::_indexed_getter(uint32_t index, Variant &retVal)
{
    if (index >= m_array.size())
        return CHECK_ERROR(CALL_E_BADINDEX);

    retVal = m_array[index];

    return 0;
}

result_t List::_indexed_setter(uint32_t index, Variant newVal)
{
    if (index >= m_array.size())
        return CHECK_ERROR(CALL_E_BADINDEX);

    if (m_freeze)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (newVal.type() == Variant::VT_JSValue)
        setJSObject();

    m_array[index] = newVal;

    return 0;
}

result_t List::freeze()
{
    m_freeze = true;
    return 0;
}

result_t List::get_length(int32_t &retVal)
{
    retVal = (int32_t) m_array.size();
    return 0;
}

result_t List::resize(int32_t sz)
{
    if (m_freeze)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    m_array.resize(sz);
    return 0;
}

result_t List::push(Variant v)
{
    if (m_freeze)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (v.type() == Variant::VT_JSValue)
        setJSObject();

    m_array.append(v);
    return 0;
}

result_t List::push(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    if (m_freeze)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    int32_t len = args.Length();
    int32_t i;

    for (i = 0; i < len; i++)
        push(args[i]);
    return 0;
}

result_t List::pop(Variant &retVal)
{
    if (!m_array.size())
        return CALL_RETURN_NULL;

    if (m_freeze)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = m_array[m_array.size() - 1];
    m_array.pop();

    return 0;
}

result_t List::slice(int32_t start, int32_t end,
                     obj_ptr<List_base> &retVal)
{
    if (end < 0)
        end = (int32_t)m_array.size();

    retVal = new List();
    while (start < end)
        retVal->push(m_array[start++]);

    return 0;
}

result_t List::concat(const v8::FunctionCallbackInfo<v8::Value> &args,
                      obj_ptr<List_base> &retVal)
{
    if (m_freeze)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    obj_ptr<List> a;
    int32_t i, len;

    a = new List();

    len = (int32_t)m_array.size();
    for (i = 0; i < len; i++)
        a->push(m_array[i]);

    len = args.Length();
    for (i = 0; i < len; i++)
    {
        v8::Local<v8::Value> v = args[i];
        obj_ptr<List_base> a1 = List_base::getInstance(v);

        if (a1)
        {
            int32_t i1, len1;

            a1->get_length(len1);
            for (i1 = 0; i1 < len1; i1++)
            {
                Variant v1;

                a1->_indexed_getter(i1, v1);
                a->push(v1);
            }
        }
        else if (v->IsArray())
        {
            int32_t i1, len1;
            v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast(v);

            len1 = arr->Length();
            for (i1 = 0; i1 < len1; i1++)
                a->push(arr->Get(i1));
        }
        else
            return CHECK_ERROR(CALL_E_INVALIDARG);
    }

    retVal = a;

    return 0;
}

v8::Local<v8::Value> List::_call(v8::Local<v8::Function> func,
                                 v8::Local<v8::Object> thisp, int32_t i)
{
    v8::Local<v8::Value> args[] =
    { m_array[i], v8::Number::New(holder()->m_isolate, i) };

    return func->Call(thisp, 2, args);
}

result_t List::every(v8::Local<v8::Function> func,
                     v8::Local<v8::Object> thisp, bool &retVal)
{
    int32_t i, len;

    len = (int32_t)m_array.size();
    for (i = 0; i < len; i++)
    {
        v8::Local<v8::Value> r = _call(func, thisp, i);

        if (r.IsEmpty())
            return CALL_E_JAVASCRIPT;

        if (!r->BooleanValue())
        {
            retVal = false;
            return 0;
        }
    }

    retVal = true;
    return 0;
}

result_t List::some(v8::Local<v8::Function> func,
                    v8::Local<v8::Object> thisp, bool &retVal)
{
    int32_t i, len;

    len = (int32_t)m_array.size();
    for (i = 0; i < len; i++)
    {
        v8::Local<v8::Value> r = _call(func, thisp, i);

        if (r.IsEmpty())
            return CALL_E_JAVASCRIPT;

        if (r->BooleanValue())
        {
            retVal = true;
            return 0;
        }
    }

    retVal = false;
    return 0;
}

result_t List::filter(v8::Local<v8::Function> func,
                      v8::Local<v8::Object> thisp, obj_ptr<List_base> &retVal)
{
    obj_ptr<List> a;
    int32_t i, len;

    a = new List();

    len = (int32_t)m_array.size();
    for (i = 0; i < len; i++)
    {
        v8::Local<v8::Value> r = _call(func, thisp, i);

        if (r.IsEmpty())
            return CALL_E_JAVASCRIPT;

        if (r->BooleanValue())
            a->push(m_array[i]);
    }

    retVal = a;

    return 0;
}

result_t List::forEach(v8::Local<v8::Function> func,
                       v8::Local<v8::Object> thisp)
{
    int32_t i, len;

    len = (int32_t)m_array.size();
    for (i = 0; i < len; i++)
    {
        v8::Local<v8::Value> r = _call(func, thisp, i);

        if (r.IsEmpty())
            return CALL_E_JAVASCRIPT;
    }

    return 0;
}

result_t List::map(v8::Local<v8::Function> func,
                   v8::Local<v8::Object> thisp, obj_ptr<List_base> &retVal)
{
    obj_ptr<List> a;
    int32_t i, len;

    a = new List();

    len = (int32_t)m_array.size();
    for (i = 0; i < len; i++)
    {
        v8::Local<v8::Value> r = _call(func, thisp, i);

        if (r.IsEmpty())
            return CALL_E_JAVASCRIPT;

        a->push(r);
    }

    retVal = a;

    return 0;
}

result_t List::toArray(v8::Local<v8::Array> &retVal)
{
    v8::Local<v8::Array> a = v8::Array::New(holder()->m_isolate, (int32_t) m_array.size());
    int32_t i;

    for (i = 0; i < (int32_t) m_array.size(); i++)
        a->Set(i, m_array[i]);

    retVal = a;

    return 0;
}

result_t List::toJSON(const char *key, v8::Local<v8::Value> &retVal)
{
    result_t hr;
    v8::Local<v8::Array> as;

    hr = toArray(as);
    if (hr < 0)
        return hr;

    retVal = as;
    return 0;
}

} /* namespace fibjs */
