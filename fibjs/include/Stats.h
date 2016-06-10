/*
 * Stats.h
 *
 *  Created on: Nov 26, 2013
 *      Author: lion
 */

#ifndef STATS_H_
#define STATS_H_

#include "ifs/Stats.h"
#include "QuickArray.h"

namespace fibjs
{

class Stats: public Stats_base
{
public:
    // Stats_base
    virtual result_t inc(const char *key);
    virtual result_t dec(const char *key);
    virtual result_t add(const char *key, int32_t value);
    virtual result_t reset();
    virtual result_t uptime(int32_t &retVal);
    virtual result_t _named_getter(const char *property, int32_t &retVal);
    virtual result_t _named_enumerator(v8::Local<v8::Array> &retVal);

public:
    void inc(int32_t n)
    {
        m_counters[n]++;
    }

    void dec(int32_t n)
    {
        m_counters[n]--;
    }

    void add(int32_t n, int32_t value)
    {
        m_counters[n] += value;
    }

public:
    void init(int32_t sn, int32_t n);
    void set_key(int32_t n, const char *key);
    result_t set_key(int32_t n, v8::Local<v8::Value> key);

    void init(const char **staticKeys, int32_t sn, const char **keys, int32_t n)
    {
        int32_t i;

        init(sn, sn + n);

        for (i = 0; i < sn; i++)
            set_key(i, staticKeys[i]);

        for (i = 0; i < n; i++)
            set_key(sn + i, keys[i]);
    }

    void init(const char **keys, int32_t n)
    {
        init(NULL, 0, keys, n);
    }

private:
    int32_t find(const char *key);

private:
    int32_t m_static, m_size;
    std::vector<std::string> m_keys;
    QuickArray<std::atomic_intptr_t> m_counters;
    date_t m_date;
};

} /* namespace fibjs */

#endif /* STATS_H_ */
