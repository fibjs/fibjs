/*
 * path.cpp
 *
 *  Created on: Apr 7, 2012
 *      Author: lion
 */

#include "ifs/path.h"

namespace fibjs
{

#ifdef _WIN32

inline bool isSeparator(char ch)
{
	return ch == '/' || ch == '\\';
}

#define pathcmp qstricmp
#define PATH_CHAR	'\\'

#else

inline bool isSeparator(char ch)
{
	return ch == '/';
}

#define pathcmp qstrcmp
#define PATH_CHAR	'/'

#endif

inline char fixChar(char ch)
{
	return isSeparator(ch) ? PATH_CHAR : ch;
}

result_t path_base::normalize(const char* path, std::string& retVal)
{
	std::string str;
	const char *p1 = path;
	const char *p2 = NULL;
	int pos = 0;

	str.resize(4096);

#ifdef _WIN32
	if(path[0] != 0 && path[1] == ':')
	{
		path += 2;
		if(isSeparator(*path))
			path ++;
		p2 = path;
	}else if(isSeparator(path[0]) && isSeparator(path[1]))
	{
		path += 2;
		while(*path && !isSeparator(*path))
			path ++;

		if(*path)
		{
			path++;

			while(*path && !isSeparator(*path))
				path ++;

			if(*path)
				path ++;
		}

		p2 = path;
	}

	while(p1 < p2)
		str[pos ++] = fixChar(*p1++);
#endif

	while(*p1)
		str[pos ++] = fixChar(*p1++);

	str.resize(pos);

	retVal = str;

	return 0;
}

result_t path_base::basename(const char* path, const char* ext,
		std::string& retVal)
{
	char ch;
	const char *p1 = path;
	int extlen = qstrlen(ext);

	while (*path)
	{
		ch = *path++;
		if (isSeparator(ch))
			p1 = path;
	}

	if (extlen && ((int) (path - p1) >= extlen) && !pathcmp(ext, path - extlen,
			extlen))
		path -= extlen;

	retVal.append(p1, (int) (path - p1));

	return 0;
}

result_t path_base::extname(const char* path, std::string& retVal)
{
	char ch;
	const char *p1 = NULL;

	if (*path == '.')
		path++;

	while (*path)
	{
		ch = *path++;
		if (isSeparator(ch))
		{
			if (*path == '.')
				path++;
			p1 = NULL;
		}
		else if (ch == '.')
			p1 = path - 1;
	}

	if (p1)
		retVal.append(p1, (int) (path - p1));

	return 0;
}

result_t path_base::dirname(const char* path, std::string& retVal)
{
	char ch;
	const char *p1 = path;
	const char *p2 = NULL;

#ifdef _WIN32
	if(path[0] != 0 && path[1] == ':')
	{
		path += 2;
		if(isSeparator(*path))
			path ++;
		p2 = path;
	}else if(isSeparator(path[0]) && isSeparator(path[1]))
	{
		path += 2;
		while(*path && !isSeparator(*path))
			path ++;

		if(*path)
		{
			path++;

			while(*path && !isSeparator(*path))
				path ++;

			if(*path)
				path ++;
		}

		p2 = path;
	}
#endif

	while (*path)
	{
		ch = *path++;
		if (isSeparator(ch) && *path)
			p2 = path - 1;
	}

	if (p2 == NULL)
		p2 = path;

	if (isSeparator(*p2) && p2 == p1)
		p2++;

	retVal.append(p1, (int) (p2 - p1));

	return 0;
}

}
