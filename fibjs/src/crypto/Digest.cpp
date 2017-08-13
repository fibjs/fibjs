/*
 * Digest.cpp
 *
 *  Created on: Aug 2, 2012
 *      Author: lion
 */

#include "object.h"
#include "ifs/hash.h"
#include "Digest.h"
#include "Buffer.h"
#include <string.h>

namespace fibjs {

Digest::Digest(mbedtls_md_type_t algo)
{
    m_bMac = false;
    m_iAlgo = algo;

    mbedtls_md_init(&m_ctx);
    mbedtls_md_setup(&m_ctx, mbedtls_md_info_from_type(algo), 0);
    mbedtls_md_starts(&m_ctx);
}

Digest::Digest(mbedtls_md_type_t algo, const char* key, int32_t sz)
{
    m_bMac = true;
    m_iAlgo = algo;

    mbedtls_md_init(&m_ctx);
    mbedtls_md_setup(&m_ctx, mbedtls_md_info_from_type(algo), 1);
    mbedtls_md_hmac_starts(&m_ctx, (unsigned char*)key, sz);
}

Digest::~Digest()
{
    mbedtls_md_free(&m_ctx);
}

result_t Digest::update(Buffer_base* data, obj_ptr<Digest_base>& retVal)
{
    if (m_iAlgo < 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    exlib::string str;
    data->toString(str);

    if (m_bMac)
        mbedtls_md_hmac_update(&m_ctx, (const unsigned char*)str.c_str(),
            (int32_t)str.length());
    else
        mbedtls_md_update(&m_ctx, (const unsigned char*)str.c_str(),
            (int32_t)str.length());

    retVal = this;

    return 0;
}

result_t Digest::digest(obj_ptr<Buffer_base>& retVal)
{
    if (m_iAlgo < 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    exlib::string strBuf;

    strBuf.resize(mbedtls_md_get_size(m_ctx.md_info));

    if (m_bMac)
        mbedtls_md_hmac_finish(&m_ctx, (unsigned char*)&strBuf[0]);
    else
        mbedtls_md_finish(&m_ctx, (unsigned char*)&strBuf[0]);

    m_iAlgo = -1;
    mbedtls_md_hmac_reset(&m_ctx);

    retVal = new Buffer(strBuf);

    return 0;
}

result_t Digest::digest(Buffer_base* data,
    obj_ptr<Buffer_base>& retVal)
{
    if (m_iAlgo < 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    obj_ptr<Digest_base> r;
    update(data, r);
    return digest(retVal);
}

result_t Digest::get_size(int32_t& retVal)
{
    if (m_iAlgo < 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = mbedtls_md_get_size(m_ctx.md_info);
    return 0;
}

} /* namespace fibjs */
