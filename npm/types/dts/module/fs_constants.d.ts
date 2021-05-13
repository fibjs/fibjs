/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description fs 模块常用常量定义模块
 * 
 *  引用方法：
 *  ```JavaScript
 *  var constants = require('fs').constants
 *  ```
 *  
 */
declare module 'fs_constants' {
    /**
     * @description seek 方式常量，移动到绝对位置 
     */
    export const SEEK_SET: 0;

    /**
     * @description seek 方式常量，移动到当前位置的相对位置 
     */
    export const SEEK_CUR: 1;

    /**
     * @description seek 方式常量，移动到文件结尾的相对位置 
     */
    export const SEEK_END: 2;

    export const UV_FS_SYMLINK_DIR: 1;

    export const UV_FS_SYMLINK_JUNCTION: 2;

    export const O_RDONLY: 0;

    export const O_WRONLY: 1;

    export const O_RDWR: 2;

    export const UV_DIRENT_UNKNOWN: 0;

    export const UV_DIRENT_FILE: 1;

    export const UV_DIRENT_DIR: 2;

    export const UV_DIRENT_LINK: 3;

    export const UV_DIRENT_FIFO: 4;

    export const UV_DIRENT_SOCKET: 5;

    export const UV_DIRENT_CHAR: 6;

    export const UV_DIRENT_BLOCK: 7;

    export const S_IFMT: 61440;

    export const S_IFREG: 32768;

    export const S_IFDIR: 16384;

    export const S_IFCHR: 8192;

    export const S_IFBLK: 24576;

    export const S_IFIFO: 4096;

    export const S_IFLNK: 40960;

    export const S_IFSOCK: 49152;

    export const O_CREAT: 512;

    export const O_EXCL: 2048;

    export const UV_FS_O_FILEMAP: 0;

    export const O_NOCTTY: 131072;

    export const O_TRUNC: 1024;

    export const O_APPEND: 8;

    export const O_DIRECTORY: 1048576;

    export const O_NOFOLLOW: 256;

    export const O_SYNC: 128;

    export const O_DSYNC: 4194304;

    export const O_SYMLINK: 2097152;

    export const O_NONBLOCK: 4;

    export const S_IRWXU: 448;

    export const S_IRUSR: 256;

    export const S_IWUSR: 128;

    export const S_IXUSR: 64;

    export const S_IRWXG: 56;

    export const S_IRGRP: 32;

    export const S_IWGRP: 16;

    export const S_IXGRP: 8;

    export const S_IRWXO: 7;

    export const S_IROTH: 4;

    export const S_IWOTH: 2;

    export const S_IXOTH: 1;

    export const F_OK: 0;

    export const R_OK: 4;

    export const W_OK: 2;

    export const X_OK: 1;

    export const UV_FS_COPYFILE_EXCL: 1;

    export const COPYFILE_EXCL: 1;

    export const UV_FS_COPYFILE_FICLONE: 2;

    export const COPYFILE_FICLONE: 2;

    export const UV_FS_COPYFILE_FICLONE_FORCE: 4;

    export const COPYFILE_FICLONE_FORCE: 4;

}

