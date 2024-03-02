/*
 * X509CertificateRequest.cpp
 *
 *  Created on: Fec 27, 2024
 *      Author: lion
 */

#include "ifs/crypto.h"
#include "X509CertificateRequest.h"
#include "X509Certificate.h"
#include "KeyObject.h"
#include "crypto/x509.h"
#include "openssl/x509v3.h"

namespace fibjs {

bool SafeX509SubjectAltNamePrint(const BIOPointer& out, X509_EXTENSION* ext);

result_t crypto_base::createCertificateRequest(Buffer_base* csr, obj_ptr<X509CertificateRequest_base>& retVal)
{
    obj_ptr<X509CertificateRequest> csr_ = new X509CertificateRequest();
    result_t hr = csr_->load_csr(csr);
    if (hr != 0)
        return hr;

    retVal = csr_;
    return 0;
}

result_t crypto_base::createCertificateRequest(v8::Local<v8::Object> options,
    obj_ptr<X509CertificateRequest_base>& retVal)
{
    obj_ptr<X509CertificateRequest> csr_ = new X509CertificateRequest();
    result_t hr = csr_->create(options);
    if (hr != 0)
        return hr;

    retVal = csr_;
    return 0;
}

result_t X509CertificateRequest::load_csr(Buffer_base* csr)
{
    Buffer* buf_csr = Buffer::Cast(csr);
    BIOPointer bio = BIO_new_mem_buf(buf_csr->data(), buf_csr->length());
    if (bio == nullptr)
        return openssl_error();

    m_csr = PEM_read_bio_X509_REQ(
        bio, nullptr, [](char*, int, int, void*) {
            return 0;
        },
        nullptr);
    if (m_csr == nullptr)
        return openssl_error();

    return 0;
}

static result_t get_opt(v8::Local<v8::Object> options, const char* subject_name,
    obj_ptr<KeyObject>& key, X509NamePointer& name, const EVP_MD*& md_)
{
    Isolate* isolate = Isolate::current(options);
    v8::Local<v8::Context> context = isolate->context();
    result_t hr;

    obj_ptr<KeyObject_base> key_;
    hr = crypto_base::createPrivateKey(options, key_);
    if (hr < 0)
        return hr;
    key = (KeyObject*)(KeyObject_base*)key_;

    name = X509_NAME_new();
    v8::Local<v8::Object> subject;
    hr = GetConfigValue(isolate, options, subject_name, subject, true);
    if (hr == 0) {
        v8::Local<v8::Array> props = subject->GetPropertyNames(context).FromMaybe(v8::Local<v8::Array>());
        int32_t len = props->Length();
        for (int i = 0; i < len; i++) {
            exlib::string key_, value_;

            GetConfigValue(isolate, props, i, key_, true);
            hr = GetConfigValue(isolate, subject, key_.c_str(), value_, true);
            if (hr != 0)
                return hr;

            if (X509_NAME_add_entry_by_txt(name, key_.c_str(), MBSTRING_ASC, (unsigned char*)value_.c_str(), -1, -1, 0) != 1)
                return openssl_error();
        }
    } else if (hr != CALL_E_PARAMNOTOPTIONAL)
        return Runtime::setError("subject must be object");

    exlib::string hashAlgorithm = "sha256";
    hr = GetConfigValue(isolate, options, "hashAlgorithm", hashAlgorithm, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    md_ = _evp_md_type(hashAlgorithm.c_str());
    if (md_ == nullptr)
        return Runtime::setError("unknown digest");

    return 0;
}

result_t X509CertificateRequest::create(v8::Local<v8::Object> options)
{
    obj_ptr<KeyObject> key;
    X509NamePointer name;
    const EVP_MD* md;
    result_t hr = get_opt(options, "subject", key, name, md);
    if (hr < 0)
        return hr;

    X509ReqPointer csr = X509_REQ_new();
    if (csr == nullptr)
        return openssl_error();

    X509_REQ_set_version(csr, 1);

    if (X509_REQ_set_pubkey(csr, key->pkey()) != 1)
        return openssl_error();

    if (X509_REQ_set_subject_name(csr, name) != 1)
        return openssl_error();

    if (X509_REQ_sign(csr, key->pkey(), md) <= 0)
        return openssl_error();

    m_csr = csr.release();

    return 0;
}

result_t X509CertificateRequest::get_subject(exlib::string& retVal)
{
    X509_NAME* name = X509_REQ_get_subject_name(m_csr);
    if (name == nullptr)
        return CALL_RETURN_UNDEFINED;

    BIOPointer bio(BIO_new(BIO_s_mem()));
    if (X509_NAME_print_ex(bio, name, 0, kX509NameFlagsMultiline) <= 0)
        return openssl_error();

    return return_bio(bio, retVal);
}

result_t X509CertificateRequest::get_publicKey(obj_ptr<KeyObject_base>& retVal)
{
    if (m_publicKey) {
        retVal = m_publicKey;
        return 0;
    }

    EVP_PKEY* pkey = X509_REQ_get_pubkey(m_csr);
    if (pkey == nullptr)
        return openssl_error();

    obj_ptr<KeyObject> key = new KeyObject();
    result_t hr = key->createPublicKeyFromPKey(pkey);
    if (hr != 0)
        return hr;

    retVal = m_publicKey = key;
    return 0;
}

result_t X509CertificateRequest::get_subjectAltName(exlib::string& retVal)
{
    STACK_OF(X509_EXTENSION)* exts = X509_REQ_get_extensions(m_csr);
    if (exts == nullptr)
        return CALL_RETURN_UNDEFINED;

    int index = X509v3_get_ext_by_NID(exts, NID_subject_alt_name, -1);
    if (index < 0)
        return CALL_RETURN_UNDEFINED;

    X509_EXTENSION* ext = X509v3_get_ext(exts, index);
    if (ext == nullptr)
        return openssl_error();

    BIOPointer bio(BIO_new(BIO_s_mem()));
    if (!SafeX509SubjectAltNamePrint(bio, ext))
        return openssl_error();

    sk_X509_EXTENSION_pop_free(exts, X509_EXTENSION_free);

    return return_bio(bio, retVal);
}

result_t X509CertificateRequest::get_infoAccess(exlib::string& retVal)
{
    STACK_OF(X509_EXTENSION)* exts = X509_REQ_get_extensions(m_csr);
    if (exts == nullptr)
        return CALL_RETURN_UNDEFINED;

    int index = X509v3_get_ext_by_NID(exts, NID_info_access, -1);
    if (index < 0)
        return CALL_RETURN_UNDEFINED;

    X509_EXTENSION* ext = X509v3_get_ext(exts, index);
    if (ext == nullptr)
        return openssl_error();

    BIOPointer bio(BIO_new(BIO_s_mem()));
    if (!SafeX509SubjectAltNamePrint(bio, ext))
        return openssl_error();

    sk_X509_EXTENSION_pop_free(exts, X509_EXTENSION_free);

    return return_bio(bio, retVal);
}

result_t X509CertificateRequest::get_raw(obj_ptr<Buffer_base>& retVal)
{
    int size = i2d_X509_REQ(m_csr, nullptr);
    if (size < 0)
        return openssl_error();

    obj_ptr<Buffer> buf = new Buffer(NULL, size);
    unsigned char* p = buf->data();
    if (i2d_X509_REQ(m_csr, &p) < 0)
        return openssl_error();

    retVal = buf;

    return 0;
}

result_t X509CertificateRequest::get_pem(exlib::string& retVal)
{
    BIOPointer bio(BIO_new(BIO_s_mem()));

    if (PEM_write_bio_X509_REQ(bio, m_csr) <= 0)
        return openssl_error();

    return return_bio(bio, retVal);
}

result_t X509CertificateRequest::toString(exlib::string& retVal)
{
    return get_pem(retVal);
}

result_t X509CertificateRequest::checkPrivateKey(KeyObject_base* privateKey, bool& retVal)
{
    KeyObject* key = (KeyObject*)privateKey;
    if (key->type() != KeyObject::kKeyTypePrivate)
        return CALL_E_INVALIDARG;

    retVal = X509_REQ_check_private_key(m_csr, key->pkey()) != 0;
    return 0;
}

result_t X509CertificateRequest::fill_option(X509* cert, v8::Local<v8::Object> options, const char* key, int nid, const char** names)
{
    Isolate* isolate = holder();
    v8::Local<v8::Array> list;
    result_t hr;

    hr = GetConfigValue(isolate, options, key, list, true);
    if (hr == CALL_E_PARAMNOTOPTIONAL)
        return 0;
    if (hr < 0)
        return hr;

    exlib::string str;
    int32_t len = list->Length();
    for (int32_t i = 0; i < len; i++) {
        exlib::string item;
        hr = GetConfigValue(isolate, list, i, item, true);
        if (hr != 0)
            return hr;

        int32_t p;
        for (p = 0; p < 8; p++) {
            if (item == names[p]) {
                if (!str.empty())
                    str.append(",");
                str.append(item);
                break;
            }
        }
        if (p == 8)
            return Runtime::setError("unknown item: " + item);
    }

    X509V3_CTX v3_ctx;
    X509V3_set_ctx(&v3_ctx, NULL, cert, NULL, NULL, 0);
    X509ExtPointer ext = X509V3_EXT_conf_nid(NULL, &v3_ctx, nid, str.c_str());
    if (ext == nullptr)
        return openssl_error();

    X509_add_ext(cert, ext, -1);

    return 0;
}

result_t X509CertificateRequest::issue(v8::Local<v8::Object> options, obj_ptr<X509Certificate_base>& retVal)
{
    obj_ptr<KeyObject> key;
    X509NamePointer name;
    const EVP_MD* md;
    result_t hr = get_opt(options, "issuer", key, name, md);
    if (hr < 0)
        return hr;

    Isolate* isolate = holder();

    X509Pointer cert = X509_new();
    if (cert == nullptr)
        return openssl_error();

    {
        X509_CINF* xi = &cert->cert_info;
        if (sk_X509_ATTRIBUTE_num(m_csr->req_info.attributes) != 0) {
            if ((xi->version = ASN1_INTEGER_new()) == NULL)
                return openssl_error();
            if (!ASN1_INTEGER_set(xi->version, 2))
                return openssl_error();
        }

        BignumPointer bn = BN_new();
        if (bn == nullptr)
            return openssl_error();

        if (!BN_pseudo_rand(bn, 64, 0, 0))
            return openssl_error();
        if (!BN_to_ASN1_INTEGER(bn, &xi->serialNumber))
            return openssl_error();

        int32_t days = 100;
        hr = GetConfigValue(isolate, options, "days", days, true);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;

        if (X509_gmtime_adj(xi->validity.notBefore, 0) == NULL)
            return openssl_error();
        if (X509_gmtime_adj(xi->validity.notAfter, (long)60 * 60 * 24 * days) == NULL)
            return openssl_error();
    }

    {
        exlib::string basic_constraints = "critical";

        bool ca = false;
        hr = GetConfigValue(isolate, options, "ca", ca, true);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;
        if (ca)
            basic_constraints.append(", CA:TRUE");
        else
            basic_constraints.append(", CA:FALSE");

        int32_t pathlen = -1;
        hr = GetConfigValue(isolate, options, "pathlen", pathlen, true);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;
        if (pathlen >= 0) {
            char buf[64];
            snprintf(buf, sizeof(buf), ", pathlen:%d", pathlen);
            basic_constraints.append(buf);
        }

        X509V3_CTX v3_ctx;
        X509V3_set_ctx(&v3_ctx, NULL, cert, NULL, NULL, 0);
        X509ExtPointer ext = X509V3_EXT_conf_nid(NULL, &v3_ctx, NID_basic_constraints, basic_constraints.c_str());
        if (ext == nullptr)
            return openssl_error();

        X509_add_ext(cert, ext, -1);
    }

    hr = fill_option(cert, options, "keyUsage", NID_key_usage, xfKeyUsages);
    if (hr < 0)
        return hr;

    hr = fill_option(cert, options, "type", NID_netscape_cert_type, xfCertTypes);
    if (hr < 0)
        return hr;

    {
        const X509_NAME* xn = X509_REQ_get_subject_name(m_csr);
        if (xn == nullptr)
            return Runtime::setError("no subject in csr");
        if (!X509_set_subject_name(cert, xn))
            return openssl_error();

        EVPKeyPointer pubkey = X509_REQ_get_pubkey(m_csr);
        if (pubkey == nullptr)
            return Runtime::setError("no public key in csr");
        if (!X509_set_pubkey(cert, pubkey))
            return openssl_error();
    }

    if (!X509_set_issuer_name(cert, name))
        return openssl_error();

    if (!X509_sign(cert, key->pkey(), md))
        return openssl_error();

    retVal = new X509Certificate(cert.release());

    return 0;
}

} // namespace fibjs
