/*
 * Stat.cpp
 *
 *  Created on: Apr 8, 2012
 *      Author: lion
 */

#include "Stat.h"
#include "ifs/path.h"

namespace fibjs
{

Stat::Stat(const char* path, struct stat& st)
{
	path_base::basename(path, "", name);
	size = st.st_size;
	mtime = st.st_mtime * 1000ll;
	atime = st.st_atime * 1000ll;
	ctime = st.st_ctime * 1000ll;

	m_isReadable = (st.st_mode | S_IRUSR) != 0;
	m_isWritable = (st.st_mode | S_IWUSR) != 0;
	m_isExecutable = (st.st_mode | S_IXUSR) != 0;

	m_isDirectory = S_ISDIR(st.st_mode);
	m_isFile = S_ISREG(st.st_mode);
	m_isSymbolicLink = S_ISLNK(st.st_mode);
}

result_t Stat::get_name(std::string& retVal)
{
	retVal = name;
	return 0;
}

result_t Stat::get_size(double& retVal)
{
	retVal = size;
	return 0;
}

result_t Stat::get_mtime(int64_t& retVal)
{
	retVal = mtime;
	return 0;
}

result_t Stat::get_atime(int64_t& retVal)
{
	retVal = atime;
	return 0;
}

result_t Stat::get_ctime(int64_t& retVal)
{
	retVal = ctime;
	return 0;
}

result_t Stat::isWritable(bool& retVal)
{
	retVal = m_isWritable;
	return 0;
}

result_t Stat::isReadable(bool& retVal)
{
	retVal = m_isReadable;
	return 0;
}

result_t Stat::isExecutable(bool& retVal)
{
	retVal = m_isExecutable;
	return 0;
}

result_t Stat::isHidden(bool& retVal)
{
	retVal = m_isHidden;
	return 0;
}

result_t Stat::isDirectory(bool& retVal)
{
	retVal = m_isDirectory;
	return 0;
}

result_t Stat::isFile(bool& retVal)
{
	retVal = m_isFile;
	return 0;
}

result_t Stat::isSymbolicLink(bool& retVal)
{
	retVal = m_isSymbolicLink;
	return 0;
}

}
