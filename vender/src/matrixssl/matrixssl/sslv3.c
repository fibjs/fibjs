/*
 *	sslv3.c
 *	Release $Name: MATRIXSSL-3-3-1-OPEN $
 *
 *	SSLv3.0 specific code per http://wp.netscape.com/eng/ssl3.
 *	Primarily dealing with secret generation, message authentication codes
 *	and handshake hashing.
 */
/*
 *	Copyright (c) AuthenTec, Inc. 2011-2012
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
 *	commercial license for this software may be purchased from AuthenTec at
 *	http://www.authentec.com/Products/EmbeddedSecurity/SecurityToolkits.aspx
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

#include "matrixsslApi.h"

#ifndef DISABLE_SSLV3
/******************************************************************************/
/*
	Constants used for key generation
*/
static const unsigned char SENDER_CLIENT[5] = "CLNT";	/* 0x434C4E54 */
static const unsigned char SENDER_SERVER[5] = "SRVR";	/* 0x53525652 */

static const unsigned char pad1[48]={
	0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 
	0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 
	0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 
	0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 
	0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 
	0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36 
};

static const unsigned char pad2[48]={
	0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 
	0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 
	0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 
	0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 
	0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 
	0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c
};

static const unsigned char *salt[10]={
	(const unsigned char *)"A",
	(const unsigned char *)"BB",
	(const unsigned char *)"CCC",
	(const unsigned char *)"DDDD",
	(const unsigned char *)"EEEEE",
	(const unsigned char *)"FFFFFF",
	(const unsigned char *)"GGGGGGG",
	(const unsigned char *)"HHHHHHHH",
	(const unsigned char *)"IIIIIIIII",
	(const unsigned char *)"JJJJJJJJJJ"
};

/******************************************************************************/

static int32 createKeyBlock(ssl_t *ssl, unsigned char *clientRandom,
						  unsigned char *serverRandom, 
						  unsigned char *masterSecret, uint32 secretLen);

/******************************************************************************/
/*
 *	Generates all key material.
 */
int32 sslDeriveKeys(ssl_t *ssl)
{
	psDigestContext_t	md5Ctx, sha1Ctx;
	unsigned char		buf[MD5_HASH_SIZE + SHA1_HASH_SIZE];
	unsigned char		*tmp;
	uint32				i;

/*
	If this session is resumed, we want to reuse the master secret to 
	regenerate the key block with the new random values.
*/
	if (ssl->flags & SSL_FLAGS_RESUMED) {
		goto skipPremaster;
	}
/*
	master_secret =
		MD5(pre_master_secret + SHA('A' + pre_master_secret +
			ClientHello.random + ServerHello.random)) +
		MD5(pre_master_secret + SHA('BB' + pre_master_secret +
			ClientHello.random + ServerHello.random)) +
		MD5(pre_master_secret + SHA('CCC' + pre_master_secret +
			ClientHello.random + ServerHello.random));
*/
	tmp = ssl->sec.masterSecret;
	for (i = 0; i < 3; i++) {
		psSha1Init(&sha1Ctx);
		psSha1Update(&sha1Ctx, salt[i], i + 1);
		psSha1Update(&sha1Ctx, ssl->sec.premaster, ssl->sec.premasterSize);
		psSha1Update(&sha1Ctx, ssl->sec.clientRandom, SSL_HS_RANDOM_SIZE);
		psSha1Update(&sha1Ctx, ssl->sec.serverRandom, SSL_HS_RANDOM_SIZE);
		psSha1Final(&sha1Ctx, buf);
		
		psMd5Init(&md5Ctx);
		psMd5Update(&md5Ctx, ssl->sec.premaster, ssl->sec.premasterSize);
		psMd5Update(&md5Ctx, buf, SHA1_HASH_SIZE);
		psMd5Final(&md5Ctx, tmp);
		tmp += MD5_HASH_SIZE;
	}
	memset(buf, 0x0, MD5_HASH_SIZE + SHA1_HASH_SIZE);
/*
	premaster is now allocated for DH reasons.  Can free here
*/
	psFree(ssl->sec.premaster);
	ssl->sec.premaster = NULL;
	ssl->sec.premasterSize = 0;

skipPremaster:
	if (createKeyBlock(ssl, ssl->sec.clientRandom, ssl->sec.serverRandom, 
			ssl->sec.masterSecret, SSL_HS_MASTER_SIZE) < 0) {
		psTraceInfo("Unable to create key block\n");
		return PS_FAILURE;
	}
	
	return SSL_HS_MASTER_SIZE;
}

/******************************************************************************/
/*
	Generate the key block as follows.  '+' indicates concatination.  
	key_block =
		MD5(master_secret + SHA(`A' + master_secret +
			ServerHello.random + ClientHello.random)) +
		MD5(master_secret + SHA(`BB' + master_secret +
			ServerHello.random + ClientHello.random)) +
		MD5(master_secret + SHA(`CCC' + master_secret +
			ServerHello.random + ClientHello.random)) + 
		[...];
*/
static int32 createKeyBlock(ssl_t *ssl, unsigned char *clientRandom,
						  unsigned char *serverRandom,
						  unsigned char *masterSecret, uint32 secretLen)
{
	psDigestContext_t	md5Ctx, sha1Ctx;
	unsigned char		buf[MD5_HASH_SIZE + SHA1_HASH_SIZE];
	unsigned char		*tmp;
	int32				ret = 0;
	uint32				i, keyIter, reqKeyLen;

/*
	We must generate enough key material to fill the various keys
*/
	reqKeyLen = 2 * ssl->cipher->macSize + 
				2 * ssl->cipher->keySize + 
				2 * ssl->cipher->ivSize;
/*
	Find the right number of iterations to make the requested length key block
*/
	keyIter = 1;
	while (MD5_HASH_SIZE * keyIter < reqKeyLen) {
		keyIter++;
	}
	if (keyIter > sizeof(salt)/sizeof(char*)) {
		psTraceIntInfo("Error: Not enough salt for key length %d\n", reqKeyLen);
		return PS_FAILURE;
	}

	tmp = ssl->sec.keyBlock;
	for (i = 0; i < keyIter; i++) {
		psSha1Init(&sha1Ctx);
		psSha1Update(&sha1Ctx, salt[i], i + 1);
		psSha1Update(&sha1Ctx, masterSecret, secretLen);
		psSha1Update(&sha1Ctx, serverRandom, SSL_HS_RANDOM_SIZE);
		psSha1Update(&sha1Ctx, clientRandom, SSL_HS_RANDOM_SIZE);
		psSha1Final(&sha1Ctx, buf);
		
		psMd5Init(&md5Ctx);
		psMd5Update(&md5Ctx, masterSecret, secretLen);
		psMd5Update(&md5Ctx, buf, SHA1_HASH_SIZE);
		psMd5Final(&md5Ctx, tmp);
		tmp += MD5_HASH_SIZE;
		ret += MD5_HASH_SIZE;
	}
	memset(buf, 0x0, MD5_HASH_SIZE + SHA1_HASH_SIZE);
/*
	Client and server use different read/write values, with the Client 
	write value being the server read value.
*/
	if (ssl->flags & SSL_FLAGS_SERVER) {
		ssl->sec.rMACptr = ssl->sec.keyBlock;
		ssl->sec.wMACptr = ssl->sec.rMACptr + ssl->cipher->macSize;
		ssl->sec.rKeyptr = ssl->sec.wMACptr + ssl->cipher->macSize;
		ssl->sec.wKeyptr = ssl->sec.rKeyptr + ssl->cipher->keySize;
		ssl->sec.rIVptr = ssl->sec.wKeyptr + ssl->cipher->keySize;
		ssl->sec.wIVptr = ssl->sec.rIVptr + ssl->cipher->ivSize;
	} else {
		ssl->sec.wMACptr = ssl->sec.keyBlock;
		ssl->sec.rMACptr = ssl->sec.wMACptr + ssl->cipher->macSize;
		ssl->sec.wKeyptr = ssl->sec.rMACptr + ssl->cipher->macSize;
		ssl->sec.rKeyptr = ssl->sec.wKeyptr + ssl->cipher->keySize;
		ssl->sec.wIVptr = ssl->sec.rKeyptr + ssl->cipher->keySize;
		ssl->sec.rIVptr = ssl->sec.wIVptr + ssl->cipher->ivSize;
	}

	return ret;
}

/******************************************************************************/
/*
	Combine the running hash of the handshake mesages with some constants
	and mix them up a bit more.  Output the result to the given buffer.
	This data will be part of the Finished handshake message.
*/
int32 sslGenerateFinishedHash(psDigestContext_t *md5, psDigestContext_t *sha1, 
								unsigned char *masterSecret,
								unsigned char *out, int32 sender)
{
	psDigestContext_t	omd5,osha1;
	unsigned char		ihash[SHA1_HASH_SIZE];

/*
	md5Hash = MD5(master_secret + pad2 + 
		MD5(handshake_messages + sender + master_secret + pad1));
*/
	if (sender >= 0) {
		psMd5Update(md5,
			(sender & SSL_FLAGS_SERVER) ? SENDER_SERVER : SENDER_CLIENT, 4);
	}
	psMd5Update(md5, masterSecret, SSL_HS_MASTER_SIZE);
	psMd5Update(md5, pad1, sizeof(pad1));
	psMd5Final(md5, ihash);

	psMd5Init(&omd5);
	psMd5Update(&omd5, masterSecret, SSL_HS_MASTER_SIZE);
	psMd5Update(&omd5, pad2, sizeof(pad2));
	psMd5Update(&omd5, ihash, MD5_HASH_SIZE);
	psMd5Final(&omd5, out);
/*
	The SHA1 hash is generated in the same way, except only 40 bytes
	of pad1 and pad2 are used.
	sha1Hash = SHA1(master_secret + pad2 + 
		SHA1(handshake_messages + sender + master_secret + pad1));
*/
	if (sender >= 0) {
		psSha1Update(sha1, 
			(sender & SSL_FLAGS_SERVER) ? SENDER_SERVER : SENDER_CLIENT, 4);
	}
	psSha1Update(sha1, masterSecret, SSL_HS_MASTER_SIZE);
	psSha1Update(sha1, pad1, 40);
	psSha1Final(sha1, ihash);

	psSha1Init(&osha1);
	psSha1Update(&osha1, masterSecret, SSL_HS_MASTER_SIZE);
	psSha1Update(&osha1, pad2, 40);
	psSha1Update(&osha1, ihash, SHA1_HASH_SIZE);
	psSha1Final(&osha1, out + MD5_HASH_SIZE);

	return MD5_HASH_SIZE + SHA1_HASH_SIZE;
}

#ifdef USE_SHA_MAC
/******************************************************************************/
/*
	SSLv3 uses a method similar to HMAC to generate the SHA1 message MAC.
	For SHA1, 40 bytes of the pad are used.

	SHA1(MAC_write_secret + pad2 + 
		SHA1(MAC_write_secret + pad1 + seq_num + length + content));
*/
int32 ssl3HMACSha1(unsigned char *key, unsigned char *seq, 
						unsigned char type, unsigned char *data, uint32 len,
						unsigned char *mac)
{
	psDigestContext_t	sha1;
	unsigned char		ihash[SHA1_HASH_SIZE];
	int32				i;

	psSha1Init(&sha1);
	psSha1Update(&sha1, key, SHA1_HASH_SIZE);
	psSha1Update(&sha1, pad1, 40);
	psSha1Update(&sha1, seq, 8);
	ihash[0] = type;
	ihash[1] = (len & 0xFF00) >> 8;
	ihash[2] = len & 0xFF;
	psSha1Update(&sha1, ihash, 3);
	psSha1Update(&sha1, data, len);
	psSha1Final(&sha1, ihash);

	psSha1Init(&sha1);
	psSha1Update(&sha1, key, SHA1_HASH_SIZE);
	psSha1Update(&sha1, pad2, 40);
	psSha1Update(&sha1, ihash, SHA1_HASH_SIZE);
	psSha1Final(&sha1, mac);

/*
	Increment sequence number
*/
	for (i = 7; i >= 0; i--) {
		seq[i]++;
		if (seq[i] != 0) {
			break; 
		}
	}
	return SHA1_HASH_SIZE;
}
#endif /* USE_SHA_MAC */

#ifdef USE_MD5_MAC
/******************************************************************************/
/*
	SSLv3 uses a method similar to HMAC to generate the MD5 message MAC.
	For MD5, 48 bytes of the pad are used.

	MD5(MAC_write_secret + pad2 + 
		MD5(MAC_write_secret + pad1 + seq_num + length + content));
*/
int32 ssl3HMACMd5(unsigned char *key, unsigned char *seq, 
						unsigned char type, unsigned char *data, uint32 len,
						unsigned char *mac)
{
	psDigestContext_t	md5;
	unsigned char		ihash[MD5_HASH_SIZE];
	int32				i;

	psMd5Init(&md5);
	psMd5Update(&md5, key, MD5_HASH_SIZE);
	psMd5Update(&md5, pad1, 48);
	psMd5Update(&md5, seq, 8);
	ihash[0] = type;
	ihash[1] = (len & 0xFF00) >> 8;
	ihash[2] = len & 0xFF;
	psMd5Update(&md5, ihash, 3);
	psMd5Update(&md5, data, len);
	psMd5Final(&md5, ihash);

	psMd5Init(&md5);
	psMd5Update(&md5, key, MD5_HASH_SIZE);
	psMd5Update(&md5, pad2, 48);
	psMd5Update(&md5, ihash, MD5_HASH_SIZE);
	psMd5Final(&md5, mac);

/*
	Increment sequence number
*/
	for (i = 7; i >= 0; i--) {
		seq[i]++;
		if (seq[i] != 0) {
			break; 
		}
	}
	return MD5_HASH_SIZE;
}

#endif /* USE_MD5_MAC */

#endif /* DISABLE_SSLV3 */
/******************************************************************************/


