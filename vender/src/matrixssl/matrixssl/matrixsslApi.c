/*
 *	matrixsslApi.c
 *  Release $Name: MATRIXSSL-3-3-1-OPEN $
 * 
 *	MatrixSSL Public API Layer
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

/******************************************************************************/
/*
	Create a new client SSL session
	This creates internal SSL buffers and cipher structures
	Clients initiate the connection with a 'HelloRequest', and this data
		is placed in the outgoing buffer for the caller to send.
 
	keys	Keys structure initialized with matrixSslReadKeys
 
	sid		A pointer to storage for a session ID. If there is not yet session
			cache information, the sid.cipherId should be set to 
			SSL_NULL_WITH_NULL_NULL. After a successful connection to a server,
			this sid structure will be populated with session cache credentials
			and for subsequent connections should be used without modification 
			of the cipherId.

	cipherSpec	Requested cipher to negotiate to (0 for server's choice)
				If non-zero, and server doesn't have it, conn will fail

	certCb	Optional callback to call when validating cert
 
	extensions	Optional TLS extensions (usually NULL)
 
	ssl		The ssl_t session structure is returned using this value, on success

	extCb	TLS reply extensions from the server
 
	Return	MATRIXSSL_REQUEST_SEND on success
			< 0 on error
			
	Do not need to call DeleteSession on failure
 */
#ifdef USE_CLIENT_SIDE_SSL
int32 matrixSslNewClientSession(ssl_t **ssl, sslKeys_t *keys,
		sslSessionId_t *sid, uint32 cipherSpec,
		int32 (*certCb)(ssl_t *ssl, psX509Cert_t *cert, int32 alert),
		tlsExtension_t *extensions, int32 (*extCb)(ssl_t *ssl,
		unsigned short extType, unsigned short extLen, void *e))
{
	ssl_t		*lssl;
	psBuf_t		tmp;
	uint32		len;
	int32		rc;

	if (!ssl) {
		return PS_ARG_FAIL;
	}
	
	*ssl = NULL;
	lssl = NULL;

/*
	Give priority to cipher suite if session id is provided and doesn't match
*/
	if (cipherSpec != 0 && sid != NULL && sid->cipherId != 0) {
		if (cipherSpec != sid->cipherId) {
			psTraceInfo("Explicit cipher suite will override session cache\n");
			memset(sid->id, 0, SSL_MAX_SESSION_ID_SIZE);
			memset(sid->masterSecret, 0, SSL_HS_MASTER_SIZE);
			sid->cipherId = 0;
		}
	}

	if ((rc = matrixSslNewSession(&lssl, keys, sid, 0)) < 0) {
		return rc;
	}

	if (certCb) {
		matrixSslSetCertValidator(lssl, (sslCertCb_t)certCb);
	}
	
	if (extCb) {
		lssl->extCb = (sslExtCb_t)extCb;
	}


RETRY_HELLO:
	tmp.size = lssl->outsize;
	tmp.buf = tmp.start = tmp.end = lssl->outbuf;
	if ((rc = matrixSslEncodeClientHello(lssl, &tmp, cipherSpec, &len,
			extensions)) < 0) {
		if (rc == SSL_FULL) {
			if ((tmp.buf = psRealloc(lssl->outbuf, len)) == NULL) {
				matrixSslDeleteSession(lssl);
				return PS_MEM_FAIL;
			}
			lssl->outbuf = tmp.buf;
			lssl->outsize = len;
			goto RETRY_HELLO;
		} else {
			matrixSslDeleteSession(lssl);
			return rc;
		}
	}
	psAssert(tmp.start == tmp.buf);
	lssl->outlen = tmp.end - tmp.start;
	*ssl = lssl;
	return MATRIXSSL_REQUEST_SEND;
}
#endif /* USE_CLIENT_SIDE_SSL */

#ifdef USE_SERVER_SIDE_SSL
/******************************************************************************/
/*
	Create a new server SSL session
	This creates internal SSL buffers and cipher structures
	Internal SSL state is set to expect an incoming 'HelloRequest'

	Return	MATRIXSSL_SUCCESS on success
			< 0 on error
 */
int32 matrixSslNewServerSession(ssl_t **ssl, sslKeys_t *keys,
			int32 (*certCb)(ssl_t *ssl, psX509Cert_t *cert, int32 alert))
{
	ssl_t		*lssl;
	int32		flags = SSL_FLAGS_SERVER;
	
	if (!ssl) {
		return PS_ARG_FAIL;
	}
	
	*ssl = NULL;
	lssl = NULL;
	

	psAssert(certCb == NULL);
	if (matrixSslNewSession(&lssl, keys, NULL, flags) < 0) {
		goto NEW_SVR_ERROR;
	}

#ifdef SSL_REHANDSHAKES_ENABLED
	lssl->rehandshakeCount = DEFAULT_RH_CREDITS;
#endif /* SSL_REHANDSHAKES_ENABLED */
	
	lssl->maxPtFrag = SSL_MAX_PLAINTEXT_LEN;
	*ssl = lssl;
	return MATRIXSSL_SUCCESS;
	
NEW_SVR_ERROR:
	if (lssl) matrixSslDeleteSession(lssl);
	return PS_FAILURE;
}
#endif /* USE_SERVER_SIDE_SSL */

/******************************************************************************/
/*
	Caller is asking for allocated buffer storage to recv data into
 
	buf		Populated with a transient area where data can be read into
 
	Return	> 0, size of 'buf' in bytes
			<= 0 on error
 */
int32 matrixSslGetReadbuf(ssl_t *ssl, unsigned char **buf)
{
	if (!ssl || !buf) {
		return PS_ARG_FAIL;
	}
	psAssert(ssl && ssl->insize > 0 && ssl->inbuf != NULL);
	/* If there's unprocessed data in inbuf, have caller append to it */
	*buf = ssl->inbuf + ssl->inlen;
	return ssl->insize - ssl->inlen;
}

/******************************************************************************/
/*
 	Caller is asking if there is any encoded, outgoing SSL data that should be 
		sent out the transport layer.
 
	buf		if provided, is updated to point to the data to be sent

	Return	> 0, the number of bytes to send
			0 if there is no pending data
			< 0 on error
 */
int32 matrixSslGetOutdata(ssl_t *ssl, unsigned char **buf)
{
	if (!ssl) {
		return PS_ARG_FAIL;
	}

	psAssert(ssl->outsize > 0 && ssl->outbuf != NULL);
	if (buf) {
		*buf = ssl->outbuf;
	}
	return ssl->outlen;	/* Can be 0 */
}

/******************************************************************************/
/*
	Caller is asking for an allocated buffer to write plaintext into.
		That plaintext will then be encoded when the caller subsequently calls
		matrixSslEncodeWritebuf()
		
	This is also explicitly called by matrixSslEncodeToOutdata	
	
	ssl		SSL session context
 
	buf		The data storage to write into will be populated here on success
 
	requestedLen	The amount of buffer space the caller would like to use
 
	Return	> 0, success returns # bytes available for plaintext at buf
			PS_MEM_FAIL if requiredLen too large for current memory
			<= 0 on error
 */
int32 matrixSslGetWritebuf(ssl_t *ssl, unsigned char **buf, uint32 requestedLen)
{
	uint32			requiredLen, sz, overhead;
	unsigned char	*p;
	
#ifdef USE_BEAST_WORKAROUND
	/* This is a client-only feature */
	if (!(ssl->flags & SSL_FLAGS_SERVER)) {
		/* Not a problem at all beginning in TLS 1.1 (version 3.2) and never
			a problem on stream ciphers */
		if ((ssl->majVer == SSL3_MAJ_VER) && (ssl->minVer <= TLS_MIN_VER)
				&& (ssl->enBlockSize > 1) && (requestedLen > 1) &&
				!(ssl->bFlags & BFLAG_STOP_BEAST)) {
			ssl->bFlags |= BFLAG_STOP_BEAST;
		}
	}
#endif
	
	if (!ssl || !buf) {
		return PS_ARG_FAIL;
	}
	psAssert(ssl->outsize > 0 && ssl->outbuf != NULL);
	
/*
	First thing is to ensure under the maximum allowed plaintext len according
	to the SSL specification (or the negotiated max).  If not, set it to the
	max for the calculations and make sure that exact max is returned to the
	caller.  The responsibilty for fragmenting the message is left to them
*/
	if (requestedLen > (uint32)ssl->maxPtFrag) {
		requestedLen = ssl->maxPtFrag;
	}
	
/*
	What is the total encoded size for a plaintext requestedLen.  The overhead
	includes leading header as well as trailing MAC and pad
*/
	requiredLen = matrixSslGetEncodedSize(ssl, requestedLen);
	psAssert(requiredLen >= requestedLen);
	overhead = requiredLen - requestedLen;


/*
	Get current available space in outbuf
*/
	if (ssl->outsize < ssl->outlen) {
		return PS_FAILURE;
	}
	sz = ssl->outsize - ssl->outlen;

/*
	If not enough free space for requiredLen, grow the buffer
*/
	if (sz < requiredLen) {
		if ((p = psRealloc(ssl->outbuf, ssl->outsize + 
				(requiredLen - sz))) == NULL) {
			return PS_MEM_FAIL;
		}
		ssl->outbuf = p;
		ssl->outsize = ssl->outsize + (requiredLen - sz);
/*
		Recalculate available free space
*/
		if (ssl->outsize < ssl->outlen) {
			return PS_FAILURE;
		}
		sz = ssl->outsize - ssl->outlen;
	}
/*
	Now that requiredLen has been confirmed/created, return number of available
	plaintext bytes 
*/
	if (requestedLen < (uint32)ssl->maxPtFrag) {
		requestedLen = sz - overhead;
		if (requestedLen > (uint32)ssl->maxPtFrag) {
			requestedLen = ssl->maxPtFrag;
		}
	}

/*
	Now return the pointer that has skipped past the record header
*/
#ifdef USE_TLS_1_1
/*
	If a block cipher is being used TLS 1.1 requires the use
	of an explicit IV.  This is an extra random block of data
	prepended to the plaintext before encryption.  Account for
	that extra length here.
*/
	if ((ssl->flags & SSL_FLAGS_WRITE_SECURE) &&
			(ssl->flags & SSL_FLAGS_TLS_1_1) &&	(ssl->enBlockSize > 1)) {
		*buf = ssl->outbuf + ssl->outlen + ssl->recordHeadLen + ssl->enBlockSize;
		return requestedLen; /* may not be what was passed in */
	}
#endif /* USE_TLS_1_1 */

#ifdef USE_BEAST_WORKAROUND
	if (ssl->bFlags & BFLAG_STOP_BEAST) {
		/* The reserved space accounts for a full encoding of a 1 byte record.
			The final -1 is so that when the second encrypt arrives it will
			land as an in-situ */
		overhead = ((ssl->enMacSize + 1) % ssl->enBlockSize) ? 
			ssl->enBlockSize : 0;
		*buf = ssl->outbuf + ssl->outlen + (2 * ssl->recordHeadLen) + overhead +
			(ssl->enBlockSize * ((ssl->enMacSize + 1)/ssl->enBlockSize)) - 1;
	} else {
		*buf = ssl->outbuf + ssl->outlen + ssl->recordHeadLen;
	}
#else					
	*buf = ssl->outbuf + ssl->outlen + ssl->recordHeadLen;
#endif	
	return requestedLen; /* may not be what was passed in */
}

/******************************************************************************/
/*
	Encode (encrypt) 'len' bytes of plaintext data that has been placed into
	the buffer given by matrixSslGetWritebuf().  This is an in-situ encode.
	
	CAN ONLY BE CALLED AFTER A PREVIOUS CALL TO matrixSslGetWritebuf

	len		>= 0.If len is zero, we send out a blank ssl record
			len must be <= size returned by matrixSslGetWritebuf()
 
	Returns < 0 on error, total #bytes in outgoing data buf on success
 */
int32 matrixSslEncodeWritebuf(ssl_t *ssl, uint32 len)
{
	unsigned char	*origbuf;
	int32			rc, reserved;
	
	if (!ssl || ((int32)len < 0)) {
		return PS_ARG_FAIL;
	}
	if (ssl->bFlags & BFLAG_CLOSE_AFTER_SENT) {
		return PS_PROTOCOL_FAIL;
	}
	psAssert(ssl->outsize > 0 && ssl->outbuf != NULL);
	/* Caller was given proper locations and lengths in GetWritebuf() */
	origbuf = ssl->outbuf + ssl->outlen;
	if (ssl->outbuf == NULL || (ssl->outsize - ssl->outlen) < (int32)len) {
		return PS_FAILURE;
	}
	
	reserved = ssl->recordHeadLen;
#ifdef USE_BEAST_WORKAROUND
	if (ssl->bFlags & BFLAG_STOP_BEAST) {
		rc = ((ssl->enMacSize + 1) % ssl->enBlockSize) ? ssl->enBlockSize : 0;
		reserved += ssl->recordHeadLen + rc +
			(ssl->enBlockSize * ((ssl->enMacSize + 1)/ssl->enBlockSize)) - 1;
	}
#endif
	
#ifdef USE_TLS_1_1
/*
	If a block cipher is being used TLS 1.1 requires the use
	of an explicit IV.  This is an extra random block of data
	prepended to the plaintext before encryption.  Account for
	that extra length here.
*/
	if ((ssl->flags & SSL_FLAGS_WRITE_SECURE) &&
			(ssl->flags & SSL_FLAGS_TLS_1_1) &&	(ssl->enBlockSize > 1)) {
		reserved += ssl->enBlockSize;
	}
#endif /* USE_TLS_1_1 */

	rc = matrixSslEncode(ssl, origbuf, (ssl->outsize - ssl->outlen),
		origbuf + reserved, &len);
	if (rc < 0) {
		psAssert(rc != SSL_FULL);	/* should not happen */
		return PS_FAILURE;
	}

	ssl->outlen += len;
	return ssl->outlen;
}

/******************************************************************************/
/*
	This public API allows the user to encrypt the plaintext buffer of their
	choice into the internal outbuf that is retrieved when matrixSslGetOutdata
	is called.  This is non-in-situ support and will leave the callers
	plaintext buffer intact

	ptBuf	The plaintext buffer to be converted into an SSL application data
			record.  
	len		The length, in bytes, of the ptBuf plaintext data
	
	Returns < 0 on error, total #bytes in outgoing data buf on success
*/
int32 matrixSslEncodeToOutdata(ssl_t *ssl, unsigned char *ptBuf, uint32 len)
{
	unsigned char	*internalBuf;
	int32			rc, fragLen, recLen, index;
	
	if (!ssl || !ptBuf) {
		return PS_ARG_FAIL;
	}
	if (ssl->bFlags & BFLAG_CLOSE_AFTER_SENT) {
		return PS_PROTOCOL_FAIL;
	}
	
	
	/* Fragmentation support */
	index = 0;
	while (len > 0) {
		/*	We just call matrixSslGetWritebuf to prepare the buffer */
		if ((rc = matrixSslGetWritebuf(ssl, &internalBuf, len)) < 0) {
			psTraceIntInfo("matrixSslEncodeToOutbuf allocation error: %d\n",
				rc);
			return rc;
		}
		recLen = fragLen = min((uint32)rc, len);
		psAssert(ssl->outsize > 0 && ssl->outbuf != NULL);
	
		if (ssl->outbuf == NULL ||
				(ssl->outsize - ssl->outlen) < (int32)fragLen) {
			return PS_FAILURE;
		}
		internalBuf = ssl->outbuf + ssl->outlen;
	
		rc = matrixSslEncode(ssl, internalBuf, (ssl->outsize - ssl->outlen),
			ptBuf + index, (uint32*)&fragLen);
		if (rc < 0) {
			psAssert(rc != SSL_FULL);	/* should not happen */
			return PS_FAILURE;
		}
		index += recLen;
		len -= recLen;
		ssl->outlen += fragLen;
	}
	return ssl->outlen;
}


/******************************************************************************/
/*
	Helper to shrink buffers down to default size
*/
#define SSL_INBUF	0
#define SSL_OUTBUF	1

static void revertToDefaultBufsize(ssl_t *ssl, uint16 inOrOut)
{
	int32			defaultSize;
	unsigned char	*p;

	if (inOrOut == SSL_INBUF) {		
		defaultSize = SSL_DEFAULT_IN_BUF_SIZE;
		if (ssl->insize > defaultSize && ssl->inlen < defaultSize) {
			/* It's not fatal if we can't realloc it smaller */
			if ((p = psRealloc(ssl->inbuf, defaultSize)) != NULL) {
				ssl->inbuf = p;
				ssl->insize	 = defaultSize;
			}
		}
	} else {
		defaultSize = SSL_DEFAULT_OUT_BUF_SIZE;
		if (ssl->outsize > defaultSize && ssl->outlen < defaultSize) {
			/* It's not fatal if we can't realloc it smaller */
			if ((p = psRealloc(ssl->outbuf, defaultSize)) != NULL) {
				ssl->outbuf = p;
				ssl->outsize = defaultSize;
			}
		}
	}
}

/******************************************************************************/
/*
	Caller has received data from the network and is notifying the SSL layer
 */
int32 matrixSslReceivedData(ssl_t *ssl, uint32 bytes, unsigned char **ptbuf, 
							uint32 *ptlen)
{
	unsigned char	*buf, *prevBuf;
	int32			rc, decodeRet, size, sanity, decodeErr;
	uint32			processed, start, len, reqLen;
	unsigned char	alertLevel, alertDesc;
	unsigned char	*p;

	if (!ssl || !ptbuf || !ptlen) {
		return PS_ARG_FAIL;
	}

	psAssert(ssl->outsize > 0 && ssl->outbuf != NULL);
	psAssert(ssl->insize > 0 && ssl->inbuf != NULL);
	*ptbuf = NULL;
	*ptlen = 0;
	ssl->inlen += bytes;
	if (ssl->inlen == 0) {
		return PS_SUCCESS; /* Nothing to do.  Basically a poll */
	}
	/* Parameterized sanity check to avoid infinite loops */
	if (matrixSslHandshakeIsComplete(ssl)) {
		/* Minimum possible record size once negotiated */
		sanity = ssl->inlen / (SSL3_HEADER_LEN + MD5_HASH_SIZE);
	} else {
		/* Even with an SSLv2 hello, the sanity check will let 1 pass through */
		sanity = ssl->inlen / (SSL3_HEADER_LEN + SSL3_HANDSHAKE_HEADER_LEN);
	}
	/* This is outside the loop b/c we may want to parse within inbuf later */
	buf = ssl->inbuf;
DECODE_MORE:
	if (sanity-- < 0) {
		return PS_PROTOCOL_FAIL;	/* We've tried to decode too many times */
	}
	len = ssl->inlen;
	size = ssl->insize - (buf - ssl->inbuf);
	prevBuf = buf;
	decodeRet = matrixSslDecode(ssl, &buf, &len, size, &start, &reqLen,
						 &decodeErr, &alertLevel, &alertDesc);

/* 
	Convenience for the cases that expect buf to have moved
		- calculate the number of encoded bytes that were decoded
*/
	processed = buf - prevBuf;
	rc = PS_PROTOCOL_FAIL;
	switch (decodeRet) {

	case MATRIXSSL_SUCCESS:
		
		ssl->inlen -= processed;
		if (ssl->inlen > 0) {
			psAssert(buf > ssl->inbuf);
/*
			Pack ssl->inbuf so there is immediate maximum room for potential
			outgoing data that needs to be written
*/
			memmove(ssl->inbuf, buf, ssl->inlen);
			buf = ssl->inbuf;
			goto DECODE_MORE;	/* More data in buffer to process */
		}
/*
		In this case, we've parsed a finished message and no additional data is
		available to parse. We let the client know the handshake is complete, 
		which can be used as a trigger to begin for example a HTTP request.
*/
		if (!(ssl->bFlags & BFLAG_HS_COMPLETE)) {
			if (matrixSslHandshakeIsComplete(ssl)) {
				ssl->bFlags |= BFLAG_HS_COMPLETE;
#ifdef USE_CLIENT_SIDE_SSL
				matrixSslGetSessionId(ssl, ssl->sid);
#endif /* USE_CLIENT_SIDE_SSL */
				rc = MATRIXSSL_HANDSHAKE_COMPLETE;
			} else {
				rc = MATRIXSSL_REQUEST_RECV; /* Need to recv more handshake data */
			}
		} else {
			/* This is an error - we shouldn't get here */
		}
		break;


	case SSL_SEND_RESPONSE:
#ifdef ENABLE_FALSE_START
		/*
			If FALSE START is supported, there may be APPLICATION_DATA directly
			following the FINISHED message, even though we haven't sent our 
			CHANGE_CIPHER_SPEC or FINISHED message. This is signalled by buf
			having been moved forward, and our response being put directly into
			ssl->outbuf, rather than in buf (ssl->inbuf). Return a REQUEST_SEND
			so that the data in outbuf is flushed before the remaining data in
			ssl->inbuf is parsed.
		 */
		if ((ssl->flags & SSL_FLAGS_FALSE_START) && buf != prevBuf) {
			ssl->inlen -= processed;
			psAssert(ssl->inlen > 0);
			psAssert((uint32)ssl->inlen == start);
			psAssert(buf > ssl->inbuf);
			memmove(ssl->inbuf, buf, ssl->inlen);	/* Pack ssl->inbuf */
			buf = ssl->inbuf;
			return MATRIXSSL_REQUEST_SEND;
		}
#endif
		/*
			This must be handshake data (or alert) or we'd be in PROCESS_DATA
			so there is no way there is anything left inside inbuf to process.
			...so processed isn't valid because the output params are outbuf
			related and we simply reset inlen
		*/
		ssl->inlen = 0;

		/* If alert, close connection after sending */
		if (alertDesc != SSL_ALERT_NONE) {
			ssl->bFlags |= BFLAG_CLOSE_AFTER_SENT;
		}
		psAssert(prevBuf == buf);
		psAssert(ssl->insize >= (int32)len);
		psAssert(start == 0);
		psAssert(buf == ssl->inbuf);
		if (ssl->outlen > 0) { 	/* if data's in outbuf, append inbuf */
			if (ssl->outlen + (int32)len > ssl->outsize) {
				if ((p = psRealloc(ssl->outbuf, ssl->outlen + len)) == NULL) {
					return PS_MEM_FAIL;
				}
				ssl->outbuf = p;
				ssl->outsize = ssl->outlen + len;
			}
			memcpy(ssl->outbuf + ssl->outlen, ssl->inbuf, len);
			ssl->outlen += len;
		} else { /* otherwise, swap inbuf and outbuf */
			buf = ssl->outbuf; ssl->outbuf = ssl->inbuf; ssl->inbuf = buf;
			ssl->outlen = len;
			len = ssl->outsize; ssl->outsize = ssl->insize; ssl->insize = len;
			buf = ssl->inbuf;
			len = ssl->outlen;
		}
		rc = MATRIXSSL_REQUEST_SEND;	/* We queued data to send out */
		break;

	case MATRIXSSL_ERROR:
		return decodeErr; /* Will be a negative value */

	case SSL_ALERT:
		if (alertLevel == SSL_ALERT_LEVEL_FATAL) {
			psTraceIntInfo("Received FATAL alert %d.\n", alertDesc);
		} else {
			/* Closure notify is the normal case */
			if (alertDesc == SSL_ALERT_CLOSE_NOTIFY) {
				psTraceInfo("Normal SSL closure alert\n");
			} else {
				psTraceIntInfo("Received WARNING alert %d\n", alertDesc);
			}
		}
		/* Let caller access the 2 data bytes (severity and description) */
		psAssert(len == 2);
		*ptbuf = prevBuf;
		*ptlen = len;
		ssl->inlen -= processed;
		return MATRIXSSL_RECEIVED_ALERT;
			
	case SSL_PARTIAL:
		if (reqLen > SSL_MAX_BUF_SIZE) {
			return PS_MEM_FAIL;
		}
		if (reqLen > (uint32)ssl->insize) {
			if ((p = psRealloc(ssl->inbuf, reqLen)) == NULL) {
				return PS_MEM_FAIL;
			}
			ssl->inbuf = p;
			ssl->insize = reqLen;
			buf = ssl->inbuf;
			/* Don't need to change inlen */
		}


		rc = MATRIXSSL_REQUEST_RECV;	/* Expecting more data */
		break;

	/* We've got outgoing data that's larger than our buffer */
	case SSL_FULL:
		if (reqLen > SSL_MAX_BUF_SIZE) {
			return PS_MEM_FAIL;
		}
		/* We balk if we get a large handshake message */
		if (reqLen > SSL_MAX_PLAINTEXT_LEN &&
				!matrixSslHandshakeIsComplete(ssl)) {
			if (reqLen > SSL_MAX_PLAINTEXT_LEN) {
				return PS_MEM_FAIL;
			}
		}
		/* 
			Can't envision any possible case where there is remaining data
			in inbuf to process and are getting SSL_FULL. 
		*/
		ssl->inlen = 0;
		
		/* Grow inbuf */
		if (reqLen > (uint32)ssl->insize) {
			len = ssl->inbuf - buf;
			if ((p = psRealloc(ssl->inbuf, reqLen)) == NULL) {
				return PS_MEM_FAIL;
			}
			ssl->inbuf = p;
			ssl->insize = reqLen;
			buf = ssl->inbuf + len;
			/* Note we leave inlen untouched here */
		} else {
			return PS_PROTOCOL_FAIL;	/* error in our encoding */
		}
		goto DECODE_MORE;

	case SSL_PROCESS_DATA:
/*
		Possible we received a finished message and app data in the same
		flight. In this case, the caller is not notified that the handshake
		is complete, but rather is notified that there is application data to
		process.
 */
		if (!(ssl->bFlags & BFLAG_HS_COMPLETE) && 
			matrixSslHandshakeIsComplete(ssl)) {
			ssl->bFlags |= BFLAG_HS_COMPLETE;
#ifdef USE_CLIENT_SIDE_SSL
			matrixSslGetSessionId(ssl, ssl->sid);
#endif /* USE_CLIENT_SIDE_SSL */
		}
/*
		 .	prevbuf points to start of unencrypted data
		 .	buf points to start of any remaining unencrypted data
		 .	start is length of remaining encrypted data yet to decode
		 .	len is length of unencrypted data ready for user processing
 */			
		ssl->inlen -= processed;
		psAssert((uint32)ssl->inlen == start);
	
		/* Call user plaintext data handler */
		psAssert(ssl->bFlags & BFLAG_HS_COMPLETE);
#ifdef USE_TLS_1_1
		/* Been ignoring the explicit IV up to this final return point. */
		if ((ssl->flags & SSL_FLAGS_WRITE_SECURE) &&
				(ssl->flags & SSL_FLAGS_TLS_1_1) &&	(ssl->enBlockSize > 1)) {
			len -= ssl->enBlockSize;
			prevBuf += ssl->enBlockSize;
		}
#endif /* USE_TLS_1_1 */
		*ptbuf = prevBuf;
		*ptlen = len;
		return MATRIXSSL_APP_DATA;
	} /* switch decodeRet */
	
	if (ssl->inlen > 0 && (buf != ssl->inbuf)) {
		psAssert(0);
	}
/*
	Shrink inbuf to default size once inlen < default size, and we aren't
	expecting any more data in the buffer. If SSL_PARTIAL, don't shrink the
	buffer, since we expect to fill it up shortly.
*/
	if (decodeRet != SSL_PARTIAL) {
		revertToDefaultBufsize(ssl, SSL_INBUF);
	}
	
	return rc;
}

/******************************************************************************/
/*
	Plaintext data has been processed as a response to MATRIXSSL_APP_DATA or
	MATRIXSSL_RECEIVED_ALERT return codes from matrixSslReceivedData()
	Return:
		< 0 on error
		0 if there is no more incoming ssl data in the buffer
			Caller should take whatever action is appropriate to the specific
			protocol implementation, eg. read for more data, close, etc.
		> 0 error code is same meaning as from matrixSslReceivedData()
			In this case, ptbuf and ptlen will be modified and caller should
			handle return code identically as from matrixSslReceivedData()
			This is the case when more than one SSL record is in the buffer
 */
int32 matrixSslProcessedData(ssl_t *ssl, unsigned char **ptbuf, uint32 *ptlen)
{
	uint32	ctlen;
	
	if (!ssl || !ptbuf || !ptlen) {
		return PS_ARG_FAIL;
	}
	*ptbuf = NULL;
	*ptlen = 0;
	
	psAssert(ssl->insize > 0 && ssl->inbuf != NULL);
	/* Move any remaining data to the beginning of the buffer */
	if (ssl->inlen > 0) {
		ctlen = ssl->rec.len + SSL3_HEADER_LEN;
		memmove(ssl->inbuf, ssl->inbuf + ctlen, ssl->inlen);
	}
	/* Shrink inbuf to default size once inlen < default size */
	revertToDefaultBufsize(ssl, SSL_INBUF);

	/* If there's more data, try to decode it here and return that code */
	if (ssl->inlen > 0) {
		/* NOTE: ReceivedData cannot return 0 */
		return matrixSslReceivedData(ssl, 0, ptbuf, ptlen);
	}
	return MATRIXSSL_SUCCESS;
}

/******************************************************************************/
/*
	Returns < 0 on error
 */
int32 matrixSslEncodeClosureAlert(ssl_t *ssl)
{
	sslBuf_t		sbuf;
	int32			rc;
	uint32			reqLen, newLen;
	unsigned char	*p;
	
	if (!ssl) {
		return PS_ARG_FAIL;
	}
	psAssert(ssl->outsize > 0 && ssl->outbuf != NULL);
/*
	Only encode the closure alert if we aren't already flagged for close
	If we are flagged, we do not want to send any more data
 */
	newLen = 0;
	if (!(ssl->bFlags & BFLAG_CLOSE_AFTER_SENT)) {
		ssl->bFlags |= BFLAG_CLOSE_AFTER_SENT;
L_CLOSUREALERT:
		sbuf.buf = sbuf.start = sbuf.end = ssl->outbuf + ssl->outlen;
		sbuf.size = ssl->outsize - ssl->outlen;
		rc = sslEncodeClosureAlert(ssl, &sbuf, &reqLen);
		if (rc == SSL_FULL && newLen == 0) {
			newLen = ssl->outlen + reqLen;
			if (newLen > SSL_MAX_BUF_SIZE) {
				return PS_MEM_FAIL;
			}
			if ((p = psRealloc(ssl->outbuf, newLen)) == NULL) {
				return PS_MEM_FAIL;
			}
			ssl->outbuf = p;
			ssl->outsize = newLen;
			goto L_CLOSUREALERT; /* Try one more time */
		} else if (rc != PS_SUCCESS) {
			return rc;
		}
		ssl->outlen += sbuf.end - sbuf.start;
	}
	return MATRIXSSL_SUCCESS;
}

#ifdef SSL_REHANDSHAKES_ENABLED
/******************************************************************************/
/*
	Encode a CLIENT_HELLO or HELLO_REQUEST to re-handshake an existing
	connection.
		
	Can't "downgrade" the re-handshake.  This means if keys or certCb are
	NULL we stick with whatever the session already has loaded.
	
	keys should be NULL if no change in key material is being made
	
	cipherSpec is only used by clients
 */
int32 matrixSslEncodeRehandshake(ssl_t *ssl, sslKeys_t *keys,
				int32 (*certCb)(ssl_t *ssl, psX509Cert_t *cert, int32 alert),
				uint32 sessionOption, uint32 cipherSpec)
{
	sslBuf_t		sbuf;
	int32			rc;		
	uint32			reqLen, newLen;
	unsigned char	*p;
	
	if (!ssl) {
		return PS_ARG_FAIL;
	}
	if (ssl->bFlags & BFLAG_CLOSE_AFTER_SENT) {
		return PS_PROTOCOL_FAIL;
	}	
	psAssert(ssl->outsize > 0 && ssl->outbuf != NULL);
/*
	The only explicit option that can be passsed in is
	SSL_OPTION_FULL_HANDSHAKE to indicate no resumption is allowed
*/
	if (sessionOption == SSL_OPTION_FULL_HANDSHAKE) {
		matrixSslSetSessionOption(ssl, sessionOption, NULL);
	}
/*
	If the key material or cert callback are provided we have to assume it
	was intentional to "upgrade" the re-handshake and we force full handshake
	No big overhead calling SetSessionOption with FULL_HS multiple times.
*/
	if (keys != NULL) {
		ssl->keys = keys;
		matrixSslSetSessionOption(ssl, SSL_OPTION_FULL_HANDSHAKE, NULL);
	}

	if (certCb != NULL) { 
		if (ssl->flags & SSL_FLAGS_SERVER) {
			psAssert(certCb == NULL);
#ifdef USE_CLIENT_SIDE_SSL
		} else {
			matrixSslSetCertValidator(ssl, (sslCertCb_t)certCb);
			matrixSslSetSessionOption(ssl, SSL_OPTION_FULL_HANDSHAKE, NULL);
#endif /* USE_CLIENT_SIDE_SSL */
		}
	}	
	
/*
	If cipher spec is different from current, assume a full handshake
*/
	if (!(ssl->flags & SSL_FLAGS_SERVER)) {
		if (cipherSpec != 0 && ssl->cipher->ident != cipherSpec) {
			matrixSslSetSessionOption(ssl, SSL_OPTION_FULL_HANDSHAKE, NULL);
		}
	}
/*
	Options are set.  Encode the HELLO message
*/
	newLen = 0;
L_REHANDSHAKE:
	if (ssl->flags & SSL_FLAGS_SERVER) {
		sbuf.buf = sbuf.start = sbuf.end = ssl->outbuf + ssl->outlen;
		sbuf.size = ssl->outsize - ssl->outlen;
		if ((rc = matrixSslEncodeHelloRequest(ssl, &sbuf, &reqLen)) < 0) {
			if (rc == SSL_FULL && newLen == 0) {
				newLen = ssl->outlen + reqLen;
				if (newLen < SSL_MAX_BUF_SIZE) {
					if ((p = psRealloc(ssl->outbuf, newLen)) == NULL){
						return PS_MEM_FAIL;
					}
					ssl->outbuf = p;
					ssl->outsize = newLen;
					goto L_REHANDSHAKE;
				}
			}
			return rc;
		}
	} else {
		sbuf.buf = sbuf.start = sbuf.end = ssl->outbuf + ssl->outlen;
		sbuf.size = ssl->outsize - ssl->outlen;
		if ((rc = matrixSslEncodeClientHello(ssl, &sbuf, cipherSpec, &reqLen,
				NULL)) < 0) {
			if (rc == SSL_FULL && newLen == 0) {
				newLen = ssl->outlen + reqLen;
				if (newLen < SSL_MAX_BUF_SIZE) {
					if ((p = psRealloc(ssl->outbuf, newLen)) == NULL) {
						return PS_MEM_FAIL;
					}
					ssl->outbuf = p;
					ssl->outsize = newLen;
					goto L_REHANDSHAKE;
				}
			}
			return rc;
		}
	}
	ssl->outlen += sbuf.end - sbuf.start;
	return MATRIXSSL_SUCCESS;
}

/* Undocumented helper function to add rehandshake credits for testing */
void matrixSslAddRehandshakeCredits(ssl_t *ssl, int32 credits)
{
	if ((ssl->rehandshakeCount + credits) < 0x8000) {
		ssl->rehandshakeCount += credits;
	}
}

#else /* !SSL_REHANDSHAKES_ENABLED */
int32 matrixSslEncodeRehandshake(ssl_t *ssl, sslKeys_t *keys,
				int32 (*certCb)(ssl_t *ssl, psX509Cert_t *cert, int32 alert),
				uint32 sessionOption, uint32 cipherSpec)
{
	psTraceInfo("Rehandshaking is disabled.  matrixSslEncodeRehandshake off\n");
	return PS_FAILURE;
}
#endif /* SSL_REHANDSHAKES_ENABLED */

/******************************************************************************/
/*
	Caller is indicating 'bytes' of data was written
 */
int32 matrixSslSentData(ssl_t *ssl, uint32 bytes)
{
	int32		rc;
	
	if (!ssl) {
		return PS_ARG_FAIL;
	}
	if (bytes == 0) {
		if (ssl->outlen > 0) {
			return MATRIXSSL_REQUEST_SEND;
		} else {
			return MATRIXSSL_SUCCESS;	/* Nothing to do */
		}
	}
	psAssert(ssl->outsize > 0 && ssl->outbuf != NULL);
	ssl->outlen -= bytes;

	rc = MATRIXSSL_SUCCESS;
	if (ssl->outlen > 0) {
		memmove(ssl->outbuf, ssl->outbuf + bytes, ssl->outlen);
		rc = MATRIXSSL_REQUEST_SEND;
	}
	/* 	If there's nothing left to flush, reallocate the buffer smaller. */
	if ((ssl->outlen == 0) && (ssl->bFlags & BFLAG_CLOSE_AFTER_SENT)) {
		/* We want to close the connection now */
		rc = MATRIXSSL_REQUEST_CLOSE;
	} else  {
		revertToDefaultBufsize(ssl, SSL_OUTBUF);
	}
	/* Indicate the handshake is complete, in this case, the finished message
		is being/has been just sent. Occurs in session resumption. */
	if (!(ssl->bFlags & BFLAG_HS_COMPLETE) && 
			matrixSslHandshakeIsComplete(ssl)) {
		ssl->bFlags |= BFLAG_HS_COMPLETE;
#ifdef USE_CLIENT_SIDE_SSL
		matrixSslGetSessionId(ssl, ssl->sid);
#endif /* USE_CLIENT_SIDE_SSL */
		rc = MATRIXSSL_HANDSHAKE_COMPLETE;
#ifdef USE_SSL_INFORMATIONAL_TRACE
		/* Client side resumed completion or server standard completion */
		matrixSslPrintHSDetails(ssl);
#endif		
	}
	return rc;
}


/******************************************************************************/

