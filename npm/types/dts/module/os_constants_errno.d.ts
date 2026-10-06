/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description The errno table of os.constants: the POSIX error codes of the running platform
 *
 *  The object is reached through `require('os').constants.errno` and is not requireable on its
 *  own. The names follow the C library and libuv; the numbers are the values of the host (on
 *  Linux x86-64 they match `<asm-generic/errno-base.h>` and `<asm-generic/errno.h>` exactly).
 *  Convert a numeric error into its name with this table, and use it to document the errors an
 *  operation can raise; the table itself is read-only.
 *
 *  Concepts:
 *
 *  - **Errors carry the name**: a failed call throws an Error whose `code` is the symbolic name
 *    ('ENOENT'), so compare `e.code` rather than a number. On POSIX `e.errno` is the negated
 *    libuv code (-2 for ENOENT) while this table holds the positive system value (2): compare
 *    `-e.errno` with the table, or rely on `e.code`.
 *  - **Aliases**: EAGAIN and EWOULDBLOCK share one value, and so do ENOTSUP and EOPNOTSUPP
 *    (11 and 95 on Linux); POSIX allows the unsupported-operation pair to differ, so test both
 *    names when the code is not known.
 *  - **Platform**: the numbers are platform-specific; on Windows libuv uses its own mapping and
 *    adds the WSA* codes. Never hardcode a number in portable code - look it up here at
 *    runtime or compare names.
 *
 *  Import:
 *  ```JavaScript
 *  const errno = require('os').constants.errno;
 *  ```
 *
 *  Example 1 — turn a caught error into its table entry:
 *  ```JavaScript
 *  const errno = require('os').constants.errno;
 *  const fs = require('fs');
 *
 *  try {
 *      fs.unlink('/no-such-file-fibjs.txt');
 *  } catch (e) {
 *      console.log(e.code);                    // ENOENT
 *      console.log(-e.errno, errno.ENOENT);    // 2 2
 *      console.log(-e.errno === errno.ENOENT); // true
 *  }
 *  ```
 *
 *  Example 2 — common failures and their codes:
 *  ```JavaScript
 *  const errno = require('os').constants.errno;
 *  const fs = require('fs');
 *  const os = require('os');
 *  const path = require('path');
 *
 *  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-errno-'));
 *  fs.mkdir(path.join(dir, 'sub'));
 *  fs.writeFile(path.join(dir, 'sub', 'file.txt'), 'x');
 *
 *  function codeOf(fn) {
 *      try {
 *          fn();
 *      } catch (e) {
 *          return e.code;
 *      }
 *      return 'no error';
 *  }
 *  console.log(codeOf(() => fs.mkdir(path.join(dir, 'sub'))));      // EEXIST
 *  console.log(codeOf(() => fs.rmdir(path.join(dir, 'sub'))));      // ENOTEMPTY
 *  console.log(codeOf(() => fs.unlink(path.join(dir, 'missing')))); // ENOENT
 *  console.log(errno.EEXIST, errno.ENOTEMPTY);                      // 17 39
 *
 *  fs.rmSync(dir, { recursive: true, force: true });
 *  ```
 *
 *  Example 3 — representative values and aliases:
 *  ```JavaScript
 *  const errno = require('os').constants.errno;
 *
 *  // Permission, existence and timeout codes as reported by the host.
 *  console.log(errno.EACCES, errno.ENOENT, errno.ETIMEDOUT); // 13 2 110
 *
 *  // Each alias pair shares a single value.
 *  console.log(errno.EAGAIN === errno.EWOULDBLOCK);          // true
 *  console.log(errno.ENOTSUP === errno.EOPNOTSUPP);          // true
 *  console.log(Object.keys(errno).length);                   // 79 on Linux
 *  ```
 *
 */
declare module 'os_constants_errno' {
    /**
     * @description Argument list too long
     */
    export const E2BIG: 7;

    /**
     * @description Permission denied
     */
    export const EACCES: 13;

    /**
     * @description Address already in use
     */
    export const EADDRINUSE: 98;

    /**
     * @description Address not available
     */
    export const EADDRNOTAVAIL: 99;

    /**
     * @description Address family not supported
     */
    export const EAFNOSUPPORT: 97;

    /**
     * @description Resource temporarily unavailable, try again
     */
    export const EAGAIN: 11;

    /**
     * @description Operation already in progress
     */
    export const EALREADY: 114;

    /**
     * @description Bad file descriptor
     */
    export const EBADF: 9;

    /**
     * @description Bad message
     */
    export const EBADMSG: 74;

    /**
     * @description Device or resource busy
     */
    export const EBUSY: 16;

    /**
     * @description Operation canceled
     */
    export const ECANCELED: 125;

    /**
     * @description No child processes
     */
    export const ECHILD: 10;

    /**
     * @description Connection aborted
     */
    export const ECONNABORTED: 103;

    /**
     * @description Connection refused
     */
    export const ECONNREFUSED: 111;

    /**
     * @description Connection reset
     */
    export const ECONNRESET: 104;

    /**
     * @description Resource deadlock avoided
     */
    export const EDEADLK: 35;

    /**
     * @description Destination address required
     */
    export const EDESTADDRREQ: 89;

    /**
     * @description Mathematics argument out of domain of function
     */
    export const EDOM: 33;

    /**
     * @description Disk quota exceeded
     */
    export const EDQUOT: 122;

    /**
     * @description File exists
     */
    export const EEXIST: 17;

    /**
     * @description Bad address
     */
    export const EFAULT: 14;

    /**
     * @description File too large
     */
    export const EFBIG: 27;

    /**
     * @description Host is unreachable
     */
    export const EHOSTUNREACH: 113;

    /**
     * @description Identifier removed
     */
    export const EIDRM: 43;

    /**
     * @description Illegal byte sequence
     */
    export const EILSEQ: 84;

    /**
     * @description Operation in progress
     */
    export const EINPROGRESS: 115;

    /**
     * @description Interrupted system call
     */
    export const EINTR: 4;

    /**
     * @description Invalid argument
     */
    export const EINVAL: 22;

    /**
     * @description I/O error
     */
    export const EIO: 5;

    /**
     * @description Socket is connected
     */
    export const EISCONN: 106;

    /**
     * @description Is a directory
     */
    export const EISDIR: 21;

    /**
     * @description Too many levels of symbolic links
     */
    export const ELOOP: 40;

    /**
     * @description Too many open files
     */
    export const EMFILE: 24;

    /**
     * @description Too many links
     */
    export const EMLINK: 31;

    /**
     * @description Message too long
     */
    export const EMSGSIZE: 90;

    /**
     * @description Multihop attempted
     */
    export const EMULTIHOP: 72;

    /**
     * @description File name too long
     */
    export const ENAMETOOLONG: 36;

    /**
     * @description Network is down
     */
    export const ENETDOWN: 100;

    /**
     * @description Network dropped connection on reset
     */
    export const ENETRESET: 102;

    /**
     * @description Network is unreachable
     */
    export const ENETUNREACH: 101;

    /**
     * @description Too many open files in system
     */
    export const ENFILE: 23;

    /**
     * @description No buffer space available
     */
    export const ENOBUFS: 105;

    /**
     * @description No data available
     */
    export const ENODATA: 61;

    /**
     * @description No such device
     */
    export const ENODEV: 19;

    /**
     * @description No such file or directory
     */
    export const ENOENT: 2;

    /**
     * @description Exec format error
     */
    export const ENOEXEC: 8;

    /**
     * @description No locks available
     */
    export const ENOLCK: 37;

    /**
     * @description Link has been severed
     */
    export const ENOLINK: 67;

    /**
     * @description Out of memory
     */
    export const ENOMEM: 12;

    /**
     * @description No message of desired type
     */
    export const ENOMSG: 42;

    /**
     * @description Protocol not available
     */
    export const ENOPROTOOPT: 92;

    /**
     * @description No space left on device
     */
    export const ENOSPC: 28;

    /**
     * @description No STREAM resources
     */
    export const ENOSR: 63;

    /**
     * @description Not a STREAM
     */
    export const ENOSTR: 60;

    /**
     * @description Function not implemented
     */
    export const ENOSYS: 38;

    /**
     * @description Socket is not connected
     */
    export const ENOTCONN: 107;

    /**
     * @description Not a directory
     */
    export const ENOTDIR: 20;

    /**
     * @description Directory not empty
     */
    export const ENOTEMPTY: 39;

    /**
     * @description Not a socket
     */
    export const ENOTSOCK: 88;

    /**
     * @description Operation not supported
     */
    export const ENOTSUP: 95;

    /**
     * @description Inappropriate ioctl for device
     */
    export const ENOTTY: 25;

    /**
     * @description No such device or address
     */
    export const ENXIO: 6;

    /**
     * @description Operation not supported on socket
     */
    export const EOPNOTSUPP: 95;

    /**
     * @description Value too large to be stored in data type
     */
    export const EOVERFLOW: 75;

    /**
     * @description Operation not permitted
     */
    export const EPERM: 1;

    /**
     * @description Broken pipe
     */
    export const EPIPE: 32;

    /**
     * @description Protocol error
     */
    export const EPROTO: 71;

    /**
     * @description Protocol not supported
     */
    export const EPROTONOSUPPORT: 93;

    /**
     * @description Protocol wrong type for socket
     */
    export const EPROTOTYPE: 91;

    /**
     * @description Numerical result out of range
     */
    export const ERANGE: 34;

    /**
     * @description Read-only file system
     */
    export const EROFS: 30;

    /**
     * @description Invalid seek
     */
    export const ESPIPE: 29;

    /**
     * @description No such process
     */
    export const ESRCH: 3;

    /**
     * @description Stale file handle
     */
    export const ESTALE: 116;

    /**
     * @description Timer expired
     */
    export const ETIME: 62;

    /**
     * @description Connection timed out
     */
    export const ETIMEDOUT: 110;

    /**
     * @description Text file busy
     */
    export const ETXTBSY: 26;

    /**
     * @description Operation would block
     */
    export const EWOULDBLOCK: 11;

    /**
     * @description Cross-device link
     */
    export const EXDEV: 18;

}

