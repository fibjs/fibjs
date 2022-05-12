/*
 * PKey_ecc.cpp
 *
 *  Created on: May 2, 2022
 *      Author: lion
 */

#include "Buffer.h"
#include "PKey_impl.h"
#include "ssl.h"

extern "C" {
int ecsdsa_sign(mbedtls_ecp_keypair* ctx, int sdsa, mbedtls_ecp_keypair* to_ctx, const unsigned char* hash, size_t hlen,
    unsigned char* sig, size_t* slen, int (*f_rng)(void*, unsigned char*, size_t), void* p_rng);
int ecsdsa_verify(mbedtls_ecp_keypair* ctx, int sdsa, mbedtls_ecp_keypair* to_ctx, const unsigned char* hash, size_t hlen,
    const unsigned char* sig, size_t slen, int (*f_rng)(void*, unsigned char*, size_t), void* p_rng);
}

namespace fibjs {

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

    { MBEDTLS_ECP_DP_CURVE448, "X448" },
    { MBEDTLS_ECP_DP_CURVE448, "x448" },

    { MBEDTLS_ECP_DP_ED25519, "Ed25519" },
    { MBEDTLS_ECP_DP_ED25519, "ed25519" },

    { MBEDTLS_ECP_DP_BLS12381_G1, "BLS12381_G1" },
    { MBEDTLS_ECP_DP_BLS12381_G2, "BLS12381_G2" },
};

int32_t PKey_ecc::get_curve_id(exlib::string& curve)
{
    int32_t i;

    for (i = 0; i < ARRAYSIZE(curves); i++)
        if (!strcmp(curve.c_str(), curves[i].name))
            return curves[i].id;

    return MBEDTLS_ECP_DP_NONE;
}

const char* PKey_ecc::get_curve_name(int32_t id)
{
    int32_t i;

    for (i = 0; i < ARRAYSIZE(curves); i++)
        if (curves[i].id == id)
            return curves[i].name;

    return NULL;
}

int PKey_ecc::load_group(mbedtls_ecp_group* grp, int32_t id)
{
    if (id >= MBEDTLS_ECP_DP_ED25519) {
        grp->id = (mbedtls_ecp_group_id)id;
        grp->pbits = 256;
        return 0;
    }

    return mbedtls_ecp_group_load(grp, (mbedtls_ecp_group_id)id);
}

PKey_ecc::PKey_ecc()
{
    mbedtls_pk_setup(&m_key, mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY));
}

PKey_ecc::PKey_ecc(mbedtls_pk_context& key)
    : PKey(key)
{
    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);
    int32_t id = ecp->grp.id;

    if (id < MBEDTLS_ECP_DP_ED25519) {
        m_alg = id == MBEDTLS_ECP_DP_SM2P256R1 ? "SM2" : "ECDSA";

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

                    // secp256k1_ec_pubkey_decompress(secp256k1_ctx, (unsigned char*)data.c_buffer(), &sz);
                    mbedtls_mpi_copy(&ecp->Q.Y, &r);
                    // mbedtls_mpi_read_binary(&ecp->Q.Y, (const unsigned char*)data.c_str() + ksz + 1, ksz);

                    mbedtls_mpi_free(&r);
                    mbedtls_mpi_free(&n);
                }
            }
        }
    }
}

PKey_ecc::PKey_ecc(int32_t id)
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

PKey_ecc* PKey_ecc::create(mbedtls_pk_context& key)
{
    int32_t id = mbedtls_pk_ec(key)->grp.id;

    if (id == MBEDTLS_ECP_DP_SECP256K1)
        return new PKey_p256k1(key);
    else if (id == MBEDTLS_ECP_DP_ED25519)
        return new PKey_25519(key);
    else if (id == MBEDTLS_ECP_DP_BLS12381_G1)
        return new PKey_bls_g1(key);
    else if (id == MBEDTLS_ECP_DP_BLS12381_G2)
        return new PKey_bls_g2(key);

    return new PKey_ecc(key);
}

result_t PKey_ecc::generateKey(exlib::string curve, obj_ptr<PKey_base>& retVal)
{
    int32_t id = get_curve_id(curve);
    switch (id) {
    case MBEDTLS_ECP_DP_NONE:
        return CHECK_ERROR(Runtime::setError("PKey: Unknown curve"));
    case MBEDTLS_ECP_DP_SECP256K1:
        retVal = new PKey_p256k1();
        break;
    case MBEDTLS_ECP_DP_ED25519:
        retVal = new PKey_25519();
        break;
    case MBEDTLS_ECP_DP_BLS12381_G1:
        retVal = new PKey_bls_g1();
        break;
    case MBEDTLS_ECP_DP_BLS12381_G2:
        retVal = new PKey_bls_g2();
        break;
    default:
        retVal = new PKey_ecc(id);
    }

    return 0;
}

result_t PKey_ecc::get_publicKey(obj_ptr<PKey_base>& retVal)
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

    obj_ptr<PKey_ecc> pk = PKey_ecc::create(ctx);
    pk->m_alg = m_alg;

    retVal = pk;

    return 0;
}

result_t PKey_ecc::isPrivate(bool& retVal)
{
    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);
    retVal = mbedtls_mpi_cmp_int(&ecp->d, 0);
    return 0;
}

result_t PKey_ecc::clone(obj_ptr<PKey_base>& retVal)
{
    mbedtls_pk_context ctx;

    mbedtls_pk_init(&ctx);
    mbedtls_pk_setup(&ctx, m_key.pk_info);

    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);
    mbedtls_ecp_keypair* ecp1 = mbedtls_pk_ec(ctx);

    load_group(&ecp1->grp, ecp->grp.id);
    mbedtls_mpi_copy(&ecp1->d, &ecp->d);
    mbedtls_ecp_copy(&ecp1->Q, &ecp->Q);

    obj_ptr<PKey_ecc> pk = PKey_ecc::create(ctx);
    pk->m_alg = m_alg;

    retVal = pk;

    return 0;
}

result_t PKey_ecc::equals(PKey_base* key, bool& retVal)
{
    retVal = false;

    obj_ptr<PKey> pkey = (PKey*)key;

    mbedtls_pk_type_t type = mbedtls_pk_get_type(&m_key);
    mbedtls_pk_type_t type1 = mbedtls_pk_get_type(&pkey->m_key);
    if (type != type1)
        return 0;

    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);
    mbedtls_ecp_keypair* ecp1 = mbedtls_pk_ec(pkey->m_key);

    if (ecp->grp.id != ecp1->grp.id)
        return 0;

    if (mbedtls_mpi_cmp_mpi(&ecp->Q.X, &ecp1->Q.X)
        || mbedtls_mpi_cmp_mpi(&ecp->Q.Y, &ecp1->Q.Y)
        || mbedtls_mpi_cmp_mpi(&ecp->d, &ecp1->d))
        return 0;

    retVal = true;
    return 0;
}

result_t PKey_ecc::sign(Buffer_base* data, PKey_base* key, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    result_t hr;

    obj_ptr<PKey> to_key = (PKey*)key;
    if (to_key) {
        mbedtls_pk_type_t type;

        type = mbedtls_pk_get_type(&to_key->m_key);
        if (type != MBEDTLS_PK_ECKEY && type != MBEDTLS_PK_SM2)
            return CHECK_ERROR(CALL_E_INVALIDARG);

        mbedtls_ecp_keypair* ecp1 = mbedtls_pk_ec(m_key);
        mbedtls_ecp_keypair* ecp2 = mbedtls_pk_ec(to_key->m_key);
        if (ecp1->grp.id != ecp2->grp.id)
            return CHECK_ERROR(Runtime::setError("Public key is not valid for specified curve"));
    }

    int32_t ret;
    exlib::string str;
    exlib::string output;
    size_t olen = MBEDTLS_ECDSA_MAX_LEN;

    data->toString(str);
    output.resize(MBEDTLS_ECDSA_MAX_LEN);

    ret = ecsdsa_sign(mbedtls_pk_ec(m_key), m_alg == "ECSDSA", key ? mbedtls_pk_ec(to_key->m_key) : NULL,
        (const unsigned char*)str.c_str(), str.length(), (unsigned char*)output.c_buffer(), &olen,
        mbedtls_ctr_drbg_random, &g_ssl.ctr_drbg);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    output.resize(olen);

    if (ac->m_ctx[1].string() == "bin") {
        hr = der2bin(output, output);
        if (hr < 0)
            return hr;
    }

    retVal = new Buffer(output);

    return 0;
}

result_t PKey_ecc::verify(Buffer_base* data, Buffer_base* sign, PKey_base* key, bool& retVal, AsyncEvent* ac)
{
    obj_ptr<PKey> to_key = (PKey*)key;
    if (key) {
        result_t hr;
        bool priv;

        mbedtls_pk_type_t type = mbedtls_pk_get_type(&to_key->m_key);
        if (type != MBEDTLS_PK_ECKEY && type != MBEDTLS_PK_SM2)
            return CHECK_ERROR(CALL_E_INVALIDARG);

        hr = to_key->isPrivate(priv);
        if (hr < 0)
            return hr;

        if (!priv)
            return CHECK_ERROR(CALL_E_INVALIDARG);

        mbedtls_ecp_keypair* ecp1 = mbedtls_pk_ec(m_key);
        mbedtls_ecp_keypair* ecp2 = mbedtls_pk_ec(to_key->m_key);
        if (ecp1->grp.id != ecp2->grp.id)
            return CHECK_ERROR(Runtime::setError("Public key is not valid for specified curve"));
    }

    int32_t ret;
    exlib::string str;
    exlib::string strsign;

    data->toString(str);
    sign->toString(strsign);

    if (ac->m_ctx[1].string() == "bin") {
        result_t hr = bin2der(strsign, strsign);
        if (hr < 0)
            return hr;
    }

    ret = ecsdsa_verify(mbedtls_pk_ec(m_key), m_alg == "ECSDSA", key ? mbedtls_pk_ec(to_key->m_key) : NULL,
        (const unsigned char*)str.c_str(), str.length(), (const unsigned char*)strsign.c_str(), strsign.length(),
        mbedtls_ctr_drbg_random, &g_ssl.ctr_drbg);
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

result_t PKey_ecc::der2bin(const exlib::string& der, exlib::string& bin)
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

result_t PKey_ecc::bin2der(const exlib::string& bin, exlib::string& der)
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

    *sig++ = 0x30;

    if ((4 + lenS + lenR) > 0x80)
        *sig++ = 0x81;
    *sig++ = (unsigned char)(4 + lenS + lenR);

    *sig++ = 0x02;
    *sig++ = (unsigned char)lenR;
    if (rp[0] & 0x80) {
        *sig++ = 0;
        lenR--;
    }
    memcpy(sig, rp, lenR);
    sig += lenR;

    *sig++ = 0x02;
    *sig++ = (unsigned char)lenS;
    if (sp[0] & 0x80) {
        *sig++ = 0;
        lenS--;
    }
    memcpy(sig, sp, lenS);

    der = r;

    return 0;
}

result_t PKey_ecc::check_opts(v8::Local<v8::Object> opts, AsyncEvent* ac)
{
    static const char* s_keys[] = {
        "to", "format", NULL
    };

    if (!ac->isSync())
        return 0;

    Isolate* isolate = holder();
    result_t hr;

    hr = CheckConfig(opts, s_keys);
    if (hr < 0)
        return hr;

    ac->m_ctx.resize(2);

    obj_ptr<PKey_base> to;
    hr = GetConfigValue(isolate->m_isolate, opts, "to", to, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;
    ac->m_ctx[0] = to;

    exlib::string fmt = "der";
    hr = GetConfigValue(isolate->m_isolate, opts, "format", fmt, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;
    if (fmt != "der" && fmt != "bin")
        return CHECK_ERROR(Runtime::setError(exlib::string("unsupported format \'") + fmt + "\'."));
    ac->m_ctx[1] = fmt;

    return CHECK_ERROR(CALL_E_NOSYNC);
}

result_t PKey_ecc::sign(Buffer_base* data, v8::Local<v8::Object> opts, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
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

    obj_ptr<PKey_base> to = PKey_base::getInstance(ac->m_ctx[0].object());
    if (m_alg == "ECSDSA" || m_alg == "SM2")
        return sign(data, to, retVal, ac);

    if (to)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    int32_t ret;
    exlib::string str;
    exlib::string output;
    size_t olen = MBEDTLS_ECDSA_MAX_SIG_LEN(mbedtls_pk_get_bitlen(&m_key));

    data->toString(str);
    output.resize(olen);

    // alg=0~9  see https://tls.mbed.org/api/md_8h.html  enum mbedtls_md_type_t
    ret = mbedtls_pk_sign(&m_key, MBEDTLS_MD_NONE,
        (const unsigned char*)str.c_str(), str.length(),
        (unsigned char*)output.c_buffer(), olen, &olen,
        mbedtls_ctr_drbg_random, &g_ssl.ctr_drbg);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    output.resize(olen);

    if (ac->m_ctx[1].string() == "bin") {
        hr = der2bin(output, output);
        if (hr < 0)
            return hr;
    }

    retVal = new Buffer(output);

    return 0;
}

result_t PKey_ecc::verify(Buffer_base* data, Buffer_base* sign, v8::Local<v8::Object> opts, bool& retVal, AsyncEvent* ac)
{
    result_t hr = check_opts(opts, ac);
    if (hr < 0)
        return hr;

    obj_ptr<PKey_base> to = PKey_base::getInstance(ac->m_ctx[0].object());
    if (m_alg == "ECSDSA" || m_alg == "SM2")
        return verify(data, sign, to, retVal, ac);

    if (to)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    int32_t ret;
    exlib::string str;
    exlib::string strsign;

    data->toString(str);
    sign->toString(strsign);

    if (ac->m_ctx[1].string() == "bin") {
        hr = bin2der(strsign, strsign);
        if (hr < 0)
            return hr;
    }

    ret = mbedtls_pk_verify(&m_key, MBEDTLS_MD_NONE,
        (const unsigned char*)str.c_str(), str.length(),
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

result_t PKey_ecc::computeSecret(PKey_base* publicKey, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr;
    bool priv;
    mbedtls_pk_type_t type;

    obj_ptr<PKey> pubkey = (PKey*)publicKey;
    type = mbedtls_pk_get_type(&pubkey->m_key);
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
    mbedtls_ecp_keypair* ecp2 = mbedtls_pk_ec(pubkey->m_key);
    if (ecp1->grp.id != ecp2->grp.id)
        return CHECK_ERROR(Runtime::setError("Public key is not valid for specified curve"));

    int32_t ret;
    mbedtls_mpi z;
    mbedtls_mpi_init(&z);
    ret = mbedtls_ecdh_compute_shared(&ecp1->grp, &z, &ecp2->Q, &ecp1->d,
        mbedtls_ctr_drbg_random, &g_ssl.ctr_drbg);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    exlib::string data;
    int32_t sz = (int32_t)mbedtls_mpi_size(&z);

    data.resize(sz);
    mbedtls_mpi_write_binary(&z, (unsigned char*)data.c_buffer(), sz);
    mbedtls_mpi_free(&z);

    retVal = new Buffer(data);

    return 0;
}

result_t PKey_ecc::get_curve(exlib::string& retVal)
{
    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);
    const char* _name = get_curve_name(ecp->grp.id);
    if (_name)
        retVal = _name;

    return 0;
}

result_t PKey_ecc::set_alg(exlib::string newVal)
{
    int32_t id = mbedtls_pk_ec(m_key)->grp.id;

    if (id >= MBEDTLS_ECP_DP_ED25519)
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
