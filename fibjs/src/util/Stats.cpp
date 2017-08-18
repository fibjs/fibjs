/*
 * Stats.cpp
 *
 *  Created on: Nov 26, 2013
 *      Author: lion
 */

#include "object.h"
#include <Stats.h>

namespace fibjs {

result_t Stats_base::_new(v8::Local<v8::Array> keys,
    obj_ptr<Stats_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<Stats> pStats = new Stats();
    int32_t n = keys->Length();
    int32_t i;
    result_t hr;

    pStats->init(0, n);

    for (i = 0; i < n; i++) {
        hr = pStats->set_key(i, keys->Get(i));
        if (hr < 0)
            return hr;
    }

    retVal = pStats;
    return 0;
}

result_t Stats_base::_new(v8::Local<v8::Array> staticKeys,
    v8::Local<v8::Array> keys, obj_ptr<Stats_base>& retVal,
    v8::Local<v8::Object> This)
{
    obj_ptr<Stats> pStats = new Stats();
    int32_t sn = staticKeys->Length();
    int32_t n = keys->Length();
    int32_t i;
    result_t hr;

    pStats->init(sn, n + sn);

    for (i = 0; i < sn; i++) {
        hr = pStats->set_key(i, staticKeys->Get(i));
        if (hr < 0)
            return hr;
    }

    for (i = 0; i < n; i++) {
        hr = pStats->set_key(sn + i, keys->Get(i));
        if (hr < 0)
            return hr;
    }

    retVal = pStats;
    return 0;
}

void Stats::init(int32_t sn, int32_t n)
{
    m_static = sn;
    m_size = n;

    m_keys.resize(n);
    m_counters.resize(n);

    for (int32_t i = 0; i < m_size; i++)
        m_counters[i] = 0;

    m_date.now();
}

void Stats::set_key(int32_t n, const char* key)
{
    m_keys[n] = key;
}

result_t Stats::set_key(int32_t n, v8::Local<v8::Value> key)
{
    v8::String::Utf8Value str(key);
    const char* p = *str;
    if (p == NULL)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    set_key(n, p);

    return 0;
}

int32_t Stats::find(exlib::string key)
{
    int32_t i;

    for (i = 0; i < m_size; i++)
        if (m_keys[i] == key)
            return i;

    return -1;
}

result_t Stats::inc(exlib::string key)
{
    int32_t i = find(key);

    if (i < 0)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    inc(i);

    return 0;
}

result_t Stats::dec(exlib::string key)
{
    int32_t i = find(key);

    if (i < 0)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    dec(i);

    return 0;
}

result_t Stats::add(exlib::string key, int32_t value)
{
    int32_t i = find(key);

    if (i < 0)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    add(i, value);

    return 0;
}

result_t Stats::reset()
{
    for (int32_t i = m_static; i < m_size; i++)
        m_counters[i] = 0;

    m_date.now();
    return 0;
}

result_t Stats::uptime(int32_t& retVal)
{
    date_t d1;

    d1.now();

    retVal = (int32_t)d1.diff(m_date);
    return 0;
}

result_t Stats::_named_getter(const char* property, int32_t& retVal)
{
    int32_t i = find(property);

    if (i < 0)
        return CALL_RETURN_UNDEFINED;

    retVal = (int32_t)m_counters[i];

    return 0;
}

result_t Stats::_named_enumerator(v8::Local<v8::Array>& retVal)
{
    int32_t i;
    Isolate* isolate = holder();

    retVal = v8::Array::New(isolate->m_isolate);

    for (i = 0; i < m_size; i++)
        retVal->Set(i, isolate->NewFromUtf8(m_keys[i]));

    return 0;
}

} /* namespace fibjs */
