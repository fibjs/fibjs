/*
 * DBResult.cpp
 *
 *  Created on: Jul 25, 2012
 *      Author: lion
 */

#include "object.h"
#include "DBResult.h"

namespace fibjs {

result_t DBResult::_indexed_getter(uint32_t index, Variant& retVal)
{
    if (!m_size)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    return m_array->_indexed_getter(index, retVal);
}

result_t DBResult::_indexed_setter(uint32_t index, Variant newVal)
{
    if (!m_size)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    return m_array->_indexed_setter(index, newVal);
}

result_t DBResult::freeze()
{
    return m_array->freeze();
}

result_t DBResult::get_length(int32_t& retVal)
{
    if (!m_size) {
        retVal = 0;
        return 0;
    }

    return m_array->get_length(retVal);
}

result_t DBResult::resize(int32_t sz)
{
    if (!m_size)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    return m_array->resize(sz);
}

result_t DBResult::indexOf(Variant searchElement, int32_t fromIndex, int32_t& retVal)
{
    if (!m_size)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    return m_array->indexOf(searchElement, fromIndex, retVal);
}

result_t DBResult::lastIndexOf(Variant searchElement, int32_t fromIndex, int32_t& retVal)
{
    if (!m_size)
        return CHECK_ERROR(CALL_E_INVALID_CALL);
    return m_array->lastIndexOf(searchElement, fromIndex, retVal);
}

result_t DBResult::push(Variant v, int32_t& retVal)
{
    if (!m_size)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    return m_array->push(v, retVal);
}

result_t DBResult::push(std::vector<v8::Local<v8::Value>>& els, int32_t& retVal)
{
    if (!m_size)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    return m_array->push(els, retVal);
}

result_t DBResult::pushArray(v8::Local<v8::Array> data)
{
    if (!m_size)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    return m_array->pushArray(data);
}

result_t DBResult::pop(Variant& retVal)
{
    if (!m_size)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    return m_array->pop(retVal);
}

result_t DBResult::slice(int32_t start, int32_t end, obj_ptr<List_base>& retVal)
{
    if (!m_size)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    return m_array->slice(start, end, retVal);
}

result_t DBResult::concat(std::vector<v8::Local<v8::Value>>& lists, obj_ptr<List_base>& retVal)
{
    if (!m_size)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    return m_array->concat(lists, retVal);
}

result_t DBResult::every(v8::Local<v8::Function> func,
    v8::Local<v8::Value> thisArg, bool& retVal)
{
    if (!m_size)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    return m_array->every(func, thisArg, retVal);
}

result_t DBResult::some(v8::Local<v8::Function> func,
    v8::Local<v8::Value> thisArg, bool& retVal)
{
    if (!m_size)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    return m_array->some(func, thisArg, retVal);
}

result_t DBResult::filter(v8::Local<v8::Function> func,
    v8::Local<v8::Value> thisArg, obj_ptr<List_base>& retVal)
{
    if (!m_size)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    return m_array->filter(func, thisArg, retVal);
}

result_t DBResult::forEach(v8::Local<v8::Function> func,
    v8::Local<v8::Value> thisArg)
{
    if (!m_size)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    return m_array->forEach(func, thisArg);
}

result_t DBResult::map(v8::Local<v8::Function> func,
    v8::Local<v8::Value> thisArg, obj_ptr<List_base>& retVal)
{
    if (!m_size)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    return m_array->map(func, thisArg, retVal);
}

result_t DBResult::reduce(v8::Local<v8::Function> func, v8::Local<v8::Value> initVal,
    v8::Local<v8::Value>& retVal)
{
    if (!m_size)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    return m_array->reduce(func, initVal, retVal);
}

result_t DBResult::sort(v8::Local<v8::Function> func, obj_ptr<List_base>& retVal)
{
    if (!m_size)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    result_t hr = m_array->sort(func, retVal);
    if (hr < 0)
        return hr;

    retVal = this;
    return 0;
}

result_t DBResult::sort(obj_ptr<List_base>& retVal)
{
    if (!m_size)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    result_t hr = m_array->sort(retVal);
    if (hr < 0)
        return hr;

    retVal = this;
    return 0;
}

result_t DBResult::toArray(v8::Local<v8::Array>& retVal)
{
    return m_array->toArray(retVal);
}

result_t DBResult::toJSON(exlib::string key, v8::Local<v8::Value>& retVal)
{
    if (m_size)
        return m_array->toJSON(key, retVal);

    Isolate* isolate = holder();
    v8::Local<v8::Object> o = v8::Object::New(isolate->m_isolate);

    o->Set(isolate->NewFromUtf8("affected", 8),
        v8::Number::New(isolate->m_isolate, (double)m_affected));
    o->Set(isolate->NewFromUtf8("insertId", 8),
        v8::Number::New(isolate->m_isolate, (double)m_insertId));

    retVal = o;

    return 0;
}

result_t DBResult::get_insertId(int64_t& retVal)
{
    retVal = m_insertId;
    return 0;
}

result_t DBResult::get_affected(int64_t& retVal)
{
    retVal = m_affected;
    return 0;
}

result_t DBResult::get_fields(v8::Local<v8::Array>& retVal)
{
    if (!m_size)
        retVal = v8::Array::New(holder()->m_isolate);
    else
        m_fields->names(holder()->m_isolate, retVal);

    return 0;
}

} /* namespace fibjs */
