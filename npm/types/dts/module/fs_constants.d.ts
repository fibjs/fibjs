/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description Commonly used constants of the fs module
 *
 *  Usage:
 *  ```JavaScript
 *  var constants = require('fs').constants
 *  ```
 *
 */
declare module 'fs_constants' {
    /**
     * @description Seek method constant, moves to an absolute position
     */
    export const SEEK_SET: 0;

    /**
     * @description Seek method constant, moves relative to the current position
     */
    export const SEEK_CUR: 1;

    /**
     * @description Seek method constant, moves relative to the end of the file
     */
    export const SEEK_END: 2;

    /**
     * @description Symbolic link to a directory
     */
    export const UV_FS_SYMLINK_DIR: 1;

    /**
     * @description Symbolic link to a junction
     */
    export const UV_FS_SYMLINK_JUNCTION: 2;

    /**
     * @description Open for reading only
     */
    export const O_RDONLY: 0;

    /**
     * @description Open for writing only
     */
    export const O_WRONLY: 1;

    /**
     * @description Open for reading and writing
     */
    export const O_RDWR: 2;

    /**
     * @description Unknown directory entry type
     */
    export const UV_DIRENT_UNKNOWN: 0;

    /**
     * @description File directory entry type
     */
    export const UV_DIRENT_FILE: 1;

    /**
     * @description Directory entry type
     */
    export const UV_DIRENT_DIR: 2;

    /**
     * @description Symbolic link directory entry type
     */
    export const UV_DIRENT_LINK: 3;

    /**
     * @description FIFO directory entry type
     */
    export const UV_DIRENT_FIFO: 4;

    /**
     * @description Socket directory entry type
     */
    export const UV_DIRENT_SOCKET: 5;

    /**
     * @description Character device directory entry type
     */
    export const UV_DIRENT_CHAR: 6;

    /**
     * @description Block device directory entry type
     */
    export const UV_DIRENT_BLOCK: 7;

    /**
     * @description Bit mask for the file type bit field
     */
    export const S_IFMT: 61440;

    /**
     * @description Regular file
     */
    export const S_IFREG: 32768;

    /**
     * @description Directory
     */
    export const S_IFDIR: 16384;

    /**
     * @description Character device
     */
    export const S_IFCHR: 8192;

    /**
     * @description Block device
     */
    export const S_IFBLK: 24576;

    /**
     * @description FIFO
     */
    export const S_IFIFO: 4096;

    /**
     * @description Symbolic link
     */
    export const S_IFLNK: 40960;

    /**
     * @description Socket
     */
    export const S_IFSOCK: 49152;

    /**
     * @description Create the file when it does not exist
     */
    export const O_CREAT: 512;

    /**
     * @description Ensure exclusive creation of the file
     */
    export const O_EXCL: 2048;

    /**
     * @description File mapping flag
     */
    export const UV_FS_O_FILEMAP: 0;

    /**
     * @description Do not assign a controlling terminal
     */
    export const O_NOCTTY: 131072;

    /**
     * @description Truncate the file to zero length
     */
    export const O_TRUNC: 1024;

    /**
     * @description Append to the end of the file
     */
    export const O_APPEND: 8;

    /**
     * @description Open a directory
     */
    export const O_DIRECTORY: 1048576;

    /**
     * @description Do not follow symbolic links
     */
    export const O_NOFOLLOW: 256;

    /**
     * @description Synchronous I/O
     */
    export const O_SYNC: 128;

    /**
     * @description Synchronous I/O data integrity completion
     */
    export const O_DSYNC: 4194304;

    /**
     * @description Allow opening a symbolic link
     */
    export const O_SYMLINK: 2097152;

    /**
     * @description Non-blocking mode
     */
    export const O_NONBLOCK: 4;

    /**
     * @description Owner read, write and execute permission
     */
    export const S_IRWXU: 448;

    /**
     * @description Owner read permission
     */
    export const S_IRUSR: 256;

    /**
     * @description Owner write permission
     */
    export const S_IWUSR: 128;

    /**
     * @description Owner execute permission
     */
    export const S_IXUSR: 64;

    /**
     * @description Group read, write and execute permission
     */
    export const S_IRWXG: 56;

    /**
     * @description Group read permission
     */
    export const S_IRGRP: 32;

    /**
     * @description Group write permission
     */
    export const S_IWGRP: 16;

    /**
     * @description Group execute permission
     */
    export const S_IXGRP: 8;

    /**
     * @description Others read, write and execute permission
     */
    export const S_IRWXO: 7;

    /**
     * @description Others read permission
     */
    export const S_IROTH: 4;

    /**
     * @description Others write permission
     */
    export const S_IWOTH: 2;

    /**
     * @description Others execute permission
     */
    export const S_IXOTH: 1;

    /**
     * @description Test whether the file exists
     */
    export const F_OK: 0;

    /**
     * @description Test read permission
     */
    export const R_OK: 4;

    /**
     * @description Test write permission
     */
    export const W_OK: 2;

    /**
     * @description Test execute permission
     */
    export const X_OK: 1;

    /**
     * @description Exclusive file copy flag
     */
    export const UV_FS_COPYFILE_EXCL: 1;

    /**
     * @description Exclusive file copy flag
     */
    export const COPYFILE_EXCL: 1;

    /**
     * @description File clone copy flag
     */
    export const UV_FS_COPYFILE_FICLONE: 2;

    /**
     * @description File clone copy flag
     */
    export const COPYFILE_FICLONE: 2;

    /**
     * @description Forced file clone copy flag
     */
    export const UV_FS_COPYFILE_FICLONE_FORCE: 4;

    /**
     * @description Forced file clone copy flag
     */
    export const COPYFILE_FICLONE_FORCE: 4;

    /**
     * @description Treat extensionless modules as javascript, consistent with Node.js
     */
    export const EXTENSIONLESS_FORMAT_JAVASCRIPT: 0;

    /**
     * @description Treat extensionless modules as wasm, consistent with Node.js
     */
    export const EXTENSIONLESS_FORMAT_WASM: 1;

}

