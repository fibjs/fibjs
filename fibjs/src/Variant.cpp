/*
 * Variant.cpp
 *
 *  Created on: Jul 31, 2012
 *      Author: lion
 */

#include "Variant.h"
#include <math.h>

namespace fibjs
{

inline void next(int &len, int& pos)
{
	pos++;
	if (len > 0)
		len--;
}

inline int64_t getInt(const char* s, int &len, int& pos)
{
	char ch;
	int64_t n = 0;

	while (len && (ch = s[pos]) && ch >= '0' && ch <= '9')
	{
		n = n * 10 + ch - '0';
		next(len, pos);
	}

	return n;
}

inline char pick(const char* s, int &len, int& pos)
{
	return len == 0 ? 0 : s[pos];
}

void Variant::parseNumber(const char* str, int len)
{
	int64_t digit, frac = 0, exp = 0;
	double v, div = 1.0;
	bool bNeg, bExpNeg;
	int pos = 0;
	char ch;

	if (bNeg = (pick(str, len, pos) == '-'))
		next(len, pos);

	digit = getInt(str, len, pos);

	ch = pick(str, len, pos);
	if (ch == '.')
	{
		next(len, pos);

		while ((ch = pick(str, len, pos)) && ch >= '0' && ch <= '9')
		{
			frac = frac * 10 + ch - '0';
			div *= 10;
			next(len, pos);
		}
	}

	if (ch == 'e' || ch == 'E')
	{
		next(len, pos);

		bExpNeg = false;
		ch = pick(str, len, pos);
		if (ch == '+')
			next(len, pos);
		else if (ch == '-')
		{
			bExpNeg = true;
			next(len, pos);
		}

		exp = getInt(str, len, pos);
		if (bExpNeg)
			exp = -exp;
	}

	if (div > 1 || exp != 0)
	{
		v = digit + (frac / div);
		if (bNeg)
			v = -v;

		if (exp != 0)
			v *= pow((double) 10, (int) exp);

		m_type = VT_Number;
		m_Val.dblVal = v;

		return;
	}

	if (bNeg)
		digit = -digit;

	if (digit >= -2147483648ll && digit <= 2147483647ll)
	{
		m_type = VT_Integer;
		m_Val.intVal = (int32_t) digit;
	}
	else
	{
		m_type = VT_Long;
		m_Val.longVal = digit;
	}
}

}
