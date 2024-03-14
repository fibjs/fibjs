/*
 * Cipher.h
 *
 *  Created on: Apr 8, 2014
 *      Author: lion
 */

#pragma once

#include "ifs/Cipher.h"
#include "StringDecoder.h"
#include "crypto_util.h"

namespace fibjs {

class Cipher : public Cipher_base {
public:
    enum CipherKind {
        kCipher,
        kDecipher
    };

    enum AuthTagState {
        kAuthTagUnknown,
        kAuthTagKnown,
        kAuthTagPassedToOpenSSL,
        kAuthTagFinalized
    };

    static const unsigned kNoAuthTagLength = static_cast<unsigned>(-1);

public:
    Cipher(CipherKind kind)
        : kind_(kind)
    {
    }

public:
    // Cipher_base
    virtual result_t setAuthTag(Buffer_base* buffer, exlib::string encoding, obj_ptr<Cipher_base>& retVal);
    virtual result_t setAuthTag(exlib::string buffer, exlib::string encoding, obj_ptr<Cipher_base>& retVal);
    virtual result_t getAuthTag(obj_ptr<Buffer_base>& retVal);
    virtual result_t setAAD(Buffer_base* buffer, v8::Local<v8::Object> options, obj_ptr<Cipher_base>& retVal);
    virtual result_t setAAD(exlib::string buffer, v8::Local<v8::Object> options, obj_ptr<Cipher_base>& retVal);
    virtual result_t setAutoPadding(bool autoPadding, obj_ptr<Cipher_base>& retVal);
    virtual result_t update(Buffer_base* data, exlib::string inputEncoding, exlib::string outputEncoding, v8::Local<v8::Value>& retVal);
    virtual result_t update(exlib::string data, exlib::string inputEncoding, exlib::string outputEncoding, v8::Local<v8::Value>& retVal);
    virtual result_t final(exlib::string outputEncoding, v8::Local<v8::Value>& retVal);

public:
    result_t init(const exlib::string& name, Buffer_base* key, v8::Local<v8::Object> options);
    result_t initiv(const exlib::string& name, const unsigned char* key, size_t size, Buffer_base* iv, v8::Local<v8::Object> options);
    result_t initiv(const exlib::string& name, Buffer_base* key, Buffer_base* iv, v8::Local<v8::Object> options);

private:
    bool IsAuthenticatedMode() const;
    result_t InitAuthenticated(int iv_len, unsigned int auth_tag_len);
    bool CheckCCMMessageLength(int message_len);
    bool MaybePassAuthTagToOpenSSL();
    result_t setAuthTag(const char* tag, int tag_len);
    result_t setAAD(const char* data, int data_len, v8::Local<v8::Object> options);
    result_t CommonInit(const EVP_CIPHER* cipher, const unsigned char* key, unsigned int key_len,
        const unsigned char* iv, unsigned int iv_len, v8::Local<v8::Object> options);
    result_t update(const unsigned char* data, size_t len, exlib::string outputEncoding, v8::Local<v8::Value>& retVal);
    result_t get_result(Buffer* out, exlib::string outputEncoding, v8::Local<v8::Value>& retVal, bool final);

private:
    CipherCtxPointer m_ctx;
    const CipherKind kind_;
    AuthTagState auth_tag_state_ = kAuthTagUnknown;
    unsigned int auth_tag_len_ = kNoAuthTagLength;
    char auth_tag_[EVP_GCM_TLS_TAG_LEN] = {};
    bool pending_auth_failed_ = false;
    int max_message_size_ = 0;

    obj_ptr<StringDecoder_base> m_decoder;
};

}
