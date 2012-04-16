/*
 * Stat.h
 *
 *  Created on: Apr 8, 2012
 *      Author: lion
 */

#include <osconfig.h>

#ifdef MacOS

#define ftello64 ftell
#define fseeko64 fseek
#define ftruncate64 ftruncate

#elif defined(_WIN32)

#define MINGW_HAS_SECURE_API
#include <io.h>

#define ftruncate64 _chsize_s

#define S_ISLNK(m) 0

#ifdef _MSC_VER
#define _stat64 _stati64
#define ftello64 _ftelli64
#define fseeko64 _fseeki64
#define fstat64 _fstati64

#define S_IRUSR S_IREAD
#define S_IWUSR S_IWRITE
#define S_IXUSR S_IEXEC
#endif

#endif


#ifndef STAT_H_
#define STAT_H_

#include "ifs/Stat.h"
#include <sys/stat.h>

namespace fibjs
{

class Stat: public Stat_base
{
public:
	result_t getStat(const char* path);
	void fillStat(const char* path, struct stat64& st);

#ifdef _WIN32
	void fillStat(WIN32_FIND_DATAW& fd);
#endif

public:
	// Stat_base
	virtual result_t get_name(std::string& retVal);
	virtual result_t get_size(double& retVal);
	virtual result_t get_mtime(int64_t& retVal);
	virtual result_t get_atime(int64_t& retVal);
	virtual result_t get_ctime(int64_t& retVal);
	virtual result_t isWritable(bool& retVal);
	virtual result_t isReadable(bool& retVal);
	virtual result_t isExecutable(bool& retVal);
	virtual result_t isHidden(bool& retVal);
	virtual result_t isDirectory(bool& retVal);
	virtual result_t isFile(bool& retVal);
	virtual result_t isSymbolicLink(bool& retVal);

public:
	std::string name;
	double size;
	int64_t mtime, atime, ctime;
	bool m_isWritable, m_isReadable, m_isExecutable, m_isHidden;
	bool m_isDirectory, m_isFile, m_isSymbolicLink;
};

}

#endif /* STAT_H_ */
