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

#ifdef _WIN32
#define UTF16_CHAR wchar_t
typedef std::basic_string<UTF16_CHAR> utf16string;
#else
#define UTF16_CHAR unsigned short
typedef std::basic_string<UTF16_CHAR> utf16string;
#endif

int utf8_mbstowcs(const char *src, int srclen, UTF16_CHAR *dst, int dstlen);
int utf8_wcstombs(const UTF16_CHAR *src, int srclen, char *dst, int dstlen);


template<typename T>
size_t _utf8_qstrlen(const T *pStr)
{
	const T *pEnd;

	for (pEnd = pStr; *pEnd != 0; pEnd++)
		continue;

	return pEnd - pStr;
}

inline utf16string utf8to16String(const char *src, int srclen = -1)
{
	utf16string str;

	if(srclen == -1)
		srclen = (int)_utf8_qstrlen(src);

	int n = utf8_mbstowcs(src, srclen, NULL, 0);
	str.resize(n);

	utf8_mbstowcs(src, srclen, &str[0], n);

	return str;
}

#define UTF8_W(s) utf8to16String(s).c_str()

inline std::string utf16to8String(const UTF16_CHAR *src, int srclen = -1)
{
	std::string str;

	if(srclen == -1)
		srclen = (int)_utf8_qstrlen(src);

	int n = utf8_wcstombs(src, srclen, NULL, 0);
	str.resize(n);

	utf8_wcstombs(src, srclen, &str[0], n);

	return str;
}

#define UTF8_A(s) utf16to8String(s).c_str()

#endif
