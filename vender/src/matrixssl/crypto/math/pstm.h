/*
 *	pstm.h
 *	Release $Name: MATRIXSSL-3-4-2-OPEN $
 *
 *	multiple-precision integer library
 */
/*
 *	Copyright (c) 2013 INSIDE Secure Corporation
 *	Copyright (c) PeerSec Networks, 2002-2011
 *	All Rights Reserved
 *
 *	The latest version of this code is available at http://www.matrixssl.org
 *
 *	This software is open source; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This General Public License does NOT permit incorporating this software 
 *	into proprietary programs.  If you are unable to comply with the GPL, a 
 *	commercial license for this software may be purchased from INSIDE at
 *	http://www.insidesecure.com/eng/Company/Locations
 *	
 *	This program is distributed in WITHOUT ANY WARRANTY; without even the 
 *	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 *	See the GNU General Public License for more details.
 *	
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *	http://www.gnu.org/copyleft/gpl.html
 */
/******************************************************************************/

#ifndef _h_PSTMATH
#define _h_PSTMATH

/* Define this here to avoid including circular limits.h on some platforms */
#ifndef CHAR_BIT
#define CHAR_BIT	8
#endif

/******************************************************************************/
/*
    If native 64 bit integers are not supported, we do not support 32x32->64 
	in hardware, so we must set the 16 bit flag to produce 16x16->32 products.
*/
#ifndef HAVE_NATIVE_INT64
    #define PSTM_16BIT
#endif /* ! HAVE_NATIVE_INT64 */

/******************************************************************************/
/*
	Some default configurations.

	pstm_word should be the largest value the processor can hold as the product
		of a multiplication. Most platforms support a 32x32->64 MAC instruction,
		so 64bits is the default pstm_word size.
	pstm_digit should be half the size of pstm_word
 */
#ifdef PSTM_8BIT
/*	8-bit digits, 16-bit word products */
	typedef unsigned char		pstm_digit;
	typedef unsigned short		pstm_word;
	#define DIGIT_BIT			8
	
#elif defined(PSTM_16BIT)
/*	16-bit digits, 32-bit word products */
	typedef unsigned short		pstm_digit;
	typedef unsigned long		pstm_word;
	#define	DIGIT_BIT			16
	
#elif defined(PSTM_64BIT)
/*	64-bit digits, 128-bit word products */
	#ifndef __GNUC__
	#error "64bit digits requires GCC"
	#endif
	typedef unsigned long		pstm_digit;
	typedef unsigned long		pstm_word __attribute__ ((mode(TI)));
	#define DIGIT_BIT			64
	
#else
/*	This is the default case, 32-bit digits, 64-bit word products */
	typedef uint32			pstm_digit;
	typedef uint64			pstm_word;
	#define DIGIT_BIT		32
	#define PSTM_32BIT
#endif /* digit and word size */

#define PSTM_MASK			(pstm_digit)(-1)
#define PSTM_DIGIT_MAX		PSTM_MASK

/******************************************************************************/
/*
	equalities
 */
#define PSTM_LT			-1		/* less than */
#define PSTM_EQ			0		/* equal to */
#define PSTM_GT			1		/* greater than */

#define PSTM_ZPOS		0		/* positive integer */
#define PSTM_NEG		1		/* negative */

#define PSTM_OKAY		PS_SUCCESS
#define PSTM_MEM		PS_MEM_FAIL

/******************************************************************************/
/*
	Various build options
 */
#define PSTM_DEFAULT_INIT 64		/* default (64) digits of allocation */
#define PSTM_MAX_SIZE	4096

typedef struct  {
	int16	used, alloc, sign;
	pstm_digit *dp;
} pstm_int;

/******************************************************************************/
/*
	Operations on large integers
 */
#define pstm_iszero(a) (((a)->used == 0) ? PS_TRUE : PS_FALSE)
#define pstm_iseven(a) (((a)->used > 0 && (((a)->dp[0] & 1) == 0)) ? PS_TRUE : PS_FALSE)
#define pstm_isodd(a)  (((a)->used > 0 && (((a)->dp[0] & 1) == 1)) ? PS_TRUE : PS_FALSE)
#define pstm_abs(a, b)  { pstm_copy(a, b); (b)->sign  = 0; }

extern void pstm_set(pstm_int *a, pstm_digit b);

extern void pstm_zero(pstm_int * a);

extern int32 pstm_init(psPool_t *pool, pstm_int * a);

extern int32 pstm_init_size(psPool_t *pool, pstm_int * a, uint32 size);

extern int32 pstm_init_copy(psPool_t *pool, pstm_int * a, pstm_int * b,
				int16 toSqr);

extern int16 pstm_count_bits (pstm_int * a);

extern int32 pstm_init_for_read_unsigned_bin(psPool_t *pool, pstm_int *a,
				uint32 len);

extern int32 pstm_read_unsigned_bin(pstm_int *a, unsigned char *b, int32 c);

extern int32 pstm_unsigned_bin_size(pstm_int *a);	

extern int32 pstm_copy(pstm_int * a, pstm_int * b);

extern void pstm_exch(pstm_int * a, pstm_int * b);

extern void pstm_clear(pstm_int * a);

extern void pstm_clear_multi(pstm_int *mp0, pstm_int *mp1, pstm_int *mp2,
				pstm_int *mp3, pstm_int *mp4, pstm_int *mp5, pstm_int *mp6,
				pstm_int *mp7);

extern int32 pstm_grow(pstm_int * a, int16 size);

extern void pstm_clamp(pstm_int * a);

extern int32 pstm_cmp(pstm_int * a, pstm_int * b);

extern int32 pstm_cmp_mag(pstm_int * a, pstm_int * b);

extern void pstm_rshd(pstm_int *a, int16 x);

extern int32 pstm_lshd(pstm_int * a, int16 b);

extern int32 pstm_div(psPool_t *pool, pstm_int *a, pstm_int *b, pstm_int *c,
				pstm_int *d);
				
extern int32 pstm_div_2d(psPool_t *pool, pstm_int *a, int16 b, pstm_int *c,
				pstm_int *d);
	
extern int32 pstm_div_2(pstm_int * a, pstm_int * b);											

extern int32 s_pstm_sub(pstm_int *a, pstm_int *b, pstm_int *c);

extern int32 pstm_sub(pstm_int *a, pstm_int *b, pstm_int *c);

extern int32 pstm_sub_d(psPool_t *pool, pstm_int *a, pstm_digit b, pstm_int *c);

extern int32 pstm_mul_2(pstm_int * a, pstm_int * b);

extern int32 pstm_mod(psPool_t *pool, pstm_int *a, pstm_int *b, pstm_int *c);

extern int32 pstm_mulmod(psPool_t *pool, pstm_int *a, pstm_int *b, pstm_int *c,
				pstm_int *d);
			
extern int32 pstm_exptmod(psPool_t *pool, pstm_int *G, pstm_int *X, pstm_int *P,
				pstm_int *Y);

extern int32 pstm_2expt(pstm_int *a, int16 b);				
			
extern int32 pstm_add(pstm_int *a, pstm_int *b, pstm_int *c);

extern int32 pstm_to_unsigned_bin(psPool_t *pool, pstm_int *a,
				unsigned char *b);
								
extern int32 pstm_montgomery_setup(pstm_int *a, pstm_digit *rho);
				
extern int32 pstm_montgomery_reduce(psPool_t *pool, pstm_int *a, pstm_int *m,
				pstm_digit mp, pstm_digit *paD, uint32 paDlen);

extern int32 pstm_mul_comba(psPool_t *pool, pstm_int *A, pstm_int *B,
				pstm_int *C, pstm_digit *paD, uint32 paDlen);
				
extern int32 pstm_sqr_comba(psPool_t *pool, pstm_int *A, pstm_int *B,
				pstm_digit *paD, uint32 paDlen);
				
extern int32 pstm_cmp_d(pstm_int *a, pstm_digit b);
				
extern int32 pstm_montgomery_calc_normalization(pstm_int *a, pstm_int *b);

extern int32 pstm_mul_d(pstm_int *a, pstm_digit b, pstm_int *c);
#endif /* _h_PSTMATH */

