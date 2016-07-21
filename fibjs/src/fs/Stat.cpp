/*
 * Stat.cpp
 *
 *  Created on: Apr 8, 2012
 *      Author: lion
 */

#include "object.h"
#include "Stat.h"
#include "ifs/path.h"
#include "utf8.h"

namespace fibjs
{

#ifdef _WIN32

inline double FileTimeToJSTime(FILETIME &ft)
{
    return (double)(*(int64_t *)&ft - 116444736000000000) / 10000;
}

void Stat::fill(WIN32_FIND_DATAW &fd)
{
    name = utf16to8String(fd.cFileName);

    size = ((int64_t)fd.nFileSizeHigh << 32 | fd.nFileSizeLow);
    mode = 0666;

    mtime = FileTimeToJSTime(fd.ftLastWriteTime);
    atime = FileTimeToJSTime(fd.ftLastAccessTime);
    ctime = FileTimeToJSTime(fd.ftCreationTime);

    m_isDirectory = (FILE_ATTRIBUTE_DIRECTORY & fd.dwFileAttributes) != 0;
    m_isFile = (FILE_ATTRIBUTE_DIRECTORY & fd.dwFileAttributes) == 0;

    m_isReadable = true;
    m_isWritable = (FILE_ATTRIBUTE_READONLY & fd.dwFileAttributes) == 0;
    m_isExecutable = true;

    m_isSymbolicLink = false;

    m_isMemory = false;
    m_isSocket = false;
}

#endif

result_t Stat::getStat(exlib::string path)
{
    struct stat64 st;
    if (::stat64(path.c_str(), &st))
        return CHECK_ERROR(LastError());

    fill(path, st);

    return 0;
}

void Stat::fill(exlib::string path, struct stat64 &st)
{
    path_base::basename(path, "", name);

    size = st.st_size;
    mode = st.st_mode;
    mtime = (double)st.st_mtime * 1000ll;
    atime = (double)st.st_atime * 1000ll;
    ctime = (double)st.st_ctime * 1000ll;

    m_isReadable = (st.st_mode | S_IRUSR) != 0;
    m_isWritable = (st.st_mode | S_IWUSR) != 0;
    m_isExecutable = (st.st_mode | S_IXUSR) != 0;

    m_isDirectory = (S_IFDIR & st.st_mode) != 0;
    m_isFile = (S_IFREG & st.st_mode) != 0;

    m_isSymbolicLink = S_ISLNK(st.st_mode);

    m_isMemory = false;
    m_isSocket = false;
}

void Stat::init()
{
    name.resize(0);
    size = 0;
    mode = 0;

    mtime = 0;
    atime = 0;
    ctime = 0;

    m_isReadable = false;
    m_isWritable = false;
    m_isExecutable = false;

    m_isDirectory = false;
    m_isFile = false;

    m_isSymbolicLink = false;

    m_isMemory = false;
    m_isSocket = false;
}

result_t Stat::get_name(exlib::string &retVal)
{
    retVal = name;
    return 0;
}

result_t Stat::get_size(int64_t &retVal)
{
    retVal = size;
    return 0;
}

result_t Stat::get_mode(int32_t &retVal)
{
    retVal = mode;
    return 0;
}

result_t Stat::get_mtime(date_t &retVal)
{
    retVal = mtime;
    return 0;
}

result_t Stat::get_atime(date_t &retVal)
{
    retVal = atime;
    return 0;
}

result_t Stat::get_ctime(date_t &retVal)
{
    retVal = ctime;
    return 0;
}

result_t Stat::isWritable(bool &retVal)
{
    retVal = m_isWritable;
    return 0;
}

result_t Stat::isReadable(bool &retVal)
{
    retVal = m_isReadable;
    return 0;
}

result_t Stat::isExecutable(bool &retVal)
{
    retVal = m_isExecutable;
    return 0;
}

result_t Stat::isHidden(bool &retVal)
{
    retVal = m_isHidden;
    return 0;
}

result_t Stat::isDirectory(bool &retVal)
{
    retVal = m_isDirectory;
    return 0;
}

result_t Stat::isFile(bool &retVal)
{
    retVal = m_isFile;
    return 0;
}

result_t Stat::isSymbolicLink(bool &retVal)
{
    retVal = m_isSymbolicLink;
    return 0;
}

result_t Stat::isMemory(bool &retVal)
{
    retVal = m_isMemory;
    return 0;
}

result_t Stat::isSocket(bool &retVal)
{
    retVal = m_isSocket;
    return 0;
}

}
