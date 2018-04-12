/*
 * HsRegExp.h
 *
 *  Created on: Sep 08, 2017
 *      Author: asionius
 */

#include "ifs/HsRegExp.h"
#include <hyperscan/hs.h>
#include "SimpleObject.h"

#ifndef HSREGEXP_H_
#define HSREGEXP_H_

namespace fibjs {

class HsRegExp : public HsRegExp_base {
public:
    HsRegExp()
        : m_database(NULL)
        , m_scratch(NULL)
    {
    }

    ~HsRegExp()
    {
        clear();
    }

    class HsRegRes : public NObject
    {
    public:
        HsRegRes()
            : m_match(false)
        {}
        ~HsRegRes() {}

    public:
        void addOne(uint32_t id, int64_t from, int64_t to)
        {
            for (uint32_t i = 0; i < m_ids.size(); i++)
            {
                if (m_ids[i] == id)
                {
                    obj_ptr<NArray> v = new NArray();
                    v->append(from);
                    v->append(to);
                    obj_ptr<NArray> v1 = (NArray*)m_values[i].object();
                    v1->append(v);
                    return;
                }
            }
            m_ids.push_back(id);
            obj_ptr<NArray> v = new NArray();
            obj_ptr<NArray> v1 = new NArray();
            v1->append(from);
            v1->append(to);
            v->append(v1);
            m_values.push_back(v);
        }

        void addAll(std::vector<exlib::string>& patterns)
        {
            for (uint32_t i = 0; i < m_ids.size(); i++)
                add(patterns[m_ids[i]], m_values[i]);
        }

    public:
        bool m_match;

    private:
        std::vector<uint32_t> m_ids;
        std::vector<Variant> m_values;
    };

public:
    // HsRegExp_base
    virtual result_t scan(Buffer_base* buff, exlib::string codec, v8::Local<v8::Value>& retVal);
    void clear();

public:
    result_t scan(exlib::string text, v8::Local<v8::Value>& retVal);
    static int onMatch(unsigned int id, unsigned long long from, unsigned long long to,
                       unsigned int flags, void *ctx);
    result_t compile(exlib::string pattern, exlib::string flag);
    result_t compile(v8::Local<v8::Array> patterns, v8::Local<v8::Array> flags);
    result_t parseFlags(exlib::string flags, uint32_t& retVal);

private:
    std::vector<exlib::string> m_patterns;
    hs_database_t *m_database;
    hs_scratch_t *m_scratch;
};

} /* namespace fibjs */
#endif /* HSREGEXP_H_ */