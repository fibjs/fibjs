/*
 * date.cpp
 *
 *  Created on: Jul 31, 2012
 *      Author: lion
 */

#include <v8/v8.h>
#include "date.h"
#include "qstring.h"
#include <math.h>

namespace fibjs
{

inline void next(int &len, int& pos)
{
	pos++;
	if (len > 0)
		len--;
}

inline int64_t getInt(const char* str, int &len, int& pos)
{
	char ch;
	int64_t n = 0;

	while (len && (ch = str[pos]) && ch >= '0' && ch <= '9')
	{
		n = n * 10 + ch - '0';
		next(len, pos);
	}

	return n;
}

inline char pick(const char* str, int &len, int& pos)
{
	return len == 0 ? 0 : str[pos];
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

int inline checkmask(const char* data, int len, const char *mask)
{
	int i, j;
	char d;

	for (i = j = 0; i < len; i++)
	{
		d = data[j++];
		switch (mask[i])
		{
		case '\0':
			return (d == '\0');

		case '*':
			return 1;

		case '@':
			if (!qisupper(d))
				return 0;
			break;
		case '$':
			if (!qislower(d))
				return 0;
			break;
		case '!':
			if (!qisdigit(d))
				j--;
			break;
		case '#':
			if (!qisdigit(d))
				return 0;
			break;
		case '&':
			if (!qisxdigit(d))
				return 0;
			break;
		case '~':
			if ((d != ' ') && !qisdigit(d))
				return 0;
			break;
		default:
			if (mask[i] != d)
				return 0;
			break;
		}
	}

	return 0;
}

void date_t::parse(const char* str, int len)
{
	int wYear = 0, wMonth = 0, wDay = 0, wHour = 0, wMinute = 0, wSecond = 0;
	int pos = 0;
	bool bTime = false;
	char ch;
	int mint, mon;
	const char *monstr, *timstr = NULL;
	static const int months[12] =
	{ ('J' << 16) | ('a' << 8) | 'n', ('F' << 16) | ('e' << 8) | 'b',
			('M' << 16) | ('a' << 8) | 'r', ('A' << 16) | ('p' << 8) | 'r', ('M'
					<< 16) | ('a' << 8) | 'y', ('J' << 16) | ('u' << 8) | 'n',
			('J' << 16) | ('u' << 8) | 'l', ('A' << 16) | ('u' << 8) | 'g', ('S'
					<< 16) | ('e' << 8) | 'p', ('O' << 16) | ('c' << 8) | 't',
			('N' << 16) | ('o' << 8) | 'v', ('D' << 16) | ('e' << 8) | 'c' };

	if (len < 0)
		len = (int)qstrlen(str);

	if (!str || !*str)
		return;

	while (len > 0 && *str && qisspace(*str))
	{
		len--;
		++str;
	}

	if (qisdigit(*str))
	{
		wYear = (short) getInt(str, len, pos);
		ch = pick(str, len, pos);
		if (ch == '-')
		{
			next(len, pos);
			wMonth = (short) getInt(str, len, pos) - 1;

			if (pick(str, len, pos) == '-')
			{
				next(len, pos);
				wDay = (short) getInt(str, len, pos) - 1;

				if (pick(str, len, pos) == ' ')
				{
					next(len, pos);
					wHour = (short) getInt(str, len, pos);
					bTime = true;
				}
			}
		}
		else if (ch == '/')
		{
			wMonth = wYear - 1;

			next(len, pos);
			wDay = (short) getInt(str, len, pos) - 1;

			if (pick(str, len, pos) == '/')
			{
				next(len, pos);
				wYear = (short) getInt(str, len, pos);

				if (pick(str, len, pos) == ' ')
				{
					next(len, pos);
					wHour = (short) getInt(str, len, pos);
					bTime = true;
				}
			}
			else
				wYear = 2001;
		}
		else if (ch == ':')
		{
			wHour = wYear;
			wYear = 2001;
			bTime = true;
		}

		if (wYear < 100)
			wYear += wYear >= 50 ? 1900 : 2000;

		if (bTime)
		{
			if (pick(str, len, pos) == ':')
			{
				next(len, pos);
				wMinute = (short) getInt(str, len, pos);

				if (pick(str, len, pos) == ':')
				{
					next(len, pos);
					wSecond = (short) getInt(str, len, pos);
				}
			}
		}
	}
	else
	{
		while (len > 0 && *str && !qisspace(*str))
		{
			len--;
			++str;
		}

		if (*str == '\0')
			return;

		while (len > 0 && *str && qisspace(*str))
		{
			len--;
			++str;
		}

		if (checkmask(str, len, "@$$ ## #### ##:##:## *")) /* Javascript format */
		{
			wYear = (((str[7] & 0xf) * 10 + (str[8] & 0xf)) * 100
					+ ((str[9] & 0xf) * 10) + (str[10] & 0xf));

			wDay = (((str[4] & 0xf) * 10) + (str[5] & 0xf)) - 1;

			monstr = str;
			timstr = str + 12;
		}
		else if (checkmask(str, len, "## @$$ #### ##:##:## *")) /* RFC 1123 format */
		{
			wYear = (((str[7] & 0xf) * 10 + (str[8] & 0xf)) * 100
					+ ((str[9] & 0xf) * 10) + (str[10] & 0xf));

			wDay = (((str[0] & 0xf) * 10) + (str[1] & 0xf)) - 1;

			monstr = str + 3;
			timstr = str + 12;
		}
		else if (checkmask(str, len, "##-@$$-## ##:##:## *")) /* RFC 850 format  */
		{
			wYear = (((str[7] & 0xf) * 10) + (str[8] & 0xf));
			if (wYear < 70)
				wYear += 100;
			wYear += 1900;

			wDay = (((str[0] & 0xf) * 10) + (str[1] & 0xf)) - 1;

			monstr = str + 3;
			timstr = str + 10;
		}
		else if (checkmask(str, len, "@$$ ~# ##:##:## ####*")) /* asctime format  */
		{
			wYear = (((str[16] & 0xf) * 10 + (str[17] & 0xf)) * 100
					+ ((str[18] & 0xf) * 10) + (str[19] & 0xf));

			wDay = ((str[4] & 0xF) * 10 + (str[5] & 0xf)) - 1;

			monstr = str;
			timstr = str + 7;
		}
		else
			return;

		wHour = (((timstr[0] & 0xf) * 10) + (timstr[1] & 0xf));
		wMinute = (((timstr[3] & 0xf) * 10) + (timstr[4] & 0xf));
		wSecond = (((timstr[6] & 0xf) * 10) + (timstr[7] & 0xf));

		mint = (monstr[0] << 16) | (monstr[1] << 8) | monstr[2];
		for (mon = 0; mon < 12; mon++)
			if (mint == months[mon])
				break;

		wMonth = mon;
	}

	unsigned int ElapsedDays = ElapsedYearsToDays(wYear - 1);

	if (IsLeapYear(wYear))
		ElapsedDays += LeapYearDaysPrecedingMonth[wMonth];
	else
		ElapsedDays += NormalYearDaysPrecedingMonth[wMonth];

	ElapsedDays += wDay;

	d = ElapsedDays * 86400000.0
			+ ((wHour * 60 + wMinute) * 60 + wSecond) * 1000 - 62135625600000.0;
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

	int wYear = 0, wMonth = 1, wHour = 0, wMinute = 0, wSecond = 0, wDayOfWeek =
			0;

	int Days, Milliseconds, NumberOf400s, NumberOf100s, NumberOf4s;
	int64_t d1 = (int64_t) (d + 62135625600000);

	Days = (int) (d1 / 86400000);
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

