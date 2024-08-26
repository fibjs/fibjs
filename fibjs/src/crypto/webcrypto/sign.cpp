/*
 * sign.cpp
 *
 *  Created on: Aug 26, 2014
 *      Author: lion
 */

#include "object.h"
#include "ifs/subtle.h"
#include "CryptoKey.h"

namespace fibjs {

static result_t get_options(v8::Local<v8::Object> algorithm, CryptoKey* key, AsyncEvent* ac)
{
    result_t hr;
    Isolate* isolate = ac->isolate();

    ac->m_ctx.resize(2);

    exlib::string name;
    hr = GetConfigValue(isolate, algorithm, "name", name, true);
    if (hr < 0)
        return hr;

    ac->m_ctx[0] = name;

    hr = key->check_name(name);
    if (hr < 0)
        return hr;

    if (qstricmp(name.c_str(), "ecdsa") == 0) {
        exlib::string hash;
        hr = GetConfigValue(isolate, algorithm, "hash", hash, true);
        if (hr < 0)
            return hr;

        ac->m_ctx[1] = hash;
    }

    return 0;
}

result_t subtle_base::sign(v8::Local<v8::Object> algorithm, CryptoKey_base* key, Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        result_t hr = get_options(algorithm, (CryptoKey*)key, ac);
        if (hr < 0)
            return hr;

        return CALL_E_NOSYNC;
    }

    CryptoKey* _key = (CryptoKey*)key;
    exlib::string hash = ac->m_ctx[1].string();

    return _sign(hash, data, _key->m_key, kSigEncP1363, DEFAULT_PADDING, NO_SALTLEN, retVal);
}

result_t subtle_base::verify(v8::Local<v8::Object> algorithm, CryptoKey_base* key, Buffer_base* signature, Buffer_base* data, bool& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        result_t hr = get_options(algorithm, (CryptoKey*)key, ac);
        if (hr < 0)
            return hr;

        return CALL_E_NOSYNC;
    }

    CryptoKey* _key = (CryptoKey*)key;
    exlib::string hash = ac->m_ctx[1].string();

    return _verify(hash, data, _key->m_key, signature, kSigEncP1363, DEFAULT_PADDING, NO_SALTLEN, retVal);
}

}
