/*
 * Digest.cpp
 *
 *  Created on: Aug 2, 2012
 *      Author: lion
 */

#include "object.h"
#include "ifs/util.h"
#include "crypto_util.h"
#include "Digest.h"
#include "Buffer.h"
#include "encoding.h"
#include <string.h>

namespace fibjs {

Digest::Digest(const EVP_MD* md)
{
    m_bMac = false;
    m_iAlgo = EVP_MD_type(md);

    m_ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(m_ctx, md, NULL);
}

Digest::Digest(const EVP_MD* md, const char* key, int32_t sz)
{
    m_bMac = true;
    m_iAlgo = EVP_MD_type(md);

    m_ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(m_ctx, md, NULL);

    EVPKeyPointer pkey = EVP_PKEY_new_mac_key(EVP_PKEY_HMAC, NULL, (const unsigned char*)key, sz);
    EVP_DigestSignInit(m_ctx, NULL, md, NULL, pkey);
}

result_t Digest::update(Buffer_base* data, obj_ptr<Digest_base>& retVal)
{
    Buffer* buf = Buffer::Cast(data);

    if (m_bMac)
        EVP_DigestSignUpdate(m_ctx, buf->data(), buf->length());
    else
        EVP_DigestUpdate(m_ctx, buf->data(), buf->length());
    retVal = this;

    return 0;
}

result_t Digest::update(exlib::string data, exlib::string codec, obj_ptr<Digest_base>& retVal)
{
    exlib::string _data;
    result_t hr = commonDecode(codec, data, _data);
    if (hr < 0)
        return hr;

    if (m_bMac)
        EVP_DigestSignUpdate(m_ctx, (const unsigned char*)_data.c_str(), _data.length());
    else
        EVP_DigestUpdate(m_ctx, (const unsigned char*)_data.c_str(), _data.length());
    retVal = this;

    return 0;
}

result_t Digest::digest(obj_ptr<Buffer>& retVal)
{
    obj_ptr<Buffer> buf = new Buffer(NULL, EVP_MD_size(EVP_MD_CTX_md(m_ctx)));

    if (m_bMac) {
        size_t len = buf->length();
        EVP_DigestSignFinal(m_ctx, buf->data(), &len);
    } else {
        uint32_t len = buf->length();
        EVP_DigestFinal(m_ctx, buf->data(), &len);
    }

    EVP_MD_CTX_reset(m_ctx);

    retVal = buf;
    return 0;
}

result_t Digest::digest(exlib::string codec, v8::Local<v8::Value>& retVal)
{
    obj_ptr<Buffer> buf;
    result_t hr = digest(buf);
    if (hr < 0)
        return hr;

    return buf->toValue(codec, retVal);
}

result_t Digest::get_size(int32_t& retVal)
{
    retVal = EVP_MD_size(EVP_MD_CTX_md(m_ctx));
    return 0;
}

} /* namespace fibjs */
