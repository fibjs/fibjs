/*
 * Regex.cpp
 *
 *  Created on: Aug 18, 2012
 *      Author: lion
 */

#include "utf8.h"
#include "Regex.h"
#include "ifs/re.h"

namespace fibjs
{

result_t re_base::compile(const char* pattern, const char* opt,
		obj_ptr<Regex_base>& retVal)
{
	obj_ptr<Regex> re = new Regex();
	result_t hr = re->compile(pattern, opt);
	if (hr < 0)
		return hr;

	retVal = re;
	return 0;
}

result_t Regex::compile(const char* pattern, const char* opt)
{
	int o = PCRE_JAVASCRIPT_COMPAT | PCRE_EXTRA | PCRE_UTF8
			| PCRE_NEWLINE_ANYCRLF | PCRE_UCP;
	const char *error;
	int erroffset;
	char ch;

	reset();

	while ((ch = *opt++) != 0)
	{
		switch (ch)
		{
		case 'g':
			m_bGlobal = true;
			break;
		case 'i':
			m_bIgnoreCase = true;
			break;
		case 'm':
			m_bMultiline = true;
			break;
		}
	}

	if (m_bMultiline)
		o |= PCRE_MULTILINE;
	if (m_bIgnoreCase)
		o |= PCRE_CASELESS;

	m_re = pcre_compile(pattern, o, &error, &erroffset, NULL);

	if (m_re == NULL)
	{
		char buf[1024];

		sprintf(buf, "Compilation failed at offset %d: %s.", erroffset, error);
		return Runtime::setError(buf);
	}

	return 0;
}

#define RE_SIZE	32
result_t Regex::exec(const char* str, v8::Handle<v8::Array>& retVal)
{
	int rc = 0;
	int ovector[RE_SIZE];
	int len = 0;
	int i;

	if (m_bGlobal)
	{
		int n = m_nlastIndex;
		while (n > 0 && utf8_getchar(str))
			n--;
	}

	if (*str)
	{
		len = (int) qstrlen(str);

		rc = pcre_exec(m_re, NULL, str, len, 0, 0, ovector, RE_SIZE);
		if (rc < 0)
		{
			rc = 0;
			m_nlastIndex = 0;
		}
	}

	if (rc)
	{
		retVal = v8::Array::New(rc);

		for (i = 0; i < rc; i++)
			retVal->Set(i,
					v8::String::New(str + ovector[2 * i],
							ovector[2 * i + 1] - ovector[2 * i]));

		if (m_bGlobal)
			m_nlastIndex += utf8_strlen(str, ovector[2 * rc - 1]);
	}

	return rc ? 0 : CALL_RETURN_NULL;
}

result_t Regex::test(const char* str, bool& retVal)
{
	int rc = 0;
	int ovector[RE_SIZE];
	int len = 0;

	if (m_bGlobal)
	{
		int n = m_nlastIndex;
		while (n > 0 && utf8_getchar(str))
			n--;
	}

	if (*str)
	{
		len = (int) qstrlen(str);

		rc = pcre_exec(m_re, NULL, str, len, 0, 0, ovector, RE_SIZE);
		if (rc < 0)
		{
			rc = 0;
			m_nlastIndex = 0;
		}
	}

	if (rc)
	{
		retVal = true;
		if (m_bGlobal)
			m_nlastIndex += utf8_strlen(str, ovector[2 * rc - 1]);
	}

	return 0;
}

result_t Regex::get_lastIndex(int32_t& retVal)
{
	retVal = m_nlastIndex;
	return 0;
}

result_t Regex::set_lastIndex(int32_t newVal)
{
	m_nlastIndex = newVal;
	return 0;
}

result_t Regex::get_global(bool& retVal)
{
	retVal = m_bGlobal;
	return 0;
}

result_t Regex::get_ignoreCase(bool& retVal)
{
	retVal = m_bIgnoreCase;
	return 0;
}

result_t Regex::get_multiline(bool& retVal)
{
	retVal = m_bMultiline;
	return 0;
}

} /* namespace fibjs */
