/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description 常用常量定义模块
 * 
 *  引用方法：
 *  ```JavaScript
 *  var constants = require('constants');
 *  ```
 *  
 */
declare module 'constants' {
    export const RTLD_LAZY: 1;

    export const RTLD_NOW: 2;

    export const RTLD_GLOBAL: 8;

    export const RTLD_LOCAL: 4;

    export const E2BIG: 7;

    export const EACCES: 13;

    export const EADDRINUSE: 48;

    export const EADDRNOTAVAIL: 49;

    export const EAFNOSUPPORT: 47;

    export const EAGAIN: 35;

    export const EALREADY: 37;

    export const EBADF: 9;

    export const EBADMSG: 94;

    export const EBUSY: 16;

    export const ECANCELED: 89;

    export const ECHILD: 10;

    export const ECONNABORTED: 53;

    export const ECONNREFUSED: 61;

    export const ECONNRESET: 54;

    export const EDEADLK: 11;

    export const EDESTADDRREQ: 39;

    export const EDOM: 33;

    export const EDQUOT: 69;

    export const EEXIST: 17;

    export const EFAULT: 14;

    export const EFBIG: 27;

    export const EHOSTUNREACH: 65;

    export const EIDRM: 90;

    export const EILSEQ: 92;

    export const EINPROGRESS: 36;

    export const EINTR: 4;

    export const EINVAL: 22;

    export const EIO: 5;

    export const EISCONN: 56;

    export const EISDIR: 21;

    export const ELOOP: 62;

    export const EMFILE: 24;

    export const EMLINK: 31;

    export const EMSGSIZE: 40;

    export const EMULTIHOP: 95;

    export const ENAMETOOLONG: 63;

    export const ENETDOWN: 50;

    export const ENETRESET: 52;

    export const ENETUNREACH: 51;

    export const ENFILE: 23;

    export const ENOBUFS: 55;

    export const ENODATA: 96;

    export const ENODEV: 19;

    export const ENOENT: 2;

    export const ENOEXEC: 8;

    export const ENOLCK: 77;

    export const ENOLINK: 97;

    export const ENOMEM: 12;

    export const ENOMSG: 91;

    export const ENOPROTOOPT: 42;

    export const ENOSPC: 28;

    export const ENOSR: 98;

    export const ENOSTR: 99;

    export const ENOSYS: 78;

    export const ENOTCONN: 57;

    export const ENOTDIR: 20;

    export const ENOTEMPTY: 66;

    export const ENOTSOCK: 38;

    export const ENOTSUP: 45;

    export const ENOTTY: 25;

    export const ENXIO: 6;

    export const EOPNOTSUPP: 102;

    export const EOVERFLOW: 84;

    export const EPERM: 1;

    export const EPIPE: 32;

    export const EPROTO: 100;

    export const EPROTONOSUPPORT: 43;

    export const EPROTOTYPE: 41;

    export const ERANGE: 34;

    export const EROFS: 30;

    export const ESPIPE: 29;

    export const ESRCH: 3;

    export const ESTALE: 70;

    export const ETIME: 101;

    export const ETIMEDOUT: 60;

    export const ETXTBSY: 26;

    export const EWOULDBLOCK: 35;

    export const EXDEV: 18;

    export const PRIORITY_LOW: 19;

    export const PRIORITY_BELOW_NORMAL: 10;

    export const PRIORITY_NORMAL: 0;

    export const PRIORITY_ABOVE_NORMAL: -7;

    export const PRIORITY_HIGH: -14;

    export const PRIORITY_HIGHEST: -20;

    export const SIGHUP: 1;

    export const SIGINT: 2;

    export const SIGQUIT: 3;

    export const SIGILL: 4;

    export const SIGTRAP: 5;

    export const SIGABRT: 6;

    export const SIGIOT: 6;

    export const SIGBUS: 10;

    export const SIGFPE: 8;

    export const SIGKILL: 9;

    export const SIGUSR1: 30;

    export const SIGSEGV: 11;

    export const SIGUSR2: 31;

    export const SIGPIPE: 13;

    export const SIGALRM: 14;

    export const SIGTERM: 15;

    export const SIGCHLD: 20;

    export const SIGCONT: 19;

    export const SIGSTOP: 17;

    export const SIGTSTP: 18;

    export const SIGTTIN: 21;

    export const SIGTTOU: 22;

    export const SIGURG: 16;

    export const SIGXCPU: 24;

    export const SIGXFSZ: 25;

    export const SIGVTALRM: 26;

    export const SIGPROF: 27;

    export const SIGWINCH: 28;

    export const SIGIO: 23;

    export const SIGINFO: 29;

    export const SIGSYS: 12;

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

