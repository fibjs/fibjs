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
        C_SEEK_SET = 0,
        C_SEEK_CUR = 1,
        C_SEEK_END = 2,
        C_UV_FS_SYMLINK_DIR = 1,
        C_UV_FS_SYMLINK_JUNCTION = 2,
        C_O_RDONLY = 0,
        C_O_WRONLY = 1,
        C_O_RDWR = 2,
        C_UV_DIRENT_UNKNOWN = 0,
        C_UV_DIRENT_FILE = 1,
        C_UV_DIRENT_DIR = 2,
        C_UV_DIRENT_LINK = 3,
        C_UV_DIRENT_FIFO = 4,
        C_UV_DIRENT_SOCKET = 5,
        C_UV_DIRENT_CHAR = 6,
        C_UV_DIRENT_BLOCK = 7,
        C_S_IFMT = 61440,
        C_S_IFREG = 32768,
        C_S_IFDIR = 16384,
        C_S_IFCHR = 8192,
        C_S_IFBLK = 24576,
        C_S_IFIFO = 4096,
        C_S_IFLNK = 40960,
        C_S_IFSOCK = 49152,
        C_O_CREAT = 512,
        C_O_EXCL = 2048,
        C_UV_FS_O_FILEMAP = 0,
        C_O_NOCTTY = 131072,
        C_O_TRUNC = 1024,
        C_O_APPEND = 8,
        C_O_DIRECTORY = 1048576,
        C_O_NOFOLLOW = 256,
        C_O_SYNC = 128,
        C_O_DSYNC = 4194304,
        C_O_SYMLINK = 2097152,
        C_O_NONBLOCK = 4,
        C_S_IRWXU = 448,
        C_S_IRUSR = 256,
        C_S_IWUSR = 128,
        C_S_IXUSR = 64,
        C_S_IRWXG = 56,
        C_S_IRGRP = 32,
        C_S_IWGRP = 16,
        C_S_IXGRP = 8,
        C_S_IRWXO = 7,
        C_S_IROTH = 4,
        C_S_IWOTH = 2,
        C_S_IXOTH = 1,
        C_F_OK = 0,
        C_R_OK = 4,
        C_W_OK = 2,
        C_X_OK = 1,
        C_UV_FS_COPYFILE_EXCL = 1,
        C_COPYFILE_EXCL = 1,
        C_UV_FS_COPYFILE_FICLONE = 2,
        C_COPYFILE_FICLONE = 2,
        C_UV_FS_COPYFILE_FICLONE_FORCE = 4,
        C_COPYFILE_FICLONE_FORCE = 4
    };

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }
};
}

namespace fibjs {
inline ClassInfo& fs_constants_base::class_info()
{
    static ClassData::ClassConst s_const[] = {
        { "SEEK_SET", C_SEEK_SET },
        { "SEEK_CUR", C_SEEK_CUR },
        { "SEEK_END", C_SEEK_END },
        { "UV_FS_SYMLINK_DIR", C_UV_FS_SYMLINK_DIR },
        { "UV_FS_SYMLINK_JUNCTION", C_UV_FS_SYMLINK_JUNCTION },
        { "O_RDONLY", C_O_RDONLY },
        { "O_WRONLY", C_O_WRONLY },
        { "O_RDWR", C_O_RDWR },
        { "UV_DIRENT_UNKNOWN", C_UV_DIRENT_UNKNOWN },
        { "UV_DIRENT_FILE", C_UV_DIRENT_FILE },
        { "UV_DIRENT_DIR", C_UV_DIRENT_DIR },
        { "UV_DIRENT_LINK", C_UV_DIRENT_LINK },
        { "UV_DIRENT_FIFO", C_UV_DIRENT_FIFO },
        { "UV_DIRENT_SOCKET", C_UV_DIRENT_SOCKET },
        { "UV_DIRENT_CHAR", C_UV_DIRENT_CHAR },
        { "UV_DIRENT_BLOCK", C_UV_DIRENT_BLOCK },
        { "S_IFMT", C_S_IFMT },
        { "S_IFREG", C_S_IFREG },
        { "S_IFDIR", C_S_IFDIR },
        { "S_IFCHR", C_S_IFCHR },
        { "S_IFBLK", C_S_IFBLK },
        { "S_IFIFO", C_S_IFIFO },
        { "S_IFLNK", C_S_IFLNK },
        { "S_IFSOCK", C_S_IFSOCK },
        { "O_CREAT", C_O_CREAT },
        { "O_EXCL", C_O_EXCL },
        { "UV_FS_O_FILEMAP", C_UV_FS_O_FILEMAP },
        { "O_NOCTTY", C_O_NOCTTY },
        { "O_TRUNC", C_O_TRUNC },
        { "O_APPEND", C_O_APPEND },
        { "O_DIRECTORY", C_O_DIRECTORY },
        { "O_NOFOLLOW", C_O_NOFOLLOW },
        { "O_SYNC", C_O_SYNC },
        { "O_DSYNC", C_O_DSYNC },
        { "O_SYMLINK", C_O_SYMLINK },
        { "O_NONBLOCK", C_O_NONBLOCK },
        { "S_IRWXU", C_S_IRWXU },
        { "S_IRUSR", C_S_IRUSR },
        { "S_IWUSR", C_S_IWUSR },
        { "S_IXUSR", C_S_IXUSR },
        { "S_IRWXG", C_S_IRWXG },
        { "S_IRGRP", C_S_IRGRP },
        { "S_IWGRP", C_S_IWGRP },
        { "S_IXGRP", C_S_IXGRP },
        { "S_IRWXO", C_S_IRWXO },
        { "S_IROTH", C_S_IROTH },
        { "S_IWOTH", C_S_IWOTH },
        { "S_IXOTH", C_S_IXOTH },
        { "F_OK", C_F_OK },
        { "R_OK", C_R_OK },
        { "W_OK", C_W_OK },
        { "X_OK", C_X_OK },
        { "UV_FS_COPYFILE_EXCL", C_UV_FS_COPYFILE_EXCL },
        { "COPYFILE_EXCL", C_COPYFILE_EXCL },
        { "UV_FS_COPYFILE_FICLONE", C_UV_FS_COPYFILE_FICLONE },
        { "COPYFILE_FICLONE", C_COPYFILE_FICLONE },
        { "UV_FS_COPYFILE_FICLONE_FORCE", C_UV_FS_COPYFILE_FICLONE_FORCE },
        { "COPYFILE_FICLONE_FORCE", C_COPYFILE_FICLONE_FORCE }
    };

    static ClassData s_cd = {
        "fs_constants", true, s__new, NULL,
        0, NULL, 0, NULL, 0, NULL, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}
}
