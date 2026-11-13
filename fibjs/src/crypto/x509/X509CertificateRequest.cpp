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

result_t crypto_base::createCertificateRequest(CertificateRequestOptions* csr,
    obj_ptr<X509CertificateRequest_base>& retVal)
{
    if (!csr)
        return result_t(CALL_E_PARAMNOTOPTIONAL);

    obj_ptr<X509CertificateRequest> csr_ = new X509CertificateRequest();
    result_t hr = csr_->create(csr);
    if (hr.failed())
        return hr;

    retVal = csr_;
    return result_t();
}

result_t crypto_base::createCertificateRequest(Union_createCertificateRequest_csr csr,
    obj_ptr<X509CertificateRequest_base>& retVal)
{
    obj_ptr<X509CertificateRequest> csr_ = new X509CertificateRequest();
    result_t hr;

    if (std::holds_alternative<exlib::string>(csr)) {
        // the PEM text itself
        obj_ptr<Buffer_base> buf;

        hr = Buffer_base::from(std::get<exlib::string>(csr), "utf8", buf);
        if (hr.failed())
            return hr;

        hr = csr_->load_csr(buf);
    } else
        hr = csr_->load_csr(std::get<obj_ptr<Buffer_base>>(csr));

    if (hr.failed())
        return hr;

    retVal = csr_;
    return result_t();
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

    return result_t();
}

template <typename Options>
static result_t get_opt(const Options& options, NObject* subject,
    obj_ptr<KeyObject>& key, X509NamePointer& name, const EVP_MD*& md_)
{
    result_t hr;

    obj_ptr<KeyObject_base> key_;
    // `type` in a CSR/issue record is the certificate type, not the DER key
    // type, so it must not be forwarded to the key parser.
    hr = create_key_object(nullptr, options, KeyObject::kKeyTypePrivate, key_, false);
    if (hr.failed())
        return hr;
    key = key_.As<KeyObject>();

    name = X509_NAME_new();
    if (subject) {
        // NObject keeps the insertion order in `m_values`; iterate that instead
        // of the key index, whose unordered order would scramble the subject.
        for (auto& item : subject->m_values) {
            Variant& value = item.m_val;
            if (value.type() != Variant::VT_String)
                return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_ARG_TYPE").with_message("subject value must be a string");

            if (X509_NAME_add_entry_by_txt(name, item.m_pos->first.c_str(), MBSTRING_ASC,
                    (unsigned char*)value.string().c_str(), -1, -1, 0)
                != 1)
                return openssl_error();
        }
    }

    exlib::string hashAlgorithm = EVP_PKEY_id(key->pkey()) == EVP_PKEY_SM2 ? "sm3" : "sha256";
    if (options.hashAlgorithm.has_value())
        hashAlgorithm = options.hashAlgorithm.value();

    md_ = _evp_md_type(hashAlgorithm.c_str());
    if (md_ == nullptr)
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_ARG_VALUE").with_format("unknown digest algorithm: '%s'.", hashAlgorithm.c_str());

    return result_t();
}

result_t X509CertificateRequest::create(CertificateRequestOptions* options)
{
    if (!options)
        return result_t(CALL_E_PARAMNOTOPTIONAL);

    obj_ptr<KeyObject> key;
    X509NamePointer name;
    const EVP_MD* md;
    result_t hr = get_opt(*options,
        options->subject.has_value() ? options->subject.value().get() : nullptr,
        key, name, md);
    if (hr.failed())
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

    return result_t();
}

result_t X509CertificateRequest::get_subject(exlib::string& retVal)
{
    X509_NAME* name = X509_REQ_get_subject_name(m_csr);
    if (name == nullptr)
        return result_t::return_undefined();

    BIOPointer bio(BIO_new(BIO_s_mem()));
    if (X509_NAME_print_ex(bio, name, 0, kX509NameFlagsMultiline) <= 0)
        return openssl_error();

    return return_bio(bio, retVal);
}

result_t X509CertificateRequest::get_publicKey(obj_ptr<KeyObject_base>& retVal)
{
    if (m_publicKey) {
        retVal = m_publicKey;
        return result_t();
    }

    EVP_PKEY* pkey = X509_REQ_get_pubkey(m_csr);
    if (pkey == nullptr)
        return openssl_error();

    obj_ptr<KeyObject> key = new KeyObject();
    result_t hr = key->createPublicKeyFromPKey(pkey);
    if (hr.failed())
        return hr;

    retVal = m_publicKey = key;
    return result_t();
}

result_t X509CertificateRequest::get_subjectAltName(exlib::string& retVal)
{
    STACK_OF(X509_EXTENSION)* exts = X509_REQ_get_extensions(m_csr);
    if (exts == nullptr)
        return result_t::return_undefined();

    int index = X509v3_get_ext_by_NID(exts, NID_subject_alt_name, -1);
    if (index < 0)
        return result_t::return_undefined();

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
        return result_t::return_undefined();

    int index = X509v3_get_ext_by_NID(exts, NID_info_access, -1);
    if (index < 0)
        return result_t::return_undefined();

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

    return result_t();
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
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_ARG_VALUE").with_message("key must be a private KeyObject.");

    retVal = X509_REQ_check_private_key(m_csr, key->pkey()) != 0;
    return result_t();
}

result_t X509CertificateRequest::fill_option(X509* cert, std::optional<std::vector<exlib::string>>& list, int nid, const char** names)
{
    if (!list.has_value())
        return result_t();

    exlib::string str;
    for (auto& item : list.value()) {
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
            return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_ARG_VALUE").with_message("unknown item: " + item);
    }

    X509V3_CTX v3_ctx;
    X509V3_set_ctx(&v3_ctx, NULL, cert, NULL, NULL, 0);
    X509ExtPointer ext = X509V3_EXT_conf_nid(NULL, &v3_ctx, nid, str.c_str());
    if (ext == nullptr)
        return openssl_error();

    X509_add_ext(cert, ext, -1);

    return result_t();
}

result_t X509CertificateRequest::issue(CertificateIssueOptions* options, obj_ptr<X509Certificate_base>& retVal)
{
    if (!options)
        return result_t(CALL_E_PARAMNOTOPTIONAL);

    obj_ptr<KeyObject> key;
    X509NamePointer name;
    const EVP_MD* md;
    result_t hr = get_opt(*options,
        options->issuer.has_value() ? options->issuer.value().get() : nullptr,
        key, name, md);
    if (hr.failed())
        return hr;

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

        date_t validFrom;
        validFrom.now();
        if (options->validFrom.has_value())
            validFrom = options->validFrom.value();

        time_t tm = validFrom.date() / 1000;
        if (X509_time_adj(xi->validity.notBefore, 0, &tm) == NULL)
            return openssl_error();

        if (options->validTo.has_value()) {
            date_t validTo = options->validTo.value();
            tm = validTo.date() / 1000;
            if (X509_time_adj(xi->validity.notAfter, 0, &tm) == NULL)
                return openssl_error();
        } else {
            int32_t days = options->days.value_or(100);

            if (X509_time_adj(xi->validity.notAfter, (long)60 * 60 * 24 * days, &tm) == NULL)
                return openssl_error();
        }
    }

    {
        exlib::string basic_constraints = "critical";

        bool ca = options->ca.value_or(false);
        if (ca)
            basic_constraints.append(", CA:TRUE");
        else
            basic_constraints.append(", CA:FALSE");

        int32_t pathlen = options->pathlen.value_or(-1);
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

    hr = fill_option(cert, options->keyUsage, NID_key_usage, xfKeyUsages);
    if (hr.failed())
        return hr;

    hr = fill_option(cert, options->type, NID_netscape_cert_type, xfCertTypes);
    if (hr.failed())
        return hr;

    {
        const X509_NAME* xn = X509_REQ_get_subject_name(m_csr);
        if (xn == nullptr)
            return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_ARG_VALUE").with_message("no subject in csr");
        if (!X509_set_subject_name(cert, xn))
            return openssl_error();

        EVPKeyPointer pubkey = X509_REQ_get_pubkey(m_csr);
        if (pubkey == nullptr)
            return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_ARG_VALUE").with_message("no public key in csr");
        if (!X509_set_pubkey(cert, pubkey))
            return openssl_error();
    }

    if (!X509_set_issuer_name(cert, name))
        return openssl_error();

    if (!X509_sign(cert, key->pkey(), md))
        return openssl_error();

    retVal = new X509Certificate(cert.release());

    return result_t();
}

} // namespace fibjs
