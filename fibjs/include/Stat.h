/*
 * Stat.h
 *
 *  Created on: Apr 8, 2012
 *      Author: lion
 */

#ifndef STAT_H_
#define STAT_H_

#include "ifs/Stat.h"
#include <sys/stat.h>

#ifdef MacOS

#define ftello64 ftell
#define fseeko64 fseek
#define ftruncate64 ftruncate

#elif defined(_WIN32)

#define MINGW_HAS_SECURE_API
#include <io.h>

inline int ftruncate64(int fd, __int64 where)
{
    if (_lseeki64(fd, where, SEEK_SET) < 0)
        return -1;

    if (!SetEndOfFile((HANDLE)_get_osfhandle(fd)))
        return -1;

    return 0;
}

#define S_ISLNK(m) 0

#ifdef _MSC_VER
#define stat64 _stati64
#define ftello64 _ftelli64
#define fseeko64 _fseeki64
#define fstat64 _fstati64

#define S_IRUSR S_IREAD
#define S_IWUSR S_IWRITE
#define S_IXUSR S_IEXEC
#endif

#endif

#ifdef FreeBSD
#define ftello64 ftello
#define fseeko64 fseeko
#define stat64 stat
#define fstat64 fstat
#define ftruncate64 ftruncate
#endif

namespace fibjs
{

class Stat: public Stat_base
{
public:
    // Stat_base
    virtual result_t get_name(std::string &retVal);
    virtual result_t get_size(int64_t &retVal);
    virtual result_t get_mtime(date_t &retVal);
    virtual result_t get_atime(date_t &retVal);
    virtual result_t get_ctime(date_t &retVal);
    virtual result_t isWritable(bool &retVal);
    virtual result_t isReadable(bool &retVal);
    virtual result_t isExecutable(bool &retVal);
    virtual result_t isHidden(bool &retVal);
    virtual result_t isDirectory(bool &retVal);
    virtual result_t isFile(bool &retVal);
    virtual result_t isSymbolicLink(bool &retVal);
    virtual result_t isMemory(bool &retVal);
    virtual result_t isSocket(bool &retVal);

public:
    result_t getStat(const char *path);
    void fill(const char *path, struct stat64 &st);

#ifdef _WIN32
    void fill(WIN32_FIND_DATAW &fd);
#endif

    void init();

public:
    std::string name;
    int64_t size;
    date_t mtime, atime, ctime;
    bool m_isWritable, m_isReadable, m_isExecutable, m_isHidden;
    bool m_isDirectory, m_isFile, m_isSymbolicLink;
    bool m_isMemory, m_isSocket;
};

}

#endif /* STAT_H_ */
