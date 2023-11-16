/*
 * Digest.cpp
 *
 *  Created on: Aug 2, 2012
 *      Author: lion
 */

#define MBEDTLS_ALLOW_PRIVATE_ACCESS

#include "object.h"
#include "ifs/hash.h"
#include "ifs/util.h"
#include "Digest.h"
#include "Buffer.h"
#include "encoding.h"
#include <string.h>
#include "mbedtls/src/md_wrap.h"
#include "mbedtls/error.h"
#include "md_api.h"

namespace fibjs {

Digest::Digest(mbedtls_md_type_t algo)
{
    m_bMac = false;
    m_iAlgo = algo;

    mbedtls_md_init(&m_ctx);
    _md_setup(&m_ctx, algo, 0);
    _md_starts(&m_ctx);
}

Digest::Digest(mbedtls_md_type_t algo, const char* key, int32_t sz)
{
    m_bMac = true;
    m_iAlgo = algo;

    mbedtls_md_init(&m_ctx);
    _md_setup(&m_ctx, algo, 1);
    _md_hmac_starts(&m_ctx, (unsigned char*)key, sz);
}

Digest::~Digest()
{
    mbedtls_md_free(&m_ctx);
}

result_t Digest::update(Buffer_base* data, obj_ptr<Digest_base>& retVal)
{
    if (m_iAlgo < 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    Buffer* buf = Buffer::Cast(data);
    _md_update(&m_ctx, buf->data(), buf->length());

    retVal = this;

    return 0;
}

result_t Digest::update(exlib::string data, exlib::string codec, obj_ptr<Digest_base>& retVal)
{
    if (m_iAlgo < 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    exlib::string _data;
    result_t hr = commonDecode(codec, data, _data);
    if (hr < 0)
        return hr;

    _md_update(&m_ctx, (const unsigned char*)_data.c_str(), _data.length());

    retVal = this;

    return 0;
}

result_t Digest::digest(obj_ptr<Buffer_base>& retVal)
{
    if (m_iAlgo < 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    obj_ptr<Buffer> buf = new Buffer(NULL, mbedtls_md_get_size(m_ctx.md_info));

    if (m_bMac)
        _md_hmac_finish(&m_ctx, buf->data());
    else
        _md_finish(&m_ctx, buf->data());

    m_iAlgo = -1;
    _md_hmac_reset(&m_ctx);

    retVal = buf;
    return 0;
}

result_t Digest::digest(exlib::string codec, v8::Local<v8::Value>& retVal)
{
    obj_ptr<Buffer_base> buf;
    result_t hr = digest(buf);
    if (hr < 0)
        return hr;

    if ((codec == "buffer"))
        retVal = buf->wrap();
    else {
        exlib::string data;
        result_t hr = buf->toString(codec, 0, data);
        if (hr < 0)
            return hr;

        retVal = holder()->NewString(data);
    }
    return 0;
}

result_t Digest::sign(PKey_base* key, v8::Local<v8::Object> opts, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    obj_ptr<Buffer_base> buf;

    if (ac->isSync()) {
        exlib::string name;

        key->get_name(name);
        if (name == "RSA") {
            Isolate* isolate = holder();
            v8::Local<v8::Value> v;

            util_base::clone(opts, v);
            opts = v8::Local<v8::Object>::Cast(v);

            opts->Set(isolate->context(), isolate->NewString("alg"), v8::Int32::New(isolate->m_isolate, m_iAlgo)).IsJust();
        }
    } else {
        result_t hr = digest(buf);
        if (hr < 0)
            return hr;
    }

    return key->sign(buf, opts, retVal, ac);
}

result_t Digest::verify(PKey_base* key, Buffer_base* sign, v8::Local<v8::Object> opts, bool& retVal, AsyncEvent* ac)
{
    obj_ptr<Buffer_base> buf;

    if (ac->isSync()) {
        exlib::string name;

        key->get_name(name);
        if (name == "RSA") {
            Isolate* isolate = holder();
            v8::Local<v8::Value> v;

            util_base::clone(opts, v);
            opts = v8::Local<v8::Object>::Cast(v);

            opts->Set(isolate->context(), isolate->NewString("alg"), v8::Int32::New(isolate->m_isolate, m_iAlgo)).IsJust();
        }
    } else {
        result_t hr = digest(buf);
        if (hr < 0)
            return hr;
    }

    return key->verify(buf, sign, opts, retVal, ac);
}

result_t Digest::get_size(int32_t& retVal)
{
    if (m_iAlgo < 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = mbedtls_md_get_size(m_ctx.md_info);
    return 0;
}

} /* namespace fibjs */
