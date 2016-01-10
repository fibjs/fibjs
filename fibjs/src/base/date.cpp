/*
 * date.cpp
 *
 *  Created on: Jul 31, 2012
 *      Author: lion
 */

#include "Runtime.h"
#include "DateCache.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>

namespace fibjs
{

static DateCache* s_dc;

void init_date()
{
    s_dc = new DateCache();
}

inline void next(int32_t &len, int32_t &pos)
{
    pos++;
    if (len > 0)
        len--;
}

inline int64_t getInt(const char *str, int32_t &len, int32_t &pos)
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

inline char pick(const char *str, int32_t &len, int32_t &pos)
{
    return len == 0 ? 0 : str[pos];
}

const unsigned char LeapYearDayToMonth[366] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
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
    11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11
};

const unsigned char NormalYearDayToMonth[365] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
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
    11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11
};

const uint16_t LeapYearDaysPrecedingMonth[13] =
{
    0, // January
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
    31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31
};

const uint16_t NormalYearDaysPrecedingMonth[13] =
{
    0, // January
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
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31
};

#define IsLeapYear(YEARS) (                                 \
        (((YEARS) % 400 == 0) ||                            \
         (((YEARS) % 100 != 0) && ((YEARS) % 4 == 0))) ?    \
        true : false                                        \
                          )

#define MaxDaysInMonth(YEAR,MONTH) (                \
        IsLeapYear(YEAR) ?                          \
        LeapYearDaysPrecedingMonth[(MONTH) + 1] -   \
        LeapYearDaysPrecedingMonth[(MONTH)]         \
        :                                           \
        NormalYearDaysPrecedingMonth[(MONTH) + 1] - \
        NormalYearDaysPrecedingMonth[(MONTH)]       \
                                   )

#define NumberOfLeapYears(YEARS) (                          \
        ((YEARS) / 4) - ((YEARS) / 100) + ((YEARS) / 400)   \
                                 )

#define ElapsedYearsToDays(YEARS) (                 \
        ((YEARS) * 365) + NumberOfLeapYears(YEARS)  \
                                  )

int32_t inline checkmask(const char *data, int32_t len, const char *mask)
{
    int32_t i, j;
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

    return mask[i] == '*';
}

void date_t::create(int32_t Y, int32_t M, int32_t D, int32_t h, int32_t m, int32_t s, int32_t ms)
{
    if (M < 1 || D < 1)
    {
        d = NAN;
        return;
    }

    M --;
    D --;

    if (M > 11 || D > MaxDaysInMonth(Y, M) - 1)
    {
        d = NAN;
        return;
    }

    uint32_t ElapsedDays = ElapsedYearsToDays(Y - 1);

    if (IsLeapYear(Y))
        ElapsedDays += LeapYearDaysPrecedingMonth[M];
    else
        ElapsedDays += NormalYearDaysPrecedingMonth[M];

    ElapsedDays += D;

    d = ElapsedDays * 86400000.0
        + ((h * 60 + m) * 60 + s) * 1000 + ms - 62135596800000.0;
}

void date_t::parse(const char *str, int32_t len)
{
    int32_t wYear = 0, wMonth = 0, wDay = 0, wHour = 0, wMinute = 0, wSecond = 0,
            wMicroSecond = 0;
    int32_t pos = 0;
    bool bTime = false;
    char ch;
    int32_t mint, mon;
    int32_t tz = 0;
    bool bLocal = true;
    const char *monstr, *timstr = NULL, *tzstr = NULL;
    static const int32_t months[12] =
    {
        ('J' << 16) | ('a' << 8) | 'n', ('F' << 16) | ('e' << 8) | 'b',
        ('M' << 16) | ('a' << 8) | 'r', ('A' << 16) | ('p' << 8) | 'r', ('M'
        << 16) | ('a' << 8) | 'y', ('J' << 16) | ('u' << 8) | 'n',
        ('J' << 16) | ('u' << 8) | 'l', ('A' << 16) | ('u' << 8) | 'g', ('S'
        << 16) | ('e' << 8) | 'p', ('O' << 16) | ('c' << 8) | 't',
        ('N' << 16) | ('o' << 8) | 'v', ('D' << 16) | ('e' << 8) | 'c'
    };

    if (len < 0)
        len = (int32_t) qstrlen(str);

    if (!str || !*str)
        return;

    while (len > 0 && *str && qisspace(*str))
    {
        len--;
        ++str;
    }

    if (qisdigit(*str))
    {
        wYear = (int16_t) getInt(str, len, pos);
        ch = pick(str, len, pos);

        if (ch == '/' || ch == '-')
        {
            wMonth = wYear;

            next(len, pos);
            wDay = (int16_t) getInt(str, len, pos);

            ch = pick(str, len, pos);
            if (ch == '/' || ch == '-')
            {
                next(len, pos);
                wYear = (int16_t) getInt(str, len, pos);

                ch = pick(str, len, pos);
                if (ch == ' ' || ch == 'T' || ch == 't')
                {
                    if (ch != ' ')
                        bLocal = false;

                    next(len, pos);
                    wHour = (int16_t) getInt(str, len, pos);
                    bTime = true;
                }
            }
            else if (wMonth > 12 && wDay <= 12)
            {
                wYear = wMonth;
                wMonth = wDay;
                wDay = 1;
            }
            else
                wYear = 2001;

            // check m/d/y
            if (wMonth
                    > 12 && wDay <= 12 && wYear <= MaxDaysInMonth(wMonth, wDay - 1))
            {
                int32_t n = wYear;
                wYear = wMonth;
                wMonth = wDay;
                wDay = n;
            }

            wMonth--;
            wDay--;
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
                wMinute = (int16_t) getInt(str, len, pos);

                if (pick(str, len, pos) == ':')
                {
                    next(len, pos);
                    wSecond = (int16_t) getInt(str, len, pos);

                    if (pick(str, len, pos) == '.')
                    {
                        int32_t i, p = 100;

                        next(len, pos);
                        for (i = 0; i < 3; i++)
                        {
                            ch = pick(str, len, pos);
                            if (!qisdigit(ch))
                                break;

                            next(len, pos);
                            wMicroSecond += (ch - '0') * p;
                            p /= 10;
                        }
                    }
                }
            }
        }

        if (!qstricmp(str + pos, " pm", 3))
            wHour += 12;
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

        if (checkmask(str, len, "@$$ ## #### ##:##:##*")) // Javascript format
        {
            wYear = (((str[7] & 0xf) * 10 + (str[8] & 0xf)) * 100
                     + ((str[9] & 0xf) * 10) + (str[10] & 0xf));

            wDay = (((str[4] & 0xf) * 10) + (str[5] & 0xf)) - 1;

            monstr = str;
            timstr = str + 12;
            tzstr = str + 20;
        }
        else if (checkmask(str, len, "## @$$ #### ##:##:##*")) // RFC 1123 format
        {
            wYear = (((str[7] & 0xf) * 10 + (str[8] & 0xf)) * 100
                     + ((str[9] & 0xf) * 10) + (str[10] & 0xf));

            wDay = (((str[0] & 0xf) * 10) + (str[1] & 0xf)) - 1;

            monstr = str + 3;
            timstr = str + 12;
            tzstr = str + 20;
        }
        else if (checkmask(str, len, "##-@$$-## ##:##:##*")) // RFC 850 format
        {
            wYear = (((str[7] & 0xf) * 10) + (str[8] & 0xf));
            if (wYear < 70)
                wYear += 100;
            wYear += 1900;

            wDay = (((str[0] & 0xf) * 10) + (str[1] & 0xf)) - 1;

            monstr = str + 3;
            timstr = str + 10;
            tzstr = str + 18;
        }
        else if (checkmask(str, len, "@$$ ~# ##:##:## ####*")) // asctime format
        {
            wYear = (((str[16] & 0xf) * 10 + (str[17] & 0xf)) * 100
                     + ((str[18] & 0xf) * 10) + (str[19] & 0xf));

            wDay = ((str[4] & 0xF) * 10 + (str[5] & 0xf)) - 1;

            monstr = str;
            timstr = str + 7;
            tzstr = str + 20;
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

        if (tzstr)
        {
            if (!qstricmp(tzstr, " gmt", 4))
            {
                tz = 0;

                if (qisdigit(tzstr[5]))
                {
                    if (tzstr[4] == '+')
                        tz -= atoi(tzstr + 5);
                    else if (tzstr[4] == '-')
                        tz += atoi(tzstr + 5);
                }

                bLocal = false;
            }
        }
    }

    create(wYear, wMonth + 1, wDay + 1, wHour, wMinute, wSecond, wMicroSecond);

    if (isnan(d))
        return;

    if (bLocal)
        d = (double) s_dc->ToUTC((int64_t) d);
    else
        d += tz * 36000;
}

void inline putStr(char  *&ptrBuf, const char *ptr, int32_t n)
{
    memcpy(ptrBuf, ptr, n);
    ptrBuf += n;
}

void inline putInt(char  *&ptrBuf, int32_t v, int32_t n)
{
    int32_t n1 = n;
    while (n--)
    {
        ptrBuf[n] = (v % 10) + '0';
        v /= 10;
    }
    ptrBuf += n1;
}

class _date_split
{
public:
    _date_split(double d)
    {
        wYear = 0;
        wMonth = 1;
        wHour = 0;
        wMinute = 0;
        wSecond = 0;
        wDayOfWeek = 0;

        int64_t d1 = (int64_t) (d + 62135596800000ll);
        int32_t NumberOf400s, NumberOf100s, NumberOf4s;

        wDay = (int32_t) (d1 / 86400000);
        wMillisecond = d1 % 86400000;

        wDayOfWeek = (int16_t) ((wDay + 1) % 7);

        NumberOf400s = wDay / 146097;
        wDay -= NumberOf400s * 146097;

        NumberOf100s = (wDay * 100 + 75) / 3652425;
        wDay -= NumberOf100s * 36524;

        NumberOf4s = wDay / 1461;
        wDay -= NumberOf4s * 1461;

        wYear = (NumberOf400s * 400) + (NumberOf100s * 100) + (NumberOf4s * 4)
                + (wDay * 100 + 75) / 36525 + 1;

        wDay = wDay - (wDay * 100 + 75) / 36525 * 365;

        if (IsLeapYear(wYear))
        {
            wMonth = LeapYearDayToMonth[wDay];
            wDay = wDay - LeapYearDaysPrecedingMonth[wMonth];
        }
        else
        {
            wMonth = NormalYearDayToMonth[wDay];
            wDay = wDay - NormalYearDaysPrecedingMonth[wMonth];
        }

        wSecond = wMillisecond / 1000;
        wMillisecond = wMillisecond % 1000;

        wMinute = wSecond / 60;
        wSecond = wSecond % 60;

        wHour = wMinute / 60;
        wMinute = wMinute % 60;

    }

public:
    int32_t wYear, wMonth, wHour, wMinute,
            wSecond, wDayOfWeek, wDay, wMillisecond;
};


void date_t::toGMTString(std::string &retVal)
{
    if (isnan(d))
        return;

    static char szMonth[][4] =
    {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct",
        "Nov", "Dec"
    };
    static char szDays[][4] =
    { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

    _date_split ds(d);

    retVal.resize(29);
    char *ptrBuf = &retVal[0];

    putStr(ptrBuf, szDays[ds.wDayOfWeek], 3);
    putStr(ptrBuf, ", ", 2);
    putInt(ptrBuf, ds.wDay + 1, 2);
    *ptrBuf++ = ' ';
    putStr(ptrBuf, szMonth[ds.wMonth], 3);
    *ptrBuf++ = ' ';
    putInt(ptrBuf, ds.wYear, 4);
    *ptrBuf++ = ' ';
    putInt(ptrBuf, ds.wHour, 2);
    *ptrBuf++ = ':';
    putInt(ptrBuf, ds.wMinute, 2);
    *ptrBuf++ = ':';
    putInt(ptrBuf, ds.wSecond, 2);
    putStr(ptrBuf, " GMT", 4);
}

void date_t::toX509String(std::string &retVal)
{
    if (isnan(d))
        return;

    _date_split ds(d);

    retVal.resize(14);
    char *ptrBuf = &retVal[0];

    putInt(ptrBuf, ds.wYear, 4);
    putInt(ptrBuf, ds.wMonth + 1, 2);
    putInt(ptrBuf, ds.wDay + 1, 2);
    putInt(ptrBuf, ds.wHour, 2);
    putInt(ptrBuf, ds.wMinute, 2);
    putInt(ptrBuf, ds.wSecond, 2);
}

void date_t::sqlString(std::string &retVal)
{
    if (isnan(d))
        return;

    _date_split ds((double)s_dc->ToLocal((int64_t) d));

    retVal.resize(19);
    char *ptrBuf = &retVal[0];

    putInt(ptrBuf, ds.wYear, 4);
    *ptrBuf++ = '-';
    putInt(ptrBuf, ds.wMonth + 1, 2);
    *ptrBuf++ = '-';
    putInt(ptrBuf, ds.wDay + 1, 2);
    *ptrBuf++ = ' ';
    putInt(ptrBuf, ds.wHour, 2);
    *ptrBuf++ = ':';
    putInt(ptrBuf, ds.wMinute, 2);
    *ptrBuf++ = ':';
    putInt(ptrBuf, ds.wSecond, 2);
}

void date_t::stamp(std::string &retVal)
{
    if (isnan(d))
        return;

    _date_split ds((double)s_dc->ToLocal((int64_t) d));

    retVal.resize(14);
    char *ptrBuf = &retVal[0];

    putInt(ptrBuf, ds.wYear, 4);
    putInt(ptrBuf, ds.wMonth + 1, 2);
    putInt(ptrBuf, ds.wDay + 1, 2);
    putInt(ptrBuf, ds.wHour, 2);
    putInt(ptrBuf, ds.wMinute, 2);
    putInt(ptrBuf, ds.wSecond, 2);
}

void date_t::add(int32_t num, int32_t part)
{
    if (isnan(d))
        return;

    if (part == _SECOND)
        d += (int64_t)num * 1000;
    else if (part == _MINUTE)
        d += (int64_t)num * 60 * 1000;
    else if (part == _HOUR)
        d += (int64_t)num * 60 * 60 * 1000;
    else if (part == _DAY)
        d += (int64_t)num * 60 * 60 * 24 * 1000;
    else
    {
        _date_split ds(d);
        int32_t day = MaxDaysInMonth(ds.wYear, ds.wMonth) - 1;
        bool isLastday = ds.wDay == day;

        if (part == _MONTH)
        {
            if (num >= 12)
            {
                ds.wYear += num / 12;
                num = num % 12;
            }

            ds.wMonth += num;
            if (ds.wMonth > 11)
            {
                ds.wMonth -= 12;
                ds.wYear ++;
            }
        }
        else if (part == _YEAR)
            ds.wYear += num;
        else
            return;

        day = MaxDaysInMonth(ds.wYear, ds.wMonth) - 1;
        if (ds.wDay > day || isLastday)
            ds.wDay = day;

        create(ds.wYear, ds.wMonth + 1, ds.wDay + 1, ds.wHour,
               ds.wMinute, ds.wSecond, ds.wMillisecond);
    }
}

void date_t::fix(int32_t part)
{
    if (isnan(d))
        return;

    if (part == _SECOND)
        d = (double)((int64_t)d / 1000) * 1000;
    else if (part == _MINUTE)
        d = (double)((int64_t)d / (60 * 1000)) * 60 * 1000;
    else if (part == _HOUR)
        d = (double)((int64_t)d / (60 * 60 * 1000)) * 60 * 60 * 1000;
    else if (part == _DAY)
        d = (double)((int64_t)d / (60 * 60 * 24 * 1000)) * 60 * 60 * 24 * 1000;
    else
    {
        _date_split ds(d);

        if (part == _MONTH)
            create(ds.wYear, ds.wMonth + 1, 1, 0, 0, 0, 0);
        else if (part == _YEAR)
            create(ds.wYear, 1, 1, 0, 0, 0, 0);
    }
}

void date_t::toLocal()
{
    if (isnan(d))
        return;

    d = (double) s_dc->ToLocal((int64_t) d);
}

void date_t::toUTC()
{
    if (isnan(d))
        return;

    d = (double) s_dc->ToUTC((int64_t) d);
}

int32_t date_t::timezone()
{
    return s_dc->LocalOffset();
}

}
