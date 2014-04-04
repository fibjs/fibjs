/*
 *	yarrow.c
 *	Release $Name: MATRIXSSL-3-4-2-OPEN $
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

#include "../cryptoApi.h"

#ifdef USE_YARROW
/******************************************************************************/
/*
	A basic yarrow implementation hardcoded to AES and SHA.  Only one
	pool is used.
	
	The sequence of calls to start getting PRN is to call psYarrowStart,
	psYarrowAddEntropy, psYarrowReseed, and finally psYarrowRead.  Throughout
	the life of the application, psYarrowAddEntropy can be called when random
	data is read from the platform.  The psYarrowReseed function must be
	called to incorporate that entropy into the new key.
	
	Warning:  no mutex lock embedded in these calls
*/

/**
	Doesn't do much, but sets the blocklen to AES to make it necessary to call
*/  
int32 psYarrowStart(psYarrow_t *ctx)
{
	if (ctx == NULL) {
		return PS_ARG_FAIL;
	}
   /* zero the memory used */
   memset(ctx->pool, 0, sizeof(ctx->pool));
   ctx->blocklen = AESBLOCKSIZE;

   return PS_SUCCESS;
}

/**
  Add entropy to the PRNG state
  @param in       The data to add
  @param inlen    Length of the data to add
  @param prng     PRNG state to update
*/  
int32 psYarrowAddEntropy(unsigned char *in, uint32 inlen, psYarrow_t *prng)
{
	psDigestContext_t md;
	int32 err;

	if (in == NULL || prng == NULL) {
		return PS_ARG_FAIL;
	}

	/* start the hash */
	psSha1Init(&md);

	/* hash the current pool */
	psSha1Update(&md, prng->pool, SHA1_HASH_SIZE);

	/* add the new entropy */
	psSha1Update(&md, in, inlen);

	/* store result */
	if ((err = psSha1Final(&md, prng->pool)) != SHA1_HASH_SIZE) {
		return err;
	}
	return PS_SUCCESS;
}


/**
  Make the PRNG ready to read from and to reseed when desired
  @param prng   The PRNG to seed
*/  
int32 psYarrowReseed(psYarrow_t *ctx) 
{
	const unsigned char *IV; 
	int32				keylen, ctr_mode, x, err; 
	
	IV = ctx->pool;
	keylen = AESBLOCKSIZE;   /* Can only use 16 bytes for the AES key */
	ctr_mode = CTR_COUNTER_LITTLE_ENDIAN; /* little endian counter */

	/* ctrlen == counter width */
	ctx->ctrlen   = (ctr_mode & 255) ? (ctr_mode & 255) : AESBLOCKSIZE;
	if (ctx->ctrlen > AESBLOCKSIZE) {
		return PS_ARG_FAIL;
	}

	if ((ctr_mode & 0x1000) == CTR_COUNTER_BIG_ENDIAN) {
		ctx->ctrlen = AESBLOCKSIZE - ctx->ctrlen;
	}

	/* setup cipher */
	if ((err = psAesInitKey(ctx->pool, keylen, &ctx->key)) != PS_SUCCESS) {
		return err;
	}

	/* copy ctr */
	ctx->padlen   = 0;
	ctx->mode     = ctr_mode & 0x1000;
	for (x = 0; x < ctx->blocklen; x++) {
		ctx->ctr[x] = IV[x];
	}

	/* increment the IV */
	if (ctx->mode == CTR_COUNTER_LITTLE_ENDIAN) {
		/* little-endian */
		for (x = 0; x < ctx->ctrlen; x++) {
			ctx->ctr[x] = (ctx->ctr[x] + (unsigned char)1) & (unsigned char)255;
			if (ctx->ctr[x] != (unsigned char)0) {
				break;
			}
		}
	} else {
		/* big-endian */
		for (x = ctx->blocklen-1; x >= ctx->ctrlen; x--) {
			ctx->ctr[x] = (ctx->ctr[x] + (unsigned char)1) & (unsigned char)255;
			if (ctx->ctr[x] != (unsigned char)0) {
				break;
			}
		}
	}

	psAesEncryptBlock(ctx->ctr, ctx->pad, &ctx->key);
	return PS_SUCCESS;
}


/**
  Read from the PRNG
  @param out      Destination
  @param outlen   Length of output
  @param ctx     The active PRNG to read from
  @return Number of octets read
*/  
uint32 psYarrowRead(unsigned char *out, uint32 outlen, psYarrow_t *ctx)
{
	unsigned char	*pt, *ct;
	int32			x;
	uint32			len;

	/* put out in predictable state first */
	memset(out, 0x0, outlen);
   
	len = outlen;
	pt = ct = out;
    
	/* is blocklen/padlen valid? */
	if (ctx->blocklen < 1 || ctx->blocklen > (int32)sizeof(ctx->ctr) ||
			ctx->padlen   < 0 || ctx->padlen   > (int32)sizeof(ctx->pad)) {
		return 0;
	}

	while (outlen) {
		/* is the pad empty? */
		if (ctx->padlen == ctx->blocklen) {
			/* increment counter */
			if (ctx->mode == CTR_COUNTER_LITTLE_ENDIAN) {
				/* little-endian */
				for (x = 0; x < ctx->ctrlen; x++) {
					ctx->ctr[x] =
						(ctx->ctr[x] + (unsigned char)1) & (unsigned char)255;
					if (ctx->ctr[x] != (unsigned char)0) {
						break;
					}
				}
			} else {
				/* big-endian */
				for (x = ctx->blocklen-1; x >= ctx->ctrlen; x--) {
					ctx->ctr[x] =
						(ctx->ctr[x] + (unsigned char)1) & (unsigned char)255;
					if (ctx->ctr[x] != (unsigned char)0) {
						break;
					}
				}
			}

			/* encrypt new pad and reset */
			psAesEncryptBlock(ctx->ctr, ctx->pad, &ctx->key);
			ctx->padlen = 0;
		}   
		*ct++ = *pt++ ^ ctx->pad[ctx->padlen++];
		--outlen;
	}
	return len;
}


/**
  Terminate the PRNG
*/  
int32 psYarrowDone(psYarrow_t *ctx)
{
   return PS_SUCCESS;
}


/**
  Export the PRNG state
  @param out       [out] Destination
  @param outlen    [in/out] Max size and resulting size of the state
  @param prng      The PRNG to export
  @return CRYPT_OK if successful
*/  
int32 psYarrowExport(unsigned char *out, uint32 *outlen, psYarrow_t *ctx)
{
	/* we'll write 64 bytes for s&g's */
	if (*outlen < 64) {
		*outlen = 64;
		return PS_LIMIT_FAIL;
	}

	if (psYarrowRead(out, 64, ctx) != 64) {
		*outlen = 0;
		return PS_FAILURE;
	}
	*outlen = 64;

	return PS_SUCCESS;
}
 
/**
  Import a PRNG state
  @param in       The PRNG state
  @param inlen    Size of the state
  @param prng     The PRNG to import
  @return CRYPT_OK if successful
*/  
int32 psYarrowImport(unsigned char *in, uint32 inlen, psYarrow_t *ctx)
{
	int32 err;
  
	if (inlen != 64) {
		return PS_ARG_FAIL;
	}

	if ((err = psYarrowStart(ctx)) != PS_SUCCESS) {
		return err;
	}
	if ((err = psYarrowAddEntropy(in, 64, ctx)) != PS_SUCCESS) {
		return err;
	}
	if ((err = psYarrowReseed(ctx)) != PS_SUCCESS) {
		return err;
	}
	return err;
}


#endif /* USE_YARROW */
