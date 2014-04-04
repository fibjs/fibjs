/*
 *	rc2.c
 *	Release $Name: MATRIXSSL-3-4-2-OPEN $
 *
 *	rc2 stream cipher implementation
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

#ifdef USE_RC2

/* 256-entry permutation table, probably derived somehow from pi */
static const unsigned char permute[256] = {
        217,120,249,196, 25,221,181,237, 40,233,253,121, 74,160,216,157,
        198,126, 55,131, 43,118, 83,142, 98, 76,100,136, 68,139,251,162,
         23,154, 89,245,135,179, 79, 19, 97, 69,109,141,  9,129,125, 50,
        189,143, 64,235,134,183,123, 11,240,149, 33, 34, 92,107, 78,130,
         84,214,101,147,206, 96,178, 28,115, 86,192, 20,167,140,241,220,
         18,117,202, 31, 59,190,228,209, 66, 61,212, 48,163, 60,182, 38,
        111,191, 14,218, 70,105,  7, 87, 39,242, 29,155,188,148, 67,  3,
        248, 17,199,246,144,239, 62,231,  6,195,213, 47,200,102, 30,215,
          8,232,234,222,128, 82,238,247,132,170,114,172, 53, 77,106, 42,
        150, 26,210,113, 90, 21, 73,116, 75,159,208, 94,  4, 24,164,236,
        194,224, 65,110, 15, 81,203,204, 36,145,175, 80,161,244,112, 57,
        153,124, 58,133, 35,184,180,122,252,  2, 54, 91, 37, 85,151, 49,
         45, 93,250,152,227,138,146,174,  5,223, 41, 16,103,108,186,201,
        211,  0,230,207,225,158,168, 44, 99, 22,  1, 63, 88,226,137,169,
         13, 56, 52, 27,171, 51,255,176,187, 72, 12, 95,185,177,205, 46,
        197,243,219, 71,229,165,156,119, 10,166, 32,104,254,127,193,173
};

int32 psRc2Init(psCipherContext_t *ctx, unsigned char *IV,
				  unsigned char *key, uint32 keylen)
{
	int32		x, err;

	if (IV == NULL || key == NULL || ctx == NULL) {
		psTraceCrypto("psRc2Init arg fail\n");
		return PS_ARG_FAIL;
	}
/*
	setup cipher
 */
	if ((err = psRc2InitKey(key, keylen, 0, &ctx->rc2.key)) != PS_SUCCESS) {
		return err;
	}
/*
	copy IV
 */
	ctx->rc2.blocklen = 8;
	for (x = 0; x < ctx->rc2.blocklen; x++) {
		ctx->rc2.IV[x] = IV[x];
	}
	return PS_SUCCESS;
}


int32 psRc2Encrypt(psCipherContext_t *ctx, unsigned char *pt,
					 unsigned char *ct, uint32 len)
{
	int32			x;
	uint32			i;
	unsigned char	tmp[MAXBLOCKSIZE];

	if (pt == NULL || ct == NULL || ctx == NULL || (len & 0x7) != 0) {
		psTraceCrypto("Bad parameters to psRc2Encrypt\n");
		return PS_ARG_FAIL;
	}

/*
	is blocklen valid?
 */
	if (ctx->rc2.blocklen < 0 || (ctx->rc2.blocklen > 
			(int32)sizeof(ctx->rc2.IV))) {
		psTraceCrypto("Bad blocklen in psRc2Encrypt\n");
		return PS_LIMIT_FAIL;
	}

	for (i = 0; i < len; i += ctx->rc2.blocklen) {
/*
		xor IV against plaintext
 */
		for (x = 0; x < ctx->rc2.blocklen; x++) {
			tmp[x] = pt[x] ^ ctx->rc2.IV[x];
		}
/*
		encrypt
 */
		psRc2EncryptBlock(tmp, ct, &ctx->rc2.key);

/*
		store IV [ciphertext] for a future block
 */
		for (x = 0; x < ctx->rc2.blocklen; x++) {
			ctx->rc2.IV[x] = ct[x];
		}
		ct += ctx->rc2.blocklen;
		pt += ctx->rc2.blocklen;
	}

	memset(tmp, 0x0, sizeof(tmp));
	return len;
}

int32 psRc2Decrypt(psCipherContext_t *ctx, unsigned char *ct,
					 unsigned char *pt, uint32 len)
{
	int32			x;
	uint32			i;
	unsigned char	tmp[MAXBLOCKSIZE], tmp2[MAXBLOCKSIZE];

	if (pt == NULL || ct == NULL || ctx == NULL || (len & 0x7) != 0) {
		psTraceCrypto("Bad parameters to psRc2Decrypt\n");
		return PS_ARG_FAIL;
	}

/*
	is blocklen valid?
 */
	if (ctx->rc2.blocklen < 0 || (ctx->rc2.blocklen > 
			(int32)sizeof(ctx->rc2.IV))) {
		psTraceCrypto("Bad blocklen in psRc2Decrypt\n");	
		return PS_LIMIT_FAIL;
	}
	for (i = 0; i < len; i += ctx->rc2.blocklen) {
/*
		decrypt the block from ct into tmp
 */
		psRc2DecryptBlock(ct, tmp, &ctx->rc2.key);
/*
		xor IV against the plaintext of the previous step
 */
		for (x = 0; x < ctx->rc2.blocklen; x++) { 
/*
			copy CT in case ct == pt
 */
			tmp2[x] = ct[x]; 
/*
			actually decrypt the byte
 */
			pt[x] =	tmp[x] ^ ctx->rc2.IV[x]; 
		}
/*
		replace IV with this current ciphertext
 */
		for (x = 0; x < ctx->rc2.blocklen; x++) {
			ctx->rc2.IV[x] = tmp2[x];
		}
		ct += ctx->rc2.blocklen;
		pt += ctx->rc2.blocklen;
	}
	memset(tmp, 0x0, sizeof(tmp));
	memset(tmp2, 0x0, sizeof(tmp2));

	return len;
}


 /**
    Initialize the LTC_RC2 block cipher
    @param key The symmetric key you wish to pass
    @param keylen The key length in bytes
    @param num_rounds The number of rounds desired (0 for default)
    @param skey The key in as scheduled by this function.
    @return CRYPT_OK if successful
 */
int32 psRc2InitKey(unsigned char *key, uint32 keylen, uint32 num_rounds,
					psRc2Key_t *ctx)
{
   unsigned *xkey = ctx->xkey;
   unsigned char tmp[128];
   unsigned T8, TM;
   int32	i, bits;

   //if (keylen < 8 || keylen > 128) {
   //   return PS_ARG_FAIL;
   //}

   if (num_rounds != 0 && num_rounds != 16) {
      return PS_ARG_FAIL;
   }

   for (i = 0; i < (int32)keylen; i++) {
       tmp[i] = key[i] & 255;
   }

    /* Phase 1: Expand input key to 128 bytes */
    if (keylen < 128) {
        for (i = keylen; i < 128; i++) {
            tmp[i] = permute[(tmp[i - 1] + tmp[i - keylen]) & 255];
        }
    }
    
    /* Phase 2 - reduce effective key size to "bits" */
    bits = keylen<<3;
    T8   = (unsigned)(bits+7)>>3;
    TM   = (255 >> (unsigned)(7 & -bits));
    tmp[128 - T8] = permute[tmp[128 - T8] & TM];
    for (i = 127 - T8; i >= 0; i--) {
        tmp[i] = permute[tmp[i + 1] ^ tmp[i + T8]];
    }

    /* Phase 3 - copy to xkey in little-endian order */
    for (i = 0; i < 64; i++) {
        xkey[i] =  (unsigned)tmp[2*i] + ((unsigned)tmp[2*i+1] << 8);
    }        
   
    return PS_SUCCESS;
}

/**********************************************************************\
* Encrypt an 8-byte block of plaintext using the given key.            *
\**********************************************************************/
/**
  Encrypts a block of text with LTC_RC2
  @param pt The input plaintext (8 bytes)
  @param ct The output ciphertext (8 bytes)
  @param skey The key as scheduled
  @return CRYPT_OK if successful
*/
int32 psRc2EncryptBlock(unsigned char *pt, unsigned char *ct, psRc2Key_t *ctx)
{
    unsigned *xkey;
    unsigned x76, x54, x32, x10, i;

	if ((pt == NULL) || (ct == NULL) || (ctx == NULL)) {
		return PS_ARG_FAIL;
	}

    xkey = ctx->xkey;

    x76 = ((unsigned)pt[7] << 8) + (unsigned)pt[6];
    x54 = ((unsigned)pt[5] << 8) + (unsigned)pt[4];
    x32 = ((unsigned)pt[3] << 8) + (unsigned)pt[2];
    x10 = ((unsigned)pt[1] << 8) + (unsigned)pt[0];

    for (i = 0; i < 16; i++) {
        x10 = (x10 + (x32 & ~x76) + (x54 & x76) + xkey[4*i+0]) & 0xFFFF;
        x10 = ((x10 << 1) | (x10 >> 15));

        x32 = (x32 + (x54 & ~x10) + (x76 & x10) + xkey[4*i+1]) & 0xFFFF;
        x32 = ((x32 << 2) | (x32 >> 14));

        x54 = (x54 + (x76 & ~x32) + (x10 & x32) + xkey[4*i+2]) & 0xFFFF;
        x54 = ((x54 << 3) | (x54 >> 13));

        x76 = (x76 + (x10 & ~x54) + (x32 & x54) + xkey[4*i+3]) & 0xFFFF;
        x76 = ((x76 << 5) | (x76 >> 11));

        if (i == 4 || i == 10) {
            x10 = (x10 + xkey[x76 & 63]) & 0xFFFF;
            x32 = (x32 + xkey[x10 & 63]) & 0xFFFF;
            x54 = (x54 + xkey[x32 & 63]) & 0xFFFF;
            x76 = (x76 + xkey[x54 & 63]) & 0xFFFF;
        }
    }

    ct[0] = (unsigned char)x10;
    ct[1] = (unsigned char)(x10 >> 8);
    ct[2] = (unsigned char)x32;
    ct[3] = (unsigned char)(x32 >> 8);
    ct[4] = (unsigned char)x54;
    ct[5] = (unsigned char)(x54 >> 8);
    ct[6] = (unsigned char)x76;
    ct[7] = (unsigned char)(x76 >> 8);
 
    return PS_SUCCESS;
}

/**********************************************************************\
* Decrypt an 8-byte block of ciphertext using the given key.           *
\**********************************************************************/
/**
  Decrypts a block of text with LTC_RC2
  @param ct The input ciphertext (8 bytes)
  @param pt The output plaintext (8 bytes)
  @param skey The key as scheduled 
  @return CRYPT_OK if successful
*/
int32 psRc2DecryptBlock(unsigned char *ct, unsigned char *pt, psRc2Key_t *ctx)
{
    unsigned x76, x54, x32, x10;
    unsigned *xkey;
    int i;

	if ((pt == NULL) || (ct == NULL) || (ctx == NULL)) {
		return PS_ARG_FAIL;
	}

    xkey = ctx->xkey;

    x76 = ((unsigned)ct[7] << 8) + (unsigned)ct[6];
    x54 = ((unsigned)ct[5] << 8) + (unsigned)ct[4];
    x32 = ((unsigned)ct[3] << 8) + (unsigned)ct[2];
    x10 = ((unsigned)ct[1] << 8) + (unsigned)ct[0];

    for (i = 15; i >= 0; i--) {
        if (i == 4 || i == 10) {
            x76 = (x76 - xkey[x54 & 63]) & 0xFFFF;
            x54 = (x54 - xkey[x32 & 63]) & 0xFFFF;
            x32 = (x32 - xkey[x10 & 63]) & 0xFFFF;
            x10 = (x10 - xkey[x76 & 63]) & 0xFFFF;
        }

        x76 = ((x76 << 11) | (x76 >> 5));
        x76 = (x76 - ((x10 & ~x54) + (x32 & x54) + xkey[4*i+3])) & 0xFFFF;

        x54 = ((x54 << 13) | (x54 >> 3));
        x54 = (x54 - ((x76 & ~x32) + (x10 & x32) + xkey[4*i+2])) & 0xFFFF;

        x32 = ((x32 << 14) | (x32 >> 2));
        x32 = (x32 - ((x54 & ~x10) + (x76 & x10) + xkey[4*i+1])) & 0xFFFF;

        x10 = ((x10 << 15) | (x10 >> 1));
        x10 = (x10 - ((x32 & ~x76) + (x54 & x76) + xkey[4*i+0])) & 0xFFFF;
    }

    pt[0] = (unsigned char)x10;
    pt[1] = (unsigned char)(x10 >> 8);
    pt[2] = (unsigned char)x32;
    pt[3] = (unsigned char)(x32 >> 8);
    pt[4] = (unsigned char)x54;
    pt[5] = (unsigned char)(x54 >> 8);
    pt[6] = (unsigned char)x76;
    pt[7] = (unsigned char)(x76 >> 8);

    return PS_SUCCESS;
}

#endif /* USE_RC2 */

