/*
 *	x509.c
 *	Release $Name: MATRIXSSL-3-4-2-OPEN $
 *
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
#ifdef USE_X509
/******************************************************************************/

#define MAX_CERTS_PER_FILE		16

#ifdef USE_CERT_PARSE
/*
	Certificate extensions
*/
#define EXT_BASIC_CONSTRAINTS	1
#define	EXT_KEY_USAGE			2
#define EXT_SUBJ_KEY_ID			3
#define EXT_AUTH_KEY_ID			4
#define EXT_ALT_SUBJECT_NAME	5
#define EXT_CRL_DIST_PTS		6
#define EXT_AUTH_INFO_ACC		7

#define IMPLICIT_ISSUER_ID		1
#define IMPLICIT_SUBJECT_ID		2
#define EXPLICIT_EXTENSION		3

/*
	Distinguished Name attributes
*/
#define ATTRIB_COUNTRY_NAME		6
#define ATTRIB_LOCALITY			7
#define ATTRIB_ORGANIZATION		10
#define ATTRIB_ORG_UNIT			11
#define ATTRIB_DN_QUALIFIER		46
#define ATTRIB_STATE_PROVINCE	8
#define ATTRIB_COMMON_NAME		3

static const struct {
		unsigned char	hash[16];
		int32			id;
} extTable[] = {
	{ { 0xa5, 0xc4, 0x5e, 0x9a, 0xa3, 0xbb, 0x71, 0x2f, 0x07,
		0xf7, 0x4c, 0xd0, 0xcd, 0x95, 0x65, 0xda }, EXT_BASIC_CONSTRAINTS },
	{ { 0xf5, 0xab, 0x88, 0x49, 0xc4, 0xfd, 0xa2, 0x64, 0x6d,
		0x06, 0xa2, 0x3e, 0x83, 0x9b, 0xef, 0xbb }, EXT_KEY_USAGE },
	{ { 0x91, 0x54, 0x28, 0xcc, 0x81, 0x59, 0x8c, 0x71, 0x8c,
		0x53, 0xa8, 0x4d, 0xeb, 0xd3, 0xc2, 0x18 }, EXT_SUBJ_KEY_ID },
	{ { 0x48, 0x2d, 0xff, 0x49, 0xf7, 0xab, 0x93, 0xe8, 0x1f,
		0x57, 0xb5, 0xaf, 0x7f, 0xaa, 0x31, 0xbb }, EXT_AUTH_KEY_ID },
	{ { 0x5c, 0x70, 0xcb, 0xf5, 0xa4, 0x07, 0x5a, 0xcc, 0xd1,
		0x55, 0xd2, 0x44, 0xdd, 0x62, 0x2c, 0x0c }, EXT_ALT_SUBJECT_NAME },
	{ { 0xec, 0x90, 0xac, 0x73, 0xc4, 0x94, 0x66, 0x8d, 0xb0,
		0x21, 0xd0, 0xe7, 0x5c, 0x55, 0xae, 0x33 }, EXT_CRL_DIST_PTS },
	{ { 0xba, 0x71, 0x0c, 0xec, 0x2b, 0x68, 0xf7, 0xbf, 0x08,
		0x3d, 0x28, 0xf3, 0xb3, 0x12, 0xc3, 0xcb }, EXT_AUTH_INFO_ACC },
	{ { 0 }, -1 } /* Must be last for proper termination */
};

/*
	Hybrid ASN.1/X.509 cert parsing helpers
*/
static int32 getExplicitExtensions(psPool_t *pool, unsigned char **pp, 
								 uint32 len, int32 expVal,
								 x509v3extensions_t *extensions);
static int32 getSerialNum(psPool_t *pool, unsigned char **pp, uint32 len,
                        unsigned char **sn, uint32 *snLen);
static int32 getExplicitVersion(unsigned char **pp, uint32 len, int32 expVal,
						int32 *val);
static int32 getValidity(psPool_t *pool, unsigned char **pp, uint32 len,
				int32 *notBeforeTimeType, int32 *notAfterTimeType,
				char **notBefore, char **notAfter);
static int32 getImplicitBitString(psPool_t *pool, unsigned char **pp,
				uint32 len,	int32 impVal, unsigned char **bitString,
				uint32 *bitLen);

static int32 x509ConfirmSignature(unsigned char *sigHash, unsigned char *sigOut,
							uint32 sigLen);
		
#ifdef USE_CRL
static void x509FreeRevoked(x509revoked_t **revoked);
#endif
		
#endif /* USE_CERT_PARSE */
		

/******************************************************************************/
#ifdef MATRIX_USE_FILE_SYSTEM
/******************************************************************************/

static int32 pemCertFileBufToX509(psPool_t *pool, unsigned char *fileBuf,
		int32 fileBufLen, psList_t **x509certList);
/******************************************************************************/
/*
	Open a PEM X.509 certificate file and parse it
	
	Memory info:
		Caller must free outcert with psX509FreeCert on function success
		Caller does not have to free outcert on function failure
*/
int32 psX509ParseCertFile(psPool_t *pool, char *fileName,
						psX509Cert_t **outcert, int32 flags)
{
	int32			fileBufLen, err;
	unsigned char	*fileBuf;
	psList_t		*fileList, *currentFile, *x509list, *frontX509;
	psX509Cert_t	*currentCert, *firstCert, *prevCert;

	*outcert = NULL;
/*
	First test to see if there are multiple files being passed in.
	Looking for a semi-colon delimiter
*/
	if ((err = psParseList(pool, fileName, ';', &fileList)) < 0) {
		return err;
	}
	currentFile = fileList;
	firstCert = prevCert = NULL;
/*
	Recurse each individual file
*/
	while (currentFile) {
		if ((err = psGetFileBuf(pool, (char*)currentFile->item, &fileBuf,
				&fileBufLen)) < PS_SUCCESS) {
			psFreeList(fileList);
			if (firstCert) psX509FreeCert(firstCert);
			return err;
		}

		if ((err = pemCertFileBufToX509(pool, fileBuf, fileBufLen, &x509list))
				< PS_SUCCESS) {
			psFreeList(fileList);
			psFree(fileBuf);
			if (firstCert) psX509FreeCert(firstCert);
			return err;
		}
		psFree(fileBuf);

		frontX509 = x509list;
/*
		Recurse each individual cert buffer from within the file 
*/
		while (x509list != NULL) {	
			if ((err = psX509ParseCert(pool, x509list->item, x509list->len,
					&currentCert, flags)) < PS_SUCCESS) {
				psX509FreeCert(currentCert);	
				psFreeList(fileList);
				psFreeList(frontX509);
				if (firstCert) psX509FreeCert(firstCert);
				return err;
			}

			x509list = x509list->next;
			if (firstCert == NULL) {
				firstCert = currentCert;
			} else {
				prevCert->next = currentCert;
			}
			prevCert = currentCert;
			currentCert = currentCert->next;
		}
		currentFile = currentFile->next;
		psFreeList(frontX509);
	}
	psFreeList(fileList);

	*outcert = firstCert;

	return PS_SUCCESS;
}

/******************************************************************************/
/*
*/
static int32 pemCertFileBufToX509(psPool_t *pool, unsigned char *fileBuf,
		int32 fileBufLen, psList_t **x509certList)
{
	psList_t		*front, *prev, *current;
	char			*start, *end, *endTmp, *chFileBuf;

	*x509certList = NULL;
	if (fileBufLen < 0 || fileBuf == NULL) {
		psTraceCrypto("Bad parameters to pemCertFileBufToX509\n");
		return PS_ARG_FAIL;
	}
	front = current = psMalloc(pool, sizeof(psList_t));
	if (current == NULL) {
		psError("Memory allocation error first pemCertFileBufToX509\n");
		return PS_MEM_FAIL;
	}
	memset(current, 0x0, sizeof(psList_t));
	chFileBuf = (char*)fileBuf;
	while (fileBufLen > 0) {
		if (((start = strstr(chFileBuf, "-----BEGIN")) != NULL) &&
				((start = strstr(chFileBuf, "CERTIFICATE-----")) != NULL) &&
				((end = strstr(start, "-----END")) != NULL) &&
				((endTmp = strstr(end,"CERTIFICATE-----")) != NULL)) {
			start += strlen("CERTIFICATE-----");
			if (current == NULL) {
				current = psMalloc(pool, sizeof(psList_t));
				if (current == NULL) {
					psFreeList(front);
					psError("Memory allocation error: pemCertFileBufToX509\n");
					return PS_MEM_FAIL;
				}
				memset(current, 0x0, sizeof(psList_t));
				prev->next = current;
			}
			current->len = (int32)(end - start);
			end = endTmp + strlen("CERTIFICATE-----");
			while (*end == '\x0d' || *end == '\x0a' || *end == '\x09'
				   || *end == ' ') {
				end++;
			}
		} else {
			psFreeList(front);
			psTraceCrypto("File buffer does not look to be X.509 PEM format\n");
			return PS_PARSE_FAIL;
		}
		current->item = psMalloc(pool, current->len);
		if (current->item == NULL) {
			psFreeList(front);
			psError("Memory allocation error: pemCertFileBufToX509\n");
			return PS_MEM_FAIL;
		}
		memset(current->item, '\0', current->len);
		
		fileBufLen -= (int32)((unsigned char*)end - fileBuf);
		fileBuf = (unsigned char*)end;
		
		if (psBase64decode((unsigned char*)start, current->len, current->item,
							 (uint32*)&current->len) != 0) {
			psFreeList(front);
			psTraceCrypto("Unable to base64 decode certificate\n");
			return PS_PARSE_FAIL;
		}
		prev = current;
		current = current->next;
		chFileBuf = (char*)fileBuf;
	}
	*x509certList = front;
	return PS_SUCCESS;
}
#endif /* MATRIX_USE_FILE_SYSTEM */
/******************************************************************************/


/******************************************************************************/
/*
	Parse an X509 v3 ASN.1 certificate stream
	http://tools.ietf.org/html/rfc3280
		
	flags
		CERT_STORE_UNPARSED_BUFFER 
		CERT_STORE_DN_BUFFER
		
	Memory info:
		Caller must always free outcert with psX509FreeCert.  Even on failure
*/
int32 psX509ParseCert(psPool_t *pool, unsigned char *pp, uint32 size, 
						psX509Cert_t **outcert, int32 flags)
{
	psX509Cert_t		*cert;
	unsigned char		*p, *end, *certStart;
	uint32				len;
	int32				parsing, rc;
#ifdef USE_CERT_PARSE	
	psDigestContext_t	hashCtx;
	unsigned char		*certEnd;
	uint32				certLen;
	int32				plen;
#endif	

/*
	Allocate the cert structure right away.  User MUST always call
	psX509FreeCert regardless of whether this function succeeds.
	memset is important because the test for NULL is what is used
	to determine what to free
*/
	*outcert = cert = psMalloc(pool, sizeof(psX509Cert_t));
	if (cert == NULL) {
		psError("Memory allocation failure in psX509ParseCert\n");
		return PS_MEM_FAIL;
	}
	memset(cert, 0x0, sizeof(psX509Cert_t));
	
	p = pp;
	end = p + size;
/*
	Certificate  ::=  SEQUENCE  {
		tbsCertificate		TBSCertificate,
		signatureAlgorithm	AlgorithmIdentifier,
		signatureValue		BIT STRING }
*/
	parsing = 1;
	while (parsing) {

		certStart = p;	
		if ((rc = getAsnSequence(&p, (uint32)(end - p), &len)) < 0) {
			psTraceCrypto("Initial cert parse error\n");
			return rc;
		}
/*
		 If the user has specified to keep the ASN.1 buffer in the X.509
		 structure, now is the time to account for it
*/
		if (flags & CERT_STORE_UNPARSED_BUFFER) {
			cert->binLen = len + (int32)(p - certStart);
			cert->unparsedBin = psMalloc(pool, cert->binLen);
			if (cert->unparsedBin == NULL) {
				psError("Memory allocation error in psX509ParseCert\n");
				return PS_MEM_FAIL;
			}
			memcpy(cert->unparsedBin, certStart, cert->binLen);
		}
		
#ifdef USE_CERT_PARSE
		certStart = p;
/*	
		TBSCertificate  ::=  SEQUENCE  {
		version			[0]		EXPLICIT Version DEFAULT v1,
		serialNumber			CertificateSerialNumber,
		signature				AlgorithmIdentifier,
		issuer					Name,
		validity				Validity,
		subject					Name,
		subjectPublicKeyInfo	SubjectPublicKeyInfo,
		issuerUniqueID	[1]		IMPLICIT UniqueIdentifier OPTIONAL,
							-- If present, version shall be v2 or v3
		subjectUniqueID	[2]	IMPLICIT UniqueIdentifier OPTIONAL,
							-- If present, version shall be v2 or v3
		extensions		[3]	EXPLICIT Extensions OPTIONAL
							-- If present, version shall be v3	}
*/
		if ((rc = getAsnSequence(&p, (uint32)(end - p), &len)) < 0) {
			psTraceCrypto("ASN sequence parse error\n");
			return rc;
		}
		certEnd = p + len;
		certLen = certEnd - certStart;

/*
		Version  ::=  INTEGER  {  v1(0), v2(1), v3(2)  }
*/
		if ((rc = getExplicitVersion(&p, (uint32)(end - p), 0, &cert->version))
				< 0) {
			psTraceCrypto("ASN version parse error\n");
			return rc;
		}
		if (cert->version != 2) {		
			psTraceIntCrypto("Warning: non-v3 certificate version: %d\n",
				cert->version);
		}
/*
		CertificateSerialNumber  ::=  INTEGER
		There is a special return code for a missing serial number that
		will get written to the parse warning flag
*/
		if ((rc = getSerialNum(pool, &p, (uint32)(end - p), &cert->serialNumber,
				&cert->serialNumberLen)) < 0) {
			psTraceCrypto("ASN serial number parse error\n");
			return rc;
		}
/*
		AlgorithmIdentifier  ::=  SEQUENCE  {
		algorithm				OBJECT IDENTIFIER,
		parameters				ANY DEFINED BY algorithm OPTIONAL }
*/
		if ((rc = getAsnAlgorithmIdentifier(&p, (uint32)(end - p),
				&cert->certAlgorithm, 0, &plen)) < 0) {
			psTraceCrypto("Couldn't parse algorithm identifier for certAlgorithm\n");
			return rc;
		}
		psAssert(plen == 0);
/*
		Name ::= CHOICE {
		RDNSequence }

		RDNSequence ::= SEQUENCE OF RelativeDistinguishedName

		RelativeDistinguishedName ::= SET OF AttributeTypeAndValue

		AttributeTypeAndValue ::= SEQUENCE {
		type	AttributeType,
		value	AttributeValue }

		AttributeType ::= OBJECT IDENTIFIER

		AttributeValue ::= ANY DEFINED BY AttributeType
*/
		if ((rc = psX509GetDNAttributes(pool, &p, (uint32)(end - p),
				&cert->issuer, flags)) < 0) {
			psTraceCrypto("Couldn't parse issuer DN attributes\n");
			return rc;
		}
/*
		Validity ::= SEQUENCE {
		notBefore	Time,
		notAfter	Time	}
*/
		if ((rc = getValidity(pool, &p, (uint32)(end - p),
				&cert->notBeforeTimeType, &cert->notAfterTimeType,
				&cert->notBefore, &cert->notAfter)) < 0) {
			psTraceCrypto("Couldn't parse validity\n");
			return rc;
		}
/*
		Subject DN
*/
		if ((rc = psX509GetDNAttributes(pool, &p, (uint32)(end - p),
				&cert->subject,	flags)) < 0) {
			psTraceCrypto("Couldn't parse subject DN attributes\n");
			return rc;
		}
/*
		SubjectPublicKeyInfo  ::=  SEQUENCE  {
		algorithm			AlgorithmIdentifier,
		subjectPublicKey	BIT STRING	}
*/
		if ((rc = getAsnSequence(&p, (uint32)(end - p), &len)) < 0) {
			psTraceCrypto("Couldn't get ASN sequence for pubKeyAlgorithm\n");
			return rc;
		}
		if ((rc = getAsnAlgorithmIdentifier(&p, (uint32)(end - p),
				&cert->pubKeyAlgorithm, 1, &plen)) < 0) {
			psTraceCrypto("Couldn't parse algorithm id for pubKeyAlgorithm\n");
			return rc;
		}

/*
		Allocate for generic type and then populate with correct type
		based on pubKeyAlgorithm OID
*/
		cert->publicKey.key = psMalloc(pool, sizeof(pubKeyUnion_t));
		if (cert->publicKey.key == NULL) {
			psError("Memory allocation error in psX509ParseCert\n");
			return PS_MEM_FAIL;
		}
		memset(cert->publicKey.key, 0x0, sizeof(pubKeyUnion_t));
		if (cert->pubKeyAlgorithm == OID_ECDSA_KEY_ALG) {
			psTraceCrypto("ECC public key algorithm not enabled in cert parse");
			return PS_UNSUPPORTED_FAIL;
		} else if (cert->pubKeyAlgorithm == OID_RSA_KEY_ALG) {
			if ((rc = getAsnRsaPubKey(pool, &p, (uint32)(end - p),
				(psRsaKey_t*)(&cert->publicKey.key->rsa))) < 0) {
				psTraceCrypto("Couldn't get RSA pub key from cert\n");
				return rc;
			}
			cert->publicKey.type = PS_RSA;
			cert->publicKey.keysize = cert->publicKey.key->rsa.size;
		} else {
			psTraceCrypto("Unsupported public key algorithm in cert parse\n");
			return PS_UNSUPPORTED_FAIL;
		}

/*
		As the next three values are optional, we can do a specific test here
*/
		if (*p != (ASN_SEQUENCE | ASN_CONSTRUCTED)) {
			if (getImplicitBitString(pool, &p, (uint32)(end - p),
						IMPLICIT_ISSUER_ID, &cert->uniqueIssuerId,
						&cert->uniqueIssuerIdLen) < 0 ||
					getImplicitBitString(pool, &p, (uint32)(end - p),
						IMPLICIT_SUBJECT_ID, &cert->uniqueSubjectId,
						&cert->uniqueSubjectIdLen) < 0 ||
					getExplicitExtensions(pool, &p, (uint32)(end - p),
						EXPLICIT_EXTENSION, &cert->extensions) < 0) {
				psTraceCrypto("There was an error parsing a certificate\n");
				psTraceCrypto("extension.  This is likely caused by an\n");
				psTraceCrypto("extension format that is not currently\n");
				psTraceCrypto("recognized.  Please email Inside support\n");
				psTraceCrypto("to add support for the extension.\n\n");
				return PS_PARSE_FAIL;
			}
		}
/*
		This is the end of the cert.  Do a check here to be certain
*/
		if (certEnd != p) {
			psTraceCrypto("Error.  Expecting end of cert\n");
			return PS_LIMIT_FAIL;
		}
/*
		Certificate signature info
*/
		if ((rc = getAsnAlgorithmIdentifier(&p, (uint32)(end - p),
				&cert->sigAlgorithm, 0, &plen)) < 0) {
			psTraceCrypto("Couldn't get algorithm identifier for sigAlgorithm\n");
			return rc;
		}
		psAssert(plen == 0);
/*
		Signature algorithm must match that specified in TBS cert
*/
		if (cert->certAlgorithm != cert->sigAlgorithm) {
			psTraceCrypto("Parse error: mismatched signature type\n");
			return PS_CERT_AUTH_FAIL;
		}
		
/*
		Compute the hash of the cert here for CA validation
*/
		if (cert->certAlgorithm == OID_MD5_RSA_SIG) {
			psMd5Init(&hashCtx);
			psMd5Update(&hashCtx, certStart, certLen);
			psMd5Final(&hashCtx, cert->sigHash);
		} else if (cert->certAlgorithm == OID_SHA1_RSA_SIG) {
			psSha1Init(&hashCtx);
			psSha1Update(&hashCtx, certStart, certLen);
			psSha1Final(&hashCtx, cert->sigHash);
		}

		if ((rc = psX509GetSignature(pool, &p, (uint32)(end - p),
				&cert->signature, &cert->signatureLen)) < 0) {
			psTraceCrypto("Couldn't parse signature\n");
			return rc;
		}
		
#else /* !USE_CERT_PARSE */
		p = certStart + len + (int32)(p - certStart);
#endif /* USE_CERT_PARSE */
/*
		The ability to parse additional chained certs is a PKI product
		feature addition.  Chaining in MatrixSSL is handled internally.
*/
		if (p != end) {
			cert->next = psMalloc(pool, sizeof(psX509Cert_t));
			if (cert->next == NULL) {
				psError("Memory allocation error in psX509ParseCert\n");
				return PS_MEM_FAIL;
			}
			cert = cert->next;
			memset(cert, 0x0, sizeof(psX509Cert_t));
		} else {
			parsing = 0;
		}
	}
		
	return (int32)(p - pp);
}

#ifdef USE_FULL_CERT_PARSE
static void x509FreeExtensions(x509v3extensions_t *extensions)
{
	if (extensions->keyUsage)	psFree(extensions->keyUsage);
	if (extensions->sk.id)		psFree(extensions->sk.id);
	if (extensions->ak.keyId)	psFree(extensions->ak.keyId);
	if (extensions->ak.serialNum) psFree(extensions->ak.serialNum);
	if (extensions->ak.attribs.commonName)
		psFree(extensions->ak.attribs.commonName);
	if (extensions->ak.attribs.country) psFree(extensions->ak.attribs.country);
	if (extensions->ak.attribs.state) psFree(extensions->ak.attribs.state);
	if (extensions->ak.attribs.locality)
		psFree(extensions->ak.attribs.locality);
	if (extensions->ak.attribs.organization)
		psFree(extensions->ak.attribs.organization);
	if (extensions->ak.attribs.orgUnit) psFree(extensions->ak.attribs.orgUnit);
	if (extensions->ak.attribs.dnenc) psFree(extensions->ak.attribs.dnenc);
}
#endif /* USE_FULL_CERT_PARSE */

/******************************************************************************/
/*
	User must call after all calls to psX509ParseCert
	(we violate the coding standard a bit here for clarity)
*/
void psX509FreeCert(psX509Cert_t *cert)
{
	psX509Cert_t			*curr, *next;
#ifdef USE_CERT_PARSE	
	x509GeneralName_t		*active, *inc;
#endif /* USE_CERT_PARSE */	

	curr = cert;
	while (curr) {
		if (curr->unparsedBin)			psFree(curr->unparsedBin);
#ifdef USE_CERT_PARSE		
		psX509FreeDNStruct(&curr->issuer);
		psX509FreeDNStruct(&curr->subject);
		if (curr->serialNumber)			psFree(curr->serialNumber);
		if (curr->notBefore)			psFree(curr->notBefore);
		if (curr->notAfter)				psFree(curr->notAfter);
		if (curr->signature)			psFree(curr->signature);
		if (curr->uniqueIssuerId)		psFree(curr->uniqueIssuerId);
		if (curr->uniqueSubjectId)		psFree(curr->uniqueSubjectId);

		if (curr->extensions.san) {
			active = curr->extensions.san;
			while (active != NULL) {
				inc = active->next;
				psFree(active->data);
				psFree(active);
				active = inc;
			}
		}
#ifdef USE_CRL
		if (curr->extensions.crlDist) {
			active = curr->extensions.crlDist;
			while (active != NULL) {
				inc = active->next;
				psFree(active->data);
				psFree(active);
				active = inc;
			}
		}
#endif		

		if (curr->publicKey.key) {
			if (curr->pubKeyAlgorithm == OID_RSA_KEY_ALG) {
				pstm_clear(&(curr->publicKey.key->rsa.N));
				pstm_clear(&(curr->publicKey.key->rsa.e));
			}

			psFree(curr->publicKey.key);
		}

#ifdef USE_FULL_CERT_PARSE
		x509FreeExtensions(&curr->extensions);
#ifdef USE_CRL
		x509FreeRevoked(&curr->revoked);
#endif		
#endif /* USE_FULL_CERT_PARSE */
#endif /* USE_CERT_PARSE */
		next = curr->next;
		psFree(curr);
		curr = next;
	}
}	

#ifdef USE_CERT_PARSE
/******************************************************************************/
/*
	Currently just returning the raw BIT STRING and size in bytes
*/
int32 psX509GetSignature(psPool_t *pool, unsigned char **pp, uint32 len,
					unsigned char **sig, uint32 *sigLen)
{
	unsigned char   *p = *pp, *end;
	int32           ignore_bits;
	uint32			llen;

	end = p + len;
	if (len < 1 || (*(p++) != ASN_BIT_STRING) ||
			getAsnLength(&p, len - 1, &llen) < 0 || (uint32)(end - p) < llen) {
        psTraceCrypto("Initial parse error in getSignature\n");
		return PS_PARSE_FAIL;
	}
	ignore_bits = *p++;
/*
	We assume this is always 0.
*/
	psAssert(ignore_bits == 0);
/*
	Length included the ignore_bits byte
*/
	*sigLen = llen - 1;
	*sig = psMalloc(pool, *sigLen);
	if (*sig == NULL) {
		psError("Memory allocation error in getSignature\n");
		return PS_MEM_FAIL;
	}
	memcpy(*sig, p, *sigLen);
	*pp = p + *sigLen;
	return PS_SUCCESS;
}

/******************************************************************************/
/*
	Extension lookup helper for getExplicitExtensions below
*/	
static int32 lookupExt(unsigned char md5hash[MD5_HASH_SIZE])
{
	int32				i, j;
	const unsigned char	*tmp;

	for (i = 0; ;i++) {
		if (extTable[i].id == -1) {
			psTraceCrypto("Couldn't find cert extension in lookupExt\n");
			return PS_FAILURE;
		}
		tmp = extTable[i].hash;
		for (j = 0; j < MD5_HASH_SIZE; j++) {
			if (md5hash[j] != tmp[j]) {
				break;
			}
			if (j == MD5_HASH_SIZE - 1) {
				return extTable[i].id;
			}
		}
	}
}

/******************************************************************************/
/*
	Parses a sequence of GeneralName types
	TODO: the actual types should be parsed.  Just copying data blob

	GeneralName ::= CHOICE {
		otherName						[0]		OtherName,
		rfc822Name						[1]		IA5String,
		dNSName							[2]		IA5String,
		x400Address						[3]		ORAddress,
		directoryName					[4]		Name,
		ediPartyName					[5]		EDIPartyName,
		uniformResourceIdentifier		[6]		IA5String,
		iPAddress						[7]		OCTET STRING,
		registeredID					[8]		OBJECT IDENTIFIER }
*/
static int32 parseGeneralNames(psPool_t *pool, unsigned char **buf, int32 len,
				unsigned char *extEnd, x509GeneralName_t **name)
{
	unsigned char		*p, *save;
	x509GeneralName_t	*activeName, *firstName, *prevName;
	
	if (*name == NULL) {
		firstName = NULL;
	} else {
		firstName = *name;
	}
	p = *buf;
	
	while (len > 0) {
		if (firstName == NULL) {
			activeName = firstName = psMalloc(pool,	sizeof(x509GeneralName_t));
			if (activeName == NULL) {
				return PS_MEM_FAIL;
			}
			memset(firstName, 0x0, sizeof(x509GeneralName_t));
			*name = firstName;
		} else {
/*
			Find the end
*/
			prevName = firstName;
			activeName = firstName->next;
			while (activeName != NULL) {
				prevName = activeName;
				activeName = activeName->next;
			}
			prevName->next = psMalloc(pool,	sizeof(x509GeneralName_t));
			if (prevName->next == NULL) {
				/* TODO: free the list */
				return PS_MEM_FAIL;
			}
			activeName = prevName->next;
			memset(activeName, 0x0, sizeof(x509GeneralName_t));
		}
		activeName->id = *p & 0xF;
		p++; len--;
		switch (activeName->id) {
			case 0:
				memcpy(activeName->name, "other", 5);
				break;
			case 1:
				memcpy(activeName->name, "email", 5);
				break;
			case 2:
				memcpy(activeName->name, "DNS", 3);
				break;
			case 3:
				memcpy(activeName->name, "x400Address", 11);
				break;
			case 4:
				memcpy(activeName->name, "directoryName", 13);
				break;
			case 5:
				memcpy(activeName->name, "ediPartyName", 12);
				break;
			case 6:
				memcpy(activeName->name, "URI", 3);
				break;
			case 7:
				memcpy(activeName->name, "iPAddress", 9);
				break;
			case 8:
				memcpy(activeName->name, "registeredID", 12);
				break;
			default:
				memcpy(activeName->name, "unknown", 7);
				break;
		}
		
		save = p;
		if (getAsnLength(&p, (uint32)(extEnd - p), &activeName->dataLen) < 0 ||
				activeName->dataLen < 1 ||
				(uint32)(extEnd - p) < activeName->dataLen) {
			psTraceCrypto("ASN get len error in parseGeneralNames\n");
			return PS_PARSE_FAIL;
		}	
		len -= (p - save);
		
		if ((uint32)(extEnd - p) < activeName->dataLen) {
			psTraceCrypto("Error parsing altSubjectName dataLen\n");
			return PS_LIMIT_FAIL;
		}
		activeName->data = psMalloc(pool, activeName->dataLen + 1);
		if (activeName->data == NULL) {
			psError("Memory allocation error: activeName->data\n");
			return PS_MEM_FAIL;
		}
		memset(activeName->data, 0x0, activeName->dataLen + 1);
		memcpy(activeName->data, p, activeName->dataLen);
			
		p = p + activeName->dataLen;
		
		len -= activeName->dataLen; 
	}
	*buf = p;
	return PS_SUCCESS;
}

/******************************************************************************/
/*
	X509v3 extensions
*/
static int32 getExplicitExtensions(psPool_t *pool, unsigned char **pp, 
								 uint32 inlen, int32 expVal,
								 x509v3extensions_t *extensions)
{
	unsigned char		*p = *pp, *end;
	unsigned char		*extEnd, *extStart;
	int32				noid, critical;
	uint32				len, fullExtLen;
	unsigned char		oid[MD5_HASH_SIZE];
	psDigestContext_t	md5ctx;
#ifdef USE_CRL
	unsigned char		*save;
#endif

	end = p + inlen;
	if (inlen < 1) {
		return PS_ARG_FAIL;
	}
/*
	Not treating this as an error because it is optional.
*/
	if (*p != (ASN_CONTEXT_SPECIFIC | ASN_CONSTRUCTED | expVal)) {
		return 0;
	}
	p++;
	if (getAsnLength(&p, (uint32)(end - p), &len) < 0 ||
			(uint32)(end - p) < len) {
		psTraceCrypto("Initial getAsnLength failure in extension parse\n");
		return PS_PARSE_FAIL;
	}
/*
	Extensions  ::=  SEQUENCE SIZE (1..MAX) OF Extension

	Extension  ::=  SEQUENCE {
		extnID		OBJECT IDENTIFIER,
		extnValue	OCTET STRING	}
*/
	if (getAsnSequence(&p, (uint32)(end - p), &len) < 0) {
		psTraceCrypto("Initial getAsnSequence failure in extension parse\n");
		return PS_PARSE_FAIL;
	}
	extEnd = p + len;
	while ((p != extEnd) && *p == (ASN_SEQUENCE | ASN_CONSTRUCTED)) {
		if (getAsnSequence(&p, (uint32)(extEnd - p), &fullExtLen) < 0) {
			psTraceCrypto("getAsnSequence failure in extension parse\n");
			return PS_PARSE_FAIL;
		}
		extStart = p;
/*
		Conforming CAs MUST support key identifiers, basic constraints,
		key usage, and certificate policies extensions
	
		id-ce-authorityKeyIdentifier	OBJECT IDENTIFIER ::=	{ id-ce 35 }
		id-ce-basicConstraints			OBJECT IDENTIFIER ::=	{ id-ce 19 } 133
		id-ce-keyUsage					OBJECT IDENTIFIER ::=	{ id-ce 15 }
		id-ce-certificatePolicies		OBJECT IDENTIFIER ::=	{ id-ce 32 }
		id-ce-subjectAltName			OBJECT IDENTIFIER ::=	{ id-ce 17 }  131
*/
		if (extEnd - p < 1 || *p++ != ASN_OID) {
			psTraceCrypto("Malformed extension header\n");
			return PS_PARSE_FAIL;
		}
		
		if (getAsnLength(&p, (uint32)(extEnd - p), &len) < 0 ||
				(uint32)(extEnd - p) < len) {
			psTraceCrypto("Malformed extension length\n");
			return PS_PARSE_FAIL;
		}
/*
		Send the OID through a digest to get the unique id
*/
		psMd5Init(&md5ctx);
		while (len-- > 0) {
			psMd5Update(&md5ctx, p, sizeof(char));
			p++;
		}
		psMd5Final(&md5ctx, oid);
		noid = lookupExt(oid);

/*
		Possible boolean value here for 'critical' id.  It's a failure if a
		critical extension is found that is not supported
*/
		critical = 0;
		if (*p == ASN_BOOLEAN) {
			p++;
			if (*p++ != 1) {
				psTraceCrypto("Error parsing critical id for cert extension\n");
				return PS_PARSE_FAIL;
			}
			if (*p > 0) {
				/* Officially DER TRUE must be 0xFF, openssl is more lax */
				if (*p != 0xFF) {
					psTraceCrypto("Warning: DER BOOLEAN TRUE should be 0xFF\n");
				}
				critical = 1;
			}
			p++;
		}
		if (extEnd - p < 1 || (*p++ != ASN_OCTET_STRING) ||
				getAsnLength(&p, (uint32)(extEnd - p), &len) < 0 || 
				(uint32)(extEnd - p) < len) {
			psTraceCrypto("Expecting OCTET STRING in ext parse\n");
			return PS_PARSE_FAIL;
		}

		switch (noid) {
/*
			 BasicConstraints ::= SEQUENCE {
				cA						BOOLEAN DEFAULT FALSE,
				pathLenConstraint		INTEGER (0..MAX) OPTIONAL }
*/
			case EXT_BASIC_CONSTRAINTS:
				if (getAsnSequence(&p, (uint32)(extEnd - p), &len) < 0) {
					psTraceCrypto("Error parsing BasicConstraints extension\n");
					return PS_PARSE_FAIL;
				}
/*
				"This goes against PKIX guidelines but some CAs do it and some
				software requires this to avoid interpreting an end user
				certificate as a CA."
					- OpenSSL certificate configuration doc

				basicConstraints=CA:FALSE
*/
				if (len == 0) {
					break;
				}
/*
				Have seen some certs that don't include a cA bool.
*/
				if (*p == ASN_BOOLEAN) {
					p++;
					if (*p++ != 1) {
						psTraceCrypto("Error parsing BasicConstraints CA bool\n");
						return PS_PARSE_FAIL;
					}
					/* Officially DER TRUE must be 0xFF, openssl is more lax */
					if (*p > 0 && *p != 0xFF) {
						psTraceCrypto("Warning: cA TRUE should be 0xFF\n");
					} 
					extensions->bc.cA = *p++;
				} else {
					extensions->bc.cA = 0;
				}
/*
				Now need to check if there is a path constraint. Only makes
				sense if cA is true.  If it's missing, there is no limit to
				the cert path
*/
				if (*p == ASN_INTEGER) {
					if (getAsnInteger(&p, (uint32)(extEnd - p),
							&(extensions->bc.pathLenConstraint)) < 0) {
						psTraceCrypto("Error parsing BasicConstraints pathLen\n");	
						return PS_PARSE_FAIL;
					}
				} else {
					extensions->bc.pathLenConstraint = -1;
				}
				break;
			case EXT_ALT_SUBJECT_NAME:
				if (getAsnSequence(&p, (uint32)(extEnd - p), &len) < 0) {
					psTraceCrypto("Error parsing altSubjectName extension\n");
					return PS_PARSE_FAIL;
				}
				if (parseGeneralNames(pool, &p, len, extEnd, &extensions->san)
						< 0) {
					psTraceCrypto("Error parsing altSubjectName names\n");
					return PS_PARSE_FAIL;
				}
				
				break;
#ifdef USE_FULL_CERT_PARSE

#ifdef USE_CRL
			case EXT_CRL_DIST_PTS:
			
				if (getAsnSequence(&p, (int32)(extEnd - p), &fullExtLen) < 0) {
					psTraceCrypto("Error parsing authKeyId extension\n");
					return PS_PARSE_FAIL;
				}
					
				while (fullExtLen > 0) { 
					save = p;
					if (getAsnSequence(&p, (uint32)(extEnd - p), &len) < 0) {
						psTraceCrypto("getAsnSequence fail in crldist parse\n");
						return PS_PARSE_FAIL;
					}
					fullExtLen -= len + (p - save);					
					/* All memebers are optional */
					if (*p == (ASN_CONTEXT_SPECIFIC | ASN_CONSTRUCTED | 0)) {
						/* DistributionPointName */
						p++;
						if (getAsnLength(&p, (uint32)(extEnd - p), &len) < 0 ||
								len < 1 || (uint32)(extEnd - p) < len) {
							psTraceCrypto("ASN get len error in CRL extension\n");
							return PS_PARSE_FAIL;
						}			
									
						if ((*p & 0xF) == 0) { /* fullName (GeneralNames) */
							p++;
							if (getAsnLength(&p, (uint32)(extEnd - p), &len) < 0 ||
									len < 1 || (uint32)(extEnd - p) < len) {
								psTraceCrypto("ASN get len error in CRL extension\n");
								return PS_PARSE_FAIL;
							}	
							if (parseGeneralNames(pool, &p, len, extEnd,
									&extensions->crlDist) > 0) {
								psTraceCrypto("dist gen name parse fail\n");
								return PS_PARSE_FAIL;
							}
						} else if ((*p & 0xF) == 1) { /* RelativeDistName */
							p++;
							/* RelativeDistName not parsed */
							if (getAsnLength(&p, (uint32)(extEnd - p), &len) < 0
									|| len < 1 || (uint32)(extEnd - p) < len) {
								psTraceCrypto("ASN get len error in CRL extension\n");
								return PS_PARSE_FAIL;
							}
							p += len;
						} else {
							psTraceCrypto("DistributionPointName parse fail\n");
							return PS_PARSE_FAIL;
						}						
					}
					if (*p == (ASN_CONTEXT_SPECIFIC | ASN_CONSTRUCTED | 1)) {
						p++;
						/* ReasonFlags not parsed */
						if (getAsnLength(&p, (uint32)(extEnd - p), &len) < 0 ||
								len < 1 || (uint32)(extEnd - p) < len) {
							psTraceCrypto("ASN get len error in CRL extension\n");
							return PS_PARSE_FAIL;
						}
						p += len;
					}
					if (*p == (ASN_CONTEXT_SPECIFIC | ASN_CONSTRUCTED | 2)) {
						p++;
						/* General Names not parsed */
						if (getAsnLength(&p, (uint32)(extEnd - p), &len) < 0 ||
								len < 1 || (uint32)(extEnd - p) < len) {
							psTraceCrypto("ASN get len error in CRL extension\n");
							return PS_PARSE_FAIL;
						}
						p += len;
					}					
				}
				break;
#endif /* USE_CRL */
			
			case EXT_AUTH_KEY_ID:
/*
				AuthorityKeyIdentifier ::= SEQUENCE {
				keyIdentifier			[0] KeyIdentifier			OPTIONAL,
				authorityCertIssuer		[1] GeneralNames			OPTIONAL,
				authorityCertSerialNumber [2] CertificateSerialNumber OPTIONAL }

				KeyIdentifier ::= OCTET STRING
*/
				if (getAsnSequence(&p, (int32)(extEnd - p), &len) < 0) {
					psTraceCrypto("Error parsing authKeyId extension\n");
					return PS_PARSE_FAIL;
				}
/*
				Have seen a cert that has a zero length ext here.  Let it pass.
*/
				if (len == 0) {
					break;
				}
/*
				All memebers are optional
*/
				if (*p == (ASN_CONTEXT_SPECIFIC | ASN_PRIMITIVE | 0)) {
					p++;
					if (getAsnLength(&p, (int32)(extEnd - p), 
							&extensions->ak.keyLen) < 0 ||
							(uint32)(extEnd - p) < extensions->ak.keyLen) {
						psTraceCrypto("Error keyLen in authKeyId extension\n");
						return PS_PARSE_FAIL;
					}
					extensions->ak.keyId =psMalloc(pool, extensions->ak.keyLen);
					if (extensions->ak.keyId == NULL) {
						psError("Mem allocation err: extensions->ak.keyId\n");
						return PS_MEM_FAIL; 
					}
					memcpy(extensions->ak.keyId, p, extensions->ak.keyLen);
					p = p + extensions->ak.keyLen;
				}
				if (*p == (ASN_CONTEXT_SPECIFIC | ASN_CONSTRUCTED | 1)) {
					p++;
					if (getAsnLength(&p, (int32)(extEnd - p), &len) < 0 ||
							len < 1 || (uint32)(extEnd - p) < len) {
						psTraceCrypto("ASN get len error in authKeyId extension\n");
						return PS_PARSE_FAIL;
					}
					if ((*p ^ ASN_CONTEXT_SPECIFIC ^ ASN_CONSTRUCTED) != 4) {
						/* We are just dealing with DN formats here */
						psTraceIntCrypto("Error auth key-id name type: %d\n",
							*p ^ ASN_CONTEXT_SPECIFIC ^ ASN_CONSTRUCTED);
						return PS_PARSE_FAIL;
					}
					p++;
					if (getAsnLength(&p, (int32)(extEnd - p), &len) < 0 || 
							(uint32)(extEnd - p) < len) {
						psTraceCrypto("ASN get len error2 in authKeyId extension\n");
						return PS_PARSE_FAIL;
					}
					if (psX509GetDNAttributes(pool, &p, (int32)(extEnd - p),
							&(extensions->ak.attribs), 0) < 0) {
						psTraceCrypto("Error parsing ak.attribs\n");
						return PS_PARSE_FAIL;
					}
				}
				if ((*p == (ASN_CONTEXT_SPECIFIC | ASN_PRIMITIVE | 2)) ||
						(*p == ASN_INTEGER)){
/*
					Treat as a serial number (not a native INTEGER)
*/
					if (getSerialNum(pool, &p, (int32)(extEnd - p),
							&(extensions->ak.serialNum), &len) < 0) {
						psTraceCrypto("Error parsing ak.serialNum\n");
						return PS_PARSE_FAIL;
					}
					extensions->ak.serialNumLen = len;
				}
				break;

			case EXT_KEY_USAGE:
/*
				KeyUsage ::= BIT STRING {
					digitalSignature		(0),
					nonRepudiation			(1),
					keyEncipherment			(2),
					dataEncipherment		(3),
					keyAgreement			(4),
					keyCertSign				(5),

					cRLSign					(6),
					encipherOnly			(7),
					decipherOnly			(8) }
*/
				if (*p++ != ASN_BIT_STRING) {
					psTraceCrypto("Error parsing keyUsage extension\n");
					return PS_PARSE_FAIL;
				}
				if (getAsnLength(&p, (int32)(extEnd - p), &len) < 0 || 
						(uint32)(extEnd - p) < len) {
					psTraceCrypto("Malformed keyUsage extension\n");
					return PS_PARSE_FAIL;
				}
/*
				We'd expect a length of 3 with the first byte being '07' to
				account for the trailing ignore bits in the second byte.
				But it doesn't appear all certificates adhere to the ASN.1
				encoding standard very closely.  Just set it all aside for 
				user to interpret as necessary.
*/
				extensions->keyUsage = psMalloc(pool, len);
				if (extensions->keyUsage == NULL) {
					psError("Memory allocation error extensions->keyUsage\n");
					return PS_MEM_FAIL;
				}
				memcpy(extensions->keyUsage, p, len);
				extensions->keyUsageLen = len;
				p = p + len;
				break;
			case EXT_SUBJ_KEY_ID:
/*
				The value of the subject key identifier MUST be the value
				placed in the key identifier field of the Auth Key Identifier
				extension of certificates issued by the subject of
				this certificate.
*/
				if (*p++ != ASN_OCTET_STRING || getAsnLength(&p,
						(int32)(extEnd - p), &(extensions->sk.len)) < 0 ||
						(uint32)(extEnd - p) < extensions->sk.len) {
					psTraceCrypto("Error parsing subjectKeyId extension\n");
					return PS_PARSE_FAIL;
				}
				extensions->sk.id = psMalloc(pool, extensions->sk.len);
				if (extensions->sk.id == NULL) {
					psError("Memory allocation error extensions->sk.id\n");
					return PS_MEM_FAIL;
				}
				memcpy(extensions->sk.id, p, extensions->sk.len);
				p = p + extensions->sk.len;
				break;
#endif /* USE_FULL_CERT_PARSE */
/*
			Unsupported or skipping because USE_FULL_CERT_PARSE is undefined
*/
			default:
				if (critical) {
/*
					SPEC DIFFERENCE:  Ignoring an unrecognized critical
					extension.  The specification dictates an error should
					occur, but real-world experience has shown this is not
					a realistic or desirable action.  Also, no other SSL
					implementations have been found to error in this case.
					It is NOT a security risk in an RSA authenticaion sense.
*/
					psTraceCrypto("Unknown critical ext encountered\n");
				}
				p++;
/*
				Skip over based on the length reported from the ASN_SEQUENCE
				surrounding the entire extension.  It is not a guarantee that
				the value of the extension itself will contain it's own length.
*/
				p = p + (fullExtLen - (p - extStart));
				break;
		}
	}
	*pp = p;
	return 0;
}

/******************************************************************************/
/*
    Although a certificate serial number is encoded as an integer type, that
    doesn't prevent it from being abused as containing a variable length
    binary value.  Get it here.
*/ 
static int32 getSerialNum(psPool_t *pool, unsigned char **pp, uint32 len,
                        unsigned char **sn, uint32 *snLen)
{
	unsigned char   *p = *pp;
	uint32           vlen;

	if ((*p != (ASN_CONTEXT_SPECIFIC | ASN_PRIMITIVE | 2)) &&
			(*p != ASN_INTEGER)) {
		psTraceCrypto("X.509 getSerialNum failed on first bytes\n");
        return PS_PARSE_FAIL;
    }
    p++;

    if (len < 1 || getAsnLength(&p, len - 1, &vlen) < 0 || (len - 1) < vlen) {
        psTraceCrypto("ASN getSerialNum failed\n");
        return PS_PARSE_FAIL;
    }
    *snLen = vlen;
	
	if (vlen > 0) {
		*sn = psMalloc(pool, vlen);
		if (*sn == NULL) {
			psError("Memory allocation failure in getSerialNum\n");
			return PS_MEM_FAIL;
		}
		memcpy(*sn, p, vlen);
		p += vlen;
		*pp = p;
	}
	return PS_SUCCESS;
}

/******************************************************************************/
/*
	Explicit value encoding has an additional tag layer
*/
static int32 getExplicitVersion(unsigned char **pp, uint32 len, int32 expVal,
				int32 *val)
{
	unsigned char   *p = *pp;
	uint32           exLen;

	if (len < 1) {
		psTraceCrypto("Invalid length to getExplicitVersion\n");
		return PS_PARSE_FAIL;
	}
/*
	This is an optional value, so don't error if not present.  The default
	value is version 1
*/ 
	if (*p != (ASN_CONTEXT_SPECIFIC | ASN_CONSTRUCTED | expVal)) {
		*val = 0;
		return PS_SUCCESS;
	}
	p++;
	if (getAsnLength(&p, len - 1, &exLen) < 0 || (len - 1) < exLen) {
		psTraceCrypto("getAsnLength failure in getExplicitVersion\n");
		return PS_PARSE_FAIL;
	}
	if (getAsnInteger(&p, exLen, val) < 0) {
		psTraceCrypto("getAsnInteger failure in getExplicitVersion\n");
		return PS_PARSE_FAIL;
	}
	*pp = p;
	return PS_SUCCESS;
}



/******************************************************************************/
/*
	Implementation specific date parser.  Does not actually verify the date
*/
static int32 getValidity(psPool_t *pool, unsigned char **pp, uint32 len,
				int32 *notBeforeTimeType, int32 *notAfterTimeType,
				char **notBefore, char **notAfter)
{
	unsigned char   *p = *pp, *end;
	uint32          seqLen, timeLen;

	end = p + len;
	if (len < 1 || *(p++) != (ASN_SEQUENCE | ASN_CONSTRUCTED) ||
			getAsnLength(&p, len - 1, &seqLen) < 0 ||
				(uint32)(end - p) < seqLen) {
		psTraceCrypto("getValidity failed on inital parse\n");
		return PS_PARSE_FAIL;
	}
/*
	Have notBefore and notAfter times in UTCTime or GeneralizedTime formats
*/
	if ((end - p) < 1 || ((*p != ASN_UTCTIME) && (*p != ASN_GENERALIZEDTIME))) {
		psTraceCrypto("Malformed validity\n");
		return PS_PARSE_FAIL;
	}
	*notBeforeTimeType = *p;
	p++;
/*
	Allocate them as null terminated strings
*/
	if (getAsnLength(&p, seqLen, &timeLen) < 0 || (uint32)(end - p) < timeLen) {
		psTraceCrypto("Malformed validity 2\n");
		return PS_PARSE_FAIL;
	}
	*notBefore = psMalloc(pool, timeLen + 1);
	if (*notBefore == NULL) {
		psError("Memory allocation error in getValidity for notBefore\n");
		return PS_MEM_FAIL;
	}
	memcpy(*notBefore, p, timeLen);
	(*notBefore)[timeLen] = '\0';
	p = p + timeLen;
	if ((end - p) < 1 || ((*p != ASN_UTCTIME) && (*p != ASN_GENERALIZEDTIME))) {
		psTraceCrypto("Malformed validity 3\n");
		return PS_PARSE_FAIL;
	}
	*notAfterTimeType = *p;
	p++;
	if (getAsnLength(&p, seqLen - timeLen, &timeLen) < 0 ||
			(uint32)(end - p) < timeLen) {
		psTraceCrypto("Malformed validity 4\n");
		return PS_PARSE_FAIL;
	}
	*notAfter = psMalloc(pool, timeLen + 1);
	if (*notAfter == NULL) {
		psError("Memory allocation error in getValidity for notAfter\n");
        return PS_MEM_FAIL;
	}
	memcpy(*notAfter, p, timeLen);
	(*notAfter)[timeLen] = '\0';
	p = p + timeLen;

	*pp = p;
	return PS_SUCCESS;
}

/******************************************************************************/
/*
    Could be optional.  If the tag doesn't contain the value from the left
    of the IMPLICIT keyword we don't have a match and we don't incr the pointer.
*/
static int32 getImplicitBitString(psPool_t *pool, unsigned char **pp,
				uint32 len,	int32 impVal, unsigned char **bitString,
				uint32 *bitLen)
{
	unsigned char   *p = *pp;
	int32           ignore_bits;

	if (len < 1) {
		psTraceCrypto("Initial parse error in getImplicitBitString\n");
		return PS_PARSE_FAIL;
	}
/*
	We don't treat this case as an error, because of the optional nature.
*/
	if (*p != (ASN_CONTEXT_SPECIFIC | ASN_CONSTRUCTED | impVal)) {
		return PS_SUCCESS;
	}

	p++;
	if (getAsnLength(&p, len, bitLen) < 0) {
		psTraceCrypto("Malformed implicitBitString\n");
		return PS_PARSE_FAIL;
	}
	ignore_bits = *p++;
	psAssert(ignore_bits == 0);

	*bitString = psMalloc(pool, *bitLen);
	if (*bitString == NULL) {
		psError("Memory allocation error in getImplicitBitString\n");
		return PS_MEM_FAIL;
	}
	memcpy(*bitString, p, *bitLen);
	*pp = p + *bitLen;
	return PS_SUCCESS;
}


/******************************************************************************/
/*
	Implementations of this specification MUST be prepared to receive
	the following standard attribute types in issuer names:
	country, organization, organizational-unit, distinguished name qualifier,
	state or province name, and common name 
*/
int32 psX509GetDNAttributes(psPool_t *pool, unsigned char **pp, uint32 len, 
				x509DNattributes_t *attribs, int32 flags)
{
	psDigestContext_t	hash;
	unsigned char		*p = *pp;
	unsigned char		*dnEnd, *dnStart;
	int32				id, stringType, checkHiddenNull;
	uint32				llen, setlen, arcLen;
	char				*stringOut;

	dnStart = p;
	if (getAsnSequence(&p, len, &llen) < 0) {
		return PS_PARSE_FAIL;
	}
	dnEnd = p + llen;

/*
	The possibility of a CERTIFICATE_REQUEST message.  Set aside full DN
*/
	if (flags & CERT_STORE_DN_BUFFER) {
		attribs->dnencLen = (uint32)(dnEnd - dnStart);
		attribs->dnenc = psMalloc(pool, attribs->dnencLen);
		if (attribs->dnenc == NULL) {
			psError("Memory allocation error in getDNAttributes\n");
			return PS_MEM_FAIL;
		}
		memcpy(attribs->dnenc, dnStart, attribs->dnencLen);
	}
	
	psSha1Init(&hash);
	while (p < dnEnd) {
		if (getAsnSet(&p, (uint32)(dnEnd - p), &setlen) < 0) {
			psTraceCrypto("Malformed DN attributes\n");
			return PS_PARSE_FAIL;
		}
		if (getAsnSequence(&p, (uint32)(dnEnd - p), &llen) < 0) {
			psTraceCrypto("Malformed DN attributes 2\n");
			return PS_PARSE_FAIL;
		}
		if (dnEnd <= p || (*(p++) != ASN_OID) ||
				getAsnLength(&p, (uint32)(dnEnd - p), &arcLen) < 0 || 
				(uint32)(dnEnd - p) < arcLen) {
			psTraceCrypto("Malformed DN attributes 3\n");
			return PS_PARSE_FAIL;
		}
/*
		id-at   OBJECT IDENTIFIER       ::=     {joint-iso-ccitt(2) ds(5) 4}
		id-at-commonName		OBJECT IDENTIFIER		::=		{id-at 3}
		id-at-countryName		OBJECT IDENTIFIER		::=		{id-at 6}
		id-at-localityName		OBJECT IDENTIFIER		::=		{id-at 7}
		id-at-stateOrProvinceName		OBJECT IDENTIFIER	::=	{id-at 8}
		id-at-organizationName			OBJECT IDENTIFIER	::=	{id-at 10}
		id-at-organizationalUnitName	OBJECT IDENTIFIER	::=	{id-at 11}
*/
		*pp = p;
/*
		Currently we are skipping OIDs not of type {joint-iso-ccitt(2) ds(5) 4}
		However, we could be dealing with an OID we MUST support per RFC.
		domainComponent is one such example.
*/
		if (dnEnd - p < 2) {
			psTraceCrypto("Malformed DN attributes 4\n");
			return PS_LIMIT_FAIL;
		}
		if ((*p++ != 85) || (*p++ != 4) ) {
			p = *pp;
/*
			Move past the OID and string type, get data size, and skip it.
			NOTE: Have had problems parsing older certs in this area.
*/
			if ((uint32)(dnEnd - p) < arcLen + 1) {
				psTraceCrypto("Malformed DN attributes 5\n");
				return PS_LIMIT_FAIL;
			}
			p += arcLen + 1;
			if (getAsnLength(&p, (uint32)(dnEnd - p), &llen) < 0 || 
					(uint32)(dnEnd - p) < llen) {
				psTraceCrypto("Malformed DN attributes 6\n");
				return PS_PARSE_FAIL;
			}
			p = p + llen;
			continue;
		}
/*
		Next are the id of the attribute type and the ASN string type
*/
		if (arcLen != 3 || dnEnd - p < 2) {
			psTraceCrypto("Malformed DN attributes 7\n");
			return PS_LIMIT_FAIL;
		}
		id = (int32)*p++;
/*
		Done with OID parsing
*/
		stringType = (int32)*p++;

		if (getAsnLength(&p, (uint32)(dnEnd - p), &llen) < 0 ||
				(uint32)(dnEnd - p) < llen) {
			psTraceCrypto("Malformed DN attributes 8\n");
			return PS_LIMIT_FAIL;
		}
/*
		For the known 8-bit character string types, we flag that we want
		to test for a hidden null in the middle of the string to address the
		issue of www.goodguy.com\0badguy.com.  For BMPSTRING, the user will
		have to validate against the xLen member for such abuses.
*/
		checkHiddenNull = PS_FALSE;
		switch (stringType) {
			case ASN_PRINTABLESTRING:
			case ASN_UTF8STRING:
			case ASN_IA5STRING:
				checkHiddenNull = PS_TRUE;
			case ASN_T61STRING:
			case ASN_BMPSTRING:
				stringOut = psMalloc(pool, llen + 2);
				if (stringOut == NULL) {
					psError("Memory allocation error in getDNAttributes\n");
					return PS_MEM_FAIL; 
				}
				memcpy(stringOut, p, llen);
/*
				Terminate with 2 null chars to support standard string
				manipulations with any potential unicode types.
*/
				stringOut[llen] = '\0';
				stringOut[llen + 1] = '\0';

				if (checkHiddenNull) {
					if ((uint32)strlen(stringOut) != llen) {
						psFree(stringOut);
						psTraceCrypto("Malformed DN attributes 9\n");
						return PS_PARSE_FAIL;
					}
				}
		
				p = p + llen;
				llen += 2; /* Add the two null bytes for length assignments */
				break;
			default:
				psTraceIntCrypto("Unsupported DN attrib type %d\n", stringType);
				return PS_UNSUPPORTED_FAIL;
		}

		switch (id) {
			case ATTRIB_COUNTRY_NAME:
				if (attribs->country) {
					psFree(attribs->country);
				}
				attribs->country = stringOut;
				attribs->countryType = (short)stringType;
				attribs->countryLen = (short)llen;
				break;
			case ATTRIB_STATE_PROVINCE:
				if (attribs->state) {
					psFree(attribs->state);
				}
				attribs->state = stringOut;
				attribs->stateType = (short)stringType;
				attribs->stateLen = (short)llen;
				break;
			case ATTRIB_LOCALITY:
				if (attribs->locality) {
					psFree(attribs->locality);
				}
				attribs->locality = stringOut;
				attribs->localityType = (short)stringType;
				attribs->localityLen = (short)llen;
				break;
			case ATTRIB_ORGANIZATION:
				if (attribs->organization) {
					psFree(attribs->organization);
				}
				attribs->organization = stringOut;
				attribs->organizationType = (short)stringType;
				attribs->organizationLen = (short)llen;
				break;
			case ATTRIB_ORG_UNIT:
				if (attribs->orgUnit) {
					psFree(attribs->orgUnit);
				}
				attribs->orgUnit = stringOut;
				attribs->orgUnitType = (short)stringType;
				attribs->orgUnitLen = (short)llen;
				break;
			case ATTRIB_COMMON_NAME:
				if (attribs->commonName) {
					psFree(attribs->commonName);
				}
				attribs->commonName = stringOut;
				attribs->commonNameType = (short)stringType;
				attribs->commonNameLen = (short)llen;
				break;
/*
			Not a MUST support
*/
			default:
				psFree(stringOut);
				stringOut = NULL;
				break;
		}
/*
		Hash up the DN.  Nice for validation later
*/
		if (stringOut != NULL) {
			psSha1Update(&hash, (unsigned char*)stringOut, llen);
		}
	}
	psSha1Final(&hash, (unsigned char*)attribs->hash);
	*pp = p;
	return PS_SUCCESS;
}

/******************************************************************************/
/*
	Free helper
*/
void psX509FreeDNStruct(x509DNattributes_t *dn)
{
	if (dn->country)		psFree(dn->country);
	if (dn->state)			psFree(dn->state);
	if (dn->locality)		psFree(dn->locality);
	if (dn->organization)	psFree(dn->organization);
	if (dn->orgUnit)		psFree(dn->orgUnit);
	if (dn->commonName)		psFree(dn->commonName);
	if (dn->dnenc)			psFree(dn->dnenc);
}

/******************************************************************************/
/*
	Fundamental routine to test whether the supplied issuerCert issued
	the supplied subjectCert.  There are currently two tests that are
	performed here:
		1. A strict SHA1 hash comparison of the Distinguished Name details
		2. A test of the public key cryptographic cert signature 

	subjectCert may be a chain.  Cert chains must always be passed with
	the child-most as the first in the list (the 'next' structure member
	points to the parent).  The authentication of the entire chain
	will be tested before the issuerCert is used to authenticate the
	parent-most certificate
	
	issuerCert must be a single certificate or NULL (can't be a chain)
	
	If there is no issuerCert the parent-most subejct cert will always
	be tested as a self-signed CA certificate.
	
	So there are three uses:
	1. Test a cert was issued by another (single subjectCert, single issuerCert)
	1. Test a self signed cert (single cert to subjectCert, no issuerCert)
	2. Test a CA terminated chain (cert chain to subjectCert, no issuerCert)

	This function exits with a failure code on the first authentication
	that doesn't succeed.  The 'authStatus' members may be examined for more
	information of where the authentication failed.

	The 'authStatus' member of the issuerCert will be set to PS_FALSE 
	since it will not be authenticated. 

	The 'authStatus' members of the subjectCert structures will always
	be reset to PS_FALSE when this routine is called and set to PS_TRUE 
	when authenticated.  Any error during the authentication will set the 
	current subject cert 'authStatus' member to PS_CERT_AUTH_FAIL and the
	function will return with an error code. 

	Return codes:
		PS_SUCCESS			- yes 

		PS_CERT_AUTH_FAIL	- nope. these certs are not a match
		PS_UNSUPPORTED_FAIL	- unrecognized cert format
		PS_ARG_FAIL			- local, psRsaDecryptPub 
		PS_LIMIT_FAIL		- psRsaDecryptPub 
		PS_FAILURE			- internal psRsaDecryptPub failure

	There is nothing for the caller to free at the completion of this
	routine.
*/
int32 psX509AuthenticateCert(psPool_t *pool, psX509Cert_t *subjectCert,
							psX509Cert_t *issuerCert)
{
	psX509Cert_t	*ic, *sc;
	unsigned char	sigOut[10 + MAX_HASH_SIZE + 9];	/* Max size */
	int32			sigType, issuerFlag, rc;
	uint32			sigLen;
	unsigned char	*tempSig, *rsaData;
	psPool_t	*pkiPool = NULL;
#ifdef USE_CRL
	x509revoked_t	*curr, *next;
#endif

	rc = 0;
	sigLen = 0;
	if (subjectCert == NULL) {
		psTraceCrypto("No subject cert given to psX509AuthenticateCert\n");
		return PS_ARG_FAIL;
	}

/*
	Determine what we've been passed
*/
	if (issuerCert == NULL) {
		issuerFlag = PS_FALSE;
	} else {
		if (issuerCert->next != NULL) {
			psTraceCrypto("Multiple issuer certs passed to psX509AuthenticateCert\n");
			return PS_ARG_FAIL;
		}
		issuerFlag = PS_TRUE;
		issuerCert->authStatus = PS_FALSE;
	}

/*
	Initial walk to reset auth flags in subjectCert and find first sc and ic
*/
	sc = subjectCert;
	while (sc) {
		sc->authStatus = PS_FALSE;
		sc = sc->next;
	}

	sc = subjectCert;
	ic = sc->next;
	if (ic == NULL) {
		if (issuerFlag) {
			ic = issuerCert; /* Easy case of single subject and single issuer */
		} else {
			ic = sc; /* A single subject cert was passed for self-signed test */
		}
	}
/*
	Error on first problem seen and set the subject status to FAIL
*/
	while (ic) {
/*
		Certificate authority constraint only available in version 3 certs
*/
		if ((ic->version > 1) && (ic->extensions.bc.cA <= 0)) {
			psTraceCrypto("Issuer does not have basicConstraint CA permissions\n");
			sc->authStatus = PS_CERT_AUTH_FAIL_BC;
			return PS_CERT_AUTH_FAIL_BC;
		}
/*
		Use sha1 hash of issuer fields computed at parse time to compare
*/
		if (memcmp(sc->issuer.hash, ic->subject.hash, SHA1_HASH_SIZE) != 0) {
			if (sc == ic) {
				psTraceCrypto("Info: not a self-signed certificate\n");
			} else {
				psTraceCrypto("Issuer DN attributes do not match subject\n");
			}
			sc->authStatus = PS_CERT_AUTH_FAIL_DN;
			return PS_CERT_AUTH_FAIL_DN;
		}
/*
		Signature confirmation
		The sigLen is the ASN.1 size in bytes for encoding the hash.
		The magic 10 is comprised of the SEQUENCE and ALGORITHM ID overhead.
		The magic 9, 8, or 5 is the OID length of the corresponding algorithm.
*/
		sigType = PS_UNSUPPORTED_FAIL;
		if (sc->sigAlgorithm ==  OID_MD5_RSA_SIG ||
				sc->sigAlgorithm == OID_MD2_RSA_SIG) {
			sigType = RSA_TYPE_SIG;
			sigLen = 10 + MD5_HASH_SIZE + 8;
		} 
		if (sc->sigAlgorithm == OID_SHA1_RSA_SIG) {
			sigLen = 10 + SHA1_HASH_SIZE + 5;
			sigType = RSA_TYPE_SIG;
		}
		if (sigType == PS_UNSUPPORTED_FAIL) {
			sc->authStatus = PS_CERT_AUTH_FAIL_SIG;
			psTraceIntCrypto("Unsupported certificate signature algorithm %d\n",
				subjectCert->sigAlgorithm);
			return sigType;
		}
	
		if (sigType == RSA_TYPE_SIG) {
			psAssert(sigLen <= sizeof(sigOut));
/*
			psRsaDecryptPub destroys the 'in' parameter so let it be a tmp
*/
			tempSig = psMalloc(pool, sc->signatureLen);
			if (tempSig == NULL) {
				psError("Memory allocation error: psX509AuthenticateCert\n");
				return PS_MEM_FAIL;
			}
			memcpy(tempSig, sc->signature, sc->signatureLen);
			rsaData = NULL;
			
			if ((rc = psRsaDecryptPub(pkiPool,
					(psRsaKey_t*)&(ic->publicKey.key->rsa),
					tempSig, sc->signatureLen, sigOut, sigLen, rsaData)) < 0) {
				psTraceCrypto("Unable to RSA decrypt certificate signature\n");
				sc->authStatus = PS_CERT_AUTH_FAIL_SIG;
				psFree(tempSig);
				return rc;					
			}
			psFree(tempSig);
			rc = x509ConfirmSignature(sc->sigHash, sigOut, sigLen);
		}

/*
		Test what happen in the signature test?
*/
		if (rc < PS_SUCCESS) {
			sc->authStatus = PS_CERT_AUTH_FAIL_SIG;
			return rc;
		}
		
#ifdef USE_CRL
		/* Does this issuer have a list of revoked serial numbers that needs
			to be checked? */
		if (ic->revoked) {
			curr = ic->revoked;
			while (curr != NULL) {
				next = curr->next;
				if (curr->serialLen == sc->serialNumberLen) {
					if (memcmp(curr->serial, sc->serialNumber, curr->serialLen)
							== 0) {
						sc->authStatus = PS_CERT_AUTH_FAIL_REVOKED;
						return -1;
					}
				}
				curr = next;
			}
			
		}
#endif		
		
/*
		Fall through to here only if passed all checks.
*/
		sc->authStatus = PS_CERT_AUTH_PASS;
/*
		Loop control for finding next ic and sc.
*/
		if (ic == sc) {
			ic = NULL; /* Single self-signed test completed */
		} else if (ic == issuerCert) {
			ic = NULL; /* If issuerCert was used, that was the final test */
		} else {
			sc = ic;
			ic = sc->next;
			if (ic == NULL) { /* Reached end of chain */
				if (issuerFlag) {
					ic = issuerCert; /* Final test against issuer */
				} else {
					ic = sc; /* Self-signed test on final subectCert */
				}
			}
		}
		
	}
	return PS_SUCCESS;
}

/******************************************************************************/
/*
	Do the signature validation for a subject certificate against a
	known CA certificate
*/
static int32 x509ConfirmSignature(unsigned char *sigHash, unsigned char *sigOut,
							uint32 sigLen)
{
	unsigned char	*end, *p = sigOut;
	unsigned char	hash[MAX_HASH_SIZE];
	int32			oi, plen;
	uint32			len;

	end = p + sigLen;
/*
	DigestInfo ::= SEQUENCE {
		digestAlgorithm DigestAlgorithmIdentifier,
		digest Digest }

	DigestAlgorithmIdentifier ::= AlgorithmIdentifier

	Digest ::= OCTET STRING
*/
	if (getAsnSequence(&p, (uint32)(end - p), &len) < 0) {
		psTraceCrypto("Initial parse error in x509ConfirmSignature\n");
		return PS_PARSE_FAIL;
	}

/*
	Could be MD5 or SHA1
 */
	if (getAsnAlgorithmIdentifier(&p, (uint32)(end - p), &oi, 0, &plen) < 0) {
		psTraceCrypto("Algorithm ID parse error in x509ConfirmSignature\n");
		return PS_PARSE_FAIL;
	}
	psAssert(plen == 0);
	if ((*p++ != ASN_OCTET_STRING) ||
			getAsnLength(&p, (uint32)(end - p), &len) < 0 ||
				(uint32)(end - p) <  len) {
		psTraceCrypto("getAsnLength parse error in x509ConfirmSignature\n");
		return PS_PARSE_FAIL;
	}
	memcpy(hash, p, len);
	if (oi == OID_MD5_ALG || oi == OID_MD2_ALG) {
		if (len != MD5_HASH_SIZE) {
			psTraceCrypto("MD5_HASH_SIZE error in x509ConfirmSignature\n");
			return PS_LIMIT_FAIL;
		}
	} else if (oi == OID_SHA1_ALG) {
		if (len != SHA1_HASH_SIZE) {
			psTraceCrypto("SHA1_HASH_SIZE error in x509ConfirmSignature\n");
			return PS_LIMIT_FAIL;
		}
	} else {
		psTraceCrypto("Unsupported alg ID error in x509ConfirmSignature\n");
		return PS_UNSUPPORTED_FAIL;
	}
/*
	hash should match sigHash
*/
	if (memcmp(hash, sigHash, len) != 0) {
		psTraceCrypto("Signature failure in x509ConfirmSignature\n");
		return PS_SIGNATURE_MISMATCH;
	}
	return PS_SUCCESS;
}

/******************************************************************************/
#ifdef USE_CRL
static void x509FreeRevoked(x509revoked_t **revoked)
{
	x509revoked_t		*next, *curr = *revoked;
	
	while (curr) {
		next = curr->next;
		psFree(curr->serial);
		psFree(curr);
		curr = next;
	}
	*revoked = NULL;
}

/*
	Parse a CRL and confirm was issued by supplied CA.
	
	Only interested in the revoked serial numbers which are stored in the
	CA structure if all checks out.  Used during cert validation as part of
	the default tests
*/
int32 psX509ParseCrl(psPool_t *pool, psX509Cert_t *CA, int append,
						unsigned char *crlBin, int32 crlBinLen)
{
	unsigned char		*end, *start, *revStart, *sigStart, *sigEnd,*p = crlBin;
	int32				oi, plen, sigLen, version, rc;
	unsigned char		sigHash[SHA512_HASH_SIZE], sigOut[SHA512_HASH_SIZE];
	x509revoked_t		*curr, *next;
	x509DNattributes_t	issuer;
	x509v3extensions_t	ext;
	psDigestContext_t	hashCtx;
	psPool_t			*pkiPool = MATRIX_NO_POOL;
	uint32				glen, ilen, timelen;

	end = p + crlBinLen;
	/*
		CertificateList  ::=  SEQUENCE  {
			tbsCertList          TBSCertList,
			signatureAlgorithm   AlgorithmIdentifier,
			signatureValue       BIT STRING  }

		TBSCertList  ::=  SEQUENCE  {
			version                 Version OPTIONAL,
                                     -- if present, shall be v2
			signature               AlgorithmIdentifier,
			issuer                  Name,
			thisUpdate              Time,
			nextUpdate              Time OPTIONAL,
			revokedCertificates     SEQUENCE OF SEQUENCE  {
             userCertificate         CertificateSerialNumber,
             revocationDate          Time,
             crlEntryExtensions      Extensions OPTIONAL
                                           -- if present, shall be v2
                                  }  OPTIONAL,
			crlExtensions           [0]  EXPLICIT Extensions OPTIONAL
                                           -- if present, shall be v2
		}
	*/
	if (getAsnSequence(&p, (uint32)(end - p), &glen) < 0) {
		psTraceCrypto("Initial parse error in psX509ParseCrl\n");
		return PS_PARSE_FAIL;
	}
	
	sigStart = p;
	if (getAsnSequence(&p, (uint32)(end - p), &glen) < 0) {
		psTraceCrypto("Initial parse error in psX509ParseCrl\n");
		return PS_PARSE_FAIL;
	}
	if (*p == ASN_INTEGER) {
		version = 0;
		if (getAsnInteger(&p, (uint32)(end - p), &version) < 0 || version != 1){
			psTraceIntCrypto("Version parse error in psX509ParseCrl %d\n",
				version);
			return PS_PARSE_FAIL;
		}
	}
	/* signature */
	if (getAsnAlgorithmIdentifier(&p, (int32)(end - p), &oi, 0, &plen) < 0) {
		psTraceCrypto("Couldn't parse crl sig algorithm identifier\n");
		return PS_PARSE_FAIL;
	}
		
	/*
		Name            ::=   CHOICE { -- only one possibility for now --
                                 rdnSequence  RDNSequence }

		RDNSequence     ::=   SEQUENCE OF RelativeDistinguishedName

		DistinguishedName       ::=   RDNSequence

		RelativeDistinguishedName  ::=
                    SET SIZE (1 .. MAX) OF AttributeTypeAndValue
	*/
	memset(&issuer, 0x0, sizeof(x509DNattributes_t));
	if ((rc = psX509GetDNAttributes(pool, &p, (uint32)(end - p),
			&issuer, 0)) < 0) {
		psTraceCrypto("Couldn't parse crl issuer DN attributes\n");
		return rc;
	}
	if (memcmp(issuer.hash, CA->subject.hash, SHA1_HASH_SIZE) != 0) {
		psTraceCrypto("CRL NOT ISSUED BY THIS CA\n");
		psX509FreeDNStruct(&issuer);
		return PS_CERT_AUTH_FAIL_DN;
	}
	psX509FreeDNStruct(&issuer);
	
	/* thisUpdate TIME */
	if ((end - p) < 1 || ((*p != ASN_UTCTIME) && (*p != ASN_GENERALIZEDTIME))) {
		psTraceCrypto("Malformed thisUpdate CRL\n");
		return PS_PARSE_FAIL;
	}
	p++;
	if (getAsnLength(&p, (uint32)(end - p), &timelen) < 0 ||
			(uint32)(end - p) < timelen) {
		psTraceCrypto("Malformed thisUpdate CRL\n");
		return PS_PARSE_FAIL;
	}	
	p += timelen;	/* Skip it */
	/* nextUpdateTIME - Optional */
	if ((end - p) < 1 || ((*p == ASN_UTCTIME) || (*p == ASN_GENERALIZEDTIME))) {
		p++;
		if (getAsnLength(&p, (uint32)(end - p), &timelen) < 0 ||
				(uint32)(end - p) < timelen) {
			psTraceCrypto("Malformed nextUpdateTIME CRL\n");
			return PS_PARSE_FAIL;
		}	
		p += timelen;	/* Skip it */
	}
	/* 
		revokedCertificates     SEQUENCE OF SEQUENCE  {
             userCertificate         CertificateSerialNumber,
             revocationDate          Time,
             crlEntryExtensions      Extensions OPTIONAL
                                           -- if present, shall be v2
                                  }  OPTIONAL,
	*/
	if (getAsnSequence(&p, (uint32)(end - p), &glen) < 0) {
		psTraceCrypto("Initial revokedCertificates error in psX509ParseCrl\n");
		return PS_PARSE_FAIL;
	}
	
	if (CA->revoked) {
		/* Append or refresh */
		if (append == 0) { 
			/* refresh */
			x509FreeRevoked(&CA->revoked);
			CA->revoked = curr = psMalloc(pool, sizeof(x509revoked_t));
			if (curr == NULL) {
				return PS_MEM_FAIL;
			}
		} else {
			/* append.  not looking for duplicates */
			curr = psMalloc(pool, sizeof(x509revoked_t));
			if (curr == NULL) {
				return PS_MEM_FAIL;
			}
			next = CA->revoked;
			while (next->next != NULL) {
				next = next->next;
			}
			next->next = curr;
		}
	} else {
		CA->revoked = curr = psMalloc(pool, sizeof(x509revoked_t));
		if (curr == NULL) {
			return PS_MEM_FAIL;
		}
	}
	memset(curr, 0x0, sizeof(x509revoked_t));
	
	
	
	while (glen > 0) {
		revStart = p;
		if (getAsnSequence(&p, (uint32)(end - p), &ilen) < 0) {
			psTraceCrypto("Deep revokedCertificates error in psX509ParseCrl\n");
			return PS_PARSE_FAIL;
		}
		start = p;
		if ((rc = getSerialNum(pool, &p, (uint32)(end - p), &curr->serial,
				&curr->serialLen)) < 0) {
			psTraceCrypto("ASN serial number parse error\n");
			return rc;
		}
		/* skipping time and extensions */
		p += ilen - (uint32)(p - start);
		glen -= (uint32)(p - revStart);
		
		// psTraceBytes("revoked", curr->serial, curr->serialLen);
		if (glen > 0) {
			if ((next = psMalloc(pool, sizeof(x509revoked_t))) == NULL) {
				x509FreeRevoked(&CA->revoked);
				return PS_MEM_FAIL;
			}
			memset(next, 0x0, sizeof(x509revoked_t));
			curr->next = next;
			curr = next;
		}
	}
	memset(&ext, 0x0, sizeof(x509v3extensions_t));
	if (getExplicitExtensions(pool, &p, (uint32)(end - p), 0, &ext) < 0) {
		psTraceCrypto("Extension parse error in psX509ParseCrl\n");
		x509FreeRevoked(&CA->revoked);
		return PS_PARSE_FAIL;
	}
	x509FreeExtensions(&ext);
	sigEnd = p;
	
	if (getAsnAlgorithmIdentifier(&p, (int32)(end - p), &oi, 0, &plen) < 0) {
		x509FreeRevoked(&CA->revoked);
		psTraceCrypto("Couldn't parse crl sig algorithm identifier\n");
		return PS_PARSE_FAIL;
	}
	
	if ((rc = psX509GetSignature(pool, &p, (uint32)(end - p), &revStart, &ilen))
			< 0) {
		x509FreeRevoked(&CA->revoked);		
		psTraceCrypto("Couldn't parse signature\n");
		return rc;
	}
	
	if (oi == OID_MD5_RSA_SIG) {
		sigLen = MD5_HASH_SIZE;
		psMd5Init(&hashCtx);
		psMd5Update(&hashCtx, sigStart, (uint32)(sigEnd - sigStart));
		psMd5Final(&hashCtx, sigHash);
	} else if (oi == OID_SHA1_RSA_SIG) {
		sigLen = SHA1_HASH_SIZE;
		psSha1Init(&hashCtx);
		psSha1Update(&hashCtx, sigStart, (uint32)(sigEnd - sigStart));
		psSha1Final(&hashCtx, sigHash);
	}
	else {
		psTraceCrypto("Need more signatuare alg support for CRL\n");
		x509FreeRevoked(&CA->revoked);	
		return PS_UNSUPPORTED_FAIL;
	}
	


	if ((rc = pubRsaDecryptSignedElement(pkiPool, &(CA->publicKey),
			revStart, ilen, sigOut, sigLen, NULL)) < 0) {
		x509FreeRevoked(&CA->revoked);
		psTraceCrypto("Unable to RSA decrypt CRL signature\n");
		return rc;					
	}
	
			
	if (memcmp(sigHash, sigOut, sigLen) != 0) {
		x509FreeRevoked(&CA->revoked);	
		psTraceCrypto("Unable to verify CRL signature\n");
		return PS_CERT_AUTH_FAIL_SIG;					
	}
	
	return PS_SUCCESS;
}
#endif /* USE_CRL */
#endif /* USE_CERT_PARSE */

#endif /* USE_X509 */
/******************************************************************************/


