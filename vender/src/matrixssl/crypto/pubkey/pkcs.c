/*
 *	pkcs.c
 *	Release $Name: MATRIXSSL-3-4-2-OPEN $
 *
 *	Collection of RSA PKCS standards 
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

/******************************************************************************/
/*
    Pad a value to be encrypted by RSA, according to PKCS#1 v1.5
    http://www.rsasecurity.com/rsalabs/pkcs/pkcs-1/
    When encrypting a value with RSA, the value is first padded to be
    equal to the public key size using the following method:
        00 <id> <data> 00 <value to be encrypted>
    - id denotes a public or private key operation
    - if id is private, data is however many non-zero bytes it takes to pad the
        value to the key length (randomLen = keyLen - 3 - valueLen).
    - if id is public, data is FF for the same length as described above
    - There must be at least 8 bytes of data.
*/
int32 pkcs1Pad(unsigned char *in, uint32 inlen, unsigned char *out,
                       uint32 outlen, int32 cryptType)
{
    unsigned char   *c;
    int32           randomLen;

    randomLen = outlen - 3 - inlen;
    if (randomLen < 8) {
		psTraceCrypto("pkcs1Pad failure\n");
        return PS_LIMIT_FAIL;
    }
    c = out;
    *c = 0x00;
    c++;
    *c = (unsigned char)cryptType;
    c++;
    if (cryptType == PUBKEY_TYPE) {
        while (randomLen-- > 0) {
            *c++ = 0xFF;
        }
    } else {
        if (psGetPrng(NULL, c, (uint32)randomLen) < 0) {
            return PS_PLATFORM_FAIL;
        }
/*
        SECURITY:  Read through the random data and change all 0x0 to 0x01.
        This is per spec that no random bytes should be 0
*/
        while (randomLen-- > 0) {
            if (*c == 0x0) {
                *c = 0x01;
            }
            c++;
        }
    }
    *c = 0x00;
    c++;
    memcpy(c, in, inlen);

    return outlen;
}

/******************************************************************************/
/*
    Unpad a value decrypted by RSA, according to PKCS#1 v1.5
    http://www.rsasecurity.com/rsalabs/pkcs/pkcs-1/
   
    When decrypted, the data will look like the pad, including the inital
    byte (00).  Form:
        00 <decryptType> <random data (min 8 bytes)> 00 <value to be encrypted>

    We don't worry about v2 rollback issues because we don't support v2
*/
int32 pkcs1Unpad(unsigned char *in, uint32 inlen, unsigned char *out,
                        uint32 outlen, int32 decryptType)
{
    unsigned char   *c, *end;

    if (inlen < outlen + 10) {
		psTraceCrypto("pkcs1Unpad failure\n");
        return PS_ARG_FAIL;
    }
    c = in;
    end = in + inlen;
/*
    Verify the first byte (block type) is correct.
*/
    if (*c++ != 0x00 || *c != decryptType) {
		psTraceCrypto("pkcs1Unpad parse failure\n");
        return PS_FAILURE;
    }
    c++;
/*
    Skip over the random, non-zero bytes used as padding
*/
    while (c < end && *c != 0x0) {
        if (decryptType == PUBKEY_TYPE) {
            if (*c != 0xFF) {
				psTraceCrypto("pkcs1Unpad pubkey parse failure\n");
                return PS_FAILURE;
            }
        }
        c++;
    }
    c++;
/*
    The length of the remaining data should be equal to what was expected
    Combined with the initial length check, there must be >= 8 bytes of pad
    ftp://ftp.rsa.com/pub/pdfs/bulletn7.pdf
*/
    if ((uint32)(end - c) != outlen) {
		psTraceCrypto("pkcs1Unpad verification failure\n");
        return PS_LIMIT_FAIL;
    }
/*
    Copy the value bytes to the out buffer
*/
    while (c < end) {
        *out = *c;
        out++; c++;
    }
    return outlen;
}

#ifdef USE_PRIVATE_KEY_PARSING
/******************************************************************************/
/*
	Parse a a private key structure in DER formatted ASN.1
	Per ftp://ftp.rsasecurity.com/pub/pkcs/pkcs-1/pkcs-1v2-1.pdf
	RSAPrivateKey ::= SEQUENCE {
		version Version,
		modulus INTEGER, -- n
		publicExponent INTEGER, -- e
		privateExponent INTEGER, -- d
		prime1 INTEGER, -- p
		prime2 INTEGER, -- q
		exponent1 INTEGER, -- d mod (p-1)
		exponent2 INTEGER, -- d mod (q-1)
		coefficient INTEGER, -- (inverse of q) mod p
		otherPrimeInfos OtherPrimeInfos OPTIONAL
	}
	Version ::= INTEGER { two-prime(0), multi(1) }
	  (CONSTRAINED BY {-- version must be multi if otherPrimeInfos present --})

	Which should look something like this in hex (pipe character 
	is used as a delimiter):
	ftp://ftp.rsa.com/pub/pkcs/ascii/layman.asc
	30	Tag in binary: 00|1|10000 -> UNIVERSAL | CONSTRUCTED | SEQUENCE (16)
	82	Length in binary: 1 | 0000010 -> LONG LENGTH | LENGTH BYTES (2)
	04 A4	Length Bytes (1188)
	02	Tag in binary: 00|0|00010 -> UNIVERSAL | PRIMITIVE | INTEGER (2)
	01	Length in binary: 0|0000001 -> SHORT LENGTH | LENGTH (1)
	00	INTEGER value (0) - RSAPrivateKey.version
	02	Tag in binary: 00|0|00010 -> UNIVERSAL | PRIMITIVE | INTEGER (2)
	82	Length in binary: 1 | 0000010 -> LONG LENGTH | LENGTH BYTES (2)
	01 01	Length Bytes (257)
	[]	257 Bytes of data - RSAPrivateKey.modulus (2048 bit key)
	02	Tag in binary: 00|0|00010 -> UNIVERSAL | PRIMITIVE | INTEGER (2)
	03	Length in binary: 0|0000011 -> SHORT LENGTH | LENGTH (3)
	01 00 01	INTEGER value (65537) - RSAPrivateKey.publicExponent
	...

	OtherPrimeInfos is not supported in this routine, and an error will be
	returned if they are present
*/
int32 pkcs1ParsePrivBin(psPool_t *pool, unsigned char *p,
				uint32 size, psPubKey_t **pubkey)
{
	unsigned char	*end, *seq;
	psRsaKey_t		*key;
	int32			version;
	uint32			seqlen;

	*pubkey = psNewPubKey(pool);
	if (*pubkey == NULL) {
		psError("Memory allocation error in pkcs1ParsePrivBin\n");
		return PS_MEM_FAIL;
	}
	(*pubkey)->type = PS_RSA; /* PKCS1 is always RSA */
	key = &((*pubkey)->key->rsa);
	key->optimized = 0;
	end = p + size;
	if (getAsnSequence(&p, size, &seqlen) < 0) {
		psFreePubKey(*pubkey);
		*pubkey = NULL;
		return PS_PARSE_FAIL;
	}
	seq = p;
	if (getAsnInteger(&p, (uint32)(end - p), &version) < 0 || version != 0 ||
		getAsnBig(pool, &p, (uint32)(end - p), &(key->N)) < 0 ||
		getAsnBig(pool, &p, (uint32)(end - p), &(key->e)) < 0 ||
		getAsnBig(pool, &p, (uint32)(end - p), &(key->d)) < 0 ||
		getAsnBig(pool, &p, (uint32)(end - p), &(key->p)) < 0 ||
		getAsnBig(pool, &p, (uint32)(end - p), &(key->q)) < 0 ||
		getAsnBig(pool, &p, (uint32)(end - p), &(key->dP)) < 0 ||
		getAsnBig(pool, &p, (uint32)(end - p), &(key->dQ)) < 0 ||
		getAsnBig(pool, &p, (uint32)(end - p), &(key->qP)) < 0 ||
			(uint32)(p - seq) != seqlen) {
		psTraceCrypto("ASN RSA private key extract parse error\n");
		psFreePubKey(*pubkey);
		*pubkey = NULL;
		return PS_PARSE_FAIL;
	}


/*
	 If we made it here, the key is ready for optimized decryption
	 Set the key length of the key
 */
	key->optimized = 1;
	key->size = pstm_unsigned_bin_size(&key->N);	
	(*pubkey)->keysize = key->size;
	return PS_SUCCESS;
}



#ifdef USE_PKCS8
/******************************************************************************/
/*
	Parse PKCS#8 format keys (from DER formatted binary)
 
	'key' is dynamically allocated and must be freed with psFreePubKey() if 
		no error is returned from this API
 
	Unencrypted private keys are supported if 'pass' is NULL
	Encrypted private keys are supported if 'pass' is non-null for the 
		des-EDE3-CBC algorithm only (3DES). Other PKCS#5 symmetric algorithms
		are not supported.
*/
int32 pkcs8ParsePrivBin(psPool_t *pool, unsigned char *p, int32 size,
			char *pass, psPubKey_t **key)
{
	unsigned char	*end;
	int32			version, oi, plen;
	uint32			seqlen, len;
	psPubKey_t		*rsakey;
#ifdef USE_PKCS5
	unsigned char	desKeyBin[24];
	psCipherContext_t	ctx;
	char			iv[8], salt[8];
	int32			icount;
#endif /* USE_PKCS5 */	
	
	*key = NULL;
	end = p + size;

	if (pass) {
#ifdef USE_PKCS5
/*		An encrypted PKCS#8 key has quite a bit more information we must parse 
		We actually parse a good bit of PKCS#5 structures here
*/
		if (getAsnSequence(&p, (int32)(end - p), &seqlen) < 0) {
			return PS_FAILURE;
		}
		if (getAsnAlgorithmIdentifier(&p, (int32)(end - p), &oi, 0, &plen) < 0){
			psTraceCrypto("Couldn't parse PKCS#8 algorithm identifier\n");
			return PS_FAILURE;
		}
		if (oi != OID_PKCS_PBES2 || plen != 53) {
			psTraceCrypto("Only supporting PKCS#8 id-PBES2 OID\n");
			return PS_FAILURE;		
		}
		if (getAsnSequence(&p, (int32)(end - p), &seqlen) < 0) {
			return PS_FAILURE;
		}
		if (getAsnAlgorithmIdentifier(&p, (int32)(end - p), &oi, 0, &plen) < 0){
			psTraceCrypto("Couldn't parse PKCS#8 keyDerivationFunc\n");
			return PS_FAILURE;
		}
		if (oi != OID_PKCS_PBKDF2 || plen != 16) {
			psTraceCrypto("Only support PKCS#8 id-PBKDF2 OID\n");
			return PS_FAILURE;
		}
		if (getAsnSequence(&p, (int32)(end - p), &seqlen) < 0) {
			return PS_FAILURE;
		}
		if ((*p++ != ASN_OCTET_STRING) ||
				getAsnLength(&p, (int32)(end - p), &len) < 0 || 
				len != 8) {
			psTraceCrypto("Couldn't parse PKCS#8 param salt\n");
			return PS_FAILURE;
		}
		/* Get the PBKDF2 Salt */
		memcpy(salt, p, 8); p += 8;
		/* Get the PBKDF2 Iteration count (rounds) */
		if (getAsnInteger(&p, (int32)(end - p), &icount) < 0) {
			psTraceCrypto("Couldn't parse PKCS#8 param iterationCount\n");
			return PS_FAILURE;
		}
		/* Get encryptionScheme */
		if (getAsnAlgorithmIdentifier(&p, (int32)(end - p), &oi, 0, &plen)
				< 0){
			psTraceCrypto("Couldn't parse PKCS#8 encryptionScheme\n");
			return PS_FAILURE;
		}
		if (oi != OID_DES_EDE3_CBC || plen != 10) {
			psTraceCrypto("Only support des-EDE3-CBC OID\n");
			return PS_FAILURE;
		}
		if ((*p++ != ASN_OCTET_STRING) ||
				getAsnLength(&p, (int32)(end - p), &len) < 0 || 
				len != 8) {
			psTraceCrypto("Couldn't parse PKCS#8 param CBC IV\n");
			return PS_FAILURE;
		}
		/* Get the 3DES IV */
		memcpy(iv, p, 8); p += 8;
		/* Now p points to the 3DES encrypted RSA key */
		if ((*p++ != ASN_OCTET_STRING) ||
				getAsnLength(&p, (int32)(end - p), &len) < 0 ||
				(uint32)(end - p) < len) {
			psTraceCrypto("PKCS#8 decryption error\n");
			return PS_FAILURE;
		}
		/* Derive the 3DES key and decrypt the RSA key*/
		pkcs5pbkdf2((unsigned char*)pass, (int32)strlen(pass), 
			(unsigned char*)salt, 8, icount, (unsigned char*)desKeyBin, 24);
		psDes3Init(&ctx, (unsigned char*)iv, desKeyBin, 24);
		if ((int32)len != psDes3Decrypt(&ctx, p, p, len)) {
			psTraceCrypto("PKCS#8 decryption error\n");
			return PS_FAILURE;
		}
#else /* !USE_PKCS5 */
/*
 *		The private key is encrypted, but PKCS5 support has been turned off
 */
		psTraceCrypto("USE_PKCS5 must be enabled for key file password\n");
		return PS_UNSUPPORTED_FAIL;  
#endif /* USE_PKCS5 */		
	}
	
	/* PrivateKeyInfo per PKCS#8 Section 6. */
	if (getAsnSequence(&p, (int32)(end - p), &seqlen) < 0) {
		psTraceCrypto("Initial PrivateKeyInfo parse failure\n");
		if (pass) {
			psTraceCrypto("Is it possible the password is incorrect?\n");
		}
		return PS_FAILURE;
	}
	/* Version */
	if (getAsnInteger(&p, (int32)(end - p), &version) < 0 || version != 0) {
		psTraceCrypto("Couldn't parse PKCS#8 algorithm identifier\n");
		return PS_FAILURE;
	}
	/* privateKeyAlgorithmIdentifier */
	if (getAsnAlgorithmIdentifier(&p, (int32)(end - p), &oi, 1, &plen) < 0) {
		psTraceCrypto("Couldn't parse PKCS#8 algorithm identifier\n");
		return PS_FAILURE;
	}
	if (oi != OID_RSA_KEY_ALG || plen != 0) {
		psTraceCrypto("Unsupported public key type in PKCS#8 parse\n");
		return PS_UNSUPPORTED_FAIL;
	}
	/* PrivateKey Octet Stream */
	if ((*p++ != ASN_OCTET_STRING) ||
		getAsnLength(&p, (int32)(end - p), &len) < 0 ||
			(uint32)(end - p) < len) {
		psTraceCrypto("getAsnLength parse error in x509ConfirmSignature\n");
		return PS_FAILURE;
	}
	if (oi == OID_RSA_KEY_ALG) {
		/* Create the actual key here from the octet string */
		if (pkcs1ParsePrivBin(pool, p, len, &rsakey) < 0) {
			return PS_FAILURE;
		}
	}
	p += len;
	
	plen = (int32)(end - p);
	if (plen > 0) {
		/* attributes [0] Attributes OPTIONAL */
		if (*p == (ASN_CONTEXT_SPECIFIC | ASN_CONSTRUCTED)) {
			/* Yeah, have some... ignoring */
			p++;
			if (getAsnLength(&p, (int32)(end - p), &len) < 0) {
				psTraceCrypto("Error parsing pkcs#8 PrivateKey attributes\n");
				return PS_FAILURE;
			}
			p += len;
			plen = (int32)(end - p);
		}
		/*  Any remaining bytes should be non ASN.1 bytes that correspond
			to the 3DES block padding */
		while (p < end) {
			if (*p++ != (char)plen) {
				goto PKCS8_FAIL;	
			}
		}
	} else if (plen < 0) {
		goto PKCS8_FAIL;	
	}
	
	*key = rsakey;
		
	return PS_SUCCESS;
	
PKCS8_FAIL:
	psFreePubKey(rsakey);
	psTraceCrypto("Did not parse key in PKCS#8 parse\n");
	return PS_FAILURE;
}

#ifdef MATRIX_USE_FILE_SYSTEM
#ifdef USE_PKCS12
/******************************************************************************/
/*
	A PKCS #7 ContentInfo, whose contentType is signedData in public-key
	integrity mode and data in password integrity mode.
	
	Returns integrity mode or < 0 on failure
*/
#define PASSWORD_INTEGRITY	1
#define PUBKEY_INTEGRITY	2
static int32 psParseIntegrityMode(unsigned char **buf, int32 totLen)
{
	unsigned char	*p, *end;
	uint32			totcontentlen, len;
	int32			rc, oi, oiLen;
	
	p = *buf;
	end = p + totLen;
		
	if ((rc = getAsnAlgorithmIdentifier(&p, totLen, &oi, 0, &oiLen)) < 0) {
		psTraceCrypto("Initial integrity parse error\n");
		return rc;
	}

	if (oi == PKCS7_DATA) {
		/* Data ::= OCTET STRING */
		if (*p++ != (ASN_CONTEXT_SPECIFIC | ASN_CONSTRUCTED)) {
			return PS_PARSE_FAIL;
		}
		if ((rc = getAsnLength(&p, (int32)(end - p), &len)) < 0) {
			return PS_PARSE_FAIL;
		}
		if ((*p++ != ASN_OCTET_STRING) ||
				getAsnLength(&p, (int32)(end - p), &totcontentlen) < 0) {
			psTraceCrypto("Couldn't parse data from ContentInfo\n");
			return PS_FAILURE;
		}
		rc = PASSWORD_INTEGRITY;
	} else if (oi == PKCS7_SIGNED_DATA) {
		psTraceCrypto("SignedData integrity mode not supported\n");
		return PS_UNSUPPORTED_FAIL;
		/* rc = PUBKEY_INTEGRITY; */
	} else {
		psTraceCrypto("Unknown integrity mode\n");
		return PS_UNSUPPORTED_FAIL;
	}
	
	*buf = p;
	return rc;
}

/******************************************************************************/
/*
	Generate a key given a password, salt and iteration value.
	
	B.2 General method from PKCS#12
	
	Assumptions:  hash is SHA-1, password is < 64 bytes
*/
static int32 pkcs12pbe(psPool_t *pool, unsigned char *password, uint32 passLen,
				unsigned char *salt, int saltLen, int32 iter, int32 id,
				unsigned char **out, uint32 *outlen)
{
	psDigestContext_t	ctx;
	pstm_int			bigb, bigone, bigtmp;
	unsigned char		diversifier[64], saltpass[128], hash[SHA1_HASH_SIZE];
	unsigned char		B[65];
	unsigned char		*p, *front;
	int32				i, j, copy, count, cpyLen, binsize;
	
	*out = NULL;
	memset(diversifier, id, 64);
	
	for (i = 0; i < 64;) {
		if ((64 - i) < saltLen) {
			memcpy(&saltpass[i], salt, 64 - i);
			i = 64;
		} else {
			memcpy(&saltpass[i], salt, saltLen);
			i += saltLen;
		}
	}	
	
	for (i = 0; i < 64; i++) {
		saltpass[64 + i] = password[i % passLen];
	}
	
	if (*outlen == SHA1_HASH_SIZE) {
		count = 1;
	} else {
		count = (*outlen/SHA1_HASH_SIZE) + 1;
	}
	cpyLen = *outlen;
	
	front = p = psMalloc(pool, cpyLen);
	if (front == NULL) {
		return PS_MEM_FAIL;
	}
	
	while (count) {
		psSha1Init(&ctx);
		psSha1Update(&ctx, diversifier, 64);
		psSha1Update(&ctx, saltpass, 128);
		psSha1Final(&ctx, hash);
		for (j = 1; j < iter; j++) {
			psSha1Init(&ctx);
			psSha1Update(&ctx, hash, SHA1_HASH_SIZE);
			psSha1Final(&ctx, hash);
		}
		/* Copy into outgoing key now */
		copy = min(cpyLen, SHA1_HASH_SIZE);
		memcpy(p, hash, copy);
		p += copy;
		count--;
		cpyLen -= copy;
		
		if (cpyLen > 0) {
			/* manipulate saltpass */
			for (j = 0; j < 64; j++) {
				B[j] = hash[j % SHA1_HASH_SIZE];
			}
			if (pstm_init_for_read_unsigned_bin(pool, &bigb, 64) < 0) {
				return PS_MEM_FAIL;
			}
			if (pstm_read_unsigned_bin(&bigb, B, 64) < 0) {
				pstm_clear(&bigb);
				return PS_MEM_FAIL;
			}
			if (pstm_init_for_read_unsigned_bin(pool, &bigone, 1) < 0) {
				pstm_clear(&bigb);
				return PS_MEM_FAIL;
			}
			pstm_set(&bigone, 1);
			if (pstm_add(&bigb, &bigone, &bigb) < 0) {
				pstm_clear(&bigone);
				pstm_clear(&bigb);
				return PS_MEM_FAIL;
			}
			pstm_clear(&bigone);
			for (j = 0; j < 128; j+=64) {
				if (pstm_init_for_read_unsigned_bin(pool, &bigtmp, 64) < 0) {
					pstm_clear(&bigone);
					pstm_clear(&bigb);
					return PS_MEM_FAIL;
				}
				if (pstm_read_unsigned_bin(&bigtmp, saltpass + j, 64) < 0) {
					pstm_clear(&bigone);
					pstm_clear(&bigb);
					pstm_clear(&bigtmp);
					return PS_MEM_FAIL;
				}
				if (pstm_add(&bigb, &bigtmp, &bigtmp) < 0) {
					pstm_clear(&bigone);
					pstm_clear(&bigb);
					pstm_clear(&bigtmp);
					return PS_MEM_FAIL;
				}
				binsize = pstm_unsigned_bin_size(&bigtmp);
				if (binsize > 64) {
					psAssert(binsize == 65);
					if (pstm_to_unsigned_bin(pool, &bigtmp, B) < 0) {
						pstm_clear(&bigone);
						pstm_clear(&bigb);
						pstm_clear(&bigtmp);
						return PS_MEM_FAIL;
					}
					memcpy(saltpass + j, B + 1, 64);
				} else {
					if (pstm_to_unsigned_bin(pool, &bigtmp, saltpass + j) < 0) {
						pstm_clear(&bigone);
						pstm_clear(&bigb);
						pstm_clear(&bigtmp);
						return PS_MEM_FAIL;
					}
				}
				pstm_clear(&bigtmp);
			}
			pstm_clear(&bigone);
			pstm_clear(&bigb);
		}	
	}
	
	*out = front;	
	return PS_SUCCESS;
}

/******************************************************************************/
/*
	Return value is how many bytes were parsed out of buf
*/
static int32 pkcs12import(psPool_t *pool, unsigned char **buf, int32 bufLen,
				unsigned char *password, int32 passLen,
				unsigned char **plaintext, uint32 *ptLen)
{
	psCipherContext_t	ctx;
	unsigned char	*p, *start, *end, *iv, *decryptKey, *pt;
	unsigned char	salt[8];
	int32			rc, oi, tmpint;
	uint32			tmplen, keyLen, ivLen;
	short			armor, cipher;
	
	*plaintext = NULL;
	*ptLen = 0;
	
	p = start = *buf;
	end = p + bufLen;
	
	/* Encryption Algorithm */
	if ((rc = getAsnAlgorithmIdentifier(&p, (int32)(end - p), &oi, 0,
			&tmpint)) < 0) {
		psTraceCrypto("Initial pkcs12import parse failure\n");
		return rc;
	}

	if (oi == OID_PKCS_PBESHA40RC2) {
#ifdef USE_RC2	
		armor = PBE12;
		cipher = AUTH_SAFE_RC2;
		keyLen = 8;
#else
		psTraceCrypto("Must enable USE_RC2 in cryptoConfig.h to parse\n");
		return PS_UNSUPPORTED_FAIL;
#endif		
	} else if (oi == OID_PKCS_PBESHA3DES3) {
		armor = PBE12;
		cipher = AUTH_SAFE_3DES;
		keyLen = DES3_KEY_LEN;
	} else {
		psTraceIntCrypto("Unsupported PBE algorithm %d\n", oi);
		return PS_UNSUPPORTED_FAIL;
	}

	if (armor == PBE12) {
		/* If PKCS12 param will be 
		pkcs-12PbeParams ::= SEQUENCE { 
			salt OCTET STRING, 
			iterations INTEGER 
		}
		*/
		if ((rc = getAsnSequence(&p, (int32)(end - p), &tmplen)) < 0) {
			psTraceCrypto("Initial PBE12 parse failure\n");
			return rc;
		}
		/* salt len */
		if ((*p++ != ASN_OCTET_STRING) ||
				getAsnLength(&p, (int32)(end - p), &tmplen) < 0) {
			return PS_PARSE_FAIL;
		}
		if ((tmplen > (uint32)(end - p)) || tmplen > 8) {
			psTraceCrypto("Bad salt length parsing import\n");
			return PS_LIMIT_FAIL;
		}
		memcpy(salt, p, tmplen);
		p += tmplen;
		/* iteration count */
		if (getAsnInteger(&p, (int32)(end - p), &tmpint) < 0) {
			return PS_PARSE_FAIL;
		}
		if (pkcs12pbe(pool, password, passLen, salt, 8, tmpint,
				PKCS12_KEY_ID, &decryptKey, &keyLen) < 0) {
			psTraceCrypto("Error generating pkcs12 key\n");
			return PS_UNSUPPORTED_FAIL;
		}
		ivLen = 8;
		if (pkcs12pbe(pool, password, passLen, salt, 8, tmpint,
				PKCS12_IV_ID, &iv, &ivLen) < 0) {
			psTraceCrypto("Error generating pkcs12 iv\n");
			psFree(decryptKey);
			return PS_UNSUPPORTED_FAIL;	
		}
	
	} else {
		/* PBES2 */
	}

	/* Got the keys but we still need to find the start of the encrypted data.
		Have seen a few different BER variations at this point in the spec
		depending on what wrapper we are in. Try all that we know about
	*/
	if (*p == (ASN_CONTEXT_SPECIFIC | ASN_PRIMITIVE)) {
		p++;
		if ((rc = getAsnLength(&p, (int32)(end - p), &tmplen)) < 0) {
			psFree(decryptKey);
			psFree(iv);
			return PS_PARSE_FAIL;
		}
	} else if (*p == (ASN_CONTEXT_SPECIFIC | ASN_CONSTRUCTED)) {
		p++;
		if ((rc = getAsnLength(&p, (int32)(end - p), &tmplen)) < 0) {
			psFree(decryptKey);
			psFree(iv);
			return PS_PARSE_FAIL;
		}
		if (*p++ != ASN_OCTET_STRING || getAsnLength(&p,
				(int32)(end - p), &tmplen) < 0) {
			psFree(decryptKey);
			psFree(iv);	
			return PS_PARSE_FAIL;
		}
	} else if (*p == ASN_OCTET_STRING) {
		p++;
		if ((rc = getAsnLength(&p, (int32)(end - p), &tmplen)) < 0) {
			psFree(decryptKey);
			psFree(iv);	
			return PS_PARSE_FAIL;
		}
	} else {
		psTraceCrypto("Error finding ciphertext in pkcs12import\n");
		psFree(decryptKey);
		psFree(iv);	
		return PS_PARSE_FAIL;
	}

	/* Don't decrypt in-situ because we'll need to MAC this all later */
	if ((pt = psMalloc(pool, tmplen)) == NULL) {
		psTraceCrypto("Out-of-memory.  Increase SSL_KEY_POOL_SIZE\n");
		psFree(decryptKey);
		psFree(iv);
		return PS_MEM_FAIL;
	}
	if (cipher == AUTH_SAFE_3DES) {
		if ((rc = psDes3Init(&ctx, iv, decryptKey, 24)) < 0) {
			psFree(decryptKey);
			psFree(iv);
			psFree(pt);
			return rc;
		}
		if ((rc = psDes3Decrypt(&ctx, p, pt, tmplen)) < 0) {
			psFree(decryptKey);
			psFree(iv);
			psFree(pt);
			return rc;
		}
	}
#ifdef USE_RC2	
	if (cipher == AUTH_SAFE_RC2) {
	
		/* This is a 40-bit RC2! */
		if ((rc = psRc2Init(&ctx, iv, decryptKey, 5)) < 0) {
			psFree(decryptKey);
			psFree(iv);
			psFree(pt);
			return rc;
		}
		if ((rc = psRc2Decrypt(&ctx, p, pt, tmplen)) < 0) {
			psFree(decryptKey);
			psFree(iv);
			psFree(pt);
			return rc;
		}
	}
#endif /* USE_RC2 */

	psFree(decryptKey);
	psFree(iv);
			
	*plaintext = pt;
	*ptLen = tmplen;		
	return (int32)(p - start);
}

/******************************************************************************/
/*
	Determines what the safebag is and loads the material into the users
	data structure (cert or private key)
*/
static int32 parseSafeContents(psPool_t *pool, unsigned char *password,
			uint32 passLen,	psX509Cert_t **cert, psPubKey_t **privKey,
			unsigned char *buf,	uint32 totlen)
{
	psX509Cert_t		*currCert, *frontCert;
	unsigned char		*p, *end, *pt, *safeLen;
	uint32				tmplen, cryptlen;
	int32				rc, tmpint, bagoi, certoi;
#ifdef PARSE_PKCS12_SAFE_ATTRIBS
	uint32				attriblen;
	int32				attriboi;
#endif

	p = buf;
	end = p + totlen;
			
	/*	SafeContents ::= SEQUENCE OF SafeBag
	
		SafeBag ::= SEQUENCE { 
			bagId     BAG-TYPE.&id ({PKCS12BagSet}) 
			bagValue  [0] EXPLICIT BAG-TYPE.&Type({PKCS12BagSet}{@bagId}), 
			bagAttributes SET OF PKCS12Attribute OPTIONAL 
	} */		
	if ((rc = getAsnSequence(&p, (int32)(end - p), &tmplen)) < 0) {
		psTraceCrypto("Initial SafeContents parse failure\n");
		return rc;
	}
	
	end = p + tmplen;
	
	while (p < end) {
		/*
		bagtypes OBJECT IDENTIFIER ::= {pkcs-12 10 1} 
			
		BAG-TYPE ::= TYPE-IDENTIFIER 
			keyBag BAG-TYPE ::= {KeyBag IDENTIFIED BY {bagtypes 1}} 
			pkcs8ShroudedKeyBag BAG-TYPE ::= {PKCS8ShroudedKeyBag IDENTIFIED BY
				{bagtypes 2}} 
			certBag BAG-TYPE ::= {CertBag IDENTIFIED BY {bagtypes 3}} 
			crlBag BAG-TYPE ::= {CRLBag IDENTIFIED BY {bagtypes 4}} 
			secretBag BAG-TYPE ::= {SecretBag IDENTIFIED BY {bagtypes 5}} 
			safeContentsBag BAG-TYPE ::= {SafeContents IDENTIFIED BY
				{bagtypes 6}} 
			
		PKCS12BagSet BAG-TYPE ::= { 
			keyBag | pkcs8ShroudedKeyBag | certBag | crlBag | secretBag | 
				safeContentsBag, ... -- For future extensions} 

		*/
		if ((rc = getAsnAlgorithmIdentifier(&p, (int32)(end - p), &bagoi, 0,
				&tmpint)) < 0) {
			psTraceCrypto("Initial BagType parse failure\n");	
			return rc;
		}
		safeLen = p + tmpint;
		if (*p++ != (ASN_CONTEXT_SPECIFIC | ASN_CONSTRUCTED)) {
			return PS_PARSE_FAIL;
		}	
		if ((rc = getAsnLength(&p, (int32)(end - p), &tmplen)) < 0) {
			return PS_PARSE_FAIL;
		}

		switch (bagoi) {
			case PKCS12_BAG_TYPE_CERT:
				/*
				CertBag ::= SEQUENCE { 
				  certId    BAG-TYPE.&id   ({CertTypes}), 
				  certValue [0] EXPLICIT BAG-TYPE.&Type ({CertTypes}{@certId}) } 

				x509Certificate BAG-TYPE ::= 
					{OCTET STRING IDENTIFIED BY {certTypes 1}} 
					-- DER-encoded X.509 certificate stored in OCTET STRING 
				sdsiCertificate BAG-TYPE ::= 
					{IA5String IDENTIFIED BY {certTypes 2}} 
					-- Base64-encoded SDSI certificate stored in IA5String 

				CertTypes BAG-TYPE ::= { 
					x509Certificate | 
					sdsiCertificate, 
					... -- For future extensions 
				} 
				*/
				if ((rc = getAsnAlgorithmIdentifier(&p, (int32)(end - p),
						&certoi, 0, &tmpint)) < 0) {
					psTraceCrypto("Initial CertBag parse failure\n");	
					return rc;
				}
				if (certoi != PKCS9_CERT_TYPE_X509) {
					psTraceIntCrypto("Unsupported CertBag type %d\n", certoi);
					return PS_UNSUPPORTED_FAIL;
				}
				if (*p++ != (ASN_CONTEXT_SPECIFIC | ASN_CONSTRUCTED)) {
					return PS_PARSE_FAIL;
				}	
				if ((rc = getAsnLength(&p, (int32)(end - p), &tmplen)) < 0) {
					return rc;
				}
				if ((*p++ != ASN_OCTET_STRING) ||
						getAsnLength(&p, (int32)(end - p), &tmplen) < 0) {
					psTraceCrypto("Couldn't extract X509 CertBag\n");
					return PS_FAILURE;
				}
				/* Support cert chains */
				currCert = frontCert = *cert;
				while (currCert != NULL) {
					currCert = currCert->next;
				}
				if ((rc = psX509ParseCert(pool, p, tmplen, &currCert,
						CERT_STORE_UNPARSED_BUFFER)) < 0) {
					psX509FreeCert(*cert);
					*cert = NULL;
					psTraceCrypto("Couldn't parse certificate from CertBag\n");
					return rc;
				}
				if (*cert == NULL) {
					*cert = currCert;
				} else {
					while (frontCert->next != NULL) {
						frontCert = frontCert->next;
					}
					frontCert->next = currCert;
				}
				p += rc;
				break;
				
			case PKCS12_BAG_TYPE_SHROUD:
				/* A PKCS8ShroudedKeyBag holds a private key, which has been
					shrouded in accordance with PKCS #8.  Note that a
					PKCS8ShroudedKeyBag holds only one shrouded private key. */				
				if (getAsnSequence(&p, (int32)(end - p), &tmplen) < 0) {
					psTraceCrypto("Initial PKCS8 ShroudKeyBag parse failure\n");
					return PS_PARSE_FAIL;
				}
				if ((rc = pkcs12import(pool, &p, (int32)(end - p), password,
						passLen, &pt, &cryptlen)) < 0) {
					psTraceIntCrypto("Import failed from AuthSafe %d\n", rc);
					return rc;	
				}
				p += rc;
				/* Result of decrypt will be a PKCS#8 key */
				if ((rc = pkcs8ParsePrivBin(pool, pt, cryptlen, NULL, privKey))
						< 0) {
					psFree(pt);
					psTraceIntCrypto("Failed shroud PKCS8 key parse %d\n", rc);
					return rc;
				}
				psFree(pt);
				p += cryptlen;
				break;	
			default:
				psTraceIntCrypto("Unsupported BAG_TYPE %d\n", bagoi);
				return PS_UNSUPPORTED_FAIL;
		}

		/* Attributes are at the end of the data */
		while (p != safeLen) {
#ifdef PARSE_PKCS12_SAFE_ATTRIBS
			/* 
				TODO: incomplete parse
				
				PKCS12Attribute ::= SEQUENCE { 
					attrId ATTRIBUTE.&id ({PKCS12AttrSet}), 
					attrValues SET OF ATTRIBUTE.&Type ({PKCS12AttrSet}{@attrId}) 
				} -- This type is compatible with the X.500 type ’Attribute’ 

				PKCS12AttrSet ATTRIBUTE ::= {
					PKCS 12 V1.0: PERSONAL INFORMATION EXCHANGE SYNTAX 9 
					friendlyName | -- from PKCS #9 
					localKeyId, -- from PKCS #9 
					... -- Other attributes are allowed 
				}
			*/ 
			if ((rc = getAsnSet(&p, (int32)(end - p), &attriblen)) < 0) {
				return rc;
			}
			if ((rc = getAsnAlgorithmIdentifier(&p, (int32)(end - p), &attriboi,
					0, &pLen)) < 0) {
				return rc;
			}
			if ((rc = getAsnSet(&p, (int32)(end - p), &attriblen)) < 0) {
				return rc;
			}
			if ((*p++ != ASN_OCTET_STRING) ||
					getAsnLength(&p, (int32)(end - p), &tmplen) < 0) {
				psTraceCrypto("Couldn't parse PKCS#8 param salt\n");
				return PS_FAILURE;
			}
			p += tmplen;
#else
			p = safeLen;
#endif		
		}
	}
	return totlen;
}

/******************************************************************************/
/*
	AuthenticatedSafe ::= SEQUENCE OF ContentInfo 
		-- Data if unencrypted 
		-- EncryptedData if password-encrypted 
		-- EnvelopedData if public key-encrypted
*/
static int32 psParseAuthenticatedSafe(psPool_t *pool, psX509Cert_t **cert,
			psPubKey_t **privKey, unsigned char *importPass, int32 ipassLen,
			unsigned char **buf, int32 totLen)
{
	unsigned char		*p, *end, *pt;
	uint32				tmplen;
	int32				rc, oi, tmpint;

	p = *buf;
	end = p + totLen;
	if ((rc = getAsnSequence(&p, (int32)(end - p), &tmplen)) < 0) {
		psTraceCrypto("Initial authenticated safe parse failure\n");
		return rc;
	}
	
	end = p + tmplen; /* Set end to be end of authSafe for list walk */
	
	while (p < end) {
		if ((rc = getAsnAlgorithmIdentifier(&p, (int32)(end - p), &oi, 0,
				&tmpint)) < 0) {
			psTraceCrypto("Initial content info parse failure\n");
			return rc;
		}
		if (oi == PKCS7_ENCRYPTED_DATA) {
			/* password protected mode */
			if (*p++ != (ASN_CONTEXT_SPECIFIC | ASN_CONSTRUCTED)) {
				psTraceCrypto("Initial pkcs7 encrypted data parse failure\n");
				return PS_PARSE_FAIL;
			}
			if ((rc = getAsnLength(&p, (int32)(end - p), &tmplen)) < 0) {
				return PS_PARSE_FAIL;
			}
			/* EncryptedData ::= SEQUENCE {
				version Version,
				encryptedContentInfo EncryptedContentInfo } */
			if ((rc = getAsnSequence(&p, (int32)(end - p), &tmplen)) < 0) {
				return rc;
			}
			/* Version */
			if (getAsnInteger(&p, (int32)(end - p), &tmpint) < 0 ||
					tmpint != 0) {
				psTraceIntCrypto("Unsupported encryptd data version %d\n",
					tmpint);
				return PS_UNSUPPORTED_FAIL;
			}
			/* 
			EncryptedContentInfo ::= SEQUENCE {
				contentType     ContentType,
				contentEncryptionAlgorithm ContentEncryptionAlgorithmIdentifier,
				encryptedContent [0] IMPLICIT EncryptedContent OPTIONAL }
				
			5.1 2b) If SCi is to be encrypted with a password, make a
			ContentInfo CIi of type EncryptedData.  The encryptedContentInfo
			field of CIi has its contentType field set to data and its
			encryptedContent field set to the encryption of the BER-encoding
			of SCi (note that the tag and length octets shall be present). */
			if ((rc = getAsnAlgorithmIdentifier(&p, (int32)(end - p), &oi, 0,
					&tmpint)) < 0) {
				psTraceCrypto("Initial EncryptedContentInfo parse failure\n");
				return rc;
			}
			psAssert(oi == PKCS7_DATA);
		
			if ((rc = pkcs12import(pool, &p, (int32)(end - p), importPass,
					ipassLen, &pt, &tmplen)) < 0) {
				psTraceIntCrypto("Import failed from AuthSafe %d\n", rc);
				return rc;	
			}
			p += rc;
			
			/* pt is now a BER-encoded SafeContents */
			if ((rc = parseSafeContents(pool, importPass, ipassLen, cert,
					privKey, pt, tmplen)) < 0) {
				psTraceCrypto("Error parsing encrypted safe contents\n");
				psTraceCrypto("Is it possible the password is incorrect?\n");	
				psFree(pt);
				return rc;
			}
			psFree(pt);
			p += rc;
		} else if (oi == PKCS7_DATA) {
			/* Data ::= OCTET STRING */
			if (*p++ != (ASN_CONTEXT_SPECIFIC | ASN_CONSTRUCTED)) {
				psTraceCrypto("Initial pkcs7 data parse failure\n");
				return PS_PARSE_FAIL;
			}
			if ((rc = getAsnLength(&p, (int32)(end - p), &tmplen)) < 0) {
				return PS_PARSE_FAIL;
			}
			if (*p++ != ASN_OCTET_STRING || getAsnLength(&p,
						(int32)(end - p), &tmplen) < 0) {
					return PS_PARSE_FAIL;
			}
			if ((rc = parseSafeContents(pool, importPass, ipassLen, cert,
					privKey, p, tmplen)) < 0) {
				psTraceCrypto("Error parsing plaintext safe contents\n");	
				return rc;
			}
			p += rc;
		} else {
			psTraceIntCrypto("Unsupported PKCS7 data type parse %d\n", oi);
			return PS_UNSUPPORTED_FAIL;
		}
	}

	*buf = p;
	return PS_SUCCESS;
}

/******************************************************************************/
/*
	
	MacData ::= SEQUENCE { 
		mac DigestInfo, 
		macSalt OCTET STRING, 
		iterations INTEGER DEFAULT 1  
	} 
*/
int32 psPkcs12Parse(psPool_t *pool, psX509Cert_t **cert, psPubKey_t **privKey,
		const unsigned char *file, int32 flags, unsigned char *password,
		int32 pLen, unsigned char *macPass, int32 macPassLen)
{
	psHmacContext_t hmac;
	unsigned char	*fileBuf, *p, *end, *macStart, *macEnd, *macKey;
	unsigned char	iwidePass[128]; /* 63 char password max */
	unsigned char	mwidePass[128];
	unsigned char	mac[SHA1_HASH_SIZE];
	unsigned char	macSalt[20];
	unsigned char	digest[SHA1_HASH_SIZE];
	uint32			tmplen, digestLen, macKeyLen;
	int32			fsize, i, j, rc, tmpint, mpassLen, ipassLen, integrity, oi;
	
	if ((rc = psGetFileBuf(pool, (char *)file, &fileBuf, &fsize)) < PS_SUCCESS){
		psTraceStrCrypto("Couldn't open PKCS#12 file %s\n", (char*)file);
		return rc;
	}
	p = fileBuf;
	end = p + fsize;

	/* Begin with a PFX 
		PFX ::= SEQUENCE { 
		version INTEGER {v3(3)}(v3,...),
		authSafe ContentInfo, 
		macData    MacData OPTIONAL } */
	if ((rc = getAsnSequence(&p, (int32)(end - p), &tmplen)) < 0) {
		psTraceCrypto("Initial PKCS#12 parse fail\n");
		goto ERR_FBUF;
	}
	/* Version */
	if (getAsnInteger(&p, (int32)(end - p), &tmpint) < 0 || tmpint != 3) {
		psTraceIntCrypto("Unsupported PKCS#12 version %d\n", tmpint);
		rc = PS_UNSUPPORTED_FAIL;
		goto ERR_FBUF;
	}
	
	/* Content type is the integrity mode (4 of the spec). 
		signedData for public-key integrity or data for password integrity */
	if ((integrity = psParseIntegrityMode(&p, (int32)(end - p)))
			< PS_SUCCESS) {
		psTraceCrypto("Couldn't determine PKCS#12 integrity\n");	
		rc = integrity;
		goto ERR_FBUF;
	}
	
	/* Passwords are wide BMPString types
		ipass is import password
		mpass is MAC password */
	ipassLen = (pLen * 2) + 2; /* 2 for each char put double 0x0 to terminate */
	memset(iwidePass, 0x0, ipassLen);
	j = 0;
	for (i = 1; i < ipassLen - 1; i+=2) {
		memset(&iwidePass[i], password[j], 1);
		j++;
	}
	
	/* Content data is an AuthenticatedSafe */
	macStart = p;
	if ((rc = psParseAuthenticatedSafe(pool, cert, privKey, iwidePass, ipassLen,
			&p, (int32)(end - p))) < PS_SUCCESS) {
		psTraceIntCrypto("PKCS#12 AuthenticatedSafe parse failure %d\n", rc);	
		goto ERR_FBUF;
	}
	macEnd = p;
	
	/* Integrity validation */
	if (integrity == PASSWORD_INTEGRITY) {
		mpassLen = (macPassLen * 2) + 2;
		memset(mwidePass, 0x0, mpassLen);
		j = 0;
		for (i = 1; i < mpassLen - 1; i+=2) {
			memset(&mwidePass[i], macPass[j], 1);
			j++;
		}
		/* MacData ::= SEQUENCE {
			mac 		DigestInfo,
			macSalt		OCTET STRING,
			iterations	INTEGER DEFAULT 1
			-- Note: The default is for historical reasons and its use is
			-- deprecated. A higher value, like 1024 is recommended. } */
		if ((rc = getAsnSequence(&p, (int32)(end - p), &tmplen)) < 0) {
			psTraceCrypto("Initial password integrity parse failure\n");	
			goto ERR_FBUF;
		}
		/* DigestInfo ::= SEQUENCE {
			digestAlgorithm DigestAlgorithmIdentifier,
			digest          Digest } */
		if ((rc = getAsnSequence(&p, (int32)(end - p), &tmplen)) < 0) {
			psTraceCrypto("Sequence password integrity parse failure\n");
			goto ERR_FBUF;
		}	
		if ((rc = getAsnAlgorithmIdentifier(&p, (uint32)(end - p),
				&oi, 0, &tmpint)) < 0) {
			psTraceCrypto("Algorithm password integrity parse failure\n");
			goto ERR_FBUF;
		}
		if ((*p++ != ASN_OCTET_STRING) ||
				getAsnLength(&p, (int32)(end - p), &tmplen) < 0) {
			psTraceCrypto("Octet digest password integrity parse failure\n");
			rc = PS_PARSE_FAIL;
			goto ERR_FBUF;
		}
		memcpy(digest, p, tmplen);
		p += tmplen;
		if ((*p++ != ASN_OCTET_STRING) ||
				getAsnLength(&p, (int32)(end - p), &tmplen) < 0) {
			psTraceCrypto("Octet macSalt password integrity parse failure\n");
			rc = PS_PARSE_FAIL;
			goto ERR_FBUF;
		}
		if (tmplen > 20) {
			psTraceCrypto("macSalt length too long\n");
			rc = PS_PARSE_FAIL;
			goto ERR_FBUF;
		}
		memcpy(macSalt, p, tmplen);
		p += tmplen;
		/* Iteration count */
		if (getAsnInteger(&p, (int32)(end - p), &tmpint) < 0) {
			psTraceCrypto("Iteration password integrity parse failure\n");
			rc = PS_PARSE_FAIL;
			goto ERR_FBUF;
		}
		psAssert(p == end); /* That's all folks */
		
		if (oi == OID_SHA1_ALG) {
			/* When password integrity mode is used to secure a PFX PDU,
				an SHA-1 HMAC is computed on the BER-encoding of the contents
				of the content field of the authSafe field in the PFX PDU */
				macKeyLen = 20;
				if (pkcs12pbe(pool, mwidePass, mpassLen, macSalt, tmplen,
						tmpint, PKCS12_MAC_ID, &macKey, &macKeyLen) < 0) {
					psTraceCrypto("Error generating pkcs12 hmac key\n");
					rc = PS_UNSUPPORTED_FAIL;
					goto ERR_FBUF;
				}
				digestLen = (uint32)(macEnd - macStart);
				psHmacSha1Init(&hmac, macKey, macKeyLen);
				psHmacSha1Update(&hmac, macStart, digestLen);
				psHmacSha1Final(&hmac, mac);
				psFree(macKey);
				if (memcmp(digest, mac, SHA1_HASH_SIZE) != 0) {
					psTraceCrypto("CAUTION: PKCS#12 MAC did not validate\n");
				}
		} else {
			psTraceCrypto("PKCS#12 must use SHA1 HMAC validation\n");
			rc = PS_UNSUPPORTED_FAIL;
			goto ERR_FBUF;
		}
	
	}
	rc = PS_SUCCESS;
ERR_FBUF:
	psFree(fileBuf);
	return rc;
}

#endif /* USE_PKCS12 */
#endif /* MATRIX_USE_FILE_SYSTEM */

#endif /* USE_PKCS8 */

#ifdef MATRIX_USE_FILE_SYSTEM
static const char encryptHeader[] = "DEK-Info: DES-EDE3-CBC,";

#ifdef USE_PKCS5
/******************************************************************************/
/*
	Convert an ASCII hex representation to a binary buffer.
	Decode enough data out of 'hex' buffer to produce 'binlen' bytes in 'bin'
	Two digits of ASCII hex map to the high and low nybbles (in that order),
	so this function assumes that 'hex' points to 2x 'binlen' bytes of data.
	Return the number of bytes processed from hex (2x binlen) or < 0 on error.
*/
static int32 hexToBinary(unsigned char *hex, unsigned char *bin, int32 binlen)
{
	unsigned char	*end, c, highOrder;

	highOrder = 1;
	for (end = hex + binlen * 2; hex < end; hex++) {
		c = *hex;
		if ('0' <= c && c <='9') {
			c -= '0';
		} else if ('a' <= c && c <='f') {
			c -= ('a' - 10);
		} else if ('A' <= c && c <='F') {
			c -= ('A' - 10);
		} else {
			psTraceCrypto("hexToBinary failure\n");
			return PS_FAILURE;
		}
		if (highOrder++ & 0x1) {
			*bin = c << 4;
		} else {
			*bin |= c;
			bin++;
		}
	}
	return binlen * 2;
}
#endif /* USE_PKCS5 */

/******************************************************************************/
/*
	File must be a PEM format RSA keys
	Memory info:
		Caller must call psFreePubKey on outkey on function success
		Caller does not need to free outkey on function failure
*/
int32 pkcs1ParsePrivFile(psPool_t *pool, char *fileName, char *password,
			psPubKey_t **outkey)
{
	unsigned char	*DERout;
	int32			rc;
	uint32			DERlen;
	psPubKey_t		*rsakey;

	*outkey = NULL;

	if ((rc = pkcs1DecodePrivFile(pool, fileName, password, &DERout, &DERlen))
			< PS_SUCCESS) {
		return rc;
	}

	if ((rc = pkcs1ParsePrivBin(pool, DERout, DERlen, &rsakey)) < 0) {
#ifdef USE_PKCS8
		/* This logic works for processing PKCS#8 files becuase the above file
			and bin decodes will always leave the unprocessed buffer intact and
			the password protection is done in the internal ASN.1 encoding */
		if ((rc = pkcs8ParsePrivBin(pool, DERout, DERlen, password,
				&rsakey)) < 0) {
			psFree(DERout);
			return rc;
		}
#else
		psFree(DERout);
		return rc;
#endif
	}
	
	psFree(DERout);
	*outkey = rsakey;
	return PS_SUCCESS;
}

/******************************************************************************/
/*
	Return the DER stream from a private key PEM file
	
	Memory info:
		Caller must call psFree on DERout on function success
*/
int32 pkcs1DecodePrivFile(psPool_t *pool, char *fileName, char *password,
			unsigned char **DERout, uint32 *DERlen)
{
	unsigned char	*keyBuf;
	char			*start, *end, *endTmp;
	int32			keyBufLen, rc;
	uint32			PEMlen = 0;
#ifdef USE_PKCS5
	psCipherContext_t	ctx;
	unsigned char	passKey[DES3_KEY_LEN];
	unsigned char	cipherIV[DES3_IV_LEN];
	int32			tmp, encrypted = 0;
#endif /* USE_PKCS5 */

	if (fileName == NULL) {
		psTraceCrypto("No fileName passed to pkcs1ParsePrivFile\n");
		return PS_ARG_FAIL;
	}
	if ((rc = psGetFileBuf(pool, fileName, &keyBuf, &keyBufLen)) < PS_SUCCESS) {
		return rc;
	}
	start = end = NULL;

/*
 *	Check header and encryption parameters.
 */
	if (((start = strstr((char*)keyBuf, "-----BEGIN")) != NULL) && 
			((start = strstr((char*)keyBuf, "PRIVATE KEY-----")) != NULL) &&
			((end = strstr(start, "-----END")) != NULL) &&
			((endTmp = strstr(end, "PRIVATE KEY-----")) != NULL)) {
		start += strlen("PRIVATE KEY-----");
		while (*start == '\x0d' || *start == '\x0a') {
			start++;
		}
		PEMlen = (uint32)(end - start);
	} else {
		psTraceCrypto("File buffer does not look to be in PKCS#1 PEM format\n");
		psFree(keyBuf);
		return PS_PARSE_FAIL;
	}

	if (strstr((char*)keyBuf, "Proc-Type:") &&
			strstr((char*)keyBuf, "4,ENCRYPTED")) {
#ifdef USE_PKCS5
		encrypted++;
		if (password == NULL) {
			psTraceCrypto("No password given for encrypted private key file\n");
			psFree(keyBuf);
			return PS_ARG_FAIL;
		}
		if ((start = strstr((char*)keyBuf, encryptHeader)) == NULL) {
			psTraceCrypto("Unrecognized private key file encoding\n");
			psFree(keyBuf);
			return PS_PARSE_FAIL;
		}
		start += strlen(encryptHeader);
		/* we assume here that header points to at least 16 bytes of data */
		tmp = hexToBinary((unsigned char*)start, cipherIV, DES3_IV_LEN);
		if (tmp < 0) {
			psTraceCrypto("Invalid private key file salt\n");
			psFree(keyBuf);
			return PS_FAILURE;
		}
		start += tmp;
		pkcs5pbkdf1((unsigned char*)password, strlen(password),
			cipherIV, 1, (unsigned char*)passKey);
		PEMlen = (int32)(end - start);
#else  /* !USE_PKCS5 */
/*
 *		The private key is encrypted, but PKCS5 support has been turned off
 */
		psTraceCrypto("USE_PKCS5 must be enabled for key file password\n");
		psFree(keyBuf);
		return PS_UNSUPPORTED_FAIL;  
#endif /* USE_PKCS5 */
	}

/*
	Take the raw input and do a base64 decode
 */
	*DERout = psMalloc(pool, PEMlen);
	if (*DERout == NULL) {
		psFree(keyBuf);
		psError("Memory allocation error in pkcs1ParsePrivFile\n");
		return PS_MEM_FAIL;
	}
	*DERlen = PEMlen;
	if ((rc = psBase64decode((unsigned char*)start, PEMlen, *DERout,
			DERlen)) != 0) {
			psTraceCrypto("Error base64 decode of private key\n");			
		if (password) {
			psTraceCrypto("Is it possible the password is incorrect?\n");
		}
		psFree(*DERout);
		psFree(keyBuf);
		return rc;
	}
	psFree(keyBuf);
	
#ifdef USE_PKCS5
/*
 *	Decode
 */
	if (encrypted == 1 && password) {
		psDes3Init(&ctx, cipherIV, passKey, DES3_KEY_LEN);
		psDes3Decrypt(&ctx, *DERout, *DERout, *DERlen);
	}
#endif /* USE_PKCS5 */

	return PS_SUCCESS;
}

#endif /* MATRIX_USE_FILE_SYSTEM */
#endif /* USE_PRIVATE_KEY_PARSING */
/******************************************************************************/

/******************************************************************************/
#ifdef USE_PKCS5
/******************************************************************************/
/*
 Generate a key given a password and salt value.
 PKCS#5 2.0 PBKDF1 key derivation format with MD5 and count == 1 per:
 http://www.rsasecurity.com/rsalabs/pkcs/pkcs-5/index.html
 
 This key is compatible with the algorithm used by OpenSSL to encrypt keys
 generated with 'openssl genrsa'.  If other encryption formats are used
 (for example PBKDF2), or an iteration count != 1 is used, they are not 
 compatible with this simple implementation.  OpenSSL provides many options
 for converting key formats to the one used here.
 
 A 3DES key is 24 bytes long, to generate it with this algorithm,
 we md5 hash the password and salt for the first 16 bytes.  We then 
 hash these first 16 bytes with the password and salt again, generating 
 another 16 bytes.  We take the first 16 bytes and 8 of the second 16 to 
 form the 24 byte key.
 
 salt is assumed to point to 8 bytes of data
 key is assumed to point to 24 bytes of data
 */
void pkcs5pbkdf1(unsigned char *pass, uint32 passlen, unsigned char *salt,
				 int32 iter, unsigned char *key)
{
	psDigestContext_t	state;
	unsigned char		md5[MD5_HASH_SIZE];
	
	psAssert(iter == 1);
	
	psMd5Init(&state);
	psMd5Update(&state, pass, passlen);
	psMd5Update(&state, salt, 8);
	psMd5Final(&state, md5);
	memcpy(key, md5, MD5_HASH_SIZE);
	
	psMd5Init(&state);
	psMd5Update(&state, md5, MD5_HASH_SIZE);
	psMd5Update(&state, pass, passlen);
	psMd5Update(&state, salt, 8);
	psMd5Final(&state, md5);
	memcpy(key + MD5_HASH_SIZE, md5, 24 - MD5_HASH_SIZE);
	
	memset(md5, 0x0, sizeof(md5));	/* zero out sensitive info */
}

#if defined(USE_HMAC) && defined(USE_SHA1)
/******************************************************************************/
/*
	Generate a key given a password, salt and iteration value.
	PKCS#5 2.0 PBKDF2 key derivation format with HMAC-SHA per:
	http://www.rsasecurity.com/rsalabs/pkcs/pkcs-5/index.html
	 
	Given a password, a salt, and an iteration count (rounds), generate a 
	key suitable for encrypting data with 3DES, AES, etc.
	key should point to storage as large as kLen
*/
void pkcs5pbkdf2(unsigned char *password, uint32 pLen, 
				 unsigned char *salt, uint32 sLen, int32 rounds,
				 unsigned char *key, uint32 kLen)
{
	int32				itts;
	uint32				blkno;
	unsigned long		stored, left, i;
	unsigned char		buf[2][SHA1_HASH_SIZE];
	psHmacContext_t		hmac;
	
	psAssert(password && salt && key && kLen);
	
	left   = kLen;
	blkno  = 1;
	stored = 0;
	while (left != 0) {
		/* process block number blkno */
		memset(buf[0], 0x0, SHA1_HASH_SIZE * 2);
		
		/* store current block number and increment for next pass */
		STORE32H(blkno, buf[1]);
		++blkno;
		
		/* get PRF(P, S||int(blkno)) */
		psHmacSha1Init(&hmac, password, pLen);
		psHmacSha1Update(&hmac, salt, sLen);
		psHmacSha1Update(&hmac, buf[1], 4);
		psHmacSha1Final(&hmac, buf[0]);
		
		/* now compute repeated and XOR it in buf[1] */
		memcpy(buf[1], buf[0], SHA1_HASH_SIZE);
		for (itts = 1; itts < rounds; ++itts) {
			psHmacSha1Init(&hmac, password, pLen);
			psHmacSha1Update(&hmac, buf[0], SHA1_HASH_SIZE);
			psHmacSha1Final(&hmac, buf[0]);
			for (i = 0; i < SHA1_HASH_SIZE; i++) {
				buf[1][i] ^= buf[0][i];
			}
		}
		/* now emit upto x bytes of buf[1] to output */
		for (i = 0; i < SHA1_HASH_SIZE && left != 0; ++i) {
			key[stored++] = buf[1][i];
			--left;
		}
	}
	
	memset(buf[0], 0x0, SHA1_HASH_SIZE * 2);
	memset(&hmac, 0x0, sizeof(psHmacContext_t));
}
#endif /* USE_HMAC && USE_SHA1 */
#endif /* USE_PKCS5 */


/******************************************************************************/
/******************************************************************************/


