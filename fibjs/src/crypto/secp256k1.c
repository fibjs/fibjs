
#include "mbedtls/src/secp256k1_api.h"
#include "secp256k1/src/secp256k1.c"

int secp256k1_ec_pubkey_decode(const secp256k1_context* ctx, unsigned char* pubkey, int* pubkeylen)
{
    secp256k1_ge p;
    int ret = 0;
    (void)ctx;

    if (secp256k1_eckey_pubkey_parse(&p, pubkey, *pubkeylen)) {
        size_t sz = *pubkeylen;
        ret = secp256k1_eckey_pubkey_serialize(&p, pubkey, &sz, 0);
        *pubkeylen = (int)sz;
    }
    return ret;
}
