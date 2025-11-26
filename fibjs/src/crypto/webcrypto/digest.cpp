/*
 * digest.cpp
 *
 *  Created on: Aug 23, 2014
 *      Author: lion
 */

#include "object.h"
#include "ifs/subtle.h"
#include "crypto_util.h"
#include "Buffer.h"

namespace fibjs {

result_t subtle_base::digest(exlib::string algorithm, Buffer_base* data, std::shared_ptr<v8::BackingStore>& retVal, AsyncEvent* ac)
{
    const EVP_MD* md = _evp_md_type(algorithm.c_str());
    if (md == NULL)
        return CHECK_ERROR(Runtime::setError("subtle: unknown algorithm: " + algorithm));

    Buffer* buf = Buffer::Cast(data);

    EVPMDPointer ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, md, NULL);
    EVP_DigestUpdate(ctx, buf->data(), buf->length());

    uint32_t len = EVP_MD_size(md);
    std::shared_ptr<v8::BackingStore> datas = NewBackingStore(len);
    EVP_DigestFinal(ctx, (unsigned char*)datas->Data(), &len);

    retVal = std::move(datas);

    return 0;
}

result_t subtle_base::digest(v8::Local<v8::Object> algorithm, Buffer_base* data, std::shared_ptr<v8::BackingStore>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        ac->m_ctx.resize(1);

        exlib::string alg;
        result_t hr = GetConfigValue(algorithm, "name", alg, true);
        if (hr < 0)
            return hr;

        ac->m_ctx[0] = alg;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    exlib::string alg = ac->m_ctx[0].string();
    return digest(alg, data, retVal, ac);
}

}
