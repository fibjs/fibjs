/*
 * PKey.cpp
 *
 *  Created on: Apr 11, 2014
 *      Author: lion
 */

#include "ifs/crypto.h"
#include "PKey.h"
#include "Cipher.h"
#include "polarssl/ecdsa.h"
#include "polarssl/rsa.h"
#include "polarssl/entropy.h"
#include "polarssl/ctr_drbg.h"

namespace fibjs
{

result_t PKey_base::_new(obj_ptr<PKey_base> &retVal)
{
    retVal = new PKey();
    return 0;
}

PKey::PKey()
{
    pk_init(&m_key);
}

PKey::~PKey()
{
    pk_free(&m_key);
}

void PKey::clear()
{
    if (pk_get_type(&m_key))
    {
        pk_free(&m_key);
        pk_init(&m_key);
    }
}

result_t PKey::toString(std::string &retVal)
{
    int ret;
    std::string buf;

    buf.resize(pk_get_len(&m_key) * 8 + 128);
    ret = pk_write_key_pem(&m_key, (unsigned char *)&buf[0], buf.length());
    if (ret != 0)
        return Cipher::setError(ret);

    buf.resize(qstrlen(buf.c_str()));
    retVal = buf;

    return 0;
}

result_t PKey::genRsaKey(int32_t size, exlib::AsyncEvent *ac)
{
    if (size < 128 || size > 8192)
        return Runtime::setError("Invalid key size");

    if (!ac)
        return CALL_E_NOSYNC;

    clear();

    char buf[1024];
    entropy_context entropy;
    ctr_drbg_context ctr_drbg;
    const char *pers = "fibjs";
    int ret;

    memset(buf, 0, sizeof(buf));

    entropy_init(&entropy);
    ret = ctr_drbg_init(&ctr_drbg, entropy_func, &entropy,
                        (const unsigned char *) pers, strlen(pers));
    if (ret != 0)
    {
        entropy_free(&entropy);
        return Cipher::setError(ret);
    }

    ret = pk_init_ctx(&m_key, pk_info_from_type(POLARSSL_PK_RSA));
    if (ret != 0)
    {
        entropy_free(&entropy);
        return Cipher::setError(ret);
    }

    ret = rsa_gen_key(pk_rsa(m_key), ctr_drbg_random, &ctr_drbg,
                      size, 65537);
    entropy_free(&entropy);

    if (ret != 0)
        return Cipher::setError(ret);

    return 0;
}

result_t PKey::genEcKey(int32_t size, exlib::AsyncEvent *ac)
{
    return 0;
}

}
