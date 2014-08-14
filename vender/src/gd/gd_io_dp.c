/*
 * io_dp.c
 *
 * Implements the dynamic pointer interface.
 *
 * Based on GD.pm code by Lincoln Stein for interfacing to libgd.
 * Added support for reading as well as support for 'tell' and 'seek'.
 *
 * As will all I/O modules, most functions are for local use only (called
 * via function pointers in the I/O context).
 *
 * gdDPExtractData is the exception to this: it will return the pointer to
 * the internal data, and reset the internal storage.
 *
 * Written/Modified 1999, Philip Warner.
 */

#	include "config.h"

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "gd.h"
#include "gdhelpers.h"

#define TRUE	1
#define FALSE	0

/* this is used for creating images in main memory */
typedef struct dpStruct
{
	void *data;
	int logicalSize;
	int realSize;
	int dataGood;
	int pos;
	int freeOK;
}
dynamicPtr;

typedef struct dpIOCtx
{
	gdIOCtx ctx;
	dynamicPtr *dp;
}
dpIOCtx;

typedef struct dpIOCtx *dpIOCtxPtr;

/* these functions operate on in-memory dynamic pointers */
static int allocDynamic(dynamicPtr *dp, int initialSize, void *data);
static int appendDynamic(dynamicPtr *dp, const void *src, int size);
static int gdReallocDynamic(dynamicPtr *dp, int required);
static int trimDynamic(dynamicPtr *dp);
static void gdFreeDynamicCtx(struct gdIOCtx *ctx);
static dynamicPtr *newDynamic(int initialSize, void *data, int freeOKFlag);

static int dynamicPutbuf(struct gdIOCtx *, const void *, int);
static void dynamicPutchar(struct gdIOCtx *, int a);

static int dynamicGetbuf(gdIOCtxPtr ctx, void *buf, int len);
static int dynamicGetchar(gdIOCtxPtr ctx);

static int dynamicSeek(struct gdIOCtx *, const int);
static long dynamicTell(struct gdIOCtx *);

/* return data as a dynamic pointer */
BGD_DECLARE(gdIOCtx *) gdNewDynamicCtx(int initialSize, void *data)
{
	/* 2.0.23: Phil Moore: 'return' keyword was missing! */
	return gdNewDynamicCtxEx(initialSize, data, 1);
}

BGD_DECLARE(gdIOCtx *) gdNewDynamicCtxEx(int initialSize, void *data, int freeOKFlag)
{
	dpIOCtx *ctx;
	dynamicPtr *dp;

	ctx = (dpIOCtx *)gdMalloc(sizeof (dpIOCtx));
	if(ctx == NULL) {
		return NULL;
	}

	dp = newDynamic(initialSize, data, freeOKFlag);
	if(!dp) {
		gdFree (ctx);
		return NULL;
	};

	ctx->dp = dp;

	ctx->ctx.getC = dynamicGetchar;
	ctx->ctx.putC = dynamicPutchar;

	ctx->ctx.getBuf = dynamicGetbuf;
	ctx->ctx.putBuf = dynamicPutbuf;

	ctx->ctx.seek = dynamicSeek;
	ctx->ctx.tell = dynamicTell;

	ctx->ctx.gd_free = gdFreeDynamicCtx;

	return (gdIOCtx *)ctx;
}

BGD_DECLARE(void *) gdDPExtractData (struct gdIOCtx *ctx, int *size)
{
	dynamicPtr *dp;
	dpIOCtx *dctx;
	void *data;

	dctx = (dpIOCtx *)ctx;
	dp = dctx->dp;

	/* clean up the data block and return it */
	if(dp->dataGood) {
		trimDynamic(dp);
		*size = dp->logicalSize;
		data = dp->data;
	} else {
		*size = 0;
		data = NULL;
		/* 2.0.21: never free memory we don't own */
		if((dp->data != NULL) && (dp->freeOK)) {
			gdFree(dp->data);
		}
	}

	dp->data = NULL;
	dp->realSize = 0;
	dp->logicalSize = 0;

	return data;
}

static void gdFreeDynamicCtx(struct gdIOCtx *ctx)
{
	dynamicPtr *dp;
	dpIOCtx *dctx;

	dctx = (dpIOCtx *)ctx;
	dp = dctx->dp;

	gdFree(ctx);

	/* clean up the data block and return it */
	/* 2.0.21: never free memory we don't own */
	if((dp->data != NULL) && (dp->freeOK)) {
		gdFree(dp->data);
		dp->data = NULL;
	}

	dp->realSize = 0;
	dp->logicalSize = 0;

	gdFree(dp);
}

static long dynamicTell(struct gdIOCtx *ctx)
{
	dpIOCtx *dctx;

	dctx = (dpIOCtx *)ctx;
	return (dctx->dp->pos);
}

static int dynamicSeek(struct gdIOCtx *ctx, const int pos)
{
	int bytesNeeded;
	dynamicPtr *dp;
	dpIOCtx *dctx;

	dctx = (dpIOCtx *)ctx;
	dp = dctx->dp;

	if(!dp->dataGood) {
		return FALSE;
	}

	bytesNeeded = pos;
	if(bytesNeeded > dp->realSize) {
		/* 2.0.21 */
		if(!dp->freeOK) {
			return FALSE;
		}

		if(overflow2(dp->realSize, 2)) {
			return FALSE;
		}

		if(!gdReallocDynamic(dp, dp->realSize * 2)) {
			dp->dataGood = FALSE;
			return FALSE;
		}
	}

	/* if we get here, we can be sure that we have enough bytes
	 * to copy safely */

	/* Extend the logical size if we seek beyond EOF. */
	if(pos > dp->logicalSize) {
		dp->logicalSize = pos;
	};

	dp->pos = pos;

	return TRUE;
}

/* return data as a dynamic pointer */
static dynamicPtr *newDynamic(int initialSize, void *data, int freeOKFlag)
{
	dynamicPtr *dp;

	dp = (dynamicPtr *) gdMalloc(sizeof (dynamicPtr));
	if(dp == NULL) {
		return NULL;
	}

	if(!allocDynamic(dp, initialSize, data)) {
		gdFree(dp);
		return NULL;
	}

	dp->pos = 0;
	dp->freeOK = freeOKFlag;

	return dp;
}

static int dynamicPutbuf(struct gdIOCtx *ctx, const void *buf, int size)
{
	dpIOCtx *dctx;
	dctx = (dpIOCtx *)ctx;

	appendDynamic(dctx->dp, buf, size);

	if(dctx->dp->dataGood) {
		return size;
	} else {
		return -1;
	};
}

static void dynamicPutchar(struct gdIOCtx *ctx, int a)
{
	unsigned char b;
	dpIOCtxPtr dctx;

	b = a;
	dctx = (dpIOCtxPtr) ctx;

	appendDynamic(dctx->dp, &b, 1);
}

static int dynamicGetbuf(gdIOCtxPtr ctx, void *buf, int len)
{
	int rlen, remain;
	dpIOCtxPtr dctx;
	dynamicPtr *dp;

	dctx = (dpIOCtxPtr) ctx;
	dp = dctx->dp;

	remain = dp->logicalSize - dp->pos;
	if(remain >= len) {
		rlen = len;
	} else {
		if(remain == 0) {
			/* 2.0.34: EOF is incorrect. We use 0 for
			 * errors and EOF, just like fileGetbuf,
			 * which is a simple fread() wrapper.
			 * TBB. Original bug report: Daniel Cowgill. */
			return 0; /* NOT EOF */
		}

		rlen = remain;
	}

	memcpy(buf, (void *) ((char *)dp->data + dp->pos), rlen);
	dp->pos += rlen;

	return rlen;
}

static int dynamicGetchar(gdIOCtxPtr ctx)
{
	unsigned char b;
	int rv;

	rv = dynamicGetbuf(ctx, &b, 1);

	if(rv != 1) {
		return EOF;
	} else {
		return b; /* (b & 0xff); */
	}
}

/**********************************************************************
 * InitDynamic - Return a dynamically resizable void*
 **********************************************************************/
static int allocDynamic(dynamicPtr *dp, int initialSize, void *data)
{
	if(data == NULL) {
		dp->logicalSize = 0;
		dp->dataGood = FALSE;
		dp->data = gdMalloc(initialSize);
	} else {
		dp->logicalSize = initialSize;
		dp->dataGood = TRUE;
		dp->data = data;
	}

	if(dp->data != NULL) {
		dp->realSize = initialSize;
		dp->dataGood = TRUE;
		dp->pos = 0;
		return TRUE;
	} else {
		dp->realSize = 0;
		return FALSE;
	}
}

/* append bytes to the end of a dynamic pointer */
static int appendDynamic(dynamicPtr * dp, const void *src, int size)
{
	int bytesNeeded;
	char *tmp;

	if(!dp->dataGood) {
		return FALSE;
	}

	/* bytesNeeded = dp->logicalSize + size; */
	bytesNeeded = dp->pos + size;

	if(bytesNeeded > dp->realSize) {
		/* 2.0.21 */
		if(!dp->freeOK) {
			return FALSE;
		}

		if(overflow2(dp->realSize, 2)) {
			return FALSE;
		}

		if(!gdReallocDynamic(dp, bytesNeeded * 2)) {
			dp->dataGood = FALSE;
			return FALSE;
		}
	}

	/* if we get here, we can be sure that we have enough bytes
	 * to copy safely */

	/*printf("Mem OK Size: %d, Pos: %d\n", dp->realSize, dp->pos); */

	tmp = (char *)dp->data;
	memcpy ((void *)(tmp + (dp->pos)), src, size);
	dp->pos += size;

	if(dp->pos > dp->logicalSize) {
		dp->logicalSize = dp->pos;
	};

	return TRUE;
}

/* grow (or shrink) dynamic pointer */
static int gdReallocDynamic(dynamicPtr *dp, int required)
{
	void *newPtr;

	/* First try gdRealloc().  If that doesn't work, make a new
	 * memory block and copy. */
	if((newPtr = gdRealloc(dp->data, required))) {
		dp->realSize = required;
		dp->data = newPtr;
		return TRUE;
	}

	/* create a new pointer */
	newPtr = gdMalloc(required);
	if(!newPtr) {
		dp->dataGood = FALSE;
		return FALSE;
	}

	/* copy the old data into it */
	memcpy(newPtr, dp->data, dp->logicalSize);
	gdFree(dp->data);
	dp->data = newPtr;

	dp->realSize = required;
	return TRUE;
}

/* trim pointer so that its real and logical sizes match */
static int trimDynamic(dynamicPtr *dp)
{
	/* 2.0.21: we don't reallocate memory we don't own */
	if(!dp->freeOK) {
		return TRUE;
	}

	return gdReallocDynamic(dp, dp->logicalSize);
}
