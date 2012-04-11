/*
 * fs.cpp
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

#include <string.h>
#include <dirent.h>

#include "ifs/fs.h"
#include "File.h"
#include "Stat.h"
#include "ObjectArray.h"

namespace fibjs
{

result_t fs_base::open(const char* fname, const char* mode,
		obj_ptr<File_base>& retVal)
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
		obj_ptr<File_base>& retVal)
{
	return 0;
}

result_t fs_base::tmpFile(obj_ptr<File_base>& retVal)
{
	retVal = new (File);
	return 0;
}

result_t fs_base::readFile(const char* fname, std::string& retVal)
{
	obj_ptr<File_base> f;
	obj_ptr<Buffer_base> buf;
	result_t hr;

	hr = open(fname, "r+", f);
	if(hr < 0)
		return hr;

	hr = f->read(-1, buf);
	if(hr < 0)
		return hr;

	return buf->toString(retVal);
}

result_t fs_base::writeFile(const char* fname, const char* txt)
{
	FILE* file = fopen(fname, "wb+");
	if (file == NULL)
		return LastError();

	int size = (int) strlen(txt);

	fwrite(txt, 1, size, file);
	fclose(file);

	return 0;
}

result_t fs_base::exists(const char* path, bool& retVal)
{
	retVal = access(path, F_OK) == 0;
	return 0;
}

result_t fs_base::unlink(const char* path)
{
	if(::unlink(path))
		return LastError();

	return 0;
}

result_t fs_base::mkdir(const char* path)
{
	if (::mkdir(path, 715))
		return LastError();

	return 0;
}

result_t fs_base::rmdir(const char* path)
{
	if (::rmdir(path))
		return LastError();

	return 0;
}

result_t fs_base::rename(const char* from, const char* to)
{
	if (::rename(from, to))
		return LastError();

	return 0;
}

result_t fs_base::stat(const char* path, obj_ptr<Stat_base>& retVal)
{
	struct stat st;
	if (::stat(path, &st))
		return LastError();

	obj_ptr<Stat> pStat = new Stat(path, st);

	retVal = pStat;

	return 0;
}

result_t fs_base::readdir(const char* path, obj_ptr<ObjectArray_base>& retVal)
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

		hr = stat(fpath.c_str(), fstat);
		if (hr >= 0)
			oa->push(fstat);
	}
	::closedir(dp);

	retVal = oa;

	return 0;
}

}
