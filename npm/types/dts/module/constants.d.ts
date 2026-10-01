/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description Common constants definition module
 *
 *  Reference method:
 *  ```JavaScript
 *  var constants = require('constants');
 *  ```
 *
 */
declare module 'constants' {
    /**
     * @description Lazy loading of a dynamic library
     */
    export const RTLD_LAZY: 1;

    /**
     * @description Immediate loading of a dynamic library
     */
    export const RTLD_NOW: 2;

    /**
     * @description Loading a dynamic library in the global scope
     */
    export const RTLD_GLOBAL: 8;

    /**
     * @description Loading a dynamic library in the local scope
     */
    export const RTLD_LOCAL: 4;

    /**
     * @description Argument list too long error
     */
    export const E2BIG: 7;

    /**
     * @description Permission denied error
     */
    export const EACCES: 13;

    /**
     * @description Address already in use error
     */
    export const EADDRINUSE: 48;

    /**
     * @description Address not available error
     */
    export const EADDRNOTAVAIL: 49;

    /**
     * @description Address family not supported error
     */
    export const EAFNOSUPPORT: 47;

    /**
     * @description Resource temporarily unavailable error
     */
    export const EAGAIN: 35;

    /**
     * @description Connection already in progress error
     */
    export const EALREADY: 37;

    /**
     * @description Bad file descriptor error
     */
    export const EBADF: 9;

    /**
     * @description Bad message error
     */
    export const EBADMSG: 94;

    /**
     * @description Device or resource busy error
     */
    export const EBUSY: 16;

    /**
     * @description Operation canceled error
     */
    export const ECANCELED: 89;

    /**
     * @description No child processes error
     */
    export const ECHILD: 10;

    /**
     * @description Connection aborted error
     */
    export const ECONNABORTED: 53;

    /**
     * @description Connection refused error
     */
    export const ECONNREFUSED: 61;

    /**
     * @description Connection reset error
     */
    export const ECONNRESET: 54;

    /**
     * @description Resource deadlock avoided error
     */
    export const EDEADLK: 11;

    /**
     * @description Destination address required error
     */
    export const EDESTADDRREQ: 39;

    /**
     * @description Numerical argument out of domain error
     */
    export const EDOM: 33;

    /**
     * @description Disk quota exceeded error
     */
    export const EDQUOT: 69;

    /**
     * @description File exists error
     */
    export const EEXIST: 17;

    /**
     * @description Bad address error
     */
    export const EFAULT: 14;

    /**
     * @description File too large error
     */
    export const EFBIG: 27;

    /**
     * @description Host unreachable error
     */
    export const EHOSTUNREACH: 65;

    /**
     * @description Identifier removed error
     */
    export const EIDRM: 90;

    /**
     * @description Illegal byte sequence error
     */
    export const EILSEQ: 92;

    /**
     * @description Operation now in progress error
     */
    export const EINPROGRESS: 36;

    /**
     * @description Interrupted system call error
     */
    export const EINTR: 4;

    /**
     * @description Invalid argument error
     */
    export const EINVAL: 22;

    /**
     * @description Input/output error
     */
    export const EIO: 5;

    /**
     * @description Socket is connected error
     */
    export const EISCONN: 56;

    /**
     * @description Is a directory error
     */
    export const EISDIR: 21;

    /**
     * @description Too many levels of symbolic links error
     */
    export const ELOOP: 62;

    /**
     * @description Too many open files error
     */
    export const EMFILE: 24;

    /**
     * @description Too many links error
     */
    export const EMLINK: 31;

    /**
     * @description Message too long error
     */
    export const EMSGSIZE: 40;

    /**
     * @description Multihop attempted error
     */
    export const EMULTIHOP: 95;

    /**
     * @description File name too long error
     */
    export const ENAMETOOLONG: 63;

    /**
     * @description Network is down error
     */
    export const ENETDOWN: 50;

    /**
     * @description Network connection reset error
     */
    export const ENETRESET: 52;

    /**
     * @description Network is unreachable error
     */
    export const ENETUNREACH: 51;

    /**
     * @description Too many open files in system error
     */
    export const ENFILE: 23;

    /**
     * @description No buffer space available error
     */
    export const ENOBUFS: 55;

    /**
     * @description No data available error
     */
    export const ENODATA: 96;

    /**
     * @description No such device error
     */
    export const ENODEV: 19;

    /**
     * @description No such file or directory error
     */
    export const ENOENT: 2;

    /**
     * @description Exec format error
     */
    export const ENOEXEC: 8;

    /**
     * @description No locks available error
     */
    export const ENOLCK: 77;

    /**
     * @description Link has been severed error
     */
    export const ENOLINK: 97;

    /**
     * @description Out of memory error
     */
    export const ENOMEM: 12;

    /**
     * @description No message of desired type error
     */
    export const ENOMSG: 91;

    /**
     * @description Protocol not available error
     */
    export const ENOPROTOOPT: 42;

    /**
     * @description No space left on device error
     */
    export const ENOSPC: 28;

    /**
     * @description Out of streams resources error
     */
    export const ENOSR: 98;

    /**
     * @description Device not a stream error
     */
    export const ENOSTR: 99;

    /**
     * @description Function not implemented error
     */
    export const ENOSYS: 78;

    /**
     * @description Socket is not connected error
     */
    export const ENOTCONN: 57;

    /**
     * @description Not a directory error
     */
    export const ENOTDIR: 20;

    /**
     * @description Directory not empty error
     */
    export const ENOTEMPTY: 66;

    /**
     * @description Socket operation on non-socket error
     */
    export const ENOTSOCK: 38;

    /**
     * @description Operation not supported error
     */
    export const ENOTSUP: 45;

    /**
     * @description Inappropriate I/O control operation error
     */
    export const ENOTTY: 25;

    /**
     * @description No such device or address error
     */
    export const ENXIO: 6;

    /**
     * @description Operation not supported on socket error
     */
    export const EOPNOTSUPP: 102;

    /**
     * @description Value too large for the data type error
     */
    export const EOVERFLOW: 84;

    /**
     * @description Operation not permitted error
     */
    export const EPERM: 1;

    /**
     * @description Broken pipe error
     */
    export const EPIPE: 32;

    /**
     * @description Protocol error
     */
    export const EPROTO: 100;

    /**
     * @description Protocol not supported error
     */
    export const EPROTONOSUPPORT: 43;

    /**
     * @description Protocol wrong type error
     */
    export const EPROTOTYPE: 41;

    /**
     * @description Result too large error
     */
    export const ERANGE: 34;

    /**
     * @description Read-only file system error
     */
    export const EROFS: 30;

    /**
     * @description Invalid seek error
     */
    export const ESPIPE: 29;

    /**
     * @description No such process error
     */
    export const ESRCH: 3;

    /**
     * @description Stale file handle error
     */
    export const ESTALE: 70;

    /**
     * @description Timer expired error
     */
    export const ETIME: 101;

    /**
     * @description Connection timed out error
     */
    export const ETIMEDOUT: 60;

    /**
     * @description Text file busy error
     */
    export const ETXTBSY: 26;

    /**
     * @description Operation would block error
     */
    export const EWOULDBLOCK: 35;

    /**
     * @description Cross-device link error
     */
    export const EXDEV: 18;

    /**
     * @description Low priority
     */
    export const PRIORITY_LOW: 19;

    /**
     * @description Below normal priority
     */
    export const PRIORITY_BELOW_NORMAL: 10;

    /**
     * @description Normal priority
     */
    export const PRIORITY_NORMAL: 0;

    /**
     * @description Above normal priority
     */
    export const PRIORITY_ABOVE_NORMAL: -7;

    /**
     * @description High priority
     */
    export const PRIORITY_HIGH: -14;

    /**
     * @description Highest priority
     */
    export const PRIORITY_HIGHEST: -20;

    /**
     * @description Hangup signal
     */
    export const SIGHUP: 1;

    /**
     * @description Interrupt signal
     */
    export const SIGINT: 2;

    /**
     * @description Quit signal
     */
    export const SIGQUIT: 3;

    /**
     * @description Illegal instruction signal
     */
    export const SIGILL: 4;

    /**
     * @description Trace trap signal
     */
    export const SIGTRAP: 5;

    /**
     * @description Abort signal
     */
    export const SIGABRT: 6;

    /**
     * @description IOT trap signal
     */
    export const SIGIOT: 6;

    /**
     * @description Bus error signal
     */
    export const SIGBUS: 10;

    /**
     * @description Floating point exception signal
     */
    export const SIGFPE: 8;

    /**
     * @description Kill signal
     */
    export const SIGKILL: 9;

    /**
     * @description User defined signal 1
     */
    export const SIGUSR1: 30;

    /**
     * @description Segmentation fault signal
     */
    export const SIGSEGV: 11;

    /**
     * @description User defined signal 2
     */
    export const SIGUSR2: 31;

    /**
     * @description Broken pipe signal
     */
    export const SIGPIPE: 13;

    /**
     * @description Alarm clock signal
     */
    export const SIGALRM: 14;

    /**
     * @description Termination signal
     */
    export const SIGTERM: 15;

    /**
     * @description Child process terminated or stopped signal
     */
    export const SIGCHLD: 20;

    /**
     * @description Continue execution signal
     */
    export const SIGCONT: 19;

    /**
     * @description Stop execution signal
     */
    export const SIGSTOP: 17;

    /**
     * @description Terminal stop signal
     */
    export const SIGTSTP: 18;

    /**
     * @description Background process attempts to read signal
     */
    export const SIGTTIN: 21;

    /**
     * @description Background process attempts to write signal
     */
    export const SIGTTOU: 22;

    /**
     * @description Socket urgent condition signal
     */
    export const SIGURG: 16;

    /**
     * @description CPU time limit exceeded signal
     */
    export const SIGXCPU: 24;

    /**
     * @description File size limit exceeded signal
     */
    export const SIGXFSZ: 25;

    /**
     * @description Virtual timer expired signal
     */
    export const SIGVTALRM: 26;

    /**
     * @description Profiling timer expired signal
     */
    export const SIGPROF: 27;

    /**
     * @description Window size change signal
     */
    export const SIGWINCH: 28;

    /**
     * @description I/O now possible signal
     */
    export const SIGIO: 23;

    /**
     * @description Information request signal
     */
    export const SIGINFO: 29;

    /**
     * @description Bad system call signal
     */
    export const SIGSYS: 12;

    /**
     * @description Symbolic link to a directory
     */
    export const UV_FS_SYMLINK_DIR: 1;

    /**
     * @description Symbolic link to a junction point
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
     * @description Directory directory entry type
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
     * @description Bit mask of the file type bit field
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
     * @description Create the file if it does not exist
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
     * @description Do not allocate a controlling terminal
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
     * @description Allow opening symbolic links
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
     * @description Exclusive copy file flag
     */
    export const UV_FS_COPYFILE_EXCL: 1;

    /**
     * @description Exclusive copy file flag
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
     * @description File clone force copy flag
     */
    export const UV_FS_COPYFILE_FICLONE_FORCE: 4;

    /**
     * @description File clone force copy flag
     */
    export const COPYFILE_FICLONE_FORCE: 4;

}

