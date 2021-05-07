/*
 * Digest.h
 *
 *  Created on: Aug 2, 2012
 *      Author: lion
 */

#pragma once

#include "ifs/Digest.h"
#include <mbedtls/mbedtls/md.h>

namespace fibjs {

class Digest : public Digest_base {
public:
    Digest(mbedtls_md_type_t algo);
    Digest(mbedtls_md_type_t algo, const char* key, int32_t sz);
    ~Digest();

public:
    // Digest_base
    virtual result_t update(Buffer_base* data, obj_ptr<Digest_base>& retVal);
    virtual result_t digest(exlib::string codec, v8::Local<v8::Value>& retVal);
    virtual result_t sign(PKey_base* key, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    virtual result_t verify(PKey_base* key, Buffer_base* sign, bool& retVal, AsyncEvent* ac);
    virtual result_t get_size(int32_t& retVal);

private:
    result_t digest(obj_ptr<Buffer_base>& retVal);

private:
    mbedtls_md_context_t m_ctx;
    int32_t m_iAlgo;
    bool m_bMac;
};

} /* namespace fibjs */
