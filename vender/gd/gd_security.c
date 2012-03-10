/*
   * gd_security.c
   *
   * Implements buffer overflow check routines.
   *
   * Written 2004, Phil Knirsch.
   * Based on netpbm fixes by Alan Cox.
   *
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "gd.h"

int overflow2(int a, int b)
{
	if(a <= 0 || b <= 0) {
		fprintf(stderr, "gd warning: one parameter to a memory allocation multiplication is negative or zero, failing operation gracefully\n");
		return 1;
	}
	if(a > INT_MAX / b) {
		fprintf(stderr, "gd warning: product of memory allocation multiplication would exceed INT_MAX, failing operation gracefully\n");
		return 1;
	}
	return 0;
}
