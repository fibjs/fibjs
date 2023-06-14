/*
 * ECKey.cpp
 *
 *  Created on: May 2, 2022
 *      Author: lion
 */

#include "Buffer.h"
#include "Ed25519Key.h"
#include "BlsKey.h"
#include "ECKey_p256k1.h"
#include "ssl.h"

extern "C" {
int ecsdsa_sign(mbedtls_ecp_keypair* ctx, const unsigned char* hash, size_t hlen,
    unsigned char* sig, size_t* slen, int (*f_rng)(void*, unsigned char*, size_t), void* p_rng);
int ecsdsa_verify(mbedtls_ecp_keypair* ctx, const unsigned char* hash, size_t hlen,
    const unsigned char* sig, size_t slen, int (*f_rng)(void*, unsigned char*, size_t), void* p_rng);
}

namespace fibjs {

result_t ECKey_base::_new(Buffer_base* DerKey, exlib::string password, obj_ptr<ECKey_base>& retVal,
    v8::Local<v8::Object> This)
{
    obj_ptr<PKey_base> key;

    result_t hr = PKey_base::from(DerKey, password, key);
    if (hr < 0)
        return hr;

    retVal = dynamic_cast<ECKey_base*>((PKey_base*)key);
    return retVal ? 0 : CHECK_ERROR(_ssl::setError(MBEDTLS_ERR_PK_KEY_INVALID_FORMAT));
}

result_t ECKey_base::_new(exlib::string pemKey, exlib::string password, obj_ptr<ECKey_base>& retVal,
    v8::Local<v8::Object> This)
{
    obj_ptr<PKey_base> key;

    result_t hr = PKey_base::from(pemKey, password, key);
    if (hr < 0)
        return hr;

    retVal = dynamic_cast<ECKey_base*>((PKey_base*)key);
    return retVal ? 0 : CHECK_ERROR(_ssl::setError(MBEDTLS_ERR_PK_KEY_INVALID_FORMAT));
}

result_t ECKey_base::_new(v8::Local<v8::Object> jsonKey, obj_ptr<ECKey_base>& retVal,
    v8::Local<v8::Object> This)
{
    obj_ptr<PKey_base> key;

    result_t hr = PKey_base::from(jsonKey, key);
    if (hr < 0)
        return hr;

    retVal = dynamic_cast<ECKey_base*>((PKey_base*)key);
    return retVal ? 0 : CHECK_ERROR(_ssl::setError(MBEDTLS_ERR_PK_KEY_INVALID_FORMAT));
}

struct curve_info {
    int32_t id;
    const char* name;
};

static const curve_info curves[] = {
    { MBEDTLS_ECP_DP_SECP192R1, "P-192" },
    { MBEDTLS_ECP_DP_SECP192R1, "NIST P-192" },
    { MBEDTLS_ECP_DP_SECP192R1, "p192" },
    { MBEDTLS_ECP_DP_SECP192R1, "prime192v1" },
    { MBEDTLS_ECP_DP_SECP192R1, "secp192r1" },

    { MBEDTLS_ECP_DP_SECP224R1, "P-224" },
    { MBEDTLS_ECP_DP_SECP224R1, "NIST P-224" },
    { MBEDTLS_ECP_DP_SECP224R1, "p224" },
    { MBEDTLS_ECP_DP_SECP224R1, "prime224v1" },
    { MBEDTLS_ECP_DP_SECP224R1, "secp224r1" },

    { MBEDTLS_ECP_DP_SECP256R1, "P-256" },
    { MBEDTLS_ECP_DP_SECP256R1, "NIST P-256" },
    { MBEDTLS_ECP_DP_SECP256R1, "p256" },
    { MBEDTLS_ECP_DP_SECP256R1, "prime256v1" },
    { MBEDTLS_ECP_DP_SECP256R1, "secp256r1" },

    { MBEDTLS_ECP_DP_SECP384R1, "P-384" },
    { MBEDTLS_ECP_DP_SECP384R1, "NIST P-384" },
    { MBEDTLS_ECP_DP_SECP384R1, "p384" },
    { MBEDTLS_ECP_DP_SECP384R1, "prime384v1" },
    { MBEDTLS_ECP_DP_SECP384R1, "secp384r1" },

    { MBEDTLS_ECP_DP_SECP521R1, "P-521" },
    { MBEDTLS_ECP_DP_SECP521R1, "NIST P-521" },
    { MBEDTLS_ECP_DP_SECP521R1, "p521" },
    { MBEDTLS_ECP_DP_SECP521R1, "prime521v1" },
    { MBEDTLS_ECP_DP_SECP521R1, "secp521r1" },

    { MBEDTLS_ECP_DP_BP256R1, "brainpoolP256r1" },
    { MBEDTLS_ECP_DP_BP384R1, "brainpoolP384r1" },
    { MBEDTLS_ECP_DP_BP512R1, "brainpoolP512r1" },

    { MBEDTLS_ECP_DP_SECP192K1, "secp192k1" },
    { MBEDTLS_ECP_DP_SECP224K1, "secp224k1" },
    { MBEDTLS_ECP_DP_SECP256K1, "secp256k1" },
    { MBEDTLS_ECP_DP_SECP256K1, "P-256K" },

    { MBEDTLS_ECP_DP_SM2P256R1, "SM2" },
    { MBEDTLS_ECP_DP_SM2P256R1, "sm2" },
    { MBEDTLS_ECP_DP_SM2P256R1, "sm2p256r1" },

    { MBEDTLS_ECP_DP_CURVE25519, "X25519" },
    { MBEDTLS_ECP_DP_CURVE25519, "x25519" },

    { MBEDTLS_ECP_DP_ED25519, "Ed25519" },
    { MBEDTLS_ECP_DP_ED25519, "ed25519" },

    { MBEDTLS_ECP_DP_BLS12381_G1, "BLS12381_G1" },
    { MBEDTLS_ECP_DP_BLS12381_G2, "BLS12381_G2" },
};

int32_t ECKey::get_curve_id(exlib::string& curve)
{
    int32_t i;

    for (i = 0; i < ARRAYSIZE(curves); i++)
        if (!strcmp(curve.c_str(), curves[i].name))
            return curves[i].id;

    return MBEDTLS_ECP_DP_NONE;
}

const char* ECKey::get_curve_name(int32_t id)
{
    int32_t i;

    for (i = 0; i < ARRAYSIZE(curves); i++)
        if (curves[i].id == id)
            return curves[i].name;

    return NULL;
}

int ECKey::load_group(mbedtls_ecp_group* grp, int32_t id)
{
    if (id >= MBEDTLS_ECP_DP_ED25519) {
        grp->id = (mbedtls_ecp_group_id)id;
        grp->pbits = 256;
        return 0;
    }

    return mbedtls_ecp_group_load(grp, (mbedtls_ecp_group_id)id);
}

void ECKey::init(mbedtls_pk_context& key, bool genpub, exlib::string algo)
{
    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);
    int32_t id = ecp->grp.id;
    m_alg = !algo.empty() ? algo : (id == MBEDTLS_ECP_DP_SM2P256R1 ? "SM2" : "ECDSA");

    if (genpub) {
        if (mbedtls_mpi_cmp_int(&ecp->d, 0) && !mbedtls_mpi_cmp_int(&ecp->Q.X, 0))
            mbedtls_ecp_mul(&ecp->grp, &ecp->Q, &ecp->d, &ecp->grp.G, mbedtls_ctr_drbg_random, &g_ssl.ctr_drbg);
        else if (id == MBEDTLS_ECP_DP_SECP192R1 || id == MBEDTLS_ECP_DP_SECP192K1
            || id == MBEDTLS_ECP_DP_SECP256R1 || id == MBEDTLS_ECP_DP_BP256R1
            || id == MBEDTLS_ECP_DP_SECP384R1 || id == MBEDTLS_ECP_DP_BP384R1
            || id == MBEDTLS_ECP_DP_SECP521R1 || id == MBEDTLS_ECP_DP_BP512R1
            || id == MBEDTLS_ECP_DP_SM2P256R1) {
            size_t ksz = (mbedtls_pk_get_bitlen(&m_key) + 7) / 8;
            int32_t sz = (int32_t)mbedtls_mpi_size(&ecp->Q.X);

            if (!mbedtls_mpi_cmp_int(&ecp->Q.Y, 0) && sz == (ksz + 1)) {
                exlib::string data;

                data.resize(ksz + 1);
                mbedtls_mpi_write_binary(&ecp->Q.X, (unsigned char*)data.c_buffer(), ksz + 1);
                if (data[0] == 2 || data[0] == 3) {
                    mbedtls_mpi_read_binary(&ecp->Q.X, (const unsigned char*)data.c_str() + 1, ksz);

                    mbedtls_mpi r, n;

                    mbedtls_mpi_init(&r);
                    mbedtls_mpi_init(&n);

                    // r = x^2
                    mbedtls_mpi_mul_mpi(&r, &ecp->Q.X, &ecp->Q.X);

                    // r = x^2 + a
                    if (ecp->grp.A.p == NULL) {
                        // Special case where a is -3
                        mbedtls_mpi_sub_int(&r, &r, 3);
                    } else {
                        mbedtls_mpi_add_mpi(&r, &r, &ecp->grp.A);
                    }

                    // r = x^3 + ax
                    mbedtls_mpi_mul_mpi(&r, &r, &ecp->Q.X);

                    // r = x^3 + ax + b
                    mbedtls_mpi_add_mpi(&r, &r, &ecp->grp.B);

                    // Calculate square root of r over finite field P:
                    //   r = sqrt(x^3 + ax + b) = (x^3 + ax + b) ^ ((P + 1) / 4) (mod P)

                    // n = P + 1
                    mbedtls_mpi_add_int(&n, &ecp->grp.P, 1);

                    // n = (P + 1) / 4
                    mbedtls_mpi_shift_r(&n, 2);

                    // r ^ ((P + 1) / 4) (mod p)
                    mbedtls_mpi_exp_mod(&r, &r, &n, &ecp->grp.P, NULL);

                    // Select solution that has the correct "sign" (equals odd/even solution in finite group)
                    if ((data[0] == 0x03) != mbedtls_mpi_get_bit(&r, 0)) {
                        // r = p - r
                        mbedtls_mpi_sub_mpi(&r, &ecp->grp.P, &r);
                    }

                    mbedtls_mpi_copy(&ecp->Q.Y, &r);

                    mbedtls_mpi_free(&r);
                    mbedtls_mpi_free(&n);
                }
            }
        }
    }
}

void ECKey::init(int32_t id)
{
    mbedtls_pk_type_t pk_type;

    if (id == MBEDTLS_ECP_DP_SM2P256R1) {
        pk_type = MBEDTLS_PK_SM2;
        m_alg = "SM2";
    } else {
        pk_type = MBEDTLS_PK_ECKEY;
        m_alg = "ECDSA";
    }

    mbedtls_pk_setup(&m_key, mbedtls_pk_info_from_type(pk_type));
    mbedtls_ecp_gen_key((mbedtls_ecp_group_id)id, mbedtls_pk_ec(m_key),
        mbedtls_ctr_drbg_random, &g_ssl.ctr_drbg);
}

ECKey_base* ECKey::create(mbedtls_pk_context& key, exlib::string algo)
{
    int32_t id = mbedtls_pk_ec(key)->grp.id;

    switch (id) {
    case MBEDTLS_ECP_DP_SECP256K1:
        return new ECKey_p256k1(key, algo);
    case MBEDTLS_ECP_DP_CURVE25519:
    case MBEDTLS_ECP_DP_ED25519:
        return new Ed25519Key(key);
    case MBEDTLS_ECP_DP_BLS12381_G1:
        return new BlsKey_g1(key);
    case MBEDTLS_ECP_DP_BLS12381_G2:
        return new BlsKey_g2(key);
    }

    return new ECKey_impl<ECKey_base>(key, true, algo);
}

result_t ECKey::generateKey(exlib::string curve, obj_ptr<PKey_base>& retVal)
{
    int32_t id = get_curve_id(curve);

    switch (id) {
    case MBEDTLS_ECP_DP_NONE:
        return CHECK_ERROR(Runtime::setError("PKey: Unknown curve"));
    case MBEDTLS_ECP_DP_SECP256K1:
        retVal = new ECKey_p256k1();
        break;
    case MBEDTLS_ECP_DP_CURVE25519:
    case MBEDTLS_ECP_DP_ED25519:
        retVal = new Ed25519Key(id);
        break;
    case MBEDTLS_ECP_DP_BLS12381_G1:
        retVal = new BlsKey_g1();
        break;
    case MBEDTLS_ECP_DP_BLS12381_G2:
        retVal = new BlsKey_g2();
        break;
    default:
        retVal = new ECKey_impl<ECKey_base>(id);
    }

    return 0;
}

result_t ECKey::get_publicKey(obj_ptr<PKey_base>& retVal)
{
    bool priv;

    result_t hr = isPrivate(priv);
    if (hr < 0)
        return hr;

    if (!priv)
        return CALL_RETURN_NULL;

    mbedtls_pk_context ctx;

    mbedtls_pk_init(&ctx);
    mbedtls_pk_setup(&ctx, m_key.pk_info);

    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);
    mbedtls_ecp_keypair* ecp1 = mbedtls_pk_ec(ctx);

    load_group(&ecp1->grp, ecp->grp.id);
    mbedtls_ecp_copy(&ecp1->Q, &ecp->Q);

    retVal = ECKey::create(ctx, m_alg);

    return 0;
}

result_t ECKey::clone(obj_ptr<PKey_base>& retVal)
{
    mbedtls_pk_context ctx;

    mbedtls_pk_init(&ctx);
    mbedtls_pk_setup(&ctx, m_key.pk_info);

    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);
    mbedtls_ecp_keypair* ecp1 = mbedtls_pk_ec(ctx);

    load_group(&ecp1->grp, ecp->grp.id);
    mbedtls_mpi_copy(&ecp1->d, &ecp->d);
    mbedtls_ecp_copy(&ecp1->Q, &ecp->Q);

    retVal = ECKey::create(ctx, m_alg);

    return 0;
}

result_t ECKey::equals(object_base* key, bool& retVal)
{
    retVal = false;

    PKey_base* _key = PKey_base::getInstance(key);
    if (!_key)
        return CALL_E_TYPEMISMATCH;

    mbedtls_pk_context& mkey = PKey::key(_key);

    mbedtls_pk_type_t type = mbedtls_pk_get_type(&m_key);
    mbedtls_pk_type_t type1 = mbedtls_pk_get_type(&mkey);
    if (type != type1)
        return 0;

    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);
    mbedtls_ecp_keypair* ecp1 = mbedtls_pk_ec(mkey);

    if (ecp->grp.id != ecp1->grp.id)
        return 0;

    if (mbedtls_mpi_cmp_mpi(&ecp->Q.X, &ecp1->Q.X)
        || mbedtls_mpi_cmp_mpi(&ecp->Q.Y, &ecp1->Q.Y)
        || mbedtls_mpi_cmp_mpi(&ecp->d, &ecp1->d))
        return 0;

    retVal = true;
    return 0;
}

result_t ECKey::sdsa_sign(Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    result_t hr;

    int32_t ret;
    exlib::string output;
    size_t olen = MBEDTLS_ECDSA_MAX_LEN;

    obj_ptr<Buffer> buf_data = Buffer::Cast(data);
    output.resize(MBEDTLS_ECDSA_MAX_LEN);

    ret = ecsdsa_sign(mbedtls_pk_ec(m_key), buf_data->data(), buf_data->length(),
        (unsigned char*)output.c_buffer(), &olen, mbedtls_ctr_drbg_random, &g_ssl.ctr_drbg);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    output.resize(olen);

    if (ac->m_ctx[0].string() == "raw") {
        hr = der2bin(output, output);
        if (hr < 0)
            return hr;
    }

    retVal = new Buffer(output.c_str(), output.length());

    return 0;
}

result_t ECKey::sdsa_verify(Buffer_base* data, Buffer_base* sign, bool& retVal, AsyncEvent* ac)
{
    int32_t ret;

    obj_ptr<Buffer> buf_data = Buffer::Cast(data);

    exlib::string strsign;
    sign->toString(strsign);
    if (ac->m_ctx[0].string() == "raw") {
        result_t hr = bin2der(strsign, strsign);
        if (hr < 0)
            return hr;
    }

    ret = ecsdsa_verify(mbedtls_pk_ec(m_key), buf_data->data(), buf_data->length(),
        (const unsigned char*)strsign.c_str(), strsign.length(), mbedtls_ctr_drbg_random, &g_ssl.ctr_drbg);
    if (ret == MBEDTLS_ERR_ECP_VERIFY_FAILED || ret == MBEDTLS_ERR_SM2_BAD_SIGNATURE) {
        retVal = false;
        return 0;
    }

    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    retVal = true;

    return 0;
}

static int asn1_get_num(unsigned char** p, const unsigned char* end, unsigned char* data, size_t sz)
{
    int ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    size_t len;

    if ((ret = mbedtls_asn1_get_tag(p, end, &len, MBEDTLS_ASN1_INTEGER)) != 0)
        return (ret);

    while (len && !**p) {
        len--;
        (*p)++;
    }

    if (len > sz)
        return MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL;

    if (len < sz)
        memset(data, 0, sz - len);
    memcpy(data + sz - len, *p, len);

    *p += len;

    return (ret);
}

result_t ECKey::der2bin(const exlib::string& der, exlib::string& bin)
{
    const unsigned char* data = (const unsigned char*)der.c_str();
    size_t datlen = der.length();
    const unsigned char* end = data + datlen;
    size_t ksz = (mbedtls_pk_get_bitlen(&m_key) + 7) / 8;
    unsigned char* p = (unsigned char*)data;
    size_t len;
    exlib::string r;

    r.resize(ksz * 2);

    unsigned char* sig = (unsigned char*)r.c_buffer();

    if (mbedtls_asn1_get_tag(&p, end, &len, MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE))
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;

    if (p + len != end)
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA + MBEDTLS_ERR_ASN1_LENGTH_MISMATCH;

    if (asn1_get_num(&p, end, sig, ksz) || asn1_get_num(&p, end, sig + ksz, ksz))
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;

    bin = r;

    return 0;
}

result_t ECKey::bin2der(const exlib::string& bin, exlib::string& der)
{
    const unsigned char* p0 = (const unsigned char*)bin.c_str();
    size_t sz = bin.length();
    size_t ksz = (mbedtls_pk_get_bitlen(&m_key) + 7) / 8;
    size_t size;
    exlib::string r;

    if (sz != ksz * 2)
        return MBEDTLS_ERR_ECP_VERIFY_FAILED;

    const unsigned char *rp = p0, *sp = p0 + ksz;
    size_t lenR = ksz, lenS = ksz;

    while (lenR > 1 && rp[0] == 0) {
        lenR--;
        rp++;
    }
    if (rp[0] & 0x80)
        lenR++;

    while (lenS > 1 && sp[0] == 0) {
        lenS--;
        sp++;
    }
    if (sp[0] & 0x80)
        lenS++;

    size = 6 + lenS + lenR;
    if ((4 + lenS + lenR) > 0x80)
        size++;

    r.resize(size);
    unsigned char* sig = (unsigned char*)r.c_buffer();

    *sig++ = MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE;

    if ((4 + lenS + lenR) > 0x80)
        *sig++ = 0x81;
    *sig++ = (unsigned char)(4 + lenS + lenR);

    *sig++ = MBEDTLS_ASN1_INTEGER;
    *sig++ = (unsigned char)lenR;
    if (rp[0] & 0x80) {
        *sig++ = 0;
        lenR--;
    }
    memcpy(sig, rp, lenR);
    sig += lenR;

    *sig++ = MBEDTLS_ASN1_INTEGER;
    *sig++ = (unsigned char)lenS;
    if (sp[0] & 0x80) {
        *sig++ = 0;
        lenS--;
    }
    memcpy(sig, sp, lenS);

    der = r;

    return 0;
}

result_t ECKey::check_opts(v8::Local<v8::Object> opts, AsyncEvent* ac)
{
    static const char* s_keys[] = {
        "format", NULL
    };

    if (!ac->isSync())
        return 0;

    Isolate* isolate = Isolate::current(opts);
    result_t hr;

    hr = CheckConfig(opts, s_keys);
    if (hr < 0)
        return hr;

    ac->m_ctx.resize(1);

    exlib::string fmt = "der";
    hr = GetConfigValue(isolate, opts, "format", fmt, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;
    if (fmt != "der" && fmt != "raw")
        return CHECK_ERROR(Runtime::setError(exlib::string("unsupported format \'") + fmt + "\'."));
    ac->m_ctx[0] = fmt;

    return CHECK_ERROR(CALL_E_NOSYNC);
}

result_t ECKey::sign(Buffer_base* data, v8::Local<v8::Object> opts, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    result_t hr = check_opts(opts, ac);
    if (hr < 0)
        return hr;

    bool priv;

    hr = isPrivate(priv);
    if (hr < 0)
        return hr;

    if (!priv)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (m_alg == "ECSDSA")
        return sdsa_sign(data, retVal, ac);

    int32_t ret;
    exlib::string output;
    size_t olen = MBEDTLS_ECDSA_MAX_SIG_LEN(mbedtls_pk_get_bitlen(&m_key));

    obj_ptr<Buffer> buf_data = Buffer::Cast(data);
    output.resize(olen);

    // alg=0~9  see https://tls.mbed.org/api/md_8h.html  enum mbedtls_md_type_t
    ret = mbedtls_pk_sign(&m_key, MBEDTLS_MD_NONE, buf_data->data(), buf_data->length(),
        (unsigned char*)output.c_buffer(), olen, &olen,
        mbedtls_ctr_drbg_random, &g_ssl.ctr_drbg);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    output.resize(olen);

    if (ac->m_ctx[0].string() == "raw") {
        hr = der2bin(output, output);
        if (hr < 0)
            return hr;
    }

    retVal = new Buffer(output.c_str(), output.length());

    return 0;
}

result_t ECKey::verify(Buffer_base* data, Buffer_base* sign, v8::Local<v8::Object> opts, bool& retVal, AsyncEvent* ac)
{
    result_t hr = check_opts(opts, ac);
    if (hr < 0)
        return hr;

    if (m_alg == "ECSDSA")
        return sdsa_verify(data, sign, retVal, ac);

    int32_t ret;
    exlib::string strsign;

    obj_ptr<Buffer> buf_data = Buffer::Cast(data);
    sign->toString(strsign);

    if (ac->m_ctx[0].string() == "raw") {
        hr = bin2der(strsign, strsign);
        if (hr < 0)
            return hr;
    }

    ret = mbedtls_pk_verify(&m_key, MBEDTLS_MD_NONE, buf_data->data(), buf_data->length(),
        (const unsigned char*)strsign.c_str(), strsign.length());
    if (ret == MBEDTLS_ERR_ECP_VERIFY_FAILED) {
        retVal = false;
        return 0;
    }

    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    retVal = true;

    return 0;
}

result_t ECKey::computeSecret(ECKey_base* publicKey, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr;
    bool priv;
    mbedtls_pk_type_t type;

    mbedtls_pk_context& mkey = PKey::key(publicKey);

    type = mbedtls_pk_get_type(&mkey);
    if (type != MBEDTLS_PK_ECKEY && type != MBEDTLS_PK_SM2)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    type = mbedtls_pk_get_type(&m_key);
    if (type != MBEDTLS_PK_ECKEY && type != MBEDTLS_PK_SM2)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    hr = isPrivate(priv);
    if (hr < 0)
        return hr;

    if (!priv)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    mbedtls_ecp_keypair* ecp1 = mbedtls_pk_ec(m_key);
    mbedtls_ecp_keypair* ecp2 = mbedtls_pk_ec(mkey);
    if (ecp1->grp.id != ecp2->grp.id)
        return CHECK_ERROR(Runtime::setError("Public key is not valid for specified curve"));

    int32_t ret;
    mbedtls_mpi z;
    mbedtls_mpi_init(&z);
    ret = mbedtls_ecdh_compute_shared(&ecp1->grp, &z, &ecp2->Q, &ecp1->d,
        mbedtls_ctr_drbg_random, &g_ssl.ctr_drbg);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    int32_t sz = (int32_t)mbedtls_mpi_size(&z);

    obj_ptr<Buffer> data = new Buffer(NULL, sz);
    mbedtls_mpi_write_binary(&z, data->data(), sz);
    mbedtls_mpi_free(&z);

    retVal = data;

    return 0;
}

result_t ECKey::get_curve(exlib::string& retVal)
{
    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);
    const char* _name = get_curve_name(ecp->grp.id);
    if (_name)
        retVal = _name;

    return 0;
}

result_t ECKey::set_alg(exlib::string newVal)
{
    int32_t id = mbedtls_pk_ec(m_key)->grp.id;

    if (id >= MBEDTLS_ECP_DP_ED25519 || id == MBEDTLS_ECP_DP_CURVE25519)
        return PKey::set_alg(newVal);
    else if (id == MBEDTLS_ECP_DP_SM2P256R1) {
        if (newVal != "SM2" && newVal != "ECSDSA")
            return CHECK_ERROR(CALL_E_INVALIDARG);
        m_alg = newVal;
    } else {
        if (newVal != "ECDSA" && newVal != "ECSDSA")
            return CHECK_ERROR(CALL_E_INVALIDARG);
        m_alg = newVal;
    }

    return 0;
}

}
