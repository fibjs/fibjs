/*
 * List.cpp
 *
 *  Created on: Apr 11, 2012
 *      Author: lion
 */

#include "object.h"
#include "ifs/collection.h"
#include "List.h"
#include <algorithm>

namespace fibjs {

DECLARE_MODULE(collection);

result_t List_base::_new(obj_ptr<List_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new List();
    return 0;
}

result_t List_base::_new(v8::Local<v8::Array> data, obj_ptr<List_base>& retVal,
    v8::Local<v8::Object> This)
{
    retVal = new List();
    return retVal->pushArray(data);
}

result_t List::_indexed_getter(uint32_t index, Variant& retVal)
{
    if (index < 0 || index >= m_array.size())
        return CHECK_ERROR(CALL_RETURN_NULL);

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

result_t List::get_length(int32_t& retVal)
{
    retVal = (int32_t)m_array.size();
    return 0;
}

result_t List::resize(int32_t sz)
{
    if (m_freeze)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    m_array.resize(sz);
    return 0;
}

result_t List::push(Variant v, int32_t& retVal)
{
    if (m_freeze)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (v.type() == Variant::VT_JSValue)
        setJSObject();

    m_array.push_back(v);

    retVal = (int32_t)m_array.size();
    return 0;
}

result_t List::indexOf(Variant searchElement, int32_t fromIndex, int32_t& retVal)
{
    int32_t len = (int32_t)m_array.size();
    int32_t k;

    if (fromIndex >= 0) {
        k = fromIndex;
    } else {
        k = len + fromIndex;
        if (k < 0)
            k = 0;
    }
    retVal = -1;
    while (k < len) {
        if (m_array[k].strictEqual(searchElement)) {
            retVal = k;
            return 0;
        }
        k++;
    }

    return 0;
}

result_t List::lastIndexOf(Variant searchElement, int32_t fromIndex, int32_t& retVal)
{
    int32_t k, len;
    len = (int32_t)m_array.size();
    retVal = -1;

    if (len == 0)
        return 0;
    if (!fromIndex)
        k = len - 1;
    else if (fromIndex > 0)
        k = std::min(fromIndex, len - 1);
    else
        k = len + fromIndex;

    while (k >= 0) {
        if (m_array[k].strictEqual(searchElement)) {
            retVal = k;
            return 0;
        }
        k--;
    }

    return 0;
}

result_t List::push(OptArgs els, int32_t& retVal)
{
    if (m_freeze)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    int32_t len = els.Length();
    int32_t i;

    for (i = 0; i < len; i++)
        append(els[i]);

    retVal = (int32_t)m_array.size();
    return 0;
}

result_t List::pushArray(v8::Local<v8::Array> data)
{
    if (m_freeze)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    int32_t len = data->Length();
    int32_t i;

    for (i = 0; i < len; i++)
        append(data->Get(i));
    return 0;
}

result_t List::pop(Variant& retVal)
{
    if (!m_array.size())
        return CALL_RETURN_NULL;

    if (m_freeze)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = m_array[m_array.size() - 1];
    m_array.pop_back();

    return 0;
}

result_t List::slice(int32_t start, int32_t end, obj_ptr<List_base>& retVal)
{
    if (end < 0)
        end = (int32_t)m_array.size();

    obj_ptr<List> l = new List();
    while (start < end)
        l->append(m_array[start++]);

    retVal = l;

    return 0;
}

result_t List::concat(OptArgs lists, obj_ptr<List_base>& retVal)
{
    if (m_freeze)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    obj_ptr<List> a;
    int32_t i, len;

    a = new List();

    len = (int32_t)m_array.size();
    for (i = 0; i < len; i++)
        a->append(m_array[i]);

    len = lists.Length();
    for (i = 0; i < len; i++) {
        v8::Local<v8::Value> v = lists[i];
        obj_ptr<List_base> a1 = List_base::getInstance(v);

        if (a1) {
            int32_t i1, len1;

            a1->get_length(len1);
            for (i1 = 0; i1 < len1; i1++) {
                Variant v1;

                a1->_indexed_getter(i1, v1);
                a->append(v1);
            }
        } else if (v->IsArray()) {
            int32_t i1, len1;
            v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast(v);

            len1 = arr->Length();
            for (i1 = 0; i1 < len1; i1++)
                a->append(arr->Get(i1));
        } else
            return CHECK_ERROR(CALL_E_INVALIDARG);
    }

    retVal = a;

    return 0;
}

v8::Local<v8::Value> List::_call(v8::Local<v8::Function> func,
    v8::Local<v8::Value> thisArg, int32_t i)
{
    v8::Local<v8::Value> args[] = { m_array[i], v8::Number::New(holder()->m_isolate, i), wrap() };

    return func->Call(thisArg, 3, args);
}

result_t List::every(v8::Local<v8::Function> func,
    v8::Local<v8::Value> thisArg, bool& retVal)
{
    int32_t i, len;

    len = (int32_t)m_array.size();
    for (i = 0; i < len; i++) {
        v8::Local<v8::Value> r = _call(func, thisArg, i);

        if (r.IsEmpty())
            return CALL_E_JAVASCRIPT;

        if (!r->BooleanValue()) {
            retVal = false;
            return 0;
        }
    }

    retVal = true;
    return 0;
}

result_t List::some(v8::Local<v8::Function> func,
    v8::Local<v8::Value> thisArg, bool& retVal)
{
    int32_t i, len;

    len = (int32_t)m_array.size();
    for (i = 0; i < len; i++) {
        v8::Local<v8::Value> r = _call(func, thisArg, i);

        if (r.IsEmpty())
            return CALL_E_JAVASCRIPT;

        if (r->BooleanValue()) {
            retVal = true;
            return 0;
        }
    }

    retVal = false;
    return 0;
}

result_t List::filter(v8::Local<v8::Function> func,
    v8::Local<v8::Value> thisArg, obj_ptr<List_base>& retVal)
{
    obj_ptr<List> a;
    int32_t i, len;

    a = new List();

    len = (int32_t)m_array.size();
    for (i = 0; i < len; i++) {
        v8::Local<v8::Value> r = _call(func, thisArg, i);

        if (r.IsEmpty())
            return CALL_E_JAVASCRIPT;

        if (r->BooleanValue())
            a->append(m_array[i]);
    }

    retVal = a;

    return 0;
}

result_t List::forEach(v8::Local<v8::Function> func,
    v8::Local<v8::Value> thisArg)
{
    int32_t i, len;

    len = (int32_t)m_array.size();
    for (i = 0; i < len; i++) {
        v8::Local<v8::Value> r = _call(func, thisArg, i);

        if (r.IsEmpty())
            return CALL_E_JAVASCRIPT;
    }

    return 0;
}

result_t List::map(v8::Local<v8::Function> func,
    v8::Local<v8::Value> thisArg, obj_ptr<List_base>& retVal)
{
    obj_ptr<List> a;
    int32_t i, len;

    a = new List();

    len = (int32_t)m_array.size();
    for (i = 0; i < len; i++) {
        v8::Local<v8::Value> r = _call(func, thisArg, i);

        if (r.IsEmpty())
            return CALL_E_JAVASCRIPT;

        a->append(r);
    }

    retVal = a;

    return 0;
}

result_t List::reduce(v8::Local<v8::Function> func, v8::Local<v8::Value> initVal,
    v8::Local<v8::Value>& retVal)
{
    Isolate* isolate = holder();
    int32_t i, len;

    len = (int32_t)m_array.size();
    for (i = 0; i < len; i++) {
        v8::Local<v8::Value> args[] = { initVal, m_array[i], v8::Number::New(isolate->m_isolate, i), wrap() };

        initVal = func->Call(wrap(), 4, args);
        if (initVal.IsEmpty())
            return CALL_E_JAVASCRIPT;
    }

    retVal = initVal;
    return 0;
}

result_t List::sort(v8::Local<v8::Function> func, obj_ptr<List_base>& retVal)
{
    struct MyCompare {
    public:
        MyCompare(v8::Local<v8::Function> func)
            : m_func(func)
        {
        }

        bool operator()(const VariantEx& v1, const VariantEx& v2)
        {
            v8::Local<v8::Value> args[] = { v1, v2 };

            v8::Local<v8::Value> r = m_func->Call(m_func, 2, args);
            if (r.IsEmpty())
                return false;

            return r->NumberValue() < 0;
        }

    private:
        v8::Local<v8::Function> m_func;
    };

    if (m_freeze)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    MyCompare cmp(func);

    std::sort(m_array.begin(), m_array.end(), cmp);

    retVal = this;
    return 0;
}

inline bool compare1(const VariantEx& v1, const VariantEx& v2)
{
    exlib::string s1, s2;

    v1.toString(s1);
    v2.toString(s2);

    return s1.compare(s2) < 0;
}

result_t List::sort(obj_ptr<List_base>& retVal)
{
    if (m_freeze)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    std::sort(m_array.begin(), m_array.end(), compare1);

    retVal = this;
    return 0;
}

result_t List::toArray(v8::Local<v8::Array>& retVal)
{
    v8::Local<v8::Array> a = v8::Array::New(holder()->m_isolate, (int32_t)m_array.size());
    int32_t i;

    for (i = 0; i < (int32_t)m_array.size(); i++)
        a->Set(i, m_array[i]);

    retVal = a;

    return 0;
}

result_t List::toJSON(exlib::string key, v8::Local<v8::Value>& retVal)
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
