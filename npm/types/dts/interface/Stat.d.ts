/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description File status information object
 *
 *   A Stat describes one file system entry: its type, size, permissions, owner and timestamps. It
 *   is returned by fs.stat, fs.lstat and fs.fstat (synchronous, callback or fs.promises forms) and
 *   is passed to the fs.watchFile callback; it cannot be created with `new`.
 *
 *  Concepts:
 *
 *  - **Followed or not**: fs.stat describes the target of a symbolic link, fs.lstat describes the
 *    link itself (`isSymbolicLink()` is true); the isXxx() predicates all test the mode of the
 *    described entry, so a link reported by lstat is neither a file nor a directory.
 *  - **Timestamps**: mtime is the last modification time, atime the last access time, ctime the
 *    last status (inode) change time and birthtime the creation time. Each timestamp is exposed as
 *    a Date, as milliseconds since the Unix epoch (mtimeMs) and as the nanosecond part of the
 *    timestamp (mtimeNs, 0-999999999). The Node.js `bigint` option is not implemented, so the Ns
 *    properties are always numbers and no epoch-nanosecond values are produced.
 *  - **name**: the base name of the queried path (a fibjs extension, Node.js Stats has no name);
 *    it is empty for objects that are not bound to a path.
 *  - **Permissions**: isReadable/isWritable/isExecutable test the owner permission bits of mode,
 *    not the effective access of the current user. isHidden/isMemory/isSocket are fibjs extensions;
 *    isMemory is true for entries served by the zip VFS, whose mode-based type predicates are not
 *    populated.
 *
 *  Obtained from:
 *  - `fs.stat(path)` / `fs.stat(path, options)` — the target of a symbolic link;
 *  - `fs.lstat(path)` — the link itself;
 *  - `fs.fstat(fd)` — an open file descriptor or FileHandle;
 *  - the `fs.watchFile` callback — the status before and after a detected change.
 *
 *  Example 1 — inspect a file created in a temporary directory:
 *  ```JavaScript
 *  const fs = require('fs');
 *  const os = require('os');
 *  const path = require('path');
 *
 *  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-stat-'));
 *  const file = path.join(dir, 'data.txt');
 *  fs.writeFile(file, 'hello');
 *
 *  const st = fs.stat(file);
 *  console.log(st.name, st.size, st.isFile(), st.isDirectory());
 *  console.log(st.mtime instanceof Date, typeof st.mtimeMs, st.mtimeNs >= 0);
 *
 *  fs.rmSync(dir, { recursive: true, force: true });
 *  ```
 *
 *  Example 2 — compare stat and lstat on a symbolic link:
 *  ```JavaScript
 *  const fs = require('fs');
 *  const os = require('os');
 *  const path = require('path');
 *
 *  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-stat-'));
 *  const target = path.join(dir, 'target.txt');
 *  const link = path.join(dir, 'link.txt');
 *  fs.writeFile(target, 'hello');
 *  fs.symlink(target, link);
 *
 *  console.log(fs.stat(link).isFile()); // true, the link is followed
 *  console.log(fs.lstat(link).isSymbolicLink()); // true, the link itself
 *
 *  fs.rmSync(dir, { recursive: true, force: true });
 *  ```
 *
 *  Example 3 — fstat an open file descriptor:
 *  ```JavaScript
 *  const fs = require('fs');
 *  const os = require('os');
 *  const path = require('path');
 *
 *  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-stat-'));
 *  const file = path.join(dir, 'open.txt');
 *  fs.writeFile(file, 'content');
 *
 *  const handle = fs.open(file);
 *  const st = fs.fstat(handle);
 *  console.log(st.isFile(), st.size); // true 7
 *
 *  fs.close(handle);
 *  fs.rmSync(dir, { recursive: true, force: true });
 *  ```
 *
 *  The Date and millisecond representations are independent: changing one is not reflected in the
 *  others. On platforms where birthtime is not available the birthtime fields may hold the ctime
 *  value or the Unix epoch.
 *
 */
declare class Class_Stat extends Class_object {
    /**
     * @description File name
     *
     *      The base name of the queried path, for example 'report.txt' for '/data/report.txt'; empty
     *      when the object was not created from a path. Not provided by Node.js Stats (fibjs extension).
     *
     */
    readonly name: string;

    /**
     * @description Device ID containing the file
     *
     *      Numeric identifier of the device that holds the entry, as reported by the operating system;
     *      together with ino it identifies the file on the system.
     *
     */
    readonly dev: number;

    /**
     * @description Number of Inodes in the file
     *
     *      File system specific inode number; it stays stable while the entry exists and may be reused
     *      after removal.
     *
     */
    readonly ino: number;

    /**
     * @description File permission; not supported on Windows
     *
     *      Bit field holding the file type in the S_IFMT bits and the permission bits used by the isXxx
     *      predicates; only the write bit is meaningful on Windows.
     *
     */
    readonly mode: number;

    /**
     * @description Number of hard links associated with this file
     *
     *      A regular file with one name reports 1; directories report the number of contained
     *      subdirectories plus two.
     *
     */
    readonly nlink: number;

    /**
     * @description Owner id of the file
     *
     *      Numeric user id of the owner (POSIX); 0 on platforms without POSIX ownership.
     *
     */
    readonly uid: number;

    /**
     * @description Group id of the file
     *
     *      Numeric group id of the owner (POSIX); 0 on platforms without POSIX ownership.
     *
     */
    readonly gid: number;

    /**
     * @description For special file types, the device ID containing the file
     *
     *      Device identifier for character or block special files; 0 for regular files and directories.
     *
     */
    readonly rdev: number;

    /**
     * @description File size
     *
     *      Size in bytes; 0 for directories and for file systems that do not report it. The value is
     *      exposed as a Number, so files larger than 2^53 bytes lose precision.
     *
     *      Example — check the size of a file with known content:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-size-'));
     *      const file = path.join(dir, 'size.txt');
     *      fs.writeFile(file, '12345');
     *      console.log(fs.stat(file).size); // 5
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *
     */
    readonly size: number;

    /**
     * @description File system block size for I/O operations
     *
     *      Preferred block size reported by the file system, in bytes; typically 4096 on modern
     *      systems.
     *
     */
    readonly blksize: number;

    /**
     * @description Number of blocks allocated to the file
     *
     *      Count of 512-byte blocks actually allocated, which may be larger than size/512 because of
     *      pre-allocation; 0 on Windows.
     *
     */
    readonly blocks: number;

    /**
     * @description Last modification time of the file
     *
     *      Time of the last content change as a Date, usually updated by write, truncate and utimes.
     *
     */
    readonly mtime: Date;

    /**
     * @description Last modification time of the file (ms)
     *
     *      The same instant as mtime, expressed in milliseconds since the Unix epoch; the two
     *      representations are independent objects and updating one does not change the other.
     *
     */
    readonly mtimeMs: number;

    /**
     * @description Nanosecond part of the last modification time
     *
     *      Holds the sub-second nanosecond part (0-999999999) of the timestamp. Unlike Node.js this
     *      property is always available; the bigint option that would produce epoch-nanosecond values
     *      is not implemented.
     *
     */
    readonly mtimeNs: number;

    /**
     * @description Last access time of the file
     *
     *      Time of the last access as a Date, updated by reads depending on the file system mount
     *      options (some systems mount with noatime).
     *
     */
    readonly atime: Date;

    /**
     * @description Last access time of the file (ms)
     *
     *      The same instant as atime, expressed in milliseconds since the Unix epoch.
     *
     */
    readonly atimeMs: number;

    /**
     * @description Nanosecond part of the last access time
     *
     *      Holds the sub-second nanosecond part (0-999999999) of the timestamp; always present, the
     *      bigint option is not implemented (see mtimeNs).
     *
     */
    readonly atimeNs: number;

    /**
     * @description File status change time
     *
     *      Time of the last inode change as a Date; updated by operations that modify the status such
     *      as chmod, chown, link, rename and unlink, not only by writes.
     *
     */
    readonly ctime: Date;

    /**
     * @description File status change time (ms)
     *
     *      The same instant as ctime, expressed in milliseconds since the Unix epoch.
     *
     */
    readonly ctimeMs: number;

    /**
     * @description Nanosecond part of the status change time
     *
     *      Holds the sub-second nanosecond part (0-999999999) of the timestamp; always present, the
     *      bigint option is not implemented (see mtimeNs).
     *
     */
    readonly ctimeNs: number;

    /**
     * @description File creation time
     *
     *      Creation time as a Date; file systems that do not record it may report the ctime value or
     *      the Unix epoch instead.
     *
     */
    readonly birthtime: Date;

    /**
     * @description File creation time (ms)
     *
     *      The same instant as birthtime, expressed in milliseconds since the Unix epoch.
     *
     */
    readonly birthtimeMs: number;

    /**
     * @description Nanosecond part of the creation time
     *
     *      Holds the sub-second nanosecond part (0-999999999) of the timestamp; always present, the
     *      bigint option is not implemented (see mtimeNs).
     *
     */
    readonly birthtimeNs: number;

    /**
     * @description Queries whether the file is writable
     *
     *      Tests the owner write bit (S_IWUSR) of mode, not the effective access of the current user;
     *      Node.js Stats has no such method.
     *      @return true if it is writable
     *
     */
    isWritable(): boolean;

    /**
     * @description Queries whether the file is readable
     *
     *      Tests the owner read bit (S_IRUSR) of mode, not the effective access of the current user;
     *      Node.js Stats has no such method.
     *      @return true if it is readable
     *
     */
    isReadable(): boolean;

    /**
     * @description Queries whether the file is executable
     *
     *      Tests the owner execute/search bit (S_IXUSR) of mode, not the effective access of the
     *      current user; Node.js Stats has no such method.
     *      @return true if it is executable
     *
     */
    isExecutable(): boolean;

    /**
     * @description Queries whether the file is hidden
     *
     *      Hidden-file flag; on POSIX the flag is not populated, so dotfiles report false. fibjs
     *      extension, Node.js Stats has no such method.
     *      @return true if it is hidden
     *
     */
    isHidden(): boolean;

    /**
     * @description Queries whether the Stat describes a block device
     *
     *      Tests the S_IFBLK type bits of mode; always false on Windows.
     *      @return true if it describes a block device
     *
     */
    isBlockDevice(): boolean;

    /**
     * @description Queries whether the Stat describes a character device
     *
     *      Tests the S_IFCHR type bits of mode; character devices include terminals and serial ports.
     *      @return true if it describes a character device
     *
     */
    isCharacterDevice(): boolean;

    /**
     * @description Queries whether the file is a directory
     *
     *      Tests the S_IFDIR type bits of mode. For a symbolic link described by lstat this is false,
     *      because the link itself is not a directory.
     *
     *      Example — distinguish a file from a directory:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-dir-'));
     *      const file = path.join(dir, 'a.txt');
     *      fs.writeFile(file, 'a');
     *
     *      console.log(fs.stat(dir).isDirectory()); // true
     *      console.log(fs.stat(file).isDirectory()); // false
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @return true if it is a directory
     *
     */
    isDirectory(): boolean;

    /**
     * @description Queries whether the Stat describes a FIFO pipe
     *
     *      Tests the S_IFIFO type bits of mode; always false on Windows.
     *      @return true if it describes a FIFO pipe
     *
     */
    isFIFO(): boolean;

    /**
     * @description Queries whether the file is a file
     *
     *      Tests the S_IFREG type bits of mode; directories and links reported by lstat are not files.
     *      @return true if it is a file
     *
     */
    isFile(): boolean;

    /**
     * @description Queries whether the file is a symbolic link
     *
     *      Tests the S_IFLNK type bits of mode; only lstat results are expected to report it, since
     *      stat follows the link and describes its target.
     *
     *      Example — lstat reports a link, stat reports its target:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-link-'));
     *      const target = path.join(dir, 'target.txt');
     *      const link = path.join(dir, 'link.txt');
     *      fs.writeFile(target, 'x');
     *      fs.symlink(target, link);
     *
     *      console.log(fs.lstat(link).isSymbolicLink()); // true
     *      console.log(fs.stat(link).isSymbolicLink()); // false
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @return true if it is a symbolic link
     *
     */
    isSymbolicLink(): boolean;

    /**
     * @description Queries whether the file is a memory file
     *
     *      True when the entry is served by the zip virtual file system (fs.setZipFS) instead of the
     *      real file system; such entries have no populated type bits, so the other predicates report
     *      false. fibjs extension, Node.js Stats has no such method.
     *      @return true if it is a memory file
     *
     */
    isMemory(): boolean;

    /**
     * @description Queries whether the file is a Socket
     *
     *      Tests the S_IFSOCK type bits of mode (the flag copied from another Stat); always false on
     *      Windows.
     *      @return true if it is a Socket
     *
     */
    isSocket(): boolean;

}

