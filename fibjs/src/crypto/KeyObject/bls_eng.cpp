/*
 * bls_eng.cpp
 *
 *  Created on: jul 27, 2024
 *      Author: lion
 */

#include "crypto_util.h"
#include <openssl/engine.h>
#include <crypto/asn1.h>
#include <blst/include/blst.h>
#include <iostream>

namespace fibjs {

#define BLS12_381_KEYLEN 32
#define BLS12_381_G1_LEN 48
#define BLS12_381_G2_LEN 96
#define BLS12_381_G1_SIG_LEN 96
#define BLS12_381_G2_SIG_LEN 48

#define SIGLEN(p) ((p)->ameth->pkey_id == EVP_PKEY_BLS12_381_G1 ? BLS12_381_G1_SIG_LEN : BLS12_381_G2_SIG_LEN)
#define KEYLEN(p) ((p)->ameth->pkey_id == EVP_PKEY_BLS12_381_G1 ? BLS12_381_G1_LEN : BLS12_381_G2_LEN)

const unsigned char DST_G1_POP[] = "BLS_SIG_BLS12381G2_XMD:SHA-256_SSWU_RO_POP_";
const unsigned char DST_G2_POP[] = "BLS_SIG_BLS12381G1_XMD:SHA-256_SSWU_RO_POP_";

static ECX_KEY* bls_new_key()
{
    ECX_KEY* key = ossl_ecx_key_new(NULL, ECX_KEY_TYPE_ED25519, 1, NULL);
    if (key == NULL) {
        ERR_raise(ERR_LIB_EC, ERR_R_MALLOC_FAILURE);
        return 0;
    }

    key->keylen = BLS12_381_KEYLEN;
    key->privkey = (unsigned char*)OPENSSL_secure_malloc(BLS12_381_KEYLEN);
    if (key->privkey == NULL) {
        OPENSSL_free(key);
        ERR_raise(ERR_LIB_EC, ERR_R_MALLOC_FAILURE);
        return NULL;
    }

    return key;
}

static int bls_keygen(EVP_PKEY* pkey, int type)
{
    ECX_KEY* key = bls_new_key();
    if (key == NULL)
        return 0;

    blst_scalar sk;

    RAND_bytes_ex(nullptr, key->privkey, BLS12_381_KEYLEN, 0);

    blst_keygen(&sk, key->privkey, BLS12_381_KEYLEN);
    blst_bendian_from_scalar(key->privkey, &sk);

    if (type == EVP_PKEY_BLS12_381_G1) {
        blst_p1 pk;
        blst_sk_to_pk_in_g1(&pk, &sk);
        blst_p1_compress(key->pubkey, &pk);
    } else {
        blst_p2 pk;
        blst_sk_to_pk_in_g2(&pk, &sk);
        blst_p2_compress(key->pubkey, &pk);
    }

    return EVP_PKEY_assign(pkey, type, key);
}

static int g1_keygen(EVP_PKEY_CTX* ctx, EVP_PKEY* pkey)
{
    return bls_keygen(pkey, EVP_PKEY_BLS12_381_G1);
}

static int g2_keygen(EVP_PKEY_CTX* ctx, EVP_PKEY* pkey)
{
    return bls_keygen(pkey, EVP_PKEY_BLS12_381_G2);
}

static int bls_size(const EVP_PKEY* pkey)
{
    return pkey->ameth->pkey_id == EVP_PKEY_BLS12_381_G1 ? BLS12_381_G1_SIG_LEN : BLS12_381_G2_SIG_LEN;
}

static void bls_free(EVP_PKEY* pkey)
{
    ossl_ecx_key_free(pkey->pkey.ecx);
}

static int bls_ctrl(EVP_PKEY* pkey, int op, long arg1, void* arg2)
{
    switch (op) {
    case ASN1_PKEY_CTRL_DEFAULT_MD_NID:
        *(int*)arg2 = NID_undef;
        return 2;

    default:
        return -2;
    }
}

static int bls_set_priv_key(EVP_PKEY* pkey, const unsigned char* priv, size_t len)
{
    if (len != BLS12_381_KEYLEN) {
        ERR_raise(ERR_LIB_EC, EC_R_INVALID_ENCODING);
        return 0;
    }

    ECX_KEY* key = bls_new_key();
    if (key == NULL) {
        ERR_raise(ERR_LIB_EC, ERR_R_MALLOC_FAILURE);
        return 0;
    }

    memcpy(key->privkey, priv, len);

    blst_scalar sk;
    blst_scalar_from_bendian(&sk, priv);

    if (pkey->ameth->pkey_id == EVP_PKEY_BLS12_381_G1) {
        blst_p1 pk;
        blst_sk_to_pk_in_g1(&pk, &sk);
        blst_p1_compress(key->pubkey, &pk);
    } else {
        blst_p2 pk;
        blst_sk_to_pk_in_g2(&pk, &sk);
        blst_p2_compress(key->pubkey, &pk);
    }

    return EVP_PKEY_assign(pkey, pkey->ameth->pkey_id, key);
}

static int bls_set_pub_key(EVP_PKEY* pkey, const unsigned char* pub, size_t len)
{
    if (len != KEYLEN(pkey)) {
        ERR_raise(ERR_LIB_EC, EC_R_INVALID_ENCODING);
        return 0;
    }

    ECX_KEY* key = bls_new_key();
    if (key == NULL) {
        ERR_raise(ERR_LIB_EC, ERR_R_MALLOC_FAILURE);
        return 0;
    }

    memcpy(key->pubkey, pub, len);
    return EVP_PKEY_assign(pkey, pkey->ameth->pkey_id, key);
}

static int bls_get_priv_key(const EVP_PKEY* pkey, unsigned char* priv, size_t* len)
{
    const ECX_KEY* key = pkey->pkey.ecx;

    if (priv == NULL) {
        *len = BLS12_381_KEYLEN;
        return 1;
    }

    if (key == NULL
        || key->privkey == NULL
        || *len < (size_t)BLS12_381_KEYLEN)
        return 0;

    *len = BLS12_381_KEYLEN;
    memcpy(priv, key->privkey, *len);

    return 1;
}

static int bls_get_pub_key(const EVP_PKEY* pkey, unsigned char* pub, size_t* len)
{
    const ECX_KEY* key = pkey->pkey.ecx;

    if (pub == NULL) {
        *len = KEYLEN(pkey);
        return 1;
    }

    if (key == NULL
        || *len < (size_t)KEYLEN(pkey))
        return 0;

    *len = KEYLEN(pkey);
    memcpy(pub, key->pubkey, *len);

    return 1;
}

static int bls_pkey_copy(EVP_PKEY* to, EVP_PKEY* from)
{
    ECX_KEY *ecx = from->pkey.ecx, *dupkey = NULL;
    int ret;

    if (ecx != NULL) {
        dupkey = ossl_ecx_key_dup(ecx, OSSL_KEYMGMT_SELECT_ALL);
        if (dupkey == NULL)
            return 0;
    }

    ret = EVP_PKEY_assign(to, from->type, dupkey);
    if (!ret)
        ossl_ecx_key_free(dupkey);
    return ret;
}

static int bls_sign(EVP_PKEY_CTX* ctx, unsigned char* sig, size_t* siglen, const unsigned char* tbs, size_t tbslen)
{
    EVP_PKEY* pkey = EVP_PKEY_CTX_get0_pkey(ctx);
    const ECX_KEY* key = pkey->pkey.ecx;

    blst_scalar sk;
    blst_scalar_from_bendian(&sk, key->privkey);

    if (*siglen < SIGLEN(pkey)) {
        ERR_raise(ERR_LIB_EC, EC_R_INVALID_ENCODING);
        return 0;
    }
    *siglen = SIGLEN(pkey);

    if (pkey->ameth->pkey_id == EVP_PKEY_BLS12_381_G1) {
        blst_p2 point;
        blst_hash_to_g2(&point, tbs, tbslen, DST_G1_POP, sizeof(DST_G1_POP) - 1, NULL, 0);
        blst_sign_pk_in_g1(&point, &point, &sk);
        blst_p2_compress(sig, &point);
    } else {
        blst_p1 point;
        blst_hash_to_g1(&point, tbs, tbslen, DST_G2_POP, sizeof(DST_G2_POP) - 1, NULL, 0);
        blst_sign_pk_in_g2(&point, &point, &sk);
        blst_p1_compress(sig, &point);
    }

    return 1;
}

static int bls_verify(EVP_PKEY_CTX* ctx, const unsigned char* sig, size_t siglen, const unsigned char* tbs, size_t tbslen)
{
    EVP_PKEY* pkey = EVP_PKEY_CTX_get0_pkey(ctx);
    const ECX_KEY* key = pkey->pkey.ecx;

    if (siglen != SIGLEN(pkey))
        return 0;

    if (pkey->ameth->pkey_id == EVP_PKEY_BLS12_381_G1) {
        blst_p1_affine pk;
        blst_p1_uncompress(&pk, key->pubkey);

        blst_p2_affine point;
        blst_p2_uncompress(&point, sig);

        return blst_core_verify_pk_in_g1(&pk, &point, 1, tbs, tbslen, DST_G1_POP,
                   sizeof(DST_G1_POP) - 1, NULL, 0)
                == BLST_SUCCESS
            ? 1
            : 0;
    } else {
        blst_p2_affine pk;
        blst_p2_uncompress(&pk, key->pubkey);

        blst_p1_affine point;
        blst_p1_uncompress(&point, sig);

        return blst_core_verify_pk_in_g2(&pk, &point, 1, tbs, tbslen, DST_G2_POP,
                   sizeof(DST_G2_POP) - 1, NULL, 0)
                == BLST_SUCCESS
            ? 1
            : 0;
    }

    return 0;
}

static int bls_derive(EVP_PKEY_CTX* ctx, unsigned char* key, size_t* keylen)
{
    EVP_PKEY* pkey = EVP_PKEY_CTX_get0_pkey(ctx);
    const ECX_KEY* eckey = pkey->pkey.ecx;

    if (key == NULL) {
        *keylen = KEYLEN(pkey);
        return 1;
    }

    if (*keylen < (size_t)KEYLEN(pkey)) {
        ERR_raise(ERR_LIB_EC, EC_R_BUFFER_TOO_SMALL);
        return 0;
    }

    *keylen = (size_t)KEYLEN(pkey);

    EVP_PKEY* pkey1 = EVP_PKEY_CTX_get0_peerkey(ctx);
    const ECX_KEY* eckey1 = pkey1->pkey.ecx;

    if (pkey->ameth->pkey_id == EVP_PKEY_BLS12_381_G1) {
        blst_scalar sk;
        blst_p1_affine pk;

        blst_scalar_from_bendian(&sk, eckey->privkey);
        blst_p1_uncompress(&pk, eckey1->pubkey);

        blst_p1 pk1;
        blst_p1_from_affine(&pk1, &pk);

        blst_p1 sec;
        blst_p1_mult(&sec, &pk1, (const byte*)&sk, 256);

        blst_p1_compress(key, &sec);
    } else {
        blst_scalar sk;
        blst_p2_affine pk;

        blst_scalar_from_bendian(&sk, eckey->privkey);
        blst_p2_uncompress(&pk, eckey1->pubkey);

        blst_p2 pk1;
        blst_p2_from_affine(&pk1, &pk);

        blst_p2 sec;
        blst_p2_mult(&sec, &pk1, (const byte*)&sk, 256);

        blst_p2_compress(key, &sec);
    }

    return 1;
}

static int pkey_bls_ctrl(EVP_PKEY_CTX* ctx, int type, int p1, void* p2)
{
    switch (type) {
    case EVP_PKEY_CTRL_MD:
        return 1;

    case EVP_PKEY_CTRL_DIGESTINIT:
        return 1;

    case EVP_PKEY_CTRL_PEER_KEY:
        return 1;
    }
    return -2;
}

static int pkey_ec_copy(EVP_PKEY_CTX* dst, const EVP_PKEY_CTX* src)
{
    return 1;
}

static int bls_nid[] = { NID_BLS12_381_G1, NID_BLS12_381_G2 };

static EVP_PKEY_ASN1_METHOD bls_asn1_meth[] = {
    { NID_BLS12_381_G1,
        NID_BLS12_381_G1,
        0,
        "Bls12381G1",
        "OpenSSL Bls12381G1 algorithm",

        0, 0, 0, 0, 0, 0, 0,
        bls_size,
        0, 0, 0, 0, 0, 0, 0, 0, 0,

        bls_free, bls_ctrl,

        0, 0, 0, 0, 0, 0, 0, 0,

        bls_set_priv_key,
        bls_set_pub_key,
        bls_get_priv_key,
        bls_get_pub_key,

        0, 0, 0,

        bls_pkey_copy,

        0 },
    { NID_BLS12_381_G2,
        NID_BLS12_381_G2,
        0,
        "Bls12381G2",
        "OpenSSL Bls12381G2 algorithm",

        0, 0, 0, 0, 0, 0, 0,
        bls_size,
        0, 0, 0, 0, 0, 0, 0, 0, 0,

        bls_free, bls_ctrl,

        0, 0, 0, 0, 0, 0, 0, 0,

        bls_set_priv_key,
        bls_set_pub_key,
        bls_get_priv_key,
        bls_get_pub_key,

        0, 0, 0,

        bls_pkey_copy,

        0 }
};

static EVP_PKEY_METHOD bls_pkey_meth[] = {
    { NID_BLS12_381_G1,
        0, 0,
        pkey_ec_copy,
        0, 0, 0,
        0, g1_keygen,
        0, bls_sign,
        0, bls_verify,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        bls_derive,
        pkey_bls_ctrl,
        0 },
    { NID_BLS12_381_G2,
        0, 0,
        pkey_ec_copy,
        0, 0, 0,
        0, g2_keygen,
        0, bls_sign,
        0, bls_verify,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        bls_derive,
        pkey_bls_ctrl,
        0 }
};

int bls_get_ans1_meth(ENGINE* e, EVP_PKEY_ASN1_METHOD** meths, const int** nids, int n)
{
    int pos = n == NID_BLS12_381_G1 ? 0 : 1;

    if (meths)
        *meths = &bls_asn1_meth[pos];

    if (nids)
        *nids = &bls_nid[pos];

    return 2;
}

int bls_get_pkey_meth(ENGINE* e, EVP_PKEY_METHOD** meth, const int** nids, int n)
{
    int pos = n == NID_BLS12_381_G1 ? 0 : 1;

    if (meth)
        *meth = &bls_pkey_meth[pos];

    if (nids)
        *nids = &bls_nid[pos];

    return 2;
}

void init_blst_eng()
{
    ENGINE* e = ENGINE_new();

    ENGINE_set_id(e, "Bls12381");
    ENGINE_set_name(e, "OpenSSL Bls12381 algorithm");
    ENGINE_set_pkey_asn1_meths(e, bls_get_ans1_meth);
    ENGINE_set_pkey_meths(e, bls_get_pkey_meth);

    ENGINE_register_pkey_asn1_meths(e);

    for (int i = 0; i < 2; i++) {
        EVP_PKEY_asn1_add0(&bls_asn1_meth[i]);
        EVP_PKEY_meth_add0(&bls_pkey_meth[i]);
    }
}

}
