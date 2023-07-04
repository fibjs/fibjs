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

#define DST_BBS_SUITE "BBS_BLS12381G1_XMD:SHA-256_SSWU_RO_H2G_"

const unsigned char DST_G1_BBS_SUITE[] = DST_BBS_SUITE;
const unsigned char DST_G1_BBS_SIG_SEED[] = DST_BBS_SUITE "SIG_GENERATOR_SEED_";
const unsigned char DST_G1_BBS_SIG[] = DST_BBS_SUITE "SIG_GENERATOR_DST_";
const unsigned char DST_G1_BBS_MSG_SEED[] = DST_BBS_SUITE "MESSAGE_GENERATOR_SEED";
const unsigned char DST_G1_BBS_H2S[] = DST_BBS_SUITE "H2S_";
const unsigned char DST_G1_BBS_MSG_TO_SCALAR[] = DST_BBS_SUITE "MAP_MSG_TO_SCALAR_AS_HASH_";

}
