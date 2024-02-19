/*
 * crypto.h
 *
 *  Created on: Feb 11, 2024
 *      Author: lion
 */

#pragma once

#include "utils.h"
#include <openssl/ssl.h>

namespace fibjs {

template <typename T, void (*deleter)(T*)>
class AutoPointer {
public:
    AutoPointer(T* ptr = NULL)
        : m_ptr(ptr)
    {
    }

    ~AutoPointer()
    {
        if (m_ptr)
            deleter(m_ptr);
    }

public:
    T* operator=(T* ptr)
    {
        if (m_ptr)
            deleter(m_ptr);
        m_ptr = ptr;

        return m_ptr;
    }

    operator T*() const
    {
        return m_ptr;
    }

private:
    AutoPointer(const AutoPointer&) = delete;
    AutoPointer& operator=(const AutoPointer&) = delete;

private:
    T* m_ptr = NULL;
};

using BignumCtxPointer = AutoPointer<BN_CTX, BN_CTX_free>;
using BignumPointer = AutoPointer<BIGNUM, BN_clear_free>;
using BIOPointer = AutoPointer<BIO, BIO_free_all>;
using CipherCtxPointer = AutoPointer<EVP_CIPHER_CTX, EVP_CIPHER_CTX_free>;
using DHPointer = AutoPointer<DH, DH_free>;
using DsaPointer = AutoPointer<DSA, DSA_free>;
using DsaSigPointer = AutoPointer<DSA_SIG, DSA_SIG_free>;
using ECDSASigPointer = AutoPointer<ECDSA_SIG, ECDSA_SIG_free>;
using ECGroupPointer = AutoPointer<EC_GROUP, EC_GROUP_free>;
using ECKeyPointer = AutoPointer<EC_KEY, EC_KEY_free>;
using ECPointer = AutoPointer<EC_KEY, EC_KEY_free>;
using ECPointPointer = AutoPointer<EC_POINT, EC_POINT_free>;
using EVPKeyCtxPointer = AutoPointer<EVP_PKEY_CTX, EVP_PKEY_CTX_free>;
using EVPKeyPointer = AutoPointer<EVP_PKEY, EVP_PKEY_free>;
using EVPMDPointer = AutoPointer<EVP_MD_CTX, EVP_MD_CTX_free>;
using HMACCtxPointer = AutoPointer<HMAC_CTX, HMAC_CTX_free>;
using NetscapeSPKIPointer = AutoPointer<NETSCAPE_SPKI, NETSCAPE_SPKI_free>;
using PKCS8Pointer = AutoPointer<PKCS8_PRIV_KEY_INFO, PKCS8_PRIV_KEY_INFO_free>;
using RsaPointer = AutoPointer<RSA, RSA_free>;
using RSAPointer = AutoPointer<RSA, RSA_free>;
using SSLCtxPointer = AutoPointer<SSL_CTX, SSL_CTX_free>;
using SSLPointer = AutoPointer<SSL, SSL_free>;
using SSLSessionPointer = AutoPointer<SSL_SESSION, SSL_SESSION_free>;
using X509Pointer = AutoPointer<X509, X509_free>;

result_t randomBytes(uint8_t* buf, int32_t size);

result_t openssl_error();

} /* namespace fibjs */
