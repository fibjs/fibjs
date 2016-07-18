/*
 * encoding_iconv.h
 *
 *  Created on: Sep 11, 2015
 *      Author: lion
 */

#include "Buffer.h"
#include "utf8.h"

#ifndef ENCODING_H_
#define ENCODING_H_

namespace fibjs
{

inline void baseEncode(const char *pEncodingTable, int32_t dwBits,
                       const char* data, int32_t sz, exlib::string &retVal)
{
	int32_t i, len = 0, bits = 0;
	int32_t dwData = 0;
	int32_t dwSize = 0;
	char bMask = 0xff >> (8 - dwBits);

	if (dwBits == 6)
		dwSize = (sz + 2) / 3 * 4;
	else if (dwBits == 5)
		dwSize = (sz + 4) / 5 * 8;

	retVal.resize(dwSize);

	for (i = 0; i < sz; i++)
	{
		dwData <<= 8;
		dwData |= (unsigned char) data[i];
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

inline void baseEncode(const char *pEncodingTable, int32_t dwBits,
                       exlib::string &data, exlib::string &retVal)
{
	baseEncode(pEncodingTable, dwBits, data.c_str(),
	           (int32_t)data.length(), retVal);
}

inline void baseEncode(const char *pEncodingTable, int32_t dwBits,
                       Buffer_base *data, exlib::string &retVal)
{
	exlib::string strData;
	data->toString(strData);
	baseEncode(pEncodingTable, dwBits, strData, retVal);
}

inline void baseDecode(const char *pdecodeTable, int32_t dwBits,
                       exlib::string& baseString, exlib::string &retVal)
{
	const char * _baseString = baseString.c_str();
	int32_t len = (int32_t) baseString.length();
	int32_t nWritten = 0;
	const char *end = _baseString + len;

	retVal.resize(len * dwBits / 8);

	int32_t dwCurr = 0;
	int32_t nBits = 0;
	uint32_t ch;

	while ((ch = utf8_getchar(_baseString, end)) != 0)
	{
		int32_t nCh = (ch > 0x20 && ch < 0x80) ? pdecodeTable[ch - 0x20] : -1;

		if (nCh != -1)
		{
			dwCurr <<= dwBits;
			dwCurr |= nCh;
			nBits += dwBits;

			while (nBits >= 8)
			{
				retVal[nWritten++] = (char) (dwCurr >> (nBits - 8));
				nBits -= 8;
			}
		}
	}

	retVal.resize(nWritten);
}

inline void baseDecode(const char *pdecodeTable, int32_t dwBits,
                       exlib::string& baseString, obj_ptr<Buffer_base> &retVal)
{
	exlib::string strBuf;
	baseDecode(pdecodeTable, dwBits, baseString, strBuf);
	retVal = new Buffer(strBuf);
}

} /* namespace fibjs */
#endif /* ENCODING_H_ */
