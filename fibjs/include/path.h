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

class Path
{
public:
	Path()
	{}

	Path(exlib::string init)
	{
		append(init);
	}

public:
#ifdef _WIN32
	void append(exlib::string other)
	{
		if (other.empty())
			return;

		const char* c_str = other.c_str();

		if (isPathSlash(c_str[0]) && isPathSlash(c_str[1]))
		{
			m_buf = other;
			return;
		}

		char diskId = 0;

		if (qisascii(c_str[0]) && c_str[1] == ':')
		{
			diskId = c_str[0];
			m_buf = m_disks[diskId & 0x1f];
			c_str += 2;
		}

		exlib::string base;
		const char* c_strBuf = m_buf.c_str();
		if (*c_strBuf)
		{
			if (qisascii(c_strBuf[0]) && c_strBuf[1] == ':')
			{
				diskId = c_strBuf[0];
				base = m_buf.substr(2);
			} else
				base = m_buf;
		}

		if (!base.empty() && !isPathSlash(base[base.length() - 1]))
			base.append(1, PATH_SLASH);

		if (isPathSlash(c_str[0]))
			base = c_str;
		else
			base.append(c_str);

		if (diskId)
		{
			m_buf.assign(1, diskId);
			m_buf.append(1, ':');
			m_buf.append(base);
		} else
			m_buf = base;

		if (diskId)
			m_disks[diskId & 0x1f] = m_buf;
	}

private:
	exlib::string m_disks[27];

#else
	void append(exlib::string other)
	{
		if (other.empty())
			return;

		const char* c_str = other.c_str();

		if (isPathSlash(c_str[0]))
			m_buf = other;
		else
		{
			if (!m_buf.empty() && !isPathSlash(m_buf[m_buf.length() - 1]))
				m_buf.append(1, PATH_SLASH);

			m_buf.append(other);
		}
	}

#endif

public:
	exlib::string str()
	{
		return m_buf;
	}

private:
	exlib::string m_buf;
};

inline void pathAdd(exlib::string& strBuffer, exlib::string other)
{
	Path p(strBuffer);

	p.append(other);
	strBuffer = p.str();
}

} /* namespace fibjs */

#endif /* _PATH_UTIL_H_ */
