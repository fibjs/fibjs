/*
 *	sslEncode.c
 *	Release $Name: MATRIXSSL-3-3-1-OPEN $
 *
 *	Secure Sockets Layer protocol message encoding portion of MatrixSSL
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

static int32 writeCertificate(ssl_t *ssl, sslBuf_t *out, int32 notEmpty);
static int32 writeChangeCipherSpec(ssl_t *ssl, sslBuf_t *out);
static int32 writeFinished(ssl_t *ssl, sslBuf_t *out);
static int32 writeAlert(ssl_t *ssl, unsigned char level, 
				unsigned char description, sslBuf_t *out, uint32 *requiredLen);
static int32 writeRecordHeader(ssl_t *ssl, int32 type, int32 hsType, 
				int32 *messageSize,	char *padLen, unsigned char **encryptStart,
							   unsigned char **end, unsigned char **c);

static int32 encryptRecord(ssl_t *ssl, int32 type, int32 messageSize,
				int32 padLen, unsigned char *pt, sslBuf_t *out,
				unsigned char **c);

#ifdef USE_CLIENT_SIDE_SSL
static int32 writeClientKeyExchange(ssl_t *ssl, sslBuf_t *out);
#endif /* USE_CLIENT_SIDE_SSL */


#ifdef USE_SERVER_SIDE_SSL
static int32 writeServerHello(ssl_t *ssl, sslBuf_t *out);
static int32 writeServerHelloDone(ssl_t *ssl, sslBuf_t *out);
#endif /* USE_SERVER_SIDE_SSL */

static int32 secureWriteAdditions(ssl_t *ssl, int32 numRecs);

/******************************************************************************/
/*
	This works for both in-situ and external buf
	
	buf		in	Start of allocated buffer (header bytes beyond are overwritten)
			out	Start of encrypted data on function success
					
	size	in	Total size of the allocated buffer
	
	ptBuf	in	Pointer to front of the plain text data to be encrypted
	
	len		in	Length of incoming plain text
			out	Length of encypted text on function success
			out	Length of required 'size' on SSL_FULL
*/
int32 matrixSslEncode(ssl_t *ssl, unsigned char *buf, uint32 size,
		unsigned char *ptBuf, uint32 *len)
{
	unsigned char	*c, *end, *encryptStart;
	char			padLen;
	int32			messageSize, rc;
	psBuf_t			tmpout;
	
	/* If we've had a protocol error, don't allow further use of the session
		Also, don't allow a application data record to be encoded unless the
		handshake is complete.
	*/
	if (ssl->flags & SSL_FLAGS_ERROR || ssl->hsState != SSL_HS_DONE ||
			ssl->flags & SSL_FLAGS_CLOSED) {
		psTraceInfo("Bad SSL state for matrixSslEncode call attempt\n");
		return MATRIXSSL_ERROR;
	}

	c = buf;
	end = buf + size;
	
#ifdef USE_BEAST_WORKAROUND
	if (ssl->bFlags & BFLAG_STOP_BEAST) {
		messageSize = ssl->recordHeadLen + 1; /* single byte is the fix */
		if ((rc = writeRecordHeader(ssl, SSL_RECORD_TYPE_APPLICATION_DATA, 0,
				&messageSize, &padLen, &encryptStart, &end, &c)) < 0) {
			if (rc == SSL_FULL) {
				*len = messageSize;
			}
			return rc;
		}
		psAssert(encryptStart == buf + ssl->recordHeadLen);
		c += 1;
		*len -= 1;
			
		tmpout.buf = tmpout.start = tmpout.end = buf;
		tmpout.size = size;
		if ((rc = encryptRecord(ssl, SSL_RECORD_TYPE_APPLICATION_DATA,
				messageSize, padLen, ptBuf, &tmpout, &c)) < 0) {
			return rc;
		}
		ptBuf += 1;
		tmpout.end = tmpout.end + (c - buf);
		
	}
#endif
/*
	writeRecordHeader will determine SSL_FULL cases.  The expected
	messageSize to writeRecored header is the plain text length plus the
	record header length
 */
	messageSize = ssl->recordHeadLen + *len;
	
	if (messageSize > SSL_MAX_BUF_SIZE) {
		psTraceIntInfo("Message too large for matrixSslEncode: %d\n",
			messageSize);
		return PS_MEM_FAIL;
	}
	if ((rc = writeRecordHeader(ssl, SSL_RECORD_TYPE_APPLICATION_DATA, 0,
			&messageSize, &padLen, &encryptStart, &end, &c)) < 0) {
		if (rc == SSL_FULL) {
			*len = messageSize;
		}
		return rc;
	}

	c += *len;
#ifdef USE_BEAST_WORKAROUND
	if (ssl->bFlags & BFLAG_STOP_BEAST) {
		/* The tmpout buf already contains the single byte record and has
			updated pointers for current location.  Disable at this time */
		ssl->bFlags &= ~BFLAG_STOP_BEAST;
	} else {
		tmpout.buf = tmpout.start = tmpout.end = buf;
		tmpout.size = size;
	}
#else
	tmpout.buf = tmpout.start = tmpout.end = buf;
	tmpout.size = size;
#endif	
			
	if ((rc = encryptRecord(ssl, SSL_RECORD_TYPE_APPLICATION_DATA,
			messageSize, padLen, ptBuf, &tmpout, &c)) < 0) {
		return rc;
	}
	*len = c - buf;

#ifdef SSL_REHANDSHAKES_ENABLED
	if (ssl->flags & SSL_FLAGS_SERVER) {
		ssl->rehandshakeBytes += *len;
		if (ssl->rehandshakeBytes >= BYTES_BEFORE_RH_CREDIT) {
			if (ssl->rehandshakeCount < 0x8000) {
				ssl->rehandshakeCount++;
			}
			ssl->rehandshakeBytes = 0;
		}
	}
#endif /* SSL_REHANDSHAKES_ENABLED */
	return *len;
}

/******************************************************************************/
/*
	A helper function for matrixSslGetWritebuf to determine the correct
	destination size before allocating an output buffer. 
 */
int32 matrixSslGetEncodedSize(ssl_t *ssl, uint32 len)
{	
	len += ssl->recordHeadLen;
	if (ssl->flags & SSL_FLAGS_WRITE_SECURE) {
		len += ssl->enMacSize;
#ifdef USE_TLS_1_1
/*
		If a block cipher is being used TLS 1.1 requires the use
		of an explicit IV.  This is an extra random block of data
		prepended to the plaintext before encryption.  Account for
		that extra length here.
*/
		if ((ssl->flags & SSL_FLAGS_WRITE_SECURE) &&
				(ssl->flags & SSL_FLAGS_TLS_1_1) &&	(ssl->enBlockSize > 1)) {
			len += ssl->enBlockSize;
		}
#endif /* USE_TLS_1_1 */		
		
#ifdef USE_BEAST_WORKAROUND
		if (ssl->bFlags & BFLAG_STOP_BEAST) {
			/* Original message less one */
			len += psPadLenPwr2(len - 1 - ssl->recordHeadLen, ssl->enBlockSize);
			/* The single byte record overhead */
			len += ssl->recordHeadLen + ssl->enMacSize;
			len += psPadLenPwr2(1 + ssl->enMacSize, ssl->enBlockSize);
		} else {
			len += psPadLenPwr2(len - ssl->recordHeadLen, ssl->enBlockSize);
		}
#else
		len += psPadLenPwr2(len - ssl->recordHeadLen, ssl->enBlockSize);
#endif		
	}
	return len;
}

/* Second parameter includes handshake header length */
static int32 addCertFragOverhead(ssl_t *ssl, int32 totalCertLen)
{
	int32 oh = 0;
	
	/* For each additional record, we'll need a record header and
		secureWriteAdditions.  Borrowing ssl->fragIndex and ssl->fragTotal */
	ssl->fragTotal = totalCertLen;
	ssl->fragIndex = 0;
	while (ssl->fragTotal > 0) {
		if (ssl->fragIndex == 0) {
			/* First one is accounted for below as normal */
			ssl->fragTotal -= ssl->maxPtFrag;
			ssl->fragIndex++;
		} else {
			/* Remember this stage is simply for SSL_FULL test
			  so just incr totalCertLen to add overhead */
			oh += secureWriteAdditions(ssl, 1);
			oh += ssl->recordHeadLen;
			if (ssl->fragTotal > (uint32)ssl->maxPtFrag) {
				ssl->fragTotal -= ssl->maxPtFrag;
			} else {
				ssl->fragTotal = 0;
			}
		}
	}
	return oh;
}

/******************************************************************************/
/*
	We indicate to the caller through return codes in sslDecode when we need
	to write internal data to the remote host.  The caller will call this 
	function to generate a message appropriate to our state.
*/
int32 sslEncodeResponse(ssl_t *ssl, psBuf_t *out, uint32 *requiredLen)
{
	int32			messageSize;
	int32			i, rc = MATRIXSSL_ERROR;
#ifdef USE_SERVER_SIDE_SSL
	int32			totalCertLen, extSize;
	psX509Cert_t	*cert;
#endif /* USE_SERVER_SIDE_SSL */
#ifdef USE_CLIENT_SIDE_SSL
	int32			ckeSize;
#endif /* USE_CLIENT_SIDE_SSL */

/*
	We may be trying to encode an alert response if there is an error marked
	on the connection.
*/
	if (ssl->err != SSL_ALERT_NONE) {
		rc = writeAlert(ssl, SSL_ALERT_LEVEL_FATAL, (unsigned char)ssl->err,
			out, requiredLen);
		if (rc == MATRIXSSL_ERROR) {
			ssl->flags |= SSL_FLAGS_ERROR;
		}
#ifdef USE_SERVER_SIDE_SSL
/*
		Writing a fatal alert on this session.  Let's remove this client from
		the session table as a precaution.  Additionally, if this alert is
		happening mid-handshake the master secret might not even be valid
*/
		if (ssl->flags & SSL_FLAGS_SERVER) {
			matrixClearSession(ssl, 1);
		}
#endif /* USE_SERVER_SIDE_SSL */
		return rc;
	}

	
/*
	We encode a set of response messages based on our current state
	We have to pre-verify the size of the outgoing buffer against
	all the messages to make the routine transactional.  If the first
	write succeeds and the second fails because of size, we cannot
	rollback the state of the cipher and MAC.
*/
	switch (ssl->hsState) {
/*
	If we're waiting for the ClientKeyExchange message, then we need to
	send the messages that would prompt that result on the client
*/
#ifdef USE_SERVER_SIDE_SSL
	case SSL_HS_CLIENT_KEY_EXCHANGE:

/*
			This is the entry point for a server encoding the first flight
			of a non-DH, non-client-auth handshake.
*/
			totalCertLen = 0;
				cert = ssl->keys->cert;
				for (i = 0; cert != NULL; i++) {
					psAssert(cert->unparsedBin != NULL);
					totalCertLen += cert->binLen;
					cert = cert->next;
				}
				/* Are we going to have to fragment the CERTIFICATE message? */
				if ((totalCertLen + 3 + (i * 3) + ssl->hshakeHeadLen) >
						ssl->maxPtFrag) {
					totalCertLen += addCertFragOverhead(ssl,
						totalCertLen + 3 + (i * 3) + ssl->hshakeHeadLen);	
				}
				messageSize =
					3 * ssl->recordHeadLen +
					3 * ssl->hshakeHeadLen +
					38 + SSL_MAX_SESSION_ID_SIZE +  /* server hello */
					3 + (i * 3) + totalCertLen; /* certificate */


			messageSize += secureWriteAdditions(ssl, 3);

		

/*
		Add extensions
*/
		extSize = 0; /* Two byte total length for all extensions */
		if (ssl->maxPtFrag < SSL_MAX_PLAINTEXT_LEN) {
			extSize = 2;
			messageSize += 5; /* 2 type, 2 length, 1 value */
		}
#ifdef ENABLE_SECURE_REHANDSHAKES		
/*
		The RenegotiationInfo extension lengths are well known
*/	
		if (ssl->secureRenegotiationFlag == PS_TRUE &&
				ssl->myVerifyDataLen == 0) {
			extSize = 2;
			messageSize += 5; /* ff 01 00 01 00 */
		} else if (ssl->secureRenegotiationFlag == PS_TRUE &&
				ssl->myVerifyDataLen > 0) {
			extSize = 2;
			messageSize += 5 + ssl->myVerifyDataLen +
				ssl->peerVerifyDataLen; /* 2 for total len, 5 for type+len */
		}
#endif /* ENABLE_SECURE_REHANDSHAKES */

/*
		Done with extensions.  If had some, add the two byte total length
*/
		messageSize += extSize;


		if ((out->buf + out->size) - out->end < messageSize) {
			*requiredLen = messageSize;
			return SSL_FULL;
		}
/*
		Message size complete.  Begin the flight write
*/
		rc = writeServerHello(ssl, out);

				if (rc == MATRIXSSL_SUCCESS) {
					rc = writeCertificate(ssl, out, 1);
				}


		if (rc == MATRIXSSL_SUCCESS) {
			rc = writeServerHelloDone(ssl, out);
		}
		break;

#endif /* USE_SERVER_SIDE_SSL */

/*
	If we're not waiting for any message from client, then we need to
	send our finished message
*/
	case SSL_HS_DONE:
		messageSize = 2 * ssl->recordHeadLen +
			ssl->hshakeHeadLen +
			1 + /* change cipher spec */
			MD5_HASH_SIZE + SHA1_HASH_SIZE; /* finished */
/*
		Account for possible overhead in CCS message with secureWriteAdditions
		then always account for the encryption overhead on FINISHED message.
		Correct to use ssl->cipher values for mac and block since those will
		be the ones used when encrypting FINISHED 
*/
		messageSize += secureWriteAdditions(ssl, 1);
		messageSize += ssl->cipher->macSize + (ssl->cipher->blockSize - 1);
#ifdef USE_TLS
/*
		Account for the smaller finished message size for TLS.
*/
		if (ssl->flags & SSL_FLAGS_TLS) {
			messageSize += TLS_HS_FINISHED_SIZE -
				(MD5_HASH_SIZE + SHA1_HASH_SIZE);
		}
#endif /* USE_TLS */			
#ifdef USE_TLS_1_1
/*
		Adds explict IV overhead to the FINISHED message
*/
		if (ssl->flags & SSL_FLAGS_TLS_1_1) {
			if (ssl->cipher->flags & CRYPTO_FLAGS_GCM) {
				/* The magic 1 back into messageSize is because the
					macSize + blockSize above ends up subtracting one on GCM */
				messageSize += TLS_GCM_TAG_LEN + TLS_GCM_NONCE_LEN + 1;
			} else {
				messageSize += ssl->cipher->blockSize;
			}
		}
#endif /* USE_TLS_1_1 */
		if ((out->buf + out->size) - out->end < messageSize) {
			*requiredLen = messageSize;
			return SSL_FULL;
		}
		rc = writeChangeCipherSpec(ssl, out);
		if (rc == MATRIXSSL_SUCCESS) {
			rc = writeFinished(ssl, out);
		} 
		break;
/*
	If we're expecting a Finished message, as a server we're doing 
	session resumption.  As a client, we're completing a normal
	handshake
*/
	case SSL_HS_FINISHED:
#ifdef USE_SERVER_SIDE_SSL
		if (ssl->flags & SSL_FLAGS_SERVER) {
			messageSize =
				3 * ssl->recordHeadLen +
				2 * ssl->hshakeHeadLen +
				38 + SSL_MAX_SESSION_ID_SIZE + /* server hello */
				1 + /* change cipher spec */
				MD5_HASH_SIZE + SHA1_HASH_SIZE; /* finished */
/*
			Account for possible overhead with secureWriteAdditions
			then always account for the encrypted FINISHED message.  Correct
			to use the ssl->cipher values for mac and block since those will
			always be the values used to encrypt the FINISHED message
*/				
			messageSize += secureWriteAdditions(ssl, 2);
			messageSize += ssl->cipher->macSize + (ssl->cipher->blockSize - 1);
#ifdef ENABLE_SECURE_REHANDSHAKES		
/*
			The RenegotiationInfo extension lengths are well known
*/	
			if (ssl->secureRenegotiationFlag == PS_TRUE &&
					ssl->myVerifyDataLen == 0) {
				messageSize += 7; /* 00 05 ff 01 00 01 00 */
			} else if (ssl->secureRenegotiationFlag == PS_TRUE &&
					ssl->myVerifyDataLen > 0) {
				messageSize += 2 + 5 + ssl->myVerifyDataLen +
					ssl->peerVerifyDataLen; /* 2 for tot len, 5 for type+len */
			}
#endif /* ENABLE_SECURE_REHANDSHAKES */


#ifdef USE_TLS
/*
			Account for the smaller finished message size for TLS.
			The MD5+SHA1 is SSLv3.  TLS is 12 bytes.
*/
			if (ssl->flags & SSL_FLAGS_TLS) {
				messageSize += TLS_HS_FINISHED_SIZE -
					(MD5_HASH_SIZE + SHA1_HASH_SIZE);
			}
#endif /* USE_TLS */		
#ifdef USE_TLS_1_1 
/*
			Adds explict IV overhead to the FINISHED message.  Always added
			because FINISHED is never accounted for in secureWriteAdditions
*/
			if (ssl->flags & SSL_FLAGS_TLS_1_1) {
				if (ssl->cipher->flags & CRYPTO_FLAGS_GCM) {
					/* The magic 1 back into messageSize is because the
						blockSize -1 above ends up subtracting one on GCM */
					messageSize += TLS_GCM_TAG_LEN + TLS_GCM_NONCE_LEN + 1;
				} else {
					messageSize += ssl->cipher->blockSize; /* explicitIV */
				}
			}
#endif /* USE_TLS_1_1 */
			if ((out->buf + out->size) - out->end < messageSize) {
				*requiredLen = messageSize;
				return SSL_FULL;
			}
			rc = writeServerHello(ssl, out);
			if (rc == MATRIXSSL_SUCCESS) {
				rc = writeChangeCipherSpec(ssl, out);
			}
			if (rc == MATRIXSSL_SUCCESS) {
				rc = writeFinished(ssl, out);
			}
		}
#endif /* USE_SERVER_SIDE_SSL */
#ifdef USE_CLIENT_SIDE_SSL
/*
		Encode entry point for client side final flight encodes.
		First task here is to find out size of ClientKeyExchange message
*/
		if (!(ssl->flags & SSL_FLAGS_SERVER)) {
			ckeSize = 0;
/*
					Normal RSA auth cipher suite case
*/
					if (ssl->sec.cert == NULL) {
						ssl->flags |= SSL_FLAGS_ERROR;
						return MATRIXSSL_ERROR;
					}
					ckeSize = ssl->sec.cert->publicKey.keysize;
					

			messageSize = 0;
			
			if (ssl->flags & SSL_FLAGS_CLIENT_AUTH) {
/*
				No client auth support. Send a no_certificate warning for SSL3
				or an empty CERTIFICATE otherwise
*/
				if (ssl->majVer == SSL3_MAJ_VER && ssl->minVer == SSL3_MIN_VER){
					messageSize += 2 + ssl->recordHeadLen;
				} else {
					messageSize += 3 + ssl->recordHeadLen + ssl->hshakeHeadLen;
				}
			}
/*
			Account for the header and message size for all records.  The
			finished message will always be encrypted, so account for one
			largest possible MAC size and block size.  Minus one
			for padding.  The finished message is not accounted for in the
			writeSecureAddition calls below since it is accounted for here.
*/
			messageSize +=
				3 * ssl->recordHeadLen +
				2 * ssl->hshakeHeadLen + /* change cipher has no hsHead */
				ckeSize + /* client key exchange */
				1 + /* change cipher spec */
				MD5_HASH_SIZE + SHA1_HASH_SIZE + /* SSLv3 finished payload */
				ssl->cipher->macSize +
				(ssl->cipher->blockSize - 1); /* finished overhead */
#ifdef USE_TLS
/*
			Must add the 2 bytes key size length to the client key exchange
			message. Also, at this time we can account for the smaller finished
			message size for TLS.  The MD5+SHA1 is SSLv3.  TLS is 12 bytes.
*/
			if (ssl->flags & SSL_FLAGS_TLS) {
				messageSize += 2 - MD5_HASH_SIZE - SHA1_HASH_SIZE +
					TLS_HS_FINISHED_SIZE;
			}
#endif /* USE_TLS */
			if (ssl->flags & SSL_FLAGS_CLIENT_AUTH) {
				messageSize += secureWriteAdditions(ssl, 3);
			} else {
				messageSize += secureWriteAdditions(ssl, 2);
			}
			
#ifdef USE_TLS_1_1 
/*
			Adds explict IV overhead to the FINISHED message.  Always added
			because FINISHED is never accounted for in secureWriteAdditions
*/
			if (ssl->flags & SSL_FLAGS_TLS_1_1) {
				if (ssl->cipher->flags & CRYPTO_FLAGS_GCM) {
					/* The magic 1 back into messageSize is because the
					 blockSize -1 above ends up subtracting one on GCM */
					messageSize += TLS_GCM_TAG_LEN + TLS_GCM_NONCE_LEN + 1;
				} else {
					messageSize += ssl->cipher->blockSize; /* explicitIV */
				}
			}
#endif /* USE_TLS_1_1 */
/*
			The actual buffer size test to hold this flight
*/
			if ((out->buf + out->size) - out->end < messageSize) {
				*requiredLen = messageSize;
				return SSL_FULL;
			}
			rc = MATRIXSSL_SUCCESS;
			
			if (ssl->flags & SSL_FLAGS_CLIENT_AUTH) {
/*
				The TLS RFC is fairly clear that an empty certificate message
				be sent if there is no certificate match.  SSLv3 tends to lean
				toward a NO_CERTIFIATE warning alert message 	
*/
				if (ssl->sec.certMatch == 0 && ssl->majVer == SSL3_MAJ_VER
							&& ssl->minVer == SSL3_MIN_VER) {
					rc = writeAlert(ssl, SSL_ALERT_LEVEL_WARNING,
						SSL_ALERT_NO_CERTIFICATE, out, requiredLen);
				} else {
					rc = writeCertificate(ssl, out, ssl->sec.certMatch);
				}
			}

			if (rc == MATRIXSSL_SUCCESS) {
				rc = writeClientKeyExchange(ssl, out);
			}

			if (rc == MATRIXSSL_SUCCESS) {
				rc = writeChangeCipherSpec(ssl, out);
			}
			if (rc == MATRIXSSL_SUCCESS) {
				rc = writeFinished(ssl, out);
			}
		}
#endif /* USE_CLIENT_SIDE_SSL */
		break;
	}
	if (rc == MATRIXSSL_ERROR) {
		ssl->flags |= SSL_FLAGS_ERROR;
	}
	return rc;
}

/******************************************************************************/
/*
	Message size must account for any additional length a secure-write
	would add to the message.  It would be too late to check length in
	the writeRecordHeader call since some of the handshake hashing could
	have already taken place and we can't rewind those hashes.
*/
static int32 secureWriteAdditions(ssl_t *ssl, int32 numRecs)
{
	int32 add = 0;
/*
	There is a slim chance for a false FULL message due to the fact that
	the maximum padding is being calculated rather than the actual number.
	Caller must simply grow buffer and try again.  Not subtracting 1 for
	the padding overhead to support NULL ciphers that will have 0 enBlockSize
*/
	if (ssl->flags & SSL_FLAGS_WRITE_SECURE) {
		add += (numRecs * ssl->enMacSize) + /* handshake msg hash */
			(numRecs * (ssl->enBlockSize)); /* padding */
#ifdef USE_TLS_1_1
/*
		 Checks here for TLS1.1 with block cipher for explict IV additions.
 */
		if ((ssl->flags & SSL_FLAGS_TLS_1_1) &&	(ssl->enBlockSize > 1)) {
			add += (numRecs * ssl->enBlockSize); /* explicitIV */
		}
#endif /* USE_TLS_1_1 */
	}
	return add;
}

/******************************************************************************/
/*
	Write out a closure alert message (the only user initiated alert message)
	The user would call this when about to initate a socket close
	NOTICE: This is the internal function, there is a similarly named public
		API called matrixSslEncodeClosureAlert
*/
int32 sslEncodeClosureAlert(ssl_t *ssl, sslBuf_t *out, uint32 *reqLen)
{
/*
	If we've had a protocol error, don't allow further use of the session
*/
	if (ssl->flags & SSL_FLAGS_ERROR) {
		return MATRIXSSL_ERROR;
	}
	return writeAlert(ssl, SSL_ALERT_LEVEL_WARNING, SSL_ALERT_CLOSE_NOTIFY,
		out, reqLen);
}

/******************************************************************************/
/*
	Generic record header construction for alerts, handshake messages, and
	change cipher spec.  Determines message length for encryption and
	writes out to buffer up to the real message data.
	
	Incomming messageSize is the plaintext message length plus the header
	lengths.
*/
static int32 writeRecordHeader(ssl_t *ssl, int32 type, int32 hsType, 
				int32 *messageSize,	char *padLen, unsigned char **encryptStart,
				unsigned char **end, unsigned char **c)
{
	int32	messageData, msn;

	messageData = *messageSize - ssl->recordHeadLen;
	if (type == SSL_RECORD_TYPE_HANDSHAKE) {
		 messageData -= ssl->hshakeHeadLen;
	}
	if (type == SSL_RECORD_TYPE_HANDSHAKE_FIRST_FRAG) {
		 messageData -= ssl->hshakeHeadLen;
		 *messageSize = ssl->maxPtFrag + ssl->recordHeadLen;
		 type = SSL_RECORD_TYPE_HANDSHAKE;
	}


#ifdef USE_TLS_1_1
/*
	If a block cipher is being used TLS 1.1 requires the use
	of an explicit IV.  This is an extra random block of data
	prepended to the plaintext before encryption.  Account for
	that extra length here.
*/
	if ((ssl->flags & SSL_FLAGS_WRITE_SECURE) &&
			(ssl->flags & SSL_FLAGS_TLS_1_1) && (ssl->enBlockSize > 1)) {	
		*messageSize += ssl->enBlockSize;
	}
#endif /* USE_TLS_1_1 */

/*
	If this session is already in a secure-write state, determine padding
*/
	*padLen = 0;
	if (ssl->flags & SSL_FLAGS_WRITE_SECURE) {
		*messageSize += ssl->enMacSize;
		*padLen = psPadLenPwr2(*messageSize - ssl->recordHeadLen,
			ssl->enBlockSize);
		*messageSize += *padLen;
	}

	if (*end - *c < *messageSize) {
/*
		Callers other than sslEncodeResponse do not necessarily check for
		FULL before calling.  We do it here for them.
*/
		return SSL_FULL;
	}


	*c += psWriteRecordInfo(ssl, (unsigned char)type,
		*messageSize - ssl->recordHeadLen, *c);
		
/*
	All data written after this point is to be encrypted (if secure-write)
*/
	*encryptStart = *c;
	msn = 0;

#ifdef USE_TLS_1_1
/*
	Explicit IV notes taken from TLS 1.1 ietf draft.

	Generate a cryptographically strong random number R of
	length CipherSpec.block_length and prepend it to the plaintext
	prior to encryption. In this case either:

	The CBC residue from the previous record may be used
	as the mask. This preserves maximum code compatibility
	with TLS 1.0 and SSL 3. It also has the advantage that
	it does not require the ability to quickly reset the IV,
	which is known to be a problem on some systems.

	The data (R || data) is fed into the encryption process.
	The first cipher block containing E(mask XOR R) is placed
	in the IV field. The first block of content contains
	E(IV XOR data)
*/

	if ((ssl->flags & SSL_FLAGS_WRITE_SECURE) &&
			(ssl->flags & SSL_FLAGS_TLS_1_1) &&
			(ssl->enBlockSize > 1)) {
		*c += matrixSslGetPrngData(*c, ssl->enBlockSize);
	}
#endif /* USE_TLS_1_1 */

/*
	Handshake records have another header layer to write here
*/
	if (type == SSL_RECORD_TYPE_HANDSHAKE) {
		*c += psWriteHandshakeHeader(ssl, (unsigned char)hsType, messageData,
			msn, 0, messageData, *c);
	}
	return PS_SUCCESS;
}

/******************************************************************************/
/*
	Encrypt the message using the current cipher.  This call is used in
	conjunction with the writeRecordHeader function above to finish writing
	an SSL record.  Updates handshake hash if necessary, generates message
	MAC, writes the padding, and does the encryption.

	messageSize is the final size, with header, mac and padding of the output
	messageSize - 5 = ssl.recLen
	*c - encryptStart = plaintext length
*/
static int32 encryptRecord(ssl_t *ssl, int32 type, int32 messageSize,
						   int32 padLen, unsigned char *pt,
						   sslBuf_t *out, unsigned char **c)
{
	unsigned char	*encryptStart;
	void			*encryptCtx;
	int32			rc, ptLen, divLen, modLen;

	encryptStart = out->end + ssl->recordHeadLen;
	
	
	ptLen = *c - encryptStart;
#ifdef USE_TLS
#ifdef USE_TLS_1_1
	if ((ssl->flags & SSL_FLAGS_WRITE_SECURE) &&
			(ssl->flags & SSL_FLAGS_TLS_1_1) &&	(ssl->enBlockSize > 1)) {
/*
		Don't add the random bytes into the hash of the message.  Makes
		things very easy on the other side to simply discard the randoms
*/
		if (type == SSL_RECORD_TYPE_HANDSHAKE) {
			sslUpdateHSHash(ssl, pt + ssl->enBlockSize,
				ptLen - ssl->enBlockSize);
		}
		if (type == SSL_RECORD_TYPE_APPLICATION_DATA) {
			/* Application data is passed in with real pt from user but
				with the length of the explict IV added already */
			*c += ssl->generateMac(ssl, (unsigned char)type,
				pt, ptLen - ssl->enBlockSize, *c);
			/* While we are in here, let's see if this is an in-situ case */
			if (encryptStart + ssl->enBlockSize == pt) {
				pt = encryptStart;
			} else {
				/* Not in-situ.  Encrypt the explict IV now */
				if ((rc = ssl->encrypt(&ssl->sec.encryptCtx, encryptStart,
						encryptStart, ssl->enBlockSize)) < 0) {
					psTraceIntInfo("Error encrypting explicit IV: %d\n", rc);
					return MATRIXSSL_ERROR;
				}
				encryptStart += ssl->enBlockSize;
				ptLen -= ssl->enBlockSize;
			}
		} else {
			/* Handshake messages have been passed in with plaintext that
				begins with the explicit IV and size included */
			*c += ssl->generateMac(ssl, (unsigned char)type,
				pt + ssl->enBlockSize, ptLen - ssl->enBlockSize, *c);
		}
	} else {
#endif /* USE_TLS_1_1 */
		if (type == SSL_RECORD_TYPE_HANDSHAKE) {
			if ((rc = sslUpdateHSHash(ssl, pt, ptLen)) < 0) {
				return rc;
			}
		}
		if (ssl->generateMac) {
			*c += ssl->generateMac(ssl, (unsigned char)type, pt, ptLen, *c);
		}
#ifdef USE_TLS_1_1
	}
#endif /* USE_TLS_1_1 */
#else /* USE_TLS */
	if (type == SSL_RECORD_TYPE_HANDSHAKE) {
		sslUpdateHSHash(ssl, pt, ptLen);
	}
	*c += ssl->generateMac(ssl, (unsigned char)type, pt, 
		ptLen, *c);
#endif /* USE_TLS */
	
	*c += sslWritePad(*c, (unsigned char)padLen);

	encryptCtx = &ssl->sec.encryptCtx;
	
	if (pt == encryptStart) {
		/* In-situ encode */
		if ((rc = ssl->encrypt(encryptCtx, pt, encryptStart, 
				(uint32)(*c - encryptStart))) < 0 || 
				*c - out->end != messageSize) {
			psTraceIntInfo("Error encrypting: %d\n", rc);
			return MATRIXSSL_ERROR;
		}
	} else {
		/*
			Non-insitu requres two encrypts, one for plaintext and one for the
			any < blockSize remainder of the plaintext and the mac and pad
		*/
    	if (ssl->flags & SSL_FLAGS_WRITE_SECURE) {
			if (ssl->cipher->blockSize > 1) {
				divLen = ptLen & ~(ssl->cipher->blockSize - 1);
				modLen = ptLen & (ssl->cipher->blockSize - 1);
			} else {
					divLen = ptLen;
					modLen = 0;
			}
			if (divLen > 0) {
				rc = ssl->encrypt(encryptCtx, pt, encryptStart,	divLen);
				if (rc < 0) {
					psTraceIntInfo("Error encrypting: %d\n", rc);
					return MATRIXSSL_ERROR;
				}
			}
			if (modLen > 0) {
				memcpy(encryptStart + divLen, pt + divLen, modLen);
			}
			rc = ssl->encrypt(encryptCtx, encryptStart + divLen,
				encryptStart + divLen, modLen + ssl->cipher->macSize + padLen);
		} else {
			rc = ssl->encrypt(encryptCtx, pt, encryptStart, 
				(uint32)(*c - encryptStart));
		}
		if (rc < 0 || (*c - out->end != messageSize)) {
			psTraceIntInfo("Error encrypting: %d\n", rc);
			return MATRIXSSL_ERROR;
		}
	}

	return MATRIXSSL_SUCCESS;
}

#ifdef USE_SERVER_SIDE_SSL
/******************************************************************************/
/*
	Write out the ServerHello message
*/
static int32 writeServerHello(ssl_t *ssl, sslBuf_t *out)
{
	unsigned char	*c, *end, *encryptStart;
	char			padLen;
	int32			messageSize, rc, t;
	psTime_t		pst;
	int32			extLen = 0;	

	psTraceHs("<<< Server creating SERVER_HELLO message\n");
	c = out->end;
	end = out->buf + out->size;
/*
	Calculate the size of the message up front, and verify we have room
	We assume there will be a sessionId in the message, and make adjustments
	below if there is no sessionId.
*/
	messageSize =
		ssl->recordHeadLen +
		ssl->hshakeHeadLen +
		38 + SSL_MAX_SESSION_ID_SIZE;

#ifdef ENABLE_SECURE_REHANDSHAKES		
/*
	The RenegotiationInfo extension lengths are well known
*/	
	if (ssl->secureRenegotiationFlag == PS_TRUE && ssl->myVerifyDataLen == 0) {
		extLen = 7; /* 00 05 ff 01 00 01 00 */
	} else if (ssl->secureRenegotiationFlag == PS_TRUE &&
			ssl->myVerifyDataLen > 0) {
		extLen = 2 + 5 + ssl->myVerifyDataLen + ssl->peerVerifyDataLen;
	}
#endif /* ENABLE_SECURE_REHANDSHAKES */


	if (ssl->maxPtFrag < SSL_MAX_PLAINTEXT_LEN) {
		if (extLen == 0) {
			extLen = 2;
		}
		extLen += 5;
	}

	messageSize += extLen;
	
/*
	 First 4 bytes of the serverRandom are the unix time to prevent replay
	 attacks, the rest are random
*/	
	t = psGetTime(&pst);
	ssl->sec.serverRandom[0] = (unsigned char)((t & 0xFF000000) >> 24);
	ssl->sec.serverRandom[1] = (unsigned char)((t & 0xFF0000) >> 16);
	ssl->sec.serverRandom[2] = (unsigned char)((t & 0xFF00) >> 8);
	ssl->sec.serverRandom[3] = (unsigned char)(t & 0xFF);
	if (matrixSslGetPrngData(ssl->sec.serverRandom + 4,
			SSL_HS_RANDOM_SIZE - 4) < 0) {
		return MATRIXSSL_ERROR;
	}
/*
	We register session here because at this point the serverRandom value is
	populated.  If we are able to register the session, the sessionID and
	sessionIdLen fields will be non-NULL, otherwise the session couldn't
	be registered.
*/
	if (!(ssl->flags & SSL_FLAGS_RESUMED)) {
		matrixRegisterSession(ssl);
	}
	messageSize -= (SSL_MAX_SESSION_ID_SIZE - ssl->sessionIdLen);

	if ((rc = writeRecordHeader(ssl, SSL_RECORD_TYPE_HANDSHAKE,
			SSL_HS_SERVER_HELLO, &messageSize, &padLen, &encryptStart,
			&end, &c)) < 0) {
		return rc;
	}
/*
	First two fields in the ServerHello message are the major and minor
	SSL protocol versions we agree to talk with
*/
	*c = ssl->majVer; c++;
	*c = ssl->minVer; c++;
/*
	The next 32 bytes are the server's random value, to be combined with
	the client random and premaster for key generation later
*/
	memcpy(c, ssl->sec.serverRandom, SSL_HS_RANDOM_SIZE);
	c += SSL_HS_RANDOM_SIZE;
/*
	The next data is a single byte containing the session ID length,
	and up to 32 bytes containing the session id.
	First register the session, which will give us a session id and length
	if not all session slots in the table are used
*/
	*c = (unsigned char)ssl->sessionIdLen; c++;
	if (ssl->sessionIdLen > 0) {
        memcpy(c, ssl->sessionId, ssl->sessionIdLen);
		c += ssl->sessionIdLen;
	}
/*
	Two byte cipher suite we've chosen based on the list sent by the client
	and what we support.
	One byte compression method (always zero)
*/
	*c = (ssl->cipher->ident & 0xFF00) >> 8; c++;
	*c = ssl->cipher->ident & 0xFF; c++;
	*c = 0; c++;
	

	if (extLen != 0) {
		extLen -= 2; /* Don't add self to total extension len */
		*c = (extLen & 0xFF00) >> 8; c++;
		*c = extLen & 0xFF; c++;
		
		if (ssl->maxPtFrag < SSL_MAX_PLAINTEXT_LEN) {
			*c = 0x0; c++;
			*c = 0x1; c++;
			*c = 0x0; c++;
			*c = 0x1; c++;
			
			if (ssl->maxPtFrag == 0x200) {
				*c = 0x1; c++;
			}
			if (ssl->maxPtFrag == 0x400) {
				*c = 0x2; c++;
			}
			if (ssl->maxPtFrag == 0x800) {
				*c = 0x3; c++;
			}
			if (ssl->maxPtFrag == 0x1000) {
				*c = 0x4; c++;
			}
		}
		
#ifdef ENABLE_SECURE_REHANDSHAKES
		if (ssl->secureRenegotiationFlag == PS_TRUE) {
			/* RenegotiationInfo*/
			*c = (EXT_RENEGOTIATION_INFO & 0xFF00) >> 8; c++;
			*c = EXT_RENEGOTIATION_INFO & 0xFF; c++;
			if (ssl->myVerifyDataLen == 0) {
				*c = 0; c++;
				*c = 1; c++;
				*c = 0; c++;
			} else {
				*c =((ssl->myVerifyDataLen+ssl->peerVerifyDataLen+1)&0xFF00)>>8;
				c++;
				*c = (ssl->myVerifyDataLen + ssl->peerVerifyDataLen + 1) & 0xFF;
				c++;
				*c = (ssl->myVerifyDataLen + ssl->peerVerifyDataLen) & 0xFF;c++;
				memcpy(c, ssl->peerVerifyData, ssl->peerVerifyDataLen);
				c += ssl->peerVerifyDataLen;
				memcpy(c, ssl->myVerifyData, ssl->myVerifyDataLen);
				c += ssl->myVerifyDataLen;
			}
		}
#endif /* ENABLE_SECURE_REHANDSHAKES */

	}

	if ((rc = encryptRecord(ssl, SSL_RECORD_TYPE_HANDSHAKE, messageSize,
			padLen, encryptStart, out, &c)) < 0) {
		return rc;
	}
/*
	If we're resuming a session, we now have the clientRandom, master and 
	serverRandom, so we can derive keys which we'll be using shortly.
*/
	if (ssl->flags & SSL_FLAGS_RESUMED) {
		sslCreateKeys(ssl);
	}
/*
	Verify that we've calculated the messageSize correctly, really this
	should never fail; it's more of an implementation verification
*/
	if (c - out->end != messageSize) {
		return MATRIXSSL_ERROR;
	}
	out->end = c;
	return MATRIXSSL_SUCCESS;
}

/******************************************************************************/
/*
	ServerHelloDone message is a blank handshake message
*/
static int32 writeServerHelloDone(ssl_t *ssl, sslBuf_t *out)
{
	unsigned char	*c, *end, *encryptStart;
	char			padLen;
	int32				messageSize, rc;

	psTraceHs("<<< Server creating SERVER_HELLO_DONE message\n");
	c = out->end;
	end = out->buf + out->size;
	messageSize =
		ssl->recordHeadLen +
		ssl->hshakeHeadLen;

	if ((rc = writeRecordHeader(ssl, SSL_RECORD_TYPE_HANDSHAKE,
			SSL_HS_SERVER_HELLO_DONE, &messageSize, &padLen,
			&encryptStart, &end, &c)) < 0) {
		return rc;
	}

	if ((rc = encryptRecord(ssl, SSL_RECORD_TYPE_HANDSHAKE, messageSize,
			padLen, encryptStart, out, &c)) < 0) {
		return rc;
	}

	if (c - out->end != messageSize) {
		psTraceInfo("Error generating hello done for write\n");
		return MATRIXSSL_ERROR;
	}
	out->end = c;
	return MATRIXSSL_SUCCESS;
}


/******************************************************************************/
/*
	Server initiated rehandshake public API call.
*/
int32 matrixSslEncodeHelloRequest(ssl_t *ssl, sslBuf_t *out, 
								  uint32 *requiredLen)
{
	unsigned char	*c, *end, *encryptStart;
	char			padLen;
	int32				messageSize, rc;

	*requiredLen = 0;
	psTraceHs("<<< Server creating HELLO_REQUEST message\n");
	if (ssl->flags & SSL_FLAGS_ERROR || ssl->flags & SSL_FLAGS_CLOSED) {
		psTraceInfo("SSL flag error in matrixSslEncodeHelloRequest\n");
		return MATRIXSSL_ERROR;
	}
	if (!(ssl->flags & SSL_FLAGS_SERVER) || (ssl->hsState != SSL_HS_DONE)) {
		psTraceInfo("SSL state error in matrixSslEncodeHelloRequest\n");
		return MATRIXSSL_ERROR;
	}

	c = out->end;
	end = out->buf + out->size;
	messageSize = ssl->recordHeadLen + ssl->hshakeHeadLen;
	if ((rc = writeRecordHeader(ssl, SSL_RECORD_TYPE_HANDSHAKE,
			SSL_HS_HELLO_REQUEST, &messageSize, &padLen,
			&encryptStart, &end, &c)) < 0) {
		*requiredLen = messageSize;
		return rc;
	}

	if ((rc = encryptRecord(ssl, SSL_RECORD_TYPE_HANDSHAKE, messageSize,
			padLen, encryptStart, out, &c)) < 0) {
		return rc;
	}

	if (c - out->end != messageSize) {
		psTraceInfo("Error generating hello request for write\n");
		return MATRIXSSL_ERROR;
	}
	out->end = c;

	return MATRIXSSL_SUCCESS;
}
#else /* USE_SERVER_SIDE_SSL */
int32 matrixSslEncodeHelloRequest(ssl_t *ssl, sslBuf_t *out, 
								  uint32 *requiredLen)
{
		psTraceInfo("Library not built with USE_SERVER_SIDE_SSL\n");
		return PS_UNSUPPORTED_FAIL;
}
#endif /* USE_SERVER_SIDE_SSL */


#ifdef USE_SERVER_SIDE_SSL
/*
	A fragmented write of the CERTIFICATE handhshake message.  This is the
	only handshake message that supports fragmentation because it is the only
	message where the 512byte plaintext max of the max_fragment extension can
	be exceeded.
*/
static int32 writeMultiRecordCertificate(ssl_t *ssl, sslBuf_t *out,
				int32 notEmpty, int32 totalClen, int32 lsize)
{
	psX509Cert_t	*cert, *future;
	unsigned char	*c, *end, *encryptStart;
	char			padLen;
	int32			messageSize, rc, certLen;
	int32			midWrite, midSizeWrite, countDown, firstOne = 1;
	
	c = out->end;
	end = out->buf + out->size;
	
	midSizeWrite = midWrite = 0;
	
	while (totalClen > 0) {
		if (firstOne) {
			firstOne = 0;
			countDown = ssl->maxPtFrag;
			messageSize = totalClen + lsize + ssl->recordHeadLen + ssl->hshakeHeadLen;
			if ((rc = writeRecordHeader(ssl,
					SSL_RECORD_TYPE_HANDSHAKE_FIRST_FRAG, SSL_HS_CERTIFICATE,
					&messageSize, &padLen, &encryptStart, &end, &c)) < 0) {
				return rc;
			}
			/*	Write out the certs	*/
			*c = (unsigned char)(((totalClen + (lsize - 3)) & 0xFF0000) >> 16);
			c++;
			*c = ((totalClen + (lsize - 3)) & 0xFF00) >> 8; c++;
			*c = ((totalClen + (lsize - 3)) & 0xFF); c++;
			countDown -= ssl->hshakeHeadLen + 3;
			
			if (notEmpty) {
				cert = ssl->keys->cert;
				while (cert) {
					psAssert(cert->unparsedBin != NULL);
					certLen = cert->binLen;
					midWrite = 0;
					if (certLen > 0) {
						if (countDown < 3) {
							/* Fragment falls right on cert len write.  Has
								to be at least one byte or countDown would have
								been 0 and got us out of here already*/
							*c = (unsigned char)((certLen & 0xFF0000) >> 16);
							c++; countDown--;
							midSizeWrite = 2;
							if (countDown != 0) {
								*c = (certLen & 0xFF00) >> 8; c++; countDown--;
								midSizeWrite = 1;
								if (countDown != 0) {
									*c = (certLen & 0xFF); c++; countDown--;
									midSizeWrite = 0;
								}
							}
							break;
						} else {
							*c = (unsigned char)((certLen & 0xFF0000) >> 16);
							c++;
							*c = (certLen & 0xFF00) >> 8; c++;
							*c = (certLen & 0xFF); c++;
							countDown -= 3;
						}
						midWrite = min(certLen, countDown);
						memcpy(c, cert->unparsedBin, midWrite);
						certLen -= midWrite;
						c += midWrite;
						totalClen -= midWrite;
						countDown -= midWrite;
						if (countDown == 0) {
							break;
						}
					}
					cert = cert->next;
				}
			}
			
			if ((rc = encryptRecord(ssl, SSL_RECORD_TYPE_HANDSHAKE, messageSize,
					padLen, encryptStart, out, &c)) < 0) {
				return rc;
			}
			out->end = c;
		} else {
/*
			Not-first fragments
*/
			if (midSizeWrite > 0) {
				messageSize = midSizeWrite;
			} else {
				messageSize = 0;
			}
			if ((certLen + messageSize) > ssl->maxPtFrag) {
				messageSize += ssl->maxPtFrag;
			} else {			
				messageSize += certLen;
				if (cert->next != NULL) {
					future = cert->next;
					while (future != NULL) {
						if (messageSize + future->binLen + 3 >
								(uint32)ssl->maxPtFrag) {
							messageSize = ssl->maxPtFrag;
							future = NULL;
						} else {
							messageSize += 3 + future->binLen;
							future = future->next;
						}
						
					}
				}			
			}

			countDown = messageSize;
			messageSize += ssl->recordHeadLen;
			/* Second, etc... */
			if ((rc = writeRecordHeader(ssl, SSL_RECORD_TYPE_HANDSHAKE_FRAG,
					SSL_HS_CERTIFICATE, &messageSize, &padLen, &encryptStart,
					&end, &c)) < 0) {
				return rc;
			}
			
			if (midSizeWrite > 0) {
				if (midSizeWrite == 2) {
					*c = (certLen & 0xFF00) >> 8; c++;
					*c = (certLen & 0xFF); c++;
					countDown -= 2;
				} else {
					*c = (certLen & 0xFF); c++;
					countDown -= 1;
				}
				midSizeWrite = 0;
			}
			
			if (countDown < certLen) {
				memcpy(c, cert->unparsedBin + midWrite, countDown);
				certLen -= countDown;
				c += countDown;
				totalClen -= countDown;
				midWrite += countDown;
				countDown = 0;
			} else {
				memcpy(c, cert->unparsedBin + midWrite, certLen);
				c += certLen;
				totalClen -= certLen;
				countDown -= certLen;
				certLen -= certLen;
			}
				
			while (countDown > 0) {				
				cert = cert->next;
				certLen = cert->binLen;
				midWrite = 0;
				if (countDown < 3) {
					/* Fragment falls right on cert len write */
					*c = (unsigned char)((certLen & 0xFF0000) >> 16);
					c++; countDown--;
					midSizeWrite = 2;
					if (countDown != 0) {
						*c = (certLen & 0xFF00) >> 8; c++; countDown--;
						midSizeWrite = 1;
						if (countDown != 0) {
							*c = (certLen & 0xFF); c++; countDown--;
							midSizeWrite = 0;
						}
					}
					break;
				} else {
					*c = (unsigned char)((certLen & 0xFF0000) >> 16);
					c++;
					*c = (certLen & 0xFF00) >> 8; c++;
					*c = (certLen & 0xFF); c++;
					countDown -= 3;
				}
				midWrite = min(certLen, countDown);
				memcpy(c, cert->unparsedBin, midWrite);
				certLen -= midWrite;
				c += midWrite;
				totalClen -= midWrite;
				countDown -= midWrite;
				if (countDown == 0) {
					break;
				}			
			
			}
			if ((rc = encryptRecord(ssl, SSL_RECORD_TYPE_HANDSHAKE, messageSize,
					padLen, encryptStart, out, &c)) < 0) {
				return rc;
			}
			out->end = c;
		}
	}
	
	out->end = c;
	return MATRIXSSL_SUCCESS;
}
#endif /* USE_SERVER_SIDE_SSL */	

/******************************************************************************/
/*
	Write a Certificate message.
	The encoding of the message is as follows:
		3 byte length of certificate data (network byte order)
		If there is no certificate,
			3 bytes of 0
		If there is one certificate,
			3 byte length of certificate + 3
			3 byte length of certificate
			certificate data
		For more than one certificate:
			3 byte length of all certificate data
			3 byte length of first certificate
			first certificate data
			3 byte length of second certificate
			second certificate data
	Certificate data is the base64 section of an X.509 certificate file
	in PEM format decoded to binary.  No additional interpretation is required.
*/
static int32 writeCertificate(ssl_t *ssl, sslBuf_t *out, int32 notEmpty)
{
#ifdef USE_SERVER_SIDE_SSL
	psX509Cert_t	*cert;
	uint32			certLen;
#endif /* USE_SERVER_SIDE_SSL */

	unsigned char	*c, *end, *encryptStart;
	char			padLen;
	int32			totalCertLen, lsize, messageSize, i, rc;

	psTraceStrHs("<<< %s creating CERTIFICATE  message\n",
		(ssl->flags & SSL_FLAGS_SERVER) ? "Server" : "Client");


	c = out->end;
	end = out->buf + out->size;

/*
	Determine total length of certs
*/
	totalCertLen = i = 0;
	if (notEmpty) {
#ifdef USE_SERVER_SIDE_SSL	
		cert = ssl->keys->cert;
		for (; cert != NULL; i++) {
			psAssert(cert->unparsedBin != NULL);
			totalCertLen += cert->binLen;
			cert = cert->next;
		}
#else 
		return PS_DISABLED_FEATURE_FAIL;
#endif /* USE_SERVER_SIDE_SSL */
	}
	
/*
	Account for the 3 bytes of certChain len for each cert and get messageSize
*/
	lsize = 3 + (i * 3);
	
	/* TODO DTLS: Make sure this maxPtFrag is consistent with the fragment
		extension and is not interfering with DTLS notions of fragmentation */
	if ((totalCertLen + lsize + ssl->hshakeHeadLen) > ssl->maxPtFrag) {
#ifdef USE_SERVER_SIDE_SSL	
		return writeMultiRecordCertificate(ssl, out, notEmpty,
				totalCertLen, lsize);
#endif /* USE_SERVER_SIDE_SSL */				
	} else {
		messageSize =
			ssl->recordHeadLen +
			ssl->hshakeHeadLen +
			lsize + totalCertLen;

		if ((rc = writeRecordHeader(ssl, SSL_RECORD_TYPE_HANDSHAKE,
				SSL_HS_CERTIFICATE, &messageSize, &padLen, &encryptStart,
				&end, &c)) < 0) {
			return rc;
		}

/*
		Write out the certs
*/
		*c = (unsigned char)(((totalCertLen + (lsize - 3)) & 0xFF0000) >> 16);
		c++;
		*c = ((totalCertLen + (lsize - 3)) & 0xFF00) >> 8; c++;
		*c = ((totalCertLen + (lsize - 3)) & 0xFF); c++;

#ifdef USE_SERVER_SIDE_SSL
		if (notEmpty) {
			cert = ssl->keys->cert;
			while (cert) {
				psAssert(cert->unparsedBin != NULL);
				certLen = cert->binLen;
				if (certLen > 0) {
					*c = (unsigned char)((certLen & 0xFF0000) >> 16); c++;
					*c = (certLen & 0xFF00) >> 8; c++;
					*c = (certLen & 0xFF); c++;
					memcpy(c, cert->unparsedBin, certLen);
					c += certLen;
				}
				cert = cert->next;
			}
		}
#endif /* USE_SERVER_SIDE_SSL */
	
		if ((rc = encryptRecord(ssl, SSL_RECORD_TYPE_HANDSHAKE, messageSize,
				padLen, encryptStart, out, &c)) < 0) {
			return rc;
		}

		if (c - out->end != messageSize) {
			psTraceInfo("Error parsing certificate for write\n");
			return MATRIXSSL_ERROR;
		}
		out->end = c;
	}
	return MATRIXSSL_SUCCESS;
}

/******************************************************************************/
/*
	Write the ChangeCipherSpec message.  It has its own message type
	and contains just one byte of value one.  It is not a handshake 
	message, so it isn't included in the handshake hash.
*/
static int32 writeChangeCipherSpec(ssl_t *ssl, sslBuf_t *out)
{
	unsigned char	*c, *end, *encryptStart;
	char			padLen;
	int32				messageSize, rc;

	psTraceStrHs("<<< %s creating CHANGE_CIPHER_SPEC message\n",
		(ssl->flags & SSL_FLAGS_SERVER) ? "Server" : "Client");

	c = out->end;
	end = out->buf + out->size;
	messageSize = ssl->recordHeadLen + 1;

	if ((rc = writeRecordHeader(ssl, SSL_RECORD_TYPE_CHANGE_CIPHER_SPEC, 0,
			&messageSize, &padLen, &encryptStart, &end, &c)) < 0) {
		return rc;
	}
	*c = 1; c++;

	if ((rc = encryptRecord(ssl, SSL_RECORD_TYPE_CHANGE_CIPHER_SPEC,
			messageSize, padLen, encryptStart, out, &c)) < 0) {
		return rc;
	}

	if (c - out->end != messageSize) {
		psTraceInfo("Error generating change cipher for write\n");
		return MATRIXSSL_ERROR;
	}
	out->end = c;
/*
	After the peer parses the ChangeCipherSpec message, it will expect
	the next message to be encrypted, so activate encryption on outgoing
	data now
*/
	sslActivateWriteCipher(ssl);


	return MATRIXSSL_SUCCESS;
}

/******************************************************************************/
/*
	Write the Finished message
	The message contains the 36 bytes, the 16 byte MD5 and 20 byte SHA1 hash
	of all the handshake messages so far (excluding this one!)
*/
static int32 writeFinished(ssl_t *ssl, sslBuf_t *out)
{
	unsigned char	*c, *verify, *end, *encryptStart;
	char			padLen;
	int32			messageSize, verifyLen, rc;

	psTraceStrHs("<<< %s creating FINISHED message\n",
		(ssl->flags & SSL_FLAGS_SERVER) ? "Server" : "Client");

	c = out->end;
	end = out->buf + out->size;
	verifyLen = MD5_HASH_SIZE + SHA1_HASH_SIZE;
#ifdef USE_TLS
	if (ssl->flags & SSL_FLAGS_TLS) {
		verifyLen = TLS_HS_FINISHED_SIZE;
	}
#endif /* USE_TLS */
	messageSize = ssl->recordHeadLen + ssl->hshakeHeadLen + verifyLen;

	if ((rc = writeRecordHeader(ssl, SSL_RECORD_TYPE_HANDSHAKE, SSL_HS_FINISHED,
			&messageSize, &padLen, &encryptStart, &end, &c)) < 0) {
		return rc;
	}
/*
	Output the hash of messages we've been collecting so far into the buffer
*/
	verify = c;
	c += sslSnapshotHSHash(ssl, c, ssl->flags & SSL_FLAGS_SERVER);
#ifdef ENABLE_SECURE_REHANDSHAKES
	memcpy(ssl->myVerifyData, verify, verifyLen);
	ssl->myVerifyDataLen = verifyLen;
#endif /* ENABLE_SECURE_REHANDSHAKES */	

	if ((rc = encryptRecord(ssl, SSL_RECORD_TYPE_HANDSHAKE, messageSize,
			padLen, encryptStart, out, &c)) < 0) {
		return rc;
	}

	if (c - out->end != messageSize) {
		psTraceInfo("Error generating finished for write\n");
		return MATRIXSSL_ERROR;
	}
	out->end = c;
	


#ifdef USE_CLIENT_SIDE_SSL
	if (ssl->sec.cert) {
		psX509FreeCert(ssl->sec.cert);
		ssl->sec.cert = NULL;
	}
#endif /* USE_CLIENT_SIDE_SSL */


	return MATRIXSSL_SUCCESS;
}

/******************************************************************************/
/*
	Write an Alert message
	The message contains two bytes: AlertLevel and AlertDescription
*/
static int32 writeAlert(ssl_t *ssl, unsigned char level, 
						unsigned char description, sslBuf_t *out, 
						uint32 *requiredLen)
{
	unsigned char	*c, *end, *encryptStart;
	char			padLen;
	int32				messageSize, rc;

	c = out->end;
	end = out->buf + out->size;
	messageSize = 2 + ssl->recordHeadLen;

	if ((rc = writeRecordHeader(ssl, SSL_RECORD_TYPE_ALERT, 0, &messageSize,
			&padLen, &encryptStart, &end, &c)) < 0) {
		*requiredLen = messageSize;
		return rc;
	}
	*c = level; c++;
	*c = description; c++;

	if ((rc = encryptRecord(ssl, SSL_RECORD_TYPE_ALERT, messageSize,
			padLen, encryptStart, out, &c)) < 0) {
		*requiredLen = messageSize;
		return rc;
	}

	out->end = c;
	return MATRIXSSL_SUCCESS;
}

#ifdef USE_CLIENT_SIDE_SSL
/******************************************************************************/
/*
	Write out the ClientHello message to a buffer
*/
int32 matrixSslEncodeClientHello(ssl_t *ssl, sslBuf_t *out, uint32 cipherSpec, 
					uint32 *requiredLen, tlsExtension_t *userExt)
{
	unsigned char	*c, *end, *encryptStart;
	char			padLen;
	int32			messageSize, rc, cipherLen, cookieLen, addScsv, t;
	psTime_t		pst;
	tlsExtension_t	*ext;
	uint32			extLen;
	sslCipherSpec_t	*cipherDetails;

	psTraceHs("<<< Client creating CLIENT_HELLO  message\n");
	*requiredLen = 0;
	if (out == NULL || out->buf == NULL || ssl == NULL) {
		return PS_ARG_FAIL;
	}
	if (ssl->flags & SSL_FLAGS_ERROR || ssl->flags & SSL_FLAGS_CLOSED) {
		psTraceInfo("SSL flag error in matrixSslEncodeClientHello\n");
		return MATRIXSSL_ERROR;
	}
	if (ssl->flags & SSL_FLAGS_SERVER || (ssl->hsState != SSL_HS_SERVER_HELLO &&
			ssl->hsState != SSL_HS_DONE &&
			ssl->hsState != SSL_HS_HELLO_REQUEST )) {
		psTraceInfo("SSL state error in matrixSslEncodeClientHello\n");	
		return MATRIXSSL_ERROR;
	}
	
	sslInitHSHash(ssl);

	cookieLen = 0;
/*
	If session resumption is being done on a rehandshake we send our full
	list of supported suites to support a server that is trying to change
	the cipher suite on the renegotiation. If no resumption, clear the RESUMED
	flag in case the caller is attempting to bypass matrixSslEncodeRehandshake.
*/
	if (ssl->sessionIdLen > 0) {
		cipherSpec = 0;
	} else {
		ssl->flags &= ~SSL_FLAGS_RESUMED;
	}

/*
	If a cipher is specified it is two bytes length and two bytes data. 
*/
	if (cipherSpec == 0) {
		if ((cipherLen = sslGetCipherSpecListLen(ssl)) == 2) {
			psTraceInfo("No cipher suites enabled (or no key material)\n");
			return MATRIXSSL_ERROR;
		}
	} else {
		if ((cipherDetails = sslGetCipherSpec(ssl, cipherSpec)) == NULL) {
			psTraceIntInfo("Cipher suite not supported: %d\n", cipherSpec);
			return PS_UNSUPPORTED_FAIL;
		}
		cipherLen = 4;
	}
	
	addScsv = 0;
#ifdef ENABLE_SECURE_REHANDSHAKES
/*
	Initial CLIENT_HELLO will use the SCSV mechanism for greatest compat
*/
	if (ssl->myVerifyDataLen == 0) {
		cipherLen += 2; /* cipher id 0x00FF */
		addScsv = 1;
	}
#endif

/*
	Calculate the size of the message up front, and write header
*/
	messageSize = ssl->recordHeadLen + ssl->hshakeHeadLen +
		5 + SSL_HS_RANDOM_SIZE + ssl->sessionIdLen + cipherLen + cookieLen;
	
/*
	Extension lengths
*/
	extLen = 0;
	
/*
	Max Fragment extension request
*/
	ssl->maxPtFrag = SSL_MAX_PLAINTEXT_LEN;
	if (ssl->minVer > 0 &&
			(REQUESTED_MAX_PLAINTEXT_RECORD_LEN < SSL_MAX_PLAINTEXT_LEN)) {
		if (REQUESTED_MAX_PLAINTEXT_RECORD_LEN == 0x200 ||
				REQUESTED_MAX_PLAINTEXT_RECORD_LEN == 0x400 ||
				REQUESTED_MAX_PLAINTEXT_RECORD_LEN == 0x800 ||
				REQUESTED_MAX_PLAINTEXT_RECORD_LEN == 0x1000) {
			extLen = 2 + 5; /* 2 for total ext len + 5 for ourselves */
			/* Also indicate that we're requesting a different plaintext size */
			ssl->maxPtFrag = 0XFF;
		}			
	}
#ifdef ENABLE_SECURE_REHANDSHAKES
/*
	Subsequent CLIENT_HELLOs must use a populated RenegotiationInfo extension
*/	
	if (ssl->myVerifyDataLen != 0) {
		if (extLen == 0) {
			extLen = 2; /* First extension found so total len */
		}
		extLen += ssl->myVerifyDataLen + 5; /* 5 type/len/len */
	}
#endif /* ENABLE_SECURE_REHANDSHAKES */


/*
	Add any user-provided extensions
*/
	ext = userExt;
	if (ext && extLen == 0) {
		extLen = 2; /* Start with the initial len */
	}	
	while (ext) {		
		extLen += ext->extLen + 4; /* +4 for type and length of each */ 
		ext = ext->next;
	}
	messageSize += extLen;

	c = out->end;
	end = out->buf + out->size;

	if ((rc = writeRecordHeader(ssl, SSL_RECORD_TYPE_HANDSHAKE,
			SSL_HS_CLIENT_HELLO, &messageSize, &padLen, &encryptStart,
			&end, &c)) < 0) {
		*requiredLen = messageSize;
		return rc;
	}

/*
	First 4 bytes of the serverRandom are the unix time to prevent replay
	attacks, the rest are random
*/
	t = psGetTime(&pst);
	ssl->sec.clientRandom[0] = (unsigned char)((t & 0xFF000000) >> 24);
	ssl->sec.clientRandom[1] = (unsigned char)((t & 0xFF0000) >> 16);
	ssl->sec.clientRandom[2] = (unsigned char)((t & 0xFF00) >> 8);
	ssl->sec.clientRandom[3] = (unsigned char)(t & 0xFF);
	if ((rc = matrixSslGetPrngData(ssl->sec.clientRandom + 4,
			SSL_HS_RANDOM_SIZE - 4)) < PS_SUCCESS) {
		return rc;
	}
/*
	First two fields in the ClientHello message are the maximum major 
	and minor SSL protocol versions we support
*/
	*c = ssl->majVer; c++;
	*c = ssl->minVer; c++;
/*
	The next 32 bytes are the server's random value, to be combined with
	the client random and premaster for key generation later
*/
	memcpy(c, ssl->sec.clientRandom, SSL_HS_RANDOM_SIZE);
	c += SSL_HS_RANDOM_SIZE;
/*
	The next data is a single byte containing the session ID length,
	and up to 32 bytes containing the session id.
	If we are asking to resume a session, then the sessionId would have
	been set at session creation time.
*/
	*c = (unsigned char)ssl->sessionIdLen; c++;
	if (ssl->sessionIdLen > 0) {
        memcpy(c, ssl->sessionId, ssl->sessionIdLen);
		c += ssl->sessionIdLen;
	}
/*
	Write out the length and ciphers we support
	Client can request a single specific cipher in the cipherSpec param
*/
	if (cipherSpec == 0) {
		if ((rc = sslGetCipherSpecList(ssl, c, (int32)(end - c), addScsv)) < 0){
			return SSL_FULL;
		}
		c += rc;
	} else {
		if ((int32)(end - c) < cipherLen) {
			return SSL_FULL;
		}
		cipherLen -= 2; /* don't include yourself */
		*c = (cipherLen & 0xFF00) >> 8; c++;
		*c = cipherLen & 0xFF; c++;
		*c = (cipherSpec & 0xFF00) >> 8; c++;
		*c = cipherSpec & 0xFF; c++;
#ifdef ENABLE_SECURE_REHANDSHAKES
		if (addScsv == 1) {
			*c = ((TLS_EMPTY_RENEGOTIATION_INFO_SCSV & 0xFF00) >> 8); c++;
			*c = TLS_EMPTY_RENEGOTIATION_INFO_SCSV  & 0xFF; c++;
		}
#endif			
	}	
/*
	Followed by two bytes (len and compression method (always zero))
*/
	*c = 1; c++;
	*c = 0; c++;
/*
	Extensions
*/
	if (extLen > 0) {
		extLen -= 2; /* Don't include yourself in the length */
		*c = (extLen & 0xFF00) >> 8; c++; /* Total list length */
		*c = extLen & 0xFF; c++;
		
		/* Max fragment extension */
		if (ssl->maxPtFrag == 0XFF) {
			*c = 0x00; c++;
			*c = 0x01; c++;
			*c = 0x00; c++;
			*c = 0x01; c++;
			if (REQUESTED_MAX_PLAINTEXT_RECORD_LEN == 0x200) {
				*c = 0x01; c++;
			} else if (REQUESTED_MAX_PLAINTEXT_RECORD_LEN == 0x400) {
				*c = 0x02; c++;
			} else if (REQUESTED_MAX_PLAINTEXT_RECORD_LEN == 0x800) {
				*c = 0x03; c++;
			} else if (REQUESTED_MAX_PLAINTEXT_RECORD_LEN == 0x1000) {
				*c = 0x04; c++;
			}
		}
#ifdef ENABLE_SECURE_REHANDSHAKES	
/*
		Populated RenegotiationInfo extension
*/
		if (ssl->myVerifyDataLen > 0) {
			*c = (EXT_RENEGOTIATION_INFO & 0xFF00) >> 8; c++;
			*c = EXT_RENEGOTIATION_INFO & 0xFF; c++;
			*c = ((ssl->myVerifyDataLen + 1) & 0xFF00) >> 8; c++;
			*c = (ssl->myVerifyDataLen + 1) & 0xFF; c++;
			*c = ssl->myVerifyDataLen & 0xFF; c++;
			memcpy(c, ssl->myVerifyData, ssl->myVerifyDataLen);
			c += ssl->myVerifyDataLen;
		}
#endif /* ENABLE_SECURE_REHANDSHAKES */	

/*
		User-provided extensions
*/
		if (userExt) {
			ext = userExt;
			while (ext) {
				*c = (ext->extType & 0xFF00) >> 8; c++;
				*c = ext->extType & 0xFF; c++;
			
				*c = (ext->extLen & 0xFF00) >> 8; c++;
				*c = ext->extLen & 0xFF; c++;
				if (ext->extLen == 1 && ext->extData == NULL) {
					memset(c, 0x0, 1);
				} else {
					memcpy(c, ext->extData, ext->extLen);
				}
				c += ext->extLen;
				ext = ext->next;
			}
		}
	}

	if ((rc = encryptRecord(ssl, SSL_RECORD_TYPE_HANDSHAKE, messageSize,
			padLen, encryptStart, out, &c)) < 0) {
		return rc;
	}
/*
	Verify that we've calculated the messageSize correctly, really this
	should never fail; it's more of an implementation verification
*/
	if (c - out->end != messageSize) {
		return MATRIXSSL_ERROR;
	}
	out->end = c;

/*
	Could be a rehandshake so clean	up old context if necessary.
	Always explicitly set state to beginning.  
*/
	if (ssl->hsState == SSL_HS_DONE) {
		sslResetContext(ssl);
	}

/*
	Could be a rehandshake on a previous connection that used client auth.
	Reset our local client auth state as the server is always the one
	responsible for initiating it.
*/
	ssl->flags &= ~SSL_FLAGS_CLIENT_AUTH;
	ssl->hsState = SSL_HS_SERVER_HELLO;
	return MATRIXSSL_SUCCESS;
}

/******************************************************************************/
/*
	Write a ClientKeyExchange message.
*/
static int32 writeClientKeyExchange(ssl_t *ssl, sslBuf_t *out)
{
	unsigned char	*c, *end, *encryptStart;
	char			padLen;
	int32			messageSize, explicitLen, rc;
	uint32			keyLen;
	psPool_t		*pkiPool = NULL;
	void			*rsaData = NULL;

	psTraceHs("<<< Client creating CLIENT_KEY_EXCHANGE message\n");

	c = out->end;
	end = out->buf + out->size;
	messageSize = keyLen = 0;


/*
	Determine messageSize for the record header
*/
			/* Standard RSA auth suites */
			keyLen = ssl->sec.cert->publicKey.keysize;

	messageSize += ssl->recordHeadLen + ssl->hshakeHeadLen + keyLen;
	explicitLen = 0;
#ifdef USE_TLS
/*
	Must add the key size length to the message
*/
	if (ssl->flags & SSL_FLAGS_TLS) {
		messageSize += 2;
		explicitLen = 1;		
	}
#endif /* USE_TLS */


	if ((rc = writeRecordHeader(ssl, SSL_RECORD_TYPE_HANDSHAKE,
			SSL_HS_CLIENT_KEY_EXCHANGE, &messageSize, &padLen,
			&encryptStart, &end, &c)) < 0) {
		return rc;
	}
		
/*
	ClientKeyExchange message contains the encrypted premaster secret.
	The base premaster is the original SSL protocol version we asked for
	followed by 46 bytes of random data.
	These 48 bytes are padded to the current RSA key length and encrypted
	with the RSA key.
*/
	if (explicitLen == 1) {
/*
		Add the two bytes of key length
*/
		if (keyLen > 0) {
			*c = (keyLen & 0xFF00) >> 8; c++;
			*c = (keyLen & 0xFF); c++;
		}
	}



/*
			Standard RSA suite
*/
			ssl->sec.premasterSize = SSL_HS_RSA_PREMASTER_SIZE;
			ssl->sec.premaster = psMalloc(ssl->hsPool,
										  SSL_HS_RSA_PREMASTER_SIZE);
			if (ssl->sec.premaster == NULL) {
				return SSL_MEM_ERROR;
			}							  
			ssl->sec.premaster[0] = ssl->reqMajVer;
			ssl->sec.premaster[1] = ssl->reqMinVer;
			if (matrixSslGetPrngData(ssl->sec.premaster + 2,
					SSL_HS_RSA_PREMASTER_SIZE - 2) < 0) {
				return MATRIXSSL_ERROR;
			}
			
			if (csRsaEncryptPub(pkiPool, &ssl->sec.cert->publicKey,
					ssl->sec.premaster, ssl->sec.premasterSize, c,
					(uint32)(end - c), rsaData) != (int32)keyLen) {
				return MATRIXSSL_ERROR;
			}
			c += keyLen;
		

	if ((rc = encryptRecord(ssl, SSL_RECORD_TYPE_HANDSHAKE, messageSize,
			padLen, encryptStart, out, &c)) < 0) {
		return rc;
	}

	if (c - out->end != messageSize) {
		psTraceInfo("Invalid ClientKeyExchange length\n");
		return MATRIXSSL_ERROR;
	}

/*
	Now that we've got the premaster secret, derive the various symmetric
	keys using it and the client and server random values
*/
	sslCreateKeys(ssl);

	out->end = c;
	return MATRIXSSL_SUCCESS;
}


#else /* USE_CLIENT_SIDE_SSL */
/******************************************************************************/
/*
	Stub out this function rather than ifdef it out in the public header
*/
int32 matrixSslEncodeClientHello(ssl_t *ssl, sslBuf_t *out, uint32 cipherSpec, 
								 uint32 *requiredLen, tlsExtension_t *userExt)
{
	psTraceInfo("Library not built with USE_CLIENT_SIDE_SSL\n");
	return PS_UNSUPPORTED_FAIL;
}
#endif /* USE_CLIENT_SIDE_SSL */



/******************************************************************************/
/*
	Write out a SSLv3 record header.
	Assumes 'c' points to a buffer of at least SSL3_HEADER_LEN bytes
		1 byte type (SSL_RECORD_TYPE_*)
		1 byte major version
		1 byte minor version
		2 bytes length (network byte order)
	Returns the number of bytes written
*/
int32 psWriteRecordInfo(ssl_t *ssl, unsigned char type, int32 len, 
							   unsigned char *c)
{
	if (type == SSL_RECORD_TYPE_HANDSHAKE_FRAG) {
		type = SSL_RECORD_TYPE_HANDSHAKE;
	}
	*c = type; c++;
	*c = ssl->majVer; c++;
	*c = ssl->minVer; c++;
	*c = (len & 0xFF00) >> 8; c++;
	*c = (len & 0xFF);
	

	return ssl->recordHeadLen;
}

/******************************************************************************/
/*
	Write out an ssl handshake message header.
	Assumes 'c' points to a buffer of at least ssl->hshakeHeadLen bytes
		1 byte type (SSL_HS_*)
		3 bytes length (network byte order)
	Returns the number of bytes written
*/
int32 psWriteHandshakeHeader(ssl_t *ssl, unsigned char type, int32 len, 
								int32 seq, int32 fragOffset, int32 fragLen,
								unsigned char *c)
{
	*c = type; c++;
	*c = (unsigned char)((len & 0xFF0000) >> 16); c++;
	*c = (len & 0xFF00) >> 8; c++;
	*c = (len & 0xFF);

	return ssl->hshakeHeadLen;
}

/******************************************************************************/
/*
	Write pad bytes and pad length per the TLS spec.  Most block cipher
	padding fills each byte with the number of padding bytes, but SSL/TLS
	pretends one of these bytes is a pad length, and the remaining bytes are
	filled with that length.  The end result is that the padding is identical
	to standard padding except the values are one less. For SSLv3 we are not
	required to have any specific pad values, but they don't hurt.

	PadLen	Result
	0
	1		00
	2		01 01
	3		02 02 02
	4		03 03 03 03
	5		04 04 04 04 04
	6		05 05 05 05 05 05
	7		06 06 06 06 06 06 06
	8		07 07 07 07 07 07 07 07
	9		08 08 08 08 08 08 08 08 08
	...
	15		...

	We calculate the length of padding required for a record using
	psPadLenPwr2()
*/
int32 sslWritePad(unsigned char *p, unsigned char padLen)
{
	unsigned char c = padLen;

	while (c-- > 0) {
		*p++ = padLen - 1;
	}
	return padLen;
}

/******************************************************************************/
