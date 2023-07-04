/*
 * bls_func.h
 *
 *  Created on: Jun 29, 2023
 *      Author: lion
 */

#pragma once

#include "defs.h"
#include <blst/include/blst.h>
#include <stdio.h>
#include <vector>

namespace fibjs {
void output(const char* msg, const unsigned char* data, size_t datlen)
{
    printf("%s: ", msg);
    for (size_t i = 0; i < datlen; i++)
        printf("%02x", data[i]);
    puts("");
}

void output(const char* msg, const std::vector<uint8_t> data)
{
    output(msg, data.data(), data.size());
}

void output(const char* msg, const blst_p1_affine& v)
{
    unsigned char dat[G1_COMPRESSED_SIZE];
    blst_p1_affine_compress(dat, &v);
    output(msg, dat, sizeof(dat));
}

void output(const char* msg, const blst_p1& v)
{
    blst_p1_affine v1;
    blst_p1_to_affine(&v1, &v);
    output(msg, v1);
}

void output(const char* msg, const blst_scalar& v)
{
    unsigned char dat[FR_BYTE_SIZE];
    blst_bendian_from_scalar(dat, &v);
    output(msg, dat, sizeof(dat));
}

void output(const char* msg, const blst_fp& v)
{
    unsigned char dat[48];
    blst_bendian_from_fp(dat, &v);
    output(msg, dat, sizeof(dat));
}

void output(const char* msg, const blst_fr& v)
{
    blst_scalar v1;
    blst_scalar_from_fr(&v1, &v);
    output(msg, v1);
}

static void export_p1()
{
    uint8_t data[] = {
        0x86, 0x4d, 0xf3, 0xae, 0x75, 0xa0, 0x23, 0x85, 0x2b, 0x57,
        0x7c, 0x6a, 0xa4, 0x6d, 0x16, 0x08, 0xd7, 0xbf, 0xb7, 0x3c,
        0x59, 0xc7, 0x3d, 0xfd, 0x47, 0x25, 0x0e, 0xa0, 0x1c, 0x04,
        0xec, 0x1a, 0xd2, 0x05, 0x60, 0xe8, 0xe4, 0xac, 0xa8, 0x22,
        0x96, 0xca, 0x7c, 0x4e, 0x1b, 0x7c, 0x36, 0x20
    };

    blst_p1_affine v;
    uint64_t* p = (uint64_t*)&v;

    blst_p1_uncompress(&v, data);
    for (size_t i = 0; i < 12; i++)
        printf("TO_LIMB_T(0x%016llx),\n", p[i]);
}

}
