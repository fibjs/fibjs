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

#include "ifs/fs.h"
#include "File.h"
#include <string.h>
#include "Stat.h"

#include <sys/stat.h>

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
	FILE* file = fopen(fname, "rb");
	if (file == NULL)
		return LastError();

	fseek(file, 0, SEEK_END);
	int size = ftell(file);
	rewind(file);

	std::string buf;

	buf.resize(size);

	for (int i = 0; i < size;)
	{
		int read = static_cast<int> (fread(&buf[i], 1, size - i, file));
		i += read;
	}
	fclose(file);

	retVal = buf;

	return 0;
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

result_t fs_base::mkdir(const char* path)
{
	if(::mkdir(path, 715))
		return LastError();

	return 0;
}

result_t fs_base::rmdir(const char* path)
{
	if(::rmdir(path))
		return LastError();

	return 0;
}

result_t fs_base::rename(const char* from, const char* to)
{
	if(::rename(from, to))
		return LastError();

	return 0;
}

result_t fs_base::stat(const char* path, obj_ptr<Stat_base>& retVal)
{
	struct stat st;
	if (::stat(path, &st))
		return LastError();

	obj_ptr<Stat> pStat = new Stat();

	pStat->size = st.st_size;
	pStat->mtime = st.st_mtime * 1000ll;
	pStat->atime = st.st_atime * 1000ll;
	pStat->ctime = st.st_ctime * 1000ll;

	retVal = pStat;

	return 0;
}
}
