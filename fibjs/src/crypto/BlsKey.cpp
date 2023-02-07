/*
 * BlsKey.cpp
 *
 *  Created on: Sep 11, 2022
 *      Author: lion
 */

#include "Buffer.h"
#include "BlsKey.h"
#include <blst/include/blst.h>
#include "ssl.h"

namespace fibjs {

result_t BlsKey_base::_new(v8::Local<v8::Object> jsonKey, obj_ptr<BlsKey_base>& retVal,
    v8::Local<v8::Object> This)
{
    obj_ptr<PKey_base> key;

    result_t hr = PKey_base::from(jsonKey, key);
    if (hr < 0)
        return hr;

    retVal = dynamic_cast<BlsKey_base*>((PKey_base*)key);
    return retVal ? 0 : CHECK_ERROR(_ssl::setError(MBEDTLS_ERR_PK_KEY_INVALID_FORMAT));
}

result_t BlsKey_base::aggregateSignature(v8::Local<v8::Array> sigs, obj_ptr<Buffer_base>& retVal)
{
    Isolate* isolate = Isolate::current();
    obj_ptr<Buffer_base> buf;
    exlib::string s;
    result_t hr;
    int32_t sig_len = sigs->Length();
    int32_t len;

    if (sig_len == 0)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    hr = GetConfigValue(isolate->m_isolate, sigs, 0, buf);
    if (hr < 0)
        return hr;

    buf->get_length(len);
    if (len == 48) {
        blst_p1 point;
        blst_p1_affine pk;

        buf->toString(s);
        blst_p1_uncompress(&pk, (const byte*)s.c_str());
        blst_p1_from_affine(&point, &pk);

        for (int32_t i = 1; i < sig_len; i++) {
            hr = GetConfigValue(isolate->m_isolate, sigs, 1, buf);
            if (hr < 0)
                return hr;

            buf->get_length(len);
            if (len != 48)
                return CHECK_ERROR(Runtime::setError("BlsKey: invalid signature length."));

            buf->toString(s);
            blst_p1_uncompress(&pk, (const byte*)s.c_str());
            blst_p1_add_or_double_affine(&point, &point, &pk);
        }

        s.resize(48);
        blst_p1_compress((byte*)s.c_buffer(), &point);
    } else if (len == 96) {
        blst_p2 point;
        blst_p2_affine pk;

        buf->toString(s);
        blst_p2_uncompress(&pk, (const byte*)s.c_str());
        blst_p2_from_affine(&point, &pk);

        for (int32_t i = 1; i < sig_len; i++) {
            hr = GetConfigValue(isolate->m_isolate, sigs, 1, buf);
            if (hr < 0)
                return hr;

            buf->get_length(len);
            if (len != 96)
                return CHECK_ERROR(Runtime::setError("BlsKey: invalid signature length."));

            buf->toString(s);
            blst_p2_uncompress(&pk, (const byte*)s.c_str());
            blst_p2_add_or_double_affine(&point, &point, &pk);
        }

        s.resize(96);
        blst_p2_compress((byte*)s.c_buffer(), &point);
    } else
        return CHECK_ERROR(Runtime::setError("BlsKey: invalid signature length."));

    retVal = new Buffer(s);

    return 0;
}

result_t BlsKey_base::aggregatePublicKey(v8::Local<v8::Array> sigs, obj_ptr<BlsKey_base>& retVal)
{
    Isolate* isolate = Isolate::current();
    obj_ptr<BlsKey_base> k;
    result_t hr;
    int32_t key_len = sigs->Length();

    if (key_len == 0)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    hr = GetConfigValue(isolate->m_isolate, sigs, 0, k);
    if (hr < 0)
        return hr;

    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(PKey::key(k));
    if (ecp->grp.id == (mbedtls_ecp_group_id)MBEDTLS_ECP_DP_BLS12381_G1) {
        unsigned char buf[48];
        blst_p1 point;
        blst_p1_affine pk;

        mbedtls_mpi_write_binary(&ecp->Q.X, buf, 48);
        blst_p1_uncompress(&pk, buf);
        blst_p1_from_affine(&point, &pk);

        for (int32_t i = 1; i < key_len; i++) {
            obj_ptr<BlsKey_base> k1;

            hr = GetConfigValue(isolate->m_isolate, sigs, i, k1);
            if (hr < 0)
                return hr;

            mbedtls_ecp_keypair* ecp1 = mbedtls_pk_ec(PKey::key(k1));
            if (ecp1->grp.id != (mbedtls_ecp_group_id)MBEDTLS_ECP_DP_BLS12381_G1)
                return CHECK_ERROR(Runtime::setError("BlsKey: invalid g1 public key."));

            mbedtls_mpi_write_binary(&ecp1->Q.X, buf, 48);
            blst_p1_uncompress(&pk, buf);
            blst_p1_add_or_double_affine(&point, &point, &pk);
        }

        blst_p1_compress((byte*)buf, &point);
        mbedtls_mpi_read_binary(&ecp->Q.X, (unsigned char*)buf, 48);
    } else {
        unsigned char buf[96];
        blst_p2 point;
        blst_p2_affine pk;

        mbedtls_mpi_write_binary(&ecp->Q.X, buf, 96);
        blst_p2_uncompress(&pk, buf);
        blst_p2_from_affine(&point, &pk);

        for (int32_t i = 1; i < key_len; i++) {
            obj_ptr<BlsKey_base> k1;

            hr = GetConfigValue(isolate->m_isolate, sigs, i, k1);
            if (hr < 0)
                return hr;

            mbedtls_ecp_keypair* ecp1 = mbedtls_pk_ec(PKey::key(k1));
            if (ecp1->grp.id != (mbedtls_ecp_group_id)MBEDTLS_ECP_DP_BLS12381_G2)
                return CHECK_ERROR(Runtime::setError("BlsKey: invalid g2 public key."));

            mbedtls_mpi_write_binary(&ecp1->Q.X, buf, 96);
            blst_p2_uncompress(&pk, buf);
            blst_p2_add_or_double_affine(&point, &point, &pk);
        }

        blst_p2_compress((byte*)buf, &point);
        mbedtls_mpi_read_binary(&ecp->Q.X, (unsigned char*)buf, 96);
    }

    retVal = k;

    return 0;
}

}
