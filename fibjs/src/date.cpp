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

}

