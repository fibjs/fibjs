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

namespace fibjs {

#ifdef _WIN32

inline double FileTimeToJSTime(FILETIME& ft)
{
    return (double)(*(int64_t*)&ft - 116444736000000000) / 10000;
}
#define MILLIONu (1000U * 1000U)
#define BILLIONu (1000U * 1000U * 1000U)

#define FILETIME_TO_UINT(filetime) \
    (*((uint64_t*)&(filetime)) - (uint64_t)116444736 * BILLIONu)

#define FILETIME_TO_TIME_T(filetime) \
    (FILETIME_TO_UINT(filetime) / (10u * MILLIONu))

#define FILETIME_TO_TIME_NS(filetime, secs) \
    ((FILETIME_TO_UINT(filetime) - (secs * (uint64_t)10 * MILLIONu)) * 100U)

#define FILETIME_TO_TIMESPEC(ts, filetime)                               \
    do {                                                                 \
        (ts).tv_sec = (long)FILETIME_TO_TIME_T(filetime);                \
        (ts).tv_nsec = (long)FILETIME_TO_TIME_NS(filetime, (ts).tv_sec); \
    } while (0)

#define TIME_T_TO_FILETIME(time, filetime_ptr)                                                              \
    do {                                                                                                    \
        uint64_t bigtime = ((uint64_t)((time) * (uint64_t)10 * MILLIONu)) + (uint64_t)116444736 * BILLIONu; \
        (filetime_ptr)->dwLowDateTime = bigtime & 0xFFFFFFFF;                                               \
        (filetime_ptr)->dwHighDateTime = bigtime >> 32;                                                     \
    } while (0)

bool tryFillStatByNtApi(HANDLE handle, obj_ptr<Stat> statobj)
{
    HMODULE hNtdll;
    sNtQueryInformationFile pNtQueryInformationFile;

    hNtdll = LoadLibrary(TEXT("ntdll.dll"));
    if (!hNtdll)
        return false;

    pNtQueryInformationFile = (sNtQueryInformationFile)GetProcAddress(hNtdll, "NtQueryInformationFile");
    if (!pNtQueryInformationFile)
        return false;

    FILE_ALL_INFORMATION file_info;
    FILE_FS_VOLUME_INFORMATION volume_info;
    NTSTATUS nt_status = -1;
    IO_STATUS_BLOCK io_status;

    nt_status = pNtQueryInformationFile(handle,
        &io_status,
        &file_info,
        sizeof file_info,
        FileAllInformation);

    if (io_status.Status == STATUS_NOT_IMPLEMENTED) {
        statobj->dev = 0;
    } else if (NT_ERROR(nt_status)) {
        sRtlNtStatusToDosError pRtlNtStatusToDosError;
        pRtlNtStatusToDosError = (sRtlNtStatusToDosError)GetProcAddress(hNtdll, "RtlNtStatusToDosError");
        if (!pRtlNtStatusToDosError)
            return false;
        SetLastError(pRtlNtStatusToDosError(nt_status));
        return false;
    } else {
        statobj->dev = volume_info.VolumeSerialNumber;
    }

    timespec ts_tmp;

    FILETIME_TO_TIMESPEC(ts_tmp, file_info.BasicInformation.LastWriteTime);
    statobj->mtimeNs = (double)ts_tmp.tv_nsec;
    statobj->mtime = (double)ts_tmp.tv_sec * 1000ll + (statobj->mtimeNs / 1000000000.0);

    FILETIME_TO_TIMESPEC(ts_tmp, file_info.BasicInformation.LastAccessTime);
    statobj->atimeNs = (double)ts_tmp.tv_nsec;
    statobj->atime = (double)ts_tmp.tv_sec * 1000ll + (statobj->atimeNs / 1000000000.0);

    FILETIME_TO_TIMESPEC(ts_tmp, file_info.BasicInformation.ChangeTime);
    statobj->ctimeNs = (double)ts_tmp.tv_nsec;
    statobj->ctime = (double)ts_tmp.tv_sec * 1000ll + (statobj->ctimeNs / 1000000000.0);

    FILETIME_TO_TIMESPEC(ts_tmp, file_info.BasicInformation.CreationTime);
    statobj->birthtimeNs = (double)ts_tmp.tv_nsec;
    statobj->birthtime = (double)ts_tmp.tv_sec * 1000ll + (statobj->birthtimeNs / 1000000000.0);

    statobj->ino = file_info.InternalInformation.IndexNumber.QuadPart;

    statobj->blocks = (uint64_t)file_info.StandardInformation.AllocationSize.QuadPart >> 9;

    statobj->nlink = file_info.StandardInformation.NumberOfLinks;

    statobj->blksize = 4096;
    statobj->gid = 0;
    statobj->uid = 0;
    statobj->rdev = 0;

    return true;
}

void Stat::fill(exlib::string path, WIN32_FIND_DATAW& fd)
{
    exlib::string strPath;
    name = utf16to8String(fd.cFileName);

    size = ((int64_t)fd.nFileSizeHigh << 32 | fd.nFileSizeLow);
    mode = S_IREAD | S_IEXEC;

    mtime = FileTimeToJSTime(fd.ftLastWriteTime);
    atime = FileTimeToJSTime(fd.ftLastAccessTime);
    ctime = FileTimeToJSTime(fd.ftCreationTime);

    uid = 0;
    gid = 0;

    if ((m_isDirectory = (FILE_ATTRIBUTE_DIRECTORY & fd.dwFileAttributes) != 0) == true)
        mode |= S_IFDIR;
    if ((m_isFile = (FILE_ATTRIBUTE_DIRECTORY & fd.dwFileAttributes) == 0) == true)
        mode |= S_IFREG;

    m_isReadable = true;
    if ((m_isWritable = (FILE_ATTRIBUTE_READONLY & fd.dwFileAttributes) == 0) == true)
        mode |= S_IWRITE;
    m_isExecutable = true;

    m_isSymbolicLink = false;
    if (fs_base::cc_readlink(path, strPath) == 0)
        m_isSymbolicLink = true;

    m_isMemory = false;
    m_isSocket = false;
}

void Stat::fill(exlib::string safe_name, BY_HANDLE_FILE_INFORMATION& fd)
{
    path_base::basename(safe_name, "", name);

    size = ((int64_t)fd.nFileSizeHigh << 32 | fd.nFileSizeLow);
    ino = ((int64_t)fd.nFileIndexHigh << 32 | fd.nFileIndexLow);
    mode = S_IREAD | S_IEXEC;

    mtime = FileTimeToJSTime(fd.ftLastWriteTime);
    atime = FileTimeToJSTime(fd.ftLastAccessTime);
    ctime = FileTimeToJSTime(fd.ftCreationTime);
    nlink = fd.nNumberOfLinks;
    dev = fd.dwVolumeSerialNumber;

    uid = 0;
    gid = 0;

    if ((m_isDirectory = (FILE_ATTRIBUTE_DIRECTORY & fd.dwFileAttributes) != 0) == true)
        mode |= S_IFDIR;
    if ((m_isFile = (FILE_ATTRIBUTE_DIRECTORY & fd.dwFileAttributes) == 0) == true)
        mode |= S_IFREG;

    m_isReadable = true;
    if ((m_isWritable = (FILE_ATTRIBUTE_READONLY & fd.dwFileAttributes) == 0) == true)
        mode |= S_IWRITE;
    m_isExecutable = true;

    m_isSymbolicLink = false;
    exlib::string strLinkPath;
    if (fs_base::cc_readlink(safe_name, strLinkPath) == 0)
        m_isSymbolicLink = true;

    m_isMemory = false;
    m_isSocket = false;
}

result_t Stat::getStat(exlib::string path)
{
    result_t hr = path_base::normalize(path, path);
    if (hr < 0)
        return hr;

    if (path.length() > 0 && isWin32PathSlash(path.c_str()[path.length() - 1]))
        path.resize(path.length() - 1);

    WIN32_FIND_DATAW fd;
    HANDLE hFind;

    hFind = FindFirstFileW(UTF8_W(path), &fd);
    if (hFind == INVALID_HANDLE_VALUE)
        return CHECK_ERROR(LastError());

    fill(path, fd);
    FindClose(hFind);

    if (!m_isFile)
        return 0;

    int32_t f_fd;
    hr = file_open(path, "r", mode, f_fd);
    if (hr < 0)
        return hr;

    BY_HANDLE_FILE_INFORMATION fdata;
    HANDLE fHandle = (HANDLE)_get_osfhandle(f_fd);
    if (!GetFileInformationByHandle(fHandle, &fdata))
        return CHECK_ERROR(LastError());

    fill(path, fdata);
    tryFillStatByNtApi(fHandle, this);
    ::_close(f_fd);

    return 0;
}

result_t Stat::getLstat(exlib::string path)
{
    return getStat(path);
}

#else

result_t Stat::getStat(exlib::string path)
{
    struct stat64 st;
    if (::stat64(path.c_str(), &st))
        return CHECK_ERROR(LastError());

    fill(path, st);

    return 0;
}

result_t Stat::getLstat(exlib::string path)
{
    struct stat64 st;
    if (::lstat64(path.c_str(), &st))
        return CHECK_ERROR(LastError());

    fill(path, st);
    return 0;
}

#endif

void Stat::fill(exlib::string path, struct stat64& st)
{
    path_base::basename(path, "", name);

    dev = st.st_dev;
    ino = st.st_ino;
    mode = st.st_mode;

    nlink = st.st_nlink;

    uid = st.st_uid;
    gid = st.st_gid;

    rdev = st.st_rdev;

    size = st.st_size;
#ifndef _WIN32
    blksize = st.st_blksize;
    blocks = st.st_blocks;
#ifdef Darwin
    mtimeNs = (double)st.st_mtimespec.tv_nsec;
    mtime = (double)st.st_mtimespec.tv_sec * 1000ll + (mtimeNs / 1000000000.0);

    atimeNs = (double)st.st_atimespec.tv_nsec;
    atime = (double)st.st_atimespec.tv_sec * 1000ll + (atimeNs / 1000000000.0);

    ctimeNs = (double)st.st_ctimespec.tv_nsec;
    ctime = (double)st.st_ctimespec.tv_sec * 1000ll + (ctimeNs / 1000000000.0);

    birthtimeNs = (double)st.st_birthtimespec.tv_nsec;
    birthtime = (double)st.st_birthtimespec.tv_sec * 1000ll + (birthtimeNs / 1000000000.0);
#else /* Darwin */
    mtime = (double)st.st_mtime * 1000ll;
    mtimeNs = (uint64_t)(st.st_mtime * 1000000) % 1000000 * 1000;
    atime = (double)st.st_atime * 1000ll;
    atimeNs = (uint64_t)(st.st_atime * 1000000) % 1000000 * 1000;
    ctime = (double)st.st_ctime * 1000ll;
    ctimeNs = (uint64_t)(st.st_ctime * 1000000) % 1000000 * 1000;
    birthtime = ctime.date();
    birthtimeNs = ctimeNs;
#endif /* Darwin */
    m_isBlockDevice = S_ISBLK(st.st_mode);
    m_isCharacterDevice = S_ISCHR(st.st_mode);
    m_isFIFO = S_ISFIFO(st.st_mode);
#endif

    m_isReadable = (S_IRUSR & st.st_mode) != 0;
    m_isWritable = (S_IWUSR & st.st_mode) != 0;
    m_isExecutable = (S_IXUSR & st.st_mode) != 0;

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
