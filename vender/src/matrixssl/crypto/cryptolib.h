/*
 *	cryptolib.h
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

#ifndef _h_PS_CRYPTOLIB
#define _h_PS_CRYPTOLIB

#include "symmetric/symmetric.h"
#include "digest/digest.h"
#include "math/pstm.h"
#include "pubkey/pubkey.h"
#include "keyformat/asn1.h"
#include "keyformat/x509.h"
#include "prng/prng.h"

/******************************************************************************/
/*
	Configuration checks
*/
#ifdef USE_CERT_PARSE
	#ifndef USE_X509
	#error "Must enable USE_X509 if USE_CERT_PARSE is enabled"
	#endif	
	#if !defined(USE_MD5) || !defined(USE_SHA1)
	#error "Both USE_MD5 and USE_SHA1 must be enabled when enabling USE_X509"
	#endif
#endif 

#ifdef USE_HMAC
	#if !defined(USE_MD5) && !defined(USE_SHA1)
	#error "Must enable either MD5 or SHA1 in cryptoConfig.h for HMAC support"
	#endif
#endif

#ifdef USE_PKCS5
	#ifndef USE_MD5
	#error "Enable USE_MD5 in cryptoConfig.h for PKCS5 support"
	#endif
	#ifndef USE_3DES
	#error "Enable USE_3DES in cryptoConfig.h for PKCS5 support"
	#endif
#endif

#ifdef USE_PKCS8
	#ifndef USE_HMAC 
	#error "Enable USE_HMAC in cryptoConfig.h for PKCS8 support"
	#endif
#endif




#ifdef USE_PKCS12
	#ifndef USE_PKCS8
	#error "Enable USE_PKCS8 in cryptoConfig.h for PKCS12 support"
	#endif
#endif

/******************************************************************************/
/*
	Crypto trace
*/
#ifndef USE_CRYPTO_TRACE
#define psTraceCrypto(x) 
#define psTraceStrCrypto(x, y) 
#define psTraceIntCrypto(x, y)
#define psTracePtrCrypto(x, y)
#else
#define psTraceCrypto(x) _psTrace(x)
#define psTraceStrCrypto(x, y) _psTraceStr(x, y)
#define psTraceIntCrypto(x, y) _psTraceInt(x, y)
#define psTracePtrCrypto(x, y) _psTracePtr(x, y)
#endif /* USE_CRYPTO_TRACE */


/******************************************************************************/
/*
	Helpers
*/
extern int32 psBase64decode(const unsigned char *in,  uint32 len,
                    unsigned char *out, uint32 *outlen);

/******************************************************************************/
/*
    RFC 3279 OID
    Matrix uses an oid summing mechanism to arrive at these defines.
	The byte values of the OID are summed to produce a "relatively unique" int

    The duplicate defines do not pose a problem as long as they don't 
    exist in the same OID groupings
*/
#ifdef USE_X509
/* Raw digest algorithms */
#define OID_SHA1_ALG			88
#define OID_SHA256_ALG			414
#define OID_SHA512_ALG			416
#define OID_MD2_ALG				646
#define OID_MD5_ALG				649

/* Signature algorithms */
#define OID_MD2_RSA_SIG			646 
#define OID_MD5_RSA_SIG			648 /* 42.134.72.134.247.13.1.1.4 */
#define OID_SHA1_RSA_SIG		649 /* 42.134.72.134.247.13.1.1.5 */
#define OID_SHA256_RSA_SIG		655 /* 42.134.72.134.247.13.1.1.11 */
#define OID_SHA384_RSA_SIG		656 /* 42.134.72.134.247.13.1.1.11 */
#define OID_SHA512_RSA_SIG		657 /* 42.134.72.134.247.13.1.1.13 */
#define OID_SHA1_ECDSA_SIG		520	/* 42.134.72.206.61.4.1 */
#define OID_SHA224_ECDSA_SIG	523 /* 42.134.72.206.61.4.3.1 */
#define OID_SHA256_ECDSA_SIG	524 /* 42.134.72.206.61.4.3.2 */
#define OID_SHA384_ECDSA_SIG	525 /* 42.134.72.206.61.4.3.3 */
#define OID_SHA512_ECDSA_SIG	526 /* 42.134.72.206.61.4.3.4 */
#endif /* USE_X509 */

/* Public key algorithms */
#define OID_RSA_KEY_ALG			645 /* 42.134.72.134.247.13.1.1.1 */
#define OID_ECDSA_KEY_ALG		518 /* 42.134.72.206.61.2.1 */

#ifdef USE_PKCS5
#define OID_DES_EDE3_CBC		652 /* 42.134.72.134.247.13.3.7 */

#define OID_PKCS_PBKDF2			660 /* 42.134.72.134.247.13.1.5.12 */
#define OID_PKCS_PBES2			661 /* 42.134.72.134.247.13.1.5.13 */
#endif /* USE_PKCS5 */

#ifdef USE_PKCS12
#define OID_PKCS_PBESHA128RC4	657
#define OID_PKCS_PBESHA40RC4	658
#define OID_PKCS_PBESHA3DES3	659
#define OID_PKCS_PBESHA3DES2	660 /* warning: collision with pkcs5 */
#define OID_PKCS_PBESHA128RC2	661 /* warning: collision with pkcs5 */
#define OID_PKCS_PBESHA40RC2	662

#define PKCS7_DATA					651
#define PKCS7_SIGNED_DATA			652
#define PKCS7_ENVELOPED_DATA		653
#define PKCS7_SIGNED_ENVELOPED_DATA	654
#define PKCS7_DIGESTED_DATA			655
#define PKCS7_ENCRYPTED_DATA		656

#define PKCS12_BAG_TYPE_KEY			667
#define PKCS12_BAG_TYPE_SHROUD		668
#define PKCS12_BAG_TYPE_CERT		669
#define PKCS12_BAG_TYPE_CRL			670
#define PKCS12_BAG_TYPE_SECRET		671
#define PKCS12_BAG_TYPE_SAFE		672

#define PBE12						1
#define PBES2						2
#define AUTH_SAFE_3DES				1
#define AUTH_SAFE_RC2				2

#define PKCS12_KEY_ID				1
#define PKCS12_IV_ID				2
#define PKCS12_MAC_ID				3

#define PKCS9_CERT_TYPE_X509		675
#define PKCS9_CERT_TYPE_SDSI		676
#endif /* USE_PKCS12 */

/******************************************************************************/
/* These values are all mutually exlusive bits to define Cipher flags */
#define CRYPTO_FLAGS_AES	0x01
#define CRYPTO_FLAGS_AES256	0x02
#define CRYPTO_FLAGS_3DES	0x04
#define CRYPTO_FLAGS_ARC4	0x08
#define CRYPTO_FLAGS_SEED	0x10

#define CRYPTO_FLAGS_SHA1	0x20
#define CRYPTO_FLAGS_SHA2	0x40
#define CRYPTO_FLAGS_MD5	0x80

#define CRYPTO_FLAGS_TLS		0x100
#define CRYPTO_FLAGS_TLS_1_1	0x200
#define CRYPTO_FLAGS_TLS_1_2	0x400

#define CRYPTO_FLAGS_INBOUND	0x800
#define CRYPTO_FLAGS_ARC4INIT	0x1000
#define CRYPTO_FLAGS_BLOCKING	0x2000

#define CRYPTO_FLAGS_DISABLED	0x4000
#define CRYPTO_FLAGS_GCM		0x8000

#define CRYPTO_FLAGS_SHA3		0x10000 /* SHA-384 */

/******************************************************************************/

#define	CRYPT_INVALID_KEYSIZE	-21 
#define	CRYPT_INVALID_ROUNDS	-22

/******************************************************************************/
/* 32-bit Rotates */
/******************************************************************************/
#if defined(_MSC_VER)
/******************************************************************************/

/* instrinsic rotate */
#include <stdlib.h>
#pragma intrinsic(_lrotr,_lrotl)
#define ROR(x,n) _lrotr(x,n)
#define ROL(x,n) _lrotl(x,n)

/******************************************************************************/
#elif defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__)) && \
		!defined(INTEL_CC) && !defined(PS_NO_ASM)

static inline unsigned ROL(unsigned word, int i)
{
   asm ("roll %%cl,%0"
      :"=r" (word)
      :"0" (word),"c" (i));
   return word;
}

static inline unsigned ROR(unsigned word, int i)
{
   asm ("rorl %%cl,%0"
      :"=r" (word)
      :"0" (word),"c" (i));
   return word;
}

/******************************************************************************/
#else

/* rotates the hard way */
#define ROL(x, y) \
	( (((unsigned long)(x)<<(unsigned long)((y)&31)) | \
	(((unsigned long)(x)&0xFFFFFFFFUL)>>(unsigned long)(32-((y)&31)))) & \
	0xFFFFFFFFUL)
#define ROR(x, y) \
	( ((((unsigned long)(x)&0xFFFFFFFFUL)>>(unsigned long)((y)&31)) | \
	((unsigned long)(x)<<(unsigned long)(32-((y)&31)))) & 0xFFFFFFFFUL)

#endif /* 32-bit Rotates */
/******************************************************************************/

#ifdef HAVE_NATIVE_INT64
#ifdef _MSC_VER
	#define CONST64(n) n ## ui64
#else
	#define CONST64(n) n ## ULL
#endif
#endif

/******************************************************************************/
/*
 Controls endianess and size of registers.  Leave uncommented to get
 platform neutral [slower] code detect x86-32 machines somewhat
 */
#if (defined(_MSC_VER) && defined(WIN32)) || \
(defined(__GNUC__) && (defined(__DJGPP__) || defined(__CYGWIN__) || \
defined(__MINGW32__) || defined(__i386__)))
#define ENDIAN_LITTLE
#define ENDIAN_32BITWORD
#endif

/* #define ENDIAN_LITTLE */
/* #define ENDIAN_BIG */

/* #define ENDIAN_32BITWORD */
/* #define ENDIAN_64BITWORD */

#if (defined(ENDIAN_BIG) || defined(ENDIAN_LITTLE)) && \
!(defined(ENDIAN_32BITWORD) || defined(ENDIAN_64BITWORD))
#error You must specify a word size as well as endianess
#endif

#if !(defined(ENDIAN_BIG) || defined(ENDIAN_LITTLE))
#define ENDIAN_NEUTRAL
#endif

/*
 helper macros
 */
#if defined (ENDIAN_NEUTRAL)

#define STORE32L(x, y) { \
(y)[3] = (unsigned char)(((x)>>24)&255); \
(y)[2] = (unsigned char)(((x)>>16)&255);  \
(y)[1] = (unsigned char)(((x)>>8)&255); \
(y)[0] = (unsigned char)((x)&255); \
}

#define LOAD32L(x, y) { \
x = ((unsigned long)((y)[3] & 255)<<24) | \
((unsigned long)((y)[2] & 255)<<16) | \
((unsigned long)((y)[1] & 255)<<8)  | \
((unsigned long)((y)[0] & 255)); \
}

#define STORE64L(x, y) { \
(y)[7] = (unsigned char)(((x)>>56)&255); \
(y)[6] = (unsigned char)(((x)>>48)&255); \
(y)[5] = (unsigned char)(((x)>>40)&255); \
(y)[4] = (unsigned char)(((x)>>32)&255); \
(y)[3] = (unsigned char)(((x)>>24)&255); \
(y)[2] = (unsigned char)(((x)>>16)&255); \
(y)[1] = (unsigned char)(((x)>>8)&255); \
(y)[0] = (unsigned char)((x)&255); \
}

#define LOAD64L(x, y) { \
x = (((uint64)((y)[7] & 255))<<56)|(((uint64)((y)[6] & 255))<<48)| \
(((uint64)((y)[5] & 255))<<40)|(((uint64)((y)[4] & 255))<<32)| \
(((uint64)((y)[3] & 255))<<24)|(((uint64)((y)[2] & 255))<<16)| \
(((uint64)((y)[1] & 255))<<8)|(((uint64)((y)[0] & 255))); \
}

#define STORE32H(x, y) { \
(y)[0] = (unsigned char)(((x)>>24)&255); \
(y)[1] = (unsigned char)(((x)>>16)&255); \
(y)[2] = (unsigned char)(((x)>>8)&255); \
(y)[3] = (unsigned char)((x)&255); \
}

#define LOAD32H(x, y) { \
x = ((unsigned long)((y)[0] & 255)<<24) | \
((unsigned long)((y)[1] & 255)<<16) | \
((unsigned long)((y)[2] & 255)<<8)  | \
((unsigned long)((y)[3] & 255)); \
}

#define STORE64H(x, y) { \
(y)[0] = (unsigned char)(((x)>>56)&255); \
(y)[1] = (unsigned char)(((x)>>48)&255); \
(y)[2] = (unsigned char)(((x)>>40)&255); \
(y)[3] = (unsigned char)(((x)>>32)&255); \
(y)[4] = (unsigned char)(((x)>>24)&255); \
(y)[5] = (unsigned char)(((x)>>16)&255); \
(y)[6] = (unsigned char)(((x)>>8)&255); \
(y)[7] = (unsigned char)((x)&255); \
}

#define LOAD64H(x, y) { \
x = (((uint64)((y)[0] & 255))<<56)|(((uint64)((y)[1] & 255))<<48) | \
(((uint64)((y)[2] & 255))<<40)|(((uint64)((y)[3] & 255))<<32) | \
(((uint64)((y)[4] & 255))<<24)|(((uint64)((y)[5] & 255))<<16) | \
(((uint64)((y)[6] & 255))<<8)|(((uint64)((y)[7] & 255))); \
}

#endif /* ENDIAN_NEUTRAL */

#ifdef ENDIAN_LITTLE

#define STORE32H(x, y) { \
(y)[0] = (unsigned char)(((x)>>24)&255); \
(y)[1] = (unsigned char)(((x)>>16)&255); \
(y)[2] = (unsigned char)(((x)>>8)&255); \
(y)[3] = (unsigned char)((x)&255); \
}

#define LOAD32H(x, y) { \
x = ((unsigned long)((y)[0] & 255)<<24) | \
((unsigned long)((y)[1] & 255)<<16) | \
((unsigned long)((y)[2] & 255)<<8)  | \
((unsigned long)((y)[3] & 255)); \
}

#define STORE64H(x, y) { \
(y)[0] = (unsigned char)(((x)>>56)&255); \
(y)[1] = (unsigned char)(((x)>>48)&255); \
(y)[2] = (unsigned char)(((x)>>40)&255); \
(y)[3] = (unsigned char)(((x)>>32)&255); \
(y)[4] = (unsigned char)(((x)>>24)&255); \
(y)[5] = (unsigned char)(((x)>>16)&255); \
(y)[6] = (unsigned char)(((x)>>8)&255); \
(y)[7] = (unsigned char)((x)&255); \
}

#define LOAD64H(x, y) { \
x = (((uint64)((y)[0] & 255))<<56)|(((uint64)((y)[1] & 255))<<48) | \
(((uint64)((y)[2] & 255))<<40)|(((uint64)((y)[3] & 255))<<32) | \
(((uint64)((y)[4] & 255))<<24)|(((uint64)((y)[5] & 255))<<16) | \
(((uint64)((y)[6] & 255))<<8)|(((uint64)((y)[7] & 255))); }

#ifdef ENDIAN_32BITWORD 

#define STORE32L(x, y) { \
unsigned long __t = (x); memcpy(y, &__t, 4); \
}

#define LOAD32L(x, y)  memcpy(&(x), y, 4);

#define STORE64L(x, y) { \
(y)[7] = (unsigned char)(((x)>>56)&255); \
(y)[6] = (unsigned char)(((x)>>48)&255); \
(y)[5] = (unsigned char)(((x)>>40)&255); \
(y)[4] = (unsigned char)(((x)>>32)&255); \
(y)[3] = (unsigned char)(((x)>>24)&255); \
(y)[2] = (unsigned char)(((x)>>16)&255); \
(y)[1] = (unsigned char)(((x)>>8)&255); \
(y)[0] = (unsigned char)((x)&255); \
}

#define LOAD64L(x, y) { \
x = (((uint64)((y)[7] & 255))<<56)|(((uint64)((y)[6] & 255))<<48)| \
(((uint64)((y)[5] & 255))<<40)|(((uint64)((y)[4] & 255))<<32)| \
(((uint64)((y)[3] & 255))<<24)|(((uint64)((y)[2] & 255))<<16)| \
(((uint64)((y)[1] & 255))<<8)|(((uint64)((y)[0] & 255))); \
}

#else /* 64-bit words then  */

#define STORE32L(x, y) \
{ unsigned long __t = (x); memcpy(y, &__t, 4); }

#define LOAD32L(x, y) \
{ memcpy(&(x), y, 4); x &= 0xFFFFFFFF; }

#define STORE64L(x, y) \
{ uint64 __t = (x); memcpy(y, &__t, 8); }

#define LOAD64L(x, y) \
{ memcpy(&(x), y, 8); }

#endif /* ENDIAN_64BITWORD */
#endif /* ENDIAN_LITTLE */

#ifdef ENDIAN_BIG

#define STORE32L(x, y) { \
(y)[3] = (unsigned char)(((x)>>24)&255); \
(y)[2] = (unsigned char)(((x)>>16)&255); \
(y)[1] = (unsigned char)(((x)>>8)&255); \
(y)[0] = (unsigned char)((x)&255); \
}

#define LOAD32L(x, y) { \
x = ((unsigned long)((y)[3] & 255)<<24) | \
((unsigned long)((y)[2] & 255)<<16) | \
((unsigned long)((y)[1] & 255)<<8)  | \
((unsigned long)((y)[0] & 255)); \
}

#define STORE64L(x, y) { \
(y)[7] = (unsigned char)(((x)>>56)&255); \
(y)[6] = (unsigned char)(((x)>>48)&255); \
(y)[5] = (unsigned char)(((x)>>40)&255); \
(y)[4] = (unsigned char)(((x)>>32)&255); \
(y)[3] = (unsigned char)(((x)>>24)&255); \
(y)[2] = (unsigned char)(((x)>>16)&255); \
(y)[1] = (unsigned char)(((x)>>8)&255); \
(y)[0] = (unsigned char)((x)&255); \
}

#define LOAD64L(x, y) { \
x = (((uint64)((y)[7] & 255))<<56)|(((uint64)((y)[6] & 255))<<48) | \
(((uint64)((y)[5] & 255))<<40)|(((uint64)((y)[4] & 255))<<32) | \
(((uint64)((y)[3] & 255))<<24)|(((uint64)((y)[2] & 255))<<16) | \
(((uint64)((y)[1] & 255))<<8)|(((uint64)((y)[0] & 255))); \
}

#ifdef ENDIAN_32BITWORD 

#define STORE32H(x, y) \
{ unsigned long __t = (x); memcpy(y, &__t, 4); }

#define LOAD32H(x, y) memcpy(&(x), y, 4);

#define STORE64H(x, y) { \
(y)[0] = (unsigned char)(((x)>>56)&255); \
(y)[1] = (unsigned char)(((x)>>48)&255); \
(y)[2] = (unsigned char)(((x)>>40)&255); \
(y)[3] = (unsigned char)(((x)>>32)&255); \
(y)[4] = (unsigned char)(((x)>>24)&255); \
(y)[5] = (unsigned char)(((x)>>16)&255); \
(y)[6] = (unsigned char)(((x)>>8)&255); \
(y)[7] = (unsigned char)((x)&255); \
}

#define LOAD64H(x, y) { \
x = (((uint64)((y)[0] & 255))<<56)|(((uint64)((y)[1] & 255))<<48)| \
(((uint64)((y)[2] & 255))<<40)|(((uint64)((y)[3] & 255))<<32)| \
(((uint64)((y)[4] & 255))<<24)|(((uint64)((y)[5] & 255))<<16)| \
(((uint64)((y)[6] & 255))<<8)| (((uint64)((y)[7] & 255))); \
}

#else /* 64-bit words then  */

#define STORE32H(x, y) \
{ unsigned long __t = (x); memcpy(y, &__t, 4); }

#define LOAD32H(x, y) \
{ memcpy(&(x), y, 4); x &= 0xFFFFFFFF; }

#define STORE64H(x, y) \
{ uint64 __t = (x); memcpy(y, &__t, 8); }

#define LOAD64H(x, y) \
{ memcpy(&(x), y, 8); }

#endif /* ENDIAN_64BITWORD */
#endif /* ENDIAN_BIG */

#ifdef HAVE_NATIVE_INT64
#define ROL64c(x, y) \
( (((x)<<((uint64)(y)&63)) | \
(((x)&CONST64(0xFFFFFFFFFFFFFFFF))>>((uint64)64-((y)&63)))) & CONST64(0xFFFFFFFFFFFFFFFF))

#define ROR64c(x, y) \
( ((((x)&CONST64(0xFFFFFFFFFFFFFFFF))>>((uint64)(y)&CONST64(63))) | \
((x)<<((uint64)(64-((y)&CONST64(63)))))) & CONST64(0xFFFFFFFFFFFFFFFF))
#endif /* HAVE_NATIVE_INT64 */	  
/******************************************************************************/


/******************************************************************************/
/*
	Return the length of padding bytes required for a record of 'LEN' bytes
	The name Pwr2 indicates that calculations will work with 'BLOCKSIZE'
	that are powers of 2.
	Because of the trailing pad length byte, a length that is a multiple
	of the pad bytes
*/
#define psPadLenPwr2(LEN, BLOCKSIZE) \
	BLOCKSIZE <= 1 ? (unsigned char)0 : \
	(unsigned char)(BLOCKSIZE - ((LEN) & (BLOCKSIZE - 1)))


#endif /* _h_PS_CRYPTOLIB */
/******************************************************************************/

