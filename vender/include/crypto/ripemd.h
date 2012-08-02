/* mhash_ripemd.h
 *
 * RIPEMD-160 is a 160-bit cryptographic hash function, designed by Hans
 * Dobbertin, Antoon Bosselaers, and Bart Preneel. It is intended to be
 * used as a secure replacement for the 128-bit hash functions MD4, MD5,
 * and RIPEMD
 * See also: http://www.esat.kuleuven.ac.be/~bosselae/ripemd160.html
 *
 * RIPEMD-128 is a plug-in substitute for RIPEMD (or MD4 and MD5, for
 * that matter) with a 128-bit result. 128-bit hash results do not
 * offer sufficient protection for the next ten years, and
 * applications using 128-bit hash functions should consider upgrading
 * to a 160-bit hash function.
 *
 * RIPEMD-256 and RIPEMD-320 are optional extensions of, respectively,
 * RIPEMD-128 and RIPEMD-160, and are intended for applications of
 * hash functions that require a longer hash result without needing a
 * larger security level.
 */

/* The following code was written by Nikos Mavroyanopoulos and B. 
 * Poettering for the mhash library.
 */

#if !defined(__MHASH_RIPEMD_H)
#define __MHASH_RIPEMD_H

/* The RIPEMD block sizes and message digest sizes, in bytes */

#define RIPEMD_DATASIZE    64
#define RIPEMD_DATALEN     16

#define RIPEMD128_DIGESTSIZE  16
#define RIPEMD160_DIGESTSIZE  20
#define RIPEMD256_DIGESTSIZE  32
#define RIPEMD320_DIGESTSIZE  40

#define RIPEMD_STATESIZE      10 /* state size in 32 bit words */


#ifdef _WIN32
typedef unsigned __int64 rmd_uint64;
#else
typedef unsigned long long rmd_uint64;
#endif

/* The structure for storing RIPEMD info */

typedef struct ripemd_ctx {
  unsigned int digest[RIPEMD_STATESIZE];     /* chaining varialbles */
  rmd_uint64 bitcount;                     /* 64-bit bit counter */
  unsigned char block[RIPEMD_DATASIZE];        /* RIPEMD data buffer */
  unsigned int index;                        /* index into buffer */
  unsigned int digest_len;                   /* determines the algorithm to use */
} RIPEMD_CTX;

#ifdef __cplusplus
extern "C" {
#endif

void ripemd128_init(struct ripemd_ctx *ctx);
void ripemd160_init(struct ripemd_ctx *ctx);
void ripemd256_init(struct ripemd_ctx *ctx);
void ripemd320_init(struct ripemd_ctx *ctx);
void ripemd_update(struct ripemd_ctx *ctx, unsigned char *buffer, unsigned int len);
void ripemd_final(struct ripemd_ctx *ctx, unsigned char * s);

#ifdef __cplusplus
}
#endif

#endif /* __MHASH_RIPEMD_H */
