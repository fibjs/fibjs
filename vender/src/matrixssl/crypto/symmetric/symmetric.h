/*
 *	symmetric.h
 *	Release $Name: MATRIXSSL-3-3-1-OPEN $
 *
 *	Header for internal symmetric key cryptography support
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

#ifndef _h_PS_SYMMETRIC
#define _h_PS_SYMMETRIC

/******************************************************************************/
#ifdef USE_AES
/******************************************************************************/
typedef struct {
	uint32 eK[64], dK[64];
	int32 Nr;
} psAesKey_t;


typedef struct {
	int32			blocklen;
	unsigned char	IV[16];
	psAesKey_t		key;
} aes_CBC;

#endif /* USE_AES */
/******************************************************************************/


/******************************************************************************/
#if defined(USE_3DES) || defined(USE_DES)
/******************************************************************************/
#define DES3_KEY_LEN	24
#define DES3_IV_LEN		8
#define DES_KEY_LEN		8

typedef struct {
	uint32 ek[3][32], dk[3][32];
} psDes3Key_t;

/*
	A block cipher CBC structure
 */
typedef struct {
	int32				blocklen;
	unsigned char		IV[8];
	psDes3Key_t			key;
} des3_CBC;

#endif /* USE_3DES || USE_DES */
/******************************************************************************/

/******************************************************************************/
#ifdef USE_ARC4
typedef struct {
	unsigned char	state[256];
	uint32	byteCount;
	unsigned char	x;
	unsigned char	y;
} psRc4Key_t;
#endif /* USE_ARC4 */
/******************************************************************************/
#ifdef USE_RC2
typedef struct {
	unsigned xkey[64];
} psRc2Key_t;

typedef struct {
	int32				blocklen;
	unsigned char		IV[8];
	psRc2Key_t			key;
} rc2_CBC;
#endif /* USE_RC2 */
/******************************************************************************/
/*	Universal types and defines */
/******************************************************************************/
#define MAXBLOCKSIZE	24

typedef union {
#ifdef USE_RC2
	rc2_CBC		rc2;
#endif
#ifdef USE_ARC4
	psRc4Key_t	arc4;
#endif
#ifdef USE_3DES
	des3_CBC	des3;
#endif
#ifdef USE_AES
	aes_CBC		aes;
#endif
} psCipherContext_t;

#define byte(x, n) (((x) >> (8 * (n))) & 255)

#endif /* _h_PS_SYMMETRIC */
/******************************************************************************/
