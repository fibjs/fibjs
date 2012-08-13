/*
 * HttpCollection.cpp
 *
 *  Created on: Aug 9, 2012
 *      Author: lion
 */

#include "HttpCollection.h"

namespace fibjs
{

size_t HttpCollection::size()
{
	size_t sz = 0;
	int32_t i;

	for (i = 0; i < m_count; i++)
		sz += m_array[i * 2].length() + m_array[i * 2 + 1].length() + 4;

	return sz;
}

inline void cp(char* buf, size_t sz, size_t& pos, const char* str, size_t szStr)
{
	buf += pos;

	pos += szStr;
	if (pos > sz)
	{
		szStr -= pos - sz;
		pos = sz;
	}

	memcpy(buf, str, szStr);
}

size_t HttpCollection::getData(char* buf, size_t sz)
{
	size_t pos = 0;
	int32_t i;

	for (i = 0; i < m_count; i++)
	{
		std::string& n = m_array[i * 2];
		std::string& v = m_array[i * 2 + 1];

		cp(buf, sz, pos, n.c_str(), n.length());
		cp(buf, sz, pos, ": ", 2);
		cp(buf, sz, pos, v.c_str(), v.length());
		cp(buf, sz, pos, "\r\n", 2);
	}

	return pos;
}

result_t HttpCollection::clear()
{
	m_count = 0;
	return 0;
}

result_t HttpCollection::has(const char* name, bool& retVal)
{
	int32_t i;

	retVal = false;
	for (i = 0; i < m_count; i++)
		if (!qstrcmp(m_array[i * 2].c_str(), name))
		{
			retVal = true;
			break;
		}

	return 0;
}

result_t HttpCollection::first(const char* name, std::string& retVal)
{
	int32_t i;

	for (i = 0; i < m_count; i++)
		if (!qstrcmp(m_array[i * 2].c_str(), name))
		{
			retVal = m_array[i * 2 + 1];
			return 0;
		}

	return CALL_RETURN_NULL;
}

result_t HttpCollection::all(const char* name, v8::Handle<v8::Array>& retVal)
{
	int32_t i, n = 0;

	retVal = v8::Array::New();

	for (i = 0; i < m_count; i++)
		if (!qstrcmp(m_array[i * 2].c_str(), name))
		{
			std::string& v = m_array[i * 2 + 1];
			retVal->Set(n++, v8::String::New(v.c_str(), (int) v.length()));
		}

	return 0;
}

inline result_t _map(HttpCollection* o, v8::Handle<v8::Object> m,
		result_t (HttpCollection::*fn)(const char* name, const char* value))
{
	v8::Handle<v8::Array> ks = m->GetPropertyNames();
	int len = ks->Length();
	int i;

	for (i = 0; i < len; i++)
	{
		v8::Handle<v8::Value> k = ks->Get(i);
		v8::Handle<v8::Value> v = m->Get(k);

		(o->*fn)(*v8::String::Utf8Value(k), *v8::String::Utf8Value(v));
	}

	return 0;
}

result_t HttpCollection::add(const char* name, const char* value)
{
	m_array[m_count * 2] = name;
	m_array[m_count * 2 + 1] = value;
	m_count++;

	return 0;
}

result_t HttpCollection::add(v8::Handle<v8::Object> map)
{
	return _map(this, map, &HttpCollection::add);
}

result_t HttpCollection::set(const char* name, const char* value)
{
	int32_t i;
	bool bFound = false;

	for (i = 0; i < m_count; i++)
		if (!qstrcmp(m_array[i * 2].c_str(), name))
		{
			m_array[i * 2 + 1] = value;
			bFound = true;
			break;
		}

	if (bFound)
	{
		int32_t p = ++i;

		for (; i < m_count; i++)
			if (qstrcmp(m_array[i * 2].c_str(), name))
			{
				if (i != p)
				{
					m_array[p * 2] = m_array[i * 2];
					m_array[p * 2 + 1] = m_array[i * 2 + 1];
				}

				p++;
			}

		m_count = p;
	}
	else
		add(name, value);

	return 0;
}

result_t HttpCollection::set(v8::Handle<v8::Object> map)
{
	return _map(this, map, &HttpCollection::set);
}

result_t HttpCollection::remove(const char* name)
{
	int32_t i;
	int32_t p = 0;

	for (i = 0; i < m_count; i++)
		if (qstrcmp(m_array[i * 2].c_str(), name))
		{
			if (i != p)
			{
				m_array[p * 2] = m_array[i * 2];
				m_array[p * 2 + 1] = m_array[i * 2 + 1];
			}

			p++;
		}

	m_count = p;

	return 0;
}

result_t HttpCollection::_named_getter(const char* property,
		std::string& retVal)
{
	return first(property, retVal);
}

result_t HttpCollection::_named_setter(const char* property, const char* newVal)
{
	return set(property, newVal);
}

} /* namespace fibjs */
