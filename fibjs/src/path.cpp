/*
 * path.cpp
 *
 *  Created on: Apr 7, 2012
 *      Author: lion
 */

#include "ifs/path.h"
#include <sstream>

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

result_t path_base::basename(const char* path, const char* ext,
		std::string& retVal)
{
	char ch;
	const char *p1 = path;
	int extlen = (int)qstrlen(ext);

	while (*path)
	{
		ch = *path++;
		if (isPathSlash(ch))
			p1 = path;
	}

	if (extlen && ((int) (path - p1) >= extlen)
			&& !pathcmp(ext, path - extlen, extlen))
		path -= extlen;

	retVal.assign(p1, (int) (path - p1));

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
		if (isPathSlash(ch))
		{
			if (*path == '.')
				path++;
			p1 = NULL;
		}
		else if (ch == '.')
			p1 = path - 1;
	}

	if (p1)
		retVal.assign(p1, (int) (path - p1));

	return 0;
}

result_t path_base::dirname(const char* path, std::string& retVal)
{
	char ch;
	const char *p1 = path;
	const char *p2 = NULL;

#ifdef _WIN32
	if (path[0] != 0 && path[1] == ':')
	{
		path += 2;
		if (isPathSlash(*path))
			path++;
		p2 = path;
	}
	else if (isPathSlash(path[0]) && isPathSlash(path[1]))
	{
		path += 2;
		while (*path && !isPathSlash(*path))
			path++;

		if (*path)
		{
			path++;

			while (*path && !isPathSlash(*path))
				path++;

			if (*path)
				path++;
		}

		p2 = path;
	}
#endif

	while (*path)
	{
		ch = *path++;
		if (isPathSlash(ch) && *path)
			p2 = path - 1;
	}

	if (p2 == NULL)
		p2 = p1;

	if (isPathSlash(*p2) && p2 == p1)
		p2++;

	retVal.assign(p1, (int) (p2 - p1));

	return 0;
}

inline char fixChar(char ch)
{
	return isPathSlash(ch) ? PATH_SLASH : ch;
}

result_t path_base::normalize(const char* path, std::string& retVal)
{
	std::string str;
	const char *p1 = path;
	char *pstr;
	int pos = 0;
	int root = 0;
	bool bRoot = false;

	str.resize(4096);
	pstr = &str[0];

#ifdef _WIN32
	if (p1[0] != 0 && p1[1] == ':')
	{
		pstr[pos++] = p1[0];
		pstr[pos++] = ':';
		p1 += 2;

		if (isPathSlash(*p1))
		{
			pstr[pos++] = PATH_SLASH;
			p1++;
			bRoot = true;
		}
	}
	else if (isPathSlash(p1[0]) && isPathSlash(p1[1]))
	{
		pstr[pos++] = PATH_SLASH;
		pstr[pos++] = PATH_SLASH;
		p1 += 2;

		while (*p1 && !isPathSlash(*p1))
			pstr[pos++] = *p1++;

		if (*p1)
		{
			pstr[pos++] = PATH_SLASH;
			p1++;

			while (*p1 && !isPathSlash(*p1))
				pstr[pos++] = *p1++;

			if (*p1)
			{
				pstr[pos++] = PATH_SLASH;
				p1++;
				bRoot = true;
			}
		}
	}
	else
#endif
	if (isPathSlash(p1[0]))
	{
		pstr[pos++] = PATH_SLASH;
		p1++;
		bRoot = true;
	}

	root = pos;

	while (*p1)
	{
		if (isPathSlash(p1[0]))
		{
			p1++;
		}
		else if (p1[0] == '.' && (!p1[1] || isPathSlash(p1[1])))
		{
			p1 += p1[1] ? 2 : 1;
		}
		else if ((p1[0] == '.') && (p1[1] == '.') && (!p1[2] || isPathSlash(p1[2])))
		{
			if (pos > root)
			{
				if ((pstr[pos - 2] == '.') && (pstr[pos - 3] == '.')
						&& ((root == pos - 3) || (pstr[pos - 4] == PATH_SLASH)))
				{
					pstr[pos++] = '.';
					pstr[pos++] = '.';
					pstr[pos++] = PATH_SLASH;
				}
				else
				{
					pos--;
					while (pos > root && !isPathSlash(pstr[pos - 1]))
						pos--;
				}
			}
			else if (!bRoot)
			{
				pstr[pos++] = '.';
				pstr[pos++] = '.';
				pstr[pos++] = PATH_SLASH;
			}

			p1 += p1[2] ? 3 : 2;
		}
		else
		{
			while (*p1 && !isPathSlash(*p1))
				pstr[pos++] = *p1++;
			if (*p1)
			{
				p1++;
				pstr[pos++] = PATH_SLASH;
			}
		}
	}

	if (pos > root && isPathSlash(pstr[pos - 1]))
		pos--;

	str.resize(pos);

	retVal = str;

	return 0;
}

result_t path_base::combine(const v8::Arguments& args, std::string& retVal)
{
	std::string strBuffer;
	int argc = args.Length();
	int i;
	bool bRoot;

#ifdef _WIN32
	char diskID = 0;
#endif

	for (i = 0; i < argc; i++)
	{
		v8::String::Utf8Value s(args[i]);
		const char *p = *s;

		if (p && *p)
		{
#ifdef _WIN32
			if (p[0] != 0 && p[1] == ':')
			{
				if(!pathcmpchr(p[0], diskID) && !isPathSlash(p[2]))
				{
					p += 2;
					bRoot = false;
				}else
				{
					bRoot = true;
					diskID = p[0];
				}
			}else if(isPathSlash(p[0]) && isPathSlash(p[1]))
			{
				bRoot = true;
				diskID = 0;
			}else
#endif
			if (isPathSlash(p[0]))
			{
				bRoot = true;
#ifdef _WIN32
				diskID = 0;
#endif
			}else
				bRoot = false;

			if (bRoot)
				strBuffer.resize(0);

			strBuffer.append(p);

			if (i < argc - 1 && !isPathSlash(p[s.length() - 1]))
				strBuffer.append(1, PATH_SLASH);
		}
		else
		{
			strBuffer.append(1, '.');
			if (i < argc - 1)
				strBuffer.append(1, PATH_SLASH);
		}
	}

	return normalize(strBuffer.c_str(), retVal);
}

}
