/*
 * crypto.h
 *
 *  Created on: Feb 11, 2024
 *      Author: lion
 */

#pragma once

#include "utils.h"
#include "encoding.h"
#include "Buffer.h"
#include "ifs/KeyObject.h"
#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/core_names.h>

extern "C" {
#include <crypto/evp.h>
#include <crypto/ec.h>

void ossl_x25519_public_from_ed25519(uint8_t out_public_value[32], const uint8_t public_value[32]);
void ossl_x25519_private_from_ed25519(uint8_t out_private_key[32], const uint8_t private_key[32]);
}

#define NID_BLS12_381_G1 1301
#define EVP_PKEY_BLS12_381_G1 NID_BLS12_381_G1
#define NID_BLS12_381_G2 1302
#define EVP_PKEY_BLS12_381_G2 NID_BLS12_381_G2

namespace fibjs {

struct EmbedCert {
    int32_t size;
    const char* data;
};

extern EmbedCert g_root_ca[];

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

    T* operator->() const
    {
        return m_ptr;
    }

    operator T*&()
    {
        return m_ptr;
    }

    T* release()
    {
        T* ptr = m_ptr;
        m_ptr = NULL;
        return ptr;
    }

private:
    AutoPointer(const AutoPointer&) = delete;
    AutoPointer& operator=(const AutoPointer&) = delete;

private:
    T* m_ptr = NULL;
};

using ASN1Pointer = AutoPointer<ASN1_STRING, ASN1_STRING_free>;
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
using X509NamePointer = AutoPointer<X509_NAME, X509_NAME_free>;
using NetscapeSPKIPointer = AutoPointer<NETSCAPE_SPKI, NETSCAPE_SPKI_free>;
using PKCS8Pointer = AutoPointer<PKCS8_PRIV_KEY_INFO, PKCS8_PRIV_KEY_INFO_free>;
using RsaPointer = AutoPointer<RSA, RSA_free>;
using SSLCtxPointer = AutoPointer<SSL_CTX, SSL_CTX_free>;
using SSLPointer = AutoPointer<SSL, SSL_free>;
using SSLSessionPointer = AutoPointer<SSL_SESSION, SSL_SESSION_free>;
using X509Pointer = AutoPointer<X509, X509_free>;
using X509ReqPointer = AutoPointer<X509_REQ, X509_REQ_free>;
using X509ExtPointer = AutoPointer<X509_EXTENSION, X509_EXTENSION_free>;

result_t randomBytes(uint8_t* buf, int32_t size);

const EVP_MD* _evp_md_type(const char* algo);
int GetCurveFromName(const char* name);
result_t openssl_error();

inline result_t GetKeyBuffer(Isolate* isolate, v8::Local<v8::Object> o, obj_ptr<Buffer_base>& buf)
{
    result_t hr;
    v8::Local<v8::Value> v;
    hr = GetConfigValue(isolate, o, "key", v, true);
    if (hr < 0)
        return hr;

    if (v->IsString()) {
        exlib::string _buf;
        exlib::string encoding = "utf8";

        hr = GetArgumentValue(isolate, v, _buf);
        if (hr < 0)
            return hr;

        hr = GetConfigValue(isolate, o, "encoding", encoding, true);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;

        hr = commonDecode(encoding, _buf, _buf);
        if (hr < 0)
            return hr;

        buf = new Buffer((const unsigned char*)_buf.c_str(), _buf.length());
    } else {
        obj_ptr<Buffer_base> _buf;
        hr = GetArgumentValue(isolate, v, _buf);
        if (hr < 0)
            return hr;
        buf = _buf;
    }

    return 0;
}

inline result_t return_bio(BIO* bio, exlib::string& retVal)
{
    BUF_MEM* mem = nullptr;
    BIO_get_mem_ptr(bio, &mem);
    retVal = exlib::string(mem->data, mem->length);
    return 0;
}

const int kX509NameFlagsMultiline = ASN1_STRFLGS_ESC_2253 | ASN1_STRFLGS_ESC_CTRL | ASN1_STRFLGS_UTF8_CONVERT | XN_FLAG_SEP_MULTILINE | XN_FLAG_FN_SN;
const int kX509NameFlagsRFC2253WithinUtf8JSON = XN_FLAG_RFC2253 & ~ASN1_STRFLGS_ESC_MSB & ~ASN1_STRFLGS_ESC_CTRL;

extern const char* xfKeyUsages[];
extern const char* xfCertTypes[];

inline int okp_curve_nid(const char* name)
{
    if (!qstricmp(name, "X25519"))
        return NID_X25519;
    if (!qstricmp(name, "X448"))
        return NID_X448;
    if (!qstricmp(name, "Ed25519"))
        return NID_ED25519;
    if (!qstricmp(name, "Ed448"))
        return NID_ED448;
    if (!qstricmp(name, "Bls12381G1"))
        return NID_BLS12_381_G1;
    if (!qstricmp(name, "Bls12381G2"))
        return NID_BLS12_381_G2;

    return NID_undef;
}

inline const char* okp_curve_name(int nid)
{
    switch (nid) {
    case NID_X25519:
        return "X25519";
    case NID_X448:
        return "X448";
    case NID_ED25519:
        return "Ed25519";
    case NID_ED448:
        return "Ed448";
    case NID_BLS12_381_G1:
        return "Bls12381G1";
    case NID_BLS12_381_G2:
        return "Bls12381G2";
    }

    return NULL;
}

inline bool is_okp_curve(int nid)
{
    return nid == NID_X25519 || nid == NID_X448
        || nid == NID_ED25519 || nid == NID_ED448
        || nid == NID_BLS12_381_G1 || nid == NID_BLS12_381_G2;
}

enum DSASigEnc {
    kSigEncDER,
    kSigEncP1363
};

static const unsigned int kNoDsaSignature = std::numeric_limits<unsigned int>::max();
static const int DEFAULT_PADDING = std::numeric_limits<int>::max();
static const int NO_SALTLEN = std::numeric_limits<int>::max();

result_t _sign(exlib::string algorithm, Buffer_base* data, KeyObject_base* privateKey, DSASigEnc enc, int padding, int salt_len, obj_ptr<Buffer_base>& retVal);
result_t _verify(exlib::string algorithm, Buffer_base* data, KeyObject_base* publicKey, Buffer_base* signature,
    DSASigEnc enc, int padding, int salt_len, bool& retVal);

}
