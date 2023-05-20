/*
 * PKey_der.cpp
 *
 *  Created on: May 4, 2021
 *      Author: lion
 */

#define MBEDTLS_ALLOW_PRIVATE_ACCESS

#include "object.h"
#include "ifs/crypto.h"
#include "PKey.h"
#include "Ed25519Key.h"
#include "ssl.h"
#include "Buffer.h"

namespace fibjs {

result_t PKey_base::from(Buffer_base* DerKey, exlib::string password, obj_ptr<PKey_base>& retVal)
{
    int32_t ret;
    mbedtls_pk_context ctx;

    mbedtls_pk_init(&ctx);

    do {
        result_t hr = Ed25519Key::from(DerKey, retVal);
        if (hr >= 0)
            return hr;

        obj_ptr<Buffer> buf_key = Buffer::Cast(DerKey);

        ret = mbedtls_pk_parse_key(&ctx, buf_key->data(), buf_key->length(),
            !password.empty() ? (unsigned char*)password.c_str() : NULL,
            password.length(), mbedtls_ctr_drbg_random, &g_ssl.ctr_drbg);
        if (ret != MBEDTLS_ERR_PK_KEY_INVALID_FORMAT)
            break;

        ret = mbedtls_pk_parse_public_key(&ctx, buf_key->data(), buf_key->length());
    } while (false);

    if (ret != 0) {
        mbedtls_pk_free(&ctx);
        return CHECK_ERROR(_ssl::setError(ret));
    }

    retVal = PKey::create(ctx);

    return 0;
}

result_t PKey::der(obj_ptr<Buffer_base>& retVal)
{
    result_t hr;
    bool priv;

    hr = isPrivate(priv);
    if (hr < 0)
        return hr;

    int32_t ret;
    exlib::string buf;

    buf.resize(8192);
    if (priv)
        ret = mbedtls_pk_write_key_der(&m_key, (unsigned char*)buf.c_buffer(), buf.length());
    else
        ret = mbedtls_pk_write_pubkey_der(&m_key, (unsigned char*)buf.c_buffer(), buf.length());
    if (ret < 0)
        return CHECK_ERROR(_ssl::setError(ret));

    retVal = new Buffer(buf.c_str() + buf.length() - ret, ret);

    return 0;
}

}
