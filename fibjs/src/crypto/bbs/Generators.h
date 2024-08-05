/*
 * Generators.h
 *
 *  Created on: Jun 29, 2023
 *      Author: lion
 */

#pragma once

#include "defs.h"
#include <exlib/include/utils.h>
#include <blst/include/bbs.h>
#include <memory>

namespace fibjs {

static void add_mul(blst_p1& b, const blst_p1& p, const blst_scalar& s)
{
    blst_p1 g;

    blst_p1_mult(&g, &p, (const byte*)&s, 256);
    blst_p1_add(&b, &b, &g);
}

static void bbs_expand_message(unsigned char* bytes, size_t len_in_bytes, const unsigned char* msg, size_t msg_len,
    const unsigned char* DST, size_t DST_len, int32_t suite);
static void bbs_hash_to_g1(blst_p1* out, const byte* msg, size_t msg_len,
    const byte* DST, size_t DST_len, int32_t suite);

class Generators {
public:
    Generators(const Generators& g)
        : h_ptr(g.h_ptr)
        , q1(g.q1)
        , h(g.h)
        , size(g.size)
    {
    }

    Generators(size_t sz, int32_t suite)
        : h_ptr(suite == 0 ? get<Bls12381Sha256>(sz) : get<Bls12381Shake256>(sz))
        , q1(h_ptr.get()[0])
        , h(h_ptr.get() + 1)
        , size(sz)
    {
    }

public:
    blst_p1 compute_b(const blst_scalar* msgs, const blst_scalar& domain, int32_t suite)
    {
        blst_p1 b;

        blst_p1_from_affine(&b, suite == 0 ? &BLS12_381_G1_P1 : &BLS12_381_G1_P1_XOF);

        add_mul(b, q1, domain);
        for (size_t i = 0; i < size; i++)
            add_mul(b, h[i], msgs[i]);

        return b;
    }

private:
    std::shared_ptr<blst_p1> h_ptr;

public:
    blst_p1& q1;
    const blst_p1* h;
    size_t size;

private:
    template <int32_t suite = 0>
    static std::shared_ptr<blst_p1> get(size_t sz)
    {
        static int64_t num = 0;
        static unsigned char v[G1_COMPRESSED_SIZE + sizeof(num)];
        static exlib::spinlock s_lock;

        static size_t pos = 0;
        static std::shared_ptr<blst_p1> h_ptr;

        s_lock.lock();

        if (num == 0) {
            bbs_expand_message(v, G1_COMPRESSED_SIZE, DST(MSG_SEED, suite), DST(SIG_SEED, suite), suite);
            num++;
        }

        if (pos < sz + 1) {
            blst_p1* h_ptr_new = new blst_p1[sz + 1];
            if (pos)
                memcpy(h_ptr_new, h_ptr.get(), pos * sizeof(blst_p1));

            h_ptr.reset(h_ptr_new);

            while (pos < sz + 1) {
                v[G1_COMPRESSED_SIZE] = (num >> 56) & 0xff;
                v[G1_COMPRESSED_SIZE + 1] = (num >> 48) & 0xff;
                v[G1_COMPRESSED_SIZE + 2] = (num >> 40) & 0xff;
                v[G1_COMPRESSED_SIZE + 3] = (num >> 32) & 0xff;
                v[G1_COMPRESSED_SIZE + 4] = (num >> 24) & 0xff;
                v[G1_COMPRESSED_SIZE + 5] = (num >> 16) & 0xff;
                v[G1_COMPRESSED_SIZE + 6] = (num >> 8) & 0xff;
                v[G1_COMPRESSED_SIZE + 7] = num & 0xff;
                num++;

                bbs_expand_message(v, G1_COMPRESSED_SIZE, v, sizeof(v), DST(SIG_SEED, suite), suite);
                bbs_hash_to_g1(&h_ptr.get()[pos++], v, G1_COMPRESSED_SIZE, DST(SIG, suite), suite);
            }
        }

        std::shared_ptr<blst_p1> gens = h_ptr;

        s_lock.unlock();

        return gens;
    }
};

}
