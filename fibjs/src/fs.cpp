/*
 * fs.cpp
 *
 *  Created on: Apr 7, 2012
 *      Author: lion
 */

#include "object.h"

#include <string.h>

#include "utf8.h"
#include "ifs/fs.h"
#include "File.h"
#include "Stat.h"
#include "ObjectArray.h"

namespace fibjs
{

result_t fs_base::open(const char* fname, const char* mode,
		obj_ptr<File_base>& retVal, AsyncCall* ac)
{
	obj_ptr<File> pFile = new File;
	result_t hr;

	hr = pFile->Open(fname, mode);
	if (hr < 0)
		return hr;

	retVal = pFile;

	return 0;
}

result_t fs_base::create(const char* fname, bool Overwrite,
		obj_ptr<File_base>& retVal, AsyncCall* ac)
{
	return 0;
}

result_t fs_base::tmpFile(obj_ptr<File_base>& retVal, AsyncCall* ac)
{
	retVal = new File();
	return 0;
}

result_t fs_base::readFile(const char* fname, std::string& retVal, AsyncCall* ac)
{
	obj_ptr<File> f = new File;
	obj_ptr<Buffer_base> buf;
	result_t hr;

	hr = f->Open(fname, "r");
	if (hr < 0)
		return hr;

	hr = f->read(-1, buf, NULL);
	if(hr < 0)
		return hr;

	return buf->toString(retVal);
}

result_t fs_base::writeFile(const char* fname, const char* txt, AsyncCall* ac)
{
	obj_ptr<File> pFile = new File;
	result_t hr;

	hr = pFile->Open(fname, "w");
	if (hr < 0)
		return hr;

	return pFile->Write(txt, (int)qstrlen(txt));
}

result_t fs_base::stat(const char* path, obj_ptr<Stat_base>& retVal, AsyncCall* ac)
{
	obj_ptr<Stat> pStat = new Stat();

	result_t hr = pStat->getStat(path);
	if(hr < 0)
		return hr;

	retVal = pStat;

	return 0;
}

}

#ifndef _WIN32
#include <dirent.h>

namespace fibjs
{

result_t fs_base::exists(const char* path, bool& retVal, AsyncCall* ac)
{
	retVal = access(path, F_OK) == 0;
	return 0;
}

result_t fs_base::unlink(const char* path, AsyncCall* ac)
{
	if(::unlink(path))
		return LastError();

	return 0;
}

result_t fs_base::mkdir(const char* path, AsyncCall* ac)
{
	if (::mkdir(path, 715))
		return LastError();

	return 0;
}

result_t fs_base::rmdir(const char* path, AsyncCall* ac)
{
	if (::rmdir(path))
		return LastError();

	return 0;
}

result_t fs_base::rename(const char* from, const char* to, AsyncCall* ac)
{
	if (::rename(from, to))
		return LastError();

	return 0;
}

result_t fs_base::readdir(const char* path, obj_ptr<ObjectArray_base>& retVal, AsyncCall* ac)
{
	DIR * dp;
	struct dirent * ep;
	std::string fpath;
	result_t hr;
	obj_ptr<ObjectArray> oa;

	dp = ::opendir(path);
	if (dp == NULL)
		return LastError();

	oa = new ObjectArray();

	while ((ep = ::readdir(dp)))
	{
		obj_ptr<Stat_base> fstat;

		fpath = path;
		fpath += '/';
		fpath += ep->d_name;

		hr = stat(fpath.c_str(), fstat, NULL);
		if (hr >= 0)
			oa->push(fstat);
	}
	::closedir(dp);

	retVal = oa;

	return 0;
}

}

#else

#include <direct.h>

namespace fibjs
{

result_t fs_base::exists(const char* path, bool& retVal, AsyncCall* ac)
{
	retVal = _waccess(UTF8_W(path), 0) == 0;
	return 0;
}

result_t fs_base::unlink(const char* path, AsyncCall* ac)
{
	if(::_wunlink(UTF8_W(path)))
		return LastError();

	return 0;
}

result_t fs_base::mkdir(const char* path, AsyncCall* ac)
{
	if (::_wmkdir(UTF8_W(path)))
		return LastError();

	return 0;
}

result_t fs_base::rmdir(const char* path, AsyncCall* ac)
{
	if (::_wrmdir(UTF8_W(path)))
		return LastError();

	return 0;
}

result_t fs_base::rename(const char* from, const char* to, AsyncCall* ac)
{
	if (::_wrename(UTF8_W(from), UTF8_W(to)))
		return LastError();
	
	return 0;
}

result_t fs_base::readdir(const char* path, obj_ptr<ObjectArray_base>& retVal, AsyncCall* ac)
{
	WIN32_FIND_DATAW fd;
	HANDLE hFind;
	std::wstring fpath;
	obj_ptr<ObjectArray> oa;

	fpath = utf8to16String(path);
	fpath.append(L"/*", 2);

	hFind = FindFirstFileW(fpath.c_str(), &fd);
	if (hFind == INVALID_HANDLE_VALUE)
		return LastError();

	oa = new ObjectArray();

	do
	{
		obj_ptr<Stat> pStat = new Stat();
		pStat->fillStat(fd);
		oa->push(pStat);
	}while(FindNextFileW(hFind, &fd));

	FindClose(hFind);

	retVal = oa;

	return 0;
}

}

#endif

