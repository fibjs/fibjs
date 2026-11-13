/*
 * sign.h
 *
 *  Created on: Feb 24, 2024
 *      Author: lion
 */

#include "crypto_util.h"
#include "ifs/Sign.h"
#include "ifs/Verify.h"
#include "ifs/SignCreateOptions.h"

namespace fibjs {

class Sign : public Sign_base {
public:
    Sign(const EVP_MD* md);
    result_t set_options(SignCreateOptions* options);

public:
    // Sign_base
    virtual result_t update(Union_update_data data, exlib::string codec, obj_ptr<Sign_base>& retVal);
    virtual result_t sign(SignOptions* privateKey, exlib::string encoding, v8::Local<v8::Value>& retVal);
    virtual result_t sign(Union_sign_privateKey privateKey, exlib::string encoding, v8::Local<v8::Value>& retVal);

public:
    // key forms: the merged entry dispatches to them
    result_t sign_keyobj(KeyObject_base* privateKey, exlib::string encoding, v8::Local<v8::Value>& retVal);

public:
    result_t sign(KeyObject_base* key, DSASigEnc enc, int padding, int salt_len,
        exlib::string encoding, v8::Local<v8::Value>& retVal);

private:
    EVPMDPointer m_ctx;
    DSASigEnc m_enc = kSigEncDER;
    int m_padding = DEFAULT_PADDING;
    int m_salt_len = RSA_PSS_SALTLEN_MAX_SIGN;
};

class Verify : public Verify_base {
public:
    Verify(const EVP_MD* md);
    result_t set_options(SignCreateOptions* options);

public:
    // Verify_base
    virtual result_t update(Union_update_data data, exlib::string codec, obj_ptr<Verify_base>& retVal);
    virtual result_t verify(SignOptions* privateKey, Union_verify_signature signature,
        exlib::string encoding, bool& retVal);
    virtual result_t verify(Union_verify_privateKey privateKey, Union_verify_signature signature,
        exlib::string encoding, bool& retVal);

public:
    // key forms: the merged entry dispatches to them
    result_t verify_keyobj(KeyObject_base* privateKey, Buffer_base* signature, bool& retVal);
    result_t verify_keyobj(KeyObject_base* privateKey, exlib::string signature, exlib::string encoding, bool& retVal);

public:
    result_t verify(KeyObject_base* key, const unsigned char* signature, size_t sig_len, DSASigEnc enc, int padding, int salt_len, bool& retVal);
    result_t verify(KeyObject_base* key, Buffer_base* signature, DSASigEnc enc, int padding, int salt_len, bool& retVal);
    result_t verify(KeyObject_base* key, exlib::string signature, exlib::string encoding, DSASigEnc enc, int padding, int salt_len, bool& retVal);

private:
    EVPMDPointer m_ctx;
    DSASigEnc m_enc = kSigEncDER;
    int m_padding = DEFAULT_PADDING;
    int m_salt_len = RSA_PSS_SALTLEN_MAX_SIGN;
};

}
