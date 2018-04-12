/*
 * HsRegExp.cpp
 *
 *  Created on: Sep 08, 2017
 *      Author: asionius
 */

#include "HsRegExp.h"
#include "Buffer.h"

#ifndef _WIN32
#include <dlfcn.h>
#endif

namespace fibjs {

#ifndef _WIN32

static hs_error_t (*_hs_alloc_scratch)(const hs_database_t *, hs_scratch_t **);
static hs_error_t (*_hs_scan)(const hs_database_t *, const char *,
                              unsigned int, unsigned int,
                              hs_scratch_t *, match_event_handler,
                              void *);
static hs_error_t (*_hs_free_compile_error)(hs_compile_error_t *);
static hs_error_t (*_hs_compile)(const char *, unsigned int,
                                 unsigned int, const hs_platform_info_t *,
                                 hs_database_t **, hs_compile_error_t **);
static hs_error_t (*_hs_compile_multi)(const char *const *,
                                       const unsigned int *, const unsigned int *,
                                       unsigned int, unsigned int,
                                       const hs_platform_info_t *,
                                       hs_database_t **, hs_compile_error_t **);
static hs_error_t (*_hs_free_database)(hs_database_t *);
static hs_error_t (*_hs_free_scratch)(hs_scratch_t *);

static inline int32_t load_libhyperscan()
{
    static void* s_handle = NULL;
    static bool b_init = false;
    if (!b_init)
        b_init = true;

#if defined(Darwin)
    s_handle = dlopen("libhs.dylib", RTLD_LAZY);
#else
    s_handle = dlopen("libhs.so", RTLD_LAZY);
#endif

    if (!s_handle)
        return CHECK_ERROR(Runtime::setError("Cannot load library: 'libhs.so'"));

    _hs_alloc_scratch = (hs_error_t(*)(const hs_database_t *, hs_scratch_t **))dlsym(s_handle, "hs_alloc_scratch");
    _hs_scan = (hs_error_t(*)(const hs_database_t *, const char *,
                              unsigned int, unsigned int,
                              hs_scratch_t *, match_event_handler,
                              void *))dlsym(s_handle, "hs_scan");
    _hs_free_compile_error = (hs_error_t(*)(hs_compile_error_t *))dlsym(s_handle, "hs_free_compile_error");
    _hs_compile = (hs_error_t(*)(const char *, unsigned int,
                                 unsigned int, const hs_platform_info_t *,
                                 hs_database_t **, hs_compile_error_t **))dlsym(s_handle, "hs_compile");
    _hs_compile_multi = (hs_error_t(*)(const char *const *,
                                       const unsigned int *, const unsigned int *,
                                       unsigned int, unsigned int,
                                       const hs_platform_info_t *,
                                       hs_database_t **, hs_compile_error_t **))dlsym(s_handle, "hs_compile_multi");
    _hs_free_database = (hs_error_t(*)(hs_database_t *))dlsym(s_handle, "hs_free_database");
    _hs_free_scratch = (hs_error_t(*)(hs_scratch_t *))dlsym(s_handle, "hs_free_scratch");

    if (!_hs_alloc_scratch
            || !_hs_scan
            || !_hs_free_compile_error
            || !_hs_compile
            || !_hs_compile_multi
            || !_hs_free_database
            || !_hs_free_scratch)
        return CHECK_ERROR(Runtime::setError("Symbol not found"));

    return 0;
}

int HsRegExp::onMatch(unsigned int id, unsigned long long from, unsigned long long to,
                      unsigned int flags, void *ctx)
{
    HsRegRes *res = (HsRegRes*)ctx;
    if (!res->m_match) res->m_match = true;
    res->addOne(id, (int64_t)from, (int64_t)to);
    return 0;
}

result_t HsRegExp::scan(exlib::string text, v8::Local<v8::Value>& retVal)
{
    exlib::string errMsg;

    if (!m_scratch)
    {
        hs_error_t err = _hs_alloc_scratch(m_database, &m_scratch);
        if (err != HS_SUCCESS) {
            errMsg += "Could not allocate scratch space";
            return CHECK_ERROR(Runtime::setError(errMsg));
        }
    }
    HsRegRes *res = new HsRegRes();
    if (_hs_scan(m_database, text.c_str(), text.length(), 0,
                 m_scratch, onMatch, res) != HS_SUCCESS) {
        errMsg = "Unable to scan text";
        delete res;
        return CHECK_ERROR(Runtime::setError(errMsg));
    }
    if (res->m_match)
    {
        res->addAll(m_patterns);
        res->valueOf(retVal);
    }
    else
        retVal = v8::Null(Isolate::current()->m_isolate);

    delete res;
    return 0;
}

result_t HsRegExp::scan(Buffer_base* buff, exlib::string codec, v8::Local<v8::Value>& retVal)
{

    exlib::string str;
    result_t hr;

    hr = buff->toString(codec, 0, str);
    if (hr < 0)
        return CHECK_ERROR(hr);

    return scan(str, retVal);
}

result_t HsRegExp::compile(exlib::string pattern, exlib::string flag)
{
    uint32_t flags;
    hs_compile_error_t *compile_err;
    result_t hr;

    hr = load_libhyperscan();
    if (hr < 0)
        return CHECK_ERROR(hr);

    hr = parseFlags(flag, flags);
    if (hr < 0)
        return CHECK_ERROR(hr);

    m_patterns.push_back(pattern);
    if (_hs_compile(pattern.c_str(), flags, HS_MODE_BLOCK, NULL, &m_database,
                    &compile_err) != HS_SUCCESS) {
        exlib::string errMsg = "Unable to compile pattern \"";
        errMsg += pattern;
        errMsg += "\": ";
        errMsg += compile_err->message;
        _hs_free_compile_error(compile_err);
        return CHECK_ERROR(Runtime::setError(errMsg));
    }

    return 0;
}

result_t HsRegExp::compile(v8::Local<v8::Array> patterns, v8::Local<v8::Array> flags)
{
    result_t hr;
    int32_t patternsSz = patterns->Length();
    int32_t flagsSz = flags->Length();
    uint32_t flag;

    hr = load_libhyperscan();
    if (hr < 0)
        return CHECK_ERROR(hr);

    if (!patternsSz)
        return CHECK_ERROR(Runtime::setError("patterns length should be larger than zero"));

    Isolate* isolate = Isolate::current();
    std::vector<const char *> cstrPatterns;
    std::vector<uint32_t> nFlags;
    std::vector<uint32_t> ids;
    exlib::string pstr;

    for (int32_t i = 0; i < patternsSz; i++) {
        v8::Local<v8::Value> pv = patterns->Get(i);
        if (i < flagsSz) {
            v8::Local<v8::Value> fv = flags->Get(i);
            exlib::string fstr;
            hr = GetArgumentValue(isolate->m_isolate, fv, fstr);
            if (hr < 0)
                return CHECK_ERROR(hr);
            hr = parseFlags(fstr, flag);
            if (hr < 0)
                return CHECK_ERROR(hr);
            nFlags.push_back(flag);
        }
        else
            nFlags.push_back(0);

        hr = GetArgumentValue(isolate->m_isolate, pv, pstr);
        if (hr < 0)
            return CHECK_ERROR(hr);

        m_patterns.push_back(pstr);
        ids.push_back(i);
    }

    for (size_t i = 0; i < m_patterns.size(); i++) {
        cstrPatterns.push_back(m_patterns[i].c_str());
    }

    hs_compile_error_t *compileErr;
    hs_error_t err = _hs_compile_multi(cstrPatterns.data(), nFlags.data(),
                                       ids.data(), cstrPatterns.size(), HS_MODE_BLOCK,
                                       nullptr, &m_database, &compileErr);
    if (err != HS_SUCCESS) {
        exlib::string errMsg;
        if (compileErr->expression < 0) {
            // The error does not refer to a particular expression.
            errMsg += compileErr->message;
        } else {
            errMsg += "Pattern '";
            errMsg += cstrPatterns[compileErr->expression];
            errMsg += "' failed with error '";
            errMsg += compileErr->message;
            errMsg += "'";
        }
        // As the compileErr pointer points to dynamically allocated memory,
        // if we get an error, we must be sure to release it. This is not
        // necessary when no error is detected.
        _hs_free_compile_error(compileErr);
        return CHECK_ERROR(Runtime::setError(errMsg));
    }

    return 0;
}

result_t HsRegExp::parseFlags(exlib::string flags, uint32_t& retVal)
{
    uint32_t flag = 0;
    exlib::string errMsg;

    for (size_t i = 0; i < flags.length(); i++)
    {
        switch (flags[i]) {
        case 'i':
            flag |= HS_FLAG_CASELESS; break;
        case 'm':
            flag |= HS_FLAG_MULTILINE; break;
        case 's':
            flag |= HS_FLAG_DOTALL; break;
        case 'H':
            flag |= HS_FLAG_SINGLEMATCH; break;
        case 'V':
            flag |= HS_FLAG_ALLOWEMPTY; break;
        case '8':
            flag |= HS_FLAG_UTF8; break;
        case 'W':
            flag |= HS_FLAG_UCP; break;
        case 'L':
            flag |= HS_FLAG_SOM_LEFTMOST; break;
        default:
            errMsg += "Unsupported flag '";
            errMsg += flags[i];
            errMsg += "'";
            return CHECK_ERROR(Runtime::setError(errMsg));
        }
    }
    retVal = flag;
    return 0;
}

void HsRegExp::clear()
{
    if (m_database)
    {
        _hs_free_database(m_database);
        m_database = NULL;
    }
    if (m_scratch)
    {
        _hs_free_scratch(m_scratch);
        m_scratch = NULL;
    }
}

#else


static int HsRegExp::onMatch(unsigned int id, unsigned long long from, unsigned long long to,
                             unsigned int flags, void *ctx)
{
    return 0;
}

result_t HsRegExp::scan(exlib::string text, v8::Local<v8::Object>& retVal)
{
    return 0;
}

result_t HsRegExp::scan(Buffer_base* buff, exlib::string codec, v8::Local<v8::Value>& retVal)
{
    return 0;
}

result_t HsRegExp::compile(exlib::string pattern, exlib::string flag)
{
    return 0;
}

result_t HsRegExp::compile(v8::Local<v8::Array> patterns, v8::Local<v8::Array> flags)
{
    return 0;
}

result_t HsRegExp::parseFlags(exlib::string pattern, uint32_t& retVal)
{
    return 0;
}

#endif
} /* namespace fibjs */
