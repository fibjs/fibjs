/*
 *	pubkey.h
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

#ifndef _h_PS_PUBKEY
#define _h_PS_PUBKEY

#define PUBKEY_TYPE		0x01
#define PRIVKEY_TYPE	0x02

/* Public Key types for psPubKey_t */
#define PS_RSA	1
#define	PS_ECC	2
#define PS_DH	3

/* Sig types */
#define	RSA_TYPE_SIG			5
#define	DSA_TYPE_SIG			6

/*
	Pub key speed or size optimization handling
*/
#if defined(PS_PUBKEY_OPTIMIZE_FOR_FASTER_SPEED) &&	defined(PS_PUBKEY_OPTIMIZE_FOR_SMALLER_RAM)
#error "May only enable either PS_PUBKEY_OPTIMIZE_FOR_FASTER_SPEED or PS_PUBKEY_OPTIMIZE_FOR_SMALLER_RAM"
#endif

#if !defined(PS_PUBKEY_OPTIMIZE_FOR_FASTER_SPEED) && !defined(PS_PUBKEY_OPTIMIZE_FOR_SMALLER_RAM)
#define PS_PUBKEY_OPTIMIZE_FOR_SMALLER_RAM
#endif

#ifdef PS_PUBKEY_OPTIMIZE_FOR_SMALLER_RAM
#define PS_EXPTMOD_WINSIZE		3
#endif

#ifdef PS_PUBKEY_OPTIMIZE_FOR_FASTER_SPEED
#define PS_EXPTMOD_WINSIZE		5
#endif

/******************************************************************************/
#ifdef USE_RSA
/******************************************************************************/
/*
    Primary RSA Key struct.  Define here for crypto
*/
typedef struct {
    pstm_int    e, d, N, qP, dP, dQ, p, q;
    uint32      size;   /* Size of the key in bytes */
    int32       optimized; /* 1 for optimized */
} psRsaKey_t;


#endif /* USE_RSA */
/******************************************************************************/

/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
/*
	Univeral public key type

	The pubKey name comes from the generic public-key crypto terminology and
	does not mean these key are restricted to the public side only. These
	may be private keys.
*/
/******************************************************************************/
typedef union {
#ifdef USE_RSA
    psRsaKey_t	rsa;
#else
	short		notEmpty; /* Prevents from being empty */
#endif /* USE_RSA */
} pubKeyUnion_t;

typedef struct {
	pubKeyUnion_t	*key;
	uint32			keysize; /* in bytes */
	int32			type; /* PS_RSA, PS_ECC, PS_DH */ 
} psPubKey_t;

/******************************************************************************/
/*
	Internal helpers
*/
extern int32 pkcs1Pad(unsigned char *in, uint32 inlen, unsigned char *out,
				uint32 outlen, int32 cryptType);
extern int32 pkcs1Unpad(unsigned char *in, uint32 inlen, unsigned char *out,
				uint32 outlen, int32 decryptType);

#ifdef USE_RSA				
extern void psRsaFreeKey(psRsaKey_t *key);
#endif /* USE_RSA */
/******************************************************************************/
#endif /* _h_PS_PUBKEY */

