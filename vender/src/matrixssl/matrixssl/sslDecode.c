/*
 *	sslDecode.c
 *	Release $Name: MATRIXSSL-3-4-2-OPEN $
 *
 *	Secure Sockets Layer protocol message decoding portion of MatrixSSL
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

#define SSL_MAX_IGNORED_MESSAGE_COUNT	1024

static int32 parseSSLHandshake(ssl_t *ssl, char *inbuf, uint32 len);

#ifdef USE_CERT_CHAIN_PARSING
static int32 parseSingleCert(ssl_t *ssl, unsigned char *c, unsigned char *end, 
						   int32 certLen);
#endif /* USE_CERT_CHAIN_PARSING */

static int32 addCompressCount(ssl_t *ssl, int32 padLen);
/******************************************************************************/
/*
	Parse incoming data per http://wp.netscape.com/eng/ssl3
	
	Input parameters to decode:
	.	buf points to the start of data to decode
	.	len points to the length in bytes of data to decode
	.	size is the number of allocated bytes that follow buf
	

	
	Meaningful parameters after the call to decode:
	MATRIXSSL_SUCCESS
	.	buf will point to the first undecoded byte (could==inbuf or inbuf+inlen)
	.	remaining will indicate how many more bytes of undecoded data remain
	*	call again if more to decode or return if handshake is complete
	
	SSL_PARTIAL
	.	buf will not have moved (because partials start parse over)
	.	reqLen will indicate how many bytes the entire full record is 
	*	get more data from peer and call again
	
	SSL_FULL (implies decode completed fully but couldn't fit response)
	.	buf will not have moved (it is reset to the front of final record)
	.	len will be 0 to indicate no remaining unprocessed data
	.	reqLen will inform how large buf should be grown before re-invoking
	*	realloc the buf to the reqLen size and call again
	
	SSL_SEND_RESPONSE
	.	buf will point to the encoded handshake data to send
	.	len will be length of data to send (from start offset)
	*	pass the buf to the transport layer for sending to peer
	
	SSL_ALERT
	.	buf will point to start of received alert (2 bytes alert level and desc)
	.	len will be length of alert data (should be 2)
	.	alertLevel will be 1 (warning) or 2 (fatal)
	.	alertDesc will be SSL specified alert code
	
	MATRIXSSL_ERROR (unrecoverable failure)
	.	decodeErr is internal parse err code
	
	SSL_PROCESS_DATA (ONLY CASE WITH DECRYPTED DATA AND POSSIBLE UNENCRYPTED)
	.	unencrypted user data ready for processing is at prevBuf
	.	buf points to start of any remaining unencrypted data
	.	remaining is length of remaining encrypted data yet to decode
	.	len is length of unencrypted data ready for user processing
	*	pass unencypted data to application level
	*	call decode again if more encrypted data remaining
	
*/
int32 matrixSslDecode(ssl_t *ssl, unsigned char **buf, uint32 *len,
					uint32 size, uint32 *remaining, uint32 *requiredLen,
					int32 *error, unsigned char *alertLevel,
					unsigned char *alertDescription)
{
	unsigned char	*c, *p, *end, *pend, *ctStart, *origbuf;
	unsigned char	*mac, macError;
	int32			rc;
	unsigned char	padLen;
	psBuf_t			tmpout;
	psDigestContext_t	dummyMd;
#ifdef USE_CERT_CHAIN_PARSING
	int32			certlen, i, nextCertLen;
#endif /* USE_CERT_CHAIN_PARSING */
/*
	If we've had a protocol error, don't allow further use of the session
*/
	*error = PS_SUCCESS;
	if (ssl->flags & SSL_FLAGS_ERROR || ssl->flags & SSL_FLAGS_CLOSED) {
		psTraceInfo("Can't use matrixSslDecode on closed/error-flagged sess\n");
		*error = PS_PROTOCOL_FAIL;
		return MATRIXSSL_ERROR;
	}

	origbuf = *buf;	/* Save the original buffer location */


/*
	This flag is set if the previous call to this routine returned an SSL_FULL
	error from encodeResponse, indicating that there is data to be encoded, 
	but the out buffer was not big enough to handle it.  If we fall in this 
	case, the user has increased the out buffer size and is re-calling this 
	routine
*/
	if (ssl->flags & SSL_FLAGS_NEED_ENCODE) {
		ssl->flags &= ~SSL_FLAGS_NEED_ENCODE;
		goto encodeResponse;
	}
	*requiredLen = 0;
	c = *buf; /* c is record parse pointer */
	end = *buf + *len;
	
/*
	Processing the SSL Record header:
	If the high bit of the first byte is set and this is the first 
	message we've seen, we parse the request as an SSLv2 request
	http://wp.netscape.com/eng/security/SSL_2.html
	SSLv2 also supports a 3 byte header when padding is used, but this should 
	not be required for the initial plaintext message, so we don't support it
	v3 Header:
		1 byte type
		1 byte major version
		1 byte minor version
		2 bytes length
	v2 Header
		2 bytes length (ignore high bit)
*/
	if (end - c == 0) {
/*
		This case could happen if change cipher spec was last
		message	in the buffer or if there is a zero-length record
		at the end of a multi-record application data buffer.
*/
		return MATRIXSSL_SUCCESS;
	}
	if (end - c < SSL2_HEADER_LEN) {
		*requiredLen = SSL2_HEADER_LEN;
		return SSL_PARTIAL;
	}
#ifdef USE_CERT_CHAIN_PARSING
/*
	If we're in process of parsing a partial record, then skip the 
	usual record header parse.  Currently we're only supporting
	partial parsing for the certificate messages since they are the
	largest in size.
*/
	if (ssl->rec.partial != 0x0) {
		psAssert(ssl->rec.type == SSL_RECORD_TYPE_HANDSHAKE);
		psAssert(ssl->hsState == SSL_HS_CERTIFICATE);
/*
		Get this next record length based on the certificate size,
		which will always be the first three bytes of a partial here
*/
		ssl->rec.len = c[0] << 16;
		ssl->rec.len |= c[1] << 8;
		ssl->rec.len |= c[2];
		ssl->rec.len += 3;
		goto SKIP_RECORD_PARSE;
	}
#endif /* USE_CERT_CHAIN_PARSING */

	if (ssl->majVer != 0 || (*c & 0x80) == 0) {
		if (end - c < ssl->recordHeadLen) {
			*requiredLen = ssl->recordHeadLen;
			return SSL_PARTIAL;
		}
		ssl->rec.type = *c; c++;
		ssl->rec.majVer = *c; c++;
		ssl->rec.minVer = *c; c++;
		ssl->rec.len = *c << 8; c++;
		ssl->rec.len += *c; c++;
	} else {
		ssl->rec.type = SSL_RECORD_TYPE_HANDSHAKE;
		ssl->rec.majVer = 2;
		ssl->rec.minVer = 0;
		ssl->rec.len = (*c & 0x7f) << 8; c++;
		ssl->rec.len += *c; c++;
	}
/*
	Validate the various record headers.  The type must be valid,
	the major and minor versions must match the negotiated versions (if we're
	past ClientHello) and the length must be < 16K and > 0
*/
	if (ssl->rec.type != SSL_RECORD_TYPE_CHANGE_CIPHER_SPEC &&
			ssl->rec.type != SSL_RECORD_TYPE_ALERT &&
			ssl->rec.type != SSL_RECORD_TYPE_HANDSHAKE &&
			ssl->rec.type != SSL_RECORD_TYPE_APPLICATION_DATA) {
		ssl->err = SSL_ALERT_UNEXPECTED_MESSAGE;
		psTraceIntInfo("Record header type not valid: %d\n", ssl->rec.type);
		goto encodeResponse;
	}

/*
	Verify the record version numbers unless this is the first record we're
	reading.
*/
	if (ssl->hsState != SSL_HS_SERVER_HELLO &&
			ssl->hsState != SSL_HS_CLIENT_HELLO) {
		if (ssl->rec.majVer != ssl->majVer || ssl->rec.minVer != ssl->minVer) {
#ifdef SSL_REHANDSHAKES_ENABLED
			/* If in DONE state and this version doesn't match the previously
				negotiated one that can be OK because a CLIENT_HELLO for a
				rehandshake might be acting like a first time send and using
				a lower version to get to the parsing phase.  Unsupported 
				versions will be weeded out at CLIENT_HELLO parse time */
			if (ssl->hsState != SSL_HS_DONE ||
					ssl->rec.type != SSL_RECORD_TYPE_HANDSHAKE) {
				ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
				psTraceInfo("Record header version not valid\n");
				goto encodeResponse;
			}
#else
			ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
			psTraceInfo("Record header version not valid\n");
			goto encodeResponse;
#endif	
		}
	}
/*
	Verify max and min record lengths
*/
	if (ssl->rec.len > SSL_MAX_RECORD_LEN || ssl->rec.len == 0) {
		ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
		psTraceIntInfo("Record header length not valid: %d\n", ssl->rec.len);
		goto encodeResponse;
	}
/*
	This implementation requires the entire SSL record to be in the 'in' buffer
	before we parse it.  This is because we need to MAC the entire record before
	allowing it to be used by the caller.
*/
#ifdef USE_CERT_CHAIN_PARSING
SKIP_RECORD_PARSE:
	if ((end - c < ssl->rec.len) || ssl->rec.partial) {
/*
		This feature will only work if the CERTIFICATE message is sent in a
		different record from the SERVER_HELLO message.
*/
		if (ssl->hsState != SSL_HS_CERTIFICATE) {
			ssl->rec.partial = 0x0;
			*requiredLen = ssl->rec.len + ssl->recordHeadLen;
			return SSL_PARTIAL;
		}
/*
		Not supporting cert stream parsing for re-handshake.  This is
		important because the block cipher assumes a single pass is a record
		and will use explicit IV each pass */
*/
		if (ssl->flags & SSL_FLAGS_READ_SECURE) {
			ssl->rec.partial = 0x0;
			*requiredLen = ssl->rec.len + ssl->recordHeadLen;
			return SSL_PARTIAL;
		}
/*
		Manipulate the rec.len for partial handling
*/
		i = 0;
		if (ssl->rec.partial == 0x0) {
/*
			Initialization for partial parse counters
*/
			ssl->rec.hsBytesHashed = 0;
			ssl->rec.hsBytesParsed = 0;
			ssl->rec.partial = 0x1;
			ssl->rec.trueLen = ssl->rec.len + ssl->recordHeadLen;
			ssl->rec.len = 0;
/*
			Best to identify and isolate full certificate boundaries
			ASAP to keep parsing logic as high level as possible.

			Current state of record buffer: pointer at start of HS record
			which begins with 4 bytes of hsType(1) and hsLen(3).  After
			the header are 3 bytes of certchainlen and 3 bytes of first
			cert len.  Make sure we have at least one full cert here before
			allowing the partial parse.
*/
			if (end - c < (ssl->hshakeHeadLen + 6)) { /* 3*2 cert chain len */
				ssl->rec.partial = 0x0; /* Unusable.  Reset */
				*requiredLen = ssl->hshakeHeadLen + 6;
				return SSL_PARTIAL;
			}
			ssl->rec.len += (ssl->hshakeHeadLen + 3);
			i = ssl->hshakeHeadLen;
			certlen = c[i] << 16; i++;
			certlen |= c[i] << 8; i++;
			certlen |= c[i]; i++;
/*
			This feature only works if the CERTIFICATE message is the only
			message in the record.  Test this by seeing that trueLen doesn't
			claim there is more to follow
*/
			if (ssl->rec.trueLen != (certlen + 3 + ssl->hshakeHeadLen +
					ssl->recordHeadLen)) {
				ssl->rec.partial = 0x0; /* Unusable.  Reset */
				*requiredLen = ssl->rec.trueLen;
				return SSL_PARTIAL;
			}
/*
			First cert length
*/
			ssl->rec.len += 3;
			certlen = c[i] << 16; i++;
			certlen |= c[i] << 8; i++;
			certlen |= c[i];
			ssl->rec.len += certlen;
		}
/*
		One complete cert?
*/	
		if (end - c < ssl->rec.len) {
/*
			If there isn't a full cert in the first partial, we reset and
			handle as the standard SSL_PARTIAL case.
*/
			if (ssl->rec.hsBytesParsed == 0) {
				ssl->rec.partial = 0x0; /* Unusable.  Reset */
				*requiredLen = ssl->rec.len + ssl->recordHeadLen;
			} else {
				/* Record header has already been parsed */
				*requiredLen = ssl->rec.len;
			}
			return SSL_PARTIAL; /* Standard partial case */
		}
/*
		More than one complete cert?
*/	
		while (end - c > ssl->rec.len)
		{
			if (ssl->rec.len + ssl->rec.hsBytesParsed == ssl->rec.trueLen) {
/*				
				Don't try to read another cert if the total of already parsed
				record and the length of the current record match the 'trueLen'.
				If they are equal, we know we are on the final cert and don't
				need to look for more
*/
				break;
			}
			psAssert(ssl->rec.len + ssl->rec.hsBytesParsed <= ssl->rec.trueLen);
			nextCertLen = c[ssl->rec.len] << 16;
			nextCertLen |= c[ssl->rec.len + 1] << 8;
			nextCertLen |= c[ssl->rec.len + 2];
			if (end - c > (ssl->rec.len + nextCertLen + 3)) {
				ssl->rec.len += (nextCertLen + 3);
			} else {
				break;
			}
		}
	}
#else
	if (end - c < ssl->rec.len) {	
		*requiredLen = ssl->rec.len + ssl->recordHeadLen;
		return SSL_PARTIAL;
		
	}
#endif

/*
	Decrypt the entire record contents.  The record length should be
	a multiple of block size, or decrypt will return an error
	If we're still handshaking and sending plaintext, the decryption 
	callback will point to a null provider that passes the data unchanged
*/
	ctStart = origbuf; /* Clear-text start.  Decrypt to the front */


	/* Sanity check ct len.  Step 1 of Lucky 13 MEE-TLS-CBC decryption. 
		max{b, t + 1} is always "t + 1" because largest possible blocksize
		is 16 and smallest possible tag len is 16. Multiple of block size test
		is done in decrypt */
	if ((ssl->flags & SSL_FLAGS_READ_SECURE) && (ssl->deBlockSize > 1)) {

#ifdef USE_TLS_1_1
		if (ssl->flags & SSL_FLAGS_TLS_1_1) {
			if (ssl->rec.len < (ssl->deMacSize + 1 + ssl->deBlockSize)) {
				ssl->err = SSL_ALERT_DECRYPTION_FAILED;
				psTraceInfo("Ciphertext length failed sanity\n"); 
				goto encodeResponse;
			}
		} else {
			if (ssl->rec.len < (ssl->deMacSize + 1)) {
				ssl->err = SSL_ALERT_DECRYPTION_FAILED;
				psTraceInfo("Ciphertext length failed sanity\n"); 
				goto encodeResponse;
			}
		}
#else
		if (ssl->rec.len < (ssl->deMacSize + 1)) {
			ssl->err = SSL_ALERT_DECRYPTION_FAILED;
			psTraceInfo("Ciphertext length failed sanity\n"); 
			goto encodeResponse;
		}
#endif /* USE_TLS_1_1 */
	}

	/* CT to PT */
	if (ssl->decrypt(ssl, c, ctStart, ssl->rec.len) < 0) {
		ssl->err = SSL_ALERT_DECRYPTION_FAILED;
		psTraceInfo("Couldn't decrypt record data\n"); 
		goto encodeResponse;
	}
	
	c += ssl->rec.len;

/*
	If we're reading a secure message, we need to validate the MAC and 
	padding (if using a block cipher).  Insecure messages do not have 
	a trailing MAC or any padding.

	SECURITY - There are several vulnerabilities in block cipher padding
	that we handle in the below code.  For more information see:
	http://www.openssl.org/~bodo/tls-cbc.txt
*/
	if (ssl->flags & SSL_FLAGS_READ_SECURE) {
/*
		Start tracking MAC errors, rather then immediately catching them to
		stop timing and alert description attacks that differentiate between
		a padding error and a MAC error.
*/
		macError = 0;
/*
		Decode padding only if blocksize is > 0 (we're using a block cipher),
		otherwise no padding will be present, and the mac is the last 
		macSize bytes of the record.
*/
		if (ssl->deBlockSize <= 1) {
			mac = ctStart + ssl->rec.len - ssl->deMacSize;
		} else {
/*
			The goal from here through completion of ssl->verifyMac call is a
			constant processing time for a given record length.  Going to
			follow the suggestions of the Lucky 13 research paper section
			"Careful implementation of MEE-TLS-CBC decryption".
			http://www.isg.rhul.ac.uk/tls/TLStiming.pdf

			Consistent timing is still a "goal" here.  This implementation
			accounts for the largest timing discrepencies but is not a 
			strict "clock cycles" equalizer.  The complexity of the attack
			circumstances and plaintext recovery possibilities using these
			techniques is almost entirely in the academic realm. Improvements
			to this code will be an ongoing process as research uncovers
			more practical plaintext recovery threats.
			
			Our first step is to create a hash context that might possibly
			be used to compress some dummy data for Step 5.
*/
				psSha1Init(&dummyMd);
/*			
			Verify the pad data for block ciphers
			c points within the cipher text, p points within the plaintext
			The last byte of the record is the pad length
*/
			p = ctStart + ssl->rec.len;
			padLen = *(p - 1);
/*
			SSL3.0 requires the pad length to be less than blockSize
			TLS can have a pad length up to 255 for obfuscating the data len
*/
			if (ssl->majVer == SSL3_MAJ_VER && ssl->minVer == SSL3_MIN_VER && 
					padLen >= ssl->deBlockSize) {
				macError = 1;
			}
/*
			The minimum record length is the size of the mac, plus pad bytes
			plus one length byte
*/
			if (ssl->rec.len < ssl->deMacSize + padLen + 1) {
				/* Step 3 of Lucky 13 MEE-TLS-CBC decryption: Run a loop as
					if there were 256 bytes of padding, with a dummy check
					in each iteration*/
				for (rc = 255; rc >= 0; rc--) {
					/* make the test a moving target so it doesn't get 
						optimized out at compile. The loop is written
						this way so the macError assignment will be done
						only once */
					if ((unsigned char)rc == padLen) {
						macError = 1;	/* No incr to avoid any wraps */
					}
				}
			}
#ifdef USE_TLS
/*
			TLS specifies that all pad bytes must have the same value
			as the final pad length byte.  Some SSL3 implementations also 
			do this by convention, but some just fill with random bytes.
			(We're just overloading the 'mac' ptr here, this has nothing to 
			do with real MAC.)
*/
			if (!macError && ssl->majVer == TLS_MAJ_VER &&
					ssl->minVer >= TLS_MIN_VER) {
				for (mac = p - padLen - 1; mac < p; mac++) {
					if (*mac != padLen) {
						macError = 1;
					}
				}
				/* Lucky 13 step 4. If this fails, then run a loop as if there
					were 256 - padlen - 1 bytes of padding, with a dummy
					check in each iteration */
				if (macError) {
					for (rc = 256 - padLen - 1; rc > 0; rc--) {
						/* make the test a moving target so it doesn't get 
							optimized out at compile.  Again, make it so
							the loop condition doesn't get hit more than
							once. */
						if ((unsigned char)rc == padLen) {
							macError = 2; /* change value for smart compilers */
						}
					}
				}	
			}
#endif /* USE_TLS */
/*
			The mac starts macSize bytes before the padding and length byte.
			If we have a macError, just fake the mac as the last macSize bytes
			of the record, so we are sure to have enough bytes to verify
			against, we'll fail anyway, so the actual contents don't matter.
*/
			if (!macError) {
				/* No padding errors */
				mac = p - padLen - 1 - ssl->deMacSize;
				/* Lucky 13 step 5: Otherwise (the padding is now correctly
					formatted) run a loop as if there were 256 - padlen - 1
					bytes of padding, doing a dummy check in each iteration */
				for (rc = 256 - padLen - 1; rc > 0; rc--) {
					/* make this test look like the others */
					if ((unsigned char)rc == padLen) {
						macError = 1; /* not really an error.  reset below */
					}
				}	
				macError = 0;
			} else {
				/* Lucky 13 step 3 and 4 condition:  Then let P' denote the ﬁrst
					plen - t bytes of P, compute a MAC on SQN||HDR||P' and do a
					constant-time comparison of the computed MAC with the
					last t bytes of P. Return fatal error. */
				mac = origbuf + ssl->rec.len - ssl->deMacSize;
			}
		}
/*
		Verify the MAC of the message by calculating our own MAC of the message
		and comparing it to the one in the message.  We do this step regardless
		of whether or not we've already set macError to stop timing attacks.
		Clear the mac in the callers buffer if we're successful
*/
#ifdef USE_TLS_1_1
		if ((ssl->flags & SSL_FLAGS_TLS_1_1) && (ssl->deBlockSize > 1)) {
			ctStart += ssl->deBlockSize; /* skip explicit IV */
		}
#endif		
		if (ssl->deBlockSize > 1) {
			/* Run this helper regardless of error status thus far */
			rc = addCompressCount(ssl, padLen);
			if (macError == 0) {
			/* Lucky 13 Step 5.  Doing this extra MAC compression here rather
				than inside the real verify to keep this code patch at the
				protocol level.
			*/
					while (rc > 0) {
						sha1_compress(&dummyMd);
						rc--;
					}
			}
		}
		
		if (ssl->verifyMac(ssl, ssl->rec.type, ctStart, 
				(uint32)(mac - ctStart), mac) < 0 || macError) {
			ssl->err = SSL_ALERT_BAD_RECORD_MAC;
			psTraceInfo("Couldn't verify MAC or pad of record data\n");
			goto encodeResponse;
		}
		
		memset(mac, 0x0, ssl->deMacSize);
/*
		Record data starts at ctStart and ends at mac
*/
		p = ctStart;
		pend = mac;
		
	} else {
/*
		The record data is the entire record as there is no MAC or padding
*/
		p = ctStart;
		pend = mac = ctStart + ssl->rec.len;
	}
	
	
/*
	Check now for maximum plaintext length of 16kb. 
*/
	if (ssl->maxPtFrag == 0xFF) { /* Still negotiating size */
		if ((int32)(pend - p) > SSL_MAX_PLAINTEXT_LEN) {
			ssl->err = SSL_ALERT_RECORD_OVERFLOW;
			psTraceInfo("Record overflow\n");
			goto encodeResponse;
		}
	} else {
		if ((int32)(pend - p) > ssl->maxPtFrag) {
			ssl->err = SSL_ALERT_RECORD_OVERFLOW;
			psTraceInfo("Record overflow\n");
			goto encodeResponse;
		}
	}

/*
	Take action based on the actual record type we're dealing with
	'p' points to the start of the data, and 'pend' points to the end
*/
	switch (ssl->rec.type) {
	case SSL_RECORD_TYPE_CHANGE_CIPHER_SPEC:
		psTraceStrHs(">>> %s parsing CHANGE_CIPHER_SPEC message\n",
			(ssl->flags & SSL_FLAGS_SERVER) ? "Server" : "Client");
/*
		Body is single byte with value 1 to indicate that the next message
		will be encrypted using the negotiated cipher suite
*/
		if (pend - p < 1) {
			ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
			psTraceInfo("Invalid length for CipherSpec\n");
			goto encodeResponse;
		}
		if (*p == 1) {
			p++;
		} else {
			ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
			psTraceInfo("Invalid value for CipherSpec\n");
			goto encodeResponse;
		}
		
/*
		If we're expecting finished, then this is the right place to get
		this record.  It is really part of the handshake but it has its
		own record type.
		Activate the read cipher callbacks, so we will decrypt incoming
		data from now on.
*/
		if (ssl->hsState == SSL_HS_FINISHED) {
			sslActivateReadCipher(ssl);
		} else {
			ssl->err = SSL_ALERT_UNEXPECTED_MESSAGE;
			psTraceIntInfo("Invalid CipherSpec order: %d\n", ssl->hsState);
			goto encodeResponse;
		}
		*buf = c;
		*remaining = *len - (c - origbuf);
		return MATRIXSSL_SUCCESS;

	case SSL_RECORD_TYPE_ALERT:
/*
		Decoded an alert
		1 byte alert level (warning or fatal)
		1 byte alert description corresponding to SSL_ALERT_*
*/
		if (pend - p < 2) {
			ssl->err = SSL_ALERT_DECODE_ERROR;
			psTraceInfo("Error in length of alert record\n");
			goto encodeResponse;
		}
		*alertLevel = *p; p++;
		*alertDescription = *p; p++;
		*len =  2;
/*
		If the alert is fatal, or is a close message (usually a warning),
		flag the session with ERROR so it cannot be used anymore.
		Caller can decide whether or not to close on other warnings.
*/
		if (*alertLevel == SSL_ALERT_LEVEL_FATAL) { 
			ssl->flags |= SSL_FLAGS_ERROR;
		}
		if (*alertDescription == SSL_ALERT_CLOSE_NOTIFY) {
			ssl->flags |= SSL_FLAGS_CLOSED;
		}
		*buf = c;
		return SSL_ALERT;

	case SSL_RECORD_TYPE_HANDSHAKE:
/*
		We've got one or more handshake messages in the record data.
		The handshake parsing function will take care of all messages
		and return an error if there is any problem.
		If there is a response to be sent (either a return handshake
		or an error alert, send it).  If the message was parsed, but no
		response is needed, loop up and try to parse another message
*/
#ifdef USE_CERT_CHAIN_PARSING
		if (ssl->rec.partial) {
			if (ssl->rec.hsBytesParsed == 0) {
/*
				Account for the SSL record header for first pass
*/
				ssl->rec.hsBytesParsed = ssl->recordHeadLen;
			}
		}
#endif
		rc = parseSSLHandshake(ssl, (char*)p, (uint32)(pend - p));
		/* If the entire fragment is present, the parse has occured */
		if (ssl->fragMessage != NULL) {
			if (ssl->fragIndex == ssl->fragTotal) {
				psFree(ssl->fragMessage);
				ssl->fragMessage = NULL;
				ssl->fragIndex = ssl->fragTotal = 0;
			}
		}
		switch (rc) {
		case MATRIXSSL_SUCCESS:
			*remaining = *len - (c - origbuf);
			*buf = c;
			return MATRIXSSL_SUCCESS;


		case SSL_PROCESS_DATA:
			/*
				We're here when we've processed an SSL header that requires
				a response. In all cases (except FALSE START), we would not
				expect to have any data remaining in the incoming buffer, since
				the peer would be waiting for our response.
			 */
#ifdef ENABLE_FALSE_START
			if (c < origbuf + *len) {
				/* 
					If there's still incoming data in the buffer, it could be
					FALSE START app data immediately after the FINISHED message,
					and before we've had a chance to encode and send our 
					CHANGE_CIPHER_SPEC and FINISHED message. We hack around
					some values to support this case.
					http://tools.ietf.org/html/draft-bmoeller-tls-falsestart-00
				 */
				if (*c == SSL_RECORD_TYPE_APPLICATION_DATA && 
						ssl->hsState == SSL_HS_DONE &&
						(ssl->flags & SSL_FLAGS_SERVER)) {
					psTraceHs(">>> Server buffering FALSE START APPLICATION_DATA\n");
					ssl->flags |= SSL_FLAGS_FALSE_START;
					*remaining = *len - (c - origbuf);
					*buf = c;
				} else {
					/*
						Implies successful parse of last message in flight so
						reset the buffer to start to write response
					*/
#endif
					/* If this asserts, try defining ENABLE_FALSE_START */
					psAssert(origbuf + *len == c);
					*buf = origbuf;
#ifdef ENABLE_FALSE_START
				}
			} else {
				*buf = origbuf;
			}
#endif
			goto encodeResponse;

		case MATRIXSSL_ERROR:
		case SSL_MEM_ERROR:
			if (ssl->err == SSL_ALERT_NONE) {
				ssl->err = SSL_ALERT_INTERNAL_ERROR;
			}
			goto encodeResponse;
		}
		break;

	case SSL_RECORD_TYPE_APPLICATION_DATA:
/*
		Data is in the out buffer, let user handle it
		Don't allow application data until handshake is complete, and we are
		secure.  It is ok to let application data through on the client
		if we are in the SERVER_HELLO state because this could mean that
		the client has sent a CLIENT_HELLO message for a rehandshake
		and is awaiting reply.
*/
		if ((ssl->hsState != SSL_HS_DONE && ssl->hsState != SSL_HS_SERVER_HELLO)
				|| !(ssl->flags & SSL_FLAGS_READ_SECURE)) {
			ssl->err = SSL_ALERT_UNEXPECTED_MESSAGE;
			psTraceIntInfo("Incomplete handshake: %d\n", ssl->hsState);
			goto encodeResponse;
		}
/*
		Insitu for application data is more tricky than it is for SSL handshake
		messages.  This is because there is never going to be any 'out' data
		for handshake messages until the final record of a flight is parsed.
		Whereas application data necessarily has an 'out' for every 'in'
		record because it is the decrypted data of the 'in'.  So, the managed
		cases result anytime there is more than 1 app record in the 'in' buffer
		where the insitu must hold BOTH a decrypted buffer and the next
		encrypted record.
		
		Create so that:
		.	buf points to start of any remaining unencrypted data
		.	start is length of remaining encrypted data yet to decode
		.	len is length of unencrypted data ready for user processing

*/
		*buf = c;
		*remaining = *len - (c - origbuf);	
		*len = mac - origbuf;
/*
		SECURITY - If the mac is at the current out->end, then there is no data 
		in the record.  These records are valid, but are usually not sent by
		the application layer protocol.  Rather, they are initiated within the 
		remote SSL protocol implementation to avoid some types of attacks when
		using block ciphers.  For more information see:
		http://www.openssl.org/~bodo/tls-cbc.txt

		SECURITY - Returning blank messages has the potential
		for denial of service, because we are not changing the state of the
		system in any way when processing these messages, (although the upper
		level protocol may). To counteract this, we maintain a counter 
		that we share with other types of ignored messages. If too many in a 
		row occur, an alert will be sent and the connection closed.
		We implement this as a leaky bucket, so if a non-blank message comes
		in, the ignored message count is decremented, ensuring that we only
		error on a large number of consecutive blanks.
*/
		if (ctStart == mac) {
			if (ssl->ignoredMessageCount++ >= SSL_MAX_IGNORED_MESSAGE_COUNT) {
				ssl->err = SSL_ALERT_UNEXPECTED_MESSAGE;
				psTraceIntInfo("Exceeded limit on ignored messages: %d\n", 
							   SSL_MAX_IGNORED_MESSAGE_COUNT);
				goto encodeResponse;
			}
		} else if (ssl->ignoredMessageCount > 0) {
			ssl->ignoredMessageCount--;
		}
		
		return SSL_PROCESS_DATA;
	}
/*
	Should not get here under normal operation
*/
	psTraceIntInfo("Invalid record type in matrixSslDecode: %d\n",
		ssl->rec.type);
	*error = PS_PROTOCOL_FAIL;
	return MATRIXSSL_ERROR;

encodeResponse:
/*
	We decoded a record that needs a response, either a handshake response
	or an alert if we've detected an error.  
*/
#ifdef ENABLE_FALSE_START
	if ((ssl->flags & SSL_FLAGS_FALSE_START) && *buf != origbuf) {
		/*
			Encode the output into ssl->outbuf in this case, rather than back
			into origbuf, since there is still valid data in origbuf that 
			needs to be decoded later.
			Other places in this function we do not reference the ssl inbuf
			or outbuf directly, but this was the cleanest way for this hack.
			Caller must test to see if *buf has been modified if 
			ssl->flags & SSL_FLAGS_FALSE_START
		 */
		tmpout.buf = tmpout.start = tmpout.end = ssl->outbuf + ssl->outlen;
		tmpout.size = ssl->outsize - ssl->outlen;
		memset(origbuf, 0x0, (*buf - origbuf));	/* SECURITY (see below) */
	} else {
#endif
		psAssert(origbuf == *buf);
		tmpout.buf = tmpout.end = tmpout.start = origbuf;
		tmpout.size = size;
/*	
		SECURITY - Clear the decoded incoming record from outbuf before encoding
		the response into outbuf.
*/
		memset(tmpout.buf, 0x0, tmpout.size);
#ifdef ENABLE_FALSE_START
	}
#endif

#ifdef USE_CLIENT_SIDE_SSL	
	if (ssl->hsState == SSL_HS_HELLO_REQUEST) {
/*
		Don't clear the session info.  If receiving a HELLO_REQUEST from a 
		MatrixSSL enabled server the determination on whether to reuse the 
		session is made on that side, so always send the current session
*/
		rc = matrixSslEncodeClientHello(ssl, &tmpout, 0, requiredLen, NULL);					
	} else {
#endif /* USE_CLIENT_SIDE_SSL */
		rc = sslEncodeResponse(ssl, &tmpout, requiredLen);
#ifdef USE_CLIENT_SIDE_SSL		
	}
#endif /* USE_CLIENT_SIDE_SSL */
	*alertDescription = SSL_ALERT_NONE;
	if (rc == MATRIXSSL_SUCCESS) {
		if (ssl->err != SSL_ALERT_NONE) {
			/* We know this is always a fatal alert due to an error in
				message parsing or creation so flag this session as error */
			ssl->flags |= SSL_FLAGS_ERROR;
/*
			If tmpbuf has data, it is an alert that needs to be sent so let
			it fall through. Not sure how we would ever not have data in tmpout
*/
			if (tmpout.buf == tmpout.end) {
				psTraceInfo("Unexpected data\n");
				*error = PS_PROTOCOL_FAIL;
				return MATRIXSSL_ERROR;
			}
			*alertDescription = (unsigned char)ssl->err;
			*alertLevel = SSL_ALERT_LEVEL_FATAL;
		}
#ifdef ENABLE_FALSE_START
		if ((ssl->flags & SSL_FLAGS_FALSE_START) && *buf != origbuf) {
			/* Update outlen with the data we added */
			ssl->outlen += tmpout.end - tmpout.buf;
		} else {
#endif
			*remaining = 0;
			*len = tmpout.end - tmpout.buf;
#ifdef ENABLE_FALSE_START
		}
#endif
		return SSL_SEND_RESPONSE;
	}
	if (rc == SSL_FULL) {
#ifdef ENABLE_FALSE_START
		/* We don't support growing outbuf in the false start case */
		if (*buf != origbuf) {
			psAssert(rc != SSL_FULL);
			*error = rc;
			return MATRIXSSL_ERROR;
		}
#endif
		ssl->flags |= SSL_FLAGS_NEED_ENCODE;
		*len = 0; /* No data left to decode */
		/* requiredLen is set by sslEncode Response or ClientHello above */
		return SSL_FULL;
	}
	psAssert(rc < 0);
	*error = rc;
	return MATRIXSSL_ERROR;
}

/* Return the number of additional MAC compressions that are needed to blind
	the padding/hmac logic for thwarting Lucky 13 style attacks
*/
static int32 addCompressCount(ssl_t *ssl, int32 padLen)
{
	int32	l1, l2, c1, c2, len;
	
	c1 = c2 = 0;
	len = ssl->rec.len;
	
#ifdef USE_TLS_1_1
	if (ssl->flags & SSL_FLAGS_TLS_1_1) {
		len -= ssl->deBlockSize; /* skip explicit IV */
	}
#endif	
	l1 = 13 + len - ssl->deMacSize;
	l2 = 13 + len - padLen - 1 - ssl->deMacSize;
	
	if (ssl->deMacSize == SHA1_HASH_SIZE || ssl->deMacSize == SHA256_HASH_SIZE){
		while (l1 > 64) {
			c1++; l1 -= 64;
		} 
		if (l1 > 56) {
			c1++;
		}
		while (l2 > 64) {
			c2++; l2 -= 64;
		}
		if (l2 > 56) {
			c2++;
		}
	}

	return c1 - c2;
}

/******************************************************************************/
/*
	The workhorse for parsing handshake messages.  Also enforces the state
	machine	for proper ordering of handshake messages.
	Parameters:
	ssl - ssl context
	inbuf - buffer to read handshake message from
	len - data length for the current ssl record.  The ssl record
		can contain multiple handshake messages, so we may need to parse
		them all here.
	Return:
		MATRIXSSL_SUCCESS
		SSL_PROCESS_DATA
		MATRIXSSL_ERROR - see ssl->err for details
		MEM_FAIL 
		-MATRIXSSL_ERROR and MEM_FAIL will be caught and an alert sent.  If you
			want to specifiy the alert the set ss->err.  Otherwise it will
			be an INTERNAL_ERROR
*/
static int32 parseSSLHandshake(ssl_t *ssl, char *inbuf, uint32 len)
{
	unsigned char	*c, *end;
	unsigned char	hsType;
	int32			rc, i;
	short			renegotiationExt;
	uint32			hsLen, extLen, extType, cipher = 0;
	unsigned char	hsMsgHash[SHA384_HASH_SIZE];
	psPool_t	*pkiPool;
	void		*rsaData = NULL;

#ifdef USE_SERVER_SIDE_SSL
	unsigned char	*p;
	int32			suiteLen;
	uint32			challengeLen, pubKeyLen;
#ifdef USE_CLIENT_AUTH
	int32			certVerifyLen;
	unsigned char	certVerify[SHA384_HASH_SIZE];
#endif /* USE_CLIENT_AUTH */
#endif /* USE_SERVER_SIDE_SSL */

#ifdef USE_CLIENT_SIDE_SSL
	int32			certTypeLen;
	uint32			sessionIdLen;
	unsigned char	*extData;
#endif /* USE_CLIENT_SIDE_SSL */

#ifdef USE_CLIENT_SIDE_SSL
	int32			certChainLen, parseLen = 0;
	uint32			certLen;
	psX509Cert_t	*cert, *currentCert;
#endif /* USE_CLIENT_SIDE_SSL */



	rc = MATRIXSSL_SUCCESS;
	c = (unsigned char*)inbuf;
	end = (unsigned char*)(inbuf + len);
	
	/* Immediately check if we are working with a fragmented message. */
	if (ssl->fragMessage != NULL) {
		/* Just borrowing hsLen variable.  Is the rest here or do we still
			need more? */
		hsLen = min((uint32)(end - c), ssl->fragTotal - ssl->fragIndex);
		memcpy(ssl->fragMessage + ssl->fragIndex, c, hsLen);
		ssl->fragIndex += hsLen;
		c += hsLen;
		
		if (ssl->fragIndex == ssl->fragTotal) {
			c = ssl->fragMessage + ssl->hshakeHeadLen;
			end = ssl->fragMessage + ssl->fragTotal;
			hsLen = ssl->fragTotal - ssl->hshakeHeadLen;
			goto SKIP_HSHEADER_PARSE;
		} else {
			return MATRIXSSL_SUCCESS;
		}				
	}

	
#ifdef USE_CERT_CHAIN_PARSING
	if (ssl->rec.partial && (ssl->rec.hsBytesParsed > ssl->recordHeadLen)) {
		goto SKIP_HSHEADER_PARSE;
	}
#endif /* USE_CERT_CHAIN_PARSING */

parseHandshake:
	if (end - c < 1) {
		ssl->err = SSL_ALERT_DECODE_ERROR;
		psTraceInfo("Invalid length of handshake message\n");
		return MATRIXSSL_ERROR;
	}
	hsType = *c; c++;
	
#ifndef SSL_REHANDSHAKES_ENABLED		
/*
	If all rehandshaking is disabled, just catch that here and alert.
*/
	if (ssl->flags & SSL_FLAGS_SERVER) {
		if (hsType == SSL_HS_CLIENT_HELLO && ssl->hsState == SSL_HS_DONE) {
			psTraceInfo("Closing conn with client. Rehandshake is disabled\n");
			ssl->err = SSL_ALERT_NO_RENEGOTIATION;
			return MATRIXSSL_ERROR;
		}
	} else {
		if (hsType == SSL_HS_HELLO_REQUEST && ssl->hsState == SSL_HS_DONE) {
			psTraceInfo("Closing conn with server. Rehandshake is disabled\n");
			ssl->err = SSL_ALERT_NO_RENEGOTIATION;
			return MATRIXSSL_ERROR;
		}
	}
#endif  /* SSL_REHANDSHAKES_ENABLED */

	
/*
	hsType is the received handshake type and ssl->hsState is the expected
	handshake type.  If it doesn't match, there are some possible cases
	that are not errors.  These are checked here. 
*/
	if (hsType != ssl->hsState && 
			(hsType != SSL_HS_CLIENT_HELLO || ssl->hsState != SSL_HS_DONE)) {

/*
		A mismatch is possible in the client authentication case.
		The optional CERTIFICATE_REQUEST may be appearing instead of 
		SERVER_HELLO_DONE.
*/
		if ((hsType == SSL_HS_CERTIFICATE_REQUEST) &&
				(ssl->hsState == SSL_HS_SERVER_HELLO_DONE)) {
/*
			This is where the client is first aware of requested client
			authentication so we set the flag here.

*/
			ssl->flags |= SSL_FLAGS_CLIENT_AUTH;
			ssl->hsState = SSL_HS_CERTIFICATE_REQUEST;
			goto hsStateDetermined;		
		}
/*
		Another possible mismatch allowed is for a HELLO_REQEST message.
		Indicates a rehandshake initiated from the server.
*/
		if ((hsType == SSL_HS_HELLO_REQUEST) &&
				(ssl->hsState == SSL_HS_DONE) &&
				!(ssl->flags & SSL_FLAGS_SERVER)) {
			sslResetContext(ssl);
			ssl->hsState = hsType;
			goto hsStateDetermined;
		}
		



		ssl->err = SSL_ALERT_UNEXPECTED_MESSAGE;
		psTraceIntInfo("Out-of-order handshake message: %d\n", hsType);
		psTraceIntInfo("Wanted: %d\n", ssl->hsState);
		return MATRIXSSL_ERROR;
	}
	
hsStateDetermined:	
	if (hsType == SSL_HS_CLIENT_HELLO) { 
		sslInitHSHash(ssl);
		if (ssl->hsState == SSL_HS_DONE) {
#ifdef SSL_REHANDSHAKES_ENABLED
			/* This is a mechanism where each X bytes of data transfer gains
				you a re-handshake credit.  Prevents the DOS attack	of repeat
				re-handshake requests */
			if (ssl->rehandshakeCount == 0) {
				ssl->err = SSL_ALERT_NO_RENEGOTIATION;
				psTraceInfo("Client re-handshaking too often\n");
				return MATRIXSSL_ERROR;
			}
			ssl->rehandshakeBytes = 0; /* reset */
			ssl->rehandshakeCount--;
#endif /* SSL_REHANDSHAKES_ENABLED */
/*
			Rehandshake. Server receiving client hello on existing connection
*/
			sslResetContext(ssl);
			ssl->hsState = hsType;
		}
	}

/*
	We need to get a copy of the message hashes to compare to those sent
	in the finished message (which does not include a hash of itself)
	before we update the handshake hashes
*/
	if (ssl->hsState == SSL_HS_FINISHED) {
		sslSnapshotHSHash(ssl, hsMsgHash, 
			(ssl->flags & SSL_FLAGS_SERVER) ? 0 : SSL_FLAGS_SERVER);
	}
#ifdef USE_CLIENT_AUTH
	if (ssl->hsState == SSL_HS_CERTIFICATE_VERIFY) {
		/* Same issue as above for client auth.  Need a handshake snapshot
			that doesn't include this message we are about to process */
		sslSnapshotHSHash(ssl, hsMsgHash, -1);
	}
#endif /* USE_CLIENT_AUTH */

/*
	Process the handshake header and update the ongoing handshake hash
	SSLv3:
		1 byte type
		3 bytes length
	SSLv2:
		1 byte type
*/
	if (ssl->rec.majVer >= SSL3_MAJ_VER) {
		if (end - c < 3) {
			ssl->err = SSL_ALERT_DECODE_ERROR;
			psTraceInfo("Invalid length of handshake message\n");
			return MATRIXSSL_ERROR;
		}
		hsLen = *c << 16; c++;
		hsLen += *c << 8; c++;
		hsLen += *c; c++;
#ifdef USE_CERT_CHAIN_PARSING
		if (((uint32)(end - c) < hsLen) && !ssl->rec.partial) {
#else
		if ((uint32)(end - c) < hsLen) {
#endif
			/* Support for fragmented handshake messages - non-DTLS */
			if (ssl->fragMessage == NULL) {
				/* Initial indication there is a fragmented message */
				ssl->fragTotal = hsLen + ssl->hshakeHeadLen;
				ssl->fragMessage = psMalloc(ssl->hsPool, ssl->fragTotal);
				if (ssl->fragMessage == NULL) {
					ssl->err = SSL_ALERT_INTERNAL_ERROR;
					psTraceInfo("Memory allocation error\n");
					return MATRIXSSL_ERROR;
				}
				ssl->fragIndex = (uint32)(end - c) + ssl->hshakeHeadLen;
				memcpy(ssl->fragMessage, c - ssl->hshakeHeadLen,
					ssl->fragIndex);
				return MATRIXSSL_SUCCESS;
			} else {
				ssl->err = SSL_ALERT_DECODE_ERROR;
				psTraceInfo("Invalid handshake length\n");
				return MATRIXSSL_ERROR;
			}
		}
SKIP_HSHEADER_PARSE:

#ifdef USE_CERT_CHAIN_PARSING
			if (ssl->rec.partial) {
/*
				Length of partial certificate records are being managed
				manually with ssl->rec.len.  The first pass will need to
				include the record header in the hash.
*/
				if (ssl->rec.hsBytesHashed == 0) {
					sslUpdateHSHash(ssl, c - ssl->hshakeHeadLen, ssl->rec.len);
				} else {
					sslUpdateHSHash(ssl, c, ssl->rec.len);
				}
				ssl->rec.hsBytesHashed += ssl->rec.len;
			} else {
				sslUpdateHSHash(ssl, c - ssl->hshakeHeadLen,
					hsLen + ssl->hshakeHeadLen);
			}
#else
			sslUpdateHSHash(ssl, c - ssl->hshakeHeadLen,
				hsLen + ssl->hshakeHeadLen);

#endif

	} else if (ssl->rec.majVer == SSL2_MAJ_VER) {
/*
		Assume that the handshake len is the same as the incoming ssl record
		length minus 1 byte (type), this is verified in SSL_HS_CLIENT_HELLO
*/
		hsLen = len - 1;
		sslUpdateHSHash(ssl, (unsigned char*)inbuf, len);
	} else {
		ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
		psTraceIntInfo("Invalid record version: %d\n", ssl->rec.majVer);
		return MATRIXSSL_ERROR;
	}
/*
	Finished with header.  Process each type of handshake message.
*/
	switch(ssl->hsState) {

#ifdef USE_SERVER_SIDE_SSL
	case SSL_HS_CLIENT_HELLO:
/*
		First two bytes are the highest supported major and minor SSL versions
*/
		psTraceHs(">>> Server parsing CLIENT_HELLO\n");
		if (end - c < 2) {
			ssl->err = SSL_ALERT_DECODE_ERROR;
			psTraceInfo("Invalid ssl header version length\n");
			return MATRIXSSL_ERROR;
		}
		ssl->reqMajVer = *c; c++;
		ssl->reqMinVer = *c; c++;
/*
		Client should always be sending highest supported protocol.  Server
		will reply with a match or a lower version if enabled
*/
		if (ssl->reqMajVer >= SSL3_MAJ_VER) {
			ssl->majVer = ssl->reqMajVer;
#ifdef USE_TLS
			if (ssl->reqMinVer >= TLS_MIN_VER) {
#ifndef DISABLE_TLS_1_0			
				ssl->minVer = TLS_MIN_VER;
				ssl->flags |= SSL_FLAGS_TLS;
#endif				
#ifdef USE_TLS_1_1 /* TLS_1_1 */
				if (ssl->reqMinVer >= TLS_1_1_MIN_VER) {
#ifndef DISABLE_TLS_1_1				
					ssl->minVer = TLS_1_1_MIN_VER;
					ssl->flags |= SSL_FLAGS_TLS_1_1 | SSL_FLAGS_TLS;
#endif					
				}
#endif /* USE_TLS_1_1 */
				if (ssl->minVer == 0) {
					/* TLS versions are disabled.  Go SSLv3 if available. */
#ifdef DISABLE_SSLV3
					ssl->err = SSL_ALERT_PROTOCOL_VERSION;
					psTraceInfo("Can't support client's SSL version\n");
					return MATRIXSSL_ERROR;
#endif
					ssl->minVer = SSL3_MIN_VER;
				}
			} else if (ssl->reqMinVer == 0) {
#ifdef DISABLE_SSLV3
				ssl->err = SSL_ALERT_PROTOCOL_VERSION;
				psTraceInfo("Client wanted to talk SSLv3 but it's disabled\n");
				return MATRIXSSL_ERROR;
#else			
				ssl->minVer = SSL3_MIN_VER;
#endif /* DISABLE_SSLV3 */				
			}

#else
			ssl->minVer = SSL3_MIN_VER;

#endif /* USE_TLS */

		} else {
			ssl->err = SSL_ALERT_PROTOCOL_VERSION;
			psTraceIntInfo("Unsupported ssl version: %d\n", ssl->reqMajVer);
			return MATRIXSSL_ERROR;
		}
/*
		Support SSLv3 and SSLv2 ClientHello messages.  Browsers usually send v2
		messages for compatibility
*/
		if (ssl->rec.majVer > SSL2_MAJ_VER) {
/*
			Next is a 32 bytes of random data for key generation
			and a single byte with the session ID length
*/
			if (end - c < SSL_HS_RANDOM_SIZE + 1) {
				ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
				psTraceIntInfo("Invalid length of random data %d\n",
					(int32)(end - c));
				return MATRIXSSL_ERROR;
			}
			memcpy(ssl->sec.clientRandom, c, SSL_HS_RANDOM_SIZE);
			c += SSL_HS_RANDOM_SIZE;
			ssl->sessionIdLen = *c; c++;
/*
			If a session length was specified, the client is asking to
			resume a previously established session to speed up the handshake.
*/
			if (ssl->sessionIdLen > 0) {		
				if (ssl->sessionIdLen > SSL_MAX_SESSION_ID_SIZE || 
						end - c < ssl->sessionIdLen) {
					ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
					return MATRIXSSL_ERROR;
				}
				memcpy(ssl->sessionId, c, ssl->sessionIdLen);
				c += ssl->sessionIdLen;
/*
				Look up the session id for ssl session resumption.  If found, we
				load the pre-negotiated masterSecret and cipher.
				A resumed request must meet the following restrictions:
					The id must be present in the lookup table
					The requested version must match the original version
					The cipher suite list must contain the original cipher suite
*/
				if (matrixResumeSession(ssl) >= 0) {
					ssl->flags &= ~SSL_FLAGS_CLIENT_AUTH;
					ssl->flags |= SSL_FLAGS_RESUMED;
				} else {
					ssl->flags &= ~SSL_FLAGS_RESUMED;
					memset(ssl->sessionId, 0, SSL_MAX_SESSION_ID_SIZE);
					ssl->sessionIdLen = 0;
				}
			} else {
/*
				Always clear the RESUMED flag if no client session id specified
*/
				ssl->flags &= ~SSL_FLAGS_RESUMED;
			}
/*
			Next is the two byte cipher suite list length, network byte order.  
			It must not be zero, and must be a multiple of two.
*/
			if (end - c < 2) {
				ssl->err = SSL_ALERT_DECODE_ERROR;
				psTraceInfo("Invalid cipher suite list length\n");
				return MATRIXSSL_ERROR;
			}
			suiteLen = *c << 8; c++;
			suiteLen += *c; c++;
			if (suiteLen == 0 || suiteLen & 1) {
				ssl->err = SSL_ALERT_DECODE_ERROR;
				psTraceIntInfo("Unable to parse cipher suite list: %d\n",
					suiteLen);
				return MATRIXSSL_ERROR;
			}
/*
			Now is 'suiteLen' bytes of the supported cipher suite list,
			listed in order of preference.  Loop through and find the 
			first cipher suite we support.
*/
			if (end - c < suiteLen) {
				ssl->err = SSL_ALERT_DECODE_ERROR;
				psTraceInfo("Malformed clientHello message\n");
				return MATRIXSSL_ERROR;
			}
#ifdef ENABLE_SECURE_REHANDSHAKES
/*
			Below we stop looking after the first cipher we match but we need
			to search for SCSV if secure rehandshakes are on and first conn 
*/
			if (ssl->myVerifyDataLen == 0) {
				extLen = 0; /* just overloading existing var */
				while (extLen < (uint32)suiteLen) {
					cipher = c[extLen] << 8; extLen++;
					cipher += c[extLen]; extLen++;
					if (cipher == TLS_EMPTY_RENEGOTIATION_INFO_SCSV) {
						ssl->secureRenegotiationFlag = PS_TRUE;
					}
				}
			}
#endif						
			p = c + suiteLen;
			while (c < p) {
				cipher = *c << 8; c++;
				cipher += *c; c++;	
/*
				A resumed session can only match the cipher originally 
				negotiated. Otherwise, match the first cipher that we support
*/
				if (ssl->flags & SSL_FLAGS_RESUMED) {
					psAssert(ssl->cipher != NULL);
					if (ssl->cipher->ident == cipher) {
						c = p;
						break;
					}
				} else {
					if ((ssl->cipher = sslGetCipherSpec(ssl, cipher)) != NULL) {
						c = p;
						break;
					}
				}
			}
/*
			If we fell to the default cipher suite, we didn't have
			any in common with the client, or the client is being bad
			and requesting the null cipher!
*/
			if (ssl->cipher == NULL || ssl->cipher->ident != cipher || 
					cipher == SSL_NULL_WITH_NULL_NULL) {
				psTraceInfo("Can't support requested cipher\n");
				ssl->cipher = sslGetCipherSpec(ssl, SSL_NULL_WITH_NULL_NULL);
				ssl->err = SSL_ALERT_HANDSHAKE_FAILURE;
				return MATRIXSSL_ERROR;
			}

/*
			Compression parameters
*/
			if (end - c < 1) {
				ssl->err = SSL_ALERT_DECODE_ERROR;
				psTraceInfo("Invalid compression header length\n");
				return MATRIXSSL_ERROR;
			}
			extLen = *c++;
			if ((uint32)(end - c) < extLen) {
				ssl->err = SSL_ALERT_DECODE_ERROR;
				psTraceInfo("Invalid compression header length\n");
				return MATRIXSSL_ERROR;
			}
			c += extLen;

/*
			There could be extension data to parse here:
			Two byte length and extension info.
			http://www.faqs.org/rfcs/rfc3546.html

			NOTE:  This c != end test is only safe because ClientHello is the
			only record/message in the flight of supported handshake protocols.
*/
			renegotiationExt = 0;
			if (c != end) {
				if (end - c < 2) {
					ssl->err = SSL_ALERT_DECODE_ERROR;
					psTraceInfo("Invalid extension header len\n");
					return MATRIXSSL_ERROR;
				}
				extLen = *c << 8; c++; /* Total length of list */
				extLen += *c; c++;
				if ((uint32)(end - c) < extLen) {
					ssl->err = SSL_ALERT_DECODE_ERROR;
					psTraceInfo("Invalid extension header len\n");
					return MATRIXSSL_ERROR;
				}	
				while (c != end) { 
					extType = *c << 8; c++; /* Individual hello ext */
					extType += *c; c++;
					if (end - c < 2) {
						ssl->err = SSL_ALERT_DECODE_ERROR;
						psTraceInfo("Invalid extension header len\n");
						return MATRIXSSL_ERROR;
					}
					extLen = *c << 8; c++; /* length of one extension */
					extLen += *c; c++;
					if ((uint32)(end - c) < extLen) {
						ssl->err = SSL_ALERT_DECODE_ERROR;
						psTraceInfo("Invalid extension header len\n");
						return MATRIXSSL_ERROR;
					}
#ifdef ENABLE_SECURE_REHANDSHAKES					
/*
					Handle incoming client extensions we support.
*/
					if (extType == EXT_RENEGOTIATION_INFO) {
						renegotiationExt = 1;
						if (ssl->secureRenegotiationFlag == PS_FALSE &&
								ssl->myVerifyDataLen == 0) {
							if (extLen == 1 && *c == '\0') {
								ssl->secureRenegotiationFlag = PS_TRUE;
							} else {
								ssl->err = SSL_ALERT_HANDSHAKE_FAILURE;
								psTraceInfo("Cln sent bad renegotiationInfo\n");
								return MATRIXSSL_ERROR;
							}						
						} else if ((extLen == ssl->peerVerifyDataLen + 1) &&
								(ssl->secureRenegotiationFlag == PS_TRUE)) {
							c++; extLen--; /* decr extLen when incr c */
							if (memcmp(c, ssl->peerVerifyData,
									ssl->peerVerifyDataLen) != 0) {
								psTraceInfo("Cli verify renegotiation fail\n");
								ssl->err = SSL_ALERT_HANDSHAKE_FAILURE;
								return MATRIXSSL_ERROR;
							}
						} else {
							psTraceInfo("Bad state/len of renegotiation ext\n");
							ssl->err = SSL_ALERT_HANDSHAKE_FAILURE;
							return MATRIXSSL_ERROR;
						}
					}
#endif /* ENABLE_SECURE_REHANDSHAKES */



					if (extType == EXT_MAX_FRAGMENT_LEN) {
						if (*c == 0x1) {
							ssl->maxPtFrag = 0x200;
						} else if (*c == 0x2) {
							ssl->maxPtFrag = 0x400;
						} else if (*c == 0x3) {
							ssl->maxPtFrag = 0x800;
						} else if (*c == 0x4) {
							ssl->maxPtFrag = 0x1000;
						} else {
							psTraceInfo("Client sent bad frag len ext value\n");
							ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
							return MATRIXSSL_ERROR;
						}
						c++; extLen--;
					}
					c += extLen;
				}				
			}
/*
			Handle the extensions that were missing or not what we wanted
*/


#ifdef ENABLE_SECURE_REHANDSHAKES
			if (renegotiationExt == 0) {
#ifdef REQUIRE_SECURE_REHANDSHAKES
/*
				Check if SCSV was sent instead
*/
				if (ssl->secureRenegotiationFlag == PS_FALSE &&
						ssl->myVerifyDataLen == 0) {
					psTraceInfo("Client doesn't support renegotiation hello\n");
					ssl->err = SSL_ALERT_HANDSHAKE_FAILURE;
					return MATRIXSSL_ERROR;	
				}
#endif /* REQUIRE_SECURE_REHANDSHAKES */
				if (ssl->secureRenegotiationFlag == PS_TRUE &&
						ssl->myVerifyDataLen > 0) {
					ssl->err = SSL_ALERT_HANDSHAKE_FAILURE;
					psTraceInfo("Cln missing renegotiationInfo on re-hndshk\n");
					return MATRIXSSL_ERROR;
				}
#ifndef ENABLE_INSECURE_REHANDSHAKES
				if (ssl->secureRenegotiationFlag == PS_FALSE &&
						ssl->myVerifyDataLen > 0) {
					ssl->err = SSL_ALERT_HANDSHAKE_FAILURE;
					psTraceInfo("Cln attempting insecure handshake\n");
					return MATRIXSSL_ERROR;
				}
#endif /* !ENABLE_INSECURE_REHANDSHAKES */				
			}
#endif /* ENABLE_SECURE_REHANDSHAKES */

		} else {
/*
			Parse a SSLv2 ClientHello message.  The same information is 
			conveyed but the order and format is different.
			First get the cipher suite length, session id length and challenge
			(client random) length - all two byte values, network byte order.
*/
			if (end - c < 6) {
				ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
				psTraceInfo("Can't parse hello message\n");
				return MATRIXSSL_ERROR;
			}
			suiteLen = *c << 8; c++;
			suiteLen += *c; c++;
			if (suiteLen == 0 || suiteLen % 3 != 0) {
				ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
				psTraceInfo("Can't parse hello message\n");
				return MATRIXSSL_ERROR;
			}
			ssl->sessionIdLen = *c << 8; c++;
			ssl->sessionIdLen += *c; c++;
/*
			A resumed session would use a SSLv3 ClientHello, not SSLv2.
*/
			if (ssl->sessionIdLen != 0) {
				ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
				psTraceInfo("Bad resumption request\n");
				return MATRIXSSL_ERROR;
			}
			challengeLen = *c << 8; c++;
			challengeLen += *c; c++;
			if (challengeLen < 16 || challengeLen > 32) {
				psTraceInfo("Bad challenge length\n");
				ssl->err = SSL_ALERT_DECODE_ERROR;
				return MATRIXSSL_ERROR;
			}
/*
			Validate the three lengths that were just sent to us, don't
			want any buffer overflows while parsing the remaining data
*/
			if ((uint32)(end - c) != suiteLen + ssl->sessionIdLen +
					challengeLen) {
				ssl->err = SSL_ALERT_DECODE_ERROR;
				psTraceInfo("Malformed SSLv2 clientHello\n");
				return MATRIXSSL_ERROR;
			}
/*
			Parse the cipher suite list similar to the SSLv3 method, except
			each suite is 3 bytes, instead of two bytes.  We define the suite
			as an integer value, so either method works for lookup.
			We don't support session resumption from V2 handshakes, so don't 
			need to worry about matching resumed cipher suite.
*/
			p = c + suiteLen;
			while (c < p) {
				cipher = *c << 16; c++;
				cipher += *c << 8; c++;
				cipher += *c; c++;
				if ((ssl->cipher = sslGetCipherSpec(ssl, cipher)) != NULL) {
					c = p;
					break;
				}
			}
			if (ssl->cipher == NULL || 
					ssl->cipher->ident == SSL_NULL_WITH_NULL_NULL) {
				ssl->cipher = sslGetCipherSpec(ssl, SSL_NULL_WITH_NULL_NULL);
				ssl->err = SSL_ALERT_HANDSHAKE_FAILURE;
				psTraceInfo("No matching cipher for SSL handshake\n");
				return MATRIXSSL_ERROR;
			}
/*
			We don't allow session IDs for v2 ClientHellos
*/
			if (ssl->sessionIdLen > 0) {
				ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
				psTraceInfo("SSLv2 sessions not allowed\n");
				return MATRIXSSL_ERROR;
			}
/*
			The client random (between 16 and 32 bytes) fills the least 
			significant bytes in the (always) 32 byte SSLv3 client random field.
*/
			memset(ssl->sec.clientRandom, 0x0, SSL_HS_RANDOM_SIZE);
			memcpy(ssl->sec.clientRandom + (SSL_HS_RANDOM_SIZE - challengeLen), 
				c, challengeLen);
			c += challengeLen;
		}
/*
		ClientHello should be the only one in the record.
*/
		if (c != end) {
			ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
			psTraceInfo("Invalid final client hello length\n");
			return MATRIXSSL_ERROR;
		}

/*
		If we're resuming a handshake, then the next handshake message we
		expect is the finished message.  Otherwise we do the full handshake.
*/
		if (ssl->flags & SSL_FLAGS_RESUMED) {
			ssl->hsState = SSL_HS_FINISHED;
		} else {
		
			ssl->hsState = SSL_HS_CLIENT_KEY_EXCHANGE;
#ifdef USE_CLIENT_AUTH
/*
			Next state in client authentication case is to receive the cert
*/
			if (ssl->flags & SSL_FLAGS_CLIENT_AUTH) {
					ssl->hsState = SSL_HS_CERTIFICATE;
			}
#endif /* USE_CLIENT_AUTH */
		}
/*
		Now that we've parsed the ClientHello, we need to tell the caller that
		we have a handshake response to write out.
		The caller should call sslWrite upon receiving this return code.
*/
		rc = SSL_PROCESS_DATA;
		break;

	case SSL_HS_CLIENT_KEY_EXCHANGE:
/*
		RSA: This message contains the premaster secret encrypted with the 
		server's public key (from the Certificate).  The premaster
		secret is 48 bytes of random data, but the message may be longer
		than that because the 48 bytes are padded before encryption 
		according to PKCS#1v1.5.  After encryption, we should have the 
		correct length.
*/
		psTraceHs(">>> Server parsing CLIENT_KEY_EXCHANGE\n");
		if ((uint32)(end - c) < hsLen) {
			ssl->err = SSL_ALERT_DECODE_ERROR;
			psTraceInfo("Invalid ClientKeyExchange length\n");
			return MATRIXSSL_ERROR;
		}

		pubKeyLen = hsLen;
#ifdef USE_TLS
/*
		TLS - Two byte length is explicit.
*/
		if (ssl->majVer >= TLS_MAJ_VER && ssl->minVer >= TLS_MIN_VER) {
			if (end - c < 2) {
				ssl->err = SSL_ALERT_DECODE_ERROR;
				psTraceInfo("Invalid ClientKeyExchange length\n");
				return MATRIXSSL_ERROR;
			}
			pubKeyLen = *c << 8; c++;
			pubKeyLen += *c; c++;
			if ((uint32)(end - c) < pubKeyLen) {
				ssl->err = SSL_ALERT_DECODE_ERROR;
				psTraceInfo("Invalid ClientKeyExchange length\n");
				return MATRIXSSL_ERROR;
			}
		}
#endif /* USE_TLS */



				/*	Standard RSA suite. Now have a handshake pool to allocate
					the premaster storage */
				ssl->sec.premasterSize = SSL_HS_RSA_PREMASTER_SIZE;
				ssl->sec.premaster = psMalloc(ssl->hsPool,
					SSL_HS_RSA_PREMASTER_SIZE);
				if (ssl->sec.premaster == NULL) {
					return SSL_MEM_ERROR;
				}
				pkiPool = NULL;


				if ((extLen = csRsaDecryptPriv(pkiPool, ssl->keys->privKey, c,
						pubKeyLen, ssl->sec.premaster, ssl->sec.premasterSize,
						rsaData)) != ssl->sec.premasterSize) {
					ssl->err = SSL_ALERT_DECRYPT_ERROR;
					return MATRIXSSL_ERROR;
				}
/*
				The first two bytes of the decrypted message should be the
				client's requested version number (which may not be the same
				as the final negotiated version). The other 46 bytes -
				pure random!
			
				SECURITY - 
				Many SSL clients (Including Microsoft IE 6.0) incorrectly set
				the first two bytes to the negotiated version rather than the
				requested version.  This is known in OpenSSL as the
				SSL_OP_TLS_ROLLBACK_BUG. We allow this to slide only if we
				don't support TLS, TLS was requested, and the negotiated
				versions match.
*/
				if (*ssl->sec.premaster != ssl->reqMajVer) {
					ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
					psTraceInfo("Incorrect version in ClientKeyExchange\n");
					return MATRIXSSL_ERROR;
				}
				if (*(ssl->sec.premaster + 1) != ssl->reqMinVer) {
#ifndef USE_TLS
					if (ssl->reqMinVer < TLS_MIN_VER ||
							*(ssl->sec.premaster + 1) != ssl->minVer) {
#endif /* USE_TLS */
						ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
						psTraceInfo("Incorrect version in ClientKeyExchange\n");
						return MATRIXSSL_ERROR;
#ifndef USE_TLS
					}
#endif /* USE_TLS */
				}

/*
		Now that we've got the premaster secret, derive the various
		symmetric keys using it and the client and server random values.
		Update the cached session (if found) with the masterSecret and
		negotiated cipher.	
*/
		if (sslCreateKeys(ssl) < 0) {
			ssl->err = SSL_ALERT_INTERNAL_ERROR;
			return MATRIXSSL_ERROR;
		}
		matrixUpdateSession(ssl);

		c += pubKeyLen;
		ssl->hsState = SSL_HS_FINISHED;


#ifdef USE_CLIENT_AUTH
/*
		In the non client auth case, we are done with the handshake pool
*/
		if (!(ssl->flags & SSL_FLAGS_CLIENT_AUTH)) {
			ssl->hsPool = NULL;
		}
#else
		ssl->hsPool = NULL;
#endif


#ifdef USE_CLIENT_AUTH
/*
		Tweak the state here for client authentication case
*/
		if (ssl->flags & SSL_FLAGS_CLIENT_AUTH) {	
			ssl->hsState = SSL_HS_CERTIFICATE_VERIFY;		
		}
#endif /* USE_CLIENT_AUTH */
		break;
#endif /* USE_SERVER_SIDE_SSL */

	case SSL_HS_FINISHED:
/*
		Before the finished handshake message, we should have seen the
		CHANGE_CIPHER_SPEC message come through in the record layer, which
		would have activated the read cipher, and set the READ_SECURE flag.
		This is the first handshake message that was sent securely.
*/
		psTraceStrHs(">>> %s parsing FINISHED message\n",
			(ssl->flags & SSL_FLAGS_SERVER) ? "Server" : "Client");
		if (!(ssl->flags & SSL_FLAGS_READ_SECURE)) {
			ssl->err = SSL_ALERT_UNEXPECTED_MESSAGE;
			psTraceInfo("Finished before ChangeCipherSpec\n");
			return MATRIXSSL_ERROR;
		}
/*
		The contents of the finished message is a 16 byte MD5 hash followed
		by a 20 byte sha1 hash of all the handshake messages so far, to verify
		that nothing has been tampered with while we were still insecure.
		Compare the message to the value we calculated at the beginning of
		this function.
*/
#ifdef USE_TLS
		if (ssl->flags & SSL_FLAGS_TLS) {
			if (hsLen != TLS_HS_FINISHED_SIZE) {
				ssl->err = SSL_ALERT_DECODE_ERROR;
				psTraceInfo("Invalid Finished length\n");
				return MATRIXSSL_ERROR;
			}
		} else {
#endif /* USE_TLS */
			if (hsLen != MD5_HASH_SIZE + SHA1_HASH_SIZE) {
				ssl->err = SSL_ALERT_DECODE_ERROR;
				psTraceInfo("Invalid Finished length\n");
				return MATRIXSSL_ERROR;
			}
#ifdef USE_TLS
		}
#endif /* USE_TLS */
		if ((uint32)(end - c) < hsLen) {
			ssl->err = SSL_ALERT_DECODE_ERROR;
			psTraceInfo("Invalid Finished length\n");
			return MATRIXSSL_ERROR;
		}
		if (memcmp(c, hsMsgHash, hsLen) != 0) {
			ssl->err = SSL_ALERT_DECRYPT_ERROR;
			psTraceInfo("Invalid handshake msg hash\n");
			return MATRIXSSL_ERROR;
		}
#ifdef ENABLE_SECURE_REHANDSHAKES		
/*
		Got the peer verify_data for secure renegotiations
*/
		memcpy(ssl->peerVerifyData, c, hsLen);
		ssl->peerVerifyDataLen = hsLen;
#endif /* ENABLE_SECURE_REHANDSHAKES */		
		c += hsLen;
		ssl->hsState = SSL_HS_DONE;
/*
		Now that we've parsed the Finished message, if we're a resumed 
		connection, we're done with handshaking, otherwise, we return
		SSL_PROCESS_DATA to get our own cipher spec and finished messages
		sent out by the caller.
*/
		if (ssl->flags & SSL_FLAGS_SERVER) {
			if (!(ssl->flags & SSL_FLAGS_RESUMED)) {
				rc = SSL_PROCESS_DATA;
			} else {
#ifdef USE_SSL_INFORMATIONAL_TRACE
				/* Server side resumed completion */
				matrixSslPrintHSDetails(ssl);
#endif
			}
		} else {
			if (ssl->flags & SSL_FLAGS_RESUMED) {
				rc = SSL_PROCESS_DATA;
			} else {
#ifdef USE_SSL_INFORMATIONAL_TRACE
				/* Client side standard completion */
				matrixSslPrintHSDetails(ssl);
#endif			
			}
		}
#ifdef USE_CLIENT_SIDE_SSL
/*
		There is also an attempt to free the cert during
		the sending of the finished message to deal with client
		and server and differing handshake types.  Both cases are 
		attempted keep the lifespan of this allocation as short as possible.
*/
		if (ssl->sec.cert) {
			psX509FreeCert(ssl->sec.cert);
			ssl->sec.cert = NULL;
		}
#endif /* USE_CLIENT_SIDE_SSL */

		break;

#ifdef USE_CLIENT_SIDE_SSL
	case SSL_HS_HELLO_REQUEST:
/*	
		No body message and the only one in record flight
*/
		psTraceHs(">>> Client parsing HELLO_REQUEST message\n");
		if (end - c != 0) {
			ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
			psTraceInfo("Invalid hello request message\n");
			return MATRIXSSL_ERROR;
		}
/*
		Intentionally not changing state here to SERVER_HELLO.  The
		encodeResponse case	this will fall into needs to distinguish
		between calling the normal sslEncodeResponse or encodeClientHello.
		The HELLO_REQUEST state is used to make that determination and the
		writing of CLIENT_HELLO will properly move the state along itself.
*/
		rc = SSL_PROCESS_DATA;
		break;

	case SSL_HS_SERVER_HELLO: 
	
		psTraceHs(">>> Client parsing SERVER_HELLO message\n");
/*
		Need to track hsLen because there is no explict	way to tell if
		hello extensions are appended so it isn't clear if the record data
		after the compression parameters are a new message or extension data
*/
		extData = c;

		
/*
		First two bytes are the negotiated SSL version
		We support only 3.0 (other options are 2.0 or 3.1)
*/
		if (end - c < 2) {
			ssl->err = SSL_ALERT_DECODE_ERROR;
			psTraceInfo("Invalid ssl header version length\n");
			return MATRIXSSL_ERROR;
		}
		ssl->reqMajVer = *c; c++;
		ssl->reqMinVer = *c; c++;
		if (ssl->reqMajVer != ssl->majVer) {
			ssl->err = SSL_ALERT_PROTOCOL_VERSION;
			psTraceIntInfo("Unsupported ssl version: %d\n", ssl->reqMajVer);
			return MATRIXSSL_ERROR;
		}

#ifdef USE_TLS
/*
		See if the protocol is being downgraded
*/
		if (ssl->reqMinVer != ssl->minVer) {
			if (ssl->reqMinVer == SSL3_MIN_VER && ssl->minVer >= TLS_MIN_VER) {
#ifdef DISABLE_SSLV3
				ssl->err = SSL_ALERT_PROTOCOL_VERSION;
				psTraceInfo("Server wants to talk SSLv3 but it's disabled\n");
				return MATRIXSSL_ERROR;
#else					
/*
				Server minVer now becomes OUR initial requested version.
				This is used during the creation of the premaster where
				this initial requested version is part of the calculation.
				The RFC actually says to use the original requested version
				but no implemenations seem to follow that and just use the
				agreed upon one.
*/
				ssl->reqMinVer = ssl->minVer;
				ssl->minVer = SSL3_MIN_VER;
				ssl->flags &= ~SSL_FLAGS_TLS;
#ifdef USE_TLS_1_1
				ssl->flags &= ~SSL_FLAGS_TLS_1_1;
#endif /* USE_TLS_1_1 */
#endif /* DISABLE_SSLV3 */	
			} else {
#ifdef USE_TLS_1_1
				if (ssl->reqMinVer == TLS_MIN_VER &&
                        ssl->minVer <= TLS_1_2_MIN_VER) {
#ifdef DISABLE_TLS_1_0
					ssl->err = SSL_ALERT_PROTOCOL_VERSION;
					psTraceInfo("Server wants to talk TLS1.0 but it's disabled\n");
					return MATRIXSSL_ERROR;
#endif											
					ssl->reqMinVer = ssl->minVer;
					ssl->minVer = TLS_MIN_VER;
					ssl->flags &= ~SSL_FLAGS_TLS_1_1;
				} else {
#endif/* USE_TLS_1_1 */
					/* Wasn't able to settle on a common protocol */
					ssl->err = SSL_ALERT_PROTOCOL_VERSION;
					psTraceIntInfo("Unsupported ssl version: %d\n",
						ssl->reqMajVer);
					return MATRIXSSL_ERROR;
#ifdef USE_TLS_1_1
				}
#endif /* USE_TLS_1_1 */		
			}
		}
#endif /* USE_TLS */

/*
		Next is a 32 bytes of random data for key generation
		and a single byte with the session ID length
*/
		if (end - c < SSL_HS_RANDOM_SIZE + 1) {
			ssl->err = SSL_ALERT_DECODE_ERROR;
			psTraceInfo("Invalid length of random data\n");
			return MATRIXSSL_ERROR;
		}
		memcpy(ssl->sec.serverRandom, c, SSL_HS_RANDOM_SIZE);
		c += SSL_HS_RANDOM_SIZE;
		sessionIdLen = *c; c++;
		if (sessionIdLen > SSL_MAX_SESSION_ID_SIZE || 
				(uint32)(end - c) < sessionIdLen) {
			ssl->err = SSL_ALERT_DECODE_ERROR;
			return MATRIXSSL_ERROR;
		}
/*
		If a session length was specified, the server has sent us a
		session Id.  We may have requested a specific session, and the
		server may or may not agree to use that session.
*/
		if (sessionIdLen > 0) {
			if (ssl->sessionIdLen > 0) {
				if (memcmp(ssl->sessionId, c, sessionIdLen) == 0) {
					ssl->flags |= SSL_FLAGS_RESUMED;
				} else {
					ssl->cipher = sslGetCipherSpec(ssl,SSL_NULL_WITH_NULL_NULL);
					memset(ssl->sec.masterSecret, 0x0, SSL_HS_MASTER_SIZE);
					ssl->sessionIdLen = (unsigned char)sessionIdLen;
					memcpy(ssl->sessionId, c, sessionIdLen);
					ssl->flags &= ~SSL_FLAGS_RESUMED;
				}
			} else {
				ssl->sessionIdLen = (unsigned char)sessionIdLen;
				memcpy(ssl->sessionId, c, sessionIdLen);
			}
			c += sessionIdLen;
		} else {
			if (ssl->sessionIdLen > 0) {
				ssl->cipher = sslGetCipherSpec(ssl, SSL_NULL_WITH_NULL_NULL);
				memset(ssl->sec.masterSecret, 0x0, SSL_HS_MASTER_SIZE);
				ssl->sessionIdLen = 0;
				memset(ssl->sessionId, 0x0, SSL_MAX_SESSION_ID_SIZE);
				ssl->flags &= ~SSL_FLAGS_RESUMED;
			}
		}
/*
		Next is the two byte cipher suite
*/
		if (end - c < 2) {
			ssl->err = SSL_ALERT_DECODE_ERROR;
			psTraceInfo("Invalid cipher suite length\n");
			return MATRIXSSL_ERROR;
		}
		cipher = *c << 8; c++;
		cipher += *c; c++;

/*
		A resumed session can only match the cipher originally 
		negotiated. Otherwise, match the first cipher that we support
*/
		if (ssl->flags & SSL_FLAGS_RESUMED) {
			psAssert(ssl->cipher != NULL);
			if (ssl->cipher->ident != cipher) {
				ssl->err = SSL_ALERT_HANDSHAKE_FAILURE;
				psTraceInfo("Can't support resumed cipher\n");
				return MATRIXSSL_ERROR;
			}
		} else {
			if ((ssl->cipher = sslGetCipherSpec(ssl, cipher)) == NULL) {
				ssl->err = SSL_ALERT_HANDSHAKE_FAILURE;
				psTraceInfo("Can't support requested cipher\n");
				return MATRIXSSL_ERROR;
			}
		}

/*
		Decode the compression parameters.
*/
		if (end - c < 1) {
			ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
			psTraceInfo("Expected compression value\n");
			return MATRIXSSL_ERROR;
		}
/*
		At this point, if we're resumed, we have all the required info
		to derive keys.  The next handshake message we expect is
		the Finished message.
*/
		c++;
	
/*
		If our sent ClientHello had an extension there could be extension data
		to parse here:  http://www.faqs.org/rfcs/rfc3546.html
		
		The explict test on hsLen is necessary for TLS 1.0 and 1.1 because
		there is no good way to tell if the remaining record data is the
		next handshake message or if it is extension data
*/
		renegotiationExt = 0;
		if (c != end && ((int32)hsLen > (c - extData))) {
			if (end - c < 2) {
				ssl->err = SSL_ALERT_DECODE_ERROR;
				psTraceInfo("Invalid extension header len\n");
				return MATRIXSSL_ERROR;
			}
			extLen = *c << 8; c++; /* Total length of list */
			extLen += *c; c++;
			if ((uint32)(end - c) < extLen) {
				ssl->err = SSL_ALERT_DECODE_ERROR;
				psTraceInfo("Invalid extension header len\n");
				return MATRIXSSL_ERROR;
			}	
			while ((int32)hsLen > (c - extData)) {
				extType = *c << 8; c++; /* Individual hello ext */
				extType += *c; c++;
				if (end - c < 2) {
					ssl->err = SSL_ALERT_DECODE_ERROR;
					psTraceInfo("Invalid extension header len\n");
					return MATRIXSSL_ERROR;
				}
				extLen = *c << 8; c++; /* length of one extension */
				extLen += *c; c++;
				if ((uint32)(end - c) < extLen) {
					ssl->err = SSL_ALERT_DECODE_ERROR;
					psTraceInfo("Invalid extension header len\n");
					return MATRIXSSL_ERROR;
				}
/*
				Deal with the server hello extensions we support
*/
				if (extType == EXT_RENEGOTIATION_INFO) {
#ifdef ENABLE_SECURE_REHANDSHAKES				
					renegotiationExt = 1;	
					if (ssl->secureRenegotiationFlag == PS_FALSE &&
							ssl->myVerifyDataLen == 0) {
						if (extLen == 1 && *c == '\0') {
							ssl->secureRenegotiationFlag = PS_TRUE;
						} else {
							ssl->err = SSL_ALERT_HANDSHAKE_FAILURE;
							psTraceInfo("Server sent bad renegotiationInfo\n");
							return MATRIXSSL_ERROR;
						}
					} else if (ssl->secureRenegotiationFlag == PS_TRUE &&
							extLen == ((ssl->myVerifyDataLen * 2) + 1)) {
						c++; extLen--;
						if (memcmp(c, ssl->myVerifyData,
								ssl->myVerifyDataLen) != 0) {
							ssl->err = SSL_ALERT_HANDSHAKE_FAILURE;
							psTraceInfo("Srv had bad my renegotiationInfo\n");
							return MATRIXSSL_ERROR;
						}
						if (memcmp(c + ssl->myVerifyDataLen,ssl->peerVerifyData,
								ssl->peerVerifyDataLen) != 0) {
							ssl->err = SSL_ALERT_HANDSHAKE_FAILURE;
							psTraceInfo("Srv had bad peer renegotiationInfo\n");
							return MATRIXSSL_ERROR;
						}
					} else {
						ssl->err = SSL_ALERT_HANDSHAKE_FAILURE;
						psTraceInfo("Server sent bad renegotiationInfo\n");
						return MATRIXSSL_ERROR;
					}
#endif /* ENABLE_SECURE_REHANDSHAKES */	
				} else if (extType == EXT_MAX_FRAGMENT_LEN) {
					if (ssl->maxPtFrag != 0xFF) {
						ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
						psTraceInfo("Server sent unexpected MAX_FRAG ext\n");
						return MATRIXSSL_ERROR;
					}
					if (*c == 0x01 &&
							(REQUESTED_MAX_PLAINTEXT_RECORD_LEN == 0x200)) {
						ssl->maxPtFrag = 0x200;
					} else if (*c == 0x02 && 
							(REQUESTED_MAX_PLAINTEXT_RECORD_LEN == 0x400)) {
						ssl->maxPtFrag = 0x400;
					} else if (*c == 0x03 && 
							(REQUESTED_MAX_PLAINTEXT_RECORD_LEN == 0x800)) {
						ssl->maxPtFrag = 0x800;
					} else if (*c == 0x04 && 
							(REQUESTED_MAX_PLAINTEXT_RECORD_LEN == 0x1000)) {
						ssl->maxPtFrag = 0x1000;
					} else {
						ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
						psTraceInfo("Server sent mismatched MAX_FRAG ext\n");
						return MATRIXSSL_ERROR;
					}
					c++; extLen--;
				} else {
					if (ssl->extCb) {
						if ((*ssl->extCb)(ssl, (unsigned short)extType,
								(unsigned short)extLen, c) < 0) {
							ssl->err = SSL_ALERT_UNSUPPORTED_EXTENSION;
							psTraceInfo("User didn't like extension\n");
							return MATRIXSSL_ERROR;
						}
					}
				}	
				c += extLen;
			}
		}
		if (ssl->maxPtFrag == 0xFF) {
			/* Server didn't respond to our MAX_FRAG request. Reset default */
			psTraceInfo("Server ignored max fragment length ext request\n");
			ssl->maxPtFrag = SSL_MAX_PLAINTEXT_LEN;
		}
		
		
		
#ifdef ENABLE_SECURE_REHANDSHAKES
		if (renegotiationExt == 0) {
#ifdef REQUIRE_SECURE_REHANDSHAKES		
			ssl->err = SSL_ALERT_HANDSHAKE_FAILURE;
			psTraceInfo("Srv doesn't support renegotiationInfo\n");
			return MATRIXSSL_ERROR;
#else
			if (ssl->secureRenegotiationFlag == PS_TRUE) {
				ssl->err = SSL_ALERT_HANDSHAKE_FAILURE;
				psTraceInfo("Srv didn't send renegotiationInfo on re-hndshk\n");
				return MATRIXSSL_ERROR;
			}
#ifndef ENABLE_INSECURE_REHANDSHAKES
/*
			This case can only be hit if ENABLE_SECURE is on because otherwise
			we wouldn't even have got this far because both would be off.
*/		
			if (ssl->secureRenegotiationFlag == PS_FALSE &&
					ssl->myVerifyDataLen > 0) {
				ssl->err = SSL_ALERT_HANDSHAKE_FAILURE;
				psTraceInfo("Srv attempting insecure renegotiation\n");
				return MATRIXSSL_ERROR;
			}
#endif /* !ENABLE_SECURE_REHANDSHAKES */				
#endif /* REQUIRE_SECURE_REHANDSHAKES */	
		}
#endif /* ENABLE_SECURE_REHANDSHAKES */		
		
						
		if (ssl->flags & SSL_FLAGS_RESUMED) {
			if (sslCreateKeys(ssl) < 0) {
				ssl->err = SSL_ALERT_INTERNAL_ERROR;
				return MATRIXSSL_ERROR;
			}
			ssl->hsState = SSL_HS_FINISHED;
		} else {
			ssl->hsState = SSL_HS_CERTIFICATE;
		}
		break;
#endif /* USE_CLIENT_SIDE_SSL */

#ifdef USE_CLIENT_SIDE_SSL
	case SSL_HS_CERTIFICATE: 
		psTraceStrHs(">>> %s parsing CERTIFICATE message\n",
			(ssl->flags & SSL_FLAGS_SERVER) ? "Server" : "Client");
#ifdef USE_CERT_CHAIN_PARSING
		if (ssl->rec.partial) {
/*
			The test for a first pass is against the record header length
*/
			if (ssl->rec.hsBytesParsed == ssl->recordHeadLen) {
/*
				Account for the one-time header portion parsed above
				and the 3 byte cert chain length about to be parsed below.
				The minimum length tests have already been performed.
*/
				ssl->rec.hsBytesParsed += ssl->hshakeHeadLen + 3;
			} else {
				goto SKIP_CERT_CHAIN_INIT;
			}
		}
#endif
		if (end - c < 3) {
			ssl->err = SSL_ALERT_DECODE_ERROR;
			psTraceInfo("Invalid Certificate message\n");
			return MATRIXSSL_ERROR;
		}
		certChainLen = *c << 16; c++;
		certChainLen |= *c << 8; c++;
		certChainLen |= *c; c++;
		if (certChainLen == 0) {
#ifdef SERVER_WILL_ACCEPT_EMPTY_CLIENT_CERT_MSG
			if (ssl->flags & SSL_FLAGS_SERVER) {
				ssl->err = SSL_ALERT_BAD_CERTIFICATE;
				ssl->flags &= ~SSL_FLAGS_CLIENT_AUTH;
				goto STRAIGHT_TO_USER_CALLBACK;
			}
#endif
			if (ssl->majVer == SSL3_MAJ_VER && ssl->minVer == SSL3_MIN_VER) {
				ssl->err = SSL_ALERT_NO_CERTIFICATE;
			} else {
				ssl->err = SSL_ALERT_BAD_CERTIFICATE;
			}
			psTraceInfo("No certificate sent to verify\n");
			return MATRIXSSL_ERROR;
		}
		if (end - c < 3) {
			ssl->err = SSL_ALERT_DECODE_ERROR;
			psTraceInfo("Invalid Certificate message\n");
			return MATRIXSSL_ERROR;
		}

#ifdef USE_CERT_CHAIN_PARSING
	SKIP_CERT_CHAIN_INIT:
		if (ssl->rec.partial) {
/*			It is possible to activate the CERT_STREAM_PARSE feature and not 
			receive a cert chain in multiple buffers.  If we are not flagged
			for 'partial' parsing, we can drop into the standard parse case
*/
			while (end - c > 0) { 
				certLen = *c << 16; c++;
				certLen |= *c << 8; c++;
				certLen |= *c; c++;
				if ((parseLen = parseSingleCert(ssl, c, end, certLen)) < 0 ) {
					return parseLen;
				}
				ssl->rec.hsBytesParsed += parseLen + 3; /* 3 for certLen */
				c += parseLen;
			}
			if (ssl->rec.hsBytesParsed < ssl->rec.trueLen) {
				return MATRIXSSL_SUCCESS;
			}

			psAssert(ssl->rec.hsBytesParsed == ssl->rec.trueLen);
/*
			Got it all.  Disable the stream mechanism.
*/
			ssl->rec.partial = 0x0;
			ssl->rec.hsBytesParsed = 0;
			ssl->rec.hsBytesHashed = 0;
		} else {
			psAssert(certChainLen > 0);
#endif /* USE_CERT_CHAIN_PARSING */
		i = 0;
		currentCert = NULL;
		while (certChainLen > 0) {
			certLen = *c << 16; c++;
			certLen |= *c << 8; c++;
			certLen |= *c; c++;

			if ((uint32)(end - c) < certLen) {
				ssl->err = SSL_ALERT_DECODE_ERROR;
				psTraceInfo("Invalid certificate length\n");
				return MATRIXSSL_ERROR;
			}
/*
			Extract the binary cert message into the cert structure
*/			
			if ((parseLen = psX509ParseCert(ssl->hsPool, c, certLen, &cert, 0))
					< 0) {
				psX509FreeCert(cert);
				if (parseLen == PS_MEM_FAIL) {
					ssl->err = SSL_ALERT_INTERNAL_ERROR;
				} else {
					ssl->err = SSL_ALERT_BAD_CERTIFICATE;
				}
				return MATRIXSSL_ERROR;
			}
			c += parseLen;

			if (i++ == 0) {
				ssl->sec.cert = cert;
				currentCert = ssl->sec.cert;
			} else {
				currentCert->next = cert;
				currentCert = currentCert->next;
			}
			certChainLen -= (certLen + 3);
		}
#ifdef USE_CERT_CHAIN_PARSING
		}
#endif /* USE_CERT_CHAIN_PARSING */

#ifdef USE_CLIENT_SIDE_SSL
/*
		Now want to test to see if supplied parent-most cert is the appropriate
		authenitcation algorithm for the chosen cipher suite.  Have seen test
		cases with OpenSSL where an RSA cert will be sent for an ECDHE_ECDSA
		suite, for example.  Just testing on the client side because client
		auth is a bit more flexible on the algorithm choices.
		
		'cert' is pointing to parent-most in chain
*/
		if (!(ssl->flags & SSL_FLAGS_SERVER)) {
			if (csCheckCertAgainstCipherSuite(cert->sigAlgorithm,
					ssl->cipher->type) == 0) {
				psTraceIntInfo("Server sent bad sig alg for cipher suite %d\n",
					cert->sigAlgorithm);
				ssl->err = SSL_ALERT_UNSUPPORTED_CERTIFICATE;
				return MATRIXSSL_ERROR;
			}
		}
#endif		
		
/*
		Time to authenticate the supplied cert against our CAs
*/
		rc = matrixValidateCerts(ssl->hsPool, ssl->sec.cert,
			ssl->keys == NULL ? NULL : ssl->keys->CAcerts);

		if (rc == PS_MEM_FAIL) {
			ssl->err = SSL_ALERT_INTERNAL_ERROR;
			return MATRIXSSL_ERROR;
		}
			
/*
		Now walk the subject certs and convert any parse or authentication error 
		into an SSL alert.  The alerts SHOULD be read by the user callback
		to determine whether they are fatal or not.  If no user callback,
		the first alert will be considered fatal.
*/
		cert = ssl->sec.cert;
		while (cert) {	
			if (ssl->err != SSL_ALERT_NONE) {
				break; /* The first alert is the logical one to send */
			}
			switch (cert->authStatus) {

			case PS_CERT_AUTH_FAIL_SIG:
				ssl->err = SSL_ALERT_BAD_CERTIFICATE;
				break;
			case PS_CERT_AUTH_FAIL_REVOKED:
				ssl->err = SSL_ALERT_CERTIFICATE_REVOKED;
				break;
			case PS_CERT_AUTH_FAIL_BC:
			case PS_CERT_AUTH_FAIL_DN:
/*
				If this was a problem in the middle of the chain it means
				the chain couldn't even validate itself.  If it is at the
				end it means a matching CA could not be found
*/			
				if (cert->next != NULL) {
					ssl->err = SSL_ALERT_BAD_CERTIFICATE;
				} else {
					ssl->err = SSL_ALERT_UNKNOWN_CA;
				}
				break;
							
			default:
				break;
			
			}
			cert = cert->next;
		}
		
/*
		The last thing we want to check before passing the certificates to
		the user callback is the case in which we don't have any
		CA files loaded but we were passed a valid chain that was
		terminated with a self-signed cert.  The fact that a CA on this
		peer has not validated the chain should result in an UNKNOWN_CA alert
		
		NOTE:  This case should only ever get hit if VALIDATE_KEY_MATERIAL
		has been disabled in matrixssllib.h
*/
		if (ssl->err == SSL_ALERT_NONE &&
				(ssl->keys == NULL || ssl->keys->CAcerts == NULL)) {
			ssl->err = SSL_ALERT_UNKNOWN_CA;
			psTraceInfo("WARNING: Valid self-signed cert or cert chain but no local authentication\n");
			rc = -1;  /* Force the check on existence of user callback */
		}
		
		if (rc < 0) {
			psTraceInfo("WARNING: cert did not pass internal validation test\n");
/*
			Cert auth failed.  If there is no user callback issue fatal alert
			because there will be no intervention to give it a second look.
*/
			if (ssl->sec.validateCert == NULL) {
/*
				ssl->err should have been set correctly above but catch
				any missed cases with the generic BAD_CERTIFICATE alert
*/
				if (ssl->err == SSL_ALERT_NONE) {
					ssl->err = SSL_ALERT_BAD_CERTIFICATE;
				}
				return MATRIXSSL_ERROR;
			}
		}

#ifdef SERVER_WILL_ACCEPT_EMPTY_CLIENT_CERT_MSG
STRAIGHT_TO_USER_CALLBACK:
#endif

/*
		Return from user validation space with knowledge that there is a fatal
		alert or that this is an ANONYMOUS connection.
*/
		rc = matrixUserCertValidator(ssl, ssl->err, ssl->sec.cert, 
				ssl->sec.validateCert);
/*
		Test what the user callback returned.
*/
		ssl->sec.anon = 0;
		if (rc == SSL_ALLOW_ANON_CONNECTION) {
			ssl->sec.anon = 1;
		} else if (rc > 0) {
/*
			User returned an alert.  May or may not be the alert that was
			determined above.
*/
			psTraceIntInfo("Certificate authentication alert %d\n", rc);
			ssl->err = rc;
			return MATRIXSSL_ERROR;
		} else if (rc < 0) {
			psTraceIntInfo("User certificate callback had an internal error\n",
				rc);
			ssl->err = SSL_ALERT_INTERNAL_ERROR;
			return MATRIXSSL_ERROR;
		}
				
		rc = MATRIXSSL_SUCCESS; /* done using rc as a temp */
/*
		User callback returned 0 (continue on).  Did they determine the alert
		was not fatal after all?
*/
		if (ssl->err != SSL_ALERT_NONE) {
			psTraceIntInfo("User certificate callback determined alert %d was NOT fatal\n",
				ssl->err);
			ssl->err = SSL_ALERT_NONE;
		}
				
/*
		Either a client or server could have been processing the cert as part of
		the authentication process.  If server, we move to the client key
		exchange state.
*/
		if (ssl->flags & SSL_FLAGS_SERVER) {
			ssl->hsState = SSL_HS_CLIENT_KEY_EXCHANGE;
		} else {
			ssl->hsState = SSL_HS_SERVER_HELLO_DONE;
		}
		break;
#endif /* USE_CLIENT_SIDE_SSL */

#ifdef USE_CLIENT_SIDE_SSL
	case SSL_HS_SERVER_HELLO_DONE: 
		psTraceHs(">>> Client parsing SERVER_HELLO_DONE message\n");
		if (hsLen != 0) {
			ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
			psTraceInfo("Invalid ServerHelloDone message\n");
			return MATRIXSSL_ERROR;
		}
		ssl->hsState = SSL_HS_FINISHED;
		rc = SSL_PROCESS_DATA;
		break;

	case SSL_HS_CERTIFICATE_REQUEST: 
		psTraceHs(">>> Client parsing CERTIFICATE_REQUEST message\n");
		if (hsLen < 4) {
			ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
			psTraceInfo("Invalid Certificate Request message\n");
			return MATRIXSSL_ERROR;
		}
/*
		Currently ignoring the authentication type request because it was
		underspecified up to TLS 1.1 and TLS 1.2 is now taking care of this
		with the supported_signature_algorithms handling
*/
		certTypeLen = *c++;
		if (end - c < certTypeLen) {
			ssl->err = SSL_ALERT_DECODE_ERROR;
			psTraceInfo("Invalid Certificate Request message\n");
			return MATRIXSSL_ERROR;
		}
		c += certTypeLen; /* Skipping (RSA_SIGN etc.) */
	
		certChainLen = 0;
		if (end - c > 1) {
			certChainLen = *c << 8; c++;
			certChainLen |= *c; c++;
        	if (end - c < certChainLen) {
				ssl->err = SSL_ALERT_DECODE_ERROR;
				psTraceInfo("Invalid Certificate Request message\n");
				return MATRIXSSL_ERROR;
			}
		}
/*
		Check the passed in DNs against our cert issuer to see if they match.
		Only supporting a single cert on the client side.
*/
		ssl->sec.certMatch = 0;
		
#ifdef USE_CLIENT_AUTH		
/*
		If the user has actually gone to the trouble to load a certificate
		to reply with, we flag that here so there is some flexibility as
		to whether we want to reply with something (even if it doesn't match)
		just in case the server is willing to do a custom test of the cert
*/
		if (ssl->keys != NULL && ssl->keys->cert) {
			ssl->sec.certMatch = SSL_ALLOW_ANON_CONNECTION;
		}
#endif /* USE_CLIENT_AUTH */
		
		while (certChainLen > 0) {
			certLen = *c << 8; c++;
			certLen |= *c; c++;
			if ((uint32)(end - c) < certLen) {
				ssl->err = SSL_ALERT_DECODE_ERROR;
				psTraceInfo("Invalid CertificateRequest message\n");
				return MATRIXSSL_ERROR;
			}
#ifdef USE_CLIENT_AUTH
/*
			Can parse the message, but will not look for a match.  The
			setting of certMatch to 1 will trigger the correct response
			in sslEncode
*/
			if (ssl->keys != NULL && ssl->keys->cert) {
/*				
				Flag a match if the hash of the DN issuer is identical
*/ 
				if (ssl->keys->cert->issuer.dnencLen == certLen) {
					if (memcmp(ssl->keys->cert->issuer.dnenc, c, certLen) == 0){
						ssl->sec.certMatch = 1;
					}
				}
			}		
#endif /* USE_CLIENT_AUTH */
			c += certLen;
			certChainLen -= (2 + certLen);
		}	
		ssl->hsState = SSL_HS_SERVER_HELLO_DONE;
		break;
#endif /* USE_CLIENT_SIDE_SSL */

#if defined(USE_CLIENT_AUTH) && defined(USE_SERVER_SIDE_SSL)
	case SSL_HS_CERTIFICATE_VERIFY: 
		psTraceHs(">>> Server parsing CERTIFICATE_VERIFY message\n");
		
		certVerifyLen =  MD5_HASH_SIZE + SHA1_HASH_SIZE;
			
		pubKeyLen = *c << 8; c++;
		pubKeyLen |= *c; c++;
		if ((uint32)(end - c) < pubKeyLen) {
			ssl->err = SSL_ALERT_DECODE_ERROR;
			psTraceInfo("Invalid Certificate Verify message\n");
			return MATRIXSSL_ERROR;
		}
/*
		The server side verification of client identity.  If we can match
		the signature we know the client has possesion of the private key.
*/
		pkiPool = NULL;
		
		if (csRsaDecryptPub(pkiPool, &ssl->sec.cert->publicKey, c,
				pubKeyLen, certVerify, certVerifyLen, rsaData) < 0) {
			psTraceInfo("Unable to publicly decrypt Certificate Verify message\n");
			return MATRIXSSL_ERROR;
		}
		
		if (memcmp(certVerify, hsMsgHash, certVerifyLen) != 0) {
			psTraceInfo("Unable to verify client certificate signature\n");
			return MATRIXSSL_ERROR;
		}
		
		c += pubKeyLen;
		ssl->hsState = SSL_HS_FINISHED;
		break;
#endif /* USE_SERVER_SIDE_SSL && USE_CLIENT_AUTH */


	default:
		ssl->err = SSL_ALERT_UNEXPECTED_MESSAGE;
		return MATRIXSSL_ERROR;
	}
	
	
/*
	if we've got more data in the record, the sender has packed
	multiple handshake messages in one record.  Parse the next one.
*/
	if (c < end) {
		goto parseHandshake;
	}
	return rc;
}

/******************************************************************************/
#ifdef USE_CLIENT_SIDE_SSL
#ifdef USE_CERT_CHAIN_PARSING
static int32 parseSingleCert(ssl_t *ssl, unsigned char *c, unsigned char *end, 
						   int32 certLen)
{
	int32			parseLen;
	psX509Cert_t	*cert, *p;

/*
	Extract the binary cert message into the cert structure
*/
	if ((parseLen = psX509ParseCert(ssl->hsPool, c, certLen, &cert, 0)) < 0) {
		psX509FreeCert(cert);
		if (parseLen == PS_MEM_FAIL) {
			ssl->err = SSL_ALERT_INTERNAL_ERROR;
		} else {
			ssl->err = SSL_ALERT_BAD_CERTIFICATE;
		}
		return MATRIXSSL_ERROR;
	}
	if (ssl->sec.cert == NULL) {
		ssl->sec.cert = cert;
	} else {
		p = ssl->sec.cert;
		while (p->next != NULL) {
			p = p->next;
		}
		p->next = cert;
	}
	return parseLen;
}
#endif /* USE_CERT_CHAIN_PARSING */
#endif /* USE_CLIENT_SIDE_SSL */

/******************************************************************************/
