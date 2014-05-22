/*
   TIFF - Tagged Image File Format Encapsulation for GD Library

   gd_tiff.c
   Copyright (C) Pierre-A. Joye, M. Retallack

   ---------------------------------------------------------------------------
   **
   ** Permission to use, copy, modify, and distribute this software and its
   ** documentation for any purpose and without fee is hereby granted, provided
   ** that the above copyright notice appear in all copies and that both that
   ** copyright notice and this permission notice appear in supporting
   ** documentation.  This software is provided "as is" without express or
   ** implied warranty.
   **
   ---------------------------------------------------------------------------
   Ctx code written by M. Retallack

   Todo:

   If we fail - cleanup
   Writer: Use gd error function, overflow check may not be necessary as
	 we write our own data (check already done)

   Implement 2 color black/white saving using group4 fax compression
   Implement function to specify encoding to use when writing tiff data

   ----------------------------------------------------------------------------
 */
/* $Id$ */

#	include "config.h"

#include <gd.h>
#include <gdfonts.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "gdhelpers.h"

#ifdef HAVE_LIBTIFF

#include "tiff.h"
#include "tiffio.h"

#define GD_SUCCESS 1
#define GD_FAILURE 0

#define TRUE 1
#define FALSE 0

/* I define those here until the new formats
 * are commited. We can then rely on the global
 * def
 */
#define GD_PALETTE 1
#define GD_TRUECOLOR 2
#define GD_GRAY 3
#define GD_INDEXED 4
#define GD_RGB 5

#define MIN(a,b) (a < b) ? a : b;
#define MAX(a,b) (a > b) ? a : b;


typedef struct tiff_handle
{
	int size;
	int pos;
	gdIOCtx *ctx;
	int written;
}
tiff_handle;

/*
   Functions for reading, writing and seeking in gdIOCtx
   This allows for non-file i/o operations with no
   explicit use of libtiff fileio wrapper functions

   Note: because libtiff requires random access, but gdIOCtx
         only supports streams, all writes are buffered
         into memory and written out on close, also all
         reads are done from a memory mapped version of the
         tiff (assuming one already exists)
*/

tiff_handle * new_tiff_handle(gdIOCtx *g)
{
	tiff_handle * t;

	if (!g) {
		fprintf(stderr, "Cannot create a new tiff handle, missing Ctx argument");
		return NULL;
	}

	t = (tiff_handle *) gdMalloc(sizeof(tiff_handle));
	if (!t) {
		fprintf(stderr, "Failed to allocate a new tiff handle");
		return NULL;
	}

	t->size = 0;
	t->pos = 0;
	t->ctx = g;
	t->written = 0;

	return t;
}

/* TIFFReadWriteProc tiff_readproc - Will use gdIOCtx procs to read required
   (previously written) TIFF file content */
static tsize_t tiff_readproc(thandle_t clientdata, tdata_t data, tsize_t size)
{
	tiff_handle *th = (tiff_handle *)clientdata;
	gdIOCtx *ctx = th->ctx;

	size = (ctx->getBuf)(ctx, data, size);

	return size;
}

/* TIFFReadWriteProc tiff_writeproc - Will use gdIOCtx procs to write out
   TIFF data */
static tsize_t tiff_writeproc(thandle_t clientdata, tdata_t data, tsize_t size)
{
	tiff_handle *th = (tiff_handle *)clientdata;
	gdIOCtx *ctx = th->ctx;

	size = (ctx->putBuf)(ctx, data, size);
	if(size + th->pos>th->size) {
		th->size = size + th->pos;
		th->pos += size;
	}

	return size;
}

/* TIFFSeekProc tiff_seekproc
 * used to move around the partially written TIFF */
static toff_t tiff_seekproc(thandle_t clientdata, toff_t offset, int from)
{
	tiff_handle *th = (tiff_handle *)clientdata;
	gdIOCtx *ctx = th->ctx;
	int result;

	switch(from) {
		default:
		case SEEK_SET:
			/* just use offset */
		break;

		case SEEK_END:
			/* invert offset, so that it is from start, not end as supplied */
			offset = th->size + offset;
		break;

		case SEEK_CUR:
			/* add current position to translate it to 'from start',
			 * not from durrent as supplied
			 */
			offset += th->pos;
		break;
	}

	/* now, move pos in both io context and buf */
	if((result = (ctx->seek)(ctx, offset))) {
		th->pos = offset;
	}

	return result ? offset : -1;
}

/* TIFFCloseProc tiff_closeproc - used to finally close the TIFF file */
static int tiff_closeproc(thandle_t clientdata)
{
	/*tiff_handle *th = (tiff_handle *)clientdata;
	gdIOCtx *ctx = th->ctx;

	(ctx->gd_free)(ctx);*/

	return 0;
}

/* TIFFSizeProc tiff_sizeproc */
static toff_t tiff_sizeproc(thandle_t clientdata)
{
	tiff_handle *th = (tiff_handle *)clientdata;
	return th->size;
}

/* TIFFMapFileProc tiff_mapproc() */
static int tiff_mapproc(thandle_t h, tdata_t *d, toff_t *o)
{
	return 0;
}

/* TIFFUnmapFileProc tiff_unmapproc */
static void tiff_unmapproc(thandle_t h, tdata_t d, toff_t o)
{

}


/*  tiffWriter
 *  ----------
 *  Write the gd image as a tiff file (called by gdImageTiffCtx)
 *  Parameters are:
 *  image:    gd image structure;
 *  out:      the stream where to write
 *  bitDepth: depth in bits of each pixel
 */
BGD_DECLARE(void) tiffWriter(gdImagePtr image, gdIOCtx *out, int bitDepth)
{
	int x, y;
	int i;
	int r, g, b, a;
	TIFF *tiff;
	int width, height;
	int color;
	char *scan;
	int samplesPerPixel = 3;
	int bitsPerSample;
	int transparentColorR = -1;
	int transparentColorG = -1;
	int transparentColorB = -1;
	uint16 extraSamples[1];
	uint16 *colorMapRed;
	uint16 *colorMapGreen;
	uint16 *colorMapBlue;

	tiff_handle *th;

	th = new_tiff_handle(out);
	if (!th) {
		return;
	}
	extraSamples[0] = EXTRASAMPLE_ASSOCALPHA;

	/* read in the width/height of gd image */
	width = gdImageSX(image);
	height = gdImageSY(image);

	/* reset clip region to whole image */
	gdImageSetClip(image, 0, 0, width, height);

	/* handle old-style single-colour mapping to 100% transparency */
	if(image->transparent != 0xffffffff) {
		/* set our 100% transparent colour value */
		transparentColorR = gdImageRed(image, image->transparent);
		transparentColorG = gdImageGreen(image, image->transparent);
		transparentColorB = gdImageBlue(image, image->transparent);
	}

	/* Open tiff file writing routines, but use special read/write/seek
	 * functions so that tiff lib writes correct bits of tiff content to
	 * correct areas of file opened and modifieable by the gdIOCtx functions
	 */
	tiff = TIFFClientOpen("", "w", th,	tiff_readproc,
										tiff_writeproc,
										tiff_seekproc,
										tiff_closeproc,
										tiff_sizeproc,
										tiff_mapproc,
										tiff_unmapproc);

	TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH, width);
	TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, height);
	TIFFSetField(tiff, TIFFTAG_COMPRESSION, COMPRESSION_DEFLATE);
	TIFFSetField(tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC,
					(bitDepth == 24) ? PHOTOMETRIC_RGB : PHOTOMETRIC_PALETTE);

	bitsPerSample = (bitDepth == 24 || bitDepth == 8) ? 8 : 1;
	TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, bitsPerSample);

	/* build the color map for 8 bit images */
	if(bitDepth != 24) {
		colorMapRed   = (uint16 *) gdMalloc(3 * (1 << bitsPerSample));
		if (!colorMapRed) {
			return;
		}
		colorMapGreen = (uint16 *) gdMalloc(3 * (1 << bitsPerSample));
		if (!colorMapGreen) {
			return;
		}
		colorMapBlue  = (uint16 *) gdMalloc(3 *  (1 << bitsPerSample));
		if (!colorMapBlue) {
			return;
		}

		for(i = 0; i < image->colorsTotal; i++) {
			colorMapRed[i]   = gdImageRed(image,i) + (gdImageRed(image,i) * 256);
			colorMapGreen[i] = gdImageGreen(image,i)+(gdImageGreen(image,i)*256);
			colorMapBlue[i]  = gdImageBlue(image,i) + (gdImageBlue(image,i)*256);
		}

		TIFFSetField(tiff, TIFFTAG_COLORMAP, colorMapRed, colorMapGreen,
															colorMapBlue);
		samplesPerPixel = 1;
	}

	/* here, we check if the 'save alpha' flag is set on the source gd image */
	if(	(bitDepth == 24) &&
		(image->saveAlphaFlag || image->transparent != 0xffffffff)) {
		/* so, we need to store the alpha values too!
		 * Also, tell TIFF what the extra sample means (associated alpha) */
		samplesPerPixel = 4;
		TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, samplesPerPixel);
		TIFFSetField(tiff, TIFFTAG_EXTRASAMPLES, 1, extraSamples);
	} else {
		TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, samplesPerPixel);
	}

	TIFFSetField(tiff, TIFFTAG_ROWSPERSTRIP, 1);

	if(overflow2(width, samplesPerPixel)) {
		return;
	}

	if(!(scan = (char *)gdMalloc(width * samplesPerPixel))) {
		return;
	}

	/* loop through y-coords, and x-coords */
	for(y = 0; y < height; y++) {
		for(x = 0; x < width; x++) {
			/* generate scan line for writing to tiff */
			color = gdImageGetPixel(image, x, y);

			a = (127 - gdImageAlpha(image, color)) * 2;
			a = (a == 0xfe) ? 0xff : a & 0xff;
			b = gdImageBlue(image, color);
			g = gdImageGreen(image, color);
			r = gdImageRed(image, color);

			/* if this pixel has the same RGB as the transparent colour,
			 * then set alpha fully transparent */
			if(	transparentColorR == r &&
				transparentColorG == g &&
				transparentColorB == b
			) {
				a = 0x00;
			}

			if(bitDepth != 24) {
				/* write out 1 or 8 bit value in 1 byte
				 * (currently treats 1bit as 8bit) */
				scan[(x * samplesPerPixel) + 0] = color;
			} else {
				/* write out 24 bit value in 3 (or 4 if transparent) bytes */
				if(image->saveAlphaFlag || image->transparent != 0xffffffff) {
					scan[(x * samplesPerPixel) + 3] = a;
				}

				scan[(x * samplesPerPixel) + 2] = b;
				scan[(x * samplesPerPixel) + 1] = g;
				scan[(x * samplesPerPixel) + 0] = r;
			}
		}

		/* Write the scan line to the tiff */
		if(TIFFWriteEncodedStrip(tiff, y, scan, width * samplesPerPixel) == -1){
			/* error handler here */
			fprintf(stderr, "Could not create TIFF\n");
			return;
		}
	}

	/* now cloase and free up resources */
	TIFFClose(tiff);
	gdFree(scan);
	gdFree(th);

	if(bitDepth != 24) {
		gdFree(colorMapRed);
		gdFree(colorMapGreen);
		gdFree(colorMapBlue);
	}
}

/*
  gdImageTiffCtx
  --------------
  Write the gd image as a tiff file
  Parameters are:
  image:  gd image structure;
  out:    the stream where to write
 */
BGD_DECLARE(void) gdImageTiffCtx(gdImagePtr image, gdIOCtx *out)
{
	int clipx1P, clipy1P, clipx2P, clipy2P;
	int bitDepth = 24;

	/* First, switch off clipping, or we'll not get all the image! */
	gdImageGetClip(image, &clipx1P, &clipy1P, &clipx2P, &clipy2P);

	/* use the appropriate routine depending on the bit depth of the image */
	if(image->trueColor) {
		bitDepth = 24;
	} else if(image->colorsTotal == 2) {
		bitDepth = 1;
	} else {
		bitDepth = 8;
	}

	tiffWriter(image, out, bitDepth);

	/* reset clipping area to the gd image's original values */
	gdImageSetClip(image, clipx1P, clipy1P, clipx2P, clipy2P);
}

/* Check if we are really in 8bit mode */
static int checkColorMap(n, r, g, b)
	int n;
	uint16 *r, *g, *b;
{
	while (n-- > 0)
		if (*r++ >= 256 || *g++ >= 256 || *b++ >= 256)
			return (16);
	return (8);
}


/* Read and convert a TIFF colormap */
static int readTiffColorMap(gdImagePtr im, TIFF *tif, char is_bw, int photometric)
{
	uint16 *redcmap, *greencmap, *bluecmap;
	uint16 bps;
	int i;

	if (is_bw) {
		if (photometric == PHOTOMETRIC_MINISWHITE) {
			gdImageColorAllocate(im, 255,255,255);
			gdImageColorAllocate(im, 0, 0, 0);
		} else {
			gdImageColorAllocate(im, 0, 0, 0);
			gdImageColorAllocate(im, 255,255,255);
		}
	} else {
		uint16 min_sample_val, max_sample_val;

		if (!TIFFGetField(tif, TIFFTAG_MINSAMPLEVALUE, &min_sample_val)) {
			min_sample_val = 0;
		}
		if (!TIFFGetField(tif, TIFFTAG_MAXSAMPLEVALUE, &max_sample_val)) {
			max_sample_val = 255;
		}

		if (photometric == PHOTOMETRIC_MINISBLACK || photometric == PHOTOMETRIC_MINISWHITE){
			/* TODO: use TIFFTAG_MINSAMPLEVALUE and TIFFTAG_MAXSAMPLEVALUE */
			/* Gray level palette */
			for (i=min_sample_val; i <= max_sample_val; i++) {
				gdImageColorAllocate(im, i,i,i);
			}
			return GD_SUCCESS;

		} else if (!TIFFGetField(tif, TIFFTAG_COLORMAP, &redcmap, &greencmap, &bluecmap)) {
			fprintf(stderr, "Cannot read the color map");
			return GD_FAILURE;
		}

		TIFFGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE, &bps);

#define	CVT(x)		(((x) * 255) / ((1L<<16)-1))
		if (checkColorMap(1<<bps, redcmap, greencmap, bluecmap) == 16) {
			for (i = (1<<bps)-1; i > 0; i--) {
				redcmap[i] = CVT(redcmap[i]);
				greencmap[i] = CVT(greencmap[i]);
				bluecmap[i] = CVT(bluecmap[i]);
			}
		}
		for (i = 0; i < 256; i++) {
			gdImageColorAllocate(im, redcmap[i], greencmap[i], bluecmap[i]);
		}
#undef CVT
	}
	return GD_SUCCESS;
}

static void readTiffBw (const unsigned char *src,
		gdImagePtr im,
		uint16       photometric,
		int          startx,
		int          starty,
		int          width,
		int          height,
		char         has_alpha,
		int          extra,
		int          align)
{
	int x = startx, y = starty;
	int src_x, src_y;
	int k;

	for (src_y = 0; src_y < height; ++src_y) {
		k = 0;
		while (k < width) {
			register unsigned char curr = *src++;
			register unsigned char mask;

			if (photometric == PHOTOMETRIC_MINISWHITE) {
				curr = ~curr;
			}
			for (mask = 0x80; mask != 0 && k < width; mask >>= 1){
				if((curr & mask) != 0) {
					gdImageSetPixel(im, x, y, 0);
				} else {
					gdImageSetPixel(im, x, y, 1);
				}
				++x;
				++k;
			}
			++src_x;
		}
		y++;
	}
}

static void readTiff8bit (const unsigned char *src,
		gdImagePtr im,
		uint16       photometric,
		int          startx,
		int          starty,
		int          width,
		int          height,
		char         has_alpha,
		int          extra,
		int          align)
{
	int    red, green, blue, alpha;
	int    x, y;

	switch (photometric) {
		case PHOTOMETRIC_PALETTE:
			/* Palette has no alpha (see TIFF specs for more details */
			for (y = starty; y < starty + height; y++) {
				for (x = startx; x < startx + width; x++) {
					gdImageSetPixel(im, x, y,*(src++));
				}
			}
			break;

		case PHOTOMETRIC_RGB:
			if (has_alpha) {
				gdImageAlphaBlending(im, 0);
				gdImageSaveAlpha(im, 1);

				for (y = starty; y < starty + height; y++) {
					for (x = startx; x < startx + width; x++) {
						red   = *src++;
						green = *src++;
						blue  = *src++;
						alpha = *src++;
						red   = MIN (red, alpha);
						blue  = MIN (blue, alpha);
						green = MIN (green, alpha);

						if (alpha) {
							gdImageSetPixel(im, x, y, gdTrueColorAlpha(red * 255 / alpha, green * 255 / alpha, blue * 255 /alpha, gdAlphaMax - (alpha >> 1)));
						} else {
							gdImageSetPixel(im, x, y, gdTrueColorAlpha(red, green, blue, gdAlphaMax - (alpha >> 1)));
						}
					}
				}

			} else {
				for (y = 0; y < height; y++) {
					for (x = 0; x < width; x++) {
						register unsigned char r = *src++;
						register unsigned char g = *src++;
						register unsigned char b = *src++;

						gdImageSetPixel(im, x, y, gdTrueColor(r, g, b));
					}
				}
			}
			break;

		case PHOTOMETRIC_MINISWHITE:
			if (has_alpha) {
				/* We don't process the extra yet */
			} else {
				for (y = starty; y < starty + height; y++) {
					for (x = startx; x < startx + width; x++) {
						gdImageSetPixel(im, x, y, ~(*src++));
					}
				}
			}
			break;

		case PHOTOMETRIC_MINISBLACK:
			if (has_alpha) {
				/* We don't process the extra yet */
			} else{
				for (y = starty; y < height; y++) {
					for (x = 0; x < width; x++) {
						gdImageSetPixel(im, x, y, *src++);
					}
				}
			}
			break;
	}
}

static int createFromTiffTiles(TIFF *tif, gdImagePtr im, uint16 bps, uint16 photometric,
		char has_alpha, char is_bw, int extra)
{
	uint16  planar;
	int im_width, im_height;
	int tile_width, tile_height;
	int  x, y, height, width;
	unsigned char *buffer;

	if (!TIFFGetField (tif, TIFFTAG_PLANARCONFIG, &planar)) {
		planar = PLANARCONFIG_CONTIG;
	}
	TIFFGetField (tif, TIFFTAG_IMAGEWIDTH, &im_width);
	TIFFGetField (tif, TIFFTAG_IMAGELENGTH, &im_height);
	TIFFGetField (tif, TIFFTAG_TILEWIDTH, &tile_width);
	TIFFGetField (tif, TIFFTAG_TILELENGTH, &tile_height);

	buffer = (unsigned char *) gdMalloc (TIFFTileSize (tif));
	if (!buffer) {
		return FALSE;
	}

	for (y = 0; y < im_height; y += tile_height) {
		for (x = 0; x < im_width; x += tile_width) {
			TIFFReadTile(tif, buffer, x, y, 0, 0);
			width = MIN(im_width - x, tile_width);
			height = MIN(im_height - y, tile_height);
			if (bps == 16) {
			} else if (bps == 8) {
				readTiff8bit(buffer, im, photometric, x, y, width, height, has_alpha, extra, 0);
			} else if (is_bw) {
				readTiffBw(buffer, im, photometric, x, y, width, height, has_alpha, extra, 0);
			} else {
					/* TODO: implement some default reader or detect this case earlier use force_rgb */
			}
		}
	}
	return TRUE;
}

static int createFromTiffLines(TIFF *tif, gdImagePtr im, uint16 bps, uint16 photometric,
		char has_alpha, char is_bw, int extra)
{
	uint16  planar;
	uint32 im_height, im_width, im_line_size;
	int y;

	unsigned char *buffer;

	if (!TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &planar)) {
		planar = PLANARCONFIG_CONTIG;
	}

	if (!TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &im_height)) {
		fprintf(stderr, "Can't fetch TIFF height\n");
		return FALSE;
	}

	if (!TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &im_width)) {
		fprintf(stderr, "Can't fetch TIFF width \n");
		return FALSE;
	}

	im_line_size = TIFFScanlineSize(tif);

	buffer = (unsigned char *)gdMalloc(im_width * 4);
	if (!buffer) {
		return GD_FAILURE;
	}
	if (planar == PLANARCONFIG_CONTIG) {
		switch (bps) {
			case 16:
					/* TODO
					 * or simply use force_rgba
					 */
				break;

			case 8:
					for (y = 0; y < im_height; y++ ) {
						if (!TIFFReadScanline (tif, buffer, y, 0)) {
							fprintf(stderr, "Error while reading scanline %i", y);
							break;
						}
						/* reading one line at a time */
						readTiff8bit(buffer, im, photometric, 0, y, im_width, 1, has_alpha, extra, 0);
					}
					break;

			default:
				if (is_bw) {
					for (y = 0; y < im_height; y++ ) {
						if (!TIFFReadScanline (tif, buffer, y, 0)) {
							fprintf(stderr, "Error while reading scanline %i", y);
							break;
						}
						/* reading one line at a time */
						readTiffBw(buffer, im, photometric, 0, y, im_width, 1, has_alpha, extra, 0);
					}
				} else {
					/* TODO: implement some default reader or detect this case earlier > force_rgb */
				}
				break;
		}
	} else {
			/* TODO: implement a reader for separate panes. We detect this case earlier for now and use force_rgb */
	}

	gdFree(buffer);
	return GD_SUCCESS;
}

static int createFromTiffRgba(TIFF * tif, gdImagePtr im)
{
	int a;
	int x, y;
	int alphaBlendingFlag = 0;
	int color;
	int width = im->sx;
	int height = im->sy;
	uint32 *buffer;
	uint32 rgba;

	/* switch off colour merging on target gd image just while we write out
	 * content - we want to preserve the alpha data until the user chooses
	 * what to do with the image */
	alphaBlendingFlag = im->alphaBlendingFlag;
	gdImageAlphaBlending(im, 0);

	buffer = (uint32 *) gdCalloc(sizeof(uint32), width * height);
	if (!buffer) {
		return GD_FAILURE;
	}

	TIFFReadRGBAImage(tif, width, height, buffer, 0);

	for(y = 0; y < height; y++) {
		for(x = 0; x < width; x++) {
			/* if it doesn't already exist, allocate a new colour,
			 * else use existing one */
			rgba = buffer[(y * width + x)];
			a = (0xff - TIFFGetA(rgba)) / 2;
			color = gdTrueColorAlpha(TIFFGetR(rgba), TIFFGetG(rgba), TIFFGetB(rgba), a);

			/* set pixel colour to this colour */
			gdImageSetPixel(im, x, height - y - 1, color);
		}
	}

	gdFree(buffer);

	/* now reset colour merge for alpha blending routines */
	gdImageAlphaBlending(im, alphaBlendingFlag);
	return GD_SUCCESS;
}

/* gdImageCreateFromTiffCtx
** ------------------------
** Create a gdImage from a TIFF file input from an gdIOCtx
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromTiffCtx(gdIOCtx *infile)
{
	TIFF *tif;
	tiff_handle *th;

	uint16 bps, spp, photometric;
	uint16 orientation;
	int width, height;
	uint16 extra, *extra_types;
	uint16 planar;
	char	has_alpha, is_bw, is_gray;
	char	flip_horizontal, flip_vertical;
	char	force_rgba = FALSE;
	char	save_transparent;
	int		image_type;
	int   ret;

	gdImagePtr im = NULL;

	th = new_tiff_handle(infile);
	if (!th) {
		return NULL;
	}

	tif = TIFFClientOpen("", "rb", th, tiff_readproc,
										tiff_writeproc,
										tiff_seekproc,
										tiff_closeproc,
										tiff_sizeproc,
										tiff_mapproc,
										tiff_unmapproc);

	if (!tif) {
		fprintf(stderr, "Cannot open TIFF image");
		return NULL;
	}

	if (!TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width)) {
		fprintf(stderr, "TIFF error, Cannot read image width");
		goto error;
	}

	if (!TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height)) {
		fprintf(stderr, "TIFF error, Cannot read image width");
		goto error;
	}

	TIFFGetFieldDefaulted (tif, TIFFTAG_BITSPERSAMPLE, &bps);

	/* Unsupported bps, force to RGBA */
	if (bps > 8 && bps != 16) {
		force_rgba = TRUE;
	}

	TIFFGetFieldDefaulted (tif, TIFFTAG_SAMPLESPERPIXEL, &spp);

	if (!TIFFGetField (tif, TIFFTAG_EXTRASAMPLES, &extra, &extra_types)) {
		extra = 0;
	}

	if (!TIFFGetField (tif, TIFFTAG_PHOTOMETRIC, &photometric)) {
		uint16 compression;
		if (TIFFGetField(tif, TIFFTAG_COMPRESSION, &compression) &&
				(compression == COMPRESSION_CCITTFAX3 ||
				 compression == COMPRESSION_CCITTFAX4 ||
				 compression == COMPRESSION_CCITTRLE ||
				 compression == COMPRESSION_CCITTRLEW))
		{
			fprintf(stderr, "Could not get photometric. "
					"Image is CCITT compressed, assuming min-is-white");
			photometric = PHOTOMETRIC_MINISWHITE;
		} else {
			fprintf(stderr, "Could not get photometric. "
					"Assuming min-is-black");

			photometric = PHOTOMETRIC_MINISBLACK;
		}
	}
	save_transparent = FALSE;

	/* test if the extrasample represents an associated alpha channel... */
	if (extra > 0 && (extra_types[0] == EXTRASAMPLE_ASSOCALPHA)) {
		has_alpha = TRUE;
		save_transparent = FALSE;
		--extra;
	} else if (extra > 0 && (extra_types[0] == EXTRASAMPLE_UNASSALPHA)) {
		has_alpha = TRUE;
		save_transparent = TRUE;
		--extra;
	} else if (extra > 0 && (extra_types[0] == EXTRASAMPLE_UNSPECIFIED)) {
		/* assuming unassociated alpha if unspecified */
		fprintf(stderr, "alpha channel type not defined, assuming alpha is not premultiplied");
		has_alpha = TRUE;
		save_transparent = TRUE;
		--extra;
	} else {
		has_alpha = FALSE;
	}

	if (photometric == PHOTOMETRIC_RGB && spp > 3 + extra) {
		has_alpha = TRUE;
		extra = spp - 4;
	} else if (photometric != PHOTOMETRIC_RGB && spp > 1 + extra) {
		has_alpha = TRUE;
		extra = spp - 2;
	}

	is_bw = FALSE;
	is_gray = FALSE;

	switch (photometric) {
		case PHOTOMETRIC_MINISBLACK:
		case PHOTOMETRIC_MINISWHITE:
			if (!has_alpha && bps == 1 && spp == 1) {
				image_type = GD_INDEXED;
				is_bw = TRUE;
			} else {
				image_type = GD_GRAY;
			}
			break;

		case PHOTOMETRIC_RGB:
			image_type = GD_RGB;
			break;

		case PHOTOMETRIC_PALETTE:
			image_type = GD_INDEXED;
			break;

		default:
			force_rgba = TRUE;
			break;
	}

	if (!TIFFGetField (tif, TIFFTAG_PLANARCONFIG, &planar)) {
		planar = PLANARCONFIG_CONTIG;
	}

	/* Force rgba if image plans are not contiguous */
	if (force_rgba || planar != PLANARCONFIG_CONTIG){
		image_type = GD_RGB;
	}

	if (!force_rgba &&
			(image_type == GD_PALETTE || image_type == GD_INDEXED || image_type == GD_GRAY)) {
		im = gdImageCreate(width, height);
		if (!im) goto error;
		readTiffColorMap(im, tif, is_bw, photometric);
	} else {
		im = gdImageCreateTrueColor(width, height);
		if (!im) goto error;
	}

#ifdef DEBUG
printf("force rgba: %i\n", force_rgba);
printf("has_alpha: %i\n", has_alpha);
printf("save trans: %i\n", save_transparent);
printf("is_bw: %i\n", is_bw);
printf("is_gray: %i\n", is_gray);
printf("type: %i\n", image_type);
#endif

	if (force_rgba) {
		ret = createFromTiffRgba(tif, im);
	} else if (TIFFIsTiled(tif)) {
		ret = createFromTiffTiles(tif, im, bps, photometric, has_alpha, is_bw, extra);
	} else {
		ret = createFromTiffLines(tif, im, bps, photometric, has_alpha, is_bw, extra);
	}

	if (!ret) {
		gdImageDestroy(im);
		im = NULL;
		goto error;
	}

	if (TIFFGetField (tif, TIFFTAG_ORIENTATION, &orientation)) {
		switch (orientation) {
			case ORIENTATION_TOPLEFT:
				flip_horizontal = FALSE;
				flip_vertical   = FALSE;
				break;

			case ORIENTATION_TOPRIGHT:
				flip_horizontal = TRUE;
				flip_vertical   = FALSE;
				break;

			case ORIENTATION_BOTRIGHT:
				flip_horizontal = TRUE;
				flip_vertical   = TRUE;
				break;

			case ORIENTATION_BOTLEFT:
				flip_horizontal = FALSE;
				flip_vertical   = TRUE;
				break;

			default:
				flip_horizontal = FALSE;
				flip_vertical   = FALSE;
				fprintf (stderr, "Orientation %d not handled yet!", orientation);
				break;
		}
	}
error:
	TIFFClose(tif);
	return im;
}

/* gdImageCreateFromTIFF
** ---------------------
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromTiff(FILE *inFile)
{
	gdImagePtr im;
	gdIOCtx *in = gdNewFileCtx(inFile);
	if (in == NULL) return NULL;
	im = gdImageCreateFromTiffCtx(in);
	in->gd_free(in);
	return im;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromTiffPtr(int size, void *data)
{
	gdImagePtr im;
	gdIOCtx *in = gdNewDynamicCtxEx (size, data, 0);
	if (in == NULL) return NULL;
	im = gdImageCreateFromTiffCtx(in);
	in->gd_free(in);
	return im;
}

/* gdImageTIFF
** -----------
 */
BGD_DECLARE(void) gdImageTiff(gdImagePtr im, FILE *outFile)
{
	gdIOCtx *out = gdNewFileCtx(outFile);
	if (out == NULL) return;
	gdImageTiffCtx(im, out); /* what's an fg again? */
	out->gd_free(out);
}

/* gdImageTIFFPtr
** --------------
 */
BGD_DECLARE(void *) gdImageTiffPtr(gdImagePtr im, int *size)
{
	void *rv;
	gdIOCtx *out = gdNewDynamicCtx (2048, NULL);
	if (out == NULL) return NULL;
	gdImageTiffCtx(im, out); /* what's an fg again? */
	rv = gdDPExtractData(out, size);
	out->gd_free(out);
	return rv;
}

#endif
