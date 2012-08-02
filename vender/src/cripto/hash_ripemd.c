/* ripemd.c
 *
 *    Copyright (C) 2000,2001 Nikos Mavroyanopoulos
 *
 *    This library is free software; you can redistribute it and/or modify it 
 *    under the terms of the GNU Library General Public License as published 
 *    by the Free Software Foundation; either version 2 of the License, or 
 *    (at your option) any later version.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Library General Public License for more details.
 *
 *    You should have received a copy of the GNU Library General Public
 *    License along with this library; if not, write to the
 *    Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *    Boston, MA 02111-1307, USA.
 */


/*
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

/* Implementation of the RIPE-MD Hash Algorithms.
 * Based on the SHA-1 implementation, (C) by A.M. Kuchling
 *
 * The ripemd160 code was written by Nikos Mavroyanopoulos, the code
 * for ripemd128, ripemd256 and ripemd320 was added by B. Poettering
 * in April 2004.
 */

#include "ripemd.h"

#include <string.h>
#include <stdio.h>

/* 32-bit rotate left - kludged with shifts */
#define ROTL(n,X)  (((X)<<(n))|((X)>>(32-(n))))

#define f0(x,y,z)  (x^y^z)
//#define f16(x,y,z) ((x&y)|(~(x) & z))
#define f16(x,y,z) ((z)^((x)&((y)^(z))))
#define f32(x,y,z) ((x|~(y))^z)
//#define f48(x,y,z) ((x&z)|(y&~(z)))
#define f48(x,y,z) ((y)^((z)&((x)^(y))))
#define f64(x,y,z) (x^(y|(~z)))

#define K0  0x00000000
#define K1  0x5A827999     /* 2^30 * sqrt(2) */
#define K2  0x6ED9EBA1     /* 2^30 * sqrt(3) */
#define K3  0x8F1BBCDC     /* 2^30 * sqrt(5) */
#define K4  0xA953FD4E     /* 2^30 * sqrt(7) */

#define KK0 0x50A28BE6     /* 2^30 * cbrt(2) */
#define KK1 0x5C4DD124     /* 2^30 * cbrt(3) */
#define KK2 0x6D703EF3     /* 2^30 * cbrt(5) */
#define KK3 0x7A6D76E9     /* 2^30 * cbrt(7) */
#define KK4 0x00000000

#define h0init 0x67452301
#define h1init 0xEFCDAB89
#define h2init 0x98BADCFE
#define h3init 0x10325476
#define h4init 0xC3D2E1F0
#define h5init 0x76543210
#define h6init 0xFEDCBA98
#define h7init 0x89ABCDEF
#define h8init 0x01234567
#define h9init 0x3C2D1E0F


/* Initialize the RIPEMD values */

void ripemd_init(struct ripemd_ctx *ctx)
{
	/* Set the h-vars to their initial values */
	ctx->digest[0] = h0init;
	ctx->digest[1] = h1init;
	ctx->digest[2] = h2init;
	ctx->digest[3] = h3init;

	/* Initialize bit count */
	ctx->bitcount = 0;

	/* Initialize buffer */
	ctx->index = 0;
}

void ripemd128_init(struct ripemd_ctx *ctx)
{
  ripemd_init(ctx);
  ctx->digest_len = 8 * RIPEMD128_DIGESTSIZE;
}

void ripemd160_init(struct ripemd_ctx *ctx)
{
  ripemd_init(ctx);
  ctx->digest[4] = h4init;
  ctx->digest_len = 8 * RIPEMD160_DIGESTSIZE;
}

void ripemd256_init(struct ripemd_ctx *ctx)
{
  ripemd_init(ctx);
  ctx->digest[4] = h5init;
  ctx->digest[5] = h6init;
  ctx->digest[6] = h7init;
  ctx->digest[7] = h8init;
  ctx->digest_len = 8 * RIPEMD256_DIGESTSIZE;
}

void ripemd320_init(struct ripemd_ctx *ctx)
{
  ripemd_init(ctx);
  ctx->digest[4] = h4init;
  ctx->digest[5] = h5init;
  ctx->digest[6] = h6init;
  ctx->digest[7] = h7init;
  ctx->digest[8] = h8init;
  ctx->digest[9] = h9init;
  ctx->digest_len = 8 * RIPEMD320_DIGESTSIZE;
}


/* The RIPEMD core functions */

#define subRound128(a, b, c, d, f, k, r, data) \
    ( a = ROTL( r, a + f(b,c,d) + data + k))

#define subRound160(a, b, c, d, e, f, k, r, data) \
    ( a = ROTL( r, a + f(b,c,d) + data + k) + e, c = ROTL(10, c) )

static void ripemd128_transform(struct ripemd_ctx *ctx, unsigned int * data)
{
	unsigned int A, B, C, D;	/* Local vars */
	unsigned int AA, BB, CC, DD;	/* Local vars */
	unsigned int T;

	/* Set up first buffer and local data buffer */
	A = AA = ctx->digest[0];
	B = BB = ctx->digest[1];
	C = CC = ctx->digest[2];
	D = DD = ctx->digest[3];

/* j=0...15 */
	subRound128(A, B, C, D, f0, K0, 11, data[0]);
	subRound128(D, A, B, C, f0, K0, 14, data[1]);
	subRound128(C, D, A, B, f0, K0, 15, data[2]);
	subRound128(B, C, D, A, f0, K0, 12, data[3]);
	subRound128(A, B, C, D, f0, K0, 5, data[4]);
	subRound128(D, A, B, C, f0, K0, 8, data[5]);
	subRound128(C, D, A, B, f0, K0, 7, data[6]);
	subRound128(B, C, D, A, f0, K0, 9, data[7]);
	subRound128(A, B, C, D, f0, K0, 11, data[8]);
	subRound128(D, A, B, C, f0, K0, 13, data[9]);
	subRound128(C, D, A, B, f0, K0, 14, data[10]);
	subRound128(B, C, D, A, f0, K0, 15, data[11]);
	subRound128(A, B, C, D, f0, K0, 6, data[12]);
	subRound128(D, A, B, C, f0, K0, 7, data[13]);
	subRound128(C, D, A, B, f0, K0, 9, data[14]);
	subRound128(B, C, D, A, f0, K0, 8, data[15]);

	subRound128(AA, BB, CC, DD, f48, KK0, 8, data[5]);
	subRound128(DD, AA, BB, CC, f48, KK0, 9, data[14]);
	subRound128(CC, DD, AA, BB, f48, KK0, 9, data[7]);
	subRound128(BB, CC, DD, AA, f48, KK0, 11, data[0]);
	subRound128(AA, BB, CC, DD, f48, KK0, 13, data[9]);
	subRound128(DD, AA, BB, CC, f48, KK0, 15, data[2]);
	subRound128(CC, DD, AA, BB, f48, KK0, 15, data[11]);
	subRound128(BB, CC, DD, AA, f48, KK0, 5, data[4]);
	subRound128(AA, BB, CC, DD, f48, KK0, 7, data[13]);
	subRound128(DD, AA, BB, CC, f48, KK0, 7, data[6]);
	subRound128(CC, DD, AA, BB, f48, KK0, 8, data[15]);
	subRound128(BB, CC, DD, AA, f48, KK0, 11, data[8]);
	subRound128(AA, BB, CC, DD, f48, KK0, 14, data[1]);
	subRound128(DD, AA, BB, CC, f48, KK0, 14, data[10]);
	subRound128(CC, DD, AA, BB, f48, KK0, 12, data[3]);
	subRound128(BB, CC, DD, AA, f48, KK0, 6, data[12]);

/* j=16...31 */
	subRound128(A, B, C, D, f16, K1, 7, data[7]);
	subRound128(D, A, B, C, f16, K1, 6, data[4]);
	subRound128(C, D, A, B, f16, K1, 8, data[13]);
	subRound128(B, C, D, A, f16, K1, 13, data[1]);
	subRound128(A, B, C, D, f16, K1, 11, data[10]);
	subRound128(D, A, B, C, f16, K1, 9, data[6]);
	subRound128(C, D, A, B, f16, K1, 7, data[15]);
	subRound128(B, C, D, A, f16, K1, 15, data[3]);
	subRound128(A, B, C, D, f16, K1, 7, data[12]);
	subRound128(D, A, B, C, f16, K1, 12, data[0]);
	subRound128(C, D, A, B, f16, K1, 15, data[9]);
	subRound128(B, C, D, A, f16, K1, 9, data[5]);
	subRound128(A, B, C, D, f16, K1, 11, data[2]);
	subRound128(D, A, B, C, f16, K1, 7, data[14]);
	subRound128(C, D, A, B, f16, K1, 13, data[11]);
	subRound128(B, C, D, A, f16, K1, 12, data[8]);

	subRound128(AA, BB, CC, DD, f32, KK1, 9, data[6]);
	subRound128(DD, AA, BB, CC, f32, KK1, 13, data[11]);
	subRound128(CC, DD, AA, BB, f32, KK1, 15, data[3]);
	subRound128(BB, CC, DD, AA, f32, KK1, 7, data[7]);
	subRound128(AA, BB, CC, DD, f32, KK1, 12, data[0]);
	subRound128(DD, AA, BB, CC, f32, KK1, 8, data[13]);
	subRound128(CC, DD, AA, BB, f32, KK1, 9, data[5]);
	subRound128(BB, CC, DD, AA, f32, KK1, 11, data[10]);
	subRound128(AA, BB, CC, DD, f32, KK1, 7, data[14]);
	subRound128(DD, AA, BB, CC, f32, KK1, 7, data[15]);
	subRound128(CC, DD, AA, BB, f32, KK1, 12, data[8]);
	subRound128(BB, CC, DD, AA, f32, KK1, 7, data[12]);
	subRound128(AA, BB, CC, DD, f32, KK1, 6, data[4]);
	subRound128(DD, AA, BB, CC, f32, KK1, 15, data[9]);
	subRound128(CC, DD, AA, BB, f32, KK1, 13, data[1]);
	subRound128(BB, CC, DD, AA, f32, KK1, 11, data[2]);

/* j=32...47 */
	subRound128(A, B, C, D, f32, K2, 11, data[3]);
	subRound128(D, A, B, C, f32, K2, 13, data[10]);
	subRound128(C, D, A, B, f32, K2, 6, data[14]);
	subRound128(B, C, D, A, f32, K2, 7, data[4]);
	subRound128(A, B, C, D, f32, K2, 14, data[9]);
	subRound128(D, A, B, C, f32, K2, 9, data[15]);
	subRound128(C, D, A, B, f32, K2, 13, data[8]);
	subRound128(B, C, D, A, f32, K2, 15, data[1]);
	subRound128(A, B, C, D, f32, K2, 14, data[2]);
	subRound128(D, A, B, C, f32, K2, 8, data[7]);
	subRound128(C, D, A, B, f32, K2, 13, data[0]);
	subRound128(B, C, D, A, f32, K2, 6, data[6]);
	subRound128(A, B, C, D, f32, K2, 5, data[13]);
	subRound128(D, A, B, C, f32, K2, 12, data[11]);
	subRound128(C, D, A, B, f32, K2, 7, data[5]);
	subRound128(B, C, D, A, f32, K2, 5, data[12]);

	subRound128(AA, BB, CC, DD, f16, KK2, 9, data[15]);
	subRound128(DD, AA, BB, CC, f16, KK2, 7, data[5]);
	subRound128(CC, DD, AA, BB, f16, KK2, 15, data[1]);
	subRound128(BB, CC, DD, AA, f16, KK2, 11, data[3]);
	subRound128(AA, BB, CC, DD, f16, KK2, 8, data[7]);
	subRound128(DD, AA, BB, CC, f16, KK2, 6, data[14]);
	subRound128(CC, DD, AA, BB, f16, KK2, 6, data[6]);
	subRound128(BB, CC, DD, AA, f16, KK2, 14, data[9]);
	subRound128(AA, BB, CC, DD, f16, KK2, 12, data[11]);
	subRound128(DD, AA, BB, CC, f16, KK2, 13, data[8]);
	subRound128(CC, DD, AA, BB, f16, KK2, 5, data[12]);
	subRound128(BB, CC, DD, AA, f16, KK2, 14, data[2]);
	subRound128(AA, BB, CC, DD, f16, KK2, 13, data[10]);
	subRound128(DD, AA, BB, CC, f16, KK2, 13, data[0]);
	subRound128(CC, DD, AA, BB, f16, KK2, 7, data[4]);
	subRound128(BB, CC, DD, AA, f16, KK2, 5, data[13]);

/* j=48...63 */
	subRound128(A, B, C, D, f48, K3, 11, data[1]);
	subRound128(D, A, B, C, f48, K3, 12, data[9]);
	subRound128(C, D, A, B, f48, K3, 14, data[11]);
	subRound128(B, C, D, A, f48, K3, 15, data[10]);
	subRound128(A, B, C, D, f48, K3, 14, data[0]);
	subRound128(D, A, B, C, f48, K3, 15, data[8]);
	subRound128(C, D, A, B, f48, K3, 9, data[12]);
	subRound128(B, C, D, A, f48, K3, 8, data[4]);
	subRound128(A, B, C, D, f48, K3, 9, data[13]);
	subRound128(D, A, B, C, f48, K3, 14, data[3]);
	subRound128(C, D, A, B, f48, K3, 5, data[7]);
	subRound128(B, C, D, A, f48, K3, 6, data[15]);
	subRound128(A, B, C, D, f48, K3, 8, data[14]);
	subRound128(D, A, B, C, f48, K3, 6, data[5]);
	subRound128(C, D, A, B, f48, K3, 5, data[6]);
	subRound128(B, C, D, A, f48, K3, 12, data[2]);

	subRound128(AA, BB, CC, DD, f0, KK4, 15, data[8]);
	subRound128(DD, AA, BB, CC, f0, KK4, 5, data[6]);
	subRound128(CC, DD, AA, BB, f0, KK4, 8, data[4]);
	subRound128(BB, CC, DD, AA, f0, KK4, 11, data[1]);
	subRound128(AA, BB, CC, DD, f0, KK4, 14, data[3]);
	subRound128(DD, AA, BB, CC, f0, KK4, 14, data[11]);
	subRound128(CC, DD, AA, BB, f0, KK4, 6, data[15]);
	subRound128(BB, CC, DD, AA, f0, KK4, 14, data[0]);
	subRound128(AA, BB, CC, DD, f0, KK4, 6, data[5]);
	subRound128(DD, AA, BB, CC, f0, KK4, 9, data[12]);
	subRound128(CC, DD, AA, BB, f0, KK4, 12, data[2]);
	subRound128(BB, CC, DD, AA, f0, KK4, 9, data[13]);
	subRound128(AA, BB, CC, DD, f0, KK4, 12, data[9]);
	subRound128(DD, AA, BB, CC, f0, KK4, 5, data[7]);
	subRound128(CC, DD, AA, BB, f0, KK4, 15, data[10]);
	subRound128(BB, CC, DD, AA, f0, KK4, 8, data[14]);

	T = ctx->digest[1] + DD + C;
        ctx->digest[1] = ctx->digest[2] + AA + D;
	ctx->digest[2] = ctx->digest[3] + BB + A;
	ctx->digest[3] = ctx->digest[0] + CC + B;
	ctx->digest[0] = T;
}

static void ripemd160_transform(struct ripemd_ctx *ctx, unsigned int * data)
{
	unsigned int A, B, C, D, E;   	/* Local vars */
	unsigned int AA, BB, CC, DD, EE;      /* Local vars */
	unsigned int T;

	/* Set up first buffer and local data buffer */
	A = AA = ctx->digest[0];
	B = BB = ctx->digest[1];
	C = CC = ctx->digest[2];
	D = DD = ctx->digest[3];
	E = EE = ctx->digest[4];

/* j=0...15 */
	subRound160(A, B, C, D, E, f0, K0, 11, data[0]);
	subRound160(E, A, B, C, D, f0, K0, 14, data[1]);
	subRound160(D, E, A, B, C, f0, K0, 15, data[2]);
	subRound160(C, D, E, A, B, f0, K0, 12, data[3]);
	subRound160(B, C, D, E, A, f0, K0, 5, data[4]);
	subRound160(A, B, C, D, E, f0, K0, 8, data[5]);
	subRound160(E, A, B, C, D, f0, K0, 7, data[6]);
	subRound160(D, E, A, B, C, f0, K0, 9, data[7]);
	subRound160(C, D, E, A, B, f0, K0, 11, data[8]);
	subRound160(B, C, D, E, A, f0, K0, 13, data[9]);
	subRound160(A, B, C, D, E, f0, K0, 14, data[10]);
	subRound160(E, A, B, C, D, f0, K0, 15, data[11]);
	subRound160(D, E, A, B, C, f0, K0, 6, data[12]);
	subRound160(C, D, E, A, B, f0, K0, 7, data[13]);
	subRound160(B, C, D, E, A, f0, K0, 9, data[14]);
	subRound160(A, B, C, D, E, f0, K0, 8, data[15]);

	subRound160(AA, BB, CC, DD, EE, f64, KK0, 8, data[5]);
	subRound160(EE, AA, BB, CC, DD, f64, KK0, 9, data[14]);
	subRound160(DD, EE, AA, BB, CC, f64, KK0, 9, data[7]);
	subRound160(CC, DD, EE, AA, BB, f64, KK0, 11, data[0]);
	subRound160(BB, CC, DD, EE, AA, f64, KK0, 13, data[9]);
	subRound160(AA, BB, CC, DD, EE, f64, KK0, 15, data[2]);
	subRound160(EE, AA, BB, CC, DD, f64, KK0, 15, data[11]);
	subRound160(DD, EE, AA, BB, CC, f64, KK0, 5, data[4]);
	subRound160(CC, DD, EE, AA, BB, f64, KK0, 7, data[13]);
	subRound160(BB, CC, DD, EE, AA, f64, KK0, 7, data[6]);
	subRound160(AA, BB, CC, DD, EE, f64, KK0, 8, data[15]);
	subRound160(EE, AA, BB, CC, DD, f64, KK0, 11, data[8]);
	subRound160(DD, EE, AA, BB, CC, f64, KK0, 14, data[1]);
	subRound160(CC, DD, EE, AA, BB, f64, KK0, 14, data[10]);
	subRound160(BB, CC, DD, EE, AA, f64, KK0, 12, data[3]);
	subRound160(AA, BB, CC, DD, EE, f64, KK0, 6, data[12]);

/* j=16...31 */
	subRound160(E, A, B, C, D, f16, K1, 7, data[7]);
	subRound160(D, E, A, B, C, f16, K1, 6, data[4]);
	subRound160(C, D, E, A, B, f16, K1, 8, data[13]);
	subRound160(B, C, D, E, A, f16, K1, 13, data[1]);
	subRound160(A, B, C, D, E, f16, K1, 11, data[10]);
	subRound160(E, A, B, C, D, f16, K1, 9, data[6]);
	subRound160(D, E, A, B, C, f16, K1, 7, data[15]);
	subRound160(C, D, E, A, B, f16, K1, 15, data[3]);
	subRound160(B, C, D, E, A, f16, K1, 7, data[12]);
	subRound160(A, B, C, D, E, f16, K1, 12, data[0]);
	subRound160(E, A, B, C, D, f16, K1, 15, data[9]);
	subRound160(D, E, A, B, C, f16, K1, 9, data[5]);
	subRound160(C, D, E, A, B, f16, K1, 11, data[2]);
	subRound160(B, C, D, E, A, f16, K1, 7, data[14]);
	subRound160(A, B, C, D, E, f16, K1, 13, data[11]);
	subRound160(E, A, B, C, D, f16, K1, 12, data[8]);

	subRound160(EE, AA, BB, CC, DD, f48, KK1, 9, data[6]);
	subRound160(DD, EE, AA, BB, CC, f48, KK1, 13, data[11]);
	subRound160(CC, DD, EE, AA, BB, f48, KK1, 15, data[3]);
	subRound160(BB, CC, DD, EE, AA, f48, KK1, 7, data[7]);
	subRound160(AA, BB, CC, DD, EE, f48, KK1, 12, data[0]);
	subRound160(EE, AA, BB, CC, DD, f48, KK1, 8, data[13]);
	subRound160(DD, EE, AA, BB, CC, f48, KK1, 9, data[5]);
	subRound160(CC, DD, EE, AA, BB, f48, KK1, 11, data[10]);
	subRound160(BB, CC, DD, EE, AA, f48, KK1, 7, data[14]);
	subRound160(AA, BB, CC, DD, EE, f48, KK1, 7, data[15]);
	subRound160(EE, AA, BB, CC, DD, f48, KK1, 12, data[8]);
	subRound160(DD, EE, AA, BB, CC, f48, KK1, 7, data[12]);
	subRound160(CC, DD, EE, AA, BB, f48, KK1, 6, data[4]);
	subRound160(BB, CC, DD, EE, AA, f48, KK1, 15, data[9]);
	subRound160(AA, BB, CC, DD, EE, f48, KK1, 13, data[1]);
	subRound160(EE, AA, BB, CC, DD, f48, KK1, 11, data[2]);

/* j=32...47 */
	subRound160(D, E, A, B, C, f32, K2, 11, data[3]);
	subRound160(C, D, E, A, B, f32, K2, 13, data[10]);
	subRound160(B, C, D, E, A, f32, K2, 6, data[14]);
	subRound160(A, B, C, D, E, f32, K2, 7, data[4]);
	subRound160(E, A, B, C, D, f32, K2, 14, data[9]);
	subRound160(D, E, A, B, C, f32, K2, 9, data[15]);
	subRound160(C, D, E, A, B, f32, K2, 13, data[8]);
	subRound160(B, C, D, E, A, f32, K2, 15, data[1]);
	subRound160(A, B, C, D, E, f32, K2, 14, data[2]);
	subRound160(E, A, B, C, D, f32, K2, 8, data[7]);
	subRound160(D, E, A, B, C, f32, K2, 13, data[0]);
	subRound160(C, D, E, A, B, f32, K2, 6, data[6]);
	subRound160(B, C, D, E, A, f32, K2, 5, data[13]);
	subRound160(A, B, C, D, E, f32, K2, 12, data[11]);
	subRound160(E, A, B, C, D, f32, K2, 7, data[5]);
	subRound160(D, E, A, B, C, f32, K2, 5, data[12]);

	subRound160(DD, EE, AA, BB, CC, f32, KK2, 9, data[15]);
	subRound160(CC, DD, EE, AA, BB, f32, KK2, 7, data[5]);
	subRound160(BB, CC, DD, EE, AA, f32, KK2, 15, data[1]);
	subRound160(AA, BB, CC, DD, EE, f32, KK2, 11, data[3]);
	subRound160(EE, AA, BB, CC, DD, f32, KK2, 8, data[7]);
	subRound160(DD, EE, AA, BB, CC, f32, KK2, 6, data[14]);
	subRound160(CC, DD, EE, AA, BB, f32, KK2, 6, data[6]);
	subRound160(BB, CC, DD, EE, AA, f32, KK2, 14, data[9]);
	subRound160(AA, BB, CC, DD, EE, f32, KK2, 12, data[11]);
	subRound160(EE, AA, BB, CC, DD, f32, KK2, 13, data[8]);
	subRound160(DD, EE, AA, BB, CC, f32, KK2, 5, data[12]);
	subRound160(CC, DD, EE, AA, BB, f32, KK2, 14, data[2]);
	subRound160(BB, CC, DD, EE, AA, f32, KK2, 13, data[10]);
	subRound160(AA, BB, CC, DD, EE, f32, KK2, 13, data[0]);
	subRound160(EE, AA, BB, CC, DD, f32, KK2, 7, data[4]);
	subRound160(DD, EE, AA, BB, CC, f32, KK2, 5, data[13]);

/* j=48...63 */
	subRound160(C, D, E, A, B, f48, K3, 11, data[1]);
	subRound160(B, C, D, E, A, f48, K3, 12, data[9]);
	subRound160(A, B, C, D, E, f48, K3, 14, data[11]);
	subRound160(E, A, B, C, D, f48, K3, 15, data[10]);
	subRound160(D, E, A, B, C, f48, K3, 14, data[0]);
	subRound160(C, D, E, A, B, f48, K3, 15, data[8]);
	subRound160(B, C, D, E, A, f48, K3, 9, data[12]);
	subRound160(A, B, C, D, E, f48, K3, 8, data[4]);
	subRound160(E, A, B, C, D, f48, K3, 9, data[13]);
	subRound160(D, E, A, B, C, f48, K3, 14, data[3]);
	subRound160(C, D, E, A, B, f48, K3, 5, data[7]);
	subRound160(B, C, D, E, A, f48, K3, 6, data[15]);
	subRound160(A, B, C, D, E, f48, K3, 8, data[14]);
	subRound160(E, A, B, C, D, f48, K3, 6, data[5]);
	subRound160(D, E, A, B, C, f48, K3, 5, data[6]);
	subRound160(C, D, E, A, B, f48, K3, 12, data[2]);

	subRound160(CC, DD, EE, AA, BB, f16, KK3, 15, data[8]);
	subRound160(BB, CC, DD, EE, AA, f16, KK3, 5, data[6]);
	subRound160(AA, BB, CC, DD, EE, f16, KK3, 8, data[4]);
	subRound160(EE, AA, BB, CC, DD, f16, KK3, 11, data[1]);
	subRound160(DD, EE, AA, BB, CC, f16, KK3, 14, data[3]);
	subRound160(CC, DD, EE, AA, BB, f16, KK3, 14, data[11]);
	subRound160(BB, CC, DD, EE, AA, f16, KK3, 6, data[15]);
	subRound160(AA, BB, CC, DD, EE, f16, KK3, 14, data[0]);
	subRound160(EE, AA, BB, CC, DD, f16, KK3, 6, data[5]);
	subRound160(DD, EE, AA, BB, CC, f16, KK3, 9, data[12]);
	subRound160(CC, DD, EE, AA, BB, f16, KK3, 12, data[2]);
	subRound160(BB, CC, DD, EE, AA, f16, KK3, 9, data[13]);
	subRound160(AA, BB, CC, DD, EE, f16, KK3, 12, data[9]);
	subRound160(EE, AA, BB, CC, DD, f16, KK3, 5, data[7]);
	subRound160(DD, EE, AA, BB, CC, f16, KK3, 15, data[10]);
	subRound160(CC, DD, EE, AA, BB, f16, KK3, 8, data[14]);

/* j=64...79 */
	subRound160(B, C, D, E, A, f64, K4, 9, data[4]);
	subRound160(A, B, C, D, E, f64, K4, 15, data[0]);
	subRound160(E, A, B, C, D, f64, K4, 5, data[5]);
	subRound160(D, E, A, B, C, f64, K4, 11, data[9]);
	subRound160(C, D, E, A, B, f64, K4, 6, data[7]);
	subRound160(B, C, D, E, A, f64, K4, 8, data[12]);
	subRound160(A, B, C, D, E, f64, K4, 13, data[2]);
	subRound160(E, A, B, C, D, f64, K4, 12, data[10]);
	subRound160(D, E, A, B, C, f64, K4, 5, data[14]);
	subRound160(C, D, E, A, B, f64, K4, 12, data[1]);
	subRound160(B, C, D, E, A, f64, K4, 13, data[3]);
	subRound160(A, B, C, D, E, f64, K4, 14, data[8]);
	subRound160(E, A, B, C, D, f64, K4, 11, data[11]);
	subRound160(D, E, A, B, C, f64, K4, 8, data[6]);
	subRound160(C, D, E, A, B, f64, K4, 5, data[15]);
	subRound160(B, C, D, E, A, f64, K4, 6, data[13]);

	subRound160(BB, CC, DD, EE, AA, f0, KK4, 8, data[12]);
	subRound160(AA, BB, CC, DD, EE, f0, KK4, 5, data[15]);
	subRound160(EE, AA, BB, CC, DD, f0, KK4, 12, data[10]);
	subRound160(DD, EE, AA, BB, CC, f0, KK4, 9, data[4]);
	subRound160(CC, DD, EE, AA, BB, f0, KK4, 12, data[1]);
	subRound160(BB, CC, DD, EE, AA, f0, KK4, 5, data[5]);
	subRound160(AA, BB, CC, DD, EE, f0, KK4, 14, data[8]);
	subRound160(EE, AA, BB, CC, DD, f0, KK4, 6, data[7]);
	subRound160(DD, EE, AA, BB, CC, f0, KK4, 8, data[6]);
	subRound160(CC, DD, EE, AA, BB, f0, KK4, 13, data[2]);
	subRound160(BB, CC, DD, EE, AA, f0, KK4, 6, data[13]);
	subRound160(AA, BB, CC, DD, EE, f0, KK4, 5, data[14]);
	subRound160(EE, AA, BB, CC, DD, f0, KK4, 15, data[0]);
	subRound160(DD, EE, AA, BB, CC, f0, KK4, 13, data[3]);
	subRound160(CC, DD, EE, AA, BB, f0, KK4, 11, data[9]);
	subRound160(BB, CC, DD, EE, AA, f0, KK4, 11, data[11]);


	T = ctx->digest[1] + DD + C;
	ctx->digest[1] = ctx->digest[2] + EE + D;
	ctx->digest[2] = ctx->digest[3] + AA + E;
	ctx->digest[3] = ctx->digest[4] + BB + A;
	ctx->digest[4] = ctx->digest[0] + CC + B;
	ctx->digest[0] = T;
}


static void ripemd256_transform(struct ripemd_ctx *ctx, unsigned int * data)
{
	unsigned int A, B, C, D;	/* Local vars */
	unsigned int AA, BB, CC, DD;	/* Local vars */
	unsigned int T;

	/* Set up first buffer and local data buffer */
	A = ctx->digest[0];
	B = ctx->digest[1];
	C = ctx->digest[2];
	D = ctx->digest[3];
	AA = ctx->digest[4];
	BB = ctx->digest[5];
	CC = ctx->digest[6];
	DD = ctx->digest[7];

/* j=0...15 */
	subRound128(A, B, C, D, f0, K0, 11, data[0]);
	subRound128(D, A, B, C, f0, K0, 14, data[1]);
	subRound128(C, D, A, B, f0, K0, 15, data[2]);
	subRound128(B, C, D, A, f0, K0, 12, data[3]);
	subRound128(A, B, C, D, f0, K0, 5, data[4]);
	subRound128(D, A, B, C, f0, K0, 8, data[5]);
	subRound128(C, D, A, B, f0, K0, 7, data[6]);
	subRound128(B, C, D, A, f0, K0, 9, data[7]);
	subRound128(A, B, C, D, f0, K0, 11, data[8]);
	subRound128(D, A, B, C, f0, K0, 13, data[9]);
	subRound128(C, D, A, B, f0, K0, 14, data[10]);
	subRound128(B, C, D, A, f0, K0, 15, data[11]);
	subRound128(A, B, C, D, f0, K0, 6, data[12]);
	subRound128(D, A, B, C, f0, K0, 7, data[13]);
	subRound128(C, D, A, B, f0, K0, 9, data[14]);
	subRound128(B, C, D, A, f0, K0, 8, data[15]);

	subRound128(AA, BB, CC, DD, f48, KK0, 8, data[5]);
	subRound128(DD, AA, BB, CC, f48, KK0, 9, data[14]);
	subRound128(CC, DD, AA, BB, f48, KK0, 9, data[7]);
	subRound128(BB, CC, DD, AA, f48, KK0, 11, data[0]);
	subRound128(AA, BB, CC, DD, f48, KK0, 13, data[9]);
	subRound128(DD, AA, BB, CC, f48, KK0, 15, data[2]);
	subRound128(CC, DD, AA, BB, f48, KK0, 15, data[11]);
	subRound128(BB, CC, DD, AA, f48, KK0, 5, data[4]);
	subRound128(AA, BB, CC, DD, f48, KK0, 7, data[13]);
	subRound128(DD, AA, BB, CC, f48, KK0, 7, data[6]);
	subRound128(CC, DD, AA, BB, f48, KK0, 8, data[15]);
	subRound128(BB, CC, DD, AA, f48, KK0, 11, data[8]);
	subRound128(AA, BB, CC, DD, f48, KK0, 14, data[1]);
	subRound128(DD, AA, BB, CC, f48, KK0, 14, data[10]);
	subRound128(CC, DD, AA, BB, f48, KK0, 12, data[3]);
	subRound128(BB, CC, DD, AA, f48, KK0, 6, data[12]);

	T = A; A = AA; AA = T;

/* j=16...31 */
	subRound128(A, B, C, D, f16, K1, 7, data[7]);
	subRound128(D, A, B, C, f16, K1, 6, data[4]);
	subRound128(C, D, A, B, f16, K1, 8, data[13]);
	subRound128(B, C, D, A, f16, K1, 13, data[1]);
	subRound128(A, B, C, D, f16, K1, 11, data[10]);
	subRound128(D, A, B, C, f16, K1, 9, data[6]);
	subRound128(C, D, A, B, f16, K1, 7, data[15]);
	subRound128(B, C, D, A, f16, K1, 15, data[3]);
	subRound128(A, B, C, D, f16, K1, 7, data[12]);
	subRound128(D, A, B, C, f16, K1, 12, data[0]);
	subRound128(C, D, A, B, f16, K1, 15, data[9]);
	subRound128(B, C, D, A, f16, K1, 9, data[5]);
	subRound128(A, B, C, D, f16, K1, 11, data[2]);
	subRound128(D, A, B, C, f16, K1, 7, data[14]);
	subRound128(C, D, A, B, f16, K1, 13, data[11]);
	subRound128(B, C, D, A, f16, K1, 12, data[8]);

	subRound128(AA, BB, CC, DD, f32, KK1, 9, data[6]);
	subRound128(DD, AA, BB, CC, f32, KK1, 13, data[11]);
	subRound128(CC, DD, AA, BB, f32, KK1, 15, data[3]);
	subRound128(BB, CC, DD, AA, f32, KK1, 7, data[7]);
	subRound128(AA, BB, CC, DD, f32, KK1, 12, data[0]);
	subRound128(DD, AA, BB, CC, f32, KK1, 8, data[13]);
	subRound128(CC, DD, AA, BB, f32, KK1, 9, data[5]);
	subRound128(BB, CC, DD, AA, f32, KK1, 11, data[10]);
	subRound128(AA, BB, CC, DD, f32, KK1, 7, data[14]);
	subRound128(DD, AA, BB, CC, f32, KK1, 7, data[15]);
	subRound128(CC, DD, AA, BB, f32, KK1, 12, data[8]);
	subRound128(BB, CC, DD, AA, f32, KK1, 7, data[12]);
	subRound128(AA, BB, CC, DD, f32, KK1, 6, data[4]);
	subRound128(DD, AA, BB, CC, f32, KK1, 15, data[9]);
	subRound128(CC, DD, AA, BB, f32, KK1, 13, data[1]);
	subRound128(BB, CC, DD, AA, f32, KK1, 11, data[2]);

	T = B; B = BB; BB = T;

/* j=32...47 */
	subRound128(A, B, C, D, f32, K2, 11, data[3]);
	subRound128(D, A, B, C, f32, K2, 13, data[10]);
	subRound128(C, D, A, B, f32, K2, 6, data[14]);
	subRound128(B, C, D, A, f32, K2, 7, data[4]);
	subRound128(A, B, C, D, f32, K2, 14, data[9]);
	subRound128(D, A, B, C, f32, K2, 9, data[15]);
	subRound128(C, D, A, B, f32, K2, 13, data[8]);
	subRound128(B, C, D, A, f32, K2, 15, data[1]);
	subRound128(A, B, C, D, f32, K2, 14, data[2]);
	subRound128(D, A, B, C, f32, K2, 8, data[7]);
	subRound128(C, D, A, B, f32, K2, 13, data[0]);
	subRound128(B, C, D, A, f32, K2, 6, data[6]);
	subRound128(A, B, C, D, f32, K2, 5, data[13]);
	subRound128(D, A, B, C, f32, K2, 12, data[11]);
	subRound128(C, D, A, B, f32, K2, 7, data[5]);
	subRound128(B, C, D, A, f32, K2, 5, data[12]);

	subRound128(AA, BB, CC, DD, f16, KK2, 9, data[15]);
	subRound128(DD, AA, BB, CC, f16, KK2, 7, data[5]);
	subRound128(CC, DD, AA, BB, f16, KK2, 15, data[1]);
	subRound128(BB, CC, DD, AA, f16, KK2, 11, data[3]);
	subRound128(AA, BB, CC, DD, f16, KK2, 8, data[7]);
	subRound128(DD, AA, BB, CC, f16, KK2, 6, data[14]);
	subRound128(CC, DD, AA, BB, f16, KK2, 6, data[6]);
	subRound128(BB, CC, DD, AA, f16, KK2, 14, data[9]);
	subRound128(AA, BB, CC, DD, f16, KK2, 12, data[11]);
	subRound128(DD, AA, BB, CC, f16, KK2, 13, data[8]);
	subRound128(CC, DD, AA, BB, f16, KK2, 5, data[12]);
	subRound128(BB, CC, DD, AA, f16, KK2, 14, data[2]);
	subRound128(AA, BB, CC, DD, f16, KK2, 13, data[10]);
	subRound128(DD, AA, BB, CC, f16, KK2, 13, data[0]);
	subRound128(CC, DD, AA, BB, f16, KK2, 7, data[4]);
	subRound128(BB, CC, DD, AA, f16, KK2, 5, data[13]);

	T = C; C = CC; CC = T;

/* j=48...63 */
	subRound128(A, B, C, D, f48, K3, 11, data[1]);
	subRound128(D, A, B, C, f48, K3, 12, data[9]);
	subRound128(C, D, A, B, f48, K3, 14, data[11]);
	subRound128(B, C, D, A, f48, K3, 15, data[10]);
	subRound128(A, B, C, D, f48, K3, 14, data[0]);
	subRound128(D, A, B, C, f48, K3, 15, data[8]);
	subRound128(C, D, A, B, f48, K3, 9, data[12]);
	subRound128(B, C, D, A, f48, K3, 8, data[4]);
	subRound128(A, B, C, D, f48, K3, 9, data[13]);
	subRound128(D, A, B, C, f48, K3, 14, data[3]);
	subRound128(C, D, A, B, f48, K3, 5, data[7]);
	subRound128(B, C, D, A, f48, K3, 6, data[15]);
	subRound128(A, B, C, D, f48, K3, 8, data[14]);
	subRound128(D, A, B, C, f48, K3, 6, data[5]);
	subRound128(C, D, A, B, f48, K3, 5, data[6]);
	subRound128(B, C, D, A, f48, K3, 12, data[2]);

	subRound128(AA, BB, CC, DD, f0, KK4, 15, data[8]);
	subRound128(DD, AA, BB, CC, f0, KK4, 5, data[6]);
	subRound128(CC, DD, AA, BB, f0, KK4, 8, data[4]);
	subRound128(BB, CC, DD, AA, f0, KK4, 11, data[1]);
	subRound128(AA, BB, CC, DD, f0, KK4, 14, data[3]);
	subRound128(DD, AA, BB, CC, f0, KK4, 14, data[11]);
	subRound128(CC, DD, AA, BB, f0, KK4, 6, data[15]);
	subRound128(BB, CC, DD, AA, f0, KK4, 14, data[0]);
	subRound128(AA, BB, CC, DD, f0, KK4, 6, data[5]);
	subRound128(DD, AA, BB, CC, f0, KK4, 9, data[12]);
	subRound128(CC, DD, AA, BB, f0, KK4, 12, data[2]);
	subRound128(BB, CC, DD, AA, f0, KK4, 9, data[13]);
	subRound128(AA, BB, CC, DD, f0, KK4, 12, data[9]);
	subRound128(DD, AA, BB, CC, f0, KK4, 5, data[7]);
	subRound128(CC, DD, AA, BB, f0, KK4, 15, data[10]);
	subRound128(BB, CC, DD, AA, f0, KK4, 8, data[14]);

	/* T = D; D = DD; DD = T; */

        ctx->digest[0] += A;
        ctx->digest[1] += B;
        ctx->digest[2] += C;
        ctx->digest[3] += DD;
        ctx->digest[4] += AA;
        ctx->digest[5] += BB;
        ctx->digest[6] += CC;
        ctx->digest[7] += D;
}

static void ripemd320_transform(struct ripemd_ctx *ctx, unsigned int * data)
{
	unsigned int A, B, C, D, E;   	/* Local vars */
	unsigned int AA, BB, CC, DD, EE;      /* Local vars */
	unsigned int T;

	/* Set up first buffer and local data buffer */
	A = ctx->digest[0];
	B = ctx->digest[1];
	C = ctx->digest[2];
	D = ctx->digest[3];
	E = ctx->digest[4];
	AA = ctx->digest[5];
	BB = ctx->digest[6];
	CC = ctx->digest[7];
	DD = ctx->digest[8];
	EE = ctx->digest[9];

/* j=0...15 */
	subRound160(A, B, C, D, E, f0, K0, 11, data[0]);
	subRound160(E, A, B, C, D, f0, K0, 14, data[1]);
	subRound160(D, E, A, B, C, f0, K0, 15, data[2]);
	subRound160(C, D, E, A, B, f0, K0, 12, data[3]);
	subRound160(B, C, D, E, A, f0, K0, 5, data[4]);
	subRound160(A, B, C, D, E, f0, K0, 8, data[5]);
	subRound160(E, A, B, C, D, f0, K0, 7, data[6]);
	subRound160(D, E, A, B, C, f0, K0, 9, data[7]);
	subRound160(C, D, E, A, B, f0, K0, 11, data[8]);
	subRound160(B, C, D, E, A, f0, K0, 13, data[9]);
	subRound160(A, B, C, D, E, f0, K0, 14, data[10]);
	subRound160(E, A, B, C, D, f0, K0, 15, data[11]);
	subRound160(D, E, A, B, C, f0, K0, 6, data[12]);
	subRound160(C, D, E, A, B, f0, K0, 7, data[13]);
	subRound160(B, C, D, E, A, f0, K0, 9, data[14]);
	subRound160(A, B, C, D, E, f0, K0, 8, data[15]);

	subRound160(AA, BB, CC, DD, EE, f64, KK0, 8, data[5]);
	subRound160(EE, AA, BB, CC, DD, f64, KK0, 9, data[14]);
	subRound160(DD, EE, AA, BB, CC, f64, KK0, 9, data[7]);
	subRound160(CC, DD, EE, AA, BB, f64, KK0, 11, data[0]);
	subRound160(BB, CC, DD, EE, AA, f64, KK0, 13, data[9]);
	subRound160(AA, BB, CC, DD, EE, f64, KK0, 15, data[2]);
	subRound160(EE, AA, BB, CC, DD, f64, KK0, 15, data[11]);
	subRound160(DD, EE, AA, BB, CC, f64, KK0, 5, data[4]);
	subRound160(CC, DD, EE, AA, BB, f64, KK0, 7, data[13]);
	subRound160(BB, CC, DD, EE, AA, f64, KK0, 7, data[6]);
	subRound160(AA, BB, CC, DD, EE, f64, KK0, 8, data[15]);
	subRound160(EE, AA, BB, CC, DD, f64, KK0, 11, data[8]);
	subRound160(DD, EE, AA, BB, CC, f64, KK0, 14, data[1]);
	subRound160(CC, DD, EE, AA, BB, f64, KK0, 14, data[10]);
	subRound160(BB, CC, DD, EE, AA, f64, KK0, 12, data[3]);
	subRound160(AA, BB, CC, DD, EE, f64, KK0, 6, data[12]);

	T = A; A = AA; AA = T;

/* j=16...31 */
	subRound160(E, A, B, C, D, f16, K1, 7, data[7]);
	subRound160(D, E, A, B, C, f16, K1, 6, data[4]);
	subRound160(C, D, E, A, B, f16, K1, 8, data[13]);
	subRound160(B, C, D, E, A, f16, K1, 13, data[1]);
	subRound160(A, B, C, D, E, f16, K1, 11, data[10]);
	subRound160(E, A, B, C, D, f16, K1, 9, data[6]);
	subRound160(D, E, A, B, C, f16, K1, 7, data[15]);
	subRound160(C, D, E, A, B, f16, K1, 15, data[3]);
	subRound160(B, C, D, E, A, f16, K1, 7, data[12]);
	subRound160(A, B, C, D, E, f16, K1, 12, data[0]);
	subRound160(E, A, B, C, D, f16, K1, 15, data[9]);
	subRound160(D, E, A, B, C, f16, K1, 9, data[5]);
	subRound160(C, D, E, A, B, f16, K1, 11, data[2]);
	subRound160(B, C, D, E, A, f16, K1, 7, data[14]);
	subRound160(A, B, C, D, E, f16, K1, 13, data[11]);
	subRound160(E, A, B, C, D, f16, K1, 12, data[8]);

	subRound160(EE, AA, BB, CC, DD, f48, KK1, 9, data[6]);
	subRound160(DD, EE, AA, BB, CC, f48, KK1, 13, data[11]);
	subRound160(CC, DD, EE, AA, BB, f48, KK1, 15, data[3]);
	subRound160(BB, CC, DD, EE, AA, f48, KK1, 7, data[7]);
	subRound160(AA, BB, CC, DD, EE, f48, KK1, 12, data[0]);
	subRound160(EE, AA, BB, CC, DD, f48, KK1, 8, data[13]);
	subRound160(DD, EE, AA, BB, CC, f48, KK1, 9, data[5]);
	subRound160(CC, DD, EE, AA, BB, f48, KK1, 11, data[10]);
	subRound160(BB, CC, DD, EE, AA, f48, KK1, 7, data[14]);
	subRound160(AA, BB, CC, DD, EE, f48, KK1, 7, data[15]);
	subRound160(EE, AA, BB, CC, DD, f48, KK1, 12, data[8]);
	subRound160(DD, EE, AA, BB, CC, f48, KK1, 7, data[12]);
	subRound160(CC, DD, EE, AA, BB, f48, KK1, 6, data[4]);
	subRound160(BB, CC, DD, EE, AA, f48, KK1, 15, data[9]);
	subRound160(AA, BB, CC, DD, EE, f48, KK1, 13, data[1]);
	subRound160(EE, AA, BB, CC, DD, f48, KK1, 11, data[2]);

	T = B; B = BB; BB = T;

/* j=32...47 */
	subRound160(D, E, A, B, C, f32, K2, 11, data[3]);
	subRound160(C, D, E, A, B, f32, K2, 13, data[10]);
	subRound160(B, C, D, E, A, f32, K2, 6, data[14]);
	subRound160(A, B, C, D, E, f32, K2, 7, data[4]);
	subRound160(E, A, B, C, D, f32, K2, 14, data[9]);
	subRound160(D, E, A, B, C, f32, K2, 9, data[15]);
	subRound160(C, D, E, A, B, f32, K2, 13, data[8]);
	subRound160(B, C, D, E, A, f32, K2, 15, data[1]);
	subRound160(A, B, C, D, E, f32, K2, 14, data[2]);
	subRound160(E, A, B, C, D, f32, K2, 8, data[7]);
	subRound160(D, E, A, B, C, f32, K2, 13, data[0]);
	subRound160(C, D, E, A, B, f32, K2, 6, data[6]);
	subRound160(B, C, D, E, A, f32, K2, 5, data[13]);
	subRound160(A, B, C, D, E, f32, K2, 12, data[11]);
	subRound160(E, A, B, C, D, f32, K2, 7, data[5]);
	subRound160(D, E, A, B, C, f32, K2, 5, data[12]);

	subRound160(DD, EE, AA, BB, CC, f32, KK2, 9, data[15]);
	subRound160(CC, DD, EE, AA, BB, f32, KK2, 7, data[5]);
	subRound160(BB, CC, DD, EE, AA, f32, KK2, 15, data[1]);
	subRound160(AA, BB, CC, DD, EE, f32, KK2, 11, data[3]);
	subRound160(EE, AA, BB, CC, DD, f32, KK2, 8, data[7]);
	subRound160(DD, EE, AA, BB, CC, f32, KK2, 6, data[14]);
	subRound160(CC, DD, EE, AA, BB, f32, KK2, 6, data[6]);
	subRound160(BB, CC, DD, EE, AA, f32, KK2, 14, data[9]);
	subRound160(AA, BB, CC, DD, EE, f32, KK2, 12, data[11]);
	subRound160(EE, AA, BB, CC, DD, f32, KK2, 13, data[8]);
	subRound160(DD, EE, AA, BB, CC, f32, KK2, 5, data[12]);
	subRound160(CC, DD, EE, AA, BB, f32, KK2, 14, data[2]);
	subRound160(BB, CC, DD, EE, AA, f32, KK2, 13, data[10]);
	subRound160(AA, BB, CC, DD, EE, f32, KK2, 13, data[0]);
	subRound160(EE, AA, BB, CC, DD, f32, KK2, 7, data[4]);
	subRound160(DD, EE, AA, BB, CC, f32, KK2, 5, data[13]);

	T = C; C = CC; CC = T;

/* j=48...63 */
	subRound160(C, D, E, A, B, f48, K3, 11, data[1]);
	subRound160(B, C, D, E, A, f48, K3, 12, data[9]);
	subRound160(A, B, C, D, E, f48, K3, 14, data[11]);
	subRound160(E, A, B, C, D, f48, K3, 15, data[10]);
	subRound160(D, E, A, B, C, f48, K3, 14, data[0]);
	subRound160(C, D, E, A, B, f48, K3, 15, data[8]);
	subRound160(B, C, D, E, A, f48, K3, 9, data[12]);
	subRound160(A, B, C, D, E, f48, K3, 8, data[4]);
	subRound160(E, A, B, C, D, f48, K3, 9, data[13]);
	subRound160(D, E, A, B, C, f48, K3, 14, data[3]);
	subRound160(C, D, E, A, B, f48, K3, 5, data[7]);
	subRound160(B, C, D, E, A, f48, K3, 6, data[15]);
	subRound160(A, B, C, D, E, f48, K3, 8, data[14]);
	subRound160(E, A, B, C, D, f48, K3, 6, data[5]);
	subRound160(D, E, A, B, C, f48, K3, 5, data[6]);
	subRound160(C, D, E, A, B, f48, K3, 12, data[2]);

	subRound160(CC, DD, EE, AA, BB, f16, KK3, 15, data[8]);
	subRound160(BB, CC, DD, EE, AA, f16, KK3, 5, data[6]);
	subRound160(AA, BB, CC, DD, EE, f16, KK3, 8, data[4]);
	subRound160(EE, AA, BB, CC, DD, f16, KK3, 11, data[1]);
	subRound160(DD, EE, AA, BB, CC, f16, KK3, 14, data[3]);
	subRound160(CC, DD, EE, AA, BB, f16, KK3, 14, data[11]);
	subRound160(BB, CC, DD, EE, AA, f16, KK3, 6, data[15]);
	subRound160(AA, BB, CC, DD, EE, f16, KK3, 14, data[0]);
	subRound160(EE, AA, BB, CC, DD, f16, KK3, 6, data[5]);
	subRound160(DD, EE, AA, BB, CC, f16, KK3, 9, data[12]);
	subRound160(CC, DD, EE, AA, BB, f16, KK3, 12, data[2]);
	subRound160(BB, CC, DD, EE, AA, f16, KK3, 9, data[13]);
	subRound160(AA, BB, CC, DD, EE, f16, KK3, 12, data[9]);
	subRound160(EE, AA, BB, CC, DD, f16, KK3, 5, data[7]);
	subRound160(DD, EE, AA, BB, CC, f16, KK3, 15, data[10]);
	subRound160(CC, DD, EE, AA, BB, f16, KK3, 8, data[14]);

	T = D; D = DD; DD = T;

/* j=64...79 */
	subRound160(B, C, D, E, A, f64, K4, 9, data[4]);
	subRound160(A, B, C, D, E, f64, K4, 15, data[0]);
	subRound160(E, A, B, C, D, f64, K4, 5, data[5]);
	subRound160(D, E, A, B, C, f64, K4, 11, data[9]);
	subRound160(C, D, E, A, B, f64, K4, 6, data[7]);
	subRound160(B, C, D, E, A, f64, K4, 8, data[12]);
	subRound160(A, B, C, D, E, f64, K4, 13, data[2]);
	subRound160(E, A, B, C, D, f64, K4, 12, data[10]);
	subRound160(D, E, A, B, C, f64, K4, 5, data[14]);
	subRound160(C, D, E, A, B, f64, K4, 12, data[1]);
	subRound160(B, C, D, E, A, f64, K4, 13, data[3]);
	subRound160(A, B, C, D, E, f64, K4, 14, data[8]);
	subRound160(E, A, B, C, D, f64, K4, 11, data[11]);
	subRound160(D, E, A, B, C, f64, K4, 8, data[6]);
	subRound160(C, D, E, A, B, f64, K4, 5, data[15]);
	subRound160(B, C, D, E, A, f64, K4, 6, data[13]);

	subRound160(BB, CC, DD, EE, AA, f0, KK4, 8, data[12]);
	subRound160(AA, BB, CC, DD, EE, f0, KK4, 5, data[15]);
	subRound160(EE, AA, BB, CC, DD, f0, KK4, 12, data[10]);
	subRound160(DD, EE, AA, BB, CC, f0, KK4, 9, data[4]);
	subRound160(CC, DD, EE, AA, BB, f0, KK4, 12, data[1]);
	subRound160(BB, CC, DD, EE, AA, f0, KK4, 5, data[5]);
	subRound160(AA, BB, CC, DD, EE, f0, KK4, 14, data[8]);
	subRound160(EE, AA, BB, CC, DD, f0, KK4, 6, data[7]);
	subRound160(DD, EE, AA, BB, CC, f0, KK4, 8, data[6]);
	subRound160(CC, DD, EE, AA, BB, f0, KK4, 13, data[2]);
	subRound160(BB, CC, DD, EE, AA, f0, KK4, 6, data[13]);
	subRound160(AA, BB, CC, DD, EE, f0, KK4, 5, data[14]);
	subRound160(EE, AA, BB, CC, DD, f0, KK4, 15, data[0]);
	subRound160(DD, EE, AA, BB, CC, f0, KK4, 13, data[3]);
	subRound160(CC, DD, EE, AA, BB, f0, KK4, 11, data[9]);
	subRound160(BB, CC, DD, EE, AA, f0, KK4, 11, data[11]);

	/* T = E; E = EE; EE = T; */

        ctx->digest[0] += A;
        ctx->digest[1] += B;
        ctx->digest[2] += C;
        ctx->digest[3] += D;
        ctx->digest[4] += EE;
        ctx->digest[5] += AA;
        ctx->digest[6] += BB;
        ctx->digest[7] += CC;
        ctx->digest[8] += DD;
        ctx->digest[9] += E;
}

static void ripemd_transform(struct ripemd_ctx *ctx, unsigned int * data)
{
  switch(ctx->digest_len)  /* select the right compression function */
    {
    case 8 * RIPEMD128_DIGESTSIZE: 
      ripemd128_transform(ctx, data); break;
    case 8 * RIPEMD160_DIGESTSIZE: 
      ripemd160_transform(ctx, data); break;
    case 8 * RIPEMD256_DIGESTSIZE: 
      ripemd256_transform(ctx, data); break;
    case 8 * RIPEMD320_DIGESTSIZE: 
      ripemd320_transform(ctx, data); break;
    }
}

#ifndef EXTRACT_UCHAR
#define EXTRACT_UCHAR(p)  (*(unsigned char *)(p))
#endif

#define STRING2INT(s) ((((((EXTRACT_UCHAR(s+3) << 8)    \
			 | EXTRACT_UCHAR(s+2)) << 8)  \
			 | EXTRACT_UCHAR(s+1)) << 8)  \
			 | EXTRACT_UCHAR(s))

static void ripemd_block(struct ripemd_ctx *ctx, unsigned char *block)
{
	unsigned int data[RIPEMD_DATALEN];
	unsigned int i;

	/* Update bit count */
	ctx->bitcount += RIPEMD_DATASIZE * 8;

	/* Endian independent conversion */
	for (i = 0; i < RIPEMD_DATALEN; i++, block += 4)
		data[i] = STRING2INT(block);

	ripemd_transform(ctx, data);
}

void ripemd_update(struct ripemd_ctx *ctx, unsigned char *buffer, unsigned int len)
{
	if (ctx->index) {	/* Try to fill partial block */
		unsigned left = RIPEMD_DATASIZE - ctx->index;
		if (len < left) {
			memcpy(ctx->block + ctx->index, buffer, len);
			ctx->index += len;
			return;	/* Finished */
		} else {
			memcpy(ctx->block + ctx->index, buffer, left);
			ripemd_block(ctx, ctx->block);
			buffer += left;
			len -= left;
		}
	}
	while (len >= RIPEMD_DATASIZE) {
		ripemd_block(ctx, buffer);
		buffer += RIPEMD_DATASIZE;
		len -= RIPEMD_DATASIZE;
	}
	if ((ctx->index = len))
		/* This assignment is intended */
		/* Buffer leftovers */
		memcpy(ctx->block, buffer, len);
}

/* Final wrapup - pad to RIPEMD_DATASIZE-byte boundary with the bit pattern
   1 0* (64-bit count of bits processed, LSB-first) */

void ripemd_final(struct ripemd_ctx *ctx, unsigned char * s)
{
	unsigned int data[RIPEMD_DATALEN];
	unsigned int i;
	unsigned int words;

	i = ctx->index;
	/* Set the first char of padding to 0x80.  This is safe since there is
	   always at least one byte free */
	ctx->block[i++] = 0x80;

	/* Fill rest of word */
	for (; i & 3; i++)
		ctx->block[i] = 0;

	/* i is now a multiple of the word size 4 */
	words = i >> 2;
	for (i = 0; i < words; i++)
		data[i] = STRING2INT(ctx->block + 4 * i);

	if (words > (RIPEMD_DATALEN - 2)) { /* No room for length in this block. 
				 * Process it and pad with another one */
		for (i = words; i < RIPEMD_DATALEN; i++)
			data[i] = 0;
		ripemd_transform(ctx, data);
		for (i = 0; i < (RIPEMD_DATALEN - 2); i++)
			data[i] = 0;
	} else
		for (i = words; i < RIPEMD_DATALEN - 2; i++)
			data[i] = 0;

	/* add length padding */
	ctx->bitcount += 8 * ctx->index;
	data[RIPEMD_DATALEN - 2] =
	  (unsigned int)(ctx->bitcount & ((1LL << 32) - 1));
	data[RIPEMD_DATALEN - 1] = 
	  (unsigned int)(ctx->bitcount >> 32);

	ripemd_transform(ctx, data);

	if (s!=NULL)
	for (i = 0; i < ctx->digest_len / 32; i++) {
		*s++ = 0xff & ctx->digest[i];
		*s++ = 0xff & (ctx->digest[i] >> 8);
		*s++ = 0xff & (ctx->digest[i] >> 16);
		*s++ = 0xff & (ctx->digest[i] >> 24);
	}
}
