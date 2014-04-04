/*
 *	matrixssl.c
 *	Release $Name: MATRIXSSL-3-4-2-OPEN $
 *
 *	The session and authentication management portions of the MatrixSSL library
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

#include "matrixsslApi.h"
/******************************************************************************/

static const char copyright[] = 
"Copyright Inside Secure Corporation. All rights reserved.";

#ifdef USE_SERVER_SIDE_SSL
static int32 verifyReadKeys(psPool_t *pool, sslKeys_t *keys);
#endif /* USE_SERVER_SIDE_SSL */


#ifdef USE_SERVER_SIDE_SSL
/*
	Static session table for session cache and lock for multithreaded env
*/
#ifdef USE_MULTITHREADING
static psMutex_t			sessionTableLock;
static psMutex_t			prngLock;
#endif /* USE_MULTITHREADING */

static sslSessionEntry_t	sessionTable[SSL_SESSION_TABLE_SIZE];
#endif /* USE_SERVER_SIDE_SSL */

#ifdef MATRIX_USE_FILE_SYSTEM
static int32 matrixSslLoadKeyMaterial(sslKeys_t *keys, const char *certFile,
				const char *privFile, const char *privPass, const char *CAfile,
				int32 privKeyType);
#endif
static int32 matrixSslLoadKeyMaterialMem(sslKeys_t *keys,
				unsigned char *certBuf,	int32 certLen, unsigned char *privBuf,
				int32 privLen, unsigned char *CAbuf, int32 CAlen,
				int32 privKeyType);

static psRandom_t gMatrixsslPrng;

 
/******************************************************************************/
/*
	Open and close the SSL module.  These routines are called once in the 
	lifetime of the application and initialize and clean up the library 
	respectively.
*/
int32 matrixSslOpen(void)
{
	/* Use copyright to avoid compiler warning about it being unused */
	if (*copyright != 'C') {
		return PS_FAILURE;
	}
	if (psCoreOpen() < 0) {
		psError("pscore open failure\n");
		return PS_FAILURE;
	}

	psInitPrng(&gMatrixsslPrng);

#ifdef USE_SERVER_SIDE_SSL
	memset(sessionTable, 0x0, 
		sizeof(sslSessionEntry_t) * SSL_SESSION_TABLE_SIZE);
#ifdef USE_MULTITHREADING		
	psCreateMutex(&sessionTableLock);
	psCreateMutex(&prngLock);
#endif /* USE_MULTITHREADING */
#endif /* USE_SERVER_SIDE_SSL */


	return PS_SUCCESS;
}

/*
	MatrixSSL PRNG retrieval
*/
int32 matrixSslGetPrngData(unsigned char *bytes, uint32 size)
{
	int32	rc;
#ifdef USE_MULTITHREADING
	psLockMutex(&prngLock);
#endif /* USE_MULTITHREADING */
	rc = psGetPrng(&gMatrixsslPrng, bytes, size);
#ifdef USE_MULTITHREADING		
	psUnlockMutex(&prngLock);
#endif /* USE_MULTITHREADING */
	return rc;
}

/*
	matrixSslClose
*/
void matrixSslClose(void)
{
#ifdef USE_SERVER_SIDE_SSL
	int32		i;

#ifdef USE_MULTITHREADING
	psLockMutex(&sessionTableLock);
#endif /* USE_MULTITHREADING */
	for (i = 0; i < SSL_SESSION_TABLE_SIZE; i++) {
		if (sessionTable[i].inUse > 1) {
			psTraceInfo("Warning: closing while session still in use\n");
		}
	}
	memset(sessionTable, 0x0, 
		sizeof(sslSessionEntry_t) * SSL_SESSION_TABLE_SIZE);
#ifdef USE_MULTITHREADING		
	psUnlockMutex(&sessionTableLock);
	psDestroyMutex(&sessionTableLock);
	psDestroyMutex(&prngLock);
#endif /* USE_MULTITHREADING */	
#endif /* USE_SERVER_SIDE_SSL */

	psCoreClose();
}

/******************************************************************************/
/*
	Must call to allocate the key structure now.  After which, LoadRsaKeys,
	LoadDhParams and/or LoadPskKey can be called 
	
	Memory info:
	Caller must free keys with matrixSslDeleteKeys on function success
	Caller does not need to free keys on function failure
*/
int32 matrixSslNewKeys(sslKeys_t **keys)
{
	psPool_t	*pool = NULL;
	sslKeys_t	*lkeys;
	
	
	lkeys = psMalloc(pool, sizeof(sslKeys_t));
	if (lkeys == NULL) {
		return PS_MEM_FAIL;
	}
	memset(lkeys, 0x0, sizeof(sslKeys_t));
	lkeys->pool = pool;
	
	*keys = lkeys;
	return PS_SUCCESS;
}


#ifdef MATRIX_USE_FILE_SYSTEM
#ifdef USE_PKCS12

/* Have seen cases where the PKCS#12 files are not in a child-to-parent order */
static void ReorderCertChain(psX509Cert_t *a_cert)
{
	psX509Cert_t* prevCert = NULL;
	psX509Cert_t* nextCert = NULL;
	psX509Cert_t* currCert = a_cert;
 
	while (currCert) {
		nextCert = currCert->next;
        while (nextCert && memcmp(currCert->issuer.hash, nextCert->subject.hash,
				SHA1_HASH_SIZE) != 0) {
			prevCert = nextCert;
			nextCert = nextCert->next;
 
			if (nextCert && memcmp(currCert->issuer.hash,
					nextCert->subject.hash, SHA1_HASH_SIZE) == 0) {
				prevCert->next = nextCert->next;
				nextCert->next = currCert->next;
				currCert->next = nextCert;
				break;
			}
		}
		currCert = currCert->next;
	}
}

/******************************************************************************/
/*
	File should be a binary .p12 or .pfx 
*/
int32 matrixSslLoadPkcs12(sslKeys_t *keys, unsigned char *certFile,
			unsigned char *importPass, int32 ipasslen,
			unsigned char *macPass, int32 mpasslen, int32 flags)
{
	unsigned char	*mPass;
	psPool_t	*pool;
	int32		rc;

	if (keys == NULL) {
		return PS_ARG_FAIL;
	}
	pool = keys->pool;
	
	if (macPass == NULL) {
		mPass = importPass;
		mpasslen = ipasslen;
	} else {
		mPass = macPass;
	}
		
	if ((rc = psPkcs12Parse(pool, &keys->cert, &keys->privKey, certFile, flags,
			importPass, ipasslen, mPass, mpasslen)) < 0) {
		if (keys->cert) {
			psX509FreeCert(keys->cert);
			keys->cert = NULL;
		}
		if (keys->privKey) {
			psFreePubKey(keys->privKey);
			keys->privKey = NULL;
		}
		return rc;
	}
	ReorderCertChain(keys->cert);
#ifdef USE_SERVER_SIDE_SSL
	if (verifyReadKeys(pool, keys) < PS_SUCCESS) {
		psTraceInfo("PKCS#12 parse success but material didn't validate\n");
		psX509FreeCert(keys->cert);
		psFreePubKey(keys->privKey);
		keys->cert = NULL;
		keys->privKey = NULL;
		return PS_CERT_AUTH_FAIL;
	}
#endif /* USE_SERVER_SIDE_SSL */	
	return PS_SUCCESS;
}
#endif /* USE_PKCS12 */

int32 matrixSslLoadRsaKeys(sslKeys_t *keys, const char *certFile,
				const char *privFile, const char *privPass, const char *CAfile)
{
	return matrixSslLoadKeyMaterial(keys, certFile, privFile, privPass, CAfile,
				PS_RSA);

}


static int32 matrixSslLoadKeyMaterial(sslKeys_t *keys, const char *certFile,
				const char *privFile, const char *privPass, const char *CAfile,
				int32 privKeyType)
{
	psPool_t	*pool;
	int32		err, flags;

	if (keys == NULL) {
		return PS_ARG_FAIL;
	}
	pool = keys->pool;

/*
	Setting flags to store raw ASN.1 stream for SSL CERTIFICATE message use
*/
	flags = CERT_STORE_UNPARSED_BUFFER;
	
#ifdef USE_CLIENT_AUTH
/*
	 If the CERTIFICATE_REQUEST message will possibly be needed we must
	 save aside the Distiguished Name portion of the certs for that message.
*/
	flags |= CERT_STORE_DN_BUFFER;
#endif /* USE_CLIENT_AUTH */

	if (certFile) {
#ifdef USE_SERVER_SIDE_SSL	
		if (keys->cert != NULL) {
			return PS_UNSUPPORTED_FAIL;
		}
		if ((err = psX509ParseCertFile(pool, (char*)certFile,
				&keys->cert, flags)) < 0) {
			return err;
		}
#else
		psTraceStrInfo("Ignoring %s certFile in matrixSslReadKeys\n",
					(char*)certFile);
#endif /* USE_SERVER_SIDE_SSL */		
	}
/*
	Parse the private key file
*/
	if (privFile) {
#ifdef USE_SERVER_SIDE_SSL
		if (keys->privKey != NULL) {
			if (keys->cert) {
				psX509FreeCert(keys->cert);
				keys->cert = NULL;
			}
			return PS_UNSUPPORTED_FAIL;
		}
		if (privKeyType == PS_RSA) {
			if ((err = pkcs1ParsePrivFile(pool, (char*)privFile,
					(char*)privPass, &keys->privKey)) < 0) {
				if (keys->cert) {	
					psX509FreeCert(keys->cert);
					keys->cert = NULL;
				}
				return err;
			}
		}
#else
		psTraceStrInfo("Ignoring %s privFile in matrixSslReadKeys\n",
					(char*)privFile);
#endif /* USE_SERVER_SIDE_SSL */			
	}

#ifdef USE_SERVER_SIDE_SSL
	if (verifyReadKeys(pool, keys) < PS_SUCCESS) {
		psTraceInfo("Cert parse success but material didn't validate\n");
		psX509FreeCert(keys->cert);
		psFreePubKey(keys->privKey);
		keys->cert = NULL;
		keys->privKey = NULL;
		return PS_CERT_AUTH_FAIL;
	}
#endif /* USE_SERVER_SIDE_SSL */
	
/*
	Not necessary to store binary representations of CA certs
*/
	flags &= ~CERT_STORE_UNPARSED_BUFFER;

	if (CAfile) {	
#ifdef USE_CLIENT_SIDE_SSL
		if (keys->CAcerts != NULL) {
			return PS_UNSUPPORTED_FAIL;
		}
		if ((err = psX509ParseCertFile(pool, (char*)CAfile,
				&keys->CAcerts, flags)) < 0) {
#ifdef USE_SERVER_SIDE_SSL				
			if (keys->cert) {
				psX509FreeCert(keys->cert);
				keys->cert = NULL;
			} 
			if (keys->privKey) {
				psFreePubKey(keys->privKey);
				keys->privKey = NULL;
			}
#endif /* USE_SERVER_SIDE_SSL */	
			return err;
		}
#else
		psTraceStrInfo("Ignoring %s CAfile in matrixSslReadKeys\n", (char*)CAfile);		
#endif /* USE_CLIENT_SIDE_SSL */
	}

	return PS_SUCCESS;
}
#endif /* MATRIX_USE_FILE_SYSTEM */

/******************************************************************************/
/*
	Memory buffer versions of ReadKeys

	This function supports cert chains and multiple CAs.  Just need to
	string them together and let psX509ParseCert handle it 
*/
int32 matrixSslLoadRsaKeysMem(sslKeys_t *keys, unsigned char *certBuf,
			int32 certLen, unsigned char *privBuf, int32 privLen,
			unsigned char *CAbuf, int32 CAlen)
{
	return matrixSslLoadKeyMaterialMem(keys, certBuf, certLen, privBuf, privLen,
				CAbuf, CAlen, PS_RSA);

}


static int32 matrixSslLoadKeyMaterialMem(sslKeys_t *keys,
				unsigned char *certBuf,	int32 certLen, unsigned char *privBuf,
				int32 privLen, unsigned char *CAbuf, int32 CAlen,
				int32 privKeyType)
{
	psPool_t	*pool;
	int32		err, flags = 0;

	if (keys == NULL) {
		return PS_ARG_FAIL;
	}
	pool = keys->pool;
	
/*
	Setting flags to store raw ASN.1 stream for SSL CERTIFICATE message use
*/
	flags = CERT_STORE_UNPARSED_BUFFER;
	
#ifdef USE_CLIENT_AUTH
/*
	Setting flag to store raw ASN.1 DN stream for CERTIFICATE_REQUEST
*/
	flags |= CERT_STORE_DN_BUFFER;
#endif /* USE_CLIENT_AUTH */
	
	if (certBuf) {	
#ifdef USE_SERVER_SIDE_SSL
		if (keys->cert != NULL) {
			psTraceInfo("WARNING: An identity certificate already exists\n");
			return PS_UNSUPPORTED_FAIL;
		}
		if ((err = psX509ParseCert(pool, certBuf, (uint32)certLen, &keys->cert,
				flags)) < 0) {
			psX509FreeCert(keys->cert);
			keys->cert = NULL;
			return err;
		}
#else
		psTraceInfo("Ignoring certBuf in matrixSslReadKeysMem\n");
#endif /* USE_SERVER_SIDE_SSL */			
	}
	
	if (privBuf) {
#ifdef USE_SERVER_SIDE_SSL
		if (keys->privKey != NULL) {
			return PS_UNSUPPORTED_FAIL;
		}
		if (privKeyType == PS_RSA) {		
			if ((err = pkcs1ParsePrivBin(pool, privBuf, (uint32)privLen,
					&keys->privKey)) < 0) {
#ifdef USE_PKCS8
				/* Attempt a PKCS#8 but mem parse doesn't take password */ 
				if ((err = pkcs8ParsePrivBin(pool, privBuf, (uint32)privLen,
						NULL, &keys->privKey)) < 0) {
					psX509FreeCert(keys->cert); keys->cert = NULL;
					return err;
				}
#else					
				psX509FreeCert(keys->cert); keys->cert = NULL;
				return err;
#endif
			}
		}

#else
		psTraceInfo("Ignoring privBuf in matrixSslReadKeysMem\n");
#endif /* USE_SERVER_SIDE_SSL */		
	}
	
#ifdef USE_SERVER_SIDE_SSL
	if (verifyReadKeys(pool, keys) < PS_SUCCESS) {
		psX509FreeCert(keys->cert);
		psFreePubKey(keys->privKey);
		keys->privKey = NULL;
		keys->cert = NULL;
		return PS_CERT_AUTH_FAIL;
	}
#endif /* USE_SERVER_SIDE_SSL */

/*
	 Not necessary to store binary representations of CA certs
*/
	flags &= ~CERT_STORE_UNPARSED_BUFFER;	
	
	if (CAbuf) {
#ifdef USE_CLIENT_SIDE_SSL
		if (keys->CAcerts != NULL) {
			return PS_UNSUPPORTED_FAIL;
		}
		if ((err = psX509ParseCert(pool, CAbuf, (uint32)CAlen, &keys->CAcerts,
				flags)) < 0) {
#ifdef USE_SERVER_SIDE_SSL		
			psFreePubKey(keys->privKey);
			psX509FreeCert(keys->cert);
			psX509FreeCert(keys->CAcerts);
			keys->privKey = NULL;
			keys->cert = keys->CAcerts = NULL;
#endif			
			return err;
		}
#else
		psTraceInfo("Ignoring CAbuf in matrixSslReadKeysMem\n");		
#endif /* USE_CLIENT_SIDE_SSL */
	}

	return PS_SUCCESS;
}

/******************************************************************************/
/*
	This will free the struct and any key material that was loaded via:
		matrixSslLoadRsaKeys
		matrixSslLoadDhParams
		matrixSslLoadPsk	
*/
void matrixSslDeleteKeys(sslKeys_t *keys)
{
	
	if (keys == NULL) {
		return;
	}
#ifdef USE_SERVER_SIDE_SSL	
	if (keys->cert) {
		psX509FreeCert(keys->cert);
	}
	
	if (keys->privKey) {
		psFreePubKey(keys->privKey);
	}
#endif /* USE_SERVER_SIDE_SSL */
	
#ifdef USE_CLIENT_SIDE_SSL
	if (keys->CAcerts) {
		psX509FreeCert(keys->CAcerts);
	}
#endif /* USE_CLIENT_SIDE_SSL */



	psFree(keys);
}

#ifdef USE_SERVER_SIDE_SSL
/*
	Validate the cert chain and the private key for the material passed
	to matrixSslReadKeys.  Good to catch any user certifiate errors as
	soon as possible
*/
static int32 verifyReadKeys(psPool_t *pool, sslKeys_t *keys)
{
#ifdef USE_CERT_PARSE
	psX509Cert_t	*tmp;
#endif

	if (keys->cert == NULL && keys->privKey == NULL) {
		return PS_SUCCESS;
	}	
/*
	 Not allowed to have a certficate with no matching private key or 
	 private key with no cert to match with
*/
	if (keys->cert != NULL && keys->privKey == NULL) {
		psTraceInfo("No private key given to matrixSslReadKeys cert\n");
		return PS_CERT_AUTH_FAIL;
	}
	if (keys->privKey != NULL && keys->cert == NULL) {
		psTraceInfo("No cert given with private key to matrixSslReadKeys\n");
		return PS_CERT_AUTH_FAIL;
	}
#ifdef USE_CERT_PARSE	
/*
	If this is a chain, we can validate it here with psX509AuthenticateCert
	Don't check the error return code from this call because the chaining
	usage restrictions will test parent-most cert for self-signed.
	 
	But we can look at 'authStatus' on all but the final cert to see
	if the rest looks good
*/
	if (keys->cert != NULL && keys->cert->next != NULL) {
		psX509AuthenticateCert(pool, keys->cert, NULL);
		tmp = keys->cert;
		while (tmp->next != NULL) {
			if (tmp->authStatus != PS_TRUE) {
				psTraceInfo("Failed to authenticate cert chain\n");
				return PS_CERT_AUTH_FAIL;
			}
			tmp = tmp->next;
		}
	}

	if (keys->privKey != NULL && keys->privKey->type == PS_RSA) {
/*
		Testing the N member just as a sanity measure rather than
		attempting a full RSA crypt operation
*/
		if (pstm_cmp(&(keys->privKey->key->rsa.N),
				   &(keys->cert->publicKey.key->rsa.N)) != PSTM_EQ) {
			psTraceInfo("Private key doesn't match cert\n");	
			return PS_CERT_AUTH_FAIL;
		}
	}
#endif /* USE_CERT_PARSE */	
	return PS_SUCCESS;
}
#endif	/* USE_SERVER_SIDE_SSL */
/******************************************************************************/



/******************************************************************************/
/*
	New SSL protocol context
	This structure is associated with a single SSL connection.  Each socket
	using SSL should be associated with a new SSL context.

	certBuf and privKey ARE NOT duplicated within the server context, in order
	to minimize memory usage with multiple simultaneous requests.  They must 
	not be deleted by caller until all server contexts using them are deleted.
*/
int32 matrixSslNewSession(ssl_t **ssl, sslKeys_t *keys, sslSessionId_t *session,
						int32 flags)
{
	psPool_t	*pool = NULL;
	ssl_t		*lssl;

/*
	First API level chance to make sure a user is not attempting to use
	client or server support that was not built into this library compile
*/
#ifndef USE_SERVER_SIDE_SSL
	if (flags & SSL_FLAGS_SERVER) {
		psTraceInfo("SSL_FLAGS_SERVER passed to matrixSslNewSession but MatrixSSL lib was not compiled with server support\n");
		return PS_ARG_FAIL;
	}
#endif

#ifndef USE_CLIENT_SIDE_SSL
	if (!(flags & SSL_FLAGS_SERVER)) {
		psTraceInfo("SSL_FLAGS_SERVER was not passed to matrixSslNewSession but MatrixSSL was not compiled with client support\n");
		return PS_ARG_FAIL;
	}
#endif

#ifndef USE_CLIENT_AUTH
	if (flags & SSL_FLAGS_CLIENT_AUTH) {
		psTraceInfo("SSL_FLAGS_CLIENT_AUTH passed to matrixSslNewSession but MatrixSSL was not compiled with USE_CLIENT_AUTH enabled\n");
		return PS_ARG_FAIL;
	}
#endif

	if (flags & SSL_FLAGS_SERVER) {
		if (keys == NULL) {
			psTraceInfo("NULL keys parameter passed to matrixSslNewSession\n");
			return PS_ARG_FAIL;
		}
		if (session != NULL) {
			psTraceInfo("Ignoring session parameter to matrixSslNewSession\n");
		}
	}

	*ssl = lssl = psMalloc(pool, sizeof(ssl_t));
	if (lssl == NULL) {
		psTraceInfo("Out of memory for ssl_t in matrixSslNewSession\n");
		return PS_MEM_FAIL;
	}
	memset(lssl, 0x0, sizeof(ssl_t));


/*
	Data buffers
*/
	lssl->outsize = SSL_DEFAULT_OUT_BUF_SIZE;
	lssl->outbuf = psMalloc(MATRIX_NO_POOL, lssl->outsize);
	if (lssl->outbuf == NULL) {
		psTraceInfo("Out of memory for outbuf in matrixSslNewSession\n");
		psFree(lssl);
		return PS_MEM_FAIL;
	}
	lssl->insize = SSL_DEFAULT_IN_BUF_SIZE;
	lssl->inbuf = psMalloc(MATRIX_NO_POOL, lssl->insize);
	if (lssl->inbuf == NULL) {
		psTraceInfo("Out of memory for inbuf in matrixSslNewSession\n");
		psFree(lssl->outbuf);
		psFree(lssl);
		return PS_MEM_FAIL;
	}

	lssl->sPool = pool;
	lssl->keys = keys;
	lssl->cipher = sslGetCipherSpec(lssl, SSL_NULL_WITH_NULL_NULL);
	sslActivateReadCipher(lssl);
	sslActivateWriteCipher(lssl);
	
	lssl->recordHeadLen = SSL3_HEADER_LEN;
	lssl->hshakeHeadLen = SSL3_HANDSHAKE_HEADER_LEN;
		

	if (flags & SSL_FLAGS_SERVER) {
		lssl->flags |= SSL_FLAGS_SERVER;
/*
		Client auth can only be requested by server, not set by client
*/
		if (flags & SSL_FLAGS_CLIENT_AUTH) {
			lssl->flags |= SSL_FLAGS_CLIENT_AUTH;
		}
		lssl->hsState = SSL_HS_CLIENT_HELLO;
	} else {
/*
		Client is first to set protocol version information based on
		compile and/or the 'flags' parameter so header information in
		the handshake messages will be correctly set.
*/
#ifdef USE_TLS
#ifndef DISABLE_TLS_1_0
		lssl->majVer = TLS_MAJ_VER;
		lssl->minVer = TLS_MIN_VER;
#endif		
#if defined(USE_TLS_1_1) && !defined(DISABLE_TLS_1_1)
		lssl->majVer = TLS_MAJ_VER;
		lssl->minVer = TLS_1_1_MIN_VER;
		lssl->flags |= SSL_FLAGS_TLS_1_1;
#endif /* USE_TLS_1_1 */
		if (lssl->majVer == 0) {
			/* USE_TLS enabled but all DISABLE_TLS versions are enabled so
				use SSLv3.  Compile time tests would catch if no versions are
				enabled at all */
			lssl->majVer = SSL3_MAJ_VER;
			lssl->minVer = SSL3_MIN_VER;
		} else {
			lssl->flags |= SSL_FLAGS_TLS;
		}
		

#else /* USE_TLS */
		lssl->majVer = SSL3_MAJ_VER;
		lssl->minVer = SSL3_MIN_VER;
#endif /* USE_TLS */


		lssl->hsState = SSL_HS_SERVER_HELLO;
		if (session != NULL && session->cipherId != SSL_NULL_WITH_NULL_NULL) {
			lssl->cipher = sslGetCipherSpec(lssl, session->cipherId);
			if (lssl->cipher == NULL) {
				psTraceInfo("Invalid session id to matrixSslNewSession\n");
			} else {
				memcpy(lssl->sec.masterSecret, session->masterSecret, 
					SSL_HS_MASTER_SIZE);
				lssl->sessionIdLen = SSL_MAX_SESSION_ID_SIZE;
				memcpy(lssl->sessionId, session->id, SSL_MAX_SESSION_ID_SIZE);
			}
		}
		lssl->sid = session;
	}
	lssl->err = SSL_ALERT_NONE;

	return PS_SUCCESS;
}


/******************************************************************************/
/*
	Delete an SSL session.  Some information on the session may stay around
	in the session resumption cache.
	SECURITY - We memset relevant values to zero before freeing to reduce 
	the risk of our keys floating around in memory after we're done.
*/
void matrixSslDeleteSession(ssl_t *ssl)
{

	if (ssl == NULL) {
		return;
	}




	ssl->flags |= SSL_FLAGS_CLOSED;
/*
	If we have a sessionId, for servers we need to clear the inUse flag in 
	the session cache so the ID can be replaced if needed.  In the client case
	the caller should have called matrixSslGetSessionId already to copy the
	master secret and sessionId, so free it now.

	In all cases except a successful updateSession call on the server, the
	master secret must be freed.
*/
#ifdef USE_SERVER_SIDE_SSL
	if (ssl->sessionIdLen > 0 && (ssl->flags & SSL_FLAGS_SERVER)) {
		matrixUpdateSession(ssl);
	}
#endif /* USE_SERVER_SIDE_SSL */
	ssl->sessionIdLen = 0;

#ifdef USE_CLIENT_SIDE_SSL
	if (ssl->sec.cert) {
		psX509FreeCert(ssl->sec.cert);
		ssl->sec.cert = NULL;
	}

#endif /* USE_CLIENT_SIDE_SSL */



/*
	Premaster could also be allocated if this DeleteSession is the result
	of a failed handshake.  This test is fine since all frees will NULL pointer
*/
	if (ssl->sec.premaster) {
		psFree(ssl->sec.premaster);
	}
	if (ssl->fragMessage) {
		psFree(ssl->fragMessage);
	}




/*
	Free the data buffers
*/
	psFree(ssl->outbuf);
	psFree(ssl->inbuf);
	
/*
	The cipher and mac contexts are inline in the ssl structure, so
	clearing the structure clears those states as well.
*/
	memset(ssl, 0x0, sizeof(ssl_t));
	psFree(ssl);
}


/******************************************************************************/
/*
	Generic session option control for changing already connected sessions.
	(ie. rehandshake control).  arg param is future for options that may
	require a value.
*/
void matrixSslSetSessionOption(ssl_t *ssl, int32 option, void *arg)
{
	if (option == SSL_OPTION_FULL_HANDSHAKE) {
#ifdef USE_SERVER_SIDE_SSL
		if (ssl->flags & SSL_FLAGS_SERVER) {
			matrixClearSession(ssl, 1);
		}
#endif /* USE_SERVER_SIDE_SSL */
		ssl->sessionIdLen = 0;
		memset(ssl->sessionId, 0x0, SSL_MAX_SESSION_ID_SIZE);
	}
		
#if defined(USE_CLIENT_AUTH) && defined(USE_SERVER_SIDE_SSL) 
	if (ssl->flags & SSL_FLAGS_SERVER) {
		if (option == SSL_OPTION_DISABLE_CLIENT_AUTH) {
			ssl->flags &= ~SSL_FLAGS_CLIENT_AUTH;
		} else if (option == SSL_OPTION_ENABLE_CLIENT_AUTH) {
			ssl->flags |= SSL_FLAGS_CLIENT_AUTH;
			matrixClearSession(ssl, 1);
		}
	}
#endif /* USE_CLIENT_AUTH && USE_SERVER_SIDE_SSL */
}

/******************************************************************************/
/*
	Will be true if the cipher suite is an 'anon' variety OR if the
	user certificate callback returned SSL_ALLOW_ANON_CONNECTION
*/
void matrixSslGetAnonStatus(ssl_t *ssl, int32 *certArg)
{
	*certArg = ssl->sec.anon;
}


#ifdef USE_SSL_INFORMATIONAL_TRACE
void matrixSslPrintHSDetails(ssl_t *ssl)
{
	if (ssl->hsState == SSL_HS_DONE) {
		psTraceInfo("\n");
		if (ssl->minVer == SSL3_MIN_VER) {
			psTraceInfo("SSL 3.0 ");
		} else if (ssl->minVer == TLS_MIN_VER) {
			psTraceInfo("TLS 1.0 ");
		} else if (ssl->minVer == TLS_1_1_MIN_VER) {
			psTraceInfo("TLS 1.1 ");
		} else if (ssl->minVer == TLS_1_2_MIN_VER) {
			psTraceInfo("TLS 1.2 ");
		}
		psTraceInfo("connection established: ");
		switch (ssl->cipher->ident) {
			case SSL_RSA_WITH_NULL_MD5:
				psTraceInfo("SSL_RSA_WITH_NULL_MD5\n");
				break;
			case SSL_RSA_WITH_NULL_SHA:
				psTraceInfo("SSL_RSA_WITH_NULL_SHA\n");
				break;
			case SSL_RSA_WITH_RC4_128_MD5:
				psTraceInfo("SSL_RSA_WITH_RC4_128_MD5\n");
				break;
			case SSL_RSA_WITH_RC4_128_SHA:
				psTraceInfo("SSL_RSA_WITH_RC4_128_SHA\n");
				break;
			case SSL_RSA_WITH_3DES_EDE_CBC_SHA:
				psTraceInfo("SSL_RSA_WITH_3DES_EDE_CBC_SHA\n");
				break;
			case TLS_RSA_WITH_AES_128_CBC_SHA:
				psTraceInfo("TLS_RSA_WITH_AES_128_CBC_SHA\n");
				break;
			case TLS_RSA_WITH_AES_256_CBC_SHA:
				psTraceInfo("TLS_RSA_WITH_AES_256_CBC_SHA\n");
				break;
			default:
				psTraceIntInfo("!!!! DEFINE ME %d !!!!\n", ssl->cipher->ident);
		}
		//psTraceBytes("	Master Secret", ssl->sec.masterSecret,
		//	SSL_HS_MASTER_SIZE);
	}
	return;
}
#endif

/******************************************************************************/
/*
	Returns PS_TRUE if we've completed the SSL handshake.
*/
int32 matrixSslHandshakeIsComplete(ssl_t *ssl)
{	
	return (ssl->hsState == SSL_HS_DONE) ? PS_TRUE : PS_FALSE;
}

#ifdef USE_CLIENT_SIDE_SSL
/******************************************************************************/
/*
	Set a custom callback to receive the certificate being presented to the
	session to perform custom authentication if needed

	NOTE: Must define either USE_CLIENT_SIDE_SSL or USE_CLIENT_AUTH
	in matrixConfig.h
*/
void matrixSslSetCertValidator(ssl_t *ssl,
			int32 (*certValidator)(void *, psX509Cert_t *, int32))
{
	if ((ssl != NULL) && (certValidator != NULL)) {
		ssl->sec.validateCert = certValidator;
	}
}
#endif /* USE_CLIENT_SIDE_SSL */

#ifdef USE_SERVER_SIDE_SSL
/******************************************************************************/
/*
	Register a session in the session resumption cache.  If successful (rc >=0),
	the ssl sessionId and sessionIdLength fields will be non-NULL upon
	return.
*/
int32 matrixRegisterSession(ssl_t *ssl)
{
	uint32		i, j;
	psTime_t	t;

	if (!(ssl->flags & SSL_FLAGS_SERVER)) {
		return PS_FAILURE;
	}

/*
	Iterate the session table, looking for an empty entry (cipher null), or
	the oldest entry that is not in use
*/
#ifdef USE_MULTITHREADING
	psLockMutex(&sessionTableLock);
#endif /* USE_MULTITHREADING */
	j = SSL_SESSION_TABLE_SIZE;
	t = sessionTable[0].accessTime;
	for (i = 0; i < SSL_SESSION_TABLE_SIZE; i++) {
		if (sessionTable[i].cipher == NULL) {
			break;
		}
		if (psCompareTime(sessionTable[i].accessTime, t) &&
				sessionTable[i].inUse == 0) {
			t = sessionTable[i].accessTime;
			j = i;
		}
	}
/*
	If there were no empty entries, get the oldest unused entry.
	If all entries are in use, return -1, meaning we can't cache the
	session at this time
*/
	if (i >= SSL_SESSION_TABLE_SIZE) {
		if (j < SSL_SESSION_TABLE_SIZE) {
			i = j;
		} else {
#ifdef USE_MULTITHREADING		
			psUnlockMutex(&sessionTableLock);
#endif /* USE_MULTITHREADING */			
			return PS_LIMIT_FAIL;
		}
	}
/*
	Register the incoming masterSecret and cipher, which could still be null, 
	depending on when we're called.
*/
	memcpy(sessionTable[i].masterSecret, ssl->sec.masterSecret,
		SSL_HS_MASTER_SIZE);
	sessionTable[i].cipher = ssl->cipher;
	sessionTable[i].inUse += 1;
#ifdef USE_MULTITHREADING	
	psUnlockMutex(&sessionTableLock);
#endif /* USE_MULTITHREADING */		
/*
	The sessionId is the current serverRandom value, with the first 4 bytes
	replaced with the current cache index value for quick lookup later.
	FUTURE SECURITY - Should generate more random bytes here for the session
	id.  We re-use the server random as the ID, which is OK, since it is
	sent plaintext on the network, but an attacker listening to a resumed
	connection will also be able to determine part of the original server
	random used to generate the master key, even if he had not seen it
	initially.
*/
	memcpy(sessionTable[i].id, ssl->sec.serverRandom, 
		min(SSL_HS_RANDOM_SIZE, SSL_MAX_SESSION_ID_SIZE));
	ssl->sessionIdLen = SSL_MAX_SESSION_ID_SIZE;
	sessionTable[i].id[0] = (unsigned char)(i & 0xFF);
	sessionTable[i].id[1] = (unsigned char)((i & 0xFF00) >> 8);
	sessionTable[i].id[2] = (unsigned char)((i & 0xFF0000) >> 16);
	sessionTable[i].id[3] = (unsigned char)((i & 0xFF000000) >> 24);
	memcpy(ssl->sessionId, sessionTable[i].id, SSL_MAX_SESSION_ID_SIZE);
/*
	startTime is used to check expiry of the entry
	accessTime is used to for cache replacement logic
	The versions are stored, because a cached session must be reused 
	with same SSL version.
*/
	psGetTime(&sessionTable[i].startTime);
	sessionTable[i].accessTime = sessionTable[i].startTime;
	sessionTable[i].majVer = ssl->majVer;
	sessionTable[i].minVer = ssl->minVer;

	return i;
}

/******************************************************************************/
/*
	Decrement inUse to keep the reference count meaningful
*/
int32 matrixClearSession(ssl_t *ssl, int32 remove)
{
	char	*id;
	uint32	i;

	if (ssl->sessionIdLen <= 0) {
		return PS_ARG_FAIL;
	}
	id = ssl->sessionId;
	
	i = (id[3] << 24) + (id[2] << 16) + (id[1] << 8) + id[0];
	if (i >= SSL_SESSION_TABLE_SIZE) {
		return PS_LIMIT_FAIL;
	}
#ifdef USE_MULTITHREADING	
	psLockMutex(&sessionTableLock);
#endif /* USE_MULTITHREADING */
	sessionTable[i].inUse -= 1;
/*
	If this is a full removal, actually delete the entry.  Also need to
	clear any RESUME flag on the ssl connection so a new session
	will be correctly registered.
*/
	if (remove) {
		memset(ssl->sessionId, 0x0, SSL_MAX_SESSION_ID_SIZE);
		ssl->sessionIdLen = 0;
		memset(&sessionTable[i], 0x0, sizeof(sslSessionEntry_t));
		ssl->flags &= ~SSL_FLAGS_RESUMED;
	}
#ifdef USE_MULTITHREADING	
	psUnlockMutex(&sessionTableLock);
#endif /* USE_MULTITHREADING */	
	return PS_SUCCESS;
}

/******************************************************************************/
/*
	Look up a session ID in the cache.  If found, set the ssl masterSecret
	and cipher to the pre-negotiated values
*/
int32 matrixResumeSession(ssl_t *ssl)
{
	char	*id;
	uint32	i;

	if (!(ssl->flags & SSL_FLAGS_SERVER)) {
		return PS_ARG_FAIL;
	}
	if (ssl->sessionIdLen <= 0) {
		return PS_ARG_FAIL;
	}
	id = ssl->sessionId;

	i = (id[3] << 24) + (id[2] << 16) + (id[1] << 8) + id[0];
#ifdef USE_MULTITHREADING		
	psLockMutex(&sessionTableLock);
#endif /* USE_MULTITHREADING */
	if (i >= SSL_SESSION_TABLE_SIZE || sessionTable[i].cipher == NULL) {
#ifdef USE_MULTITHREADING			
		psUnlockMutex(&sessionTableLock);
#endif /* USE_MULTITHREADING */		
		return PS_LIMIT_FAIL;
	}
/*
	Id looks valid.  Update the access time for expiration check.
	Expiration is done on daily basis (86400 seconds)
*/
	psGetTime(&sessionTable[i].accessTime);
	if ((memcmp(sessionTable[i].id, id, 
			(uint32)min(ssl->sessionIdLen, SSL_MAX_SESSION_ID_SIZE)) != 0) ||
			(psDiffMsecs(sessionTable[i].startTime,
				sessionTable[i].accessTime) > SSL_SESSION_ENTRY_LIFE) ||
			(sessionTable[i].majVer != ssl->majVer) ||
			(sessionTable[i].minVer != ssl->minVer)) {
#ifdef USE_MULTITHREADING			
		psUnlockMutex(&sessionTableLock);
#endif /* USE_MULTITHREADING */
		return PS_FAILURE;
	}

	memcpy(ssl->sec.masterSecret, sessionTable[i].masterSecret,
		SSL_HS_MASTER_SIZE);
	ssl->cipher = sessionTable[i].cipher;
	sessionTable[i].inUse += 1;
#ifdef USE_MULTITHREADING		
	psUnlockMutex(&sessionTableLock);
#endif /* USE_MULTITHREADING */	
	return PS_SUCCESS;


}

/******************************************************************************/
/*
	Update session information in the cache.
	This is called when we've determined the master secret and when we're
	closing the connection to update various values in the cache.
*/
int32 matrixUpdateSession(ssl_t *ssl)
{
	char	*id;
	uint32	i;

	if (!(ssl->flags & SSL_FLAGS_SERVER)) {
		return PS_ARG_FAIL;
	}
	if ((id = ssl->sessionId) == NULL) {
		return PS_ARG_FAIL;
	}
	i = (id[3] << 24) + (id[2] << 16) + (id[1] << 8) + id[0];
	if (i >= SSL_SESSION_TABLE_SIZE) {
		return PS_LIMIT_FAIL;
	}
/*
	If there is an error on the session, invalidate for any future use
*/
#ifdef USE_MULTITHREADING
	psLockMutex(&sessionTableLock);
#endif /* USE_MULTITHREADING */		
	sessionTable[i].inUse += ssl->flags & SSL_FLAGS_CLOSED ? -1 : 0;
	if (ssl->flags & SSL_FLAGS_ERROR) {
		memset(sessionTable[i].masterSecret, 0x0, SSL_HS_MASTER_SIZE);
		sessionTable[i].cipher = NULL;
#ifdef USE_MULTITHREADING		
		psUnlockMutex(&sessionTableLock);
#endif /* USE_MULTITHREADING */			
		return PS_FAILURE;
	}
	memcpy(sessionTable[i].masterSecret, ssl->sec.masterSecret,
		SSL_HS_MASTER_SIZE);
	sessionTable[i].cipher = ssl->cipher;
#ifdef USE_MULTITHREADING	
	psUnlockMutex(&sessionTableLock);
#endif /* USE_MULTITHREADING */		
	return PS_SUCCESS;
}
#endif /* USE_SERVER_SIDE_SSL */

#ifdef USE_CLIENT_SIDE_SSL
/******************************************************************************/
/*
	Get session information from the ssl structure and populate the given
	session structure.  Session will contain a copy of the relevant session
	information, suitable for creating a new, resumed session.

	NOTE: Must define USE_CLIENT_SIDE_SSL in matrixConfig.h 
	
	sslSessionId_t myClientSession;
	
	...&myClientSession
*/
int32 matrixSslGetSessionId(ssl_t *ssl, sslSessionId_t *session)
{

	if (ssl == NULL || ssl->flags & SSL_FLAGS_SERVER || session == NULL) {
		return PS_ARG_FAIL;
	}

	if (ssl->cipher != NULL && ssl->cipher->ident != SSL_NULL_WITH_NULL_NULL && 
			ssl->sessionIdLen == SSL_MAX_SESSION_ID_SIZE) {
		session->cipherId = ssl->cipher->ident;
		memcpy(session->id, ssl->sessionId, ssl->sessionIdLen);
		memcpy(session->masterSecret, ssl->sec.masterSecret, 
			SSL_HS_MASTER_SIZE);
		return PS_SUCCESS;
	}

	return PS_FAILURE;
}
#endif /* USE_CLIENT_SIDE_SSL */

/******************************************************************************/
/*
	Rehandshake. Free any allocated sec members that will be repopulated
*/
void sslResetContext(ssl_t *ssl)
{
	ssl->sec.anon = 0;
#ifdef USE_SERVER_SIDE_SSL
	if (ssl->flags & SSL_FLAGS_SERVER) {
		matrixClearSession(ssl, 0);
	}
#endif /* USE_SERVER_SIDE_SSL */



	ssl->bFlags = 0;  /* Reset buffer control */
}

#ifdef USE_CLIENT_SIDE_SSL
/******************************************************************************/
/*
*/
int32 matrixValidateCerts(psPool_t *pool, psX509Cert_t *subjectCerts,
						  psX509Cert_t *issuerCerts)
{
	psX509Cert_t    *ic, *sc, *icNext, *scNext;
	int32           rc;
	
/*
	Case #1 is no issuing cert.  Going to want to check that the final
	subject cert presented is a SelfSigned CA
*/
	if (issuerCerts == NULL) {
		return psX509AuthenticateCert(pool, subjectCerts, NULL);
	}
/*
	Case #2 is an issuing cert AND a chain of subjectCerts.
 */
	sc = subjectCerts;
	if ((ic = sc->next) != NULL) {
/*
		 We do have a chain. Authenticate the chain before even looking
		 to our issuer CAs.  We do a bit of trickery here with the chain
		 to comply with psX509AuthenticateCert restriction that chain-only
		 testing will do a self-signed test on the parent-most.
*/
		while (ic->next != NULL) {
			scNext = sc->next;
			sc->next = NULL;
			icNext = ic->next;
			ic->next = NULL;
			if ((rc = psX509AuthenticateCert(pool, sc, ic)) < PS_SUCCESS) {
				sc->next = scNext;
				ic->next = icNext;
				return rc; 
			}
			sc->next = scNext;
			ic->next = icNext;
			
			sc = sc->next;
			ic = sc->next;
		}
/*
		Test using the parent-most in chain as the subject 
*/
		scNext = sc->next;
		sc->next = NULL;
		if ((rc = psX509AuthenticateCert(pool, sc, ic)) < PS_SUCCESS) {
			sc->next = scNext;
			return rc; 
		}
		sc->next = scNext;
/*
		Lastly, set the subject to the final cert for the real issuer test
*/
		sc = sc->next;
	}
	
/*
	 Now loop through the issuer certs and see if we can authenticate this chain
	 
	 If subject cert was a chain, that has already been authenticated above so
	 we only need to pass in the single parent-most cert to be tested against
*/
	ic = issuerCerts;
	while (ic != NULL) {
		icNext = ic->next;
		ic->next = NULL;
		if ((rc = psX509AuthenticateCert(pool, sc, ic)) == PS_SUCCESS) {
			ic->next = icNext;
			return rc;
		} else if (rc == PS_MEM_FAIL) {
/*
			OK to fail on the authentication because there may be a list here
			but MEM failures prevent us from continuing at all.
*/
			return rc;
		}
		ic->next = icNext;
		ic = ic->next;
	}
/*
	Success would have returned if it happen
*/
	return PS_CERT_AUTH_FAIL;
}
#endif /* USE_CLIENT_SIDE_SSL */

/******************************************************************************/
/*
	Calls a user defined callback to allow for manual validation of the
	certificate.
*/
int32 matrixUserCertValidator(ssl_t *ssl, int32 alert, 
							  psX509Cert_t *subjectCert,
							  sslCertCb_t certValidator)
{
	int32			status;

/*
	If there is no callback, return PS_SUCCESS because there has already been
	a test for the case where the certificate did NOT PASS pubkey test
	and a callback does not exist to manually handle.  
	
	It is highly recommended that the user manually verify, but the cert
	material has internally authenticated and the user has implied that
	is sufficient enough.
*/
	if (certValidator == NULL) {
		psTraceInfo("Internal cert auth passed. No user callback registered\n");
		return PS_SUCCESS;
	}
	
/*
	Finally, let the user know what the alert status is and 
	give them the cert material to access.  Any non-zero value in alert
	indicates there is a pending fatal alert.	
	 
	The user can look at authStatus members if they want to examine the cert
	that did not pass.
*/
	if (alert == SSL_ALERT_NONE) {
		status = 0;
	} else {
		status = alert;
	}

/*
	The user callback
*/
	return certValidator(ssl, subjectCert, status);
}

/******************************************************************************/

