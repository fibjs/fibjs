/*
 * File.cpp
 *
 *  Created on: Apr 7, 2012
 *      Author: lion
 */

#include "object.h"

#ifdef SEEK_SET
#undef SEEK_SET
#undef SEEK_CUR
#undef SEEK_END
#endif

#include "File.h"
#include "Buffer.h"
#include "Stat.h"

namespace fibjs
{

result_t File::read(double bytes, obj_ptr<Buffer_base>& retVal)
{
	if (!m_file)
		return CALL_E_INVALID_CALL;

	std::string strBuf;
	result_t hr;

	if (bytes < 0)
	{
		hr = this->size(bytes);
		if (hr < 0)
			return hr;
	}

	if (bytes > 0)
	{
		strBuf.resize(bytes);
		int sz = bytes;
		char* p = &strBuf[0];

		while (sz && !feof(m_file))
		{
			int n = fread(p, 1, sz, m_file);
			sz -= n;
			p += n;

			if (n == 0 && ferror(m_file))
				return LastError();
		}

		strBuf.resize(bytes - sz);
	}

	retVal = new Buffer(strBuf);

	return 0;
}

result_t File::write(obj_ptr<Buffer_base> data)
{
	if (!m_file)
		return CALL_E_INVALID_CALL;

	std::string strBuf;

	data->toString(strBuf);

	int sz = strBuf.length();
	const char* p = strBuf.c_str();

	while (sz)
	{
		int n = fwrite(p, 1, sz, m_file);
		sz -= n;
		p += n;

		if (n == 0 && ferror(m_file))
			return LastError();
	}

	return 0;
}

result_t File::Open(const char* fname, const char* mode)
{
	char m[] = "rb\0";

	if ((*mode != 'a' && *mode != 'r' && *mode != 'w')
			|| (mode[1] != '+' && mode[1]))
		return CALL_E_INVALIDARG;

	m[0] = mode[0];
	m[2] = mode[1];

	close();

	m_file = fopen(fname, m);
	if (m_file == NULL)
		return LastError();

	name = fname;

	return 0;
}

result_t File::get_name(std::string& retVal)
{
	if (!m_file)
		return CALL_E_INVALID_CALL;

	retVal = name;
	return 0;
}

result_t File::stat(obj_ptr<Stat_base>& retVal)
{
	if (!m_file)
		return CALL_E_INVALID_CALL;

	struct stat st;
	fstat(fileno(m_file), &st);

	obj_ptr<Stat> pStat = new Stat(name.c_str(), st);
	retVal = pStat;

	return 0;
}

result_t File::size(double& retVal)
{
	if (!m_file)
		return CALL_E_INVALID_CALL;

	long p = ftell(m_file);
	if (0 == fseek(m_file, 0, SEEK_END))
	{
		retVal = ftell(m_file);
		fseek(m_file, p, SEEK_SET);
	}
	else
		return LastError();

	return 0;
}

result_t File::eof(bool& retVal)
{
	if (!m_file)
		return CALL_E_INVALID_CALL;

	retVal = feof(m_file) != 0;

	if (ferror(m_file))
		return LastError();

	return 0;
}

result_t File::seek(double offset, int32_t whence)
{
	if (!m_file)
		return CALL_E_INVALID_CALL;

	if (fseek(m_file, offset, whence) < 0)
		return LastError();

	return 0;
}

result_t File::tell(double& retVal)
{
	if (!m_file)
		return CALL_E_INVALID_CALL;

	retVal = ftell(m_file);

	if (ferror(m_file))
		return LastError();

	return 0;
}

result_t File::rewind()
{
	return seek(0, SEEK_SET);
}

result_t File::flush()
{
	if (!m_file)
		return CALL_E_INVALID_CALL;

	fflush(m_file);

	if (ferror(m_file))
		return LastError();

	return 0;
}

result_t File::close()
{
	if (m_file)
	{
		fclose(m_file);
		m_file = NULL;
	}

	return 0;
}

result_t File::truncate(double bytes)
{
	if (!m_file)
		return CALL_E_INVALID_CALL;

	if (ftruncate(fileno(m_file), bytes) < 0)
		return LastError();

	return 0;
}

}
