/*
 * sign.h
 *
 *  Created on: Feb 24, 2024
 *      Author: lion
 */

#include "crypto_util.h"
#include "ifs/Sign.h"
#include "ifs/Verify.h"

namespace fibjs {

enum DSASigEnc {
    kSigEncDER,
    kSigEncP1363
};

class Sign : public Sign_base {
public:
    Sign(const EVP_MD* md);

public:
    // Sign_base
    virtual result_t update(Buffer_base* data, obj_ptr<Sign_base>& retVal);
    virtual result_t update(exlib::string data, exlib::string codec, obj_ptr<Sign_base>& retVal);
    virtual result_t sign(Buffer_base* privateKey, exlib::string encoding, v8::Local<v8::Value>& retVal);
    virtual result_t sign(KeyObject_base* privateKey, exlib::string encoding, v8::Local<v8::Value>& retVal);
    virtual result_t sign(v8::Local<v8::Object> key, exlib::string encoding, v8::Local<v8::Value>& retVal);

public:
    result_t sign(KeyObject_base* key, DSASigEnc enc, int padding, int salt_len,
        exlib::string encoding, v8::Local<v8::Value>& retVal);

private:
    EVPMDPointer m_ctx;
};

class Verify : public Verify_base {
public:
    Verify(const EVP_MD* md);

public:
    // Verify_base
    virtual result_t update(Buffer_base* data, obj_ptr<Verify_base>& retVal);
    virtual result_t update(exlib::string data, exlib::string codec, obj_ptr<Verify_base>& retVal);
    virtual result_t verify(Buffer_base* privateKey, Buffer_base* signature, bool& retVal);
    virtual result_t verify(KeyObject_base* privateKey, Buffer_base* signature, bool& retVal);
    virtual result_t verify(v8::Local<v8::Object> key, Buffer_base* signature, bool& retVal);
    virtual result_t verify(Buffer_base* privateKey, exlib::string signature, exlib::string encoding, bool& retVal);
    virtual result_t verify(KeyObject_base* privateKey, exlib::string signature, exlib::string encoding, bool& retVal);
    virtual result_t verify(v8::Local<v8::Object> key, exlib::string signature, exlib::string encoding, bool& retVal);

public:
    result_t verify(KeyObject_base* key, const unsigned char* signature, size_t sig_len, DSASigEnc enc, int padding, int salt_len, bool& retVal);
    result_t verify(KeyObject_base* key, Buffer_base* signature, DSASigEnc enc, int padding, int salt_len, bool& retVal);
    result_t verify(KeyObject_base* key, exlib::string signature, exlib::string encoding, DSASigEnc enc, int padding, int salt_len, bool& retVal);

private:
    EVPMDPointer m_ctx;
};

}
