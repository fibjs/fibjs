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

#if !defined(PCL_PRIVATE_H)
#define PCL_PRIVATE_H

#include <stdio.h>
#include <stdlib.h>
#include "pcl_config.h"
#include "pcl.h"

#if defined(CO_USE_UCONEXT)
#include <ucontext.h>

typedef ucontext_t co_core_ctx_t;
#else
#include <setjmp.h>

typedef jmp_buf co_core_ctx_t;
#endif

/*
 * The following value must be power of two (N^2).
 */
#define CO_STK_ALIGN 256
#define CO_STK_COROSIZE ((sizeof(coroutine) + CO_STK_ALIGN - 1) & ~(CO_STK_ALIGN - 1))
#define CO_MIN_SIZE (4 * 1024)

typedef struct s_co_ctx {
	co_core_ctx_t cc;
} co_ctx_t;

typedef struct s_coroutine {
	co_ctx_t ctx;
	int alloc;
	struct s_coroutine *caller;
	struct s_coroutine *restarget;
	void (*func)(void *);
	void *data;
} coroutine;

typedef struct s_cothread_ctx {
	coroutine co_main;
	coroutine *co_curr;
	coroutine *co_dhelper;
	coroutine *dchelper;
	char stk[CO_MIN_SIZE];
} cothread_ctx;

cothread_ctx *co_get_thread_ctx(void);

#endif

