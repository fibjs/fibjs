/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _utf_8_H_
#define _utf_8_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include <string>
#include "qstring.h"

int utf8_mbstowcs(const char *src, int srclen, wchar_t *dst, int dstlen);
int utf8_wcstombs(const wchar_t *src, int srclen, char *dst, int dstlen);
wchar_t utf8_getchar(const char*& src);

inline int utf8_strlen(const char *src, int srclen)
{
	if (srclen == -1)
		srclen = (int) qstrlen(src);

	return utf8_mbstowcs(src, srclen, NULL, 0);
}

inline int utf8_strlen(const wchar_t *src, int srclen)
{
	if (srclen == -1)
		srclen = (int) qstrlen(src);

	return utf8_wcstombs(src, srclen, NULL, 0);
}

inline std::wstring utf8to16String(const char *src, int srclen = -1)
{
	std::wstring str;

	if (srclen == -1)
		srclen = (int) qstrlen(src);

	if(srclen)
	{
		int n = utf8_mbstowcs(src, srclen, NULL, 0);
		str.resize(n);

		utf8_mbstowcs(src, srclen, &str[0], n);
	}

	return str;
}

inline std::wstring utf8to16String(std::string src)
{
	return utf8to16String(src.c_str(), (int)src.length());
}

#define UTF8_W(s) utf8to16String(s).c_str()

inline std::string utf16to8String(const wchar_t *src, int srclen = -1)
{
	std::string str;

	if (srclen == -1)
		srclen = (int) qstrlen(src);

	if(srclen)
	{
		int n = utf8_wcstombs(src, srclen, NULL, 0);
		str.resize(n);

		utf8_wcstombs(src, srclen, &str[0], n);
	}

	return str;
}

inline std::string utf16to8String(std::wstring src)
{
	return utf16to8String(src.c_str(), (int)src.length());
}

#define UTF8_A(s) utf16to8String(s).c_str()

#endif
