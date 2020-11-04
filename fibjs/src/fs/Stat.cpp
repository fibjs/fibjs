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

result_t Stat::getStat(exlib::string path)
{
    uv_fs_t stat_req;

    int ret = uv_call([&] {
        return uv_fs_stat(s_uv_loop, &stat_req, path.c_str(), NULL);
    });

    if (ret != 0)
        return CHECK_ERROR(Runtime::setError(uv_strerror(ret)));

    fill(path, &stat_req.statbuf);

    return 0;
}

result_t Stat::getLstat(exlib::string path)
{
    uv_fs_t stat_req;

    int ret = uv_call([&] {
        return uv_fs_lstat(s_uv_loop, &stat_req, path.c_str(), NULL);
    });

    if (ret != 0)
        return CHECK_ERROR(Runtime::setError(uv_strerror(ret)));

    fill(path, &stat_req.statbuf);

    return 0;
}

void Stat::fill(exlib::string path, uv_stat_t* statbuf)
{
    path_base::basename(path, "", name);

    dev = statbuf->st_dev;
    ino = statbuf->st_ino;
    mode = statbuf->st_mode;

    nlink = statbuf->st_nlink;

    uid = statbuf->st_uid;
    gid = statbuf->st_gid;

    rdev = statbuf->st_rdev;

    size = statbuf->st_size;

    blksize = statbuf->st_blksize;
    blocks = statbuf->st_blocks;

    mtimeNs = (double)statbuf->st_mtim.tv_nsec;
    mtime = (double)statbuf->st_mtim.tv_sec * 1000ll + (mtimeNs / 1000000000.0);

    atimeNs = (double)statbuf->st_atim.tv_nsec;
    atime = (double)statbuf->st_atim.tv_sec * 1000ll + (atimeNs / 1000000000.0);

    ctimeNs = (double)statbuf->st_ctim.tv_nsec;
    ctime = (double)statbuf->st_ctim.tv_sec * 1000ll + (ctimeNs / 1000000000.0);

    birthtimeNs = (double)statbuf->st_birthtim.tv_nsec;
    birthtime = (double)statbuf->st_birthtim.tv_sec * 1000ll + (birthtimeNs / 1000000000.0);

#ifdef _WIN32
    m_isBlockDevice = false;

    m_isCharacterDevice = false;
    m_isFIFO = false;
#else
    m_isBlockDevice = S_ISBLK(statbuf->st_mode);
    m_isCharacterDevice = S_ISCHR(statbuf->st_mode);
    m_isFIFO = S_ISFIFO(statbuf->st_mode);
#endif

    m_isReadable = (S_IRUSR & statbuf->st_mode) != 0;
    m_isWritable = (S_IWUSR & statbuf->st_mode) != 0;
    m_isExecutable = (S_IXUSR & statbuf->st_mode) != 0;

    m_isDirectory = (S_IFDIR & statbuf->st_mode) != 0;
    m_isFile = (S_IFREG & statbuf->st_mode) != 0;

    m_isSymbolicLink = S_ISLNK(statbuf->st_mode);

    m_isMemory = false;
    m_isSocket = false;
}

void Stat::init()
{
    name.resize(0);
    size = 0;
    mode = 0;

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

    m_isBlockDevice = false;
    m_isCharacterDevice = false;
    m_isFIFO = false;

    m_isReadable = false;
    m_isWritable = false;
    m_isExecutable = false;

    m_isDirectory = false;
    m_isFile = false;

    m_isSymbolicLink = false;

    m_isMemory = false;
    m_isSocket = false;
}

void Stat::init(Stat_base* st)
{
    st->get_name(name);

    double d_tmp;
    int32_t i32_tmp;

    st->get_size(d_tmp);
    size = d_tmp;

    st->get_mode(i32_tmp);
    mode = i32_tmp;

    st->get_mtime(mtime);
    st->get_atime(atime);
    st->get_ctime(ctime);
    st->get_mtime(mtime);

    st->get_gid(i32_tmp);
    gid = i32_tmp;

    st->get_uid(i32_tmp);
    uid = i32_tmp;

    // st->isBlockDevice(m_isBlockDevice);
    // st->isCharacterDevice(m_isCharacterDevice);
    // st->isFIFO(m_isFIFO);

    st->isReadable(m_isReadable);
    st->isWritable(m_isWritable);
    st->isExecutable(m_isExecutable);

    st->isDirectory(m_isDirectory);
    st->isFile(m_isFile);

    st->isSymbolicLink(m_isSymbolicLink);

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
    retVal = dev;
    return 0;
};

result_t Stat::get_ino(int32_t& retVal)
{
    retVal = ino;
    return 0;
};

result_t Stat::get_mode(int32_t& retVal)
{
    retVal = mode;
    return 0;
};

result_t Stat::get_nlink(int32_t& retVal)
{
    retVal = nlink;
    return 0;
};

result_t Stat::get_uid(int32_t& retVal)
{
    retVal = uid;
    return 0;
};

result_t Stat::get_gid(int32_t& retVal)
{
    retVal = gid;
    return 0;
};

result_t Stat::get_rdev(int32_t& retVal)
{
    retVal = rdev;
    return 0;
};

result_t Stat::get_size(double& retVal)
{
    retVal = size;
    return 0;
};

result_t Stat::get_blksize(int32_t& retVal)
{
    retVal = blksize;
    return 0;
};

result_t Stat::get_blocks(int32_t& retVal)
{
    retVal = blocks;
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
