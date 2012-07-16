/*
 * encoding.cpp
 *
 *  Created on: Apr 10, 2012
 *      Author: lion
 */

#include "ifs/encoding.h"
#include "Buffer.h"

namespace fibjs
{

inline int utf8_charWidth(unsigned char ch)
{
	static const char utf8_length[128] =
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0x80-0x8f */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0x90-0x9f */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0xa0-0xaf */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0xb0-0xbf */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0xc0-0xcf */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0xd0-0xdf */
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, /* 0xe0-0xef */
	3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 0, 0 /* 0xf0-0xff */
	};

	return utf8_length[ch - 0x80];
}

inline uint32_t utf8_getChar(const char*& src)
{
	static const unsigned char utf8_mask[6] =
	{ 0x7f, 0x1f, 0x0f, 0x07, 0x03, 0x01 };

	static const unsigned int utf8_minval[6] =
	{ 0x0, 0x80, 0x800, 0x10000, 0x200000, 0x4000000 };

	if (!*src)
		return 0;

	unsigned char ch = (unsigned char) *src++;
	if (ch < 0x80)
		return ch;

	int len = utf8_charWidth(ch);
	uint32_t res;

	res = ch & utf8_mask[len];

	switch (len)
	{
	case 5:
		if (!*src)
			return 0;
		if ((ch = (unsigned char) *src ^ 0x80) >= 0x40)
			return '?';

		res = (res << 6) | ch;
		src++;
	case 4:
		if (!*src)
			return 0;
		if ((ch = (unsigned char) *src ^ 0x80) >= 0x40)
			return '?';

		res = (res << 6) | ch;
		src++;
	case 3:
		if (!*src)
			return 0;
		if ((ch = (unsigned char) *src ^ 0x80) >= 0x40)
			return '?';

		res = (res << 6) | ch;
		src++;
	case 2:
		if (!*src)
			return 0;
		if ((ch = (unsigned char) *src ^ 0x80) >= 0x40)
			return '?';

		res = (res << 6) | ch;
		src++;
	case 1:
		if (!*src)
			return 0;
		if ((ch = (unsigned char) *src ^ 0x80) >= 0x40)
			return '?';

		res = (res << 6) | ch;
		src++;

		if (res < utf8_minval[len])
			return '?';
		if (res >= 0x10000)
			return '?';
	}

	return res;
}

inline void baseEncode(const char *pEncodingTable, int dwBits,
		obj_ptr<Buffer_base>& data, std::string& retVal)
{
	std::string strData;
	int i, len = 0, bits = 0;
	int dwData = 0;
	int dwSize = 0;
	char bMask = 0xff >> (8 - dwBits);

	data->toString(strData);

	if (dwBits == 6)
		dwSize = ((int) strData.length() + 2) / 3 * 4;
	else if (dwBits == 5)
		dwSize = ((int) strData.length() + 4) / 5 * 8;

	retVal.resize(dwSize);

	for (i = 0; i < (int) strData.length(); i++)
	{
		dwData <<= 8;
		dwData |= (unsigned char) strData[i];
		bits += 8;

		while (bits >= dwBits)
		{
			retVal[len++] = pEncodingTable[(dwData >> (bits - dwBits)) & bMask];
			bits -= dwBits;
		}
	}

	if (bits)
		retVal[len++] = pEncodingTable[(dwData << (dwBits - bits)) & bMask];

	while (len < dwSize)
		retVal[len++] = '=';

	retVal.resize(len);
}

inline void baseDecode(const char *pdecodeTable, int dwBits,
		const char* baseString, obj_ptr<Buffer_base>& retVal)
{
	int nWritten = 0, len = (int) qstrlen(baseString);
	std::string strBuf;

	strBuf.resize(len * dwBits / 8);

	int dwCurr = 0;
	int nBits = 0;
	uint32_t ch;

	while ((ch = utf8_getChar(baseString)) != 0)
	{
		int nCh = (ch > 0x20 && ch < 0x80) ? pdecodeTable[ch - 0x20] : -1;

		if (nCh != -1)
		{
			dwCurr <<= dwBits;
			dwCurr |= nCh;
			nBits += dwBits;

			while (nBits >= 8)
			{
				strBuf[nWritten++] = (char) (dwCurr >> (nBits - 8));
				nBits -= 8;
			}
		}
	}

	strBuf.resize(nWritten);

	retVal = new Buffer(strBuf);
}

result_t encoding_base::base32Encode(obj_ptr<Buffer_base>& data,
		std::string& retVal)
{
	baseEncode("ABCDEFGHIJKLMNOPQRSTUVWXYZ234567+/", 5, data, retVal);
	return 0;
}

result_t encoding_base::base32Decode(const char* data,
		obj_ptr<Buffer_base>& retVal)
{
	static const char decodeTable[] =
	{ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 2x  !"#$%&'()*+,-./   */
	14, 11, 26, 27, 28, 29, 30, 31, -1, 6, -1, -1, -1, -1, -1, -1, /* 3x 0123456789:;<=>?   */
	-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, /* 4x @ABCDEFGHIJKLMNO   */
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1, /* 5X PQRSTUVWXYZ[\]^_   */
	-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, /* 6x `abcdefghijklmno   */
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1 /* 7X pqrstuvwxyz{\}~DEL */
	};

	baseDecode(decodeTable, 5, data, retVal);
	return 0;
}

result_t encoding_base::base64Encode(obj_ptr<Buffer_base>& data,
		std::string& retVal)
{
	baseEncode(
			"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/",
			6, data, retVal);
	return 0;
}

result_t encoding_base::base64Decode(const char* data,
		obj_ptr<Buffer_base>& retVal)
{
	static const char decodeTable[] =
	{ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, 62, -1, 63, /* 2x  !"#$%&'()*+,-./   */
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1, /* 3x 0123456789:;<=>?   */
	-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, /* 4x @ABCDEFGHIJKLMNO   */
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, 63, /* 5X PQRSTUVWXYZ[\]^_   */
	-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, /* 6x `abcdefghijklmno   */
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1 /* 7X pqrstuvwxyz{\}~DEL */
	};

	baseDecode(decodeTable, 6, data, retVal);
	return 0;
}

result_t encoding_base::hexEncode(obj_ptr<Buffer_base>& data,
		std::string& retVal)
{
	std::string strData;
	static char HexChar[] = "0123456789abcdef";
	int i, pos, len1;

	data->toString(strData);

	i = (int) strData.length() * 2;
	retVal.resize(i);

	len1 = 0;
	pos = 0;

	for (i = 0; i < (int) strData.length(); i++)
	{
		retVal[pos * 2] = HexChar[(unsigned char) strData[i] >> 4];
		retVal[pos * 2 + 1] = HexChar[(unsigned char) strData[i] & 0xf];
		pos++;
		len1 += 2;
	}

	return 0;
}

result_t encoding_base::hexDecode(const char* data,
		obj_ptr<Buffer_base>& retVal)
{
	int pos, len = (int) qstrlen(data);
	std::string strBuf;
	uint32_t ch1, ch2;

	strBuf.resize(len / 2);

	pos = 0;
	while ((ch1 = utf8_getChar(data)) != 0)
	{
		if ((ch1 >= 'a' && ch1 <= 'f') || (ch1 >= 'A' && ch1 <= 'F'))
			ch1 = (ch1 & 0xf) + 9;
		else if (ch1 >= '0' && ch1 <= '9')
			ch1 &= 0xf;
		else
			continue;

		ch2 = utf8_getChar(data);
		if (ch2 == 0)
			break;

		if ((ch2 >= 'a' && ch2 <= 'f') || (ch2 >= 'A' && ch2 <= 'F'))
			ch2 = (ch2 & 0xf) + 9;
		else if (ch2 >= '0' && ch2 <= '9')
			ch2 &= 0xf;
		else
		{
			ch2 = ch1;
			ch1 = 0;
		}

		strBuf[pos++] = (ch1 << 4) + ch2;
	}

	retVal = new Buffer(strBuf);

	return 0;
}

#define HEX_ESCAPE '%'
static const char *URITable =
		" ! #$ &'()*+,-./0123456789:; = ?@ABCDEFGHIJKLMNOPQRSTUVWXYZ    _ abcdefghijklmnopqrstuvwxyz   ~ ";
static const char *URIComponentTable =
		" !     '()*  -. 0123456789       ABCDEFGHIJKLMNOPQRSTUVWXYZ    _ abcdefghijklmnopqrstuvwxyz   ~ ";

static const char *hex = "0123456789ABCDEF";
#define HEXDATA(ch)		((ch) >= '0' && (ch) <= '9' ? (ch) - '0' : \
						(ch) >= 'a' && (ch) <= 'f' ? (ch) - 'a' + 10 : \
						(ch) >= 'A' && (ch) <= 'F' ? (ch) - 'A' + 10 : 0)

inline result_t urlencode(const char* url, std::string& retVal, const char* tab)
{
	int len;
	const char* src;
	unsigned char ch;
	char* bstr;
	std::string str;

	for (len = 0, src = url; (ch = (unsigned char) *src) != 0; src++, len++)
		if (ch < 0x20 || ch >= 0x80 || tab[ch - 0x20] == ' ')
			len += 2;

	str.resize(len);
	bstr = &str[0];

	for (src = url; (ch = (unsigned char) *src) != 0; src++)
	{
		if (ch >= 0x20 && ch < 0x80 && tab[ch - 0x20] != ' ')
			*bstr++ = ch;
		else
		{
			*bstr++ = HEX_ESCAPE;

			*bstr++ = hex[(ch >> 4) & 15];
			*bstr++ = hex[ch & 15];
		}
	}

	retVal = str;

	return 0;
}

result_t encoding_base::encodeURI(const char* url, std::string& retVal)
{
	return urlencode(url, retVal, URITable);
}

result_t encoding_base::encodeURIComponent(const char* url, std::string& retVal)
{
	return urlencode(url, retVal, URIComponentTable);
}

result_t encoding_base::decodeURI(const char* url, std::string& retVal)
{
	int len;
	const char* src;
	unsigned char ch;
	char* bstr;
	std::string str;

	for (len = 0, src = url; (ch = (unsigned char) *src) != 0; src++, len++)
		if (ch == '%' && qishex(src[1]) & qishex(src[2]))
			src += 2;

	str.resize(len);
	bstr = &str[0];

	for (src = url; (ch = (unsigned char) *src) != 0; src++)
	{
		if (ch == '%' && qishex(src[1]) & qishex(src[2]))
		{
			*bstr++ = (HEXDATA(src[1]) << 4) + HEXDATA(src[2]);
			src += 2;
		}
		else
			*bstr++ = ch;
	}

	retVal = str;

	return 0;
}

}

