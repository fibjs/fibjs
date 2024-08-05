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

    idx_i.reserve(_idx->length());
    idx_j.reserve(msg_len - _idx->length());

    size_t pos_j = 1;

    for (size_t i = 0; i < _idx->length(); i++) {
        Variant v;

        _idx->_indexed_getter(i, v);

        int32_t n = v.intVal();
        if (n < pos_j || n > msg_len)
            return CALL_E_INVALID_DATA;

        while (pos_j < n)
            idx_j.push_back(pos_j++);
        pos_j++;

        idx_i.push_back(n);
    }

    while (pos_j <= msg_len)
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

static blst_scalar calculate_challenge(const blst_p1& abar, const blst_p1& bbar, const blst_p1& c,
    const std::vector<int32_t>& idx_i, const std::vector<blst_scalar>& fr_messages, const blst_scalar& domain, Buffer_base* ph, int32_t suite)
{
    blst_scalar s;
    Buffer* buf = Buffer::Cast(ph);

    std::vector<uint8_t> data = encode(abar, bbar, c, idx_i.size(), idx_i, fr_messages, domain,
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
        return CALL_E_INVALID_DATA;

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
    blst_scalar r3 = generate_random_scalar();
    std::vector<blst_scalar> rs;

    rs.reserve(idx_j.size());
    for (size_t i = 0; i < idx_j.size(); i++)
        rs.push_back(generate_random_scalar());

    blst_p1 b = gens.compute_b(fr_messages.data(), domain, suite);

    Proof p;

    blst_p1_mult(&p.abar, &s.a, (const byte*)&r1, 256);

    blst_p1_mult(&p.bbar, &p.abar, (const byte*)&s.e, 256);
    blst_p1_cneg(&p.bbar, 1);
    add_mul(p.bbar, b, r1);

    blst_p1 c;

    blst_p1_mult(&c, &p.bbar, (const byte*)&r3, 256);
    add_mul(c, p.abar, r2);
    for (size_t i = 0; i < idx_j.size(); i++)
        add_mul(c, gens.h[idx_j[i] - 1], rs[i]);

    std::vector<blst_scalar> fr_messages_i;

    fr_messages_i.reserve(idx_i.size());
    for (size_t i = 0; i < idx_i.size(); i++)
        fr_messages_i.push_back(fr_messages[idx_i[i] - 1]);

    for (size_t i = 0; i < idx_i.size(); i++)
        idx_i[i]--;

    p.c = calculate_challenge(p.abar, p.bbar, c, idx_i, fr_messages_i, domain, (Buffer_base*)ac->m_ctx[5].object(), suite);

    blst_fr fr4;

    blst_fr_from_scalar(&fr4, &r1);
    blst_fr_cneg(&fr4, &fr4, 1);
    blst_fr_inverse(&fr4, &fr4);

    blst_fr fr2, fr3, fr5, frc, fre;
    blst_fr_from_scalar(&fr2, &r2);
    blst_fr_from_scalar(&fr3, &r3);
    blst_fr_from_scalar(&frc, &p.c);
    blst_fr_from_scalar(&fre, &s.e);

    blst_fr_mul(&fr5, &fr4, &fre);
    blst_fr_mul(&fr5, &fr5, &frc);
    blst_fr_add(&fr5, &fr5, &fr2);
    blst_scalar_from_fr(&p.r2hat, &fr5);

    blst_fr_mul(&fr5, &fr4, &frc);
    blst_fr_add(&fr5, &fr5, &fr3);
    blst_scalar_from_fr(&p.r3hat, &fr5);

    p.mhat.resize(idx_j.size());
    for (size_t i = 0; i < idx_j.size(); i++) {
        blst_fr fr6;

        blst_fr_from_scalar(&fr5, &fr_messages[idx_j[i] - 1]);
        blst_fr_mul(&fr5, &fr5, &frc);

        blst_fr_from_scalar(&fr6, &rs[i]);
        blst_fr_add(&fr5, &fr5, &fr6);
        blst_scalar_from_fr(&p.mhat[i], &fr5);
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
    if (hr < 0)
        return hr;

    if (fr_messages.size() != idx_i.size()) {
        retVal = false;
        return 0;
    }

    Generators gens(msg_len, suite);
    blst_scalar domain = calculate_domain(pk, gens, (Buffer_base*)ac->m_ctx[4].object(), suite);

    blst_p1 d, c;

    blst_p1_from_affine(&d, suite == 0 ? &BLS12_381_G1_P1 : &BLS12_381_G1_P1_XOF);
    add_mul(d, gens.q1, domain);
    for (size_t i = 0; i < idx_i.size(); i++)
        add_mul(d, gens.h[idx_i[i] - 1], fr_messages[i]);

    blst_p1_mult(&c, &p.abar, (const byte*)&p.r2hat, 256);
    add_mul(c, p.bbar, p.r3hat);
    for (size_t i = 0; i < idx_j.size(); i++)
        add_mul(c, gens.h[idx_j[i] - 1], p.mhat[i]);
    add_mul(c, d, p.c);

    for (size_t i = 0; i < idx_i.size(); i++)
        idx_i[i]--;

    blst_scalar cv = calculate_challenge(p.abar, p.bbar, c, idx_i, fr_messages, domain, (Buffer_base*)ac->m_ctx[5].object(), suite);
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
