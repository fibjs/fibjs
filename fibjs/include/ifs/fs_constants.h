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
        __SEEK_SET = 0,
        __SEEK_CUR = 1,
        __SEEK_END = 2,
        __UV_FS_SYMLINK_DIR = 1,
        __UV_FS_SYMLINK_JUNCTION = 2,
        __O_RDONLY = 0,
        __O_WRONLY = 1,
        __O_RDWR = 2,
        __UV_DIRENT_UNKNOWN = 0,
        __UV_DIRENT_FILE = 1,
        __UV_DIRENT_DIR = 2,
        __UV_DIRENT_LINK = 3,
        __UV_DIRENT_FIFO = 4,
        __UV_DIRENT_SOCKET = 5,
        __UV_DIRENT_CHAR = 6,
        __UV_DIRENT_BLOCK = 7,
        __S_IFMT = 61440,
        __S_IFREG = 32768,
        __S_IFDIR = 16384,
        __S_IFCHR = 8192,
        __S_IFBLK = 24576,
        __S_IFIFO = 4096,
        __S_IFLNK = 40960,
        __S_IFSOCK = 49152,
        __O_CREAT = 512,
        __O_EXCL = 2048,
        __UV_FS_O_FILEMAP = 0,
        __O_NOCTTY = 131072,
        __O_TRUNC = 1024,
        __O_APPEND = 8,
        __O_DIRECTORY = 1048576,
        __O_NOFOLLOW = 256,
        __O_SYNC = 128,
        __O_DSYNC = 4194304,
        __O_SYMLINK = 2097152,
        __O_NONBLOCK = 4,
        __S_IRWXU = 448,
        __S_IRUSR = 256,
        __S_IWUSR = 128,
        __S_IXUSR = 64,
        __S_IRWXG = 56,
        __S_IRGRP = 32,
        __S_IWGRP = 16,
        __S_IXGRP = 8,
        __S_IRWXO = 7,
        __S_IROTH = 4,
        __S_IWOTH = 2,
        __S_IXOTH = 1,
        __F_OK = 0,
        __R_OK = 4,
        __W_OK = 2,
        __X_OK = 1,
        __UV_FS_COPYFILE_EXCL = 1,
        __COPYFILE_EXCL = 1,
        __UV_FS_COPYFILE_FICLONE = 2,
        __COPYFILE_FICLONE = 2,
        __UV_FS_COPYFILE_FICLONE_FORCE = 4,
        __COPYFILE_FICLONE_FORCE = 4
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
        { "SEEK_SET", __SEEK_SET },
        { "SEEK_CUR", __SEEK_CUR },
        { "SEEK_END", __SEEK_END },
        { "UV_FS_SYMLINK_DIR", __UV_FS_SYMLINK_DIR },
        { "UV_FS_SYMLINK_JUNCTION", __UV_FS_SYMLINK_JUNCTION },
        { "O_RDONLY", __O_RDONLY },
        { "O_WRONLY", __O_WRONLY },
        { "O_RDWR", __O_RDWR },
        { "UV_DIRENT_UNKNOWN", __UV_DIRENT_UNKNOWN },
        { "UV_DIRENT_FILE", __UV_DIRENT_FILE },
        { "UV_DIRENT_DIR", __UV_DIRENT_DIR },
        { "UV_DIRENT_LINK", __UV_DIRENT_LINK },
        { "UV_DIRENT_FIFO", __UV_DIRENT_FIFO },
        { "UV_DIRENT_SOCKET", __UV_DIRENT_SOCKET },
        { "UV_DIRENT_CHAR", __UV_DIRENT_CHAR },
        { "UV_DIRENT_BLOCK", __UV_DIRENT_BLOCK },
        { "S_IFMT", __S_IFMT },
        { "S_IFREG", __S_IFREG },
        { "S_IFDIR", __S_IFDIR },
        { "S_IFCHR", __S_IFCHR },
        { "S_IFBLK", __S_IFBLK },
        { "S_IFIFO", __S_IFIFO },
        { "S_IFLNK", __S_IFLNK },
        { "S_IFSOCK", __S_IFSOCK },
        { "O_CREAT", __O_CREAT },
        { "O_EXCL", __O_EXCL },
        { "UV_FS_O_FILEMAP", __UV_FS_O_FILEMAP },
        { "O_NOCTTY", __O_NOCTTY },
        { "O_TRUNC", __O_TRUNC },
        { "O_APPEND", __O_APPEND },
        { "O_DIRECTORY", __O_DIRECTORY },
        { "O_NOFOLLOW", __O_NOFOLLOW },
        { "O_SYNC", __O_SYNC },
        { "O_DSYNC", __O_DSYNC },
        { "O_SYMLINK", __O_SYMLINK },
        { "O_NONBLOCK", __O_NONBLOCK },
        { "S_IRWXU", __S_IRWXU },
        { "S_IRUSR", __S_IRUSR },
        { "S_IWUSR", __S_IWUSR },
        { "S_IXUSR", __S_IXUSR },
        { "S_IRWXG", __S_IRWXG },
        { "S_IRGRP", __S_IRGRP },
        { "S_IWGRP", __S_IWGRP },
        { "S_IXGRP", __S_IXGRP },
        { "S_IRWXO", __S_IRWXO },
        { "S_IROTH", __S_IROTH },
        { "S_IWOTH", __S_IWOTH },
        { "S_IXOTH", __S_IXOTH },
        { "F_OK", __F_OK },
        { "R_OK", __R_OK },
        { "W_OK", __W_OK },
        { "X_OK", __X_OK },
        { "UV_FS_COPYFILE_EXCL", __UV_FS_COPYFILE_EXCL },
        { "COPYFILE_EXCL", __COPYFILE_EXCL },
        { "UV_FS_COPYFILE_FICLONE", __UV_FS_COPYFILE_FICLONE },
        { "COPYFILE_FICLONE", __COPYFILE_FICLONE },
        { "UV_FS_COPYFILE_FICLONE_FORCE", __UV_FS_COPYFILE_FICLONE_FORCE },
        { "COPYFILE_FICLONE_FORCE", __COPYFILE_FICLONE_FORCE }
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
