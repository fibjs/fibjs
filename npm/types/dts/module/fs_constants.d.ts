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

    /**
     * @description 符号链接到目录 
     */
    export const UV_FS_SYMLINK_DIR: 1;

    /**
     * @description 符号链接到连接点 
     */
    export const UV_FS_SYMLINK_JUNCTION: 2;

    /**
     * @description 仅打开读取 
     */
    export const O_RDONLY: 0;

    /**
     * @description 仅打开写入 
     */
    export const O_WRONLY: 1;

    /**
     * @description 打开读取和写入 
     */
    export const O_RDWR: 2;

    /**
     * @description 未知目录项类型 
     */
    export const UV_DIRENT_UNKNOWN: 0;

    /**
     * @description 文件目录项类型 
     */
    export const UV_DIRENT_FILE: 1;

    /**
     * @description 目录目录项类型 
     */
    export const UV_DIRENT_DIR: 2;

    /**
     * @description 符号链接目录项类型 
     */
    export const UV_DIRENT_LINK: 3;

    /**
     * @description FIFO目录项类型 
     */
    export const UV_DIRENT_FIFO: 4;

    /**
     * @description 套接字目录项类型 
     */
    export const UV_DIRENT_SOCKET: 5;

    /**
     * @description 字符设备目录项类型 
     */
    export const UV_DIRENT_CHAR: 6;

    /**
     * @description 块设备目录项类型 
     */
    export const UV_DIRENT_BLOCK: 7;

    /**
     * @description 文件类型位字段的位掩码 
     */
    export const S_IFMT: 61440;

    /**
     * @description 常规文件 
     */
    export const S_IFREG: 32768;

    /**
     * @description 目录 
     */
    export const S_IFDIR: 16384;

    /**
     * @description 字符设备 
     */
    export const S_IFCHR: 8192;

    /**
     * @description 块设备 
     */
    export const S_IFBLK: 24576;

    /**
     * @description FIFO 
     */
    export const S_IFIFO: 4096;

    /**
     * @description 符号链接 
     */
    export const S_IFLNK: 40960;

    /**
     * @description 套接字 
     */
    export const S_IFSOCK: 49152;

    /**
     * @description 如果文件不存在则创建文件 
     */
    export const O_CREAT: 512;

    /**
     * @description 确保文件的独占创建 
     */
    export const O_EXCL: 2048;

    /**
     * @description 文件映射标志 
     */
    export const UV_FS_O_FILEMAP: 0;

    /**
     * @description 不分配控制终端 
     */
    export const O_NOCTTY: 131072;

    /**
     * @description 将文件截断为零长度 
     */
    export const O_TRUNC: 1024;

    /**
     * @description 追加到文件末尾 
     */
    export const O_APPEND: 8;

    /**
     * @description 打开目录 
     */
    export const O_DIRECTORY: 1048576;

    /**
     * @description 不跟随符号链接 
     */
    export const O_NOFOLLOW: 256;

    /**
     * @description 同步I/O 
     */
    export const O_SYNC: 128;

    /**
     * @description 同步I/O数据完整性完成 
     */
    export const O_DSYNC: 4194304;

    /**
     * @description 允许打开符号链接 
     */
    export const O_SYMLINK: 2097152;

    /**
     * @description 非阻塞模式 
     */
    export const O_NONBLOCK: 4;

    /**
     * @description 所有者读写执行权限 
     */
    export const S_IRWXU: 448;

    /**
     * @description 所有者读权限 
     */
    export const S_IRUSR: 256;

    /**
     * @description 所有者写权限 
     */
    export const S_IWUSR: 128;

    /**
     * @description 所有者执行权限 
     */
    export const S_IXUSR: 64;

    /**
     * @description 组读写执行权限 
     */
    export const S_IRWXG: 56;

    /**
     * @description 组读权限 
     */
    export const S_IRGRP: 32;

    /**
     * @description 组写权限 
     */
    export const S_IWGRP: 16;

    /**
     * @description 组执行权限 
     */
    export const S_IXGRP: 8;

    /**
     * @description 其他人读写执行权限 
     */
    export const S_IRWXO: 7;

    /**
     * @description 其他人读权限 
     */
    export const S_IROTH: 4;

    /**
     * @description 其他人写权限 
     */
    export const S_IWOTH: 2;

    /**
     * @description 其他人执行权限 
     */
    export const S_IXOTH: 1;

    /**
     * @description 测试文件是否存在 
     */
    export const F_OK: 0;

    /**
     * @description 测试读权限 
     */
    export const R_OK: 4;

    /**
     * @description 测试写权限 
     */
    export const W_OK: 2;

    /**
     * @description 测试执行权限 
     */
    export const X_OK: 1;

    /**
     * @description 独占复制文件标志 
     */
    export const UV_FS_COPYFILE_EXCL: 1;

    /**
     * @description 独占复制文件标志 
     */
    export const COPYFILE_EXCL: 1;

    /**
     * @description 文件克隆复制标志 
     */
    export const UV_FS_COPYFILE_FICLONE: 2;

    /**
     * @description 文件克隆复制标志 
     */
    export const COPYFILE_FICLONE: 2;

    /**
     * @description 文件克隆强制复制标志 
     */
    export const UV_FS_COPYFILE_FICLONE_FORCE: 4;

    /**
     * @description 文件克隆强制复制标志 
     */
    export const COPYFILE_FICLONE_FORCE: 4;

}

