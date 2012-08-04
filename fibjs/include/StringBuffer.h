/*
 * StringArray.h
 *
 *  Created on: Aug 2, 2012
 *      Author: lion
 */

#include "QuickArray.h"
#include <string>

#ifndef STRINGARRAY_H_
#define STRINGARRAY_H_

namespace fibjs
{

class StringBuffer
{
public:
	StringBuffer() :
			m_size(0)
	{
	}

public:
	std::string str()
	{
		int i, p = 0;

		if (m_array.size() == 1)
			return m_array[0];

		std::string s;
		if (m_size == 0)
			return s;

		s.resize(m_size);
		for (i = 0; i < (int) m_array.size(); i++)
		{
			std::string& s1 = m_array[i];
			size_t len = s1.length();

			memcpy(&s[p], s1.c_str(), len);
			p += (int)len;
		}

		return s;
	}

	size_t size()
	{
		return m_size;
	}

	void append(std::string& s)
	{
		if (s.length() > 0)
		{
			m_size += s.length();
			m_array.append(s);
		}
	}

	void clear()
	{
		m_size = 0;
		m_array.resize(0);
	}

private:
	QuickArray<std::string> m_array;
	size_t m_size;
};

}

#endif /* STRINGARRAY_H_ */
