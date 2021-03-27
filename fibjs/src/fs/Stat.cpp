/*
 * Stat.cpp
 *
 *  Created on: Apr 8, 2012
 *      Author: lion
 */

#include "ifs/fs.h"
#include "object.h"
#include "Stat.h"
#include "File.h"
#include "path.h"
#include "utf8.h"
#include "winapi.h"
#include "AsyncUV.h"

namespace fibjs {

void Stat::fill(exlib::string path, const uv_stat_t* statbuf)
{
    path_base::basename(path, "", name);

    dev = statbuf->st_dev;
    ino = statbuf->st_ino;
    m_mode = statbuf->st_mode;

    nlink = statbuf->st_nlink;

    uid = statbuf->st_uid;
    gid = statbuf->st_gid;

    rdev = statbuf->st_rdev;

    size = statbuf->st_size;

    blksize = statbuf->st_blksize;
    blocks = statbuf->st_blocks;

    mtimeNs = statbuf->st_mtim.tv_nsec;
    mtime = (double)statbuf->st_mtim.tv_sec * 1000ll + (mtimeNs / 1000000000.0);

    atimeNs = statbuf->st_atim.tv_nsec;
    atime = (double)statbuf->st_atim.tv_sec * 1000ll + (atimeNs / 1000000000.0);

    ctimeNs = statbuf->st_ctim.tv_nsec;
    ctime = (double)statbuf->st_ctim.tv_sec * 1000ll + (ctimeNs / 1000000000.0);

    birthtimeNs = statbuf->st_birthtim.tv_nsec;
    birthtime = (double)statbuf->st_birthtim.tv_sec * 1000ll + (birthtimeNs / 1000000000.0);

    m_isMemory = false;
    m_isSocket = false;
}

void Stat::init()
{
    name.resize(0);
    size = 0;
    m_mode = 0;

    mtime = 0;
    mtimeNs = 0;
    atime = 0;
    mtimeNs = 0;
    ctime = 0;
    ctimeNs = 0;
    mtime = 0;
    birthtimeNs = 0;

    gid = 0;
    uid = 0;
}

void Stat::init(Stat_base* st)
{
    st->get_name(name);

    double d_tmp;
    int32_t i32_tmp;

    st->get_size(d_tmp);
    size = (int64_t)d_tmp;

    st->get_mode(i32_tmp);
    m_mode = i32_tmp;

    st->get_mtime(mtime);
    st->get_atime(atime);
    st->get_ctime(ctime);
    st->get_mtime(mtime);

    st->get_gid(i32_tmp);
    gid = i32_tmp;

    st->get_uid(i32_tmp);
    uid = i32_tmp;

    st->isMemory(m_isMemory);
    st->isSocket(m_isSocket);
}

result_t Stat::get_name(exlib::string& retVal)
{
    retVal = name;
    return 0;
};

result_t Stat::get_dev(int32_t& retVal)
{
    retVal = (int32_t)dev;
    return 0;
};

result_t Stat::get_ino(int32_t& retVal)
{
    retVal = (int32_t)ino;
    return 0;
};

result_t Stat::get_mode(int32_t& retVal)
{
    retVal = (int32_t)m_mode;
    return 0;
};

result_t Stat::get_nlink(int32_t& retVal)
{
    retVal = (int32_t)nlink;
    return 0;
};

result_t Stat::get_uid(int32_t& retVal)
{
    retVal = (int32_t)uid;
    return 0;
};

result_t Stat::get_gid(int32_t& retVal)
{
    retVal = (int32_t)gid;
    return 0;
};

result_t Stat::get_rdev(int32_t& retVal)
{
    retVal = (int32_t)rdev;
    return 0;
};

result_t Stat::get_size(double& retVal)
{
    retVal = (int32_t)size;
    return 0;
};

result_t Stat::get_blksize(int32_t& retVal)
{
    retVal = (int32_t)blksize;
    return 0;
};

result_t Stat::get_blocks(int32_t& retVal)
{
    retVal = (int32_t)blocks;
    return 0;
};

result_t Stat::get_mtime(date_t& retVal)
{
    retVal = mtime;
    return 0;
};

result_t Stat::get_mtimeMs(double& retVal)
{
    retVal = mtime.date();
    return 0;
};

result_t Stat::get_atime(date_t& retVal)
{
    retVal = atime;
    return 0;
};

result_t Stat::get_atimeMs(double& retVal)
{
    retVal = atime.date();
    return 0;
};

result_t Stat::get_ctime(date_t& retVal)
{
    retVal = ctime;
    return 0;
};

result_t Stat::get_ctimeMs(double& retVal)
{
    retVal = ctime.date();
    return 0;
};

result_t Stat::get_birthtime(date_t& retVal)
{
    retVal = birthtime;
    return 0;
};

result_t Stat::get_birthtimeMs(double& retVal)
{
    retVal = birthtime.date();
    return 0;
};

result_t Stat::isWritable(bool& retVal)
{
    retVal = (S_IWUSR & m_mode) != 0;

    return 0;
}

result_t Stat::isReadable(bool& retVal)
{
    retVal = (S_IRUSR & m_mode) != 0;

    return 0;
}

result_t Stat::isExecutable(bool& retVal)
{
    retVal = (S_IXUSR & m_mode) != 0;

    return 0;
}

result_t Stat::isHidden(bool& retVal)
{
    retVal = m_isHidden;
    return 0;
}

result_t Stat::isBlockDevice(bool& retVal)
{
#ifdef _WIN32
    retVal = false;
#else
    retVal = S_ISBLK(m_mode);
#endif

    return 0;
}

result_t Stat::isCharacterDevice(bool& retVal)
{
#ifdef _WIN32
    retVal = false;
#else
    retVal = S_ISCHR(m_mode);
#endif

    return 0;
}

result_t Stat::isDirectory(bool& retVal)
{
    retVal = (S_IFDIR & m_mode) != 0;

    return 0;
}

result_t Stat::isFIFO(bool& retVal)
{
#ifdef _WIN32
    retVal = false;
#else
    retVal = S_ISFIFO(m_mode);
#endif

    return 0;
}

result_t Stat::isFile(bool& retVal)
{
    retVal = (S_IFREG & m_mode) != 0;

    return 0;
}

result_t Stat::isSymbolicLink(bool& retVal)
{
    retVal = S_ISLNK(m_mode);

    return 0;
}

result_t Stat::isMemory(bool& retVal)
{
    retVal = m_isMemory;

    return 0;
}

result_t Stat::isSocket(bool& retVal)
{
    retVal = m_isSocket;

    return 0;
}
}
