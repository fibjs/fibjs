/*
   * gd_gd2.c
   *
   * Implements the I/O and support for the GD2 format.
   *
   * Changing the definition of GD2_DBG (below) will cause copious messages
   * to be displayed while it processes requests.
   *
   * Designed, Written & Copyright 1999, Philip Warner.
   *
 */

#include "config.h"

#include <stdio.h>
/* 2.0.29: no more errno.h, makes windows happy */
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "gd.h"
#include "gdhelpers.h"

/* 2.03: gd2 is no longer mandatory */
/* JCE - test after including gd.h so that HAVE_LIBZ can be set in
 * a config.h file included by gd.h */
#ifdef HAVE_LIBZ
#include <zlib.h>

#define TRUE 1
#define FALSE 0

/* 2.11: not part of the API, as the save routine can figure it out
	from im->trueColor, and the load routine doesn't need to tell
	the end user the saved format. NOTE: adding 2 is assumed
	to result in the correct format value for truecolor! */
#define GD2_FMT_TRUECOLOR_RAW 3
#define GD2_FMT_TRUECOLOR_COMPRESSED 4

#define gd2_compressed(fmt) (((fmt) == GD2_FMT_COMPRESSED) || \
	((fmt) == GD2_FMT_TRUECOLOR_COMPRESSED))

#define gd2_truecolor(fmt) (((fmt) == GD2_FMT_TRUECOLOR_RAW) || \
	((fmt) == GD2_FMT_TRUECOLOR_COMPRESSED))

/* Use this for commenting out debug-print statements. */
/* Just use the first '#define' to allow all the prints... */
/*#define GD2_DBG(s) (s) */
#define GD2_DBG(s)

typedef struct
{
  int offset;
  int size;
}
t_chunk_info;

extern int _gdGetColors (gdIOCtx * in, gdImagePtr im, int gd2xFlag);
extern void _gdPutColors (gdImagePtr im, gdIOCtx * out);

/* */
/* Read the extra info in the gd2 header. */
/* */
static int
_gd2GetHeader (gdIOCtxPtr in, int *sx, int *sy,
	       int *cs, int *vers, int *fmt, int *ncx, int *ncy,
	       t_chunk_info ** chunkIdx)
{
  int i;
  int ch;
  char id[5];
  t_chunk_info *cidx;
  int sidx;
  int nc;

  GD2_DBG (printf ("Reading gd2 header info\n"));

  for (i = 0; i < 4; i++)
    {
      ch = gdGetC (in);
      if (ch == EOF)
	{
	  goto fail1;
	};
      id[i] = ch;
    };
  id[4] = 0;

  GD2_DBG (printf ("Got file code: %s\n", id));

  /* Equiv. of 'magick'.  */
  if (strcmp (id, GD2_ID) != 0)
    {
      GD2_DBG (printf ("Not a valid gd2 file\n"));
      goto fail1;
    };

  /* Version */
  if (gdGetWord (vers, in) != 1)
    {
      goto fail1;
    };
  GD2_DBG (printf ("Version: %d\n", *vers));

  if ((*vers != 1) && (*vers != 2))
    {
      GD2_DBG (printf ("Bad version: %d\n", *vers));
      goto fail1;
    };

  /* Image Size */
  if (!gdGetWord (sx, in))
    {
      GD2_DBG (printf ("Could not get x-size\n"));
      goto fail1;
    }
  if (!gdGetWord (sy, in))
    {
      GD2_DBG (printf ("Could not get y-size\n"));
      goto fail1;
    }
  GD2_DBG (printf ("Image is %dx%d\n", *sx, *sy));

  /* Chunk Size (pixels, not bytes!) */
  if (gdGetWord (cs, in) != 1)
    {
      goto fail1;
    };
  GD2_DBG (printf ("ChunkSize: %d\n", *cs));

  if ((*cs < GD2_CHUNKSIZE_MIN) || (*cs > GD2_CHUNKSIZE_MAX))
    {
      GD2_DBG (printf ("Bad chunk size: %d\n", *cs));
      goto fail1;
    };

  /* Data Format */
  if (gdGetWord (fmt, in) != 1)
    {
      goto fail1;
    };
  GD2_DBG (printf ("Format: %d\n", *fmt));

  if ((*fmt != GD2_FMT_RAW) && (*fmt != GD2_FMT_COMPRESSED) &&
      (*fmt != GD2_FMT_TRUECOLOR_RAW) &&
      (*fmt != GD2_FMT_TRUECOLOR_COMPRESSED))
    {
      GD2_DBG (printf ("Bad data format: %d\n", *fmt));
      goto fail1;
    };


  /* # of chunks wide */
  if (gdGetWord (ncx, in) != 1)
    {
      goto fail1;
    };
  GD2_DBG (printf ("%d Chunks Wide\n", *ncx));

  /* # of chunks high */
  if (gdGetWord (ncy, in) != 1)
    {
      goto fail1;
    };
  GD2_DBG (printf ("%d Chunks vertically\n", *ncy));

  if (gd2_compressed (*fmt))
    {
      nc = (*ncx) * (*ncy);
      GD2_DBG (printf ("Reading %d chunk index entries\n", nc));
      sidx = sizeof (t_chunk_info) * nc;
      cidx = gdCalloc (sidx, 1);
			if (!cidx) {
				goto fail1;
			}
      for (i = 0; i < nc; i++)
	{
	  if (gdGetInt (&cidx[i].offset, in) != 1)
	    {
	      goto fail1;
	    };
	  if (gdGetInt (&cidx[i].size, in) != 1)
	    {
	      goto fail1;
	    };
	};
      *chunkIdx = cidx;
    };

  GD2_DBG (printf ("gd2 header complete\n"));

  return 1;

fail1:
  return 0;
}

static gdImagePtr
_gd2CreateFromFile (gdIOCtxPtr in, int *sx, int *sy,
		    int *cs, int *vers, int *fmt,
		    int *ncx, int *ncy, t_chunk_info ** cidx)
{
  gdImagePtr im;

  if (_gd2GetHeader (in, sx, sy, cs, vers, fmt, ncx, ncy, cidx) != 1)
    {
      GD2_DBG (printf ("Bad GD2 header\n"));
      goto fail1;
    }
  if (gd2_truecolor (*fmt))
    {
      im = gdImageCreateTrueColor (*sx, *sy);
    }
  else
    {
      im = gdImageCreate (*sx, *sy);
    }
  if (im == NULL)
    {
      GD2_DBG (printf ("Could not create gdImage\n"));
      goto fail1;
    };

  if (!_gdGetColors (in, im, (*vers) == 2))
    {
      GD2_DBG (printf ("Could not read color palette\n"));
      goto fail2;
    }
  GD2_DBG (printf ("Image palette completed: %d colours\n", im->colorsTotal));

  return im;

fail2:
  gdImageDestroy (im);
  return 0;

fail1:
  return 0;

}

static int
_gd2ReadChunk (int offset, char *compBuf, int compSize, char *chunkBuf,
	       uLongf * chunkLen, gdIOCtx * in)
{
  int zerr;

  if (gdTell (in) != offset)
    {
      GD2_DBG (printf ("Positioning in file to %d\n", offset));
      gdSeek (in, offset);
    }
  else
    {
      GD2_DBG (printf ("Already Positioned in file to %d\n", offset));
    };

  /* Read and uncompress an entire chunk. */
  GD2_DBG (printf ("Reading file\n"));
  if (gdGetBuf (compBuf, compSize, in) != compSize)
    {
      return FALSE;
    };
  GD2_DBG (printf
	   ("Got %d bytes. Uncompressing into buffer of %d bytes\n", compSize,
	    *chunkLen));
  zerr =
    uncompress ((unsigned char *) chunkBuf, chunkLen,
		(unsigned char *) compBuf, compSize);
  if (zerr != Z_OK)
    {
      GD2_DBG (printf ("Error %d from uncompress\n", zerr));
      return FALSE;
    };
  GD2_DBG (printf ("Got chunk\n"));
  return TRUE;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromGd2 (FILE * inFile)
{
  gdIOCtx *in = gdNewFileCtx (inFile);
  gdImagePtr im;

  if (in == NULL) return NULL;
  im = gdImageCreateFromGd2Ctx (in);

  in->gd_free (in);

  return im;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromGd2Ptr (int size, void *data)
{
  gdImagePtr im;
  gdIOCtx *in = gdNewDynamicCtxEx (size, data, 0);
  if(!in)
  	return 0;
  im = gdImageCreateFromGd2Ctx (in);
  in->gd_free (in);
  return im;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromGd2Ctx (gdIOCtxPtr in)
{
  int sx, sy;
  int i;
  int ncx, ncy, nc, cs, cx, cy;
  int x, y, ylo, yhi, xlo, xhi;
  int vers, fmt;
  t_chunk_info *chunkIdx = NULL;	/* So we can gdFree it with impunity. */
  unsigned char *chunkBuf = NULL;	/* So we can gdFree it with impunity. */
  int chunkNum = 0;
  int chunkMax = 0;
  uLongf chunkLen;
  int chunkPos = 0;
  int compMax = 0;
  int bytesPerPixel;
  char *compBuf = NULL;		/* So we can gdFree it with impunity. */

  gdImagePtr im;

  /* Get the header */
  im =
    _gd2CreateFromFile (in, &sx, &sy, &cs, &vers, &fmt, &ncx, &ncy,
			&chunkIdx);

  if (im == NULL)
    {
      return 0;
    }

  bytesPerPixel = im->trueColor ? 4 : 1;
  nc = ncx * ncy;

  if (gd2_compressed (fmt))
    {
      /* Find the maximum compressed chunk size. */
      compMax = 0;
      for (i = 0; (i < nc); i++)
	{
	  if (chunkIdx[i].size > compMax)
	    {
	      compMax = chunkIdx[i].size;
	    };
	};
      compMax++;

      /* Allocate buffers */
      chunkMax = cs * bytesPerPixel * cs;
      chunkBuf = gdCalloc (chunkMax, 1);
			if (!chunkBuf) {
				goto fail;
			}
      compBuf = gdCalloc (compMax, 1);
			if (!compBuf) {
				goto fail;
			}

      GD2_DBG (printf ("Largest compressed chunk is %d bytes\n", compMax));
    };

/*      if ( (ncx != sx / cs) || (ncy != sy / cs)) { */
/*              goto fail2; */
/*      }; */

  /* Read the data... */
  for (cy = 0; (cy < ncy); cy++)
    {
      for (cx = 0; (cx < ncx); cx++)
	{

	  ylo = cy * cs;
	  yhi = ylo + cs;
	  if (yhi > im->sy)
	    {
	      yhi = im->sy;
	    };

	  GD2_DBG (printf
		   ("Processing Chunk %d (%d, %d), y from %d to %d\n",
		    chunkNum, cx, cy, ylo, yhi));

	  if (gd2_compressed (fmt))
	    {

	      chunkLen = chunkMax;

	      if (!_gd2ReadChunk (chunkIdx[chunkNum].offset,
				  compBuf,
				  chunkIdx[chunkNum].size,
				  (char *) chunkBuf, &chunkLen, in))
		{
		  GD2_DBG (printf ("Error reading comproessed chunk\n"));
		  goto fail;
		};

	      chunkPos = 0;
	    };

	  for (y = ylo; (y < yhi); y++)
	    {

	      xlo = cx * cs;
	      xhi = xlo + cs;
	      if (xhi > im->sx)
		{
		  xhi = im->sx;
		};
	      /*GD2_DBG(printf("y=%d: ",y)); */
	      if (!gd2_compressed (fmt))
		{
		  for (x = xlo; x < xhi; x++)
		    {

		      if (im->trueColor)
			{
			  if (!gdGetInt (&im->tpixels[y][x], in))
			    {
			      /*printf("EOF while reading\n"); */
			      /*gdImageDestroy(im); */
			      /*return 0; */
			      im->tpixels[y][x] = 0;
			    }
			}
		      else
			{
			  int ch;
			  if (!gdGetByte (&ch, in))
			    {
			      /*printf("EOF while reading\n"); */
			      /*gdImageDestroy(im); */
			      /*return 0; */
			      ch = 0;
			    }
			  im->pixels[y][x] = ch;
			}
		    }
		}
	      else
		{
		  for (x = xlo; x < xhi; x++)
		    {
		      if (im->trueColor)
			{
			  /* 2.0.1: work around a gcc bug by being verbose.
			     TBB */
			  int a = chunkBuf[chunkPos++] << 24;
			  int r = chunkBuf[chunkPos++] << 16;
			  int g = chunkBuf[chunkPos++] << 8;
			  int b = chunkBuf[chunkPos++];
			  /* 2.0.11: tpixels */
			  im->tpixels[y][x] = a + r + g + b;
			}
		      else
			{
			  im->pixels[y][x] = chunkBuf[chunkPos++];
			}
		    };
		};
	      /*GD2_DBG(printf("\n")); */
	    };
	  chunkNum++;
	};
    };

  GD2_DBG (printf ("Freeing memory\n"));

  gdFree (chunkBuf);
  gdFree (compBuf);
  gdFree (chunkIdx);

  GD2_DBG (printf ("Done\n"));

  return im;

fail:
  gdImageDestroy (im);
	if (chunkBuf) {
		gdFree (chunkBuf);
	}
	if (compBuf) {
		gdFree (compBuf);
	}
	if (chunkIdx) {
		gdFree (chunkIdx);
	}
  return 0;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromGd2Part (FILE * inFile, int srcx, int srcy, int w, int h)
{
  gdImagePtr im;
  gdIOCtx *in = gdNewFileCtx (inFile);

  if (in == NULL) return NULL;
  im = gdImageCreateFromGd2PartCtx (in, srcx, srcy, w, h);

  in->gd_free (in);

  return im;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromGd2PartPtr (int size, void *data, int srcx, int srcy, int w,
			     int h)
{
  gdImagePtr im;
  gdIOCtx *in = gdNewDynamicCtxEx (size, data, 0);
  if(!in)
  	return 0;
  im = gdImageCreateFromGd2PartCtx (in, srcx, srcy, w, h);
  in->gd_free (in);
  return im;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromGd2PartCtx (gdIOCtx * in, int srcx, int srcy, int w, int h)
{
  int scx, scy, ecx, ecy, fsx, fsy;
  int nc, ncx, ncy, cs, cx, cy;
  int x, y, ylo, yhi, xlo, xhi;
  int dstart, dpos;
  int i;
  /* 2.0.12: unsigned is correct; fixes problems with color munging.
     Thanks to Steven Brown. */
  unsigned int ch;
  int vers, fmt;
  t_chunk_info *chunkIdx = NULL;
  unsigned char *chunkBuf = NULL;
  int chunkNum;
  int chunkMax = 0;
  uLongf chunkLen;
  int chunkPos = 0;
  int compMax;
  char *compBuf = NULL;

  gdImagePtr im;

  /* */
  /* The next few lines are basically copied from gd2CreateFromFile */
  /* - we change the file size, so don't want to use the code directly. */
  /*   but we do need to know the file size. */
  /* */
  if (_gd2GetHeader (in, &fsx, &fsy, &cs, &vers, &fmt, &ncx, &ncy, &chunkIdx)
      != 1)
    {
      goto fail1;
    }

  GD2_DBG (printf ("File size is %dx%d\n", fsx, fsy));

  /* This is the difference - make a file based on size of chunks. */
  if (gd2_truecolor (fmt))
    {
      im = gdImageCreateTrueColor (w, h);
    }
  else
    {
      im = gdImageCreate (w, h);
    }
  if (im == NULL)
    {
      goto fail1;
    };

  if (!_gdGetColors (in, im, vers == 2))
    {
      goto fail2;
    }
  GD2_DBG (printf ("Image palette completed: %d colours\n", im->colorsTotal));

  /* Process the header info */
  nc = ncx * ncy;

  if (gd2_compressed (fmt))
    {
      /* Find the maximum compressed chunk size. */
      compMax = 0;
      for (i = 0; (i < nc); i++)
	{
	  if (chunkIdx[i].size > compMax)
	    {
	      compMax = chunkIdx[i].size;
	    };
	};
      compMax++;

      if (im->trueColor)
	{
	  chunkMax = cs * cs * 4;
	}
      else
	{
	  chunkMax = cs * cs;
	}
      chunkBuf = gdCalloc (chunkMax, 1);
			if (!chunkBuf) {
				goto fail2;
			}
      compBuf = gdCalloc (compMax, 1);
			if (!compBuf) {
				goto fail2;
			}

    };

/*      Don't bother with this... */
/*      if ( (ncx != sx / cs) || (ncy != sy / cs)) { */
/*              goto fail2; */
/*      }; */


  /* Work out start/end chunks */
  scx = srcx / cs;
  scy = srcy / cs;
  if (scx < 0)
    {
      scx = 0;
    };
  if (scy < 0)
    {
      scy = 0;
    };

  ecx = (srcx + w) / cs;
  ecy = (srcy + h) / cs;
  if (ecx >= ncx)
    {
      ecx = ncx - 1;
    };
  if (ecy >= ncy)
    {
      ecy = ncy - 1;
    };

  /* Remember file position of image data. */
  dstart = gdTell (in);
  GD2_DBG (printf ("Data starts at %d\n", dstart));

  /* Loop through the chunks. */
  for (cy = scy; (cy <= ecy); cy++)
    {

      ylo = cy * cs;
      yhi = ylo + cs;
      if (yhi > fsy)
	{
	  yhi = fsy;
	};

      for (cx = scx; (cx <= ecx); cx++)
	{

	  xlo = cx * cs;
	  xhi = xlo + cs;
	  if (xhi > fsx)
	    {
	      xhi = fsx;
	    };

	  GD2_DBG (printf
		   ("Processing Chunk (%d, %d), from %d to %d\n", cx, cy, ylo,
		    yhi));

	  if (!gd2_compressed (fmt))
	    {
	      GD2_DBG (printf ("Using raw format data\n"));
	      if (im->trueColor)
		{
		  dpos =
		    (cy * (cs * fsx) * 4 + cx * cs * (yhi - ylo) * 4) +
		    dstart;
		}
	      else
		{
		  dpos = cy * (cs * fsx) + cx * cs * (yhi - ylo) + dstart;
		}
	      /* gd 2.0.11: gdSeek returns TRUE on success, not 0.
	         Longstanding bug. 01/16/03 */
	      if (!gdSeek (in, dpos))
		{
		  fprintf (stderr, "Seek error\n");
		  goto fail2;
		};
	      GD2_DBG (printf
		       ("Reading (%d, %d) from position %d\n", cx, cy,
			dpos - dstart));
	    }
	  else
	    {
	      chunkNum = cx + cy * ncx;

	      chunkLen = chunkMax;
	      if (!_gd2ReadChunk (chunkIdx[chunkNum].offset,
				  compBuf,
				  chunkIdx[chunkNum].size,
				  (char *) chunkBuf, &chunkLen, in))
		{
		  printf ("Error reading comproessed chunk\n");
		  goto fail2;
		};
	      chunkPos = 0;
	      GD2_DBG (printf
		       ("Reading (%d, %d) from chunk %d\n", cx, cy,
			chunkNum));
	    };

	  GD2_DBG (printf
		   ("   into (%d, %d) - (%d, %d)\n", xlo, ylo, xhi, yhi));
	  for (y = ylo; (y < yhi); y++)
	    {

	      for (x = xlo; x < xhi; x++)
		{
		  if (!gd2_compressed (fmt))
		    {
		      if (im->trueColor)
			{
			  if (!gdGetInt ((int *) &ch, in))
			    {
			      ch = 0;
			      /*printf("EOF while reading file\n"); */
			      /*goto fail2; */
			    }
			}
		      else
			{
			  ch = gdGetC (in);
			  if ((int) ch == EOF)
			    {
			      ch = 0;
			      /*printf("EOF while reading file\n"); */
			      /*goto fail2; */
			    }
			}
		    }
		  else
		    {
		      if (im->trueColor)
			{
			  ch = chunkBuf[chunkPos++];
			  ch = (ch << 8) + chunkBuf[chunkPos++];
			  ch = (ch << 8) + chunkBuf[chunkPos++];
			  ch = (ch << 8) + chunkBuf[chunkPos++];
			}
		      else
			{
			  ch = chunkBuf[chunkPos++];
			}
		    };

		  /* Only use a point that is in the image. */
		  if ((x >= srcx) && (x < (srcx + w)) && (x < fsx) && (x >= 0)
		      && (y >= srcy) && (y < (srcy + h)) && (y < fsy)
		      && (y >= 0))
		    {
		      /* 2.0.11: tpixels */
		      if (im->trueColor)
			{
			  im->tpixels[y - srcy][x - srcx] = ch;
			}
		      else
			{
			  im->pixels[y - srcy][x - srcx] = ch;
			}
		    }
		};
	    };
	};
    };

  gdFree (chunkBuf);
  gdFree (compBuf);
  gdFree (chunkIdx);

  return im;

fail2:
  gdImageDestroy (im);
fail1:
	if (chunkBuf) {
	  gdFree (chunkBuf);
	}
	if (compBuf) {
	  gdFree (compBuf);
	}
	if (chunkIdx) {
  	gdFree (chunkIdx);
	}
  return 0;

}

static void
_gd2PutHeader (gdImagePtr im, gdIOCtx * out, int cs, int fmt, int cx, int cy)
{
  int i;

  /* Send the gd2 id, to verify file format. */
  for (i = 0; i < 4; i++)
    {
      gdPutC ((unsigned char) (GD2_ID[i]), out);
    };

  /* */
  /* We put the version info first, so future versions can easily change header info. */
  /* */
  gdPutWord (GD2_VERS, out);
  gdPutWord (im->sx, out);
  gdPutWord (im->sy, out);
  gdPutWord (cs, out);
  gdPutWord (fmt, out);
  gdPutWord (cx, out);
  gdPutWord (cy, out);

}

static void
_gdImageGd2 (gdImagePtr im, gdIOCtx * out, int cs, int fmt)
{
  int ncx, ncy, cx, cy;
  int x, y, ylo, yhi, xlo, xhi;
  int chunkLen;
  int chunkNum = 0;
  char *chunkData = NULL;	/* So we can gdFree it with impunity. */
  char *compData = NULL;	/* So we can gdFree it with impunity. */
  uLongf compLen;
  int idxPos = 0;
  int idxSize;
  t_chunk_info *chunkIdx = NULL;
  int posSave;
  int bytesPerPixel = im->trueColor ? 4 : 1;
  int compMax = 0;

  /*printf("Trying to write GD2 file\n"); */

  /* */
  /* Force fmt to a valid value since we don't return anything. */
  /* */
  if ((fmt != GD2_FMT_RAW) && (fmt != GD2_FMT_COMPRESSED))
    {
      fmt = im->trueColor ? GD2_FMT_TRUECOLOR_COMPRESSED : GD2_FMT_COMPRESSED;
    };
  if (im->trueColor)
    {
      fmt += 2;
    }
  /* */
  /* Make sure chunk size is valid. These are arbitrary values; 64 because it seems */
  /* a little silly to expect performance improvements on a 64x64 bit scale, and  */
  /* 4096 because we buffer one chunk, and a 16MB buffer seems a little large - it may be */
  /* OK for one user, but for another to read it, they require the buffer. */
  /* */
  if (cs == 0)
    {
      cs = GD2_CHUNKSIZE;
    }
  else if (cs < GD2_CHUNKSIZE_MIN)
    {
      cs = GD2_CHUNKSIZE_MIN;
    }
  else if (cs > GD2_CHUNKSIZE_MAX)
    {
      cs = GD2_CHUNKSIZE_MAX;
    };

  /* Work out number of chunks. */
  ncx = im->sx / cs + 1;
  ncy = im->sy / cs + 1;

  /* Write the standard header. */
  _gd2PutHeader (im, out, cs, fmt, ncx, ncy);

  if (gd2_compressed (fmt))
    {
      /* */
      /* Work out size of buffer for compressed data, If CHUNKSIZE is large, */
      /* then these will be large! */
      /* */
      /* The zlib notes say output buffer size should be (input size) * 1.01 * 12 */
      /* - we'll use 1.02 to be paranoid. */
      /* */
      compMax = cs * bytesPerPixel * cs * 1.02 + 12;

      /* */
      /* Allocate the buffers.  */
      /* */
      chunkData = gdCalloc (cs * bytesPerPixel * cs, 1);
			if (!chunkData) {
				goto fail;
			}
      compData = gdCalloc (compMax, 1);
			if (!compData) {
				goto fail;
			}

      /* */
      /* Save the file position of chunk index, and allocate enough space for */
      /* each chunk_info block . */
      /* */
      idxPos = gdTell (out);
      idxSize = ncx * ncy * sizeof (t_chunk_info);
      GD2_DBG (printf ("Index size is %d\n", idxSize));
      gdSeek (out, idxPos + idxSize);

      chunkIdx = gdCalloc (idxSize * sizeof (t_chunk_info), 1);
      if (!chunkIdx) {
        goto fail;
      }
    };

  _gdPutColors (im, out);

  GD2_DBG (printf ("Size: %dx%d\n", im->sx, im->sy));
  GD2_DBG (printf ("Chunks: %dx%d\n", ncx, ncy));

  for (cy = 0; (cy < ncy); cy++)
    {
      for (cx = 0; (cx < ncx); cx++)
	{

	  ylo = cy * cs;
	  yhi = ylo + cs;
	  if (yhi > im->sy)
	    {
	      yhi = im->sy;
	    };

	  GD2_DBG (printf
		   ("Processing Chunk (%dx%d), y from %d to %d\n", cx, cy,
		    ylo, yhi));
	  chunkLen = 0;
	  for (y = ylo; (y < yhi); y++)
	    {

	      /*GD2_DBG(printf("y=%d: ",y)); */

	      xlo = cx * cs;
	      xhi = xlo + cs;
	      if (xhi > im->sx)
		{
		  xhi = im->sx;
		};

	      if (gd2_compressed (fmt))
		{
		  for (x = xlo; x < xhi; x++)
		    {
		      /* 2.0.11: use truecolor pixel array. TBB */
		      /*GD2_DBG(printf("%d...",x)); */
		      if (im->trueColor)
			{
			  int p = im->tpixels[y][x];
			  chunkData[chunkLen++] = gdTrueColorGetAlpha (p);
			  chunkData[chunkLen++] = gdTrueColorGetRed (p);
			  chunkData[chunkLen++] = gdTrueColorGetGreen (p);
			  chunkData[chunkLen++] = gdTrueColorGetBlue (p);
			}
		      else
			{
			  int p = im->pixels[y][x];
			  chunkData[chunkLen++] = p;
			}
		    };
		}
	      else
		{
		  for (x = xlo; x < xhi; x++)
		    {
		      /*GD2_DBG(printf("%d, ",x)); */

		      if (im->trueColor)
			{
			  gdPutInt (im->tpixels[y][x], out);
			}
		      else
			{
			  gdPutC ((unsigned char) im->pixels[y][x], out);
			}
		    };
		};
	      /*GD2_DBG(printf("y=%d done.\n",y)); */
	    };
	  if (gd2_compressed (fmt))
	    {
	      compLen = compMax;
	      if (compress ((unsigned char *)
			    &compData[0], &compLen,
			    (unsigned char *) &chunkData[0],
			    chunkLen) != Z_OK)
		{
		  printf ("Error from compressing\n");
		}
	      else
		{
		  chunkIdx[chunkNum].offset = gdTell (out);
		  chunkIdx[chunkNum++].size = compLen;
		  GD2_DBG (printf
			   ("Chunk %d size %d offset %d\n", chunkNum,
			    chunkIdx[chunkNum - 1].size,
			    chunkIdx[chunkNum - 1].offset));

		  if (gdPutBuf (compData, compLen, out) <= 0)
		    {
			fprintf(stderr, "gd write error\n");
		    };
		};
	    };
	};
    };
  if (gd2_compressed (fmt))
    {
      /* Save the position, write the index, restore position (paranoia). */
      GD2_DBG (printf ("Seeking %d to write index\n", idxPos));
      posSave = gdTell (out);
      gdSeek (out, idxPos);
      GD2_DBG (printf ("Writing index\n"));
      for (x = 0; x < chunkNum; x++)
	{
	  GD2_DBG (printf
		   ("Chunk %d size %d offset %d\n", x, chunkIdx[x].size,
		    chunkIdx[x].offset));
	  gdPutInt (chunkIdx[x].offset, out);
	  gdPutInt (chunkIdx[x].size, out);
	};
      /* We don't use fwrite for *endian reasons. */
      /*fwrite(chunkIdx, sizeof(int)*2, chunkNum, out); */
      gdSeek (out, posSave);
    };

  /*printf("Memory block size is %d\n",gdTell(out)); */
fail:
  GD2_DBG (printf ("Freeing memory\n"));

	if (chunkData) {
		gdFree (chunkData);
	}
	if (compData) {
		gdFree (compData);
	}
	if (chunkIdx) {
		gdFree (chunkIdx);
	}
  GD2_DBG (printf ("Done\n"));

}

BGD_DECLARE(void) gdImageGd2 (gdImagePtr im, FILE * outFile, int cs, int fmt)
{
  gdIOCtx *out = gdNewFileCtx (outFile);
  if (out == NULL) return;
  _gdImageGd2 (im, out, cs, fmt);
  out->gd_free (out);
}

BGD_DECLARE(void *) gdImageGd2Ptr (gdImagePtr im, int cs, int fmt, int *size)
{
  void *rv;
  gdIOCtx *out = gdNewDynamicCtx (2048, NULL);
  if (out == NULL) return NULL;
  _gdImageGd2 (im, out, cs, fmt);
  rv = gdDPExtractData (out, size);
  out->gd_free (out);
  return rv;
}

#else /* no HAVE_LIBZ */
BGD_DECLARE(gdImagePtr) gdImageCreateFromGd2 (FILE * inFile)
{
  fprintf (stderr, "GD2 support is not available - no libz\n");
  return NULL;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromGd2Ctx (gdIOCtxPtr in)
{
  fprintf (stderr, "GD2 support is not available - no libz\n");
  return NULL;
}
#endif /* HAVE_LIBZ */
