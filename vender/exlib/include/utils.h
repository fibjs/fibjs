/*
 *  utils.h
 *  Created on: Mar 19, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#ifndef _ex_utils_h__
#define _ex_utils_h__

#include <stdint.h>
#include "osconfig.h"

#ifdef WIN32
#include "utils_win.h"
#elif defined(x64)
#include "utils_x64.h"
#elif defined(I386)
#include "utils_x86.h"
#elif defined(arm)
#include "utils_arm.h"
#endif

namespace exlib
{

class atomic
{
public:
	atomic() : m_v(0) {
	}

	atomic(int32_t new_value)
	{
		operator=(new_value);
	}

	atomic(const atomic &new_value)
	{
		operator=(new_value);
	}

public:
	int32_t operator=(int32_t new_value)
	{
		xchg(new_value);
		return new_value;
	}

	int32_t operator=(const atomic &new_value)
	{
		int32_t v = new_value.m_v;

		xchg(v);
		return v;
	}

	operator int32_t () const
	{
		return m_v;
	}

	int32_t value() const
	{
		return m_v;
	}

	int32_t CompareAndSwap(int32_t old_value, int32_t new_value)
	{
		return exlib::CompareAndSwap(&m_v, old_value, new_value);
	}

	inline int32_t add(int32_t incr)
	{
		return atom_add(&m_v, incr);
	}

	inline int32_t xchg(int32_t new_value)
	{
		return atom_xchg(&m_v, new_value);
	}

	inline int32_t inc()
	{
		return add(1);
	}

	inline int32_t dec()
	{
		return add(-1);
	}

private:
	volatile int32_t m_v;
};

template<class T>
class atomic_ptr
{
public:
	atomic_ptr() : m_v(0) {
	}

	atomic_ptr(T* new_value)
	{
		operator=(new_value);
	}

	atomic_ptr(const atomic_ptr &new_value)
	{
		operator=(new_value);
	}

public:
	T* operator=(T* new_value)
	{
		atom_xchg(&m_v, new_value);
		return new_value;
	}

	T* operator=(const atomic_ptr &new_value)
	{
		T* v = new_value.m_v;

		atom_xchg(&m_v, v);
		return v;
	}

	operator T* () const
	{
		return m_v;
	}

	T* value() const
	{
		return m_v;
	}

	T* CompareAndSwap(T* old_value, T* new_value)
	{
		return exlib::CompareAndSwap(&m_v, old_value, new_value);
	}

	inline T* xchg(T* new_value)
	{
		return atom_xchg(&m_v, new_value);
	}

private:
	T *volatile m_v;
};

}

#endif
