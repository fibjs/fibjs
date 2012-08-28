/*
 * date.cpp
 *
 *  Created on: Jul 31, 2012
 *      Author: lion
 */

#include <v8/v8.h>
#include "date.h"
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

const unsigned char LeapYearDayToMonth[366] =
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0,
		0, // January
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1,
		1, // February
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
		2, 2, 2, 2, 2, 2,
		2, // March
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
		3, 3, 3, 3, 3,
		3, // April
		4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
		4, 4, 4, 4, 4, 4,
		4, // May
		5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		5, 5, 5, 5, 5,
		5, // June
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		6, 6, 6, 6, 6, 6,
		6, // July
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7,
		7, // August
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8,
		8, // September
		9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
		9, 9, 9, 9, 9, 9,
		9, // October
		10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
		10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
		10, // November
		11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
		11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11 };

const unsigned char NormalYearDayToMonth[365] =
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0,
		0, // January
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1,
		1, // February
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
		2, 2, 2, 2, 2, 2,
		2, // March
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
		3, 3, 3, 3, 3,
		3, // April
		4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
		4, 4, 4, 4, 4, 4,
		4, // May
		5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		5, 5, 5, 5, 5,
		5, // June
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		6, 6, 6, 6, 6, 6,
		6, // July
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7,
		7, // August
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8,
		8, // September
		9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
		9, 9, 9, 9, 9, 9,
		9, // October
		10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
		10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
		10, // November
		11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
		11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11 };

const unsigned short LeapYearDaysPrecedingMonth[13] =
{ 0, // January
		31, // February
		31 + 29, // March
		31 + 29 + 31, // April
		31 + 29 + 31 + 30, // May
		31 + 29 + 31 + 30 + 31, // June
		31 + 29 + 31 + 30 + 31 + 30, // July
		31 + 29 + 31 + 30 + 31 + 30 + 31, // August
		31 + 29 + 31 + 30 + 31 + 30 + 31 + 31, // September
		31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30, // October
		31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31, // November
		31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30, // December
		31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31 };

const unsigned short NormalYearDaysPrecedingMonth[13] =
{ 0, // January
		31, // February
		31 + 28, // March
		31 + 28 + 31, // April
		31 + 28 + 31 + 30, // May
		31 + 28 + 31 + 30 + 31, // June
		31 + 28 + 31 + 30 + 31 + 30, // July
		31 + 28 + 31 + 30 + 31 + 30 + 31, // August
		31 + 28 + 31 + 30 + 31 + 30 + 31 + 31, // September
		31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30, // October
		31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31, // November
		31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30, // December
		31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31 };

#define IsLeapYear(YEARS) (									\
		(((YEARS) % 400 == 0) ||							\
		 (((YEARS) % 100 != 0) && ((YEARS) % 4 == 0))) ?	\
		true : false										\
		)

#define MaxDaysInMonth(YEAR,MONTH) (				\
		IsLeapYear(YEAR) ?							\
		LeapYearDaysPrecedingMonth[(MONTH) + 1] -	\
		LeapYearDaysPrecedingMonth[(MONTH)]			\
		:											\
		NormalYearDaysPrecedingMonth[(MONTH) + 1] -	\
		NormalYearDaysPrecedingMonth[(MONTH)] 		\
		)

#define NumberOfLeapYears(YEARS) (							\
		((YEARS) / 4) - ((YEARS) / 100) + ((YEARS) / 400)	\
		)

#define ElapsedYearsToDays(YEARS) (					\
		((YEARS) * 365) + NumberOfLeapYears(YEARS)	\
		)

void date_t::parse(const char* str, int len)
{
	short wYear = 0, wMonth = 0, wDay = 0, wHour = 0, wMinute = 0, wSecond = 0;
	int pos = 0;
	bool bTime = false;
	char ch;

	wYear = (short) getInt(str, len, pos);
	ch = pick(str, len, pos);
	if (ch == '-')
	{
		next(len, pos);

		wMonth = (short) getInt(str, len, pos) - 1;

		if (pick(str, len, pos) != '-')
			return;
		next(len, pos);

		wDay = (short) getInt(str, len, pos) - 1;

		if (pick(str, len, pos) == ' ')
		{
			next(len, pos);
			wHour = (short) getInt(str, len, pos);
			bTime = true;
		}
	}
	else if (ch == ':')
	{
		wHour = wYear;
		wYear = 1899;
		bTime = true;
	}

	if (bTime)
	{
		if (pick(str, len, pos) != ':')
			return;
		next(len, pos);

		wMinute = (short) getInt(str, len, pos);

		if (pick(str, len, pos) != ':')
			return;
		next(len, pos);

		wSecond = (short) getInt(str, len, pos);
	}

	unsigned int ElapsedDays = ElapsedYearsToDays(wYear - 1);

	if (IsLeapYear(wYear))
		ElapsedDays += LeapYearDaysPrecedingMonth[wMonth];
	else
		ElapsedDays += NormalYearDaysPrecedingMonth[wMonth];

	ElapsedDays += wDay;

	d = ElapsedDays * 86400000.0
			+ ((wHour * 60 + wMinute) * 60 + wSecond) * 1000 - 62135596800000.0;
}

void inline putStr(char *& ptrBuf, const char *ptr, int n)
{
	memcpy(ptrBuf, ptr, n);
	ptrBuf += n;
}

void inline putInt(char *& ptrBuf, int v, int n)
{
	int n1 = n;
	while (n--)
	{
		ptrBuf[n] = (v % 10) + '0';
		v /= 10;
	}
	ptrBuf += n1;
}

void date_t::toString(std::string& retVal)
{
	static char szMonth[][4] =
	{ "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct",
			"Nov", "Dec" };
	static char szDays[][4] =
	{ "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
	short wYear = 0, wMonth = 1, wHour = 0, wMinute = 0, wSecond = 0,
			wDayOfWeek = 0;

	int Days, Milliseconds, NumberOf400s, NumberOf100s, NumberOf4s;
	int64_t d1 = (int64_t) (d + 62135596800000);

	Days = (int)(d1 / 86400000);
	Milliseconds = d1 % 86400000;

	wDayOfWeek = (short) ((Days + 1) % 7);

	NumberOf400s = Days / 146097;
	Days -= NumberOf400s * 146097;

	NumberOf100s = (Days * 100 + 75) / 3652425;
	Days -= NumberOf100s * 36524;

	NumberOf4s = Days / 1461;
	Days -= NumberOf4s * 1461;

	wYear = (NumberOf400s * 400) + (NumberOf100s * 100) + (NumberOf4s * 4)
			+ (Days * 100 + 75) / 36525 + 1;

	Days = Days - (Days * 100 + 75) / 36525 * 365;

	if (IsLeapYear(wYear))
	{
		wMonth = LeapYearDayToMonth[Days];
		Days = Days - LeapYearDaysPrecedingMonth[wMonth];
	}
	else
	{
		wMonth = NormalYearDayToMonth[Days];
		Days = Days - NormalYearDaysPrecedingMonth[wMonth];
	}

	wSecond = Milliseconds / 1000;
	Milliseconds = Milliseconds % 1000;

	wMinute = wSecond / 60;
	wSecond = wSecond % 60;

	wHour = wMinute / 60;
	wMinute = wMinute % 60;

	retVal.resize(29);
	char* ptrBuf = &retVal[0];

	putStr(ptrBuf, szDays[wDayOfWeek], 3);
	putStr(ptrBuf, ", ", 2);
	putInt(ptrBuf, Days + 1, 2);
	*ptrBuf++ = ' ';
	putStr(ptrBuf, szMonth[wMonth], 3);
	*ptrBuf++ = ' ';
	putInt(ptrBuf, wYear, 4);
	*ptrBuf++ = ' ';
	putInt(ptrBuf, wHour, 2);
	*ptrBuf++ = ':';
	putInt(ptrBuf, wMinute, 2);
	*ptrBuf++ = ':';
	putInt(ptrBuf, wSecond, 2);
	putStr(ptrBuf, " GMT", 4);
}

}

