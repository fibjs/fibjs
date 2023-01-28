/*
 * PKey_pem.cpp
 *
 *  Created on: May 4, 2021
 *      Author: lion
 */

#define MBEDTLS_ALLOW_PRIVATE_ACCESS

#include "object.h"
#include "ifs/crypto.h"
#include "PKey.h"
#include "PKey_impl.h"
#include "ssl.h"

namespace fibjs {

result_t PKey_base::_new(exlib::string pemKey, exlib::string password, obj_ptr<PKey_base>& retVal,
    v8::Local<v8::Object> This)
{
    result_t hr = from(pemKey, password, retVal);
    if (hr >= 0)
        retVal->wrap();

    return 0;
}

result_t ECCKey_base::_new(exlib::string pemKey, exlib::string password, obj_ptr<ECCKey_base>& retVal,
    v8::Local<v8::Object> This)
{
    obj_ptr<PKey_base> key;

    result_t hr = PKey_base::from(pemKey, password, key);
    if (hr < 0)
        return hr;

    retVal = dynamic_cast<ECCKey_base*>((PKey_base*)key);
    return retVal ? 0 : CHECK_ERROR(_ssl::setError(MBEDTLS_ERR_PK_KEY_INVALID_FORMAT));
}

result_t PKey_base::from(exlib::string pemKey, exlib::string password, obj_ptr<PKey_base>& retVal)
{
    int32_t ret;
    mbedtls_pk_context ctx;

    mbedtls_pk_init(&ctx);

    do {
        result_t hr = PKey_25519::from(pemKey, retVal);
        if (hr >= 0)
            return hr;

        ret = mbedtls_pk_parse_key(&ctx, (unsigned char*)pemKey.c_str(), pemKey.length() + 1,
            !password.empty() ? (unsigned char*)password.c_str() : NULL,
            password.length(), mbedtls_ctr_drbg_random, &g_ssl.ctr_drbg);
        if (ret != MBEDTLS_ERR_PK_KEY_INVALID_FORMAT)
            break;

        ret = mbedtls_pk_parse_public_key(&ctx, (unsigned char*)pemKey.c_str(), pemKey.length() + 1);
    } while (false);

    if (ret != 0) {
        mbedtls_pk_free(&ctx);
        return CHECK_ERROR(_ssl::setError(ret));
    }

    retVal = PKey::create(ctx);

    return 0;
}

result_t PKey::pem(exlib::string& retVal)
{
    result_t hr;
    bool priv;

    hr = isPrivate(priv);
    if (hr < 0)
        return hr;

    int32_t ret;
    exlib::string buf;

    buf.resize(mbedtls_pk_get_len(&m_key) * 8 + 128);
    if (priv)
        ret = mbedtls_pk_write_key_pem(&m_key, (unsigned char*)buf.c_buffer(), buf.length());
    else
        ret = mbedtls_pk_write_pubkey_pem(&m_key, (unsigned char*)buf.c_buffer(), buf.length());
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    buf.resize(qstrlen(buf.c_str()));
    retVal = buf;

    return 0;
}

}
