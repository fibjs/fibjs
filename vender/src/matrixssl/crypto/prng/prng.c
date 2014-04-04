/*
 *	prng.c
 *	Release $Name: MATRIXSSL-3-4-2-OPEN $
 *
 *	Psuedo random number generation
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

/*
	Priority order of PRNG algorithms and then default GetEntropy if none.
	Does an initial entropy source and reseeding
*/
int32 psInitPrng(psRandom_t *ctx)
{
#if defined(USE_FORTUNA) || defined(USE_YARROW)
	unsigned char	entropyBytes[RANDOM_ENTROPY_BYTES];
	int32			rc;
#endif
	
	ctx->bytecount = 0;

	
#if defined(USE_FORTUNA) || defined(USE_YARROW)
	if ((rc = psGetEntropy(entropyBytes, RANDOM_ENTROPY_BYTES)) < 0) {
		return rc;
	}
#endif

#ifdef USE_YARROW
	if ((rc = psYarrowStart(&ctx->yarrow)) < 0) {
		return rc;
	}
	if ((rc = psYarrowAddEntropy(entropyBytes, RANDOM_ENTROPY_BYTES,
			&ctx->yarrow)) < 0) {
		return rc;
	}
	if ((rc = psYarrowReseed(&ctx->yarrow)) < 0) {
		return rc;
	}
#endif
	return PS_SUCCESS;
}

/*
	Performs the read
*/
static int32 readRandomData(psRandom_t *ctx, unsigned char *bytes, uint32 size)
{
#if defined(USE_FORTUNA) || defined(USE_YARROW)
	unsigned char	entropyBytes[RANDOM_ENTROPY_BYTES];
	int32			rc;
#endif
/*	
	Return random data.  The defines above control how often to add
	entropy and reseed the key.
*/		
	ctx->bytecount += size;


	
#ifdef USE_YARROW
	if (ctx->bytecount >= RANDOM_BYTES_BEFORE_ENTROPY) {
		ctx->bytecount = 0;
		if ((rc = psGetEntropy(entropyBytes, RANDOM_ENTROPY_BYTES)) < 0) {
			return rc;
		}
		if ((rc = psYarrowAddEntropy(entropyBytes, RANDOM_ENTROPY_BYTES,
				&ctx->yarrow)) < 0) {
			return rc;
		}
		if ((rc = psYarrowReseed(&ctx->yarrow)) < 0) {
			return rc;
		}
	}
	return psYarrowRead(bytes, size, &ctx->yarrow);
#endif
/*
	If no PRNG algorithms defined, default to the low level GetEntropy function
	for all the randomness
*/
	return psGetEntropy(bytes, size);
}

/*
	Allow NULL context if caller is just doing a single read
*/
int32 psGetPrng(psRandom_t *ctx, unsigned char *bytes, uint32 size)
{
	psRandom_t		lctx;
	
	if (ctx == NULL) {
		psInitPrng(&lctx);
		return readRandomData(&lctx, bytes, size);
	} 
	return readRandomData(ctx, bytes, size);
}
