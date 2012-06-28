/*
 * fs.cpp
 *
 *  Created on: Apr 7, 2012
 *      Author: lion
 */

#include "object.h"

#include <string.h>

#include "ifs/io.h"
#include "File.h"

namespace fibjs
{

result_t io_base::open(const char* fname, const char* mode,
		obj_ptr<File_base>& retVal, exlib::AsyncEvent* ac)
{
	if(!ac)
		return CALL_E_NOSYNC;

	obj_ptr<File> pFile = new File();
	result_t hr;

	hr = pFile->open(fname, mode, ac);
	if (hr < 0)
		return hr;

	retVal = pFile;

	return 0;
}

result_t io_base::create(const char* fname, bool Overwrite,
		obj_ptr<File_base>& retVal, exlib::AsyncEvent* ac)
{
	if(!ac)
		return CALL_E_NOSYNC;

	return 0;
}

result_t io_base::tmpFile(obj_ptr<File_base>& retVal, exlib::AsyncEvent* ac)
{
	if(!ac)
		return CALL_E_NOSYNC;

	retVal = new File();
	return 0;
}

result_t io_base::readFile(const char* fname, std::string& retVal, exlib::AsyncEvent* ac)
{
	if(!ac)
		return CALL_E_NOSYNC;

	obj_ptr<File> f = new File();
	obj_ptr<Buffer_base> buf;
	result_t hr;

	hr = f->open(fname, "r", ac);
	if (hr < 0)
		return hr;

	hr = f->read(-1, buf, ac);
	f->close(ac);

	if(hr < 0 || !buf)
		return hr;

	return buf->toString(retVal);
}

result_t io_base::writeFile(const char* fname, const char* txt, exlib::AsyncEvent* ac)
{
	if(!ac)
		return CALL_E_NOSYNC;

	obj_ptr<File> f = new File();
	result_t hr;

	hr = f->open(fname, "w", ac);
	if (hr < 0)
		return hr;

	hr = f->Write(txt, (int)qstrlen(txt));
	f->close(ac);

	return hr;
}

}
