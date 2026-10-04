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

result_t subtle_base::digest(Union_digest_algorithm algorithm, Union_digest_data data,
    std::shared_ptr<v8::BackingStore>& retVal, AsyncEvent* ac)
{
    exlib::string alg;

    if (std::holds_alternative<exlib::string>(algorithm))
        alg = std::get<exlib::string>(algorithm);
    else if (ac->isSync()) {
        ac->m_ctx.resize(1);

        result_t hr = GetConfigValue(std::get<v8::Local<v8::Object>>(algorithm), "name", alg, true);
        if (hr < 0)
            return hr;

        ac->m_ctx[0] = alg;

        return CHECK_ERROR(CALL_E_NOSYNC);
    } else {
        // the algorithm object was read in the sync phase
        result_t ctx_hr = ac->ctx(0);
        if (ctx_hr < 0)
            return ctx_hr;

        alg = ac->m_ctx[0].string();
    }

    exlib::string data_str;

    if (std::holds_alternative<exlib::string>(data))
        data_str = std::get<exlib::string>(data);
    else
        std::get<obj_ptr<Buffer_base>>(data)->toString(data_str);

    const EVP_MD* md = _evp_md_type(alg.c_str());
    if (md == NULL)
        return CHECK_ERROR(Runtime::setError("subtle: unknown algorithm: " + alg));

    EVPMDPointer ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, md, NULL);
    EVP_DigestUpdate(ctx, data_str.c_str(), data_str.length());

    uint32_t len = EVP_MD_size(md);
    std::shared_ptr<v8::BackingStore> datas = NewBackingStore(len);
    EVP_DigestFinal(ctx, (unsigned char*)datas->Data(), &len);

    retVal = std::move(datas);

    return 0;
}


}
