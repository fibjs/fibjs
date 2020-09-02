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

#ifdef _WIN32

#define MINGW_HAS_SECURE_API
#include <io.h>

inline int32_t ftruncate64(int32_t fd, __int64 where)
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
#define fstat64 _fstati64

#define S_IRUSR S_IREAD
#define S_IWUSR S_IWRITE
#define S_IXUSR S_IEXEC
#endif

#else

#define _fileno fileno
#define _lseeki64 lseek
#define _read read
#define _write write
#define _close close

#ifndef Linux
#define stat64 stat
#define fstat64 fstat
#define lstat64 lstat
#define ftruncate64 ftruncate
#endif

#endif

namespace fibjs {

class Stat : public Stat_base {
public:
    virtual result_t get_name(exlib::string& retVal);
    virtual result_t get_dev(int32_t& retVal);
    virtual result_t get_ino(int32_t& retVal);
    virtual result_t get_mode(int32_t& retVal);
    virtual result_t get_nlink(int32_t& retVal);
    virtual result_t get_uid(int32_t& retVal);
    virtual result_t get_gid(int32_t& retVal);
    virtual result_t get_rdev(int32_t& retVal);
    virtual result_t get_size(double& retVal);
    virtual result_t get_blksize(int32_t& retVal);
    virtual result_t get_blocks(int32_t& retVal);
    virtual result_t get_mtime(date_t& retVal);
    virtual result_t get_mtimeMs(double& retVal);
    virtual result_t get_atime(date_t& retVal);
    virtual result_t get_atimeMs(double& retVal);
    virtual result_t get_ctime(date_t& retVal);
    virtual result_t get_ctimeMs(double& retVal);
    virtual result_t get_birthtime(date_t& retVal);
    virtual result_t get_birthtimeMs(double& retVal);
    virtual result_t isWritable(bool& retVal);
    virtual result_t isReadable(bool& retVal);
    virtual result_t isExecutable(bool& retVal);
    virtual result_t isHidden(bool& retVal);
    virtual result_t isDirectory(bool& retVal);
    virtual result_t isFile(bool& retVal);
    virtual result_t isSymbolicLink(bool& retVal);
    virtual result_t isMemory(bool& retVal);
    virtual result_t isSocket(bool& retVal);

public:
    result_t getStat(exlib::string path);
    result_t getLstat(exlib::string path);
    void fill(exlib::string path, struct stat64& st);

#ifdef _WIN32
    void fill(exlib::string path, WIN32_FIND_DATAW& fd);
    void fill(exlib::string path, BY_HANDLE_FILE_INFORMATION& fd);
#endif

    void init();
    void init(Stat_base* st);

public:
    exlib::string name;
    int64_t dev, rdev;
    int64_t ino;
    int64_t mode;
    int64_t nlink;
    int64_t uid, gid;
    int64_t size;
    int64_t blksize;
    int64_t blocks;
    date_t mtime, atime, ctime, birthtime;
    int64_t mtimeNs, atimeNs, ctimeNs, birthtimeNs;
    bool m_isBlockDevice, m_isCharacterDevice, m_isFIFO;
    bool m_isWritable, m_isReadable, m_isExecutable, m_isHidden;
    bool m_isDirectory, m_isFile, m_isSymbolicLink;
    bool m_isMemory, m_isSocket;
};
}

#endif /* STAT_H_ */
