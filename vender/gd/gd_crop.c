/* Crop support
 * manual crop using a gdRect or automatic crop using a background
 * color (automatic detections or using either the transparent color,
 * black or white).
 * An alternative method allows to crop using a given color and a
 * threshold. It works relatively well but it can be improved.
 * Maybe L*a*b* and Delta-E will give better results (and a better
 * granularity).
 */

#include <stdlib.h>
#include "gd.h"
#include "gd_color.h"

static int gdGuessBackgroundColorFromCorners(gdImagePtr im, int *color);

BGD_DECLARE(gdImagePtr) gdImageCrop(gdImagePtr src, const gdRect *crop)
{
	gdImagePtr dst;

	dst = gdImageCreateTrueColor(crop->width, crop->height);
	if (!dst) return NULL;
	gdImageCopy(dst, src, 0, 0, crop->x, crop->y, crop->width, crop->height);

	return dst;
}

BGD_DECLARE(gdImagePtr) gdImageAutoCrop(gdImagePtr im, const unsigned int mode)
{
	const int width = gdImageSX(im);
	const int height = gdImageSY(im);

	int x,y;
	int color, corners, match;
	gdRect crop;

	crop.x = 0;
	crop.y = 0;
	crop.width = 0;
	crop.height = 0;

	switch (mode) {
		case GD_CROP_TRANSPARENT:
			color = gdImageGetTransparent(im);
			break;

		case GD_CROP_BLACK:
			color = gdImageColorClosestAlpha(im, 0, 0, 0, 0);
			break;

		case GD_CROP_WHITE:
			color = gdImageColorClosestAlpha(im, 255, 255, 255, 0);
			break;

		case GD_CROP_SIDES:
			corners = gdGuessBackgroundColorFromCorners(im, &color);
			break;

		case GD_CROP_DEFAULT:
		default:
			color = gdImageGetTransparent(im);
			break;
	}

	/* TODO: Add gdImageGetRowPtr and works with ptr at the row level
	 * for the true color and palette images
	 * new formats will simply work with ptr
	 */
	match = 1;
	for (y = 0; match && y < height; y++) {
		for (x = 0; match && x < width; x++) {
			match = (color == gdImageGetPixel(im, x,y));
		}
	}

	/* Nothing to do > bye 
	 * Duplicate the image?
	 */
	if (y == height - 1) {
		return NULL;
	}

	crop.y = y -1;
	match = 1;
	for (y = height - 1; match && y >= 0; y--) {
		for (x = 0; match && x < width; x++) {
			match = (color == gdImageGetPixel(im, x,y));
		}
	}

	if (y == 0) {
		crop.height = height - crop.y + 1;
	} else {
		crop.height = y - crop.y + 2;
	}

	match = 1;
	for (x = 0; match && x < width; x++) {
		for (y = 0; match && y < crop.y + crop.height - 1; y++) {
			match = (color == gdImageGetPixel(im, x,y));
		}
	}
	crop.x = x - 1;

	match = 1;
	for (x = width - 1; match && x >= 0; x--) {
		for (y = 0; match &&  y < crop.y + crop.height - 1; y++) {
			match = (color == gdImageGetPixel(im, x,y));
		}
	}
	crop.width = x - crop.x + 2;

	return gdImageCrop(im, &crop);
}

BGD_DECLARE(gdImagePtr) gdImageThresholdCrop(gdImagePtr im, const unsigned int color, const float threshold)
{
	const int width = gdImageSX(im);
	const int height = gdImageSY(im);

	int x,y;
	int match;
	gdRect crop;

	crop.x = 0;
	crop.y = 0;
	crop.width = 0;
	crop.height = 0;

	/* Pierre: crop everything sounds bad */
	if (threshold > 100.0) {
		return NULL;
	}

	/* TODO: Add gdImageGetRowPtr and works with ptr at the row level
	 * for the true color and palette images
	 * new formats will simply work with ptr
	 */
	match = 1;
	for (y = 0; match && y < height; y++) {
		for (x = 0; match && x < width; x++) {
			match = (gdColorMatch(im, color, gdImageGetPixel(im, x,y), threshold)) > 0;
		}
	}

	/* Pierre
	 * Nothing to do > bye 
	 * Duplicate the image?
	 */
	if (y == height - 1) {
		return NULL;
	}

	crop.y = y -1;
	match = 1;
	for (y = height - 1; match && y >= 0; y--) {
		for (x = 0; match && x < width; x++) {
			match = (gdColorMatch(im, color, gdImageGetPixel(im, x, y), threshold)) > 0;
		}
	}

	if (y == 0) {
		crop.height = height - crop.y + 1;
	} else {
		crop.height = y - crop.y + 2;
	}

	match = 1;
	for (x = 0; match && x < width; x++) {
		for (y = 0; match && y < crop.y + crop.height - 1; y++) {
			match = (gdColorMatch(im, color, gdImageGetPixel(im, x,y), threshold)) > 0;
		}
	}
	crop.x = x - 1;

	match = 1;
	for (x = width - 1; match && x >= 0; x--) {
		for (y = 0; match &&  y < crop.y + crop.height - 1; y++) {
			match = (gdColorMatch(im, color, gdImageGetPixel(im, x,y), threshold)) > 0;
		}
	}
	crop.width = x - crop.x + 2;

	return gdImageCrop(im, &crop);
}

/* This algorithm comes from pnmcrop (http://netpbm.sourceforge.net/)
 * Three steps:
 *  - if 3 corners are equal.
 *  - if two are equal.
 *  - Last solution: average the colors
 */
static int gdGuessBackgroundColorFromCorners(gdImagePtr im, int *color)
{
	const int tl = gdImageGetPixel(im, 0, 0);
	const int tr = gdImageGetPixel(im, gdImageSX(im) - 1, 0);
	const int bl = gdImageGetPixel(im, 0, gdImageSY(im) -1);
	const int br = gdImageGetPixel(im, gdImageSX(im) - 1, gdImageSY(im) -1);

	if (tr == bl && tr == br) {
		*color = tr;
		return 3;
	} else if (tl == bl && tl == br) {
		*color = tl;
		return 3;
	} else if (tl == tr &&  tl == br) {
		*color = tl;
		return 3;
	} else if (tl == tr &&  tl == bl) {
		*color = tl;
		return 3;
	} else if (tl == tr  || tl == bl || tl == br) {
		*color = tl;
		return 2;
	} else if (tr == bl || tr == bl) {
		*color = tr;
		return 2;
	} else if (br == bl) {
		*color = bl;
		return 2;
	} else {
		register int r,b,g,a;

		r = (int)(0.5f + (gdImageRed(im, tl) + gdImageRed(im, tr) + gdImageRed(im, bl) + gdImageRed(im, br)) / 4);
		g = (int)(0.5f + (gdImageGreen(im, tl) + gdImageGreen(im, tr) + gdImageGreen(im, bl) + gdImageGreen(im, br)) / 4);
		b = (int)(0.5f + (gdImageBlue(im, tl) + gdImageBlue(im, tr) + gdImageBlue(im, bl) + gdImageBlue(im, br)) / 4);
		a = (int)(0.5f + (gdImageAlpha(im, tl) + gdImageAlpha(im, tr) + gdImageAlpha(im, bl) + gdImageAlpha(im, br)) / 4);
		*color = gdImageColorClosestAlpha(im, r, g, b, a);
		return 0;
	}
}
