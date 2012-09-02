/*
 * HttpUploadCollection.cpp
 *
 *  Created on: Aug 28, 2012
 *      Author: lion
 */

#include "HttpUploadCollection.h"
#include "HttpUploadData.h"
#include "MemoryStream.h"

namespace fibjs
{

void HttpUploadCollection::parse(std::string& str, const char* boundary)
{
	const char* pstr = str.c_str();
	int nSize = (int) str.length();
	std::string strName;
	std::string strFileName;
	std::string strContentType;
	const char *p, *p1, *p2, *szQueryString;
	const char *pstrSplit;
	int uiSplitSize;
	char ch;

	boundary += 20;
	while (*boundary && *boundary == ' ')
		boundary++;

	if (qstricmp(boundary, "boundary=", 9))
		return;

	boundary += 9;
	uiSplitSize = (int) qstrlen(boundary);

	pstrSplit = szQueryString = pstr;

	if (nSize < uiSplitSize + 2 || szQueryString[0] != '-'
			|| szQueryString[1] != '-'
			|| qstrcmp(szQueryString + 2, boundary, uiSplitSize))
		return;

	uiSplitSize += 2;
	szQueryString += uiSplitSize;
	nSize -= uiSplitSize;

	while (nSize)
	{
		strFileName.clear();
		strContentType.clear();

		while (nSize > 0)
		{
			ch = *szQueryString++;
			nSize--;
			if (ch != '\r')
				return;
			if (nSize > 0 && *szQueryString == '\n')
			{
				nSize--;
				szQueryString++;
			}

			p = szQueryString;
			while (nSize > 0 && *p != '\r')
			{
				nSize--;
				p++;
			}

			if (nSize == 0)
				break;

			p1 = szQueryString;
			szQueryString = p;

			if (p != p1)
			{
				if (p1 + 20 < p && !qstricmp(p1, "Content-Disposition:", 20))
				{
					p1 += 20;
					while (p1 < p && *p1 == ' ')
						p1++;
					if (p1 + 10 >= p || qstricmp(p1, "form-data;", 10))
						return;

					p1 += 10;
					while (p1 < p && *p1 == ' ')
						p1++;
					if (p1 + 5 >= p || qstricmp(p1, "name=", 5))
						return;

					p1 += 5;

					while (p1 < p && *p1 == ' ')
						p1++;

					ch = ';';
					if (*p1 == '\"')
					{
						p1++;
						ch = '\"';
					}

					p2 = p1;
					while (p1 < p && *p1 != ch)
						p1++;

					strName.assign(p2, (int) (p1 - p2));

					if (p1 < p && *p1 == '\"')
						p1++;

					if (p1 < p && *p1 == ';')
						p1++;

					while (p1 < p && *p1 == ' ')
						p1++;

					if (p1 + 9 < p && !qstricmp(p1, "filename=", 9))
					{
						p1 += 9;

						while (p1 < p && *p1 == ' ')
							p1++;

						ch = ';';
						if (*p1 == '\"')
						{
							p1++;
							ch = '\"';
						}

						p2 = p1;
						while (p1 < p && *p1 != ch)
						{
							if (*p1 == '/' || *p1 == '\\')
								p2 = p1 + 1;
							p1++;
						}

						strFileName.assign(p2, (int) (p1 - p2));
					}
				}
				else if (p1 + 13 < p && !qstricmp(p1, "Content-Type:", 13))
				{
					p1 += 13;
					while (p1 < p && *p1 == ' ')
						p1++;
					strContentType.assign(p1, (int) (p - p1));
				}
			}
			else
			{
				ch = *szQueryString++;
				nSize--;
				if (ch != '\r')
					return;
				if (nSize > 0 && *szQueryString == '\n')
				{
					nSize--;
					szQueryString++;
				}
				break;
			}
		}

		p = szQueryString;
		p1 = p + nSize;
		while (p1 > p && (p = (char*) memchr(p, '-', p1 - p))
				&& p1 > p + uiSplitSize && memcmp(p, pstrSplit, uiSplitSize))
			p++;

		if (!p || p1 <= p + uiSplitSize)
			break;

		nSize = (int) (p1 - p - uiSplitSize);
		p1 = szQueryString;
		szQueryString = p + uiSplitSize;

		p--;
		ch = *p;
		if (ch != '\n')
			return;
		if (*(p - 1) == '\r')
			p--;

		if (!strName.empty())
		{
			int uiSize = (int) (p - p1);
			std::string strTemp;
			Variant varTemp;

			strTemp.assign(p1, uiSize);

			if (strFileName.empty())
				varTemp = strTemp;
			else
			{
				obj_ptr<HttpUploadData> objTemp = new HttpUploadData();
				date_t tm;

				objTemp->m_name = strFileName;
				objTemp->m_type = strContentType;
				objTemp->m_body = new MemoryStream::CloneStream(strTemp, tm);

				varTemp = objTemp;
			}

			add(strName.c_str(), varTemp);
		}
	}
}

result_t HttpUploadCollection::clear()
{
	m_count = 0;
	return 0;
}

result_t HttpUploadCollection::has(const char* name, bool& retVal)
{
	int32_t i;

	retVal = false;
	for (i = 0; i < m_count; i++)
		if (!qstricmp(m_names[i].c_str(), name))
		{
			retVal = true;
			break;
		}

	return 0;
}

result_t HttpUploadCollection::first(const char* name, Variant& retVal)
{
	int32_t i;

	for (i = 0; i < m_count; i++)
		if (!qstricmp(m_names[i].c_str(), name))
		{
			retVal = m_values[i];
			return 0;
		}

	return CALL_RETURN_NULL;
}

result_t HttpUploadCollection::all(const char* name,
		v8::Handle<v8::Array>& retVal)
{
	int32_t i, n = 0;

	retVal = v8::Array::New();

	for (i = 0; i < m_count; i++)
		if (!qstricmp(m_names[i].c_str(), name))
			retVal->Set(n++, m_values[i]);

	return 0;
}

inline result_t _map(HttpUploadCollection* o, v8::Handle<v8::Object> m,
		result_t (HttpUploadCollection::*fn)(const char* name, Variant value))
{
	v8::Handle<v8::Array> ks = m->GetPropertyNames();
	int len = ks->Length();
	int i;
	result_t hr;

	for (i = 0; i < len; i++)
	{
		v8::Handle<v8::Value> k = ks->Get(i);
		hr = (o->*fn)(*v8::String::Utf8Value(k), m->Get(k));
		if (hr < 0)
			return hr;
	}

	return 0;
}

result_t HttpUploadCollection::add(const char* name, Variant value)
{
	m_names[m_count] = name;
	m_values[m_count] = value;
	m_count++;

	return 0;
}

result_t HttpUploadCollection::add(v8::Handle<v8::Object> map)
{
	return _map(this, map, &HttpUploadCollection::add);
}

result_t HttpUploadCollection::set(const char* name, Variant value)
{
	int32_t i;
	bool bFound = false;

	for (i = 0; i < m_count; i++)
		if (!qstricmp(m_names[i].c_str(), name))
		{
			m_values[i] = value;
			bFound = true;
			break;
		}

	if (bFound)
	{
		int32_t p = ++i;

		for (; i < m_count; i++)
			if (qstricmp(m_names[i].c_str(), name))
			{
				if (i != p)
				{
					m_names[p] = m_names[i];
					m_values[p] = m_values[i];
				}

				p++;
			}

		m_count = p;
	}
	else
		return add(name, value);

	return 0;
}

result_t HttpUploadCollection::set(v8::Handle<v8::Object> map)
{
	return _map(this, map, &HttpUploadCollection::set);
}

result_t HttpUploadCollection::remove(const char* name)
{
	int32_t i;
	int32_t p = 0;

	for (i = 0; i < m_count; i++)
		if (qstricmp(m_names[i].c_str(), name))
		{
			if (i != p)
			{
				m_names[p] = m_names[i];
				m_values[p] = m_values[i];
			}

			p++;
		}

	m_count = p;

	return 0;
}

result_t HttpUploadCollection::_named_getter(const char* property,
		Variant& retVal)
{
	return first(property, retVal);
}

result_t HttpUploadCollection::_named_setter(const char* property,
		Variant newVal)
{
	return set(property, newVal);
}

} /* namespace fibjs */
