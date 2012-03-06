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
#include "pcl.h"
#include "pcl_private.h"


static int co_set_context(co_ctx_t *ctx, void *func, char *stkbase, long stksiz)
{
	char *stack;

	stack = stkbase + stksiz - sizeof(long);

	pcl_setjmp(&ctx->cc);

#if defined(__x86_64__)
	ctx->cc.Rip = (unsigned long long) func;
	ctx->cc.Rsp = (unsigned long long) stack;
#else
	ctx->cc.Eip = (unsigned long) func;
	ctx->cc.Esp = (unsigned long) stack;
#endif

	return 0;
}

static void co_switch_context(co_ctx_t *octx, co_ctx_t *nctx)
{
	if (pcl_setjmp(&octx->cc) == 0)
		pcl_longjmp(&nctx->cc, 1);
}

static void co_runner(void)
{
	cothread_ctx *tctx = co_get_thread_ctx();
	coroutine *co = tctx->co_curr;

	co->restarget = co->caller;
	co->func(co->data);
	co_exit();
}

coroutine_t co_create(void (*func)(void *), void *data, void *stack, int size)
{
	int alloc = 0;
	coroutine *co;

	if ((size &= ~(sizeof(long) - 1)) < CO_MIN_SIZE)
		return NULL;
	if (stack == NULL) {
		size = (size + sizeof(coroutine) + CO_STK_ALIGN - 1) & ~(CO_STK_ALIGN - 1);
		stack = malloc(size);
		if (stack == NULL)
			return NULL;
		alloc = size;
	}
	co = stack;
	stack = (char *) stack + CO_STK_COROSIZE;
	co->alloc = alloc;
	co->func = func;
	co->data = data;
	if (co_set_context(&co->ctx, co_runner, stack, size - CO_STK_COROSIZE) < 0) {
		if (alloc)
			free(co);
		return NULL;
	}

	return (coroutine_t) co;
}

void co_delete(coroutine_t coro)
{
	cothread_ctx *tctx = co_get_thread_ctx();
	coroutine *co = (coroutine *) coro;

	if (co == tctx->co_curr) {
		fprintf(stderr, "[PCL] Cannot delete itself: curr=%p\n",
			tctx->co_curr);
		exit(1);
	}
	if (co->alloc)
		free(co);
}

void co_call(coroutine_t coro)
{
	cothread_ctx *tctx = co_get_thread_ctx();
	coroutine *co = (coroutine *) coro, *oldco = tctx->co_curr;

	co->caller = tctx->co_curr;
	tctx->co_curr = co;

	co_switch_context(&oldco->ctx, &co->ctx);
}

void co_resume(void)
{
	cothread_ctx *tctx = co_get_thread_ctx();

	co_call(tctx->co_curr->restarget);
	tctx->co_curr->restarget = tctx->co_curr->caller;
}

static void co_del_helper(void *data)
{
	cothread_ctx *tctx;
	coroutine *cdh;

	for (;;) {
		tctx = co_get_thread_ctx();
		cdh = tctx->co_dhelper;
		tctx->co_dhelper = NULL;
		co_delete(tctx->co_curr->caller);
		co_call((coroutine_t) cdh);
		if (tctx->co_dhelper == NULL) {
			fprintf(stderr,
				"[PCL] Resume to delete helper coroutine: curr=%p caller=%p\n",
				tctx->co_curr, tctx->co_curr->caller);
			exit(1);
		}
	}
}

void co_exit_to(coroutine_t coro)
{
	cothread_ctx *tctx = co_get_thread_ctx();
	coroutine *co = (coroutine *) coro;

	if (tctx->dchelper == NULL &&
	    (tctx->dchelper = co_create(co_del_helper, NULL,
					tctx->stk, sizeof(tctx->stk))) == NULL) {
		fprintf(stderr, "[PCL] Unable to create delete helper coroutine: curr=%p\n",
			tctx->co_curr);
		exit(1);
	}
	tctx->co_dhelper = co;

	co_call((coroutine_t) tctx->dchelper);

	fprintf(stderr, "[PCL] Stale coroutine called: curr=%p  exitto=%p  caller=%p\n",
		tctx->co_curr, co, tctx->co_curr->caller);
	exit(1);
}

void co_exit(void)
{
	cothread_ctx *tctx = co_get_thread_ctx();

	co_exit_to((coroutine_t) tctx->co_curr->restarget);
}

coroutine_t co_current(void)
{
	cothread_ctx *tctx = co_get_thread_ctx();

	return (coroutine_t) tctx->co_curr;
}

void *co_get_data(coroutine_t coro)
{
	coroutine *co = (coroutine *) coro;

	return co->data;
}

void *co_set_data(coroutine_t coro, void *data)
{
	coroutine *co = (coroutine *) coro;
	void *odata;

	odata = co->data;
	co->data = data;

	return odata;
}

