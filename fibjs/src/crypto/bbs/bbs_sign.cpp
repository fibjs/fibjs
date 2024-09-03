/*
 * bbs_sign.cpp
 *
 *  Created on: Aug 4, 2024
 *      Author: lion
 */

#include "object.h"
#include "crypto_util.h"
#include "ifs/crypto.h"
#include "Buffer.h"
#include "KeyObject.h"

#include "defs.h"
#include "args.h"
#include "encode.h"
#include "Generators.h"
#include "Signature.h"
#include "Proof.h"

namespace fibjs {

result_t get_index(Variant& idx, size_t msg_len, std::vector<int32_t>& idx_i, std::vector<int32_t>& idx_j)
{
    obj_ptr<NArray> _idx = (NArray*)idx.object();

    if (_idx->length() > msg_len)
        return Runtime::setError("crypto: index array length must be less than or equal to messages length");

    idx_i.reserve(_idx->length());
    idx_j.reserve(msg_len - _idx->length());

    size_t pos_j = 0;

    for (size_t i = 0; i < _idx->length(); i++) {
        Variant v;

        _idx->_indexed_getter(i, v);

        int32_t n = v.intVal();
        if (n < pos_j)
            return Runtime::setError("crypto: index array must be sorted");
        if (n >= msg_len)
            return Runtime::setError("crypto: index array must be less than messages length");

        while (pos_j < n)
            idx_j.push_back(pos_j++);
        pos_j++;

        idx_i.push_back(n);
    }

    while (pos_j < msg_len)
        idx_j.push_back(pos_j++);

    return 0;
}

static void bbs_hash_to_g1(blst_p1* out, const byte* msg, size_t msg_len,
    const byte* DST, size_t DST_len, int32_t suite)
{
    if (suite == Bls12381Sha256)
        blst_hash_to_g1(out, msg, msg_len, DST, DST_len);
    else
        blst_hash_to_g1_xof(out, msg, msg_len, DST, DST_len);
}

static void bbs_expand_message(unsigned char* bytes, size_t len_in_bytes, const unsigned char* msg, size_t msg_len,
    const unsigned char* DST, size_t DST_len, int32_t suite)
{
    if (suite == Bls12381Sha256)
        blst_expand_message_xmd(bytes, len_in_bytes, msg, msg_len, DST, DST_len);
    else
        blst_expand_message_xof(bytes, len_in_bytes, msg, msg_len, DST, DST_len);
}

static void blst_hash_to_scalar(blst_scalar* out, const byte* msg, size_t msg_len,
    const byte* DST, size_t DST_len, int32_t suite)
{
    byte buf[G1_COMPRESSED_SIZE];

    bbs_expand_message(buf, G1_COMPRESSED_SIZE, msg, msg_len, DST, DST_len, suite);
    blst_scalar_from_be_bytes(out, buf, G1_COMPRESSED_SIZE);
}

static std::vector<blst_scalar> messagesToFr(Variant& _msgs, int32_t suite)
{
    const NArray* msgs = (NArray*)_msgs.object();
    size_t sz = msgs->length();
    std::vector<blst_scalar> fr_messages;

    fr_messages.resize(sz);

    for (size_t i = 0; i < sz; i++) {
        Variant v;

        msgs->_indexed_getter(i, v);

        Buffer* buf = Buffer::Cast((Buffer_base*)v.object());
        blst_hash_to_scalar(&fr_messages[i], buf->data(), buf->length(), DST(MSG_TO_SCALAR, suite), suite);
    }

    return fr_messages;
}

static blst_scalar calculate_domain(const blst_p2& pk, const Generators& gens, Buffer_base* header, int32_t suite)
{
    blst_scalar s;
    Buffer* buf = Buffer::Cast(header);

    std::vector<uint8_t> data = encode(pk,
        gens.size, gens.q1, codec_impl::span(gens.h, gens.size),
        codec_impl::span(DST(SUITE, suite)),
        buf ? buf->length() : 0,
        buf ? codec_impl::span(buf->data(), buf->length()) : codec_impl::span((uint8_t*)NULL, 0));
    blst_hash_to_scalar(&s, data.data(), data.size(), DST(H2S, suite), suite);

    return s;
}

blst_p2 get_pk(object_base* key)
{
    unsigned char buf[G2_COMPRESSED_SIZE];
    size_t len = G2_COMPRESSED_SIZE;

    EVP_PKEY* pkey = static_cast<KeyObject*>(key)->pkey();
    EVP_PKEY_get_raw_public_key(pkey, buf, &len);

    blst_p2 pk;
    blst_p2_affine pka;

    blst_p2_uncompress(&pka, buf);
    blst_p2_from_affine(&pk, &pka);

    return pk;
}

blst_scalar get_sk(object_base* key)
{
    unsigned char buf[FR_BYTE_SIZE];
    size_t len = FR_BYTE_SIZE;

    EVP_PKEY* pkey = static_cast<KeyObject*>(key)->pkey();
    EVP_PKEY_get_raw_private_key(pkey, buf, &len);

    blst_scalar sk;
    blst_scalar_from_bendian(&sk, buf);
    return sk;
}

static blst_scalar calculate_challenge(const blst_p1& abar, const blst_p1& bbar,
    const blst_p1& d, const blst_p1& T1, const blst_p1& T2,
    const std::vector<int32_t>& idx_i, const std::vector<blst_scalar>& fr_messages,
    const blst_scalar& domain, Buffer_base* ph, int32_t suite)
{
    blst_scalar s;
    Buffer* buf = Buffer::Cast(ph);
    std::vector<std::pair<int32_t, blst_scalar>> idx_i_fr;

    idx_i_fr.reserve(idx_i.size());
    for (size_t i = 0; i < idx_i.size(); i++)
        idx_i_fr.push_back(std::make_pair(idx_i[i], fr_messages[i]));

    std::vector<uint8_t> data = encode(idx_i.size(), idx_i_fr, abar, bbar, d, T1, T2, domain,
        buf ? buf->length() : 0,
        buf ? codec_impl::span(buf->data(), buf->length()) : codec_impl::span((uint8_t*)NULL, 0));
    blst_hash_to_scalar(&s, data.data(), data.size(), DST(H2S, suite), suite);

    return s;
}

static blst_scalar generate_random_scalar()
{
    byte buf[48];
    blst_scalar s;

    RAND_bytes_ex(nullptr, buf, sizeof(buf), 0);
    blst_scalar_from_be_bytes(&s, buf, sizeof(buf));
    return s;
}

static result_t bbsSign_(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    int suite = ac->m_ctx[1].intVal();
    std::vector<blst_scalar> fr_messages = messagesToFr(ac->m_ctx[2], suite);
    Generators gens(fr_messages.size(), suite);

    blst_scalar domain = calculate_domain(get_pk(ac->m_ctx[0].object()), gens, (Buffer_base*)ac->m_ctx[4].object(), suite);
    blst_p1 b = gens.compute_b(fr_messages.data(), domain, suite);

    Signature s;

    s.sign(get_sk(ac->m_ctx[0].object()), b);
    s.serialize(retVal);

    return 0;
}

result_t crypto_base::bbsSign(v8::Local<v8::Array> messages, Buffer_base* privateKey, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return bbs_get_args(messages, v8::Local<v8::Array>(), privateKey, true, ac);

    return bbsSign_(retVal, ac);
}

result_t crypto_base::bbsSign(v8::Local<v8::Array> messages, KeyObject_base* privateKey, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return bbs_get_args(messages, v8::Local<v8::Array>(), privateKey, true, ac);

    return bbsSign_(retVal, ac);
}

result_t crypto_base::bbsSign(v8::Local<v8::Array> messages, v8::Local<v8::Object> key, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return bbs_get_args(messages, v8::Local<v8::Array>(), key, true, ac);

    return bbsSign_(retVal, ac);
}

static result_t bbsVerify_(Buffer_base* signature, bool& retVal, AsyncEvent* ac)
{
    blst_p2 pk = get_pk(ac->m_ctx[0].object());
    int suite = ac->m_ctx[1].intVal();
    std::vector<blst_scalar> fr_messages = messagesToFr(ac->m_ctx[2], suite);

    Signature s;
    if (!s.parse(signature)) {
        retVal = false;
        return 0;
    }

    Generators gens(fr_messages.size(), suite);
    blst_scalar domain = calculate_domain(pk, gens, (Buffer_base*)ac->m_ctx[4].object(), suite);
    blst_p1 b = gens.compute_b(fr_messages.data(), domain, suite);

    retVal = s.verify(pk, b);

    return 0;
}

result_t crypto_base::bbsVerify(v8::Local<v8::Array> messages, Buffer_base* publicKey, Buffer_base* signature, bool& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return bbs_get_args(messages, v8::Local<v8::Array>(), publicKey, false, ac);

    return bbsVerify_(signature, retVal, ac);
}

result_t crypto_base::bbsVerify(v8::Local<v8::Array> messages, KeyObject_base* publicKey, Buffer_base* signature, bool& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return bbs_get_args(messages, v8::Local<v8::Array>(), publicKey, false, ac);

    return bbsVerify_(signature, retVal, ac);
}

result_t crypto_base::bbsVerify(v8::Local<v8::Array> messages, v8::Local<v8::Object> key, Buffer_base* signature, bool& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return bbs_get_args(messages, v8::Local<v8::Array>(), key, false, ac);

    return bbsVerify_(signature, retVal, ac);
}

static result_t proofGen_(Buffer_base* signature, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    Signature s;
    if (!s.parse(signature))
        return Runtime::setError("crypto: invalid signature");

    blst_p2 pk = get_pk(ac->m_ctx[0].object());
    int suite = ac->m_ctx[1].intVal();
    std::vector<blst_scalar> fr_messages = messagesToFr(ac->m_ctx[2], suite);
    size_t msg_len = fr_messages.size();

    std::vector<int32_t> idx_i;
    std::vector<int32_t> idx_j;

    result_t hr = get_index(ac->m_ctx[3], msg_len, idx_i, idx_j);
    if (hr < 0)
        return hr;

    Generators gens(msg_len, suite);
    blst_scalar domain = calculate_domain(pk, gens, (Buffer_base*)ac->m_ctx[4].object(), suite);

    blst_scalar r1 = generate_random_scalar();
    blst_scalar r2 = generate_random_scalar();
    blst_scalar eTilda = generate_random_scalar();
    blst_scalar r1Tilda = generate_random_scalar();
    blst_scalar r3Tilda = generate_random_scalar();

    std::vector<blst_scalar> mTilda;
    mTilda.reserve(idx_j.size());
    for (size_t i = 0; i < idx_j.size(); i++)
        mTilda.push_back(generate_random_scalar());

    blst_p1 b = gens.compute_b(fr_messages.data(), domain, suite);
    blst_p1 c;

    Proof p;

    // D = B * r2
    blst_p1_mult(&p.d, &b, (const byte*)&r2, 256);

    // Abar = A * (r1 * r2)
    blst_fr fr1, fr2, fr3;
    blst_scalar r3;
    blst_fr_from_scalar(&fr1, &r1);
    blst_fr_from_scalar(&fr2, &r2);
    blst_fr_mul(&fr3, &fr1, &fr2);
    blst_scalar_from_fr(&r3, &fr3);
    blst_p1_mult(&p.abar, &s.a, (const byte*)&r3, 256);

    // Bbar = D * r1 - Abar * e
    blst_p1_mult(&p.bbar, &p.d, (const byte*)&r1, 256);
    sub_mul(p.bbar, p.abar, s.e);

    // T1 = Abar * e~ + D * r1~
    blst_p1 T1;
    blst_p1_mult(&T1, &p.abar, (const byte*)&eTilda, 256);
    add_mul(T1, p.d, r1Tilda);

    // T2 = D * r3~ + H_j1 * m~_j1 + ... + H_jU * m~_jU
    blst_p1 T2;
    blst_p1_mult(&T2, &p.d, (const byte*)&r3Tilda, 256);
    for (size_t i = 0; i < idx_j.size(); i++)
        add_mul(T2, gens.h[idx_j[i]], mTilda[i]);

    std::vector<blst_scalar> fr_messages_i;

    fr_messages_i.reserve(idx_i.size());
    for (size_t i = 0; i < idx_i.size(); i++)
        fr_messages_i.push_back(fr_messages[idx_i[i]]);

    p.c = calculate_challenge(p.abar, p.bbar, p.d, T1, T2,
        idx_i, fr_messages_i, domain, (Buffer_base*)ac->m_ctx[5].object(), suite);

    // r3 = r2^-1 (mod r)
    blst_fr_inverse(&fr3, &fr2);
    blst_scalar_from_fr(&r3, &fr3);

    // e^ = e~ + e_value * challenge
    scalar_add_mul(p.ehat, eTilda, s.e, p.c);

    // r1^ = r1~ - r1 * challenge
    scalar_sub_mul(p.r1hat, r1Tilda, r1, p.c);

    // r3^ = r3~ - r3 * challenge
    scalar_sub_mul(p.r3hat, r3Tilda, r3, p.c);

    blst_fr fr4, fr5, frc;
    blst_fr_from_scalar(&frc, &p.c);

    // m^_j = m~_j + m_j * c (mod r)
    p.mhat.resize(idx_j.size());
    for (size_t i = 0; i < idx_j.size(); i++) {
        blst_fr_from_scalar(&fr4, &fr_messages[idx_j[i]]);
        blst_fr_mul(&fr4, &fr4, &frc);

        blst_fr_from_scalar(&fr5, &mTilda[i]);
        blst_fr_add(&fr4, &fr4, &fr5);
        blst_scalar_from_fr(&p.mhat[i], &fr4);
    }

    p.serialize(retVal);

    return 0;
}

result_t crypto_base::proofGen(Buffer_base* signature, v8::Local<v8::Array> messages, v8::Local<v8::Array> index,
    Buffer_base* publicKey, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return bbs_get_args(messages, index, publicKey, false, ac);

    return proofGen_(signature, retVal, ac);
}

result_t crypto_base::proofGen(Buffer_base* signature, v8::Local<v8::Array> messages, v8::Local<v8::Array> index,
    KeyObject_base* publicKey, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return bbs_get_args(messages, index, publicKey, false, ac);

    return proofGen_(signature, retVal, ac);
}

result_t crypto_base::proofGen(Buffer_base* signature, v8::Local<v8::Array> messages, v8::Local<v8::Array> index,
    v8::Local<v8::Object> key, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return bbs_get_args(messages, index, key, false, ac);

    return proofGen_(signature, retVal, ac);
}

static result_t proofVerify_(Buffer_base* proof, bool& retVal, AsyncEvent* ac)
{
    blst_p2 pk = get_pk(ac->m_ctx[0].object());
    int suite = ac->m_ctx[1].intVal();

    Proof p;
    if (!p.parse(proof)) {
        retVal = false;
        return 0;
    }

    std::vector<blst_scalar> fr_messages = messagesToFr(ac->m_ctx[2], suite);
    size_t msg_len = fr_messages.size() + p.mhat.size();

    std::vector<int32_t> idx_i;
    std::vector<int32_t> idx_j;

    result_t hr = get_index(ac->m_ctx[3], msg_len, idx_i, idx_j);
    if (hr < 0) {
        retVal = false;
        return 0;
    }

    if (fr_messages.size() != idx_i.size()) {
        retVal = false;
        return 0;
    }

    Generators gens(msg_len, suite);
    blst_scalar domain = calculate_domain(pk, gens, (Buffer_base*)ac->m_ctx[4].object(), suite);

    // T1 = Bbar * c + Abar * e^ + D * r1^
    blst_p1 T1;
    blst_p1_mult(&T1, &p.bbar, (const byte*)&p.c, 256);
    add_mul(T1, p.abar, p.ehat);
    add_mul(T1, p.d, p.r1hat);

    // Bv = P1 + Q_1 * domain + H_i1 * msg_i1 + ... + H_iR * msg_iR
    blst_p1 Bv;
    blst_p1_from_affine(&Bv, suite == 0 ? &BLS12_381_G1_P1 : &BLS12_381_G1_P1_XOF);
    add_mul(Bv, gens.q1, domain);
    for (size_t i = 0; i < idx_i.size(); i++)
        add_mul(Bv, gens.h[idx_i[i]], fr_messages[i]);

    // T2 = Bv * c + D * r3^ + H_j1 * m^_j1 + ... +  H_jU * m^_jU
    blst_p1 T2;
    blst_p1_mult(&T2, &Bv, (const byte*)&p.c, 256);
    add_mul(T2, p.d, p.r3hat);
    for (size_t i = 0; i < idx_j.size(); i++)
        add_mul(T2, gens.h[idx_j[i]], p.mhat[i]);

    blst_scalar cv = calculate_challenge(p.abar, p.bbar, p.d, T1, T2,
        idx_i, fr_messages, domain, (Buffer_base*)ac->m_ctx[5].object(), suite);

    if (memcmp(&cv, &p.c, sizeof(blst_scalar))) {
        retVal = false;
        return 0;
    }

    retVal = p.verify(pk);

    return 0;
}

result_t crypto_base::proofVerify(v8::Local<v8::Array> messages, v8::Local<v8::Array> index, Buffer_base* publicKey,
    Buffer_base* proof, bool& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return bbs_get_args(messages, index, publicKey, false, ac);

    return proofVerify_(proof, retVal, ac);
}

result_t crypto_base::proofVerify(v8::Local<v8::Array> messages, v8::Local<v8::Array> index, KeyObject_base* publicKey,
    Buffer_base* proof, bool& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return bbs_get_args(messages, index, publicKey, false, ac);

    return proofVerify_(proof, retVal, ac);
}

result_t crypto_base::proofVerify(v8::Local<v8::Array> messages, v8::Local<v8::Array> index, v8::Local<v8::Object> key,
    Buffer_base* proof, bool& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return bbs_get_args(messages, index, key, false, ac);

    return proofVerify_(proof, retVal, ac);
}

}
