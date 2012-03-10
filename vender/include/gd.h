#ifdef __cplusplus
extern "C" {
#endif

#ifndef GD_H
#define GD_H 1

#define GD_MAJOR_VERSION 2
#define GD_MINOR_VERSION 1
#define GD_RELEASE_VERSION 0
#define GD_EXTRA_VERSION "alpha"
#define GD_VERSION_STRING "2.1.0-alpha"

/* Do the DLL dance: dllexport when building the DLL,
	dllimport when importing from it, nothing when
	not on Silly Silly Windows (tm Aardman Productions). */

/* 2.0.20: for headers */

/* 2.0.24: __stdcall also needed for Visual BASIC
	and other languages. This breaks ABI compatibility
	with previous DLL revs, but it's necessary. */

/* 2.0.29: WIN32 programmers can declare the NONDLL macro if they
	wish to build gd as a static library or by directly including
	the gd sources in a project. */

#define NONDLL 1

#ifdef NONDLL
#define BGD_DECLARE(rt) extern rt
#else
#ifdef BGDWIN32

#define BGD_DECLARE(rt) __declspec(dllexport) rt __stdcall
#else
#define BGD_DECLARE(rt) __declspec(dllimport) rt _stdcall
#endif /* BGDWIN32 */
#endif /* NONDLL */

/* 2.0.20: for actual storage of exported data, functions don't need this,
  currently needed only for font pointers */
#ifdef NONDLL
/* 2.0.25: bring back extern */
#define BGD_EXPORT_DATA_PROT extern
#define BGD_EXPORT_DATA_IMPL
#else
#ifdef BGDWIN32
#define BGD_EXPORT_DATA_PROT __declspec(dllexport) extern
#define BGD_EXPORT_DATA_IMPL __declspec(dllexport)
#else
#define BGD_EXPORT_DATA_PROT __declspec(dllimport) extern
#define BGD_EXPORT_DATA_IMPL __declspec(dllimport)
#endif /* BGDWIN32 */
#endif /* NONDLL */

#ifdef __cplusplus
extern "C"
{
#endif

/* some might want to set DEFAULT_FONTPATH from configure in config.h */
#ifdef NETWARE
/* default fontpath for netware systems, could probably be handled in configure for 2.1 */
#define DEFAULT_FONTPATH "sys:/java/nwgfx/lib/x11/fonts/ttf;."
#define PATHSEPARATOR ";"
#endif

/* 2.0.23: more Type 1 fonts */
#ifndef DEFAULT_FONTPATH
/* default fontpath for unix systems  - whatever happened to standards ! */
#define DEFAULT_FONTPATH "/usr/X11R6/lib/X11/fonts/TrueType:/usr/X11R6/lib/X11/fonts/truetype:/usr/X11R6/lib/X11/fonts/TTF:/usr/share/fonts/TrueType:/usr/share/fonts/truetype:/usr/openwin/lib/X11/fonts/TrueType:/usr/X11R6/lib/X11/fonts/Type1:/usr/lib/X11/fonts/Type1:/usr/openwin/lib/X11/fonts/Type1"
#endif
#ifndef PATHSEPARATOR
#define PATHSEPARATOR ":"
#endif

/* gd.h: declarations file for the graphic-draw module.
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  This software is provided "AS IS." Thomas Boutell and
 * Boutell.Com, Inc. disclaim all warranties, either express or implied,
 * including but not limited to implied warranties of merchantability and
 * fitness for a particular purpose, with respect to this code and accompanying
 * documentation. */

/* stdio is needed for file I/O. */
#include <stdio.h>
#include "gd_io.h"

/* The maximum number of palette entries in palette-based images.
	In the wonderful new world of gd 2.0, you can of course have
	many more colors when using truecolor mode. */

#define gdMaxColors 256

/* Image type. See functions below; you will not need to change
	the elements directly. Use the provided macros to
	access sx, sy, the color table, and colorsTotal for
	read-only purposes. */

/* If 'truecolor' is set true, the image is truecolor;
	pixels are represented by integers, which
	must be 32 bits wide or more.

	True colors are repsented as follows:

	ARGB

	Where 'A' (alpha channel) occupies only the
	LOWER 7 BITS of the MSB. This very small
	loss of alpha channel resolution allows gd 2.x
	to keep backwards compatibility by allowing
	signed integers to be used to represent colors,
	and negative numbers to represent special cases,
	just as in gd 1.x. */

#define gdAlphaMax 127
#define gdAlphaOpaque 0
#define gdAlphaTransparent 127
#define gdRedMax 255
#define gdGreenMax 255
#define gdBlueMax 255
#define gdTrueColorGetAlpha(c) (((c) & 0x7F000000) >> 24)
#define gdTrueColorGetRed(c) (((c) & 0xFF0000) >> 16)
#define gdTrueColorGetGreen(c) (((c) & 0x00FF00) >> 8)
#define gdTrueColorGetBlue(c) ((c) & 0x0000FF)

enum gdCropMode {
	GD_CROP_DEFAULT = 0,
	GD_CROP_TRANSPARENT,
	GD_CROP_BLACK,
	GD_CROP_WHITE,
	GD_CROP_SIDES
};


/* This function accepts truecolor pixel values only. The
	source color is composited with the destination color
	based on the alpha channel value of the source color.
	The resulting color is opaque. */

BGD_DECLARE(int) gdAlphaBlend (int dest, int src);

  typedef struct gdImageStruct
  {
    /* Palette-based image pixels */
    unsigned char **pixels;
    int sx;
    int sy;
    /* These are valid in palette images only. See also
       'alpha', which appears later in the structure to
       preserve binary backwards compatibility */
    int colorsTotal;
    int red[gdMaxColors];
    int green[gdMaxColors];
    int blue[gdMaxColors];
    int open[gdMaxColors];
    /* For backwards compatibility, this is set to the
       first palette entry with 100% transparency,
       and is also set and reset by the
       gdImageColorTransparent function. Newer
       applications can allocate palette entries
       with any desired level of transparency; however,
       bear in mind that many viewers, notably
       many web browsers, fail to implement
       full alpha channel for PNG and provide
       support for full opacity or transparency only. */
    int transparent;
    int *polyInts;
    int polyAllocated;
    struct gdImageStruct *brush;
    struct gdImageStruct *tile;
    int brushColorMap[gdMaxColors];
    int tileColorMap[gdMaxColors];
    int styleLength;
    int stylePos;
    int *style;
    int interlace;
    /* New in 2.0: thickness of line. Initialized to 1. */
    int thick;
    /* New in 2.0: alpha channel for palettes. Note that only
       Macintosh Internet Explorer and (possibly) Netscape 6
       really support multiple levels of transparency in
       palettes, to my knowledge, as of 2/15/01. Most
       common browsers will display 100% opaque and
       100% transparent correctly, and do something
       unpredictable and/or undesirable for levels
       in between. TBB */
    int alpha[gdMaxColors];
    /* Truecolor flag and pixels. New 2.0 fields appear here at the
       end to minimize breakage of existing object code. */
    int trueColor;
    int **tpixels;
    /* Should alpha channel be copied, or applied, each time a
       pixel is drawn? This applies to truecolor images only.
       No attempt is made to alpha-blend in palette images,
       even if semitransparent palette entries exist.
       To do that, build your image as a truecolor image,
       then quantize down to 8 bits. */
    int alphaBlendingFlag;
    /* Should the alpha channel of the image be saved? This affects
       PNG at the moment; other future formats may also
       have that capability. JPEG doesn't. */
    int saveAlphaFlag;

    /* There should NEVER BE ACCESSOR MACROS FOR ITEMS BELOW HERE, so this
       part of the structure can be safely changed in new releases. */

    /* 2.0.12: anti-aliased globals. 2.0.26: just a few vestiges after
      switching to the fast, memory-cheap implementation from PHP-gd. */
    int AA;
    int AA_color;
    int AA_dont_blend;

    /* 2.0.12: simple clipping rectangle. These values
      must be checked for safety when set; please use
      gdImageSetClip */
    int cx1;
    int cy1;
    int cx2;
    int cy2;

    /* 2.1.0: allows to specify resolution in dpi */
    unsigned int res_x;
    unsigned int res_y;
  }
  gdImage;

  typedef gdImage *gdImagePtr;

  typedef struct
  {
    /* # of characters in font */
    int nchars;
    /* First character is numbered... (usually 32 = space) */
    int offset;
    /* Character width and height */
    int w;
    int h;
    /* Font data; array of characters, one row after another.
       Easily included in code, also easily loaded from
       data files. */
    char *data;
  }
  gdFont;

/* Text functions take these. */
  typedef gdFont *gdFontPtr;

/* For backwards compatibility only. Use gdImageSetStyle()
	for MUCH more flexible line drawing. Also see
	gdImageSetBrush(). */
#define gdDashSize 4

/* Special colors. */

#define gdStyled (-2)
#define gdBrushed (-3)
#define gdStyledBrushed (-4)
#define gdTiled (-5)

/* NOT the same as the transparent color index.
	This is used in line styles only. */
#define gdTransparent (-6)

#define gdAntiAliased (-7)

/* Functions to manipulate images. */

/* Creates a palette-based image (up to 256 colors). */
BGD_DECLARE(gdImagePtr) gdImageCreate (int sx, int sy);

/* An alternate name for the above (2.0). */
#define gdImageCreatePalette gdImageCreate

/* Creates a truecolor image (millions of colors). */
BGD_DECLARE(gdImagePtr) gdImageCreateTrueColor (int sx, int sy);

/* Creates an image from various file types. These functions
	return a palette or truecolor image based on the
	nature of the file being loaded. Truecolor PNG
	stays truecolor; palette PNG stays palette-based;
	JPEG is always truecolor. */
BGD_DECLARE(gdImagePtr) gdImageCreateFromPng (FILE * fd);
BGD_DECLARE(gdImagePtr) gdImageCreateFromPngCtx (gdIOCtxPtr in);
BGD_DECLARE(gdImagePtr) gdImageCreateFromPngPtr (int size, void *data);

/* These read the first frame only */
BGD_DECLARE(gdImagePtr) gdImageCreateFromGif (FILE * fd);
BGD_DECLARE(gdImagePtr) gdImageCreateFromGifCtx (gdIOCtxPtr in);
BGD_DECLARE(gdImagePtr) gdImageCreateFromGifPtr (int size, void *data);
BGD_DECLARE(gdImagePtr) gdImageCreateFromWBMP (FILE * inFile);
BGD_DECLARE(gdImagePtr) gdImageCreateFromWBMPCtx (gdIOCtx * infile);
BGD_DECLARE(gdImagePtr) gdImageCreateFromWBMPPtr (int size, void *data);
BGD_DECLARE(gdImagePtr) gdImageCreateFromJpeg (FILE * infile);
BGD_DECLARE(gdImagePtr) gdImageCreateFromJpegCtx (gdIOCtx * infile);
BGD_DECLARE(gdImagePtr) gdImageCreateFromJpegPtr (int size, void *data);

BGD_DECLARE(gdImagePtr) gdImageCreateFromTiff(FILE *inFile);
BGD_DECLARE(gdImagePtr) gdImageCreateFromTiffCtx(gdIOCtx *infile);
BGD_DECLARE(gdImagePtr) gdImageCreateFromTiffPtr(int size, void *data);

BGD_DECLARE(gdImagePtr) gdImageCreateFromTga( FILE * fp );
BGD_DECLARE(gdImagePtr) gdImageCreateFromTgaCtx(gdIOCtx* ctx);
BGD_DECLARE(gdImagePtr) gdImageCreateFromTgaPtr(int size, void *data);

BGD_DECLARE(gdImagePtr) gdImageCreateFromBmp (FILE * inFile);
BGD_DECLARE(gdImagePtr) gdImageCreateFromBmpPtr (int size, void *data);
BGD_DECLARE(gdImagePtr) gdImageCreateFromBmpCtx (gdIOCtxPtr infile);

/* A custom data source. */
/* The source function must return -1 on error, otherwise the number
        of bytes fetched. 0 is EOF, not an error! */
/* context will be passed to your source function. */

  typedef struct
  {
    int (*source) (void *context, char *buffer, int len);
    void *context;
  }
  gdSource, *gdSourcePtr;

   /* Deprecated in favor of gdImageCreateFromPngCtx */
BGD_DECLARE(gdImagePtr) gdImageCreateFromPngSource (gdSourcePtr in);

BGD_DECLARE(gdImagePtr) gdImageCreateFromGd (FILE * in);
BGD_DECLARE(gdImagePtr) gdImageCreateFromGdCtx (gdIOCtxPtr in);
BGD_DECLARE(gdImagePtr) gdImageCreateFromGdPtr (int size, void *data);

BGD_DECLARE(gdImagePtr) gdImageCreateFromGd2 (FILE * in);
BGD_DECLARE(gdImagePtr) gdImageCreateFromGd2Ctx (gdIOCtxPtr in);
BGD_DECLARE(gdImagePtr) gdImageCreateFromGd2Ptr (int size, void *data);

BGD_DECLARE(gdImagePtr) gdImageCreateFromGd2Part (FILE * in, int srcx, int srcy, int w,
				       int h);
BGD_DECLARE(gdImagePtr) gdImageCreateFromGd2PartCtx (gdIOCtxPtr in, int srcx, int srcy,
					  int w, int h);
BGD_DECLARE(gdImagePtr) gdImageCreateFromGd2PartPtr (int size, void *data, int srcx, int srcy,
					  int w, int h);
  /* 2.0.10: prototype was missing */
BGD_DECLARE(gdImagePtr) gdImageCreateFromXbm (FILE * in);

  /* NOTE: filename, not FILE */
BGD_DECLARE(gdImagePtr) gdImageCreateFromXpm (char *filename);

BGD_DECLARE(void) gdImageDestroy (gdImagePtr im);

/* Replaces or blends with the background depending on the
	most recent call to gdImageAlphaBlending and the
	alpha channel value of 'color'; default is to overwrite.
	Tiling and line styling are also implemented
	here. All other gd drawing functions pass through this call,
	allowing for many useful effects. */

BGD_DECLARE(void) gdImageSetPixel (gdImagePtr im, int x, int y, int color);
/* FreeType 2 text output with hook to extra flags */

BGD_DECLARE(int) gdImageGetPixel (gdImagePtr im, int x, int y);
BGD_DECLARE(int) gdImageGetTrueColorPixel (gdImagePtr im, int x, int y);

BGD_DECLARE(void) gdImageAABlend (gdImagePtr im);

BGD_DECLARE(void) gdImageLine (gdImagePtr im, int x1, int y1, int x2, int y2, int color);

/* For backwards compatibility only. Use gdImageSetStyle()
	for much more flexible line drawing. */
BGD_DECLARE(void) gdImageDashedLine (gdImagePtr im, int x1, int y1, int x2, int y2,
			  int color);
/* Corners specified (not width and height). Upper left first, lower right
 	second. */
BGD_DECLARE(void) gdImageRectangle (gdImagePtr im, int x1, int y1, int x2, int y2,
			 int color);
/* Solid bar. Upper left corner first, lower right corner second. */
BGD_DECLARE(void) gdImageFilledRectangle (gdImagePtr im, int x1, int y1, int x2, int y2,
			       int color);
BGD_DECLARE(void) gdImageSetClip(gdImagePtr im, int x1, int y1, int x2, int y2);
BGD_DECLARE(void) gdImageGetClip(gdImagePtr im, int *x1P, int *y1P, int *x2P, int *y2P);
BGD_DECLARE(void) gdImageSetResolution(gdImagePtr im, const unsigned int res_x, const unsigned int res_y);
BGD_DECLARE(int) gdImageBoundsSafe (gdImagePtr im, int x, int y);
BGD_DECLARE(void) gdImageChar (gdImagePtr im, gdFontPtr f, int x, int y, int c,
		    int color);
BGD_DECLARE(void) gdImageCharUp (gdImagePtr im, gdFontPtr f, int x, int y, int c,
		      int color);
BGD_DECLARE(void) gdImageString (gdImagePtr im, gdFontPtr f, int x, int y,
		      unsigned char *s, int color);
BGD_DECLARE(void) gdImageStringUp (gdImagePtr im, gdFontPtr f, int x, int y,
			unsigned char *s, int color);
BGD_DECLARE(void) gdImageString16 (gdImagePtr im, gdFontPtr f, int x, int y,
			unsigned short *s, int color);
BGD_DECLARE(void) gdImageStringUp16 (gdImagePtr im, gdFontPtr f, int x, int y,
			  unsigned short *s, int color);

/* 2.0.16: for thread-safe use of gdImageStringFT and friends,
  call this before allowing any thread to call gdImageStringFT.
  Otherwise it is invoked by the first thread to invoke
  gdImageStringFT, with a very small but real risk of a race condition.
  Return 0 on success, nonzero on failure to initialize freetype. */
BGD_DECLARE(int) gdFontCacheSetup (void);

/* Optional: clean up after application is done using fonts in
BGD_DECLARE( )
 gdImageStringFT(). */
BGD_DECLARE(void) gdFontCacheShutdown (void);
/* 2.0.20: for backwards compatibility. A few applications did start calling
 this function when it first appeared although it was never documented.
 Simply invokes gdFontCacheShutdown. */
BGD_DECLARE(void) gdFreeFontCache (void);

/* Calls gdImageStringFT. Provided for backwards compatibility only. */
BGD_DECLARE(char *) gdImageStringTTF (gdImage * im, int *brect, int fg, char *fontlist,
			  double ptsize, double angle, int x, int y,
			  char *string);

/* FreeType 2 text output */
BGD_DECLARE(char *) gdImageStringFT (gdImage * im, int *brect, int fg, char *fontlist,
			 double ptsize, double angle, int x, int y,
			 char *string);

/* 2.0.5: provides an extensible way to pass additional parameters.
	Thanks to Wez Furlong, sorry for the delay. */

  typedef struct
  {
    int flags;			/* Logical OR of gdFTEX_ values */
    double linespacing;		/* fine tune line spacing for '\n' */
    int charmap;		/* TBB: 2.0.12: may be gdFTEX_Unicode,
				   gdFTEX_Shift_JIS, gdFTEX_Big5,
				   or gdFTEX_Adobe_Custom;
				   when not specified, maps are searched
				   for in the above order. */
    int hdpi;                   /* if (flags & gdFTEX_RESOLUTION) */
    int vdpi;			/* if (flags & gdFTEX_RESOLUTION) */
    char *xshow;	/* if (flags & gdFTEX_XSHOW)
			   then, on return, xshow is a malloc'ed
			   string containing xshow position data for
			   the last string.

			   NB. The caller is responsible for gdFree'ing
			   the xshow string.
			 */
    char *fontpath;	/* if (flags & gdFTEX_RETURNFONTPATHNAME)
                           then, on return, fontpath is a malloc'ed
                           string containing the actual font file path name
                           used, which can be interesting when fontconfig
                           is in use.

                           The caller is responsible for gdFree'ing the
                           fontpath string.
			*/

  }
  gdFTStringExtra, *gdFTStringExtraPtr;

#define gdFTEX_LINESPACE 1
#define gdFTEX_CHARMAP 2
#define gdFTEX_RESOLUTION 4
#define gdFTEX_DISABLE_KERNING 8
#define gdFTEX_XSHOW 16
/* The default unless gdFTUseFontConfig(1); has been called:
  fontlist is a full or partial font file pathname or list thereof
  (i.e. just like before 2.0.29) */
#define gdFTEX_FONTPATHNAME 32
/* Necessary to use fontconfig patterns instead of font pathnames
  as the fontlist argument, unless gdFTUseFontConfig(1); has
  been called. New in 2.0.29 */
#define gdFTEX_FONTCONFIG 64
/* Sometimes interesting when fontconfig is used: the fontpath
  element of the structure above will contain a gdMalloc'd string
  copy of the actual font file pathname used, if this flag is set
   when the call is made */
#define gdFTEX_RETURNFONTPATHNAME 128

/* If flag is nonzero, the fontlist parameter to gdImageStringFT
  and gdImageStringFTEx shall be assumed to be a fontconfig font pattern
  if fontconfig was compiled into gd. This function returns zero
  if fontconfig is not available, nonzero otherwise. */
BGD_DECLARE(int) gdFTUseFontConfig(int flag);

/* These are NOT flags; set one in 'charmap' if you set the
	gdFTEX_CHARMAP bit in 'flags'. */
#define gdFTEX_Unicode 0
#define gdFTEX_Shift_JIS 1
#define gdFTEX_Big5 2
#define gdFTEX_Adobe_Custom 3

BGD_DECLARE(char *) gdImageStringFTEx (gdImage * im, int *brect, int fg, char *fontlist,
			   double ptsize, double angle, int x, int y,
			   char *string, gdFTStringExtraPtr strex);

/* Point type for use in polygon drawing. */
typedef struct
{
	int x, y;
}
gdPoint, *gdPointPtr;

typedef struct
{
	int x, y;
	int width, height;
}
gdRect, *gdRectPtr;


BGD_DECLARE(void) gdImagePolygon (gdImagePtr im, gdPointPtr p, int n, int c);
BGD_DECLARE(void) gdImageOpenPolygon (gdImagePtr im, gdPointPtr p, int n, int c);
BGD_DECLARE(void) gdImageFilledPolygon (gdImagePtr im, gdPointPtr p, int n, int c);

/* These functions still work with truecolor images,
	for which they never return error. */
BGD_DECLARE(int) gdImageColorAllocate (gdImagePtr im, int r, int g, int b);
/* gd 2.0: palette entries with non-opaque transparency are permitted. */
BGD_DECLARE(int) gdImageColorAllocateAlpha (gdImagePtr im, int r, int g, int b, int a);
/* Assumes opaque is the preferred alpha channel value */
BGD_DECLARE(int) gdImageColorClosest (gdImagePtr im, int r, int g, int b);
/* Closest match taking all four parameters into account.
	A slightly different color with the same transparency
	beats the exact same color with radically different
	transparency */
BGD_DECLARE(int) gdImageColorClosestAlpha (gdImagePtr im, int r, int g, int b, int a);
/* An alternate method */
BGD_DECLARE(int) gdImageColorClosestHWB (gdImagePtr im, int r, int g, int b);
/* Returns exact, 100% opaque matches only */
BGD_DECLARE(int) gdImageColorExact (gdImagePtr im, int r, int g, int b);
/* Returns an exact match only, including alpha */
BGD_DECLARE(int) gdImageColorExactAlpha (gdImagePtr im, int r, int g, int b, int a);
/* Opaque only */
BGD_DECLARE(int) gdImageColorResolve (gdImagePtr im, int r, int g, int b);
/* Based on gdImageColorExactAlpha and gdImageColorClosestAlpha */
BGD_DECLARE(int) gdImageColorResolveAlpha (gdImagePtr im, int r, int g, int b, int a);

/* A simpler way to obtain an opaque truecolor value for drawing on a
	truecolor image. Not for use with palette images! */

#define gdTrueColor(r, g, b) (((r) << 16) + \
	((g) << 8) + \
	(b))

/* Returns a truecolor value with an alpha channel component.
	gdAlphaMax (127, **NOT 255**) is transparent, 0 is completely
	opaque. */

#define gdTrueColorAlpha(r, g, b, a) (((a) << 24) + \
	((r) << 16) + \
	((g) << 8) + \
	(b))

BGD_DECLARE(void) gdImageColorDeallocate (gdImagePtr im, int color);

/* Converts a truecolor image to a palette-based image,
	using a high-quality two-pass quantization routine
	which attempts to preserve alpha channel information
	as well as R/G/B color information when creating
	a palette. If ditherFlag is set, the image will be
	dithered to approximate colors better, at the expense
	of some obvious "speckling." colorsWanted can be
	anything up to 256. If the original source image
	includes photographic information or anything that
	came out of a JPEG, 256 is strongly recommended.

	Better yet, don't use these function -- write real
	truecolor PNGs and JPEGs. The disk space gain of
        conversion to palette is not great (for small images
        it can be negative) and the quality loss is ugly.

	DIFFERENCES: gdImageCreatePaletteFromTrueColor creates and
	returns a new image. gdImageTrueColorToPalette modifies
	an existing image, and the truecolor pixels are discarded. */

BGD_DECLARE(gdImagePtr) gdImageCreatePaletteFromTrueColor (gdImagePtr im, int ditherFlag,
				  int colorsWanted);

BGD_DECLARE(void) gdImageTrueColorToPalette (gdImagePtr im, int ditherFlag,
				  int colorsWanted);

/* Specifies a color index (if a palette image) or an
	RGB color (if a truecolor image) which should be
	considered 100% transparent. FOR TRUECOLOR IMAGES,
	THIS IS IGNORED IF AN ALPHA CHANNEL IS BEING
	SAVED. Use gdImageSaveAlpha(im, 0); to
	turn off the saving of a full alpha channel in
	a truecolor image. Note that gdImageColorTransparent
	is usually compatible with older browsers that
	do not understand full alpha channels well. TBB */
BGD_DECLARE(void) gdImageColorTransparent (gdImagePtr im, int color);

BGD_DECLARE(void) gdImagePaletteCopy (gdImagePtr dst, gdImagePtr src);

typedef int (*gdCallbackImageColor)(gdImagePtr im, int src);

BGD_DECLARE(int) gdImageColorReplace(gdImagePtr im, int src, int dst);
BGD_DECLARE(int) gdImageColorReplaceThreshold(gdImagePtr im, int src, int dst, float threshold);
BGD_DECLARE(int) gdImageColorReplaceArray(gdImagePtr im, int len, int *src, int *dst);
BGD_DECLARE(int) gdImageColorReplaceCallback(gdImagePtr im, gdCallbackImageColor callback);

BGD_DECLARE(void) gdImageGif (gdImagePtr im, FILE * out);
BGD_DECLARE(void) gdImagePng (gdImagePtr im, FILE * out);
BGD_DECLARE(void) gdImagePngCtx (gdImagePtr im, gdIOCtx * out);
BGD_DECLARE(void) gdImageGifCtx (gdImagePtr im, gdIOCtx * out);
BGD_DECLARE(void) gdImageTiff(gdImagePtr im, FILE *outFile);
BGD_DECLARE(void *) gdImageTiffPtr(gdImagePtr im, int *size);
BGD_DECLARE(void) gdImageTiffCtx(gdImagePtr image, gdIOCtx *out);

BGD_DECLARE(void *) gdImageBmpPtr(gdImagePtr im, int *size, int compression);
BGD_DECLARE(void) gdImageBmp(gdImagePtr im, FILE *outFile, int compression);
BGD_DECLARE(void) gdImageBmpCtx(gdImagePtr im, gdIOCtxPtr out, int compression);

/* 2.0.12: Compression level: 0-9 or -1, where 0 is NO COMPRESSION at all,
  1 is FASTEST but produces larger files, 9 provides the best
  compression (smallest files) but takes a long time to compress, and
  -1 selects the default compiled into the zlib library. */
BGD_DECLARE(void) gdImagePngEx (gdImagePtr im, FILE * out, int level);
BGD_DECLARE(void) gdImagePngCtxEx (gdImagePtr im, gdIOCtx * out, int level);

BGD_DECLARE(void) gdImageWBMP (gdImagePtr image, int fg, FILE * out);
BGD_DECLARE(void) gdImageWBMPCtx (gdImagePtr image, int fg, gdIOCtx * out);

/* Guaranteed to correctly free memory returned
	by the gdImage*Ptr functions */
BGD_DECLARE(void) gdFree (void *m);

/* Best to free this memory with gdFree(), not free() */
BGD_DECLARE(void *) gdImageWBMPPtr (gdImagePtr im, int *size, int fg);

/* 100 is highest quality (there is always a little loss with JPEG).
	0 is lowest. 10 is about the lowest useful setting. */
BGD_DECLARE(void) gdImageJpeg (gdImagePtr im, FILE * out, int quality);
BGD_DECLARE(void) gdImageJpegCtx (gdImagePtr im, gdIOCtx * out, int quality);

/* Best to free this memory with gdFree(), not free() */
BGD_DECLARE(void *) gdImageJpegPtr (gdImagePtr im, int *size, int quality);

/* Legal values for Disposal. gdDisposalNone is always used by
	the built-in optimizer if previm is passed. */

enum {
	gdDisposalUnknown,
	gdDisposalNone,
	gdDisposalRestoreBackground,
	gdDisposalRestorePrevious
};

BGD_DECLARE(void) gdImageGifAnimBegin(gdImagePtr im, FILE *outFile, int GlobalCM, int Loops);
BGD_DECLARE(void) gdImageGifAnimAdd(gdImagePtr im, FILE *outFile, int LocalCM, int LeftOfs, int TopOfs, int Delay, int Disposal, gdImagePtr previm);
BGD_DECLARE(void) gdImageGifAnimEnd(FILE *outFile);
BGD_DECLARE(void) gdImageGifAnimBeginCtx(gdImagePtr im, gdIOCtx *out, int GlobalCM, int Loops);
BGD_DECLARE(void) gdImageGifAnimAddCtx(gdImagePtr im, gdIOCtx *out, int LocalCM, int LeftOfs, int TopOfs, int Delay, int Disposal, gdImagePtr previm);
BGD_DECLARE(void) gdImageGifAnimEndCtx(gdIOCtx *out);
BGD_DECLARE(void *) gdImageGifAnimBeginPtr(gdImagePtr im, int *size, int GlobalCM, int Loops);
BGD_DECLARE(void *) gdImageGifAnimAddPtr(gdImagePtr im, int *size, int LocalCM, int LeftOfs, int TopOfs, int Delay, int Disposal, gdImagePtr previm);
BGD_DECLARE(void *) gdImageGifAnimEndPtr(int *size);

/* A custom data sink. For backwards compatibility. Use
	gdIOCtx instead. */
/* The sink function must return -1 on error, otherwise the number
        of bytes written, which must be equal to len. */
/* context will be passed to your sink function. */
  typedef struct
  {
    int (*sink) (void *context, const char *buffer, int len);
    void *context;
  }
  gdSink, *gdSinkPtr;

BGD_DECLARE(void) gdImagePngToSink (gdImagePtr im, gdSinkPtr out);

BGD_DECLARE(void) gdImageGd (gdImagePtr im, FILE * out);
BGD_DECLARE(void) gdImageGd2 (gdImagePtr im, FILE * out, int cs, int fmt);

/* Best to free this memory with gdFree(), not free() */
BGD_DECLARE(void *) gdImageGifPtr (gdImagePtr im, int *size);

/* Best to free this memory with gdFree(), not free() */
BGD_DECLARE(void *) gdImagePngPtr (gdImagePtr im, int *size);
BGD_DECLARE(void *) gdImagePngPtrEx (gdImagePtr im, int *size, int level);

/* Best to free this memory with gdFree(), not free() */
BGD_DECLARE(void *) gdImageGdPtr (gdImagePtr im, int *size);

/* Best to free this memory with gdFree(), not free() */
BGD_DECLARE(void *) gdImageGd2Ptr (gdImagePtr im, int cs, int fmt, int *size);

/* Style is a bitwise OR ( | operator ) of these.
	gdArc and gdChord are mutually exclusive;
	gdChord just connects the starting and ending
	angles with a straight line, while gdArc produces
	a rounded edge. gdPie is a synonym for gdArc.
	gdNoFill indicates that the arc or chord should be
	outlined, not filled. gdEdged, used together with
	gdNoFill, indicates that the beginning and ending
	angles should be connected to the center; this is
	a good way to outline (rather than fill) a
	'pie slice'. */
#define gdArc   0
#define gdPie   gdArc
#define gdChord 1
#define gdNoFill 2
#define gdEdged 4

BGD_DECLARE(void) gdImageFilledArc (gdImagePtr im, int cx, int cy, int w, int h, int s,
			 int e, int color, int style);
BGD_DECLARE(void) gdImageArc (gdImagePtr im, int cx, int cy, int w, int h, int s, int e,
		   int color);
BGD_DECLARE(void) gdImageEllipse(gdImagePtr im, int cx, int cy, int w, int h, int color);
BGD_DECLARE(void) gdImageFilledEllipse (gdImagePtr im, int cx, int cy, int w, int h,
			     int color);
BGD_DECLARE(void) gdImageFillToBorder (gdImagePtr im, int x, int y, int border,
			    int color);
BGD_DECLARE(void) gdImageFill (gdImagePtr im, int x, int y, int color);
BGD_DECLARE(void) gdImageCopy (gdImagePtr dst, gdImagePtr src, int dstX, int dstY,
		    int srcX, int srcY, int w, int h);
BGD_DECLARE(void) gdImageCopyMerge (gdImagePtr dst, gdImagePtr src, int dstX, int dstY,
			 int srcX, int srcY, int w, int h, int pct);
BGD_DECLARE(void) gdImageCopyMergeGray (gdImagePtr dst, gdImagePtr src, int dstX,
			     int dstY, int srcX, int srcY, int w, int h,
			     int pct);

/* Stretches or shrinks to fit, as needed. Does NOT attempt
	to average the entire set of source pixels that scale down onto the
	destination pixel. */
BGD_DECLARE(void) gdImageCopyResized (gdImagePtr dst, gdImagePtr src, int dstX, int dstY,
			   int srcX, int srcY, int dstW, int dstH, int srcW,
			   int srcH);

/* gd 2.0: stretches or shrinks to fit, as needed. When called with a
	truecolor destination image, this function averages the
	entire set of source pixels that scale down onto the
	destination pixel, taking into account what portion of the
	destination pixel each source pixel represents. This is a
	floating point operation, but this is not a performance issue
	on modern hardware, except for some embedded devices. If the
	destination is a palette image, gdImageCopyResized is
	substituted automatically. */
BGD_DECLARE(void) gdImageCopyResampled (gdImagePtr dst, gdImagePtr src, int dstX,
			     int dstY, int srcX, int srcY, int dstW, int dstH,
			     int srcW, int srcH);

/* gd 2.0.8: gdImageCopyRotated is added. Source
        is a rectangle, with its upper left corner at
        srcX and srcY. Destination is the *center* of
        the rotated copy. Angle is in degrees, same as
        gdImageArc. Floating point destination center
        coordinates allow accurate rotation of
        objects of odd-numbered width or height. */
BGD_DECLARE(void) gdImageCopyRotated (gdImagePtr dst,
			   gdImagePtr src,
			   double dstX, double dstY,
			   int srcX, int srcY,
			   int srcWidth, int srcHeight, int angle);

BGD_DECLARE(void) gdImageSetBrush (gdImagePtr im, gdImagePtr brush);
BGD_DECLARE(void) gdImageSetTile (gdImagePtr im, gdImagePtr tile);
BGD_DECLARE(void) gdImageSetAntiAliased (gdImagePtr im, int c);
BGD_DECLARE(void) gdImageSetAntiAliasedDontBlend (gdImagePtr im, int c, int dont_blend);
BGD_DECLARE(void) gdImageSetStyle (gdImagePtr im, int *style, int noOfPixels);
/* Line thickness (defaults to 1). Affects lines, ellipses,
	rectangles, polygons and so forth. */
BGD_DECLARE(void) gdImageSetThickness (gdImagePtr im, int thickness);
/* On or off (1 or 0) for all three of these. */
BGD_DECLARE(void) gdImageInterlace (gdImagePtr im, int interlaceArg);
BGD_DECLARE(void) gdImageAlphaBlending (gdImagePtr im, int alphaBlendingArg);
BGD_DECLARE(void) gdImageSaveAlpha (gdImagePtr im, int saveAlphaArg);


BGD_DECLARE(gdImagePtr) gdImageCrop(gdImagePtr src, const gdRect *crop);
BGD_DECLARE(gdImagePtr) gdImageAutoCrop(gdImagePtr im, const unsigned int mode);
BGD_DECLARE(gdImagePtr) gdImageThresholdCrop(gdImagePtr im, const unsigned int color, const float threshold);

BGD_DECLARE(void) gdImageFlipHorizontal(gdImagePtr im);
BGD_DECLARE(void) gdImageFlipVertical(gdImagePtr im);
BGD_DECLARE(void) gdImageFlipBoth(gdImagePtr im);

BGD_DECLARE(gdImagePtr) gdImageNeuQuant(gdImagePtr im, const int max_color, int sample_factor);

enum gdPixelateMode {
	GD_PIXELATE_UPPERLEFT,
	GD_PIXELATE_AVERAGE
};

BGD_DECLARE(int) gdImagePixelate(gdImagePtr im, int block_size, const unsigned int mode);

typedef struct {
	int sub;
	int plus;
	unsigned int num_colors;
	int *colors;
	unsigned int seed;
} gdScatter, *gdScatterPtr;

BGD_DECLARE(int) gdImageScatter(gdImagePtr im, int sub, int plus);
BGD_DECLARE(int) gdImageScatterColor(gdImagePtr im, int sub, int plus, int colors[], unsigned int num_colors);
BGD_DECLARE(int) gdImageScatterEx(gdImagePtr im, gdScatterPtr s);

/* Macros to access information about images. */

/* Returns nonzero if the image is a truecolor image,
	zero for a palette image. */

#define gdImageTrueColor(im) ((im)->trueColor)

#define gdImageSX(im) ((im)->sx)
#define gdImageSY(im) ((im)->sy)
#define gdImageColorsTotal(im) ((im)->colorsTotal)
#define gdImageRed(im, c) ((im)->trueColor ? gdTrueColorGetRed(c) : \
	(im)->red[(c)])
#define gdImageGreen(im, c) ((im)->trueColor ? gdTrueColorGetGreen(c) : \
	(im)->green[(c)])
#define gdImageBlue(im, c) ((im)->trueColor ? gdTrueColorGetBlue(c) : \
	(im)->blue[(c)])
#define gdImageAlpha(im, c) ((im)->trueColor ? gdTrueColorGetAlpha(c) : \
	(im)->alpha[(c)])
#define gdImageGetTransparent(im) ((im)->transparent)
#define gdImageGetInterlaced(im) ((im)->interlace)

/* These macros provide direct access to pixels in
	palette-based and truecolor images, respectively.
	If you use these macros, you must perform your own
	bounds checking. Use of the macro for the correct type
	of image is also your responsibility. */
#define gdImagePalettePixel(im, x, y) (im)->pixels[(y)][(x)]
#define gdImageTrueColorPixel(im, x, y) (im)->tpixels[(y)][(x)]

#define gdImageResolutionX(im) (im)->res_x
#define gdImageResolutionY(im) (im)->res_y

/* I/O Support routines. */

BGD_DECLARE(gdIOCtx *) gdNewFileCtx (FILE *);
  /* If data is null, size is ignored and an initial data buffer is
    allocated automatically. NOTE: this function assumes gd has the right
    to free or reallocate "data" at will! Also note that gd will free
    "data" when the IO context is freed. If data is not null, it must point
    to memory allocated with gdMalloc, or by a call to gdImage[something]Ptr.
    If not, see gdNewDynamicCtxEx for an alternative. */
BGD_DECLARE(gdIOCtx *) gdNewDynamicCtx (int size, void *data);
  /* 2.0.21: if freeFlag is nonzero, gd will free and/or reallocate "data" as
    needed as described above. If freeFlag is zero, gd will never free
    or reallocate "data", which means that the context should only be used
    for *reading* an image from a memory buffer, or writing an image to a
    memory buffer which is already large enough. If the memory buffer is
    not large enough and an image write is attempted, the write operation
    will fail. Those wishing to write an image to a buffer in memory have
    a much simpler alternative in the gdImage[something]Ptr functions. */
BGD_DECLARE(gdIOCtx *) gdNewDynamicCtxEx (int size, void *data, int freeFlag);
BGD_DECLARE(gdIOCtx *) gdNewSSCtx (gdSourcePtr in, gdSinkPtr out);
BGD_DECLARE(void *) gdDPExtractData (struct gdIOCtx *ctx, int *size);

#define GD2_CHUNKSIZE           128
#define GD2_CHUNKSIZE_MIN	64
#define GD2_CHUNKSIZE_MAX       4096

#define GD2_VERS                2
#define GD2_ID                  "gd2"

#define GD2_FMT_RAW             1
#define GD2_FMT_COMPRESSED      2

/* Image comparison definitions */
BGD_DECLARE(int) gdImageCompare (gdImagePtr im1, gdImagePtr im2);

#define GD_CMP_IMAGE		1	/* Actual image IS different */
#define GD_CMP_NUM_COLORS	2	/* Number of Colours in pallette differ */
#define GD_CMP_COLOR		4	/* Image colours differ */
#define GD_CMP_SIZE_X		8	/* Image width differs */
#define GD_CMP_SIZE_Y		16	/* Image heights differ */
#define GD_CMP_TRANSPARENT	32	/* Transparent colour */
#define GD_CMP_BACKGROUND	64	/* Background colour */
#define GD_CMP_INTERLACE	128	/* Interlaced setting */
#define GD_CMP_TRUECOLOR	256	/* Truecolor vs palette differs */

#define GD_RESOLUTION           96	/* dots per inch */

#ifdef __cplusplus
}
#endif

/* newfangled special effects */
#include "gdfx.h"

#endif				/* GD_H */

#ifdef __cplusplus
}
#endif
