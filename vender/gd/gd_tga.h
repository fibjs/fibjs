/*
 * TGA Image read support
 * Copyright (C) 2005  Andrew Ireland, Jon Keto, Michael Beal
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#define TGA_LINE_LENGTH	1024

#ifndef __TGA_H
#define __TGA_H	1


#include "gd.h"
#include "gdhelpers.h"

#ifndef BGDWIN32
#ifndef O_BYTE
#define O_BYTE	1
#endif
#endif

#ifndef _WIN32
typedef char byte;
#endif

#ifndef O_SHORT_INT
#define O_SHORT_INT	1
	typedef int short_int;
#endif

typedef struct oTga_
{
	byte  identsize;	// size of ID field that follows 18 byte header (0 usually)
	byte  colormaptype;	// type of colour map 0=none, 1=has palette					[IGNORED] Adrian requested no support
	byte  imagetype;	// type of image 0=none,1=indexed,2=rgb,3=grey,+8=rle packed

	int colormapstart;	// first colour map entry in palette						[IGNORED] Adrian requested no support
	int colormaplength;	// number of colours in palette								[IGNORED] Adrian requested no support
	byte  colormapbits;	// number of bits per palette entry 15,16,24,32				[IGNORED] Adrian requested no support

	int xstart;			// image x origin
	int ystart;			// image y origin
	int width;			// image width in pixels
	int height;			// image height in pixels
	byte  bits;			// image bits per pixel 8,16,24,32
	byte alphabits;		// alpha bits (low 4bits of header 17)
	byte fliph;			// horizontal or vertical
	byte flipv;			// flip
	char *ident;		// identifcation tag string
	int *bitmap;		// bitmap data

} oTga;

#define TGA_TYPE_NO_IMAGE						0
#define TGA_TYPE_INDEXED						1
#define TGA_TYPE_RGB							2
#define TGA_TYPE_GREYSCALE						3
#define TGA_TYPE_INDEXED_RLE					9
#define TGA_TYPE_RGB_RLE						10
#define TGA_TYPE_GREYSCALE_RLE					11
#define TGA_TYPE_INDEXED_HUFFMAN_DELTA_RLE		32
#define TGA_TYPE_RGB_HUFFMAN_DELTA_QUADTREE_RLE	33

#define TGA_BPP_8	8
#define TGA_BPP_16	16
#define TGA_BPP_24	24
#define TGA_BPP_32	32

#define TGA_RLE_FLAG	128

int read_header_tga(gdIOCtx *ctx, oTga *tga);
int read_image_tga(gdIOCtx *ctx, oTga *tga);
void free_tga(oTga *tga);

#endif //__TGA_H
