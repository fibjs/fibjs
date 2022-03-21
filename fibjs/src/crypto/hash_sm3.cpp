/*
 * hash.cpp
 *
 *  Created on: Jun 20, 2021
 *      Author: lion
 */

#define MBEDTLS_ALLOW_PRIVATE_ACCESS

#include "object.h"
#include "ifs/hash.h"
#include "Digest.h"
#include "PKey.h"
#include "Buffer.h"
#include <mbedtls/mbedtls/sm3.h>

namespace fibjs {

result_t prepare_data(PKey_base* pubKey, exlib::string id, obj_ptr<Buffer_base>& retVal)
{
    static unsigned char s_iv[] = {
        // a
        0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC,
        // b
        0x28, 0xE9, 0xFA, 0x9E, 0x9D, 0x9F, 0x5E, 0x34,
        0x4D, 0x5A, 0x9E, 0x4B, 0xCF, 0x65, 0x09, 0xA7,
        0xF3, 0x97, 0x89, 0xF5, 0x15, 0xAB, 0x8F, 0x92,
        0xDD, 0xBC, 0xBD, 0x41, 0x4D, 0x94, 0x0E, 0x93,
        // x_G
        0x32, 0xC4, 0xAE, 0x2C, 0x1F, 0x19, 0x81, 0x19,
        0x5F, 0x99, 0x04, 0x46, 0x6A, 0x39, 0xC9, 0x94,
        0x8F, 0xE3, 0x0B, 0xBF, 0xF2, 0x66, 0x0B, 0xE1,
        0x71, 0x5A, 0x45, 0x89, 0x33, 0x4C, 0x74, 0xC7,
        // y_G
        0xBC, 0x37, 0x36, 0xA2, 0xF4, 0xF6, 0x77, 0x9C,
        0x59, 0xBD, 0xCE, 0xE3, 0x6B, 0x69, 0x21, 0x53,
        0xD0, 0xA9, 0x87, 0x7C, 0xC6, 0x2A, 0x47, 0x40,
        0x02, 0xDF, 0x32, 0xE5, 0x21, 0x39, 0xF0, 0xA0
    };

    obj_ptr<PKey> sm2 = (PKey*)pubKey;

    if (mbedtls_pk_get_type(&sm2->m_key) != MBEDTLS_PK_SM2)
        return CHECK_ERROR(CALL_E_INVALID_DATA);
    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(sm2->m_key);

    exlib::string pdata;
    int32_t key_size = (int32_t)mbedtls_mpi_size(&ecp->Q.X);
    int32_t ld_len = (int32_t)id.length();
    int32_t len = ld_len + sizeof(s_iv) + key_size * 2 + 2;
    pdata.resize(len);
    char* p = pdata.c_buffer();

    *p++ = (ld_len * 8) >> 8;
    *p++ = (ld_len * 8) & 0xff;

    memcpy(p, id.c_str(), ld_len);
    p += ld_len;

    memcpy(p, s_iv, sizeof(s_iv));
    p += sizeof(s_iv);

    mbedtls_mpi_write_binary(&ecp->Q.X, (unsigned char*)p, key_size);
    p += key_size;
    mbedtls_mpi_write_binary(&ecp->Q.Y, (unsigned char*)p, key_size);
    p += key_size;

    unsigned char output[32];
    mbedtls_sm3((const unsigned char*)pdata.c_str(), len, output);

    retVal = new Buffer(output, 32);

    return 0;
}

result_t hash_base::sm3(Buffer_base* data, obj_ptr<Digest_base>& retVal)
{
    return digest(hash_base::C_SM3, data, retVal);
}

result_t hash_base::sm3(PKey_base* pubKey, exlib::string id, Buffer_base* data,
    obj_ptr<Digest_base>& retVal)
{
    result_t hr;
    obj_ptr<Buffer_base> pdata;

    hr = prepare_data(pubKey, id, pdata);
    if (hr < 0)
        return hr;

    hr = digest(hash_base::C_SM3, pdata, retVal);
    if (hr < 0)
        return hr;

    obj_ptr<Digest_base> r;
    return retVal->update(data, r);
}

result_t hash_base::hmac_sm3(Buffer_base* key, Buffer_base* data, obj_ptr<Digest_base>& retVal)
{
    return hmac(hash_base::C_SM3, key, data, retVal);
}

result_t hash_base::hmac_sm3(PKey_base* pubKey, exlib::string id, Buffer_base* key,
    Buffer_base* data, obj_ptr<Digest_base>& retVal)
{
    result_t hr;
    obj_ptr<Buffer_base> pdata;

    hr = prepare_data(pubKey, id, pdata);
    if (hr < 0)
        return hr;

    hr = hmac(hash_base::C_SM3, key, pdata, retVal);
    if (hr < 0)
        return hr;

    obj_ptr<Digest_base> r;
    return retVal->update(data, r);
}

} /* namespace fibjs */
