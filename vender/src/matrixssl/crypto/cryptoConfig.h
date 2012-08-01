/*
 *	cryptoConfig.h
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

#ifndef _h_PS_CRYPTOCONFIG
#define _h_PS_CRYPTOCONFIG

/******************************************************************************/
/* Configurable features */
/******************************************************************************/
/*
	Enable psTraceCrypto family of APIs for debugging the crypto module
*/
/* #define USE_CRYPTO_TRACE */

/******************************************************************************/
/*
	Public-Key Algorithms and performance settings
*/
#define USE_RSA

/*
	Set to either optimize for faster speed or for smaller ram usage
	when using public key operations.  Only one may be defined.
	
	The speed gain for optimizing for speed is around 5%
	The memory savings for optimizing for ram is around 50%
*/
#define PS_PUBKEY_OPTIMIZE_FOR_SMALLER_RAM
/* #define PS_PUBKEY_OPTIMIZE_FOR_FASTER_SPEED */


/******************************************************************************/
/*
	Symmetric bock ciphers (including CBC mode) and performance settings
*/
#define USE_AES
#define USE_3DES
/* #define USE_DES */

/*
	Optionally set to improve performance at the cost of larger binary code
	size.  Platforms vary, but ciphers will generally see a 5%-10% performance
	boost at the cost of 10-20 kilobytes (per algorithm).
*/
/* #define PS_AES_IMPROVE_PERF_INCREASE_CODESIZE */
/* #define PS_3DES_IMPROVE_PERF_INCREASE_CODESIZE */


/******************************************************************************/
/*
	Symmetric stream ciphers
*/
/* #define USE_ARC4 */
/* #define USE_RC2 */ /* Only PKCS#12 parse should ever want this algorithm */

/******************************************************************************/
/*
	Digest algorithms	
*/
#define USE_SHA1
#define USE_MD5

#define USE_HMAC /* Requires USE_MD5 and/or USE_SHA1 */

/*
	Optionally set to improve performance at the cost of larger binary code
	size.  Platforms vary, but digests will generally see a 5%-10% performance
	boost at the cost of 1-10 kilobytes (per algorithm).
*/
/* #define PS_MD5_IMPROVE_PERF_INCREASE_CODESIZE */
/* #define PS_SHA1_IMPROVE_PERF_INCREASE_CODESIZE */


/******************************************************************************/
/*
	X.509 Certificate
*/
#define USE_X509
#define USE_CERT_PARSE /* Usually required.  USE_X509 must be enabled */
/* #define USE_FULL_CERT_PARSE */ /* USE_CERT_PARSE must be enabled */

#define USE_BASE64_DECODE

/******************************************************************************/
/*
	PKCS support
*/
#define USE_PRIVATE_KEY_PARSING
#define USE_PKCS5 /* v2.0 3des PBKDF encrypted private keys */
#define USE_PKCS8 /* Alternative private key storage format */
/* #define USE_PKCS12 */ /* You must enable USE_PKCS8 if enabling PKCS12 */

/******************************************************************************/
/*
  	PRNG Algorithms 
*/
#define USE_YARROW

/******************************************************************************/
/*
    All below here are configurable tweaks (do not need to touch, in general)
*/
/* #define USE_BURN_STACK */

#endif /* _h_PS_CRYPTOCONFIG */
/******************************************************************************/

