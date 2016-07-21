/*
 * path.h
 *
 *  Created on: Dec 7, 2015
 *      Author: lion
 */

#ifndef _PATH_UTIL_H_
#define _PATH_UTIL_H_

#include "ifs/path.h"

namespace fibjs
{

#ifdef _WIN32

#define pathcmpchr qchricmp
#define pathcmp qstricmp

#else

#define pathcmp qstrcmp

inline bool pathcmpchr(char ch1, char ch2)
{
	return ch1 - ch2;
}

#endif

inline void pathAdd(exlib::string& strBuffer, exlib::string other)
{
	bool bRoot;

	if (!other.empty())
	{
		const char* c_str = other.c_str();
#ifdef _WIN32
		char diskID = 0;

		if (strBuffer[0] != 0 && strBuffer[1] == ':')
			diskID = strBuffer[0];

		if (c_str[0] != 0 && c_str[1] == ':')
		{
			if (!pathcmpchr(c_str[0], diskID) && !isPathSlash(c_str[2]))
			{
				c_str += 2;
				bRoot = false;
			}
			else
				bRoot = true;
		}
		else if (isPathSlash(c_str[0]) && isPathSlash(c_str[1]))
			bRoot = true;
		else
#endif
			if (isPathSlash(c_str[0]))
				bRoot = true;
			else
				bRoot = false;

		if (bRoot)
			strBuffer.clear();
		else if (!strBuffer.empty() && !isPathSlash(strBuffer[strBuffer.length() - 1]))
			strBuffer.append(1, PATH_SLASH);

		strBuffer.append(c_str);
	}
}

} /* namespace fibjs */

#endif /* _PATH_UTIL_H_ */
