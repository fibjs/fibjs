/*
 * Stats.cpp
 *
 *  Created on: Nov 26, 2013
 *      Author: lion
 */

#include <Stats.h>

namespace fibjs
{

result_t Stats_base::_new(v8::Local<v8::Array> keys,
                          obj_ptr<Stats_base> &retVal)
{
    obj_ptr<Stats> pStats = new Stats();
    int n = keys->Length();
    int i;
    result_t hr;

    pStats->init(0, n);

    for (i = 0; i < n; i++)
    {
        hr = pStats->set_key(i, keys->Get(i));
        if (hr < 0)
            return hr;
    }

    retVal = pStats;
    return 0;
}

result_t Stats_base::_new(v8::Local<v8::Array> staticKeys,
                          v8::Local<v8::Array> keys, obj_ptr<Stats_base> &retVal)
{
    obj_ptr<Stats> pStats = new Stats();
    int sn = staticKeys->Length();
    int n = keys->Length();
    int i;
    result_t hr;

    pStats->init(sn, n + sn);

    for (i = 0; i < sn; i++)
    {
        hr = pStats->set_key(i, staticKeys->Get(i));
        if (hr < 0)
            return hr;
    }

    for (i = 0; i < n; i++)
    {
        hr = pStats->set_key(sn + i, keys->Get(i));
        if (hr < 0)
            return hr;
    }

    retVal = pStats;
    return 0;
}

void Stats::init(int sn, int n)
{
    m_static = sn;
    m_size = n;

    m_keys.resize(n);
    m_counters.resize(n);

    for (int i = 0; i < m_size; i++)
        m_counters[i] = 0;

    m_date.now();
}

void Stats::set_key(int n, const char *key)
{
    m_keys[n] = key;
}

result_t Stats::set_key(int n, v8::Local<v8::Value> key)
{
    v8::String::Utf8Value str(key);
    const char *p = *str;
    if (p == NULL)
        return CALL_E_INVALIDARG;

    set_key(n, p);

    return 0;
}

int Stats::find(const char *key)
{
    int i;

    for (i = 0; i < m_size; i++)
        if (!qstrcmp(key, m_keys[i].c_str()))
            return i;

    return -1;
}

result_t Stats::inc(const char *key)
{
    int i = find(key);

    if (i < 0)
        return CALL_E_INVALIDARG;

    inc(i);

    return 0;
}

result_t Stats::dec(const char *key)
{
    int i = find(key);

    if (i < 0)
        return CALL_E_INVALIDARG;

    dec(i);

    return 0;
}

result_t Stats::add(const char *key, int32_t value)
{
    int i = find(key);

    if (i < 0)
        return CALL_E_INVALIDARG;

    add(i, value);

    return 0;
}

result_t Stats::reset()
{
    for (int i = m_static; i < m_size; i++)
        m_counters[i] = 0;

    m_date.now();
    return 0;
}

result_t Stats::uptime(int32_t &retVal)
{
    date_t d1;

    d1.now();

    retVal = (int)d1.diff(m_date);
    return 0;
}

result_t Stats::_named_getter(const char *property, int32_t &retVal)
{
    int i = find(property);

    if (i < 0)
        return CALL_RETURN_NULL;

    retVal = m_counters[i];

    return 0;
}

result_t Stats::_named_enumerator(v8::Local<v8::Array> &retVal)
{
    int i;

    retVal = v8::Array::New(isolate);

    for (i = 0; i < m_size; i++)
        retVal->Set(i,
                    v8::String::NewFromUtf8(isolate, m_keys[i].c_str(),
                                            v8::String::kNormalString,
                                            (int) m_keys[i].length()));

    return 0;
}

} /* namespace fibjs */
