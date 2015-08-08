/*
 * Regex.cpp
 *
 *  Created on: Aug 18, 2012
 *      Author: lion
 */

#include "utf8.h"
#include "Regex.h"
#include "ifs/re.h"

namespace fibjs
{

result_t re_base::compile(const char *pattern, const char *opt,
                          obj_ptr<Regex_base> &retVal)
{
    obj_ptr<Regex> re = new Regex();
    result_t hr = re->compile(pattern, opt);
    if (hr < 0)
        return hr;

    retVal = re;
    return 0;
}

result_t Regex::compile(const char *pattern, const char *opt)
{
    int32_t o = PCRE_JAVASCRIPT_COMPAT | PCRE_EXTRA | PCRE_UTF8
                | PCRE_NEWLINE_ANYCRLF | PCRE_UCP;
    const char *error;
    int32_t erroffset;
    char ch;

    reset();

    while ((ch = *opt++) != 0)
    {
        switch (ch)
        {
        case 'g':
            m_bGlobal = true;
            break;
        case 'i':
            m_bIgnoreCase = true;
            break;
        case 'm':
            m_bMultiline = true;
            break;
        }
    }

    if (m_bMultiline)
        o |= PCRE_MULTILINE;
    if (m_bIgnoreCase)
        o |= PCRE_CASELESS;

    m_re = pcre_compile(pattern, o, &error, &erroffset, NULL);

    if (m_re == NULL)
    {
        char buf[1024];

        sprintf(buf, "Regex: Compilation failed at offset %d: %s.", erroffset, error);
        return CHECK_ERROR(Runtime::setError(buf));
    }

    return 0;
}

#define RE_SIZE 32
result_t Regex::exec(const char *str, v8::Local<v8::Array> &retVal)
{
    int32_t rc = 0;
    int32_t ovector[RE_SIZE];
    int32_t len = (int32_t) qstrlen(str);
    const char *end = str + len;
    int32_t i;

    if (m_bGlobal)
    {
        int32_t n = m_nlastIndex;
        while (n > 0 && utf8_getchar(str, end))
            n--;
    }

    if (*str)
    {
        len = (int32_t) qstrlen(str);

        rc = pcre_exec(m_re, NULL, str, len, 0, 0, ovector, RE_SIZE);
        if (rc < 0)
        {
            rc = 0;
            m_nlastIndex = 0;
        }
    }

    if (rc)
    {
        Isolate* isolate = Isolate::now();
        retVal = v8::Array::New(isolate->isolate, rc);

        for (i = 0; i < rc; i++)
            retVal->Set(i,
                        v8::String::NewFromUtf8(isolate->isolate, str + ovector[2 * i],
                                                v8::String::kNormalString,
                                                ovector[2 * i + 1] - ovector[2 * i]));

        if (m_bGlobal)
            m_nlastIndex += utf8_strlen(str, ovector[2 * rc - 1]);
    }

    return rc ? 0 : CALL_RETURN_NULL;
}

result_t Regex::test(const char *str, bool &retVal)
{
    int32_t rc = 0;
    int32_t ovector[RE_SIZE];
    int32_t len = (int32_t) qstrlen(str);
    const char *end = str + len;

    if (m_bGlobal)
    {
        int32_t n = m_nlastIndex;
        while (n > 0 && utf8_getchar(str, end))
            n--;
    }

    if (*str)
    {
        len = (int32_t) qstrlen(str);

        rc = pcre_exec(m_re, NULL, str, len, 0, 0, ovector, RE_SIZE);
        if (rc < 0)
        {
            rc = 0;
            m_nlastIndex = 0;
        }
    }

    if (rc)
    {
        retVal = true;
        if (m_bGlobal)
            m_nlastIndex += utf8_strlen(str, ovector[2 * rc - 1]);
    }

    return 0;
}

result_t Regex::get_lastIndex(int32_t &retVal)
{
    retVal = m_nlastIndex;
    return 0;
}

result_t Regex::set_lastIndex(int32_t newVal)
{
    m_nlastIndex = newVal;
    return 0;
}

result_t Regex::get_global(bool &retVal)
{
    retVal = m_bGlobal;
    return 0;
}

result_t Regex::get_ignoreCase(bool &retVal)
{
    retVal = m_bIgnoreCase;
    return 0;
}

result_t Regex::get_multiline(bool &retVal)
{
    retVal = m_bMultiline;
    return 0;
}

} /* namespace fibjs */
