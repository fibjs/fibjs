/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#pragma once

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class fs_constants_base : public object_base {
    DECLARE_CLASS(fs_constants_base);

public:
    enum {
        _SEEK_SET = 0,
        _SEEK_CUR = 1,
        _SEEK_END = 2,
        _UV_FS_SYMLINK_DIR = 1,
        _UV_FS_SYMLINK_JUNCTION = 2,
        _O_RDONLY = 0,
        _O_WRONLY = 1,
        _O_RDWR = 2,
        _UV_DIRENT_UNKNOWN = 0,
        _UV_DIRENT_FILE = 1,
        _UV_DIRENT_DIR = 2,
        _UV_DIRENT_LINK = 3,
        _UV_DIRENT_FIFO = 4,
        _UV_DIRENT_SOCKET = 5,
        _UV_DIRENT_CHAR = 6,
        _UV_DIRENT_BLOCK = 7,
        _S_IFMT = 61440,
        _S_IFREG = 32768,
        _S_IFDIR = 16384,
        _S_IFCHR = 8192,
        _S_IFBLK = 24576,
        _S_IFIFO = 4096,
        _S_IFLNK = 40960,
        _S_IFSOCK = 49152,
        _O_CREAT = 512,
        _O_EXCL = 2048,
        _UV_FS_O_FILEMAP = 0,
        _O_NOCTTY = 131072,
        _O_TRUNC = 1024,
        _O_APPEND = 8,
        _O_DIRECTORY = 1048576,
        _O_NOFOLLOW = 256,
        _O_SYNC = 128,
        _O_DSYNC = 4194304,
        _O_SYMLINK = 2097152,
        _O_NONBLOCK = 4,
        _S_IRWXU = 448,
        _S_IRUSR = 256,
        _S_IWUSR = 128,
        _S_IXUSR = 64,
        _S_IRWXG = 56,
        _S_IRGRP = 32,
        _S_IWGRP = 16,
        _S_IXGRP = 8,
        _S_IRWXO = 7,
        _S_IROTH = 4,
        _S_IWOTH = 2,
        _S_IXOTH = 1,
        _F_OK = 0,
        _R_OK = 4,
        _W_OK = 2,
        _X_OK = 1,
        _UV_FS_COPYFILE_EXCL = 1,
        _COPYFILE_EXCL = 1,
        _UV_FS_COPYFILE_FICLONE = 2,
        _COPYFILE_FICLONE = 2,
        _UV_FS_COPYFILE_FICLONE_FORCE = 4,
        _COPYFILE_FICLONE_FORCE = 4
    };

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }
};
}

namespace fibjs {
inline ClassInfo& fs_constants_base::class_info()
{
    static ClassData::ClassConst s_const[] = {
        { "SEEK_SET", _SEEK_SET },
        { "SEEK_CUR", _SEEK_CUR },
        { "SEEK_END", _SEEK_END },
        { "UV_FS_SYMLINK_DIR", _UV_FS_SYMLINK_DIR },
        { "UV_FS_SYMLINK_JUNCTION", _UV_FS_SYMLINK_JUNCTION },
        { "O_RDONLY", _O_RDONLY },
        { "O_WRONLY", _O_WRONLY },
        { "O_RDWR", _O_RDWR },
        { "UV_DIRENT_UNKNOWN", _UV_DIRENT_UNKNOWN },
        { "UV_DIRENT_FILE", _UV_DIRENT_FILE },
        { "UV_DIRENT_DIR", _UV_DIRENT_DIR },
        { "UV_DIRENT_LINK", _UV_DIRENT_LINK },
        { "UV_DIRENT_FIFO", _UV_DIRENT_FIFO },
        { "UV_DIRENT_SOCKET", _UV_DIRENT_SOCKET },
        { "UV_DIRENT_CHAR", _UV_DIRENT_CHAR },
        { "UV_DIRENT_BLOCK", _UV_DIRENT_BLOCK },
        { "S_IFMT", _S_IFMT },
        { "S_IFREG", _S_IFREG },
        { "S_IFDIR", _S_IFDIR },
        { "S_IFCHR", _S_IFCHR },
        { "S_IFBLK", _S_IFBLK },
        { "S_IFIFO", _S_IFIFO },
        { "S_IFLNK", _S_IFLNK },
        { "S_IFSOCK", _S_IFSOCK },
        { "O_CREAT", _O_CREAT },
        { "O_EXCL", _O_EXCL },
        { "UV_FS_O_FILEMAP", _UV_FS_O_FILEMAP },
        { "O_NOCTTY", _O_NOCTTY },
        { "O_TRUNC", _O_TRUNC },
        { "O_APPEND", _O_APPEND },
        { "O_DIRECTORY", _O_DIRECTORY },
        { "O_NOFOLLOW", _O_NOFOLLOW },
        { "O_SYNC", _O_SYNC },
        { "O_DSYNC", _O_DSYNC },
        { "O_SYMLINK", _O_SYMLINK },
        { "O_NONBLOCK", _O_NONBLOCK },
        { "S_IRWXU", _S_IRWXU },
        { "S_IRUSR", _S_IRUSR },
        { "S_IWUSR", _S_IWUSR },
        { "S_IXUSR", _S_IXUSR },
        { "S_IRWXG", _S_IRWXG },
        { "S_IRGRP", _S_IRGRP },
        { "S_IWGRP", _S_IWGRP },
        { "S_IXGRP", _S_IXGRP },
        { "S_IRWXO", _S_IRWXO },
        { "S_IROTH", _S_IROTH },
        { "S_IWOTH", _S_IWOTH },
        { "S_IXOTH", _S_IXOTH },
        { "F_OK", _F_OK },
        { "R_OK", _R_OK },
        { "W_OK", _W_OK },
        { "X_OK", _X_OK },
        { "UV_FS_COPYFILE_EXCL", _UV_FS_COPYFILE_EXCL },
        { "COPYFILE_EXCL", _COPYFILE_EXCL },
        { "UV_FS_COPYFILE_FICLONE", _UV_FS_COPYFILE_FICLONE },
        { "COPYFILE_FICLONE", _COPYFILE_FICLONE },
        { "UV_FS_COPYFILE_FICLONE_FORCE", _UV_FS_COPYFILE_FICLONE_FORCE },
        { "COPYFILE_FICLONE_FORCE", _COPYFILE_FICLONE_FORCE }
    };

    static ClassData s_cd = {
        "fs_constants", true, s__new, NULL,
        0, NULL, 0, NULL, 0, NULL, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}
}
