/*
 *  je_bridge.cpp
 *  Created on: Aug 4, 2015
 *
 *  Copyright (c) 2015 by Leo Hoo
 *  lion@9465.net
 */

#if !defined(_WIN32)

#include <stdlib.h>
#include <jemalloc/include/jemalloc/jemalloc.h>

#if !defined(__clang__)
#define wrap(str) __wrap_##str
#else
#define wrap(str) str
#endif

extern "C" void* wrap(malloc)(size_t sz)
{
	return je_malloc(sz);
}

extern "C" void wrap(free)(void *p)
{
	je_free(p);
}

extern "C" void* wrap(realloc)(void* p, size_t sz)
{
	return je_realloc(p, sz);
}

extern "C" void* wrap(calloc)(size_t num, size_t sz)
{
	return je_calloc(num, sz);
}

void* operator new (size_t sz)
{
	return je_malloc(sz);
}

void* operator new[] (size_t sz)
{
	return je_malloc(sz);
}

void operator delete (void* p) throw()
{
	je_free(p);
}

void operator delete[] (void* p) throw()
{
	je_free(p);
}

#endif
