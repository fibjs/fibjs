/*
 * X509Certificate.cpp
 *
 *  Created on: Fec 26, 2024
 *      Author: lion
 */

#include "ifs/crypto.h"
#include "X509Certificate.h"
#include "Buffer.h"
#include "openssl/x509.h"
#include "openssl/x509v3.h"

namespace fibjs {

static inline bool IsSafeAltName(const char* name, size_t length, bool utf8)
{
    for (size_t i = 0; i < length; i++) {
        char c = name[i];
        switch (c) {
        case '"':
        case '\\':
        case ',':
        case '\'':
            return false;
        default:
            if (utf8) {
                if (static_cast<unsigned char>(c) < ' ' || c == '\x7f') {
                    return false;
                }
            } else {
                if (c < ' ' || c > '~') {
                    return false;
                }
            }
        }
    }
    return true;
}

static inline void PrintAltName(const BIOPointer& out, const char* name,
    size_t length, bool utf8,
    const char* safe_prefix)
{
    if (IsSafeAltName(name, length, utf8)) {
        if (safe_prefix != nullptr) {
            BIO_printf(out, "%s:", safe_prefix);
        }
        BIO_write(out, name, length);
    } else {
        BIO_write(out, "\"", 1);
        if (safe_prefix != nullptr) {
            BIO_printf(out, "%s:", safe_prefix);
        }
        for (size_t j = 0; j < length; j++) {
            char c = static_cast<char>(name[j]);
            if (c == '\\') {
                BIO_write(out, "\\\\", 2);
            } else if (c == '"') {
                BIO_write(out, "\\\"", 2);
            } else if ((c >= ' ' && c != ',' && c <= '~') || (utf8 && (c & 0x80))) {
                BIO_write(out, &c, 1);
            } else {
                const char hex[] = "0123456789abcdef";
                char u[] = { '\\', 'u', '0', '0', hex[(c & 0xf0) >> 4], hex[c & 0x0f] };
                BIO_write(out, u, sizeof(u));
            }
        }
        BIO_write(out, "\"", 1);
    }
}

static inline void PrintLatin1AltName(const BIOPointer& out,
    const ASN1_IA5STRING* name,
    const char* safe_prefix = nullptr)
{
    PrintAltName(out, reinterpret_cast<const char*>(name->data), name->length,
        false, safe_prefix);
}

static inline void PrintUtf8AltName(const BIOPointer& out,
    const ASN1_UTF8STRING* name,
    const char* safe_prefix = nullptr)
{
    PrintAltName(out, reinterpret_cast<const char*>(name->data), name->length,
        true, safe_prefix);
}

static bool PrintGeneralName(const BIOPointer& out, const GENERAL_NAME* gen)
{
    if (gen->type == GEN_DNS) {
        ASN1_IA5STRING* name = gen->d.dNSName;
        BIO_write(out, "DNS:", 4);
        PrintLatin1AltName(out, name);
    } else if (gen->type == GEN_EMAIL) {
        ASN1_IA5STRING* name = gen->d.rfc822Name;
        BIO_write(out, "email:", 6);
        PrintLatin1AltName(out, name);
    } else if (gen->type == GEN_URI) {
        ASN1_IA5STRING* name = gen->d.uniformResourceIdentifier;
        BIO_write(out, "URI:", 4);
        PrintLatin1AltName(out, name);
    } else if (gen->type == GEN_DIRNAME) {
        BIO_printf(out, "DirName:");
        BIOPointer tmp(BIO_new(BIO_s_mem()));
        if (X509_NAME_print_ex(tmp, gen->d.dirn, 0,
                kX509NameFlagsRFC2253WithinUtf8JSON)
            < 0) {
            return false;
        }
        char* oline = nullptr;
        long n_bytes = BIO_get_mem_data(tmp, &oline);
        PrintAltName(out, oline, static_cast<size_t>(n_bytes), true, nullptr);
    } else if (gen->type == GEN_IPADD) {
        BIO_printf(out, "IP Address:");
        const ASN1_OCTET_STRING* ip = gen->d.ip;
        const unsigned char* b = ip->data;
        if (ip->length == 4) {
            BIO_printf(out, "%d.%d.%d.%d", b[0], b[1], b[2], b[3]);
        } else if (ip->length == 16) {
            for (unsigned int j = 0; j < 8; j++) {
                uint16_t pair = (b[2 * j] << 8) | b[2 * j + 1];
                BIO_printf(out, (j == 0) ? "%X" : ":%X", pair);
            }
        } else {
            BIO_printf(out, "<invalid length=%d>", ip->length);
        }
    } else if (gen->type == GEN_RID) {
        char oline[256];
        OBJ_obj2txt(oline, sizeof(oline), gen->d.rid, true);
        BIO_printf(out, "Registered ID:%s", oline);
    } else if (gen->type == GEN_OTHERNAME) {
        bool unicode = true;
        const char* prefix = nullptr;
        int nid = OBJ_obj2nid(gen->d.otherName->type_id);
        switch (nid) {
        case NID_id_on_SmtpUTF8Mailbox:
            prefix = "SmtpUTF8Mailbox";
            break;
        case NID_XmppAddr:
            prefix = "XmppAddr";
            break;
        case NID_SRVName:
            prefix = "SRVName";
            unicode = false;
            break;
        case NID_ms_upn:
            prefix = "UPN";
            break;
        case NID_NAIRealm:
            prefix = "NAIRealm";
            break;
        }
        int val_type = gen->d.otherName->value->type;
        if (prefix == nullptr || (unicode && val_type != V_ASN1_UTF8STRING) || (!unicode && val_type != V_ASN1_IA5STRING)) {
            BIO_printf(out, "othername:<unsupported>");
        } else {
            BIO_printf(out, "othername:");
            if (unicode) {
                PrintUtf8AltName(out, gen->d.otherName->value->value.utf8string,
                    prefix);
            } else {
                PrintLatin1AltName(out, gen->d.otherName->value->value.ia5string,
                    prefix);
            }
        }
    } else if (gen->type == GEN_X400) {
        BIO_printf(out, "X400Name:<unsupported>");
    } else if (gen->type == GEN_EDIPARTY) {
        BIO_printf(out, "EdiPartyName:<unsupported>");
    }

    return true;
}

bool SafeX509SubjectAltNamePrint(const BIOPointer& out, X509_EXTENSION* ext)
{
    const X509V3_EXT_METHOD* method = X509V3_EXT_get(ext);

    GENERAL_NAMES* names = static_cast<GENERAL_NAMES*>(X509V3_EXT_d2i(ext));
    if (names == nullptr)
        return false;

    bool ok = true;

    for (int i = 0; i < sk_GENERAL_NAME_num(names); i++) {
        GENERAL_NAME* gen = sk_GENERAL_NAME_value(names, i);

        if (i != 0)
            BIO_write(out, ", ", 2);

        if (!(ok = PrintGeneralName(out, gen))) {
            break;
        }
    }
    sk_GENERAL_NAME_pop_free(names, GENERAL_NAME_free);

    return ok;
}

bool SafeX509InfoAccessPrint(const BIOPointer& out, X509_EXTENSION* ext)
{
    const X509V3_EXT_METHOD* method = X509V3_EXT_get(ext);

    AUTHORITY_INFO_ACCESS* descs = static_cast<AUTHORITY_INFO_ACCESS*>(X509V3_EXT_d2i(ext));
    if (descs == nullptr)
        return false;

    bool ok = true;

    for (int i = 0; i < sk_ACCESS_DESCRIPTION_num(descs); i++) {
        ACCESS_DESCRIPTION* desc = sk_ACCESS_DESCRIPTION_value(descs, i);

        if (i != 0)
            BIO_write(out, "\n", 1);

        char objtmp[80];
        i2t_ASN1_OBJECT(objtmp, sizeof(objtmp), desc->method);
        BIO_printf(out, "%s - ", objtmp);
        if (!(ok = PrintGeneralName(out, desc->location))) {
            break;
        }
    }
    sk_ACCESS_DESCRIPTION_pop_free(descs, ACCESS_DESCRIPTION_free);

    return ok;
}

void AddFingerprintDigest(const unsigned char* md, unsigned int md_size,
    char fingerprint[3 * EVP_MAX_MD_SIZE])
{
    unsigned int i;
    const char hex[] = "0123456789ABCDEF";

    for (i = 0; i < md_size; i++) {
        fingerprint[3 * i] = hex[(md[i] & 0xf0) >> 4];
        fingerprint[(3 * i) + 1] = hex[(md[i] & 0x0f)];
        fingerprint[(3 * i) + 2] = ':';
    }

    fingerprint[(3 * (md_size - 1)) + 2] = '\0';
}

result_t GetFingerprintDigest(const EVP_MD* method, X509* cert, exlib::string& retVal)
{
    unsigned char md[EVP_MAX_MD_SIZE];
    unsigned int md_len;
    char fingerprint[EVP_MAX_MD_SIZE * 3];

    if (!X509_digest(cert, method, md, &md_len))
        return openssl_error();

    AddFingerprintDigest(md, md_len, fingerprint);

    char* hex = OPENSSL_buf2hexstr(md, md_len);
    if (hex == nullptr)
        return openssl_error();

    retVal = hex;
    OPENSSL_free(hex);
    return 0;
}

result_t X509Certificate_base::_new(Buffer_base* cert, obj_ptr<X509Certificate_base>& retVal,
    v8::Local<v8::Object> This)
{
    obj_ptr<X509Certificate> cert_ = new X509Certificate();
    result_t hr = cert_->load_cert(cert);
    if (hr != 0)
        return hr;

    retVal = cert_;
    return 0;
}

result_t X509Certificate::load_cert(Buffer_base* cert)
{
    Buffer* buf_cert = Buffer::Cast(cert);
    BIOPointer bio = BIO_new_mem_buf(buf_cert->data(), buf_cert->length());
    if (bio == nullptr)
        return openssl_error();

    X509Certificate* now = nullptr;

    while (true) {
        X509* cert_ = PEM_read_bio_X509(
            bio, nullptr, [](char*, int, int, void*) {
                return 0;
            },
            nullptr);
        if (cert_ == nullptr)
            return now ? 0 : openssl_error();

        if (now == nullptr) {
            now = this;
            m_cert = cert_;
        } else
            now = now->m_next = new X509Certificate(cert_);
    }

    return 0;
}

result_t X509Certificate::get_ca(bool& retVal)
{
    retVal = X509_check_ca(m_cert) != 0;
    return 0;
}

result_t X509Certificate::get_pathlen(int32_t& retVal)
{
    retVal = X509_get_pathlen(m_cert);
    return 0;
}

result_t X509Certificate::get_x509_array(int32_t nid, const char** names, v8::Local<v8::Array>& retVal)
{
    ASN1_BIT_STRING* usage = (ASN1_BIT_STRING*)X509_get_ext_d2i(m_cert, nid, nullptr, nullptr);
    if (!usage || usage->length == 0)
        return CALL_RETURN_UNDEFINED;

    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();
    unsigned char keyUsage = usage->data[0];

    v8::Local<v8::Array> arr = v8::Array::New(isolate->m_isolate);
    int j = 0;
    for (int32_t i = 0; i < 8; i++)
        if (keyUsage & (0x80 >> i))
            arr->Set(context, j++, isolate->NewString(names[i]));

    retVal = arr;

    return 0;
}

result_t X509Certificate::get_keyUsage(v8::Local<v8::Array>& retVal)
{
    return get_x509_array(NID_key_usage, xfKeyUsages, retVal);
}

result_t X509Certificate::get_type(v8::Local<v8::Array>& retVal)
{
    return get_x509_array(NID_netscape_cert_type, xfCertTypes, retVal);
}

result_t X509Certificate::get_fingerprint(exlib::string& retVal)
{
    return GetFingerprintDigest(EVP_sha1(), m_cert, retVal);
}

result_t X509Certificate::get_fingerprint256(exlib::string& retVal)
{
    return GetFingerprintDigest(EVP_sha256(), m_cert, retVal);
}

result_t X509Certificate::get_fingerprint512(exlib::string& retVal)
{
    return GetFingerprintDigest(EVP_sha512(), m_cert, retVal);
}

result_t X509Certificate::get_infoAccess(exlib::string& retVal)
{
    int index = X509_get_ext_by_NID(m_cert, NID_info_access, -1);
    if (index < 0)
        return CALL_RETURN_UNDEFINED;

    X509_EXTENSION* ext = X509_get_ext(m_cert, index);
    if (ext == nullptr)
        return openssl_error();

    BIOPointer bio(BIO_new(BIO_s_mem()));
    if (!SafeX509InfoAccessPrint(bio, ext))
        return openssl_error();

    return return_bio(bio, retVal);
}

result_t X509Certificate::get_issuer(exlib::string& retVal)
{
    X509_NAME* issuer_name = X509_get_issuer_name(m_cert);
    if (issuer_name == nullptr)
        return CALL_RETURN_UNDEFINED;

    BIOPointer bio(BIO_new(BIO_s_mem()));
    if (X509_NAME_print_ex(bio, issuer_name, 0, kX509NameFlagsMultiline) <= 0)
        return openssl_error();

    return return_bio(bio, retVal);
}

result_t X509Certificate::get_publicKey(obj_ptr<KeyObject_base>& retVal)
{
    if (m_publicKey) {
        retVal = m_publicKey;
        return 0;
    }

    EVP_PKEY* pkey = X509_get_pubkey(m_cert);
    if (pkey == nullptr)
        return openssl_error();

    obj_ptr<KeyObject> key = new KeyObject();
    result_t hr = key->createPublicKeyFromPKey(pkey);
    if (hr != 0)
        return hr;

    retVal = m_publicKey = key;
    return 0;
}

result_t X509Certificate::get_raw(obj_ptr<Buffer_base>& retVal)
{
    int size = i2d_X509(m_cert, nullptr);
    if (size < 0)
        return openssl_error();

    obj_ptr<Buffer> buf = new Buffer(NULL, size);
    unsigned char* p = buf->data();
    if (i2d_X509(m_cert, &p) < 0)
        return openssl_error();

    retVal = buf;

    return 0;
}

result_t X509Certificate::get_pem(exlib::string& retVal)
{
    BIOPointer bio(BIO_new(BIO_s_mem()));

    X509Certificate* now = this;

    while (now) {
        if (PEM_write_bio_X509(bio, now->m_cert) <= 0)
            return openssl_error();
        now = now->m_next;
    }

    return return_bio(bio, retVal);
}

result_t X509Certificate::get_serialNumber(exlib::string& retVal)
{
    ASN1_INTEGER* serial_number = X509_get_serialNumber(m_cert);
    if (serial_number == nullptr)
        return CALL_RETURN_UNDEFINED;

    BignumPointer bn = ASN1_INTEGER_to_BN(serial_number, nullptr);
    if (bn == nullptr)
        return openssl_error();

    char* hex = BN_bn2hex(bn);
    if (hex == nullptr)
        return openssl_error();

    retVal = hex;
    OPENSSL_free(hex);

    return 0;
}

result_t X509Certificate::get_subject(exlib::string& retVal)
{
    X509_NAME* name = X509_get_subject_name(m_cert);
    if (name == nullptr)
        return CALL_RETURN_UNDEFINED;

    BIOPointer bio(BIO_new(BIO_s_mem()));
    if (X509_NAME_print_ex(bio, name, 0, kX509NameFlagsMultiline) <= 0)
        return openssl_error();

    return return_bio(bio, retVal);
}

result_t X509Certificate::get_subjectAltName(exlib::string& retVal)
{
    int index = X509_get_ext_by_NID(m_cert, NID_subject_alt_name, -1);
    if (index < 0)
        return CALL_RETURN_UNDEFINED;

    X509_EXTENSION* ext = X509_get_ext(m_cert, index);
    if (ext == nullptr)
        return openssl_error();

    BIOPointer bio(BIO_new(BIO_s_mem()));
    if (!SafeX509SubjectAltNamePrint(bio, ext))
        return openssl_error();

    return return_bio(bio, retVal);
}

result_t X509Certificate::get_validFrom(exlib::string& retVal)
{
    BIOPointer bio(BIO_new(BIO_s_mem()));
    ASN1_TIME_print(bio, X509_get0_notBefore(m_cert));
    return return_bio(bio, retVal);
}

result_t X509Certificate::get_validTo(exlib::string& retVal)
{
    BIOPointer bio(BIO_new(BIO_s_mem()));
    ASN1_TIME_print(bio, X509_get0_notAfter(m_cert));
    return return_bio(bio, retVal);
}

result_t X509Certificate::get_next(obj_ptr<X509Certificate_base>& retVal)
{
    if (!m_next)
        return CALL_RETURN_UNDEFINED;

    retVal = m_next;
    return 0;
}

result_t X509Certificate::checkEmail(exlib::string email, v8::Local<v8::Object> options, exlib::string& retVal)
{
    Isolate* isolate = holder();
    int flags = 0;
    exlib::string subject("default");
    result_t hr = GetConfigValue(isolate, options, "subject", subject, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    if (subject == "always")
        flags = X509_CHECK_FLAG_ALWAYS_CHECK_SUBJECT;
    else if (subject == "never")
        flags = X509_CHECK_FLAG_NEVER_CHECK_SUBJECT;
    else if (subject != "default")
        return Runtime::setError("Invalid subject option");

    switch (X509_check_email(m_cert, email.c_str(), email.length(), flags)) {
    case 1:
        retVal = email;
        break;
    case 0:
        return CALL_RETURN_UNDEFINED;
    case -2:
        return Runtime::setError("Invalid email address");
    default:
        return openssl_error();
    }

    return 0;
}

result_t X509Certificate::checkHost(exlib::string name, v8::Local<v8::Object> options, exlib::string& retVal)
{
    Isolate* isolate = holder();
    int flags = 0;
    exlib::string subject("default");
    result_t hr = GetConfigValue(isolate, options, "subject", subject, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    if (subject == "always")
        flags = X509_CHECK_FLAG_ALWAYS_CHECK_SUBJECT;
    else if (subject == "never")
        flags = X509_CHECK_FLAG_NEVER_CHECK_SUBJECT;
    else if (subject != "default")
        return Runtime::setError("Invalid subject option");

    bool wildcards = true;
    hr = GetConfigValue(isolate, options, "wildcards", wildcards, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;
    if (!wildcards)
        flags |= X509_CHECK_FLAG_NO_WILDCARDS;

    bool partialWildcards = true;
    hr = GetConfigValue(isolate, options, "partialWildcards", partialWildcards, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;
    if (!partialWildcards)
        flags |= X509_CHECK_FLAG_NO_PARTIAL_WILDCARDS;

    bool multiLabelWildcards = false;
    hr = GetConfigValue(isolate, options, "multiLabelWildcards", multiLabelWildcards, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;
    if (!multiLabelWildcards)
        flags |= X509_CHECK_FLAG_SINGLE_LABEL_SUBDOMAINS;

    bool singleLabelSubdomains = false;
    hr = GetConfigValue(isolate, options, "singleLabelSubdomains", singleLabelSubdomains, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;
    if (singleLabelSubdomains)
        flags |= X509_CHECK_FLAG_SINGLE_LABEL_SUBDOMAINS;

    char* peername;
    switch (X509_check_host(m_cert, name.c_str(), name.length(), flags, &peername)) {
    case 1:
        if (peername != nullptr) {
            retVal = peername;
            OPENSSL_free(peername);
        }
        break;
    case 0:
        return CALL_RETURN_UNDEFINED;
    case -2:
        return Runtime::setError("Invalid host name");
    default:
        return openssl_error();
    }

    return 0;
}

result_t X509Certificate::checkIP(exlib::string ip, exlib::string& retVal)
{
    switch (X509_check_ip_asc(m_cert, ip.c_str(), 0)) {
    case 1:
        retVal = ip;
        break;
    case 0:
        return CALL_RETURN_UNDEFINED;
    case -2:
        return Runtime::setError("Invalid IP address");
    default:
        return openssl_error();
    }

    return 0;
}

result_t X509Certificate::checkIssued(X509Certificate_base* issuer, bool& retVal)
{
    X509Certificate* issuer_ = (X509Certificate*)issuer;
    retVal = X509_check_issued(issuer_->m_cert, m_cert) == X509_V_OK;
    return 0;
}

result_t X509Certificate::checkPrivateKey(KeyObject_base* privateKey, bool& retVal)
{
    KeyObject* key = (KeyObject*)privateKey;
    if (key->type() != KeyObject::kKeyTypePrivate)
        return CALL_E_INVALIDARG;

    retVal = X509_check_private_key(m_cert, key->pkey()) != 0;
    return 0;
}

result_t X509Certificate::verify(KeyObject_base* publicKey, bool& retVal)
{
    KeyObject* key = (KeyObject*)publicKey;
    if (key->type() != KeyObject::kKeyTypePublic)
        return CALL_E_INVALIDARG;

    retVal = X509_verify(m_cert, key->pkey()) == 1;
    return 0;
}

result_t X509Certificate::toString(exlib::string& retVal)
{
    return get_pem(retVal);
}

}
