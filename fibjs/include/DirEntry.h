/*
 * DirEntry.h
 *
 *  Created on: Nov 21, 2025
 *      Author: lion
 */

#pragma once

#include "ifs/DirEntry.h"
#include "AsyncUV.h"
#include <sys/stat.h>

#ifdef _WIN32
// Define missing S_IF* constants for Windows
#ifndef S_IFIFO
#define S_IFIFO 0x1000 // Named pipe
#endif

#ifndef S_IFSOCK
#define S_IFSOCK 0xC000 // Socket
#endif
#endif

// Helper function to convert uv_dirent_type_t to st_mode
inline uint32_t dirent_type_to_mode(uv_dirent_type_t type)
{
    switch (type) {
    case UV_DIRENT_FILE:
        return S_IFREG;
    case UV_DIRENT_DIR:
        return S_IFDIR;
    case UV_DIRENT_LINK:
        return S_IFLNK;
    case UV_DIRENT_FIFO:
        return S_IFIFO;
    case UV_DIRENT_SOCKET:
        return S_IFSOCK;
#ifndef _WIN32
    case UV_DIRENT_BLOCK:
        return S_IFBLK;
    case UV_DIRENT_CHAR:
        return S_IFCHR;
#endif
    default:
        return 0;
    }
}

namespace fibjs {

class DirEntry : public DirEntry_base {
public:
    virtual result_t get_name(exlib::string& retVal);
    virtual result_t get_parentPath(exlib::string& retVal);
    virtual result_t isBlockDevice(bool& retVal);
    virtual result_t isCharacterDevice(bool& retVal);
    virtual result_t isDirectory(bool& retVal);
    virtual result_t isFIFO(bool& retVal);
    virtual result_t isFile(bool& retVal);
    virtual result_t isSymbolicLink(bool& retVal);
    virtual result_t isSocket(bool& retVal);

public:
    void fill(exlib::string path, exlib::string parent_path, uint32_t mode);

public:
    exlib::string name;
    exlib::string parentPath;
    uint32_t m_mode;
};
}
