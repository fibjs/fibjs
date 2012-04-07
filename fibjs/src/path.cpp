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

#define PATH_CHAR	'\\'
#define pathcmpchr qchricmp
#define pathcmp qstricmp

inline bool isSeparator(char ch)
{
	return ch == '/' || ch == '\\';
}

#else

#define PATH_CHAR	'/'
#define pathcmp qstrcmp

inline bool isSeparator(char ch)
{
	return ch == '/';
}

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
	int extlen = qstrlen(ext);

	while (*path)
	{
		ch = *path++;
		if (isSeparator(ch))
			p1 = path;
	}

	if (extlen && ((int) (path - p1) >= extlen)
			&& !pathcmp(ext, path - extlen, extlen))
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
	if (path[0] != 0 && path[1] == ':')
	{
		path += 2;
		if (isSeparator(*path))
			path++;
		p2 = path;
	}
	else if (isSeparator(path[0]) && isSeparator(path[1]))
	{
		path += 2;
		while (*path && !isSeparator(*path))
			path++;

		if (*path)
		{
			path++;

			while (*path && !isSeparator(*path))
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

inline char fixChar(char ch)
{
	return isSeparator(ch) ? PATH_CHAR : ch;
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

		if (isSeparator(*p1))
		{
			pstr[pos++] = PATH_CHAR;
			p1++;
			bRoot = true;
		}
	}
	else if (isSeparator(p1[0]) && isSeparator(p1[1]))
	{
		pstr[pos++] = PATH_CHAR;
		pstr[pos++] = PATH_CHAR;
		p1 += 2;

		while (*p1 && !isSeparator(*p1))
			pstr[pos++] = *p1++;

		if (*p1)
		{
			pstr[pos++] = PATH_CHAR;
			p1++;

			while (*p1 && !isSeparator(*p1))
				pstr[pos++] = *p1++;

			if (*p1)
			{
				pstr[pos++] = PATH_CHAR;
				p1++;
				bRoot = true;
			}
		}
	}
	else
#endif
	if (isSeparator(p1[0]))
	{
		pstr[pos++] = PATH_CHAR;
		p1++;
		bRoot = true;
	}

	root = pos;

	while (*p1)
	{
		if (isSeparator(p1[0]))
		{
			p1++;
		}
		else if (p1[0] == '.' && (!p1[1] || isSeparator(p1[1])))
		{
			p1 += p1[1] ? 2 : 1;
		}
		else if ((p1[0] == '.') && (p1[1] == '.') && (!p1[2] || isSeparator(p1[2])))
		{
			if (pos > root)
			{
				if ((pstr[pos - 2] == '.') && (pstr[pos - 3] == '.')
						&& ((root == pos - 3) || (pstr[pos - 4] == PATH_CHAR)))
				{
					pstr[pos++] = '.';
					pstr[pos++] = '.';
					pstr[pos++] = PATH_CHAR;
				}
				else
				{
					pos--;
					while (pos > root && !isSeparator(pstr[pos - 1]))
						pos--;
				}
			}
			else if (!bRoot)
			{
				pstr[pos++] = '.';
				pstr[pos++] = '.';
				pstr[pos++] = PATH_CHAR;
			}

			p1 += p1[2] ? 3 : 2;
		}
		else
		{
			while (*p1 && !isSeparator(*p1))
				pstr[pos++] = *p1++;
			if (*p1)
			{
				p1++;
				pstr[pos++] = PATH_CHAR;
			}
		}
	}

	if (pos > root && isSeparator(pstr[pos - 1]))
		pos--;

	str.resize(pos);

	retVal = str;

	return 0;
}

result_t path_base::combine(const v8::Arguments& args, std::string& retVal)
{
	v8::HandleScope handle_scope;
	std::stringstream strBuffer;
	int argc = args.Length();
	int i;
	bool bRoot;
	char diskID = 0;

	for (i = 0; i < argc; i++)
	{
		v8::String::Utf8Value s(args[i]);
		const char *p = *s;

		if (p && *p)
		{
#ifdef _WIN32
			if (p[0] != 0 && p[1] == ':')
			{
				if(!pathcmpchr(p[0], diskID) && !isSeparator(p[2]))
				{
					p += 2;
					bRoot = false;
				}else
				{
					bRoot = true;
					diskID = p[0];
				}
			}else if(isSeparator(p[0]) && isSeparator(p[1]))
			{
				bRoot = true;
				diskID = 0;
			}else
#endif
			if (isSeparator(p[0]))
			{
				bRoot = true;
				diskID = 0;
			}else
				bRoot = false;

			if (bRoot)
				strBuffer.str("");

			strBuffer << p;

			if (i < argc - 1 && !isSeparator(p[s.length() - 1]))
				strBuffer << PATH_CHAR;
		}
		else
		{
			strBuffer << '.';
			if (i < argc - 1)
				strBuffer << PATH_CHAR;
		}
	}

	return normalize(strBuffer.str().c_str(), retVal);
}

}
