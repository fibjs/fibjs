/*
 * defs.h
 *
 *  Created on: Jun 29, 2023
 *      Author: lion
 */

#pragma once

#define FR_BYTE_SIZE 32
#define G1_COMPRESSED_SIZE 48
#define G1_SERIALIZE_SIZE 96
#define G2_COMPRESSED_SIZE 96
#define G2_SERIALIZE_SIZE 192

namespace fibjs {

enum {
    Bls12381Sha256 = 0,
    Bls12381Shake256 = 1
};

#define DST_BBS_SHA256 "BBS_BLS12381G1_XMD:SHA-256_SSWU_RO_H2G_HM2S_"

const unsigned char DST_G1_BBS_SHA256_SUITE[] = DST_BBS_SHA256;
const unsigned char DST_G1_BBS_SHA256_SIG_SEED[] = DST_BBS_SHA256 "SIG_GENERATOR_SEED_";
const unsigned char DST_G1_BBS_SHA256_SIG[] = DST_BBS_SHA256 "SIG_GENERATOR_DST_";
const unsigned char DST_G1_BBS_SHA256_MSG_SEED[] = DST_BBS_SHA256 "MESSAGE_GENERATOR_SEED";
const unsigned char DST_G1_BBS_SHA256_H2S[] = DST_BBS_SHA256 "H2S_";
const unsigned char DST_G1_BBS_SHA256_MSG_TO_SCALAR[] = DST_BBS_SHA256 "MAP_MSG_TO_SCALAR_AS_HASH_";

#define DST_BBS_SHAKE256 "BBS_BLS12381G1_XOF:SHAKE-256_SSWU_RO_H2G_HM2S_"

const unsigned char DST_G1_BBS_SHAKE256_SUITE[] = DST_BBS_SHAKE256;
const unsigned char DST_G1_BBS_SHAKE256_SIG_SEED[] = DST_BBS_SHAKE256 "SIG_GENERATOR_SEED_";
const unsigned char DST_G1_BBS_SHAKE256_SIG[] = DST_BBS_SHAKE256 "SIG_GENERATOR_DST_";
const unsigned char DST_G1_BBS_SHAKE256_MSG_SEED[] = DST_BBS_SHAKE256 "MESSAGE_GENERATOR_SEED";
const unsigned char DST_G1_BBS_SHAKE256_H2S[] = DST_BBS_SHAKE256 "H2S_";
const unsigned char DST_G1_BBS_SHAKE256_MSG_TO_SCALAR[] = DST_BBS_SHAKE256 "MAP_MSG_TO_SCALAR_AS_HASH_";

#define DST(x, suite)                                                          \
    suite == Bls12381Sha256 ? DST_G1_BBS_SHA256_##x : DST_G1_BBS_SHAKE256_##x, \
        (suite == Bls12381Sha256 ? sizeof(DST_G1_BBS_SHA256_##x) : sizeof(DST_G1_BBS_SHAKE256_##x)) - 1

}
