
#include <stdio.h>
#include <stdlib.h>
#include <gd.h>
#include <math.h>


int gen_image(const char* filename, int idx, int reverse_x, int width, int height, int bgd)
{
   double gradient = height / (width*2.0);
   int offset = idx*width;
	int x1,y1,x2,y2, error = 0;

   gdImagePtr im = gdImageCreateTrueColor(width,height);
   if (bgd==1) {
		gdImageFilledRectangle(im,0,0,width-1,height-1, gdTrueColorAlpha(255, 255, 255, 0));
   } else {
   	gdImageFilledRectangle(im,0,0,width-1,height-1, gdTrueColorAlpha(255, 255, 0, 0));
   }

   gdImageSetAntiAliased(im, gdTrueColorAlpha(0,0,0,0));

   /*
   test for potential segfault (introduced with AA improvements, fixed
	with the same patch - but I didn't notice it until later).*/
   gdImageLine(im,-1,-1,-1,-1,gdAntiAliased);

	x1 = floor(reverse_x * -width + 0.5);
	y1 = (offset-width) * gradient + 0.5;

	x2 = floor(reverse_x *  width*2 + 0.5);
	y2 = floor((offset+width*2) * gradient + 0.5);

   // draw an AA line
   gdImageLine(im, x1, y1, x2, y2, gdAntiAliased);

	gdImageLine(im, 0, im->sy - 1, im->sx, im->sy - 1, 0x40FF0000);

   gdImageDestroy(im);
   return error;
}

void gd_main()
{
 	gdImagePtr im;
	FILE *fp;
	char path[9][2048];
	int i;
	char dst[2048];

	sprintf(path[0], "cramps-tile.tif");
	sprintf(path[1], "cramps.tif");
	sprintf(path[2], "ycbcr-cat.tif");
	sprintf(path[3], "jello.tif");
	sprintf(path[4], "caspian.tif");
	sprintf(path[5], "strike.tif");
	sprintf(path[6], "off_luv24.tif");
	sprintf(path[7], "off_l16.tif");
	sprintf(path[8], "fax2d.tif");

	for (i = 0; i < 9; i++) {
		printf("opening %s\n", path[i]);
		fp = fopen(path[i], "rb");
		if (!fp) {
			printf("failed, cannot open file\n");
			return;
		}

		im = gdImageCreateFromTiff(fp);
		fclose(fp);
		if (!im) {
			fprintf(stderr, "Can't load TIFF image %s\n", path[i]);
			return;
		}


		sprintf(dst, "%i.png", i);

		fp = fopen(dst, "wb");
		if (!fp) {
			fprintf(stderr, "Can't save png image fromtiff.png\n");
			gdImageDestroy(im);
			return;
		}

		gdImagePng(im, fp);
		fclose(fp);
		gdImageDestroy(im);
	}
}
