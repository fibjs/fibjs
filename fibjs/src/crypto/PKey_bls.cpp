/*
 * PKey_bls.cpp
 *
 *  Created on: Sep 11, 2022
 *      Author: lion
 */

#include "Buffer.h"
#include "PKey_impl.h"
#include <blst/include/blst.h>

namespace fibjs {

result_t PKey_base::aggregateSignatures(v8::Local<v8::Array> sigs, obj_ptr<Buffer_base>& retVal)
{
    Isolate* isolate = Isolate::current();
    obj_ptr<Buffer_base> buf;
    exlib::string s;
    result_t hr;
    int32_t sig_len = sigs->Length();
    int32_t len;

    if(sig_len == 0)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    hr = GetConfigValue(isolate->m_isolate, sigs, 0, buf);
    if (hr < 0)
        return hr;

    buf->get_length(len);
    if (len == 48) {
        blst_p1 point = { 0 };
        blst_p1_affine pk;

        buf->toString(s);
        blst_p1_uncompress(&pk, (const byte*)s.c_str());
        blst_p1_add_or_double_affine(&point, &point, &pk);

        for (int32_t i = 1; i < sig_len; i++) {
            hr = GetConfigValue(isolate->m_isolate, sigs, 1, buf);
            if (hr < 0)
                return hr;

            buf->get_length(len);
            if (len != 48)
                return CHECK_ERROR(Runtime::setError("PKey: invalid signature length."));

            buf->toString(s);
            blst_p1_uncompress(&pk, (const byte*)s.c_str());
            blst_p1_add_or_double_affine(&point, &point, &pk);
        }

        s.resize(48);
        blst_p1_compress((byte*)s.c_buffer(), &point);
    } else if (len == 96) {
        blst_p2 point = { 0 };
        blst_p2_affine pk;

        buf->toString(s);
        blst_p2_uncompress(&pk, (const byte*)s.c_str());
        blst_p2_add_or_double_affine(&point, &point, &pk);

        for (int32_t i = 1; i < sig_len; i++) {
            hr = GetConfigValue(isolate->m_isolate, sigs, 1, buf);
            if (hr < 0)
                return hr;

            buf->get_length(len);
            if (len != 96)
                return CHECK_ERROR(Runtime::setError("PKey: invalid signature length."));

            buf->toString(s);
            blst_p2_uncompress(&pk, (const byte*)s.c_str());
            blst_p2_add_or_double_affine(&point, &point, &pk);
        }

        s.resize(96);
        blst_p2_compress((byte*)s.c_buffer(), &point);
    } else
        return CHECK_ERROR(Runtime::setError("PKey: invalid signature length."));

    retVal = new Buffer(s);

    return 0;
}

}
