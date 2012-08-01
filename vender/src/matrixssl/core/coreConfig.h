/*
 *	coreConfig.h
 *	Release $Name: MATRIXSSL-3-3-1-OPEN $
 *
 *	Configuration settings for Matrix core module
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

#ifndef _h_PS_CORECONFIG
#define _h_PS_CORECONFIG

/******************************************************************************/
/* Configurable features */
/******************************************************************************/
/*
	If enabled, calls to the psError set of APIs will perform a platform
	abort on the exeutable to aid in debugging.  
*/
/* #define HALT_ON_PS_ERROR */ /* NOT RECOMMENDED FOR PRODUCTION BUILDS */

/******************************************************************************/
/*
	Turn on the psTraceCore set of APIs for log trace of the core module
*/
/* #define USE_CORE_TRACE */


/******************************************************************************/
/*
	Include the osdepMutex family of APIs	
*/
/* #define USE_MULTITHREADING */

/******************************************************************************/
/*
	Does the platform support a native 64-bit data type (long long)?
	Most 32 bit platforms do support native 64 bit integers, or at least can
		accumulate 32 bit products into a 64 bit register result.
	(Formerly this option was named USE_INT64)
*/
#define HAVE_NATIVE_INT64



#endif /* _h_PS_CORECONFIG */
/******************************************************************************/

