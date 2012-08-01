/*
 *	matrixsslConfig.h
 *	Release $Name: MATRIXSSL-3-3-1-OPEN $
 *
 *	Configuration settings for building the MatrixSSL library.
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

#ifndef _h_MATRIXSSLCONFIG
#define _h_MATRIXSSLCONFIG


#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/*
	Recommended cipher suites:

	Define the following to enable various cipher suites
	At least one of these must be defined.  If multiple are defined,
	the handshake will determine which is best for the connection.
*/
#define USE_TLS_RSA_WITH_AES_128_CBC_SHA
#define USE_TLS_RSA_WITH_AES_256_CBC_SHA
/* #define USE_SSL_RSA_WITH_3DES_EDE_CBC_SHA */
/* #define USE_SSL_RSA_WITH_RC4_128_SHA */
/* #define USE_SSL_RSA_WITH_RC4_128_MD5 */

/******************************************************************************/
/*
	These cipher suites do not combine authentication and encryption and
	are not recommended for use-cases that require strong security
*/
/* #define USE_SSL_RSA_WITH_NULL_SHA */
/* #define USE_SSL_RSA_WITH_NULL_MD5 */

/******************************************************************************/
/*
	Support for TLS protocols.
	
	- USE_TLS versions must 'stack' for compiling purposes
		- must enable TLS if enabling TLS 1.1
		- must enable TLS 1.1 if enabling TLS 1.2
	- Use the DISABLE_TLS_ defines to disallow specific protocols at runtime
		that have been enabled via USE_TLS_. 
	- There is no DISABLE_TLS_ for the latest version of the protocol.  If
		you don't want to use that version disable the USE_TLS_ define instead
*/
#define USE_TLS			/* TLS 1.0 aka SSL 3.1 */
#define USE_TLS_1_1


/* #define DISABLE_TLS_1_1 */
/* #define DISABLE_TLS_1_0 */
#define DISABLE_SSLV3	/* Disable SSL 3.0 */


/******************************************************************************/
/*
	Compile time support for server or client side SSL
*/
#define USE_CLIENT_SIDE_SSL
#define USE_SERVER_SIDE_SSL



/******************************************************************************/
/*
	The initial buffer sizes for send and receive buffers in each ssl_t session.
	Buffers are internally grown if more incoming or outgoing data storage is
	needed, up to a maximum of SSL_MAX_BUF_SIZE. Once the memory used by the
	buffer again drops below SSL_DEFAULT_X_BUF_SIZE, the buffer will be reduced
	to this size. Most standard SSL handshakes require on the order of 1024 B.
	
	SSL_DEFAULT_X_BUF_SIZE	value in bytes, maximum SSL_MAX_BUF_SIZE
 */
#define	SSL_DEFAULT_IN_BUF_SIZE		1500		/* Baseline recv buf size */
#define	SSL_DEFAULT_OUT_BUF_SIZE	1500		/* Baseline send buf size */

/******************************************************************************/
/*	
	The TLS specifications sets the maximum plaintext length at 16KB.
	However, RFC 4366 introduces a "max_fragment_length" extension that enables
	a client to request a smaller max from the server.  If not default (0x4000),
	the ONLY possible options for REQUESTED_MAX_PLAINTEXT_RECORD_LEN are:
	
		0x200 (512 bytes), 0x400 (1K), 0x800 (2K), or 0x1000 (4K)
		
	The "catch" to this request is that the server is not required to support
	this extension.  MatrixSSL servers do support this.
*/
#define REQUESTED_MAX_PLAINTEXT_RECORD_LEN	0x4000

/******************************************************************************/
/*
	If SERVER you may define the number of sessions to cache and how
	long a session will remain valid in the cache from first access.
	
	Session caching enables very fast "session resumption handshakes". Session
	caching can effectively by disabled by setting SSL_SESSION_ENTRY_LIFE to 0

	SSL_SESSION_TABLE_SIZE minimum value is 1
	SSL_SESSION_ENTRY_LIFE is in milliseconds, minimum 0

*/
#define SSL_SESSION_TABLE_SIZE	32
#define SSL_SESSION_ENTRY_LIFE	86400 * 1000 /* one day */



/******************************************************************************/
/*
	REHANDSHAKING SUPPORT
	In late 2009 An "authentication gap" exploit was discovered in the
	SSL re-handshaking protocol.  The fix to the exploit was introduced
	in RFC 5746 and is referred to here	as SECURE_REHANDSHAKES.
	
	ENABLE_SECURE_REHANDSHAKES implements RFC 5746 and will securely
	renegotiate with any implementations that support it.  It is
	recommended to leave this enabled.
	
	By enabling REQUIRE_SECURE_REHANDSHAKES, the library will test that each
	communicating peer has implemented RFC 5746 and will terminate handshakes
	with any that have not.
	
	If working with SSL peers that have not implemented RFC 5746 and
	rehandshakes are required, you may enable ENABLE_INSECURE_REHANDSHAKES
	but it is NOT RECOMMENDED
			
	It is a conflict to enable both ENABLE_INSECURE_REHANDSHAKES and
	REQUIRE_SECURE_REHANDSHAKES and a compile error will occur
	
	To completely disable rehandshaking comment out all three of these defines
*/
#define ENABLE_SECURE_REHANDSHAKES
/* #define REQUIRE_SECURE_REHANDSHAKES */
/* #define ENABLE_INSECURE_REHANDSHAKES */ /* NOT RECOMMENDED */

/******************************************************************************/
/*
	Google has an implementation of HTTPS client (Chrome) that sends application
	data immedately after the client FINISHED message, and before the server
	has responded with a CHANGE_CIPHER_SPEC and FINISHED message. This saves
	a round trip when sending the HTTP request, since it is sent with the
	FINISHED message in a single IP packet, and does not wait for the server
	handshake response.

	A similar technique is used and supported by MatrixSSL for including
	application data at the end of a finished message that does not require
	a response (a standard server Finished message for example). However,
	this implementation in Chrome is subtly and importantly different.

	MatrixSSL was written to explicitly ignore any data after any handshake
	message that requires a response, because in all cases, this response
	changes the state of the client.

	Chrome cannot be assured that the final handshake hash matches (and that
	the handshake was not tampered with) before it sends the potentially
	sensitive HTTP request.

	Apparently most other SSL implementations do allow this "trick" to 
	succeed, and so we have added support as well using a conditional compile.
	Enabling this allows Chrome browsers to connect to HTTPS servers running
	MatrixSSL. Sending app data with a client finished message from MatrixSSL
	is still NOT SUPPORTED for the security reasons above.

	For more information:
	http://tools.ietf.org/html/draft-bmoeller-tls-falsestart-00	

	UPDATE April 2012: Google has announced this feature will be removed in
	version 20 of their browser due to industry compatibility issues.
*/
#define ENABLE_FALSE_START

/******************************************************************************/
/*
    In Sept. 2011 security researchers demonstrated how a previously known
    CBC encryption weakness could be used to decrypt HTTP data over SSL.
    The attack was named BEAST (Browser Exploit Against SSL/TLS).
 
	This issue only effects TLS 1.0 (and SSL) and only if the cipher suite
	is using a symmetric CBC block cipher.  Enable USE_TLS_1_1 above to
	completely negate this workaround if TLS 1.1 is also supported by peers.
   
    As with previous SSL vulnerabilities, the attack is generally considered
    a very low risk for individual browsers as it requires the attacker
    to have control over the network to become a MITM.  They will also have
    to have knowledge of the first couple blocks of underlying plaintext
    in order to mount the attack.
    
    A zero length record proceeding a data record has been a known fix to this
    problem for years and MatrixSSL has always supported the handling of empty
    records. So alternatively, an implementation could always encode a zero
	length record before each record encode. Some old SSL implementations do
	not handle decoding zero length records, however.
   
    This BEAST fix is on the client side and moves the implementation down to
    the SSL library level so users do not need to manually send zero length
    records. This fix uses the same IV obfuscation logic as a zero length
    record by breaking up each application data record in two. Because some 
	implementations don't handle zero-length records, the the first record
    is the first byte of the plaintext message, and the second record
	contains the remainder of the message.
 
	This fix is based on the workaround implemented in Google Chrome:
	http://src.chromium.org/viewvc/chrome?view=rev&revision=97269

	This workaround adds approximagely 53 bytes to the encoded length of each 
	SSL record that is encoded, due to the additional header, padding and MAC
	of the second record.
*/
#define USE_BEAST_WORKAROUND

/******************************************************************************/
/*
	Enable certificate chain message "stream" parsing.  This allows single
	certificates to be parsed on-the-fly without having to wait for the entire
	certificate chain to be recieved in the buffer.  This is a memory saving
	feature for the application buffer but will add a small amount of code 
	size for the parsing and structure overhead.

	This feature will only save memory if the CERTIFICATE message is the
	only message in the record, and multiple certs are present in the chain.
*/
/* #define USE_CERT_CHAIN_PARSING */

/******************************************************************************/
/*
	Show which SSL messages are created and parsed
*/
/* #define USE_SSL_HANDSHAKE_MSG_TRACE */

/******************************************************************************/
/*
	Informational trace that could help pinpoint problems with SSL connections
*/
/* #define USE_SSL_INFORMATIONAL_TRACE */

/******************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* _h_MATRIXCONFIG */
/******************************************************************************/

