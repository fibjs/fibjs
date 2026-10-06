/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description The constants of the fs module: file open, access, seek, type, permission and copy
 *  flags plus the libuv directory entry type ids
 *
 *  Reach the object through `require('fs').constants` or `require('fs/promises').constants`
 *  (the same object); it is not requireable on its own. fs, FileHandle and the promise API
 *  accept these values.
 *
 *  Concepts:
 *
 *  - **Portable open flags**: the O_* values use the BSD/macOS numbering (O_APPEND 8, O_CREAT
 *    512, O_TRUNC 1024, O_EXCL 2048, O_NONBLOCK 4, O_SYNC 128, O_DSYNC 4194304, O_NOCTTY
 *    131072, O_DIRECTORY 1048576, O_NOFOLLOW 256, O_SYMLINK 2097152) and fs.open translates
 *    them to the flags of the host. They differ from `<fcntl.h>` values (on Linux O_CREAT is
 *    64, O_TRUNC 512, O_APPEND 1024, O_NONBLOCK 2048): always combine these constants instead
 *    of native literals, or a flag ends up meaning something else.
 *  - **Access flags**: F_OK/R_OK/W_OK/X_OK are the mode bits of fs.access; F_OK only checks
 *    existence, and X_OK behaves like F_OK on Windows.
 *  - **File types and modes**: S_IFMT extracts the type bits of Stat#mode and S_IF* name them;
 *    the S_IRWXU/S_IRUSR/... groups are the permission bits of chmod and of the mode argument
 *    of fs.open.
 *  - **Seek modes**: SEEK_SET/SEEK_CUR/SEEK_END (0/1/2) are exported for compatibility, but no
 *    current fibjs API takes a whence argument: positioned reads and writes take an absolute
 *    position, and a position of -1 means the current position.
 *  - **Copy flags**: COPYFILE_EXCL, COPYFILE_FICLONE and COPYFILE_FICLONE_FORCE (1/2/4) select
 *    the behavior of fs.copyFile: EXCL fails when the destination exists, and the FICLONE pair
 *    requests a copy-on-write reflink (with a fallback copy unless FORCE is set).
 *  - **Directory entries**: the UV_DIRENT_* ids are the libuv uv_dirent_type_t values reported
 *    by the directory scan; fs.Dirent exposes the same information through isXxx().
 *  - **Extensions and gaps**: O_SYMLINK, SEEK_*, the EXTENSIONLESS_FORMAT_* pair (reserved
 *    format ids that mirror Node.js internals) and UV_FS_O_FILEMAP (0 here) are fibjs-specific;
 *    Node's O_DIRECT and O_NOATIME are not provided.
 *
 *  Import:
 *  ```JavaScript
 *  const fs = require('fs');
 *  const constants = fs.constants; // also require('fs/promises').constants
 *  ```
 *
 *  Example 1 — create a file with the portable open flags:
 *  ```JavaScript
 *  const fs = require('fs');
 *  const os = require('os');
 *  const path = require('path');
 *  const C = fs.constants;
 *
 *  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-fsflags-'));
 *  const file = path.join(dir, 'note.txt');
 *
 *  // O_WRONLY | O_CREAT | O_TRUNC: 1 | 512 | 1024, translated by fs.open.
 *  const handle = fs.open(file, C.O_WRONLY | C.O_CREAT | C.O_TRUNC, 0o644);
 *  handle.write('written with portable flags');
 *  handle.close();
 *
 *  console.log(fs.readFile(file, 'utf8'));        // written with portable flags
 *  console.log(C.O_CREAT, C.O_TRUNC, C.O_APPEND); // 512 1024 8
 *
 *  fs.rmSync(dir, { recursive: true, force: true });
 *  ```
 *
 *  Example 2 — exclusive creation and append mode:
 *  ```JavaScript
 *  const fs = require('fs');
 *  const os = require('os');
 *  const path = require('path');
 *  const C = fs.constants;
 *
 *  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-fsflags-'));
 *  const file = path.join(dir, 'log.txt');
 *
 *  // O_EXCL makes the second open of the same path fail.
 *  const first = fs.open(file, C.O_WRONLY | C.O_CREAT | C.O_EXCL, 0o644);
 *  first.write('first');
 *  first.close();
 *
 *  try {
 *      fs.open(file, C.O_WRONLY | C.O_CREAT | C.O_EXCL, 0o644);
 *  } catch (e) {
 *      console.log(e.code); // EEXIST
 *  }
 *
 *  // O_APPEND writes at the end without an explicit position.
 *  const appender = fs.open(file, C.O_WRONLY | C.O_APPEND);
 *  appender.write(' second');
 *  appender.close();
 *  console.log(fs.readFile(file, 'utf8')); // first second
 *
 *  fs.rmSync(dir, { recursive: true, force: true });
 *  ```
 *
 *  Example 3 — file types, access checks and directory entry ids:
 *  ```JavaScript
 *  const fs = require('fs');
 *  const os = require('os');
 *  const path = require('path');
 *  const C = fs.constants;
 *
 *  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-fsflags-'));
 *  fs.writeFile(path.join(dir, 'data.bin'), 'x');
 *  fs.mkdir(path.join(dir, 'sub'));
 *  fs.symlink(path.join(dir, 'data.bin'), path.join(dir, 'link'));
 *
 *  // S_IFMT extracts the S_IF* type bits of a Stat#mode.
 *  function kind(file) {
 *      const mode = fs.lstat(file).mode;
 *      if ((mode & C.S_IFMT) === C.S_IFLNK) return 'link';
 *      if ((mode & C.S_IFMT) === C.S_IFDIR) return 'dir';
 *      return 'file';
 *  }
 *  console.log(kind(path.join(dir, 'data.bin')), kind(path.join(dir, 'sub')),
 *      kind(path.join(dir, 'link'))); // file dir link
 *
 *  // F_OK checks existence; R_OK/W_OK/X_OK check the access of the current user.
 *  fs.access(path.join(dir, 'data.bin'), C.F_OK | C.R_OK);
 *  console.log('readable');
 *  try {
 *      fs.access(path.join(dir, 'data.bin'), C.X_OK);
 *  } catch (e) {
 *      console.log('not executable:', e.code); // not executable: EACCES (POSIX)
 *  }
 *
 *  // The directory scan reports the UV_DIRENT_* ids for fs.Dirent.
 *  console.log(C.UV_DIRENT_FILE, C.UV_DIRENT_DIR, C.UV_DIRENT_LINK); // 1 2 3
 *
 *  fs.rmSync(dir, { recursive: true, force: true });
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
     * @description File mapping flag; 0 in fibjs (used by libuv on Windows only)
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
     * @description Extensionless format id for JavaScript (reserved; mirrors Node.js internals)
     */
    export const EXTENSIONLESS_FORMAT_JAVASCRIPT: 0;

    /**
     * @description Extensionless format id for WebAssembly (reserved; mirrors Node.js internals)
     */
    export const EXTENSIONLESS_FORMAT_WASM: 1;

}

