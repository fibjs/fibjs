/*
 *	matrixssllib.h
 *	Release $Name: MATRIXSSL-3-4-2-OPEN $
 *
 *	Internal header file used for the MatrixSSL implementation.
 *	Only modifiers of the library should be intersted in this file
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

#ifndef _h_MATRIXSSLLIB
#define _h_MATRIXSSLLIB

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************/
/*
	Start with compile-time checks for the necessary proto and crypto support.
*/ 
#if !defined(USE_TLS) && defined(DISABLE_SSLV3)
#error "Must enable a protocol: USE_TLS enabled or DISABLE_SSLV3 disabled"
#endif


#if defined(USE_TLS_1_1) && !defined(USE_TLS)
#error "Must define USE_TLS if defining USE_TLS_1_1"
#endif

#ifdef USE_TLS
#if !defined(USE_TLS_1_2) && defined(DISABLE_TLS_1_0) && defined(DISABLE_TLS_1_1) && defined(DISABLE_SSLV3)
#error "Bad combination of USE_TLS and DISABLE_TLS"
#endif
#endif

/******************************************************************************/
/*
	SHA1 and MD5 are essential elements for SSL key derivation during protocol
*/
#if !defined USE_MD5 || !defined USE_SHA1
#error "Must enable both USE_MD5 and USE_SHA1 in cryptoConfig.h for MatrixSSL"
#endif 

#if !defined USE_CLIENT_SIDE_SSL && !defined USE_SERVER_SIDE_SSL
#error "Must enable either USE_CLIENT_SIDE_SSL or USE_SERVER_SIDE_SSL (or both)"
#endif

#ifndef USE_CERT_PARSE
#ifdef USE_CLIENT_SIDE_SSL
#error "Must enable USE_CERT_PARSE if building client with USE_CLIENT_SIDE_SSL"
#endif
#ifdef USE_CLIENT_AUTH
#error "Must enable USE_CERT_PARSE if client auth (USE_CLIENT_AUTH) is needed"
#endif
#endif

/*
	X.509 is required for all configurations of SSL below
*/
#ifndef USE_X509
#error "Must enable USE_X509 in cryptoConfig.h for MatrixSSL"
#endif

#ifdef USE_TLS
	#ifndef USE_HMAC
	#error "Must enable USE_HMAC in cryptoConfig.h for TLS protocol support"
	#endif
#endif

/*
	Handle the various combos of REHANDSHAKES defines
*/
#if defined(ENABLE_INSECURE_REHANDSHAKES) && defined(REQUIRE_SECURE_REHANDSHAKES)
#error "Can't enable both ENABLE_INSECURE_REHANDSHAKES and REQUIRE_SECURE_REHANDSHAKES"
#endif

#if defined(ENABLE_INSECURE_REHANDSHAKES) || defined(ENABLE_SECURE_REHANDSHAKES)
#define SSL_REHANDSHAKES_ENABLED
#endif

#if defined(REQUIRE_SECURE_REHANDSHAKES) && !defined(ENABLE_SECURE_REHANDSHAKES)
#define SSL_REHANDSHAKES_ENABLED
#define ENABLE_SECURE_REHANDSHAKES
#endif

/******************************************************************************/
/*
	Test specific crypto features based on which cipher suites are enabled 
*/
#ifdef USE_SSL_RSA_WITH_NULL_MD5
	#define USE_MD5_MAC
	#define USE_RSA_CIPHER_SUITE
#endif

#ifdef USE_SSL_RSA_WITH_NULL_SHA
	#define USE_SHA_MAC
	#define USE_RSA_CIPHER_SUITE
#endif

#ifdef USE_SSL_RSA_WITH_RC4_128_SHA
	#ifndef USE_ARC4
	#error "Enable USE_ARC4 in cryptoConfig.h for SSL_RSA_WITH_RC4_128_SHA suite"
	#endif
	#define USE_SHA_MAC
	#define USE_RSA_CIPHER_SUITE
	#define USE_ARC4_CIPHER_SUITE
#endif

#ifdef USE_SSL_RSA_WITH_RC4_128_MD5
	#ifndef USE_ARC4
	#error "Enable USE_ARC4 in cryptoConfig.h for SSL_RSA_WITH_RC4_128_MD5 suite"
	#endif
	#define USE_MD5_MAC
	#define USE_RSA_CIPHER_SUITE
	#define USE_ARC4_CIPHER_SUITE
#endif

#ifdef USE_SSL_RSA_WITH_3DES_EDE_CBC_SHA
	#ifndef USE_3DES
	#error "Enable USE_3DES in cryptoConfig.h for SSL_RSA_WITH_3DES_EDE_CBC_SHA"
	#endif
	#define USE_SHA_MAC
	#define USE_RSA_CIPHER_SUITE
	#define USE_3DES_CIPHER_SUITE
#endif

#ifdef USE_TLS_RSA_WITH_AES_128_CBC_SHA
	#ifndef USE_AES
	#error "Enable USE_AES in cryptoConfig.h for TLS_RSA_WITH_AES_128_CBC_SHA"
	#endif
	#define USE_SHA_MAC
	#define USE_AES_CIPHER_SUITE
	#define USE_RSA_CIPHER_SUITE
#endif

#ifdef USE_TLS_RSA_WITH_AES_256_CBC_SHA
	#ifndef USE_AES
	#error "Enable USE_AES in cryptoConfig.h for TLS_RSA_WITH_AES_256_CBC_SHA"
	#endif
	#define USE_SHA_MAC
	#define USE_AES_CIPHER_SUITE
	#define USE_RSA_CIPHER_SUITE
#endif

/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
/*
	Leave this enabled for run-time check of sslKeys_t content when a cipher
	suite is matched.  Disable only if you need to manage key material yourself.
	Always conditional on whether certificate parsing is enabled because it
	looks at members that only exist if certificates have been parsed 
*/
#ifdef USE_CERT_PARSE
#define VALIDATE_KEY_MATERIAL
#endif /* USE_CERT_PARSE */
/******************************************************************************/

/******************************************************************************/
/*	SSL protocol and MatrixSSL defines */
/******************************************************************************/
/*
	Maximum SSL record size, per specification
*/
#define     SSL_MAX_PLAINTEXT_LEN		0x4000  /* 16KB */
#define     SSL_MAX_RECORD_LEN			SSL_MAX_PLAINTEXT_LEN + 2048
#define     SSL_MAX_BUF_SIZE			SSL_MAX_RECORD_LEN + 0x5
#define		SSL_MAX_DISABLED_CIPHERS	8
/*
	Maximum buffer sizes for static SSL array types 
*/
#define SSL_MAX_MAC_SIZE		48 /* SHA384 */
#define SSL_MAX_IV_SIZE			16
#define SSL_MAX_BLOCK_SIZE		16
#define SSL_MAX_SYM_KEY_SIZE	32

/*
	Negative return codes must be between -50 and -69 in the MatrixSSL module
*/
#define     SSL_FULL            -50  /* must call sslRead before decoding */
#define     SSL_PARTIAL         -51 /* more data reqired to parse full msg */
#define     SSL_SEND_RESPONSE   -52  /* decode produced output data */
#define     SSL_PROCESS_DATA    -53  /* succesfully decoded application data */
#define     SSL_ALERT           -54  /* we've decoded an alert */
#define     SSL_FILE_NOT_FOUND  -55  /* File not found */
#define     SSL_MEM_ERROR       PS_MEM_FAIL  /* Memory allocation failure */

/*
	Magic numbers for handshake header lengths
*/
#define SSL2_HEADER_LEN				2
#define SSL3_HEADER_LEN				5
#define SSL3_HANDSHAKE_HEADER_LEN	4
#define TLS_GCM_TAG_LEN				16
#define TLS_GCM_NONCE_LEN			8

/*
	matrixSslSetSessionOption defines
*/
#define	SSL_OPTION_FULL_HANDSHAKE		1
#ifdef USE_CLIENT_AUTH
#define	SSL_OPTION_DISABLE_CLIENT_AUTH	2
#define	SSL_OPTION_ENABLE_CLIENT_AUTH	3
#endif /* USE_CLIENT_AUTH */

/*
    SSL Alert levels and descriptions
    This implementation treats all alerts that are not related to 
	certificate validation as fatal
*/
#define SSL_ALERT_LEVEL_WARNING             1
#define SSL_ALERT_LEVEL_FATAL               2

#define SSL_ALERT_CLOSE_NOTIFY              0
#define SSL_ALERT_UNEXPECTED_MESSAGE        10
#define SSL_ALERT_BAD_RECORD_MAC            20
#define SSL_ALERT_DECRYPTION_FAILED			21
#define SSL_ALERT_RECORD_OVERFLOW			22
#define SSL_ALERT_DECOMPRESSION_FAILURE     30
#define SSL_ALERT_HANDSHAKE_FAILURE         40
#define SSL_ALERT_NO_CERTIFICATE            41
#define SSL_ALERT_BAD_CERTIFICATE           42
#define SSL_ALERT_UNSUPPORTED_CERTIFICATE   43
#define SSL_ALERT_CERTIFICATE_REVOKED       44
#define SSL_ALERT_CERTIFICATE_EXPIRED       45
#define SSL_ALERT_CERTIFICATE_UNKNOWN       46
#define SSL_ALERT_ILLEGAL_PARAMETER         47
#define SSL_ALERT_UNKNOWN_CA				48
#define SSL_ALERT_ACCESS_DENIED				49
#define SSL_ALERT_DECODE_ERROR				50
#define SSL_ALERT_DECRYPT_ERROR				51
#define SSL_ALERT_PROTOCOL_VERSION			70
#define SSL_ALERT_INSUFFICIENT_SECURITY		71
#define SSL_ALERT_INTERNAL_ERROR			80
#define SSL_ALERT_NO_RENEGOTIATION			100
#define SSL_ALERT_UNSUPPORTED_EXTENSION		110

/*
    Use as return code in user validation callback to allow
    anonymous connections to proceed.
	MUST NOT OVERLAP WITH ANY OF THE ALERT CODES ABOVE
*/
#define SSL_ALLOW_ANON_CONNECTION           254

/*
	Internal flags for ssl_t.flags field.
*/
#define	SSL_FLAGS_SERVER		0x00000001
#define	SSL_FLAGS_READ_SECURE	0x00000002
#define	SSL_FLAGS_WRITE_SECURE	0x00000004
#define SSL_FLAGS_RESUMED		0x00000008
#define SSL_FLAGS_CLOSED		0x00000010
#define SSL_FLAGS_NEED_ENCODE	0x00000020
#define SSL_FLAGS_ERROR			0x00000040
#define SSL_FLAGS_TLS			0x00000080
#define SSL_FLAGS_CLIENT_AUTH	0x00000100
#define SSL_FLAGS_ANON_CIPHER	0x00000200
#define SSL_FLAGS_FALSE_START	0x00000400
#define SSL_FLAGS_TLS_1_1		0x00000800
#define SSL_FLAGS_TLS_1_2		0x00001000
	
/*
	Buffer flags (ssl->bFlags)
*/
#define BFLAG_CLOSE_AFTER_SENT	0x01
#define BFLAG_HS_COMPLETE		0x02
#define BFLAG_STOP_BEAST		0x04 
	
/*
	Number of bytes server must send before creating a re-handshake credit
*/
#define DEFAULT_RH_CREDITS		1 /* Allow for one rehandshake by default */
#define	BYTES_BEFORE_RH_CREDIT	20 * 1024 * 1024

/*
	Cipher types
*/
#define CS_NULL			0
#define CS_RSA			1
#define CS_DHE_RSA		2
#define CS_DH_ANON		3
#define CS_DHE_PSK		4
#define CS_PSK			5
#define	CS_ECDHE_ECDSA	6
#define CS_ECDHE_RSA	7
#define	CS_ECDH_ECDSA	8
#define CS_ECDH_RSA		9

/*
	These are defines rather than enums because we want to store them as char,
	not int32 (enum size)
*/
#define SSL_RECORD_TYPE_CHANGE_CIPHER_SPEC		20
#define SSL_RECORD_TYPE_ALERT					21
#define SSL_RECORD_TYPE_HANDSHAKE				22
#define SSL_RECORD_TYPE_APPLICATION_DATA		23
#define SSL_RECORD_TYPE_HANDSHAKE_FIRST_FRAG	90 /*    internal */
#define SSL_RECORD_TYPE_HANDSHAKE_FRAG			91 /* non-standard types */

#define SSL_HS_HELLO_REQUEST		0
#define SSL_HS_CLIENT_HELLO			1
#define SSL_HS_SERVER_HELLO			2
#define SSL_HS_HELLO_VERIFY_REQUEST	3
#define SSL_HS_NEW_SESSION_TICKET	4
#define SSL_HS_CERTIFICATE			11
#define SSL_HS_SERVER_KEY_EXCHANGE	12
#define SSL_HS_CERTIFICATE_REQUEST	13
#define SSL_HS_SERVER_HELLO_DONE	14
#define SSL_HS_CERTIFICATE_VERIFY	15
#define SSL_HS_CLIENT_KEY_EXCHANGE	16
#define SSL_HS_FINISHED				20
#define SSL_HS_DONE					255	/* Handshake complete (internal) */

#define	INIT_ENCRYPT_CIPHER		0
#define INIT_DECRYPT_CIPHER		1

#define HMAC_CREATE	1
#define HMAC_VERIFY 2



/*
	Additional ssl alert value, indicating no error has ocurred.
*/
#define SSL_ALERT_NONE					255	/* No error */

#define SSL_HS_RANDOM_SIZE			32
#define SSL_HS_RSA_PREMASTER_SIZE	48

#define SSL2_MAJ_VER	2
#define SSL3_MAJ_VER	3
#define SSL3_MIN_VER	0
#define TLS_MIN_VER		1
#define TLS_1_1_MIN_VER	2
#define TLS_1_2_MIN_VER	3

#ifdef USE_TLS
#define TLS_HS_FINISHED_SIZE	12
#define TLS_MAJ_VER		3
#endif /* USE_TLS */

/*
	SSL cipher suite specification IDs
*/
#define SSL_NULL_WITH_NULL_NULL				0x0000
#define SSL_RSA_WITH_NULL_MD5				0x0001
#define SSL_RSA_WITH_NULL_SHA				0x0002
#define SSL_RSA_WITH_RC4_128_MD5			0x0004
#define SSL_RSA_WITH_RC4_128_SHA			0x0005		
#define SSL_RSA_WITH_3DES_EDE_CBC_SHA		0x000A		/* 10 */
#define TLS_RSA_WITH_AES_128_CBC_SHA		0x002F		/* 47 */
#define TLS_RSA_WITH_AES_256_CBC_SHA		0x0035		/* 53 */
#define TLS_EMPTY_RENEGOTIATION_INFO_SCSV	0x00FF


/*
	Supported HELLO extensions
*/
#define EXT_MAX_FRAGMENT_LEN			0x01
#define EXT_RENEGOTIATION_INFO			0xFF01
#define EXT_SIGNATURE_ALGORITHMS		0x00D

/*
	Maximum key block size for any defined cipher
	This must be validated if new ciphers are added
	Value is largest total among all cipher suites for
		2*macSize + 2*keySize + 2*ivSize
*/
#define SSL_MAX_KEY_BLOCK_SIZE			2*48 + 2*32 + 2*16 + SHA1_HASH_SIZE

/*
	Master secret is 48 bytes, sessionId is 32 bytes max
*/
#define		SSL_HS_MASTER_SIZE		48
#define		SSL_MAX_SESSION_ID_SIZE	32



#ifndef USE_SSL_HANDSHAKE_MSG_TRACE
#define psTraceHs(x) 
#define psTraceStrHs(x, y) 
#else
#define psTraceHs(x) _psTrace(x)
#define psTraceStrHs(x, y) _psTraceStr(x, y)
#endif /* USE_SSL_HANDSHAKE_MSG_TRACE */

#ifndef USE_SSL_INFORMATIONAL_TRACE
#define psTraceInfo(x) 
#define psTraceStrInfo(x, y) 
#define psTraceIntInfo(x, y)
#else
#define psTraceInfo(x) _psTrace(x)
#define psTraceStrInfo(x, y) _psTraceStr(x, y)
#define psTraceIntInfo(x, y) _psTraceInt(x, y)
#endif /* USE_SSL_INFORMATIONAL_TRACE */

/******************************************************************************/

typedef psBuf_t	sslBuf_t;
	
/******************************************************************************/	

/******************************************************************************/
/*
	SSL certificate public-key structure
*/
typedef struct {
	psPool_t		*pool;
#ifdef USE_SERVER_SIDE_SSL
	psX509Cert_t	*cert;	
	psPubKey_t		*privKey;
#endif /* USE_SERVER_SIDE_SSL */
#ifdef USE_CLIENT_SIDE_SSL
	psX509Cert_t	*CAcerts;
#endif /* USE_CLIENT_SIDE_SSL */
} sslKeys_t;

/******************************************************************************/

/******************************************************************************/
/*
	SSL record and session structures
*/
typedef struct {
	unsigned short	len;
	unsigned char	majVer;
	unsigned char	minVer;
#ifdef USE_CERT_CHAIN_PARSING
	unsigned short	hsBytesHashed;
	unsigned short	hsBytesParsed;
	unsigned short	trueLen;
	unsigned char	partial;
	unsigned char	certPad;
#endif
	unsigned char	type;
	unsigned char	pad[3];		/* Padding for 64 bit compat */
} sslRec_t;

typedef struct {
	unsigned char	clientRandom[SSL_HS_RANDOM_SIZE];	/* From ClientHello */
	unsigned char	serverRandom[SSL_HS_RANDOM_SIZE];	/* From ServerHello */
	unsigned char	masterSecret[SSL_HS_MASTER_SIZE];
	unsigned char	*premaster;							/* variable size */
	uint32			premasterSize;

	unsigned char	keyBlock[SSL_MAX_KEY_BLOCK_SIZE];	/* Storage for 'ptr' */
	unsigned char	*wMACptr;
	unsigned char	*rMACptr;
	unsigned char	*wKeyptr;
	unsigned char	*rKeyptr;
	
	/*	All maximum sizes for current cipher suites */
	unsigned char	writeMAC[SSL_MAX_MAC_SIZE];
	unsigned char	readMAC[SSL_MAX_MAC_SIZE];
	unsigned char	writeKey[SSL_MAX_SYM_KEY_SIZE];
	unsigned char	readKey[SSL_MAX_SYM_KEY_SIZE];
	unsigned char	*wIVptr;
	unsigned char	*rIVptr;
	unsigned char	writeIV[SSL_MAX_IV_SIZE];
	unsigned char	readIV[SSL_MAX_IV_SIZE];

	unsigned char	seq[8];
	unsigned char	remSeq[8];

#ifdef USE_CLIENT_SIDE_SSL
	psX509Cert_t	*cert;
	int32 (*validateCert)(void *ssl, psX509Cert_t *certInfo, int32 alert);
#endif /* USE_CLIENT_SIDE_SSL */

#ifdef USE_CLIENT_SIDE_SSL
	int32				certMatch;
#endif /* USE_CLIENT_SIDE_SSL */

	psDigestContext_t	msgHashMd5;
	psDigestContext_t	msgHashSha1;
	psCipherContext_t	encryptCtx;
	psCipherContext_t	decryptCtx;


	int32				anon;
} sslSec_t;

typedef struct {
	uint16			ident;	/* Official cipher ID */
	uint16			type;	/* Key exchange method */
	uint32			flags;	/* from CRYPTO_FLAGS_* */
	unsigned char	macSize;
	unsigned char	keySize;
	unsigned char	ivSize;
	unsigned char	blockSize;
	/* Init function */
	int32 (*init)(sslSec_t *sec, int32 type, uint32 keysize);
	/* Cipher functions */
	int32 (*encrypt)(void *ssl, unsigned char *in,
		unsigned char *out, uint32 len);
	int32 (*decrypt)(void *ssl, unsigned char *in,
		unsigned char *out, uint32 len);
	int32 (*generateMac)(void *ssl, unsigned char type, unsigned char *data,
		uint32 len, unsigned char *mac);
	int32 (*verifyMac)(void *ssl, unsigned char type, unsigned char *data,
		uint32 len, unsigned char *mac);
} sslCipherSpec_t;


typedef struct {
	unsigned char	id[SSL_MAX_SESSION_ID_SIZE];
	unsigned char	masterSecret[SSL_HS_MASTER_SIZE];
	uint32			cipherId;
} sslSessionId_t;

typedef struct {
	unsigned char	id[SSL_MAX_SESSION_ID_SIZE];
	unsigned char	masterSecret[SSL_HS_MASTER_SIZE];
	sslCipherSpec_t	*cipher;
	unsigned char	majVer;
	unsigned char	minVer;
	psTime_t		startTime;
	psTime_t		accessTime;
	int32			inUse;
} sslSessionEntry_t;

typedef struct tlsHelloExt {
	psPool_t			*pool;
	int32				extType;
	uint32				extLen;
	unsigned char		*extData;
	struct tlsHelloExt	*next;
} tlsExtension_t;
	

typedef struct ssl {
	sslRec_t		rec;			/* Current SSL record information*/
									
	sslSec_t		sec;			/* Security structure */

	sslKeys_t		*keys;			/* SSL public and private keys */

	psPool_t		*sPool;			/* SSL session pool */
	psPool_t		*hsPool;		/* Full session handshake pool */

	unsigned char	sessionIdLen;
	char			sessionId[SSL_MAX_SESSION_ID_SIZE];
	sslSessionId_t	*sid;
#ifdef USE_SERVER_SIDE_SSL	
	uint16			disabledCiphers[SSL_MAX_DISABLED_CIPHERS];
#endif /* USE_SERVER_SIDE_SSL */	
	
	unsigned char	*inbuf;
	unsigned char	*outbuf;
	int32			inlen;		/* Bytes unprocessed in inbuf */
	int32			outlen;		/* Bytes unsent in outbuf */
	int32			insize;		/* Total allocated size of inbuf */
	int32			outsize;	/* Total allocated size of outbuf */
	uint32			bFlags;		/* Buffer related flags */
	
	int32			maxPtFrag;	/* 16K by default - SSL_MAX_PLAINTEXT_LEN */
	unsigned char	*fragMessage; /* holds the constructed fragmented message */
	uint32			fragIndex;	/* How much data has been written to msg */
	uint32			fragTotal;	/* Total length of fragmented message */
		
	/* Pointer to the negotiated cipher information */
	sslCipherSpec_t	*cipher;
	
	/* 	Symmetric cipher callbacks

		We duplicate these here from 'cipher' because we need to set the
		various callbacks at different times in the handshake protocol
		Also, there are 64 bit alignment issues in using the function pointers
		within 'cipher' directly
	*/
	int32 (*encrypt)(void *ctx, unsigned char *in,
		unsigned char *out, uint32 len);
	int32 (*decrypt)(void *ctx, unsigned char *in,
		unsigned char *out, uint32 len);
	/* Message Authentication Codes */
	int32 (*generateMac)(void *ssl, unsigned char type, unsigned char *data,
		uint32 len, unsigned char *mac);
	int32 (*verifyMac)(void *ssl, unsigned char type, unsigned char *data,
		uint32 len, unsigned char *mac);

	/* Current encryption/decryption parameters */
	unsigned char	enMacSize;
	unsigned char	enIvSize;
	unsigned char	enBlockSize;
	unsigned char	deMacSize;
	unsigned char	deIvSize;
	unsigned char	deBlockSize;

	int32			flags;
	int32			hsState;		/* Next expected handshake message type */
	int32			err;			/* SSL errno of last api call */
	int32			ignoredMessageCount;

	unsigned char	reqMajVer;
	unsigned char	reqMinVer;
	unsigned char	majVer;
	unsigned char	minVer;
	unsigned char	outRecType;

#ifdef ENABLE_SECURE_REHANDSHAKES		
	unsigned char	myVerifyData[SHA384_HASH_SIZE]; /*SSLv3 max*/
	unsigned char	peerVerifyData[SHA384_HASH_SIZE];
	uint32			myVerifyDataLen;
	uint32			peerVerifyDataLen;
	int32			secureRenegotiationFlag;
#endif /* ENABLE_SECURE_REHANDSHAKES */
#ifdef SSL_REHANDSHAKES_ENABLED
	int32			rehandshakeCount; /* Make this an internal define of 1 */
	int32			rehandshakeBytes; /* Make this an internal define of 10MB */
#endif /* SSL_REHANDSHAKES_ENABLED */	
	int32			(*extCb)(void *ssl, unsigned short extType,
						unsigned short extLen, void *e);
	int32			recordHeadLen;
	int32			hshakeHeadLen;
} ssl_t;

/******************************************************************************/
/*
	Former public APIS in 1.x and 2.x. Now deprecated in 3.x
	These functions are still heavily used internally, just no longer publically
	supported.
 */
extern int32 matrixSslDecode(ssl_t *ssl, unsigned char **buf, uint32 *len,
						uint32 size, uint32 *remaining, uint32 *requiredLen,
						int32 *error, unsigned char *alertLevel,
						unsigned char *alertDescription);
extern int32 matrixSslEncode(ssl_t *ssl, unsigned char *buf, uint32 size,
						unsigned char *ptBuf, uint32 *len);
extern int32	matrixSslGetEncodedSize(ssl_t *ssl, uint32 len);
extern void		matrixSslSetCertValidator(ssl_t *ssl,
						int32 (*certValidator)(void *, psX509Cert_t *, int32));
extern int32	matrixSslNewSession(ssl_t **ssl, sslKeys_t *keys,
						sslSessionId_t *session, int32 flags);
extern void		matrixSslSetSessionOption(ssl_t *ssl, int32 option,	void *arg);
extern int32	matrixSslHandshakeIsComplete(ssl_t *ssl);
typedef int32	(*sslExtCb_t)(void *, unsigned short, unsigned short, void *);

/* This used to be prefixed with 'matrix' */
extern int32	sslEncodeClosureAlert(ssl_t *ssl, sslBuf_t *out, 
									  uint32 *reqLen);

extern int32	matrixSslEncodeHelloRequest(ssl_t *ssl, sslBuf_t *out,
					uint32 *reqLen);
extern int32	matrixSslEncodeClientHello(ssl_t *ssl, sslBuf_t *out,
					uint32 cipherSpec, uint32 *requiredLen,
					tlsExtension_t *userExt);

#ifdef USE_CLIENT_SIDE_SSL
extern int32	matrixSslGetSessionId(ssl_t *ssl, sslSessionId_t *sessionId);
#endif /* USE_CLIENT_SIDE_SSL */

extern int32 matrixSslGetPrngData(unsigned char *bytes, uint32 size);

#ifdef USE_SSL_INFORMATIONAL_TRACE
extern void matrixSslPrintHSDetails(ssl_t *ssl);
#endif /* USE_SSL_INFORMATIONAL_TRACE */

#ifdef SSL_REHANDSHAKES_ENABLED
PSPUBLIC void matrixSslAddRehandshakeCredits(ssl_t *ssl, int32 credits);
#endif

/******************************************************************************/
/*
	MatrixSSL internal cert functions
*/
typedef int32 (*sslCertCb_t)(void *, psX509Cert_t *, int32);
extern int32 matrixValidateCerts(psPool_t *pool, psX509Cert_t *subjectCerts,
				psX509Cert_t *issuerCerts);
extern int32 matrixUserCertValidator(ssl_t *ssl, int32 alert, 
				 psX509Cert_t *subjectCert, sslCertCb_t certCb);

/******************************************************************************/
/*
	sslEncode.c and sslDecode.c
*/
extern int32 psWriteRecordInfo(ssl_t *ssl, unsigned char type, int32 len,
							 unsigned char *c);
extern int32 psWriteHandshakeHeader(ssl_t *ssl, unsigned char type, int32 len, 
								int32 seq, int32 fragOffset, int32 fragLen,
								unsigned char *c);
extern int32 sslEncodeResponse(ssl_t *ssl, psBuf_t *out, uint32 *requiredLen);
extern int32 sslActivateReadCipher(ssl_t *ssl);
extern int32 sslActivateWriteCipher(ssl_t *ssl);
extern int32 sslUpdateHSHash(ssl_t *ssl, unsigned char *in, uint32 len);
extern int32 sslInitHSHash(ssl_t *ssl);
extern int32 sslSnapshotHSHash(ssl_t *ssl, unsigned char *out, int32 senderFlag);
extern int32 sslWritePad(unsigned char *p, unsigned char padLen);
extern int32 sslCreateKeys(ssl_t *ssl);
extern void sslResetContext(ssl_t *ssl);

#ifdef USE_SERVER_SIDE_SSL
extern int32 matrixRegisterSession(ssl_t *ssl);
extern int32 matrixResumeSession(ssl_t *ssl);
extern int32 matrixClearSession(ssl_t *ssl, int32 remove);
extern int32 matrixUpdateSession(ssl_t *ssl);
#endif /* USE_SERVER_SIDE_SSL */



/*
	cipherSuite.c
*/
extern sslCipherSpec_t *sslGetCipherSpec(ssl_t *ssl, uint32 cid);
extern int32 sslGetCipherSpecListLen(ssl_t *ssl);
extern int32 sslGetCipherSpecList(ssl_t *ssl, unsigned char *c, int32 len,
				int32 addScsv);
extern int32 csRsaEncryptPub(psPool_t *pool, psPubKey_t *key, 
			unsigned char *in, uint32 inlen, unsigned char *out, uint32 outlen,
			void *data);
extern int32 csRsaDecryptPub(psPool_t *pool, psPubKey_t *key, 
			unsigned char *in, uint32 inlen, unsigned char *out, uint32 outlen,
			void *data);
extern int32 csRsaEncryptPriv(psPool_t *pool, psPubKey_t *key, 
			unsigned char *in, uint32 inlen, unsigned char *out, uint32 outlen,
			void *data);
extern int32 csRsaDecryptPriv(psPool_t *pool, psPubKey_t *key, 
			unsigned char *in, uint32 inlen, unsigned char *out, uint32 outlen,
			void *data);			
#ifdef USE_CLIENT_SIDE_SSL
int32 csCheckCertAgainstCipherSuite(int32 sigAlg, int32 cipherType);
#endif				 						

#ifndef DISABLE_SSLV3
/******************************************************************************/
/*
	sslv3.c
*/
extern int32 sslGenerateFinishedHash(psDigestContext_t *md5,
				psDigestContext_t *sha1, unsigned char *masterSecret,
				unsigned char *out, int32 sender);

extern int32 sslDeriveKeys(ssl_t *ssl);

#ifdef USE_SHA_MAC
extern int32 ssl3HMACSha1(unsigned char *key, unsigned char *seq, 
						unsigned char type, unsigned char *data, uint32 len,
						unsigned char *mac);
#endif /* USE_SHA_MAC */

#ifdef USE_MD5_MAC
extern int32 ssl3HMACMd5(unsigned char *key, unsigned char *seq, 
						unsigned char type, unsigned char *data, uint32 len,
						unsigned char *mac);
#endif /* USE_MD5_MAC */
#endif /* DISABLE_SSLV3 */

#ifdef USE_TLS
/******************************************************************************/
/*
	tls.c
*/
extern int32 tlsDeriveKeys(ssl_t *ssl);

extern int32 tlsHMACSha1(ssl_t *ssl, int32 mode, unsigned char type,	
						unsigned char *data, uint32 len, unsigned char *mac);

extern int32 tlsHMACMd5(ssl_t *ssl, int32 mode, unsigned char type,	
						unsigned char *data, uint32 len, unsigned char *mac);
extern int32 prf(unsigned char *sec, uint32 secLen, unsigned char *seed,
			   uint32 seedLen, unsigned char *out, uint32 outLen);
#endif /* USE_TLS */


#ifdef USE_AES_CIPHER_SUITE
extern int32 csAesInit(sslSec_t *sec, int32 type, uint32 keysize);
extern int32 csAesEncrypt(void *ssl, unsigned char *pt,
					 unsigned char *ct, uint32 len);
extern int32 csAesDecrypt(void *ssl, unsigned char *ct,
					 unsigned char *pt, uint32 len);
#endif /* USE_AES_CIPHER_SUITE */
#ifdef USE_3DES_CIPHER_SUITE
extern int32 csDes3Encrypt(void *ssl, unsigned char *pt,
					 unsigned char *ct, uint32 len);
extern int32 csDes3Decrypt(void *ssl, unsigned char *ct,
					 unsigned char *pt, uint32 len);
#endif /* USE_3DES_CIPHER_SUITE */
#ifdef USE_ARC4_CIPHER_SUITE
extern int32 csArc4Encrypt(void *ssl, unsigned char *pt,unsigned char *ct,
					uint32 len);
extern int32 csArc4Decrypt(void *ssl, unsigned char *pt,unsigned char *ct,
					uint32 len);
#endif /* USE_ARC4_CIPHER_SUITE */
#ifdef USE_SEED_CIPHER_SUITE
extern int32 csSeedEncrypt(void *ssl, unsigned char *pt,
					 unsigned char *ct, uint32 len);
extern int32 csSeedDecrypt(void *ssl, unsigned char *ct,
					 unsigned char *pt, uint32 len);
#endif /* USE_SEED_CIPHER_SUITE */






/******************************************************************************/


#ifdef __cplusplus
}
#endif

#endif /* _h_MATRIXSSLLIB */

/******************************************************************************/

