/*
 * Digest.cpp
 *
 *  Created on: Aug 2, 2012
 *      Author: lion
 */

#include "ifs/hash.h"
#include "Digest.h"
#include "Buffer.h"
#include <string.h>

namespace fibjs
{

Digest::Digest(md_type_t algo)
{
    m_bMac = false;
    m_iAlgo = algo;

    md_init_ctx(&m_ctx, md_info_from_type(algo));
    md_starts(&m_ctx);
}

Digest::Digest(md_type_t algo, const char *key, int sz)
{
    m_bMac = true;
    m_iAlgo = algo;

    md_init_ctx(&m_ctx, md_info_from_type(algo));
    md_hmac_starts(&m_ctx, (unsigned char *)key, sz);
}

Digest::~Digest()
{
    if (m_iAlgo >= 0)
        md_hmac_reset(&m_ctx);
}

result_t Digest::update(Buffer_base *data)
{
    if (m_iAlgo < 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    std::string str;
    data->toString(str);

    if (m_bMac)
        md_hmac_update(&m_ctx, (const unsigned char *) str.c_str(),
                       (int)str.length());
    else
        md_update(&m_ctx, (const unsigned char *) str.c_str(),
                  (int)str.length());

    return 0;
}

result_t Digest::update(const char *text)
{
    if (m_iAlgo < 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (m_bMac)
        md_hmac_update(&m_ctx, (const unsigned char *) text,
                       (int)qstrlen(text));
    else
        md_update(&m_ctx, (const unsigned char *) text,
                  (int)qstrlen(text));

    return 0;
}

result_t Digest::digest(obj_ptr<Buffer_base> &retVal)
{
    if (m_iAlgo < 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    std::string strBuf;

    strBuf.resize(md_get_size(m_ctx.md_info));

    if (m_bMac)
        md_hmac_finish(&m_ctx, (unsigned char *) &strBuf[0]);
    else
        md_finish(&m_ctx, (unsigned char *) &strBuf[0]);

    m_iAlgo = -1;
    md_hmac_reset(&m_ctx);

    retVal = new Buffer(strBuf);

    return 0;
}

result_t Digest::digest(Buffer_base *data,
                        obj_ptr<Buffer_base> &retVal)
{
    if (m_iAlgo < 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    update(data);
    return digest(retVal);
}

result_t Digest::digest(const char *text, obj_ptr<Buffer_base> &retVal)
{
    if (m_iAlgo < 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    update(text);
    return digest(retVal);
}

result_t Digest::get_size(int32_t &retVal)
{
    if (m_iAlgo < 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = md_get_size(m_ctx.md_info);
    return 0;
}

} /* namespace fibjs */
