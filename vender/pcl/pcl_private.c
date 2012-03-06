/*
 *  PCL by Davide Libenzi (Portable Coroutine Library)
 *  Copyright (C) 2003..2010  Davide Libenzi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Davide Libenzi <davidel@xmailserver.org>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pcl.h"
#include "pcl_private.h"

static cothread_ctx *co_get_global_ctx(void)
{
	static cothread_ctx tctx;

	if (tctx.co_curr == NULL)
		tctx.co_curr = &tctx.co_main;

	return &tctx;
}

/*
 * MultiThread cases ...
 */

# ifdef _WIN32
/*
 * On Windows, we can use the native TLS capabilities. Pretty easy ...
 */
static __declspec(thread) cothread_ctx *tctx;

int co_thread_init(void)
{
	if ((tctx = (cothread_ctx *)
	     malloc(sizeof(cothread_ctx))) == NULL) {
		perror("allocating context");
		return -1;
	}
	memset(tctx, 0, sizeof(*tctx));
	tctx->co_curr = &tctx->co_main;

	return 0;
}

void co_thread_cleanup(void)
{
	free(tctx);
}

cothread_ctx *co_get_thread_ctx(void)
{
	/*
	 * Even in MT mode, allows for the main thread to not call
	 * the co_thread_init()/co_thread_cleanup() functions.
	 */
	return tctx != NULL ? tctx: co_get_global_ctx();
}

#else
/*
 * On Unix, we use pthread. Sligthly more complicated ...
 */
#include <pthread.h>

static int valid_key;
static pthread_key_t key;
static pthread_once_t once_control = PTHREAD_ONCE_INIT;

static void co_once_init(void)
{
	if (pthread_key_create(&key, free))
		perror("creating TLS key");
	else
		valid_key++;
}

int co_thread_init(void)
{
	cothread_ctx *tctx;

	pthread_once(&once_control, co_once_init);
	if (!valid_key)
		return -1;

	if ((tctx = (cothread_ctx *)
	     malloc(sizeof(cothread_ctx))) == NULL) {
		perror("allocating context");
		return -1;
	}
	memset(tctx, 0, sizeof(*tctx));
	tctx->co_curr = &tctx->co_main;
	if (pthread_setspecific(key, tctx)) {
		perror("setting thread context");
		free(tctx);
		return -1;
	}

	return 0;
}

void co_thread_cleanup(void)
{

}

cothread_ctx *co_get_thread_ctx(void)
{
	cothread_ctx *tctx = (cothread_ctx *)
		(valid_key ? pthread_getspecific(key): NULL);

	/*
	 * Even in MT mode, allows for the main thread to not call
	 * the co_thread_init()/co_thread_cleanup() functions.
	 */
	return tctx != NULL ? tctx: co_get_global_ctx();
}

#endif

