/*
 * Digest.h
 *
 *  Created on: Aug 2, 2012
 *      Author: lion
 */

#pragma once

#include "ifs/Digest.h"
#include "Buffer.h"
#include "crypto_util.h"

namespace fibjs {

class Digest : public Digest_base {
public:
    Digest(const EVP_MD* md);
    Digest(const EVP_MD* md, const char* key, int32_t sz);

public:
    // Digest_base
    virtual result_t update(Buffer_base* data, obj_ptr<Digest_base>& retVal);
    virtual result_t update(exlib::string data, exlib::string codec, obj_ptr<Digest_base>& retVal);
    virtual result_t digest(exlib::string codec, v8::Local<v8::Value>& retVal);
    virtual result_t get_size(int32_t& retVal);

private:
    result_t digest(obj_ptr<Buffer>& retVal);

private:
    EVPMDPointer m_ctx;
    int32_t m_iAlgo;
    bool m_bMac;
};

} /* namespace fibjs */
