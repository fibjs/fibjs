/*
 *	x509.h
 *	Release $Name: MATRIXSSL-3-3-1-OPEN $
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

#ifndef _h_PS_X509
#define _h_PS_X509

/******************************************************************************/
#ifdef USE_X509
/******************************************************************************/

/* ClientCertificateType */
enum {
	RSA_SIGN = 1,
	DSS_SIGN,
	RSA_FIXED_DH,
	DSS_FIXED_DH,
	ECDSA_SIGN = 64,
	RSA_FIXED_ECDH,
	ECDSA_FIXED_ECDH
};

/*
	Parsing flags
*/
#define	CERT_STORE_UNPARSED_BUFFER	0x1
#define	CERT_STORE_DN_BUFFER		0x2

#ifdef USE_CERT_PARSE
/*
	DN attributes are used outside the X509 area for cert requests,
	which have been included in the RSA portions of the code
*/
typedef struct {
	char	*country;
	char	*state;
	char	*locality;
	char	*organization;
	char	*orgUnit;
	char	*commonName;
	char	hash[SHA1_HASH_SIZE];	
	char	*dnenc; /* CERT_STORE_DN_BUFFER */
	uint32	dnencLen;
	short	countryType;
	short	countryLen;
	short	stateType;
	short	stateLen;
	short	localityType;
	short	localityLen;
	short	organizationType;
	short	organizationLen;
	short	orgUnitType;
	short	orgUnitLen;
	short	commonNameType;
	short	commonNameLen;
} x509DNattributes_t;

typedef struct {
	int32	cA;
	int32	pathLenConstraint;
} x509extBasicConstraints_t;

typedef struct psGeneralNameEntry {
	int32							id;
	unsigned char					name[16];
	unsigned char					*data;
	uint32							dataLen;
	struct psGeneralNameEntry		*next;
} x509GeneralName_t;

#ifdef USE_FULL_CERT_PARSE
typedef struct {
	uint32			len;
	unsigned char	*id;
} x509extSubjectKeyId_t;

typedef struct {
	uint32				keyLen;
	unsigned char		*keyId;
	x509DNattributes_t	attribs;
	uint32				serialNumLen;
	unsigned char		*serialNum;
} x509extAuthKeyId_t;
#endif /* USE_FULL_CERT_PARSE */

typedef struct {
	x509extBasicConstraints_t	bc;
	x509GeneralName_t			*san;
#ifdef USE_FULL_CERT_PARSE
	x509extSubjectKeyId_t		sk;
	x509extAuthKeyId_t			ak;
	unsigned char				*keyUsage;
	int32						keyUsageLen;
#endif /* USE_FULL_CERT_PARSE */
} x509v3extensions_t;

#endif /* USE_CERT_PARSE */

typedef struct psCert {
#ifdef USE_CERT_PARSE
	int32				version;
	unsigned char		*serialNumber;
	uint32				serialNumberLen;
	x509DNattributes_t	issuer;
	x509DNattributes_t	subject;
	int32				timeType;
	char				*notBefore;
	char				*notAfter;
	psPubKey_t			publicKey;
	int32				pubKeyAlgorithm; /* public key algorithm OID */
	int32				certAlgorithm; /* signature algorithm OID */
	int32				sigAlgorithm; /* signature algorithm OID */
	unsigned char		*signature;
	uint32				signatureLen;
	unsigned char		sigHash[MAX_HASH_SIZE]; 
	unsigned char		*uniqueIssuerId;
	uint32				uniqueIssuerIdLen;
	unsigned char		*uniqueSubjectId;
	uint32				uniqueSubjectIdLen;
	x509v3extensions_t	extensions;
	int32				authStatus; /* See psX509AuthenticateCert doc */
#endif /* USE_CERT_PARSE */	
	unsigned char		*unparsedBin; /* see psX509ParseCertFile */ 
	uint32				binLen;
	struct psCert		*next;
} psX509Cert_t;


#ifdef USE_CERT_PARSE					
extern int32 psX509GetSignature(psPool_t *pool, unsigned char **pp, uint32 len,
					unsigned char **sig, uint32 *sigLen);
extern int32 psX509GetDNAttributes(psPool_t *pool, unsigned char **pp,
				uint32 len, x509DNattributes_t *attribs, int32 flags);
extern void psX509FreeDNStruct(x509DNattributes_t *dn);
#endif /* USE_CERT_PARSE */

#endif /* USE_X509 */
/******************************************************************************/

#endif /* _h_PS_X509 */

