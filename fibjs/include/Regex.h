/*
 * Regex.h
 *
 *  Created on: Aug 18, 2012
 *      Author: lion
 */

#include "ifs/Regex.h"
#include <pcre/pcre.h>

#ifndef REGEX_H_
#define REGEX_H_

namespace fibjs
{

class Regex: public Regex_base
{
public:
    Regex() :
        m_re(NULL)
    {
        reset();
    }

    virtual ~Regex()
    {
        reset();
    }

public:
    // Regex_base
    virtual result_t exec(exlib::string str, v8::Local<v8::Array> &retVal);
    virtual result_t test(exlib::string str, bool &retVal);
    virtual result_t get_lastIndex(int32_t &retVal);
    virtual result_t set_lastIndex(int32_t newVal);
    virtual result_t get_global(bool &retVal);
    virtual result_t get_ignoreCase(bool &retVal);
    virtual result_t get_multiline(bool &retVal);

public:
    result_t compile(const char *pattern, const char *opt);

private:
    void reset()
    {
        m_bGlobal = false;
        m_bIgnoreCase = false;
        m_bMultiline = false;
        m_nlastIndex = 0;
        if (m_re)
            pcre_free(m_re);
    }

private:
    bool m_bGlobal;
    bool m_bIgnoreCase;
    bool m_bMultiline;
    int32_t m_nlastIndex;
    pcre *m_re;
};

} /* namespace fibjs */
#endif /* REGEX_H_ */
