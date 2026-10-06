/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../module/fs_constants.d.ts" />
/// <reference path="../interface/Stat.d.ts" />
/// <reference path="../interface/DirEntry.d.ts" />
/// <reference path="../interface/Dir.d.ts" />
/// <reference path="../interface/FileHandle.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/SeekableStream.d.ts" />
/// <reference path="../interface/BufferedStream.d.ts" />
/// <reference path="../interface/FSWatcher.d.ts" />
/// <reference path="../interface/StatsWatcher.d.ts" />
/**
 * @description The fs module provides file system operations: reading and writing files and directories, creating and removing them, changing permissions, querying status, resolving paths and watching files; useful for file management, logging and persisted configuration
 *
 *  Main capabilities:
 *
 *  - **Paths and existence**: `exists`, `access`, `realpath`, `readlink`, `symlink`, `link`;
 *  - **Directory operations**: `mkdir`, `mkdtemp`, `rmdir`, `rm`, `readdir`, `glob`;
 *  - **File operations**: `readFile`, `writeFile`, `appendFile`, `rename`, `copyFile`, `cp`,
 *    `truncate`, `unlink`, `chmod`, `chown`, `utimes`;
 *  - **File descriptor operations**: `open`, `close`, `read`, `write`, `fstat`, `fsync`, `fchmod`
 *    and more;
 *  - **File streams**: `openFile`, `openTextStream`, `createReadStream`, `createWriteStream`;
 *  - **File watching**: `watch`, `watchFile`, `unwatchFile`;
 *  - **zip virtual file system**: `setZipFS`, `clearZipFS`.
 *
 *  Concepts:
 *
 *  - **Call forms**: every function works synchronously without a callback and asynchronously with a
 *    trailing `(err, result)` callback. `Sync`-suffixed aliases (such as `readFileSync`) and the
 *    promise-based `fs.promises` namespace are also available.
 *  - **File descriptors and position**: `open` returns a FileHandle wrapping a descriptor; `read`
 *    and `write` take an explicit `position` and use the current file position when it is negative,
 *    so one descriptor can serve both sequential and random access. readFile/writeFile/appendFile do
 *    not close a descriptor; release it with `close`.
 *  - **flags**: the `flags` argument of the open functions accepts the strings `'r'`, `'r+'`, `'w'`,
 *    `'w+'`, `'a'`, `'a+'` or a bitwise combination of the integer flags in `fs.constants`; the
 *    full list is documented in the fs_constants module.
 *  - **Symbolic links**: stat follows a link and describes its target, while lstat describes the
 *    link itself (`isSymbolicLink()` is true); unlink and rm remove the link, not its target.
 *  - **Watching**: watch uses the platform notification service and reports the `'change'`,
 *    `'changeonly'` and `'renameonly'` events; watchFile polls the status and passes
 *    `(curStats, prevStats)` to the callback. The `recursive` option of watch is only stable on
 *    win32/darwin; on Linux it is forwarded to the uv backend but may report events at unexpected
 *    times.
 *  - **zip VFS**: setZipFS maps zip data onto a path; entries are then read through the mapping
 *    path with a `$` suffix, for example `/archive.zip$/dir/file.txt`.
 *  - **Streams**: createReadStream/createWriteStream open a file as a SeekableStream, and
 *    createReadStream accepts an inclusive `[start, end]` range; see the io module for stream
 *    positioning and back pressure.
 *
 *  Import:
 *  ```JavaScript
 *  const fs = require('fs');
 *  ```
 *
 *  Example 1 — synchronous write, read and stat in a temporary directory:
 *  ```JavaScript
 *  const fs = require('fs');
 *  const os = require('os');
 *  const path = require('path');
 *
 *  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-fs-'));
 *  const file = path.join(dir, 'hello.txt');
 *
 *  fs.writeFile(file, 'hello, world!');
 *  console.log(fs.readFile(file, 'utf8')); // hello, world!
 *  console.log(fs.stat(file).size); // 13
 *
 *  fs.rmSync(dir, { recursive: true, force: true });
 *  ```
 *
 *  Example 2 — callback and stream forms of the same operations:
 *  ```JavaScript
 *  const fs = require('fs');
 *  const os = require('os');
 *  const path = require('path');
 *
 *  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-fs-'));
 *  const file = path.join(dir, 'async.txt');
 *
 *  fs.writeFile(file, 'written with a callback', 'utf8', (err) => {
 *      if (err) throw err;
 *      fs.readFile(file, 'utf8', (err, text) => {
 *          if (err) throw err;
 *          console.log(text);
 *          // a read stream exposes the whole file through readAll()
 *          console.log(fs.createReadStream(file).readAll().toString());
 *          fs.rmSync(dir, { recursive: true, force: true });
 *      });
 *  });
 *  ```
 *
 *  Example 3 — create, list and remove a directory tree:
 *  ```JavaScript
 *  const fs = require('fs');
 *  const os = require('os');
 *  const path = require('path');
 *
 *  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-fs-'));
 *  fs.writeFile(path.join(dir, 'notes.txt'), 'note');
 *  fs.mkdir(path.join(dir, 'images'));
 *
 *  fs.readdir(dir, { withFileTypes: true }).forEach((entry) => {
 *      console.log(entry.name, entry.isDirectory() ? 'dir' : 'file');
 *  });
 *
 *  fs.rmSync(dir, { recursive: true, force: true });
 *  ```
 *
 *  Notes:
 *
 *  - `readFile` returns a Buffer by default and a string when an encoding is given; a descriptor
 *    read with an options object defaults to utf8.
 *  - writeFile(fd) seeks to the beginning and truncates the file before writing, which differs from
 *    Node.js where a descriptor write starts at the current position.
 *  - The Node.js `bigint` option is accepted in the options of the stat and watch functions but is
 *    not implemented: the `Ns` properties of Stat are always numbers holding the nanosecond part.
 *  - watch returns a watcher deriving from EventEmitter and watchFile returns a StatsWatcher
 *    deriving from EventEmitter; calling `fs.unwatchFile(target)` is equivalent to calling
 *    `StatsWatcher.close()`.
 *
 */
declare module 'fs' {
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
     * @description File existence check constant, see fs_constants
     */
    export const F_OK: 0;

    /**
     * @description Read permission check constant, see fs_constants
     */
    export const R_OK: 4;

    /**
     * @description Write permission check constant, see fs_constants
     */
    export const W_OK: 2;

    /**
     * @description Execute permission check constant, see fs_constants
     */
    export const X_OK: 1;

    /**
     * ! The constants object of the fs module, see fs_constants
     *
     *      It exposes the file access (F_OK, R_OK, W_OK, X_OK), copy (COPYFILE_*), open (O_*),
     *      file type (S_IF*) and permission (S_I*) constants used across the module; the full list
     *      is documented in the fs_constants module.
     *
     */
    const constants: typeof import ('fs_constants');

    /**
     * @description The alias of the Stat class, see Stat
     *
     *      Stat objects are returned by stat/lstat/fstat; Node.js exposes the same class as fs.Stats,
     *      while readdir with `withFileTypes` returns DirEntry objects instead.
     *
     */
    const Stats: typeof Class_Stat;

    /**
     * @description The alias of the DirEntry class, see DirEntry
     *
     *      A directory entry pairs a file name with its type, as returned by readdir with the
     *      `withFileTypes` option; Node.js exposes the same class as fs.Dirent.
     *
     */
    const Dirent: typeof Class_DirEntry;

    /**
     * @description The alias of the Dir class, see Dir
     *
     *      The directory iterator returned by opendir; entries can be read one by one with
     *      read/readSync or with `for await...of`. Node.js exposes the same class as fs.Dir.
     *
     */
    const Dir: typeof Class_Dir;

    /**
     * @description Checks whether the given file or directory exists
     *
     *      Returns false instead of throwing when the path does not exist.
     *      @param path the path to check
     *      @return true when the file or directory exists
     *
     */
    function exists(path: string): boolean;

    function exists(path: string, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description Checks whether the given file or directory exists
     *
     *      Returns false instead of throwing when the path does not exist.
     *      @param path the path to check
     *      @return true when the file or directory exists
     *
     */
    function existsSync(path: string): boolean;

    /**
     * @description Checks whether the given file or directory exists
     *
     *      Returns false instead of throwing when the path does not exist.
     *      @param path the path to check
     *      @return true when the file or directory exists
     *
     */
    function existsAsync(path: string): Promise<boolean>;

    /**
     * @description Checks whether the given file exists
     *
     *      The options parameter is kept for Node.js compatibility only and is ignored for now.
     *      @param path the path to check
     *      @param options the check options (ignored)
     *      @return true when the file exists
     *
     */
    function exists(path: string, options: FIBJS.GeneralObject): boolean;

    function exists(path: string, options: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description Checks whether the given file exists
     *
     *      The options parameter is kept for Node.js compatibility only and is ignored for now.
     *      @param path the path to check
     *      @param options the check options (ignored)
     *      @return true when the file exists
     *
     */
    function existsSync(path: string, options: FIBJS.GeneralObject): boolean;

    /**
     * @description Checks whether the given file exists
     *
     *      The options parameter is kept for Node.js compatibility only and is ignored for now.
     *      @param path the path to check
     *      @param options the check options (ignored)
     *      @return true when the file exists
     *
     */
    function existsAsync(path: string, options: FIBJS.GeneralObject): Promise<boolean>;

    /**
     * @description Checks the permissions of the current user on the given file
     *
     *      mode specifies the permissions to check, a combination of F_OK, R_OK, W_OK and X_OK from fs.constants, F_OK (file existence) by default. A failed check throws an exception.
     *      @param path the path to check
     *      @param mode the permissions to check, file existence by default
     *
     */
    function access(path: string, mode?: number): void;

    function access(path: string, mode?: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Checks the permissions of the current user on the given file
     *
     *      mode specifies the permissions to check, a combination of F_OK, R_OK, W_OK and X_OK from fs.constants, F_OK (file existence) by default. A failed check throws an exception.
     *      @param path the path to check
     *      @param mode the permissions to check, file existence by default
     *
     */
    function accessSync(path: string, mode?: number): void;

    /**
     * @description Checks the permissions of the current user on the given file
     *
     *      mode specifies the permissions to check, a combination of F_OK, R_OK, W_OK and X_OK from fs.constants, F_OK (file existence) by default. A failed check throws an exception.
     *      @param path the path to check
     *      @param mode the permissions to check, file existence by default
     *
     */
    function accessAsync(path: string, mode?: number): Promise<void>;

    /**
     * @description Creates a hard link; not supported on Windows
     *
     *      oldPath and newPath then refer to the same file content and share one inode, so removing
     *      one name does not remove the other. Throws EEXIST when newPath already exists.
     *      @param oldPath the source file
     *      @param newPath the file to create
     *
     */
    function link(oldPath: string, newPath: string): void;

    function link(oldPath: string, newPath: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Creates a hard link; not supported on Windows
     *
     *      oldPath and newPath then refer to the same file content and share one inode, so removing
     *      one name does not remove the other. Throws EEXIST when newPath already exists.
     *      @param oldPath the source file
     *      @param newPath the file to create
     *
     */
    function linkSync(oldPath: string, newPath: string): void;

    /**
     * @description Creates a hard link; not supported on Windows
     *
     *      oldPath and newPath then refer to the same file content and share one inode, so removing
     *      one name does not remove the other. Throws EEXIST when newPath already exists.
     *      @param oldPath the source file
     *      @param newPath the file to create
     *
     */
    function linkAsync(oldPath: string, newPath: string): Promise<void>;

    /**
     * @description Removes the given file
     *
     *      Throws when the file does not exist. When the path points to a directory the behavior is platform dependent; use rmdir or rm to remove directories.
     *      @param path the path to remove
     *
     */
    function unlink(path: string): void;

    function unlink(path: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Removes the given file
     *
     *      Throws when the file does not exist. When the path points to a directory the behavior is platform dependent; use rmdir or rm to remove directories.
     *      @param path the path to remove
     *
     */
    function unlinkSync(path: string): void;

    /**
     * @description Removes the given file
     *
     *      Throws when the file does not exist. When the path points to a directory the behavior is platform dependent; use rmdir or rm to remove directories.
     *      @param path the path to remove
     *
     */
    function unlinkAsync(path: string): Promise<void>;

    /**
     * @description Creates a directory
     *
     *      mode specifies the directory permissions and is ignored on Windows; an existing directory throws, unless the recursive option is used to create parent directories. A string mode is an octal number (such as '755' or '0755'), consistent with Node.js; an invalid mode throws.
     *
     *      The options object may contain:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          recursive: false, // specify whether parent directories should be created. Default: false
     *          mode: 0777 // specify the file mode. Default: 0777
     *      })
     *      ```
     *
     *      When recursive is true, the path of the first created directory is returned, consistent with Node.js; when the directory already exists, undefined is returned.
     *      @param path the directory to create
     *      @param mode the file mode or the creation options
     *      @return the path of the first created directory when recursive is true and a directory was actually created
     *
     */
    function mkdir(path: string, mode?: number | FIBJS.GeneralObject | any): any;

    function mkdir(path: string, mode?: number | FIBJS.GeneralObject | any, callback: (err: Error | undefined | null, retVal: any)=>any): void;

    /**
     * @description Creates a directory
     *
     *      mode specifies the directory permissions and is ignored on Windows; an existing directory throws, unless the recursive option is used to create parent directories. A string mode is an octal number (such as '755' or '0755'), consistent with Node.js; an invalid mode throws.
     *
     *      The options object may contain:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          recursive: false, // specify whether parent directories should be created. Default: false
     *          mode: 0777 // specify the file mode. Default: 0777
     *      })
     *      ```
     *
     *      When recursive is true, the path of the first created directory is returned, consistent with Node.js; when the directory already exists, undefined is returned.
     *      @param path the directory to create
     *      @param mode the file mode or the creation options
     *      @return the path of the first created directory when recursive is true and a directory was actually created
     *
     */
    function mkdirSync(path: string, mode?: number | FIBJS.GeneralObject | any): any;

    /**
     * @description Creates a directory
     *
     *      mode specifies the directory permissions and is ignored on Windows; an existing directory throws, unless the recursive option is used to create parent directories. A string mode is an octal number (such as '755' or '0755'), consistent with Node.js; an invalid mode throws.
     *
     *      The options object may contain:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          recursive: false, // specify whether parent directories should be created. Default: false
     *          mode: 0777 // specify the file mode. Default: 0777
     *      })
     *      ```
     *
     *      When recursive is true, the path of the first created directory is returned, consistent with Node.js; when the directory already exists, undefined is returned.
     *      @param path the directory to create
     *      @param mode the file mode or the creation options
     *      @return the path of the first created directory when recursive is true and a directory was actually created
     *
     */
    function mkdirAsync(path: string, mode?: number | FIBJS.GeneralObject | any): Promise<any>;

    /**
     * @description Creates a unique temporary directory
     *
     *      The directory is created under the system temporary directory, its name starts with prefix and ends with a random suffix.
     *
     *      Example — create a temporary directory and remove it afterwards:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-tmp-'));
     *      console.log(fs.stat(dir).isDirectory()); // true
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param prefix the prefix of the temporary directory name
     *      @return the path of the created temporary directory
     *
     */
    function mkdtemp(prefix: string): string;

    function mkdtemp(prefix: string, callback: (err: Error | undefined | null, retVal: string)=>any): void;

    /**
     * @description Creates a unique temporary directory
     *
     *      The directory is created under the system temporary directory, its name starts with prefix and ends with a random suffix.
     *
     *      Example — create a temporary directory and remove it afterwards:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-tmp-'));
     *      console.log(fs.stat(dir).isDirectory()); // true
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param prefix the prefix of the temporary directory name
     *      @return the path of the created temporary directory
     *
     */
    function mkdtempSync(prefix: string): string;

    /**
     * @description Creates a unique temporary directory
     *
     *      The directory is created under the system temporary directory, its name starts with prefix and ends with a random suffix.
     *
     *      Example — create a temporary directory and remove it afterwards:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-tmp-'));
     *      console.log(fs.stat(dir).isDirectory()); // true
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param prefix the prefix of the temporary directory name
     *      @return the path of the created temporary directory
     *
     */
    function mkdtempAsync(prefix: string): Promise<string>;

    /**
     * @description Removes a directory
     *
     *      The options may contain:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          recursive: false // remove all subdirectories and files. Default: false
     *      })
     *      ```
     *      @param path the directory to remove
     *      @param opt the removal options
     *
     */
    function rmdir(path: string, opt?: FIBJS.GeneralObject): void;

    function rmdir(path: string, opt?: FIBJS.GeneralObject, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Removes a directory
     *
     *      The options may contain:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          recursive: false // remove all subdirectories and files. Default: false
     *      })
     *      ```
     *      @param path the directory to remove
     *      @param opt the removal options
     *
     */
    function rmdirSync(path: string, opt?: FIBJS.GeneralObject): void;

    /**
     * @description Removes a directory
     *
     *      The options may contain:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          recursive: false // remove all subdirectories and files. Default: false
     *      })
     *      ```
     *      @param path the directory to remove
     *      @param opt the removal options
     *
     */
    function rmdirAsync(path: string, opt?: FIBJS.GeneralObject): Promise<void>;

    /**
     * @description Removes a file or directory
     *
     *      The options may contain:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          recursive: false, // remove all subdirectories and files. Default: false
     *          force: false // whether to ignore nonexistent paths. Default: false
     *      })
     *      ```
     *
     *      When recursive is false, only files and symbolic links can be removed; removing a directory throws EISDIR. When recursive is true, the directory and all its content are removed recursively; a symbolic link is removed itself without following the target. A nonexistent path throws ENOENT, unless force is true, which ignores nonexistent paths.
     *      @param path the directory to remove
     *      @param opt the removal options
     *
     */
    function rm(path: string, opt?: FIBJS.GeneralObject): void;

    function rm(path: string, opt?: FIBJS.GeneralObject, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Removes a file or directory
     *
     *      The options may contain:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          recursive: false, // remove all subdirectories and files. Default: false
     *          force: false // whether to ignore nonexistent paths. Default: false
     *      })
     *      ```
     *
     *      When recursive is false, only files and symbolic links can be removed; removing a directory throws EISDIR. When recursive is true, the directory and all its content are removed recursively; a symbolic link is removed itself without following the target. A nonexistent path throws ENOENT, unless force is true, which ignores nonexistent paths.
     *      @param path the directory to remove
     *      @param opt the removal options
     *
     */
    function rmSync(path: string, opt?: FIBJS.GeneralObject): void;

    /**
     * @description Removes a file or directory
     *
     *      The options may contain:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          recursive: false, // remove all subdirectories and files. Default: false
     *          force: false // whether to ignore nonexistent paths. Default: false
     *      })
     *      ```
     *
     *      When recursive is false, only files and symbolic links can be removed; removing a directory throws EISDIR. When recursive is true, the directory and all its content are removed recursively; a symbolic link is removed itself without following the target. A nonexistent path throws ENOENT, unless force is true, which ignores nonexistent paths.
     *      @param path the directory to remove
     *      @param opt the removal options
     *
     */
    function rmAsync(path: string, opt?: FIBJS.GeneralObject): Promise<void>;

    /**
     * @description Renames a file
     *
     *      Throws when the file does not exist or the target already exists.
     *      @param from the file to rename
     *      @param to the new file name
     *
     */
    function rename(from: string, to: string): void;

    function rename(from: string, to: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Renames a file
     *
     *      Throws when the file does not exist or the target already exists.
     *      @param from the file to rename
     *      @param to the new file name
     *
     */
    function renameSync(from: string, to: string): void;

    /**
     * @description Renames a file
     *
     *      Throws when the file does not exist or the target already exists.
     *      @param from the file to rename
     *      @param to the new file name
     *
     */
    function renameAsync(from: string, to: string): Promise<void>;

    /**
     * @description Copies src to dest. By default dest is overwritten when it already exists.
     *
     *      mode is an optional integer specifying the copy behavior. A mask can be built by bitwise-or of two or more values (for example fs.constants.COPYFILE_EXCL | fs.constants.COPYFILE_FICLONE).
     *      - fs.constants.COPYFILE_EXCL - the copy fails when dest already exists.
     *      - fs.constants.COPYFILE_FICLONE - the copy tries to create a copy-on-write link. When the platform does not support copy-on-write, the fallback copy mechanism is used.
     *      - fs.constants.COPYFILE_FICLONE_FORCE - the copy tries to create a copy-on-write link. When the platform does not support copy-on-write, the copy fails.
     *
     *      @param from the source file name
     *      @param to the target file name
     *      @param mode the modifiers of the copy operation, 0 by default
     *
     */
    function copyFile(from: string, to: string, mode?: number): void;

    function copyFile(from: string, to: string, mode?: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Copies src to dest. By default dest is overwritten when it already exists.
     *
     *      mode is an optional integer specifying the copy behavior. A mask can be built by bitwise-or of two or more values (for example fs.constants.COPYFILE_EXCL | fs.constants.COPYFILE_FICLONE).
     *      - fs.constants.COPYFILE_EXCL - the copy fails when dest already exists.
     *      - fs.constants.COPYFILE_FICLONE - the copy tries to create a copy-on-write link. When the platform does not support copy-on-write, the fallback copy mechanism is used.
     *      - fs.constants.COPYFILE_FICLONE_FORCE - the copy tries to create a copy-on-write link. When the platform does not support copy-on-write, the copy fails.
     *
     *      @param from the source file name
     *      @param to the target file name
     *      @param mode the modifiers of the copy operation, 0 by default
     *
     */
    function copyFileSync(from: string, to: string, mode?: number): void;

    /**
     * @description Copies src to dest. By default dest is overwritten when it already exists.
     *
     *      mode is an optional integer specifying the copy behavior. A mask can be built by bitwise-or of two or more values (for example fs.constants.COPYFILE_EXCL | fs.constants.COPYFILE_FICLONE).
     *      - fs.constants.COPYFILE_EXCL - the copy fails when dest already exists.
     *      - fs.constants.COPYFILE_FICLONE - the copy tries to create a copy-on-write link. When the platform does not support copy-on-write, the fallback copy mechanism is used.
     *      - fs.constants.COPYFILE_FICLONE_FORCE - the copy tries to create a copy-on-write link. When the platform does not support copy-on-write, the copy fails.
     *
     *      @param from the source file name
     *      @param to the target file name
     *      @param mode the modifiers of the copy operation, 0 by default
     *
     */
    function copyFileAsync(from: string, to: string, mode?: number): Promise<void>;

    /**
     * @description Copies src to dest asynchronously, including subdirectories and files.
     *
     *      When src is a directory, it is not copied recursively by default; set recursive to true for that.
     *
     *      opts supports the following options:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          recursive: false, // recursively copy directories. Default: false
     *          force: true, // overwrite existing files or directories. Default: true
     *          mode: 0 // modifiers for copy operation. Default: 0
     *      })
     *      ```
     *
     *      Copying a directory with recursive set to false throws, consistent with Node.js; an existing
     *      destination is overwritten unless force is false.
     *      @param src the source path to copy
     *      @param dest the target path to copy to
     *      @param opts the copy options
     *
     */
    function cp(src: string, dest: string, opts?: FIBJS.GeneralObject): void;

    function cp(src: string, dest: string, opts?: FIBJS.GeneralObject, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Copies src to dest asynchronously, including subdirectories and files.
     *
     *      When src is a directory, it is not copied recursively by default; set recursive to true for that.
     *
     *      opts supports the following options:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          recursive: false, // recursively copy directories. Default: false
     *          force: true, // overwrite existing files or directories. Default: true
     *          mode: 0 // modifiers for copy operation. Default: 0
     *      })
     *      ```
     *
     *      Copying a directory with recursive set to false throws, consistent with Node.js; an existing
     *      destination is overwritten unless force is false.
     *      @param src the source path to copy
     *      @param dest the target path to copy to
     *      @param opts the copy options
     *
     */
    function cpSync(src: string, dest: string, opts?: FIBJS.GeneralObject): void;

    /**
     * @description Copies src to dest asynchronously, including subdirectories and files.
     *
     *      When src is a directory, it is not copied recursively by default; set recursive to true for that.
     *
     *      opts supports the following options:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          recursive: false, // recursively copy directories. Default: false
     *          force: true, // overwrite existing files or directories. Default: true
     *          mode: 0 // modifiers for copy operation. Default: 0
     *      })
     *      ```
     *
     *      Copying a directory with recursive set to false throws, consistent with Node.js; an existing
     *      destination is overwritten unless force is false.
     *      @param src the source path to copy
     *      @param dest the target path to copy to
     *      @param opts the copy options
     *
     */
    function cpAsync(src: string, dest: string, opts?: FIBJS.GeneralObject): Promise<void>;

    /**
     * @description Sets the access permissions of the given file; not supported on Windows
     *
     *      mode may be a number or an octal string (such as '755' or '0755'), consistent with Node.js.
     *      @param path the file to operate on, a string is encoded as utf8
     *      @param mode the access permissions
     *
     */
    function chmod(path: string, mode: number | any): void;

    function chmod(path: string, mode: number | any, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Sets the access permissions of the given file; not supported on Windows
     *
     *      mode may be a number or an octal string (such as '755' or '0755'), consistent with Node.js.
     *      @param path the file to operate on, a string is encoded as utf8
     *      @param mode the access permissions
     *
     */
    function chmodSync(path: string, mode: number | any): void;

    /**
     * @description Sets the access permissions of the given file; not supported on Windows
     *
     *      mode may be a number or an octal string (such as '755' or '0755'), consistent with Node.js.
     *      @param path the file to operate on, a string is encoded as utf8
     *      @param mode the access permissions
     *
     */
    function chmodAsync(path: string, mode: number | any): Promise<void>;

    /**
     * @description Sets the access permissions of the given file without changing the target of a symbolic link; available on macOS and BSD platforms only
     *
     *      mode may be a number or an octal string (such as '755' or '0755'), consistent with Node.js.
     *      @param path the file to operate on, a string is encoded as utf8
     *      @param mode the access permissions
     *
     */
    function lchmod(path: string, mode: number | any): void;

    function lchmod(path: string, mode: number | any, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Sets the access permissions of the given file without changing the target of a symbolic link; available on macOS and BSD platforms only
     *
     *      mode may be a number or an octal string (such as '755' or '0755'), consistent with Node.js.
     *      @param path the file to operate on, a string is encoded as utf8
     *      @param mode the access permissions
     *
     */
    function lchmodSync(path: string, mode: number | any): void;

    /**
     * @description Sets the access permissions of the given file without changing the target of a symbolic link; available on macOS and BSD platforms only
     *
     *      mode may be a number or an octal string (such as '755' or '0755'), consistent with Node.js.
     *      @param path the file to operate on, a string is encoded as utf8
     *      @param mode the access permissions
     *
     */
    function lchmodAsync(path: string, mode: number | any): Promise<void>;

    /**
     * @description Sets the owner of the given file; not supported on Windows
     *
     *      Both uid and gid are required; pass -1 to keep the current value of one of them, the same
     *      convention as Node.js. Changing the owner usually requires elevated privileges.
     *      @param path the file to set
     *      @param uid the user id of the owner
     *      @param gid the group id of the owner
     *
     */
    function chown(path: string, uid: number, gid: number): void;

    function chown(path: string, uid: number, gid: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Sets the owner of the given file; not supported on Windows
     *
     *      Both uid and gid are required; pass -1 to keep the current value of one of them, the same
     *      convention as Node.js. Changing the owner usually requires elevated privileges.
     *      @param path the file to set
     *      @param uid the user id of the owner
     *      @param gid the group id of the owner
     *
     */
    function chownSync(path: string, uid: number, gid: number): void;

    /**
     * @description Sets the owner of the given file; not supported on Windows
     *
     *      Both uid and gid are required; pass -1 to keep the current value of one of them, the same
     *      convention as Node.js. Changing the owner usually requires elevated privileges.
     *      @param path the file to set
     *      @param uid the user id of the owner
     *      @param gid the group id of the owner
     *
     */
    function chownAsync(path: string, uid: number, gid: number): Promise<void>;

    /**
     * @description Sets the owner of the given file without changing the target of a symbolic link; not supported on Windows
     *
     *      Identical to chown except that when path is a symbolic link the link itself is modified;
     *      pass -1 for uid or gid to keep that value unchanged.
     *      @param path the file to set
     *      @param uid the user id of the owner
     *      @param gid the group id of the owner
     *
     */
    function lchown(path: string, uid: number, gid: number): void;

    function lchown(path: string, uid: number, gid: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Sets the owner of the given file without changing the target of a symbolic link; not supported on Windows
     *
     *      Identical to chown except that when path is a symbolic link the link itself is modified;
     *      pass -1 for uid or gid to keep that value unchanged.
     *      @param path the file to set
     *      @param uid the user id of the owner
     *      @param gid the group id of the owner
     *
     */
    function lchownSync(path: string, uid: number, gid: number): void;

    /**
     * @description Sets the owner of the given file without changing the target of a symbolic link; not supported on Windows
     *
     *      Identical to chown except that when path is a symbolic link the link itself is modified;
     *      pass -1 for uid or gid to keep that value unchanged.
     *      @param path the file to set
     *      @param uid the user id of the owner
     *      @param gid the group id of the owner
     *
     */
    function lchownAsync(path: string, uid: number, gid: number): Promise<void>;

    /**
     * @description Changes the access and modification time of the given file
     *
     *      The time arguments may be a Date object, a Unix timestamp in seconds, or a date string, consistent with Node.js.
     *      @param path the file to set
     *      @param atime the last access time: a Date object, a Unix timestamp in seconds, or a date string
     *      @param mtime the last modification time: a Date object, a Unix timestamp in seconds, or a date string
     *
     */
    function utimes(path: string, atime: any, mtime: any): void;

    function utimes(path: string, atime: any, mtime: any, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Changes the access and modification time of the given file
     *
     *      The time arguments may be a Date object, a Unix timestamp in seconds, or a date string, consistent with Node.js.
     *      @param path the file to set
     *      @param atime the last access time: a Date object, a Unix timestamp in seconds, or a date string
     *      @param mtime the last modification time: a Date object, a Unix timestamp in seconds, or a date string
     *
     */
    function utimesSync(path: string, atime: any, mtime: any): void;

    /**
     * @description Changes the access and modification time of the given file
     *
     *      The time arguments may be a Date object, a Unix timestamp in seconds, or a date string, consistent with Node.js.
     *      @param path the file to set
     *      @param atime the last access time: a Date object, a Unix timestamp in seconds, or a date string
     *      @param mtime the last modification time: a Date object, a Unix timestamp in seconds, or a date string
     *
     */
    function utimesAsync(path: string, atime: any, mtime: any): Promise<void>;

    /**
     * @description Changes the access and modification time of the symbolic link itself, without following it
     *
     *      The time arguments may be a Date object, a Unix timestamp in seconds, or a date string, consistent with Node.js.
     *      @param path the symbolic link to set
     *      @param atime the last access time: a Date object, a Unix timestamp in seconds, or a date string
     *      @param mtime the last modification time: a Date object, a Unix timestamp in seconds, or a date string
     *
     */
    function lutimes(path: string, atime: any, mtime: any): void;

    function lutimes(path: string, atime: any, mtime: any, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Changes the access and modification time of the symbolic link itself, without following it
     *
     *      The time arguments may be a Date object, a Unix timestamp in seconds, or a date string, consistent with Node.js.
     *      @param path the symbolic link to set
     *      @param atime the last access time: a Date object, a Unix timestamp in seconds, or a date string
     *      @param mtime the last modification time: a Date object, a Unix timestamp in seconds, or a date string
     *
     */
    function lutimesSync(path: string, atime: any, mtime: any): void;

    /**
     * @description Changes the access and modification time of the symbolic link itself, without following it
     *
     *      The time arguments may be a Date object, a Unix timestamp in seconds, or a date string, consistent with Node.js.
     *      @param path the symbolic link to set
     *      @param atime the last access time: a Date object, a Unix timestamp in seconds, or a date string
     *      @param mtime the last modification time: a Date object, a Unix timestamp in seconds, or a date string
     *
     */
    function lutimesAsync(path: string, atime: any, mtime: any): Promise<void>;

    /**
     * @description Queries the basic information of the given file
     *
     *      Follows symbolic links: when path is a link the returned Stat object describes its target,
     *      use lstat to describe the link itself.
     *
     *      The options overload accepts:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          // throw when the path does not exist; false returns undefined instead. Default: true
     *          "throwIfNoEntry": true
     *      })
     *      ```
     *
     *      `throwIfNoEntry` works like Node.js and only takes effect for synchronous (no callback)
     *      calls; the asynchronous form always throws.
     *
     *      Example — inspect a text file created in a temporary directory:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-stat-'));
     *      const file = path.join(dir, 'data.txt');
     *      fs.writeFile(file, 'hello');
     *
     *      const st = fs.stat(file);
     *      console.log(st.name, st.size, st.isFile()); // data.txt 5 true
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param path the file to query
     *      @return the basic information of the file, or undefined when `throwIfNoEntry` is false and
     *              the path does not exist
     *
     */
    function stat(path: string): Class_Stat;

    function stat(path: string, callback: (err: Error | undefined | null, retVal: Class_Stat)=>any): void;

    /**
     * @description Queries the basic information of the given file
     *
     *      Follows symbolic links: when path is a link the returned Stat object describes its target,
     *      use lstat to describe the link itself.
     *
     *      The options overload accepts:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          // throw when the path does not exist; false returns undefined instead. Default: true
     *          "throwIfNoEntry": true
     *      })
     *      ```
     *
     *      `throwIfNoEntry` works like Node.js and only takes effect for synchronous (no callback)
     *      calls; the asynchronous form always throws.
     *
     *      Example — inspect a text file created in a temporary directory:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-stat-'));
     *      const file = path.join(dir, 'data.txt');
     *      fs.writeFile(file, 'hello');
     *
     *      const st = fs.stat(file);
     *      console.log(st.name, st.size, st.isFile()); // data.txt 5 true
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param path the file to query
     *      @return the basic information of the file, or undefined when `throwIfNoEntry` is false and
     *              the path does not exist
     *
     */
    function statSync(path: string): Class_Stat;

    /**
     * @description Queries the basic information of the given file
     *
     *      Follows symbolic links: when path is a link the returned Stat object describes its target,
     *      use lstat to describe the link itself.
     *
     *      The options overload accepts:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          // throw when the path does not exist; false returns undefined instead. Default: true
     *          "throwIfNoEntry": true
     *      })
     *      ```
     *
     *      `throwIfNoEntry` works like Node.js and only takes effect for synchronous (no callback)
     *      calls; the asynchronous form always throws.
     *
     *      Example — inspect a text file created in a temporary directory:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-stat-'));
     *      const file = path.join(dir, 'data.txt');
     *      fs.writeFile(file, 'hello');
     *
     *      const st = fs.stat(file);
     *      console.log(st.name, st.size, st.isFile()); // data.txt 5 true
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param path the file to query
     *      @return the basic information of the file, or undefined when `throwIfNoEntry` is false and
     *              the path does not exist
     *
     */
    function statAsync(path: string): Promise<Class_Stat>;

    /**
     * @description Queries the basic information of the given file
     *
     *      The options object (throwIfNoEntry, default true) is described on the first overload; it
     *      only takes effect for synchronous (no callback) calls, the asynchronous form always throws.
     *      @param path the file to query
     *      @param options the query options
     *      @return the basic information of the file, or undefined when `throwIfNoEntry` is false and the path does not exist
     *
     */
    function stat(path: string, options: FIBJS.GeneralObject): Class_Stat;

    function stat(path: string, options: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_Stat)=>any): void;

    /**
     * @description Queries the basic information of the given file
     *
     *      The options object (throwIfNoEntry, default true) is described on the first overload; it
     *      only takes effect for synchronous (no callback) calls, the asynchronous form always throws.
     *      @param path the file to query
     *      @param options the query options
     *      @return the basic information of the file, or undefined when `throwIfNoEntry` is false and the path does not exist
     *
     */
    function statSync(path: string, options: FIBJS.GeneralObject): Class_Stat;

    /**
     * @description Queries the basic information of the given file
     *
     *      The options object (throwIfNoEntry, default true) is described on the first overload; it
     *      only takes effect for synchronous (no callback) calls, the asynchronous form always throws.
     *      @param path the file to query
     *      @param options the query options
     *      @return the basic information of the file, or undefined when `throwIfNoEntry` is false and the path does not exist
     *
     */
    function statAsync(path: string, options: FIBJS.GeneralObject): Promise<Class_Stat>;

    /**
     * @description Queries the basic information of the given file; unlike stat, when path is a symbolic link, the information of the link itself is returned instead of its target
     *
     *      The described entry is the link itself: isSymbolicLink() returns true and
     *      isFile()/isDirectory() report the link, not its target.
     *
     *      The options overload accepts:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          // throw when the path does not exist; false returns undefined instead. Default: true
     *          "throwIfNoEntry": true
     *      })
     *      ```
     *
     *      `throwIfNoEntry` works like Node.js and only takes effect for synchronous (no callback)
     *      calls; the asynchronous form always throws.
     *      @param path the file to query
     *      @return the basic information of the file, or undefined when `throwIfNoEntry` is false and
     *              the path does not exist
     *
     */
    function lstat(path: string): Class_Stat;

    function lstat(path: string, callback: (err: Error | undefined | null, retVal: Class_Stat)=>any): void;

    /**
     * @description Queries the basic information of the given file; unlike stat, when path is a symbolic link, the information of the link itself is returned instead of its target
     *
     *      The described entry is the link itself: isSymbolicLink() returns true and
     *      isFile()/isDirectory() report the link, not its target.
     *
     *      The options overload accepts:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          // throw when the path does not exist; false returns undefined instead. Default: true
     *          "throwIfNoEntry": true
     *      })
     *      ```
     *
     *      `throwIfNoEntry` works like Node.js and only takes effect for synchronous (no callback)
     *      calls; the asynchronous form always throws.
     *      @param path the file to query
     *      @return the basic information of the file, or undefined when `throwIfNoEntry` is false and
     *              the path does not exist
     *
     */
    function lstatSync(path: string): Class_Stat;

    /**
     * @description Queries the basic information of the given file; unlike stat, when path is a symbolic link, the information of the link itself is returned instead of its target
     *
     *      The described entry is the link itself: isSymbolicLink() returns true and
     *      isFile()/isDirectory() report the link, not its target.
     *
     *      The options overload accepts:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          // throw when the path does not exist; false returns undefined instead. Default: true
     *          "throwIfNoEntry": true
     *      })
     *      ```
     *
     *      `throwIfNoEntry` works like Node.js and only takes effect for synchronous (no callback)
     *      calls; the asynchronous form always throws.
     *      @param path the file to query
     *      @return the basic information of the file, or undefined when `throwIfNoEntry` is false and
     *              the path does not exist
     *
     */
    function lstatAsync(path: string): Promise<Class_Stat>;

    /**
     * @description Queries the basic information of the given file; unlike stat, when path is a symbolic link, the information of the link itself is returned instead of its target
     *
     *      The options object (throwIfNoEntry, default true) is described on the first overload; it
     *      only takes effect for synchronous (no callback) calls, the asynchronous form always throws.
     *      @param path the file to query
     *      @param options the query options
     *      @return the basic information of the file, or undefined when `throwIfNoEntry` is false and the path does not exist
     *
     */
    function lstat(path: string, options: FIBJS.GeneralObject): Class_Stat;

    function lstat(path: string, options: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_Stat)=>any): void;

    /**
     * @description Queries the basic information of the given file; unlike stat, when path is a symbolic link, the information of the link itself is returned instead of its target
     *
     *      The options object (throwIfNoEntry, default true) is described on the first overload; it
     *      only takes effect for synchronous (no callback) calls, the asynchronous form always throws.
     *      @param path the file to query
     *      @param options the query options
     *      @return the basic information of the file, or undefined when `throwIfNoEntry` is false and the path does not exist
     *
     */
    function lstatSync(path: string, options: FIBJS.GeneralObject): Class_Stat;

    /**
     * @description Queries the basic information of the given file; unlike stat, when path is a symbolic link, the information of the link itself is returned instead of its target
     *
     *      The options object (throwIfNoEntry, default true) is described on the first overload; it
     *      only takes effect for synchronous (no callback) calls, the asynchronous form always throws.
     *      @param path the file to query
     *      @param options the query options
     *      @return the basic information of the file, or undefined when `throwIfNoEntry` is false and the path does not exist
     *
     */
    function lstatAsync(path: string, options: FIBJS.GeneralObject): Promise<Class_Stat>;

    /**
     * @description Queries the basic information of the given file
     *
     *      fd may be an integer descriptor or a FileHandle object; both address the same open file.
     *      The options overload accepts an object for Node.js compatibility; no option is effective yet.
     *      @param fd the file descriptor
     *      @return the basic information of the file
     *
     */
    function fstat(fd: number | Class_FileHandle | Class_FileHandlePromise): Class_Stat;

    function fstat(fd: number | Class_FileHandle | Class_FileHandlePromise, callback: (err: Error | undefined | null, retVal: Class_Stat)=>any): void;

    /**
     * @description Queries the basic information of the given file
     *
     *      fd may be an integer descriptor or a FileHandle object; both address the same open file.
     *      The options overload accepts an object for Node.js compatibility; no option is effective yet.
     *      @param fd the file descriptor
     *      @return the basic information of the file
     *
     */
    function fstatSync(fd: number | Class_FileHandle | Class_FileHandlePromise): Class_Stat;

    /**
     * @description Queries the basic information of the given file
     *
     *      fd may be an integer descriptor or a FileHandle object; both address the same open file.
     *      The options overload accepts an object for Node.js compatibility; no option is effective yet.
     *      @param fd the file descriptor
     *      @return the basic information of the file
     *
     */
    function fstatAsync(fd: number | Class_FileHandle | Class_FileHandlePromise): Promise<Class_Stat>;

    /**
     * @description Queries the basic information of the given file
     *
     *      options currently has no effective option and is kept for Node.js compatibility only.
     *
     *      fd may be an integer descriptor or a FileHandle object; both address the same open file.
     *      @param fd the file descriptor
     *      @param options the query options
     *      @return the basic information of the file
     *
     */
    function fstat(fd: number | Class_FileHandle | Class_FileHandlePromise, options: FIBJS.GeneralObject): Class_Stat;

    function fstat(fd: number | Class_FileHandle | Class_FileHandlePromise, options: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_Stat)=>any): void;

    /**
     * @description Queries the basic information of the given file
     *
     *      options currently has no effective option and is kept for Node.js compatibility only.
     *
     *      fd may be an integer descriptor or a FileHandle object; both address the same open file.
     *      @param fd the file descriptor
     *      @param options the query options
     *      @return the basic information of the file
     *
     */
    function fstatSync(fd: number | Class_FileHandle | Class_FileHandlePromise, options: FIBJS.GeneralObject): Class_Stat;

    /**
     * @description Queries the basic information of the given file
     *
     *      options currently has no effective option and is kept for Node.js compatibility only.
     *
     *      fd may be an integer descriptor or a FileHandle object; both address the same open file.
     *      @param fd the file descriptor
     *      @param options the query options
     *      @return the basic information of the file
     *
     */
    function fstatAsync(fd: number | Class_FileHandle | Class_FileHandlePromise, options: FIBJS.GeneralObject): Promise<Class_Stat>;

    /**
     * @description Reads the given symbolic link and returns the target path it points to; not supported on Windows
     *
     *      The target is returned as stored in the link and may be relative or point to a nonexistent
     *      path; the link itself must exist.
     *
     *      The options overload accepts either an encoding string or:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          // the returned value encoding; 'buffer' returns a Buffer. Default: utf8
     *          "encoding": "utf8"
     *      })
     *      ```
     *      @param path the symbolic link to read
     *      @return the file name the symbolic link points to
     *
     */
    function readlink(path: string): any;

    function readlink(path: string, callback: (err: Error | undefined | null, retVal: any)=>any): void;

    /**
     * @description Reads the given symbolic link and returns the target path it points to; not supported on Windows
     *
     *      The target is returned as stored in the link and may be relative or point to a nonexistent
     *      path; the link itself must exist.
     *
     *      The options overload accepts either an encoding string or:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          // the returned value encoding; 'buffer' returns a Buffer. Default: utf8
     *          "encoding": "utf8"
     *      })
     *      ```
     *      @param path the symbolic link to read
     *      @return the file name the symbolic link points to
     *
     */
    function readlinkSync(path: string): any;

    /**
     * @description Reads the given symbolic link and returns the target path it points to; not supported on Windows
     *
     *      The target is returned as stored in the link and may be relative or point to a nonexistent
     *      path; the link itself must exist.
     *
     *      The options overload accepts either an encoding string or:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          // the returned value encoding; 'buffer' returns a Buffer. Default: utf8
     *          "encoding": "utf8"
     *      })
     *      ```
     *      @param path the symbolic link to read
     *      @return the file name the symbolic link points to
     *
     */
    function readlinkAsync(path: string): Promise<any>;

    /**
     * @description Reads the given symbolic link and returns the target path it points to; not supported on Windows
     *
     *      The encoding is described on the first overload; it may be passed as a string or in an
     *      options object, and 'buffer' returns a Buffer.
     *      @param path the symbolic link to read
     *      @param options the read options or the encoding of the returned value
     *      @return the decoded string when an encoding is given, or a Buffer for 'buffer'
     *
     */
    function readlink(path: string, options: FIBJS.GeneralObject | string): any;

    function readlink(path: string, options: FIBJS.GeneralObject | string, callback: (err: Error | undefined | null, retVal: any)=>any): void;

    /**
     * @description Reads the given symbolic link and returns the target path it points to; not supported on Windows
     *
     *      The encoding is described on the first overload; it may be passed as a string or in an
     *      options object, and 'buffer' returns a Buffer.
     *      @param path the symbolic link to read
     *      @param options the read options or the encoding of the returned value
     *      @return the decoded string when an encoding is given, or a Buffer for 'buffer'
     *
     */
    function readlinkSync(path: string, options: FIBJS.GeneralObject | string): any;

    /**
     * @description Reads the given symbolic link and returns the target path it points to; not supported on Windows
     *
     *      The encoding is described on the first overload; it may be passed as a string or in an
     *      options object, and 'buffer' returns a Buffer.
     *      @param path the symbolic link to read
     *      @param options the read options or the encoding of the returned value
     *      @return the decoded string when an encoding is given, or a Buffer for 'buffer'
     *
     */
    function readlinkAsync(path: string, options: FIBJS.GeneralObject | string): Promise<any>;

    /**
     * @description Returns the absolute path of the given path, unfolding relative segments and resolving symbolic links
     *
     *      Unfolds `.` and `..` segments and resolves every symbolic link, like Node.js; throws ENOENT
     *      when the path does not exist.
     *
     *      The options overload accepts either an encoding string or:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          // the returned value encoding; 'buffer' returns a Buffer. Default: utf8
     *          "encoding": "utf8"
     *      })
     *      ```
     *      @param path the path to read
     *      @return the resolved absolute path
     *
     */
    function realpath(path: string): any;

    function realpath(path: string, callback: (err: Error | undefined | null, retVal: any)=>any): void;

    /**
     * @description Returns the absolute path of the given path, unfolding relative segments and resolving symbolic links
     *
     *      Unfolds `.` and `..` segments and resolves every symbolic link, like Node.js; throws ENOENT
     *      when the path does not exist.
     *
     *      The options overload accepts either an encoding string or:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          // the returned value encoding; 'buffer' returns a Buffer. Default: utf8
     *          "encoding": "utf8"
     *      })
     *      ```
     *      @param path the path to read
     *      @return the resolved absolute path
     *
     */
    function realpathSync(path: string): any;

    /**
     * @description Returns the absolute path of the given path, unfolding relative segments and resolving symbolic links
     *
     *      Unfolds `.` and `..` segments and resolves every symbolic link, like Node.js; throws ENOENT
     *      when the path does not exist.
     *
     *      The options overload accepts either an encoding string or:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          // the returned value encoding; 'buffer' returns a Buffer. Default: utf8
     *          "encoding": "utf8"
     *      })
     *      ```
     *      @param path the path to read
     *      @return the resolved absolute path
     *
     */
    function realpathAsync(path: string): Promise<any>;

    /**
     * @description Returns the absolute path of the given path, unfolding relative segments and resolving symbolic links
     *
     *      The encoding is described on the first overload; it may be passed as a string or in an
     *      options object, and 'buffer' returns a Buffer.
     *      @param path the path to read
     *      @param options the read options or the encoding of the returned value
     *      @return the decoded string when an encoding is given, or a Buffer for 'buffer'
     *
     */
    function realpath(path: string, options: FIBJS.GeneralObject | string): any;

    function realpath(path: string, options: FIBJS.GeneralObject | string, callback: (err: Error | undefined | null, retVal: any)=>any): void;

    /**
     * @description Returns the absolute path of the given path, unfolding relative segments and resolving symbolic links
     *
     *      The encoding is described on the first overload; it may be passed as a string or in an
     *      options object, and 'buffer' returns a Buffer.
     *      @param path the path to read
     *      @param options the read options or the encoding of the returned value
     *      @return the decoded string when an encoding is given, or a Buffer for 'buffer'
     *
     */
    function realpathSync(path: string, options: FIBJS.GeneralObject | string): any;

    /**
     * @description Returns the absolute path of the given path, unfolding relative segments and resolving symbolic links
     *
     *      The encoding is described on the first overload; it may be passed as a string or in an
     *      options object, and 'buffer' returns a Buffer.
     *      @param path the path to read
     *      @param options the read options or the encoding of the returned value
     *      @return the decoded string when an encoding is given, or a Buffer for 'buffer'
     *
     */
    function realpathAsync(path: string, options: FIBJS.GeneralObject | string): Promise<any>;

    /**
     * @description Creates a symbolic link
     *
     *      On POSIX systems the link stores target as given and type is ignored; a relative target is
     *      interpreted relative to the directory of linkpath. On Windows type selects 'file', 'dir' or
     *      'junction', and a junction target must be absolute. Throws EEXIST when linkpath exists.
     *      @param target the target, which may be a file, a directory or a nonexistent path
     *      @param linkpath the symbolic link to create
     *      @param type the type of the symbolic link: 'file', 'dir' or 'junction', 'file' by default; this parameter is only effective on Windows, and for 'junction' the target path linkpath must be absolute, while target is converted to an absolute path automatically.
     *
     */
    function symlink(target: string, linkpath: string, type?: string): void;

    function symlink(target: string, linkpath: string, type?: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Creates a symbolic link
     *
     *      On POSIX systems the link stores target as given and type is ignored; a relative target is
     *      interpreted relative to the directory of linkpath. On Windows type selects 'file', 'dir' or
     *      'junction', and a junction target must be absolute. Throws EEXIST when linkpath exists.
     *      @param target the target, which may be a file, a directory or a nonexistent path
     *      @param linkpath the symbolic link to create
     *      @param type the type of the symbolic link: 'file', 'dir' or 'junction', 'file' by default; this parameter is only effective on Windows, and for 'junction' the target path linkpath must be absolute, while target is converted to an absolute path automatically.
     *
     */
    function symlinkSync(target: string, linkpath: string, type?: string): void;

    /**
     * @description Creates a symbolic link
     *
     *      On POSIX systems the link stores target as given and type is ignored; a relative target is
     *      interpreted relative to the directory of linkpath. On Windows type selects 'file', 'dir' or
     *      'junction', and a junction target must be absolute. Throws EEXIST when linkpath exists.
     *      @param target the target, which may be a file, a directory or a nonexistent path
     *      @param linkpath the symbolic link to create
     *      @param type the type of the symbolic link: 'file', 'dir' or 'junction', 'file' by default; this parameter is only effective on Windows, and for 'junction' the target path linkpath must be absolute, while target is converted to an absolute path automatically.
     *
     */
    function symlinkAsync(target: string, linkpath: string, type?: string): Promise<void>;

    /**
     * @description Changes the size of a file; when the given length is larger than the source file, it is padded with '\0', otherwise the exceeding content is lost
     *
     *      The file must exist and be writable; this is the path-based counterpart of ftruncate.
     *      @param path the path of the file to change
     *      @param len the new size of the file
     *
     */
    function truncate(path: string, len: number): void;

    function truncate(path: string, len: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Changes the size of a file; when the given length is larger than the source file, it is padded with '\0', otherwise the exceeding content is lost
     *
     *      The file must exist and be writable; this is the path-based counterpart of ftruncate.
     *      @param path the path of the file to change
     *      @param len the new size of the file
     *
     */
    function truncateSync(path: string, len: number): void;

    /**
     * @description Changes the size of a file; when the given length is larger than the source file, it is padded with '\0', otherwise the exceeding content is lost
     *
     *      The file must exist and be writable; this is the path-based counterpart of ftruncate.
     *      @param path the path of the file to change
     *      @param len the new size of the file
     *
     */
    function truncateAsync(path: string, len: number): Promise<void>;

    /**
     * @description Reads the content of a file by its file descriptor
     *
     *      length defaults to 0, which reads no data; a length must be given explicitly to read. position defaults to -1, which reads from the current file position; when position is given, the file pointer is moved there before reading.
     *
     *      fd may be an integer descriptor or a FileHandle object; position addresses that descriptor's file position.
     *      @param fd the file descriptor
     *      @param buffer the Buffer the result is written into
     *      @param offset the write offset in the Buffer, 0 by default
     *      @param length the number of bytes to read, 0 by default
     *      @param position the read position, the current file position by default
     *      @return the number of bytes actually read
     *
     */
    function read(fd: number | Class_FileHandle | Class_FileHandlePromise, buffer: Class_Buffer, offset?: number, length?: number, position?: number): number;

    function read(fd: number | Class_FileHandle | Class_FileHandlePromise, buffer: Class_Buffer, offset?: number, length?: number, position?: number, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Reads the content of a file by its file descriptor
     *
     *      length defaults to 0, which reads no data; a length must be given explicitly to read. position defaults to -1, which reads from the current file position; when position is given, the file pointer is moved there before reading.
     *
     *      fd may be an integer descriptor or a FileHandle object; position addresses that descriptor's file position.
     *      @param fd the file descriptor
     *      @param buffer the Buffer the result is written into
     *      @param offset the write offset in the Buffer, 0 by default
     *      @param length the number of bytes to read, 0 by default
     *      @param position the read position, the current file position by default
     *      @return the number of bytes actually read
     *
     */
    function readSync(fd: number | Class_FileHandle | Class_FileHandlePromise, buffer: Class_Buffer, offset?: number, length?: number, position?: number): number;

    /**
     * @description Reads the content of a file by its file descriptor
     *
     *      length defaults to 0, which reads no data; a length must be given explicitly to read. position defaults to -1, which reads from the current file position; when position is given, the file pointer is moved there before reading.
     *
     *      fd may be an integer descriptor or a FileHandle object; position addresses that descriptor's file position.
     *      @param fd the file descriptor
     *      @param buffer the Buffer the result is written into
     *      @param offset the write offset in the Buffer, 0 by default
     *      @param length the number of bytes to read, 0 by default
     *      @param position the read position, the current file position by default
     *      @return the number of bytes actually read
     *
     */
    function readAsync(fd: number | Class_FileHandle | Class_FileHandlePromise, buffer: Class_Buffer, offset?: number, length?: number, position?: number): Promise<number>;

    /**
     * @description Changes the file mode by its file descriptor. Effective on POSIX systems only.
     *
     *      fd may be an integer descriptor or a FileHandle object; the mode is applied to the file it addresses.
     *      @param fd the file descriptor
     *      @param mode the file mode
     *
     */
    function fchmod(fd: number | Class_FileHandle | Class_FileHandlePromise, mode: number): void;

    function fchmod(fd: number | Class_FileHandle | Class_FileHandlePromise, mode: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Changes the file mode by its file descriptor. Effective on POSIX systems only.
     *
     *      fd may be an integer descriptor or a FileHandle object; the mode is applied to the file it addresses.
     *      @param fd the file descriptor
     *      @param mode the file mode
     *
     */
    function fchmodSync(fd: number | Class_FileHandle | Class_FileHandlePromise, mode: number): void;

    /**
     * @description Changes the file mode by its file descriptor. Effective on POSIX systems only.
     *
     *      fd may be an integer descriptor or a FileHandle object; the mode is applied to the file it addresses.
     *      @param fd the file descriptor
     *      @param mode the file mode
     *
     */
    function fchmodAsync(fd: number | Class_FileHandle | Class_FileHandlePromise, mode: number): Promise<void>;

    /**
     * @description Changes the owner by the file descriptor. Effective on POSIX systems only.
     *
     *      fd may be an integer descriptor or a FileHandle object; the owner is changed on the file it addresses.
     *      @param fd the file descriptor
     *      @param uid the user id
     *      @param gid the group id
     *
     */
    function fchown(fd: number | Class_FileHandle | Class_FileHandlePromise, uid: number, gid: number): void;

    function fchown(fd: number | Class_FileHandle | Class_FileHandlePromise, uid: number, gid: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Changes the owner by the file descriptor. Effective on POSIX systems only.
     *
     *      fd may be an integer descriptor or a FileHandle object; the owner is changed on the file it addresses.
     *      @param fd the file descriptor
     *      @param uid the user id
     *      @param gid the group id
     *
     */
    function fchownSync(fd: number | Class_FileHandle | Class_FileHandlePromise, uid: number, gid: number): void;

    /**
     * @description Changes the owner by the file descriptor. Effective on POSIX systems only.
     *
     *      fd may be an integer descriptor or a FileHandle object; the owner is changed on the file it addresses.
     *      @param fd the file descriptor
     *      @param uid the user id
     *      @param gid the group id
     *
     */
    function fchownAsync(fd: number | Class_FileHandle | Class_FileHandlePromise, uid: number, gid: number): Promise<void>;

    /**
     * @description Changes the access and modification time of a file by its file descriptor
     *
     *      The time arguments may be a Date object, a Unix timestamp in seconds, or a date string, consistent with Node.js.
     *
     *      fd may be an integer descriptor or a FileHandle object; the times are applied to the file it addresses.
     *      @param fd the file descriptor
     *      @param atime the last access time: a Date object, a Unix timestamp in seconds, or a date string
     *      @param mtime the last modification time: a Date object, a Unix timestamp in seconds, or a date string
     *
     */
    function futimes(fd: number | Class_FileHandle | Class_FileHandlePromise, atime: any, mtime: any): void;

    function futimes(fd: number | Class_FileHandle | Class_FileHandlePromise, atime: any, mtime: any, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Changes the access and modification time of a file by its file descriptor
     *
     *      The time arguments may be a Date object, a Unix timestamp in seconds, or a date string, consistent with Node.js.
     *
     *      fd may be an integer descriptor or a FileHandle object; the times are applied to the file it addresses.
     *      @param fd the file descriptor
     *      @param atime the last access time: a Date object, a Unix timestamp in seconds, or a date string
     *      @param mtime the last modification time: a Date object, a Unix timestamp in seconds, or a date string
     *
     */
    function futimesSync(fd: number | Class_FileHandle | Class_FileHandlePromise, atime: any, mtime: any): void;

    /**
     * @description Changes the access and modification time of a file by its file descriptor
     *
     *      The time arguments may be a Date object, a Unix timestamp in seconds, or a date string, consistent with Node.js.
     *
     *      fd may be an integer descriptor or a FileHandle object; the times are applied to the file it addresses.
     *      @param fd the file descriptor
     *      @param atime the last access time: a Date object, a Unix timestamp in seconds, or a date string
     *      @param mtime the last modification time: a Date object, a Unix timestamp in seconds, or a date string
     *
     */
    function futimesAsync(fd: number | Class_FileHandle | Class_FileHandlePromise, atime: any, mtime: any): Promise<void>;

    /**
     * @description Synchronizes data to disk by the file descriptor
     *
     *      Only the file data is synchronized, not the metadata, which costs less than fsync.
     *
     *      fd may be an integer descriptor or a FileHandle object; the data of that descriptor is flushed.
     *      @param fd the file descriptor
     *
     */
    function fdatasync(fd: number | Class_FileHandle | Class_FileHandlePromise): void;

    function fdatasync(fd: number | Class_FileHandle | Class_FileHandlePromise, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Synchronizes data to disk by the file descriptor
     *
     *      Only the file data is synchronized, not the metadata, which costs less than fsync.
     *
     *      fd may be an integer descriptor or a FileHandle object; the data of that descriptor is flushed.
     *      @param fd the file descriptor
     *
     */
    function fdatasyncSync(fd: number | Class_FileHandle | Class_FileHandlePromise): void;

    /**
     * @description Synchronizes data to disk by the file descriptor
     *
     *      Only the file data is synchronized, not the metadata, which costs less than fsync.
     *
     *      fd may be an integer descriptor or a FileHandle object; the data of that descriptor is flushed.
     *      @param fd the file descriptor
     *
     */
    function fdatasyncAsync(fd: number | Class_FileHandle | Class_FileHandlePromise): Promise<void>;

    /**
     * @description Synchronizes data to disk by the file descriptor
     *
     *      Synchronizes both the file data and the metadata, making sure the written content is persisted.
     *
     *      fd may be an integer descriptor or a FileHandle object; the data and metadata of that descriptor are flushed.
     *      @param fd the file descriptor
     *
     */
    function fsync(fd: number | Class_FileHandle | Class_FileHandlePromise): void;

    function fsync(fd: number | Class_FileHandle | Class_FileHandlePromise, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Synchronizes data to disk by the file descriptor
     *
     *      Synchronizes both the file data and the metadata, making sure the written content is persisted.
     *
     *      fd may be an integer descriptor or a FileHandle object; the data and metadata of that descriptor are flushed.
     *      @param fd the file descriptor
     *
     */
    function fsyncSync(fd: number | Class_FileHandle | Class_FileHandlePromise): void;

    /**
     * @description Synchronizes data to disk by the file descriptor
     *
     *      Synchronizes both the file data and the metadata, making sure the written content is persisted.
     *
     *      fd may be an integer descriptor or a FileHandle object; the data and metadata of that descriptor are flushed.
     *      @param fd the file descriptor
     *
     */
    function fsyncAsync(fd: number | Class_FileHandle | Class_FileHandlePromise): Promise<void>;

    /**
     * @description Changes the size of a file by its file descriptor
     *
     *      Consistent with Node.js: a length of 0 empties the file, and negative values are treated as 0.
     *
     *      fd may be an integer descriptor or a FileHandle object; the file it addresses is resized.
     *      @param fd the file descriptor
     *      @param len the new size of the file, 0 by default
     *
     */
    function ftruncate(fd: number | Class_FileHandle | Class_FileHandlePromise, len?: number): void;

    function ftruncate(fd: number | Class_FileHandle | Class_FileHandlePromise, len?: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Changes the size of a file by its file descriptor
     *
     *      Consistent with Node.js: a length of 0 empties the file, and negative values are treated as 0.
     *
     *      fd may be an integer descriptor or a FileHandle object; the file it addresses is resized.
     *      @param fd the file descriptor
     *      @param len the new size of the file, 0 by default
     *
     */
    function ftruncateSync(fd: number | Class_FileHandle | Class_FileHandlePromise, len?: number): void;

    /**
     * @description Changes the size of a file by its file descriptor
     *
     *      Consistent with Node.js: a length of 0 empties the file, and negative values are treated as 0.
     *
     *      fd may be an integer descriptor or a FileHandle object; the file it addresses is resized.
     *      @param fd the file descriptor
     *      @param len the new size of the file, 0 by default
     *
     */
    function ftruncateAsync(fd: number | Class_FileHandle | Class_FileHandlePromise, len?: number): Promise<void>;

    /**
     * @description Queries the information of the file system
     *
     *      The returned object contains the type, bsize, blocks, bfree, bavail, files and ffree fields, consistent with Node.js.
     *      @param path the path to query
     *      @return the file system information object
     *
     */
    function statfs(path: string): {
        type: number;
        bsize: number;
        blocks: number;
        bfree: number;
        bavail: number;
        files: number;
        ffree: number;
    };

    function statfs(path: string, callback: (err: Error | undefined | null, retVal: {
        type: number;
        bsize: number;
        blocks: number;
        bfree: number;
        bavail: number;
        files: number;
        ffree: number;
    })=>any): void;

    /**
     * @description Queries the information of the file system
     *
     *      The returned object contains the type, bsize, blocks, bfree, bavail, files and ffree fields, consistent with Node.js.
     *      @param path the path to query
     *      @return the file system information object
     *
     */
    function statfsSync(path: string): {
        type: number;
        bsize: number;
        blocks: number;
        bfree: number;
        bavail: number;
        files: number;
        ffree: number;
    };

    /**
     * @description Queries the information of the file system
     *
     *      The returned object contains the type, bsize, blocks, bfree, bavail, files and ffree fields, consistent with Node.js.
     *      @param path the path to query
     *      @return the file system information object
     *
     */
    function statfsAsync(path: string): Promise<{
        type: number;
        bsize: number;
        blocks: number;
        bfree: number;
        bavail: number;
        files: number;
        ffree: number;
    }>;

    /**
     * @description Reads the entries of the given directory
     *
     *      Returns an array of file names under the directory, without the content of subdirectories.
     *      The overload with opts can list subdirectories recursively and return DirEntry objects.
     *      @param path the directory to query
     *      @return the array of directory entries
     *
     */
    function readdir(path: string): any[];

    function readdir(path: string, callback: (err: Error | undefined | null, retVal: any[])=>any): void;

    /**
     * @description Reads the entries of the given directory
     *
     *      Returns an array of file names under the directory, without the content of subdirectories.
     *      The overload with opts can list subdirectories recursively and return DirEntry objects.
     *      @param path the directory to query
     *      @return the array of directory entries
     *
     */
    function readdirSync(path: string): any[];

    /**
     * @description Reads the entries of the given directory
     *
     *      Returns an array of file names under the directory, without the content of subdirectories.
     *      The overload with opts can list subdirectories recursively and return DirEntry objects.
     *      @param path the directory to query
     *      @return the array of directory entries
     *
     */
    function readdirAsync(path: string): Promise<any[]>;

    /**
     * @description Opens a directory for iteration
     *
     *      Returns a Dir object; entries can be read one by one with read/readSync, or iterated with for await...of.
     *      @param path the directory to iterate
     *      @return the directory iteration object
     *
     */
    function opendir(path: string): Class_Dir;

    function opendir(path: string, callback: (err: Error | undefined | null, retVal: Class_Dir)=>any): void;

    /**
     * @description Opens a directory for iteration
     *
     *      Returns a Dir object; entries can be read one by one with read/readSync, or iterated with for await...of.
     *      @param path the directory to iterate
     *      @return the directory iteration object
     *
     */
    function opendirSync(path: string): Class_Dir;

    /**
     * @description Opens a directory for iteration
     *
     *      Returns a Dir object; entries can be read one by one with read/readSync, or iterated with for await...of.
     *      @param path the directory to iterate
     *      @return the directory iteration object
     *
     */
    function opendirAsync(path: string): Promise<Class_DirPromise>;

    /**
     * @description Reads the entries of the given directory
     *
     *      The opts parameter supports the following options, or a string is used as the encoding of the file names directly:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "recursive": false, // whether the content of subdirectories is listed too. Default: false
     *          "withFileTypes": false, // specify whether to return DirEntry objects. Default: false
     *          // the encoding of the file names; 'buffer' returns Buffer objects. Default: utf8
     *          "encoding": "utf8"
     *      })
     *      ```
     *
     *      When withFileTypes is true an array of DirEntry objects is returned, otherwise an array of
     *      file names. A string encoding is equivalent to passing it in the options; 'buffer' returns
     *      an array of Buffer objects, consistent with Node.js, but it cannot be combined with
     *      withFileTypes (an error is thrown, while Node.js returns Dirent objects with Buffer names).
     *      With recursive set to true the entries of subdirectories are included as paths relative to
     *      the queried directory.
     *
     *      Example — list a directory with names and with types:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-readdir-'));
     *      fs.writeFile(path.join(dir, 'a.txt'), 'a');
     *      fs.mkdir(path.join(dir, 'sub'));
     *
     *      fs.readdir(dir).forEach((name) => console.log(name));
     *      fs.readdir(dir, { withFileTypes: true }).forEach((entry) => {
     *          console.log(entry.name, entry.isDirectory());
     *      });
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param path the directory to query
     *      @param opts the options or the encoding of the returned file names
     *      @return the array of directory entries
     *
     */
    function readdir(path: string, opts?: FIBJS.GeneralObject | string): any[];

    function readdir(path: string, opts?: FIBJS.GeneralObject | string, callback: (err: Error | undefined | null, retVal: any[])=>any): void;

    /**
     * @description Reads the entries of the given directory
     *
     *      The opts parameter supports the following options, or a string is used as the encoding of the file names directly:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "recursive": false, // whether the content of subdirectories is listed too. Default: false
     *          "withFileTypes": false, // specify whether to return DirEntry objects. Default: false
     *          // the encoding of the file names; 'buffer' returns Buffer objects. Default: utf8
     *          "encoding": "utf8"
     *      })
     *      ```
     *
     *      When withFileTypes is true an array of DirEntry objects is returned, otherwise an array of
     *      file names. A string encoding is equivalent to passing it in the options; 'buffer' returns
     *      an array of Buffer objects, consistent with Node.js, but it cannot be combined with
     *      withFileTypes (an error is thrown, while Node.js returns Dirent objects with Buffer names).
     *      With recursive set to true the entries of subdirectories are included as paths relative to
     *      the queried directory.
     *
     *      Example — list a directory with names and with types:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-readdir-'));
     *      fs.writeFile(path.join(dir, 'a.txt'), 'a');
     *      fs.mkdir(path.join(dir, 'sub'));
     *
     *      fs.readdir(dir).forEach((name) => console.log(name));
     *      fs.readdir(dir, { withFileTypes: true }).forEach((entry) => {
     *          console.log(entry.name, entry.isDirectory());
     *      });
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param path the directory to query
     *      @param opts the options or the encoding of the returned file names
     *      @return the array of directory entries
     *
     */
    function readdirSync(path: string, opts?: FIBJS.GeneralObject | string): any[];

    /**
     * @description Reads the entries of the given directory
     *
     *      The opts parameter supports the following options, or a string is used as the encoding of the file names directly:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "recursive": false, // whether the content of subdirectories is listed too. Default: false
     *          "withFileTypes": false, // specify whether to return DirEntry objects. Default: false
     *          // the encoding of the file names; 'buffer' returns Buffer objects. Default: utf8
     *          "encoding": "utf8"
     *      })
     *      ```
     *
     *      When withFileTypes is true an array of DirEntry objects is returned, otherwise an array of
     *      file names. A string encoding is equivalent to passing it in the options; 'buffer' returns
     *      an array of Buffer objects, consistent with Node.js, but it cannot be combined with
     *      withFileTypes (an error is thrown, while Node.js returns Dirent objects with Buffer names).
     *      With recursive set to true the entries of subdirectories are included as paths relative to
     *      the queried directory.
     *
     *      Example — list a directory with names and with types:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-readdir-'));
     *      fs.writeFile(path.join(dir, 'a.txt'), 'a');
     *      fs.mkdir(path.join(dir, 'sub'));
     *
     *      fs.readdir(dir).forEach((name) => console.log(name));
     *      fs.readdir(dir, { withFileTypes: true }).forEach((entry) => {
     *          console.log(entry.name, entry.isDirectory());
     *      });
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param path the directory to query
     *      @param opts the options or the encoding of the returned file names
     *      @return the array of directory entries
     *
     */
    function readdirAsync(path: string, opts?: FIBJS.GeneralObject | string): Promise<any[]>;

    /**
     * @description Searches the given directory for files matching a name pattern
     *
     *      The opts parameter supports the following options:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "cwd": "", // specify a different working directory, default to current directory
     *          "withFileTypes": false // specify whether to return Dirent objects. Default: false
     *      })
     *      ```
     *
     *      The pattern supports the `*`, `?`, `**` and other wildcards; the absolute paths of the matching files are returned. When `cwd` is given, the matches of a relative pattern are returned relative to that directory.
     *      @param pattern the file name pattern
     *      @param opts the options
     *      @return the file list
     *
     */
    function glob(pattern: string, opts?: FIBJS.GeneralObject): any[];

    function glob(pattern: string, opts?: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: any[])=>any): void;

    /**
     * @description Searches the given directory for files matching a name pattern
     *
     *      The opts parameter supports the following options:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "cwd": "", // specify a different working directory, default to current directory
     *          "withFileTypes": false // specify whether to return Dirent objects. Default: false
     *      })
     *      ```
     *
     *      The pattern supports the `*`, `?`, `**` and other wildcards; the absolute paths of the matching files are returned. When `cwd` is given, the matches of a relative pattern are returned relative to that directory.
     *      @param pattern the file name pattern
     *      @param opts the options
     *      @return the file list
     *
     */
    function globSync(pattern: string, opts?: FIBJS.GeneralObject): any[];

    /**
     * @description Searches the given directory for files matching a name pattern
     *
     *      The opts parameter supports the following options:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "cwd": "", // specify a different working directory, default to current directory
     *          "withFileTypes": false // specify whether to return Dirent objects. Default: false
     *      })
     *      ```
     *
     *      The pattern supports the `*`, `?`, `**` and other wildcards; the absolute paths of the matching files are returned. When `cwd` is given, the matches of a relative pattern are returned relative to that directory.
     *      @param pattern the file name pattern
     *      @param opts the options
     *      @return the file list
     *
     */
    function globAsync(pattern: string, opts?: FIBJS.GeneralObject): Promise<any[]>;

    /**
     * @description Searches the given directory for files matching a set of name patterns
     *
     *      The opts parameter supports the following options:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "cwd": "", // specify a different working directory, default to current directory
     *          "withFileTypes": false // specify whether to return Dirent objects. Default: false
     *      })
     *      ```
     *
     *      The matches of all patterns are merged; a duplicate file appears only once. When `cwd` is
     *      given, the matches of relative patterns are returned relative to that directory.
     *      @param patterns the file name patterns
     *      @param opts the options
     *      @return the file list
     *
     */
    function glob(patterns: string[], opts?: FIBJS.GeneralObject): any[];

    function glob(patterns: string[], opts?: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: any[])=>any): void;

    /**
     * @description Searches the given directory for files matching a set of name patterns
     *
     *      The opts parameter supports the following options:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "cwd": "", // specify a different working directory, default to current directory
     *          "withFileTypes": false // specify whether to return Dirent objects. Default: false
     *      })
     *      ```
     *
     *      The matches of all patterns are merged; a duplicate file appears only once. When `cwd` is
     *      given, the matches of relative patterns are returned relative to that directory.
     *      @param patterns the file name patterns
     *      @param opts the options
     *      @return the file list
     *
     */
    function globSync(patterns: string[], opts?: FIBJS.GeneralObject): any[];

    /**
     * @description Searches the given directory for files matching a set of name patterns
     *
     *      The opts parameter supports the following options:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "cwd": "", // specify a different working directory, default to current directory
     *          "withFileTypes": false // specify whether to return Dirent objects. Default: false
     *      })
     *      ```
     *
     *      The matches of all patterns are merged; a duplicate file appears only once. When `cwd` is
     *      given, the matches of relative patterns are returned relative to that directory.
     *      @param patterns the file name patterns
     *      @param opts the options
     *      @return the file list
     *
     */
    function globAsync(patterns: string[], opts?: FIBJS.GeneralObject): Promise<any[]>;

    /**
     * @description Creates a readable file stream
     *
     *      options supports the following options:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "flags": "r",      // the open mode, "r" (read only) by default
     *          "start": 0,        // the start position of the read
     *          "end": undefined    // end position of the read (inclusive). Default: end of file
     *      })
     *      ```
     *
     *      When start or end is given, the returned stream only covers the [start, end] range (boundaries included); the same stream can be used for reading at explicit positions.
     *      @param fname the file name
     *      @param options the read options
     *      @return the file stream object
     *
     */
    function createReadStream(fname: string, options?: FIBJS.GeneralObject): Class_SeekableStream;

    function createReadStream(fname: string, options?: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_SeekableStream)=>any): void;

    /**
     * @description Creates a readable file stream
     *
     *      options supports the following options:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "flags": "r",      // the open mode, "r" (read only) by default
     *          "start": 0,        // the start position of the read
     *          "end": undefined    // end position of the read (inclusive). Default: end of file
     *      })
     *      ```
     *
     *      When start or end is given, the returned stream only covers the [start, end] range (boundaries included); the same stream can be used for reading at explicit positions.
     *      @param fname the file name
     *      @param options the read options
     *      @return the file stream object
     *
     */
    function createReadStreamSync(fname: string, options?: FIBJS.GeneralObject): Class_SeekableStream;

    /**
     * @description Creates a readable file stream
     *
     *      options supports the following options:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "flags": "r",      // the open mode, "r" (read only) by default
     *          "start": 0,        // the start position of the read
     *          "end": undefined    // end position of the read (inclusive). Default: end of file
     *      })
     *      ```
     *
     *      When start or end is given, the returned stream only covers the [start, end] range (boundaries included); the same stream can be used for reading at explicit positions.
     *      @param fname the file name
     *      @param options the read options
     *      @return the file stream object
     *
     */
    function createReadStreamAsync(fname: string, options?: FIBJS.GeneralObject): Promise<Class_SeekableStreamPromise>;

    /**
     * @description Opens a file and creates a writable stream
     *
     *      The stream writes from the beginning of the file and truncates existing content by default;
     *      use an 'r+' flag to write into an existing file instead. See SeekableStream for the
     *      positioning and write methods.
     *
     *      options supports:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "flags": "w" // the open mode, "w" (create or truncate) by default
     *      })
     *      ```
     *      @param fname the file name
     *      @param options the write options, supporting flags ('w' by default)
     *      @return the file stream object
     *
     */
    function createWriteStream(fname: string, options?: FIBJS.GeneralObject): Class_SeekableStream;

    function createWriteStream(fname: string, options?: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_SeekableStream)=>any): void;

    /**
     * @description Opens a file and creates a writable stream
     *
     *      The stream writes from the beginning of the file and truncates existing content by default;
     *      use an 'r+' flag to write into an existing file instead. See SeekableStream for the
     *      positioning and write methods.
     *
     *      options supports:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "flags": "w" // the open mode, "w" (create or truncate) by default
     *      })
     *      ```
     *      @param fname the file name
     *      @param options the write options, supporting flags ('w' by default)
     *      @return the file stream object
     *
     */
    function createWriteStreamSync(fname: string, options?: FIBJS.GeneralObject): Class_SeekableStream;

    /**
     * @description Opens a file and creates a writable stream
     *
     *      The stream writes from the beginning of the file and truncates existing content by default;
     *      use an 'r+' flag to write into an existing file instead. See SeekableStream for the
     *      positioning and write methods.
     *
     *      options supports:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "flags": "w" // the open mode, "w" (create or truncate) by default
     *      })
     *      ```
     *      @param fname the file name
     *      @param options the write options, supporting flags ('w' by default)
     *      @return the file stream object
     *
     */
    function createWriteStreamAsync(fname: string, options?: FIBJS.GeneralObject): Promise<Class_SeekableStreamPromise>;

    /**
     * @description Opens a file for reading, writing, or both
     *
     *      The flags parameter supports integer fs.constants flags (a combination of values such as fs.constants.O_WRONLY | fs.constants.O_CREAT), or one of the following strings:
     *      - 'r' read only; throws when the file does not exist.
     *      - 'r+' read and write; throws when the file does not exist.
     *      - 'w' write only; the file is created when missing and truncated when existing.
     *      - 'w+' read and write; the file is created when missing.
     *      - 'a' write only, appending; the file is created when missing.
     *      - 'a+' read and write, appending; the file is created when missing.
     *
     *      The returned file stream supports positioning operations such as seek, tell and rewind.
     *
     *      flags may be the integer fs.constants flags, or a string; "r" (read only) is the default.
     *      @param fname the file name
     *      @param flags the open mode
     *      @return the opened file object
     *
     */
    function openFile(fname: string, flags?: string | number): Class_SeekableStream;

    function openFile(fname: string, flags?: string | number, callback: (err: Error | undefined | null, retVal: Class_SeekableStream)=>any): void;

    /**
     * @description Opens a file for reading, writing, or both
     *
     *      The flags parameter supports integer fs.constants flags (a combination of values such as fs.constants.O_WRONLY | fs.constants.O_CREAT), or one of the following strings:
     *      - 'r' read only; throws when the file does not exist.
     *      - 'r+' read and write; throws when the file does not exist.
     *      - 'w' write only; the file is created when missing and truncated when existing.
     *      - 'w+' read and write; the file is created when missing.
     *      - 'a' write only, appending; the file is created when missing.
     *      - 'a+' read and write, appending; the file is created when missing.
     *
     *      The returned file stream supports positioning operations such as seek, tell and rewind.
     *
     *      flags may be the integer fs.constants flags, or a string; "r" (read only) is the default.
     *      @param fname the file name
     *      @param flags the open mode
     *      @return the opened file object
     *
     */
    function openFileSync(fname: string, flags?: string | number): Class_SeekableStream;

    /**
     * @description Opens a file for reading, writing, or both
     *
     *      The flags parameter supports integer fs.constants flags (a combination of values such as fs.constants.O_WRONLY | fs.constants.O_CREAT), or one of the following strings:
     *      - 'r' read only; throws when the file does not exist.
     *      - 'r+' read and write; throws when the file does not exist.
     *      - 'w' write only; the file is created when missing and truncated when existing.
     *      - 'w+' read and write; the file is created when missing.
     *      - 'a' write only, appending; the file is created when missing.
     *      - 'a+' read and write, appending; the file is created when missing.
     *
     *      The returned file stream supports positioning operations such as seek, tell and rewind.
     *
     *      flags may be the integer fs.constants flags, or a string; "r" (read only) is the default.
     *      @param fname the file name
     *      @param flags the open mode
     *      @return the opened file object
     *
     */
    function openFileAsync(fname: string, flags?: string | number): Promise<Class_SeekableStreamPromise>;

    /**
     * @description Opens a file descriptor, using integer fs.constants flags
     *
     *      The same operation is available with a string-flags form taking an octal string mode and a
     *      string-flags form taking a numeric mode defaulting to 0666. `open` returns a FileHandle that
     *      wraps the descriptor; use read, write, fstat and close on it. Consistent with Node.js, the
     *      FileHandle is not a Stream, use createReadStream/createWriteStream for streams.
     *      @param fname the file name
     *      @param flags integer flags, a combination of fs.constants values (such as fs.constants.O_WRONLY | fs.constants.O_CREAT)
     *      @param mode the file mode when the file is created, 0666 by default
     *      @return the opened file descriptor
     *
     */
    function open(fname: string, flags: number, mode?: number): Class_FileHandle;

    function open(fname: string, flags: number, mode?: number, callback: (err: Error | undefined | null, retVal: Class_FileHandle)=>any): void;

    /**
     * @description Opens a file descriptor, using integer fs.constants flags
     *
     *      The same operation is available with a string-flags form taking an octal string mode and a
     *      string-flags form taking a numeric mode defaulting to 0666. `open` returns a FileHandle that
     *      wraps the descriptor; use read, write, fstat and close on it. Consistent with Node.js, the
     *      FileHandle is not a Stream, use createReadStream/createWriteStream for streams.
     *      @param fname the file name
     *      @param flags integer flags, a combination of fs.constants values (such as fs.constants.O_WRONLY | fs.constants.O_CREAT)
     *      @param mode the file mode when the file is created, 0666 by default
     *      @return the opened file descriptor
     *
     */
    function openSync(fname: string, flags: number, mode?: number): Class_FileHandle;

    /**
     * @description Opens a file descriptor, using integer fs.constants flags
     *
     *      The same operation is available with a string-flags form taking an octal string mode and a
     *      string-flags form taking a numeric mode defaulting to 0666. `open` returns a FileHandle that
     *      wraps the descriptor; use read, write, fstat and close on it. Consistent with Node.js, the
     *      FileHandle is not a Stream, use createReadStream/createWriteStream for streams.
     *      @param fname the file name
     *      @param flags integer flags, a combination of fs.constants values (such as fs.constants.O_WRONLY | fs.constants.O_CREAT)
     *      @param mode the file mode when the file is created, 0666 by default
     *      @return the opened file descriptor
     *
     */
    function openAsync(fname: string, flags: number, mode?: number): Promise<Class_FileHandlePromise>;

    /**
     * @description Opens a file
     *
     *      mode may be a number or an octal string (such as '600', '0600', '0o600'), consistent with Node.js; an invalid mode throws.
     *      @param fname the file name
     *      @param flags the open mode
     *      @param mode the file permissions, a number or an octal string
     *      @return the file handle object
     *
     */
    function open(fname: string, flags: string, mode: any): Class_FileHandle;

    function open(fname: string, flags: string, mode: any, callback: (err: Error | undefined | null, retVal: Class_FileHandle)=>any): void;

    /**
     * @description Opens a file
     *
     *      mode may be a number or an octal string (such as '600', '0600', '0o600'), consistent with Node.js; an invalid mode throws.
     *      @param fname the file name
     *      @param flags the open mode
     *      @param mode the file permissions, a number or an octal string
     *      @return the file handle object
     *
     */
    function openSync(fname: string, flags: string, mode: any): Class_FileHandle;

    /**
     * @description Opens a file
     *
     *      mode may be a number or an octal string (such as '600', '0600', '0o600'), consistent with Node.js; an invalid mode throws.
     *      @param fname the file name
     *      @param flags the open mode
     *      @param mode the file permissions, a number or an octal string
     *      @return the file handle object
     *
     */
    function openAsync(fname: string, flags: string, mode: any): Promise<Class_FileHandlePromise>;

    /**
     * @description Opens a file descriptor
     *
     *      The flags parameter supports:
     *      - 'r' read only; throws when the file does not exist.
     *      - 'r+' read and write; throws when the file does not exist.
     *      - 'w' write only; the file is created when missing and truncated when existing.
     *      - 'w+' read and write; the file is created when missing.
     *      - 'a' write only, appending; the file is created when missing.
     *      - 'a+' read and write, appending; the file is created when missing.
     *
     *      The returned FileHandle object works with the descriptor functions fs.read, fs.write, fs.fstat and so on.
     *      @param fname the file name
     *      @param flags the open mode, "r" (read only) by default
     *      @param mode the file mode when the file is created, 0666 by default
     *      @return the opened file descriptor
     *
     */
    function open(fname: string, flags?: string, mode?: number): Class_FileHandle;

    function open(fname: string, flags?: string, mode?: number, callback: (err: Error | undefined | null, retVal: Class_FileHandle)=>any): void;

    /**
     * @description Opens a file descriptor
     *
     *      The flags parameter supports:
     *      - 'r' read only; throws when the file does not exist.
     *      - 'r+' read and write; throws when the file does not exist.
     *      - 'w' write only; the file is created when missing and truncated when existing.
     *      - 'w+' read and write; the file is created when missing.
     *      - 'a' write only, appending; the file is created when missing.
     *      - 'a+' read and write, appending; the file is created when missing.
     *
     *      The returned FileHandle object works with the descriptor functions fs.read, fs.write, fs.fstat and so on.
     *      @param fname the file name
     *      @param flags the open mode, "r" (read only) by default
     *      @param mode the file mode when the file is created, 0666 by default
     *      @return the opened file descriptor
     *
     */
    function openSync(fname: string, flags?: string, mode?: number): Class_FileHandle;

    /**
     * @description Opens a file descriptor
     *
     *      The flags parameter supports:
     *      - 'r' read only; throws when the file does not exist.
     *      - 'r+' read and write; throws when the file does not exist.
     *      - 'w' write only; the file is created when missing and truncated when existing.
     *      - 'w+' read and write; the file is created when missing.
     *      - 'a' write only, appending; the file is created when missing.
     *      - 'a+' read and write, appending; the file is created when missing.
     *
     *      The returned FileHandle object works with the descriptor functions fs.read, fs.write, fs.fstat and so on.
     *      @param fname the file name
     *      @param flags the open mode, "r" (read only) by default
     *      @param mode the file mode when the file is created, 0666 by default
     *      @return the opened file descriptor
     *
     */
    function openAsync(fname: string, flags?: string, mode?: number): Promise<Class_FileHandlePromise>;

    /**
     * @description Closes the file descriptor
     *
     *      fd may be an integer descriptor or a FileHandle object; both close the same underlying descriptor.
     *      @param fd the file descriptor
     *
     */
    function close(fd: number | Class_FileHandle | Class_FileHandlePromise): void;

    function close(fd: number | Class_FileHandle | Class_FileHandlePromise, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Closes the file descriptor
     *
     *      fd may be an integer descriptor or a FileHandle object; both close the same underlying descriptor.
     *      @param fd the file descriptor
     *
     */
    function closeSync(fd: number | Class_FileHandle | Class_FileHandlePromise): void;

    /**
     * @description Closes the file descriptor
     *
     *      fd may be an integer descriptor or a FileHandle object; both close the same underlying descriptor.
     *      @param fd the file descriptor
     *
     */
    function closeAsync(fd: number | Class_FileHandle | Class_FileHandlePromise): Promise<void>;

    /**
     * @description Opens a text file for reading, writing, or both
     *
     *      The flags parameter supports:
     *      - 'r' read only; throws when the file does not exist.
     *      - 'r+' read and write; throws when the file does not exist.
     *      - 'w' write only; the file is created when missing and truncated when existing.
     *      - 'w+' read and write; the file is created when missing.
     *      - 'a' write only, appending; the file is created when missing.
     *      - 'a+' read and write, appending; the file is created when missing.
     *
     *      The returned BufferedStream reads and writes text line by line; the line ending can be set through the EOL property.
     *      @param fname the file name
     *      @param flags the open mode, "r" (read only) by default
     *      @return the opened file object
     *
     */
    function openTextStream(fname: string, flags?: string): Class_BufferedStream;

    function openTextStream(fname: string, flags?: string, callback: (err: Error | undefined | null, retVal: Class_BufferedStream)=>any): void;

    /**
     * @description Opens a text file for reading, writing, or both
     *
     *      The flags parameter supports:
     *      - 'r' read only; throws when the file does not exist.
     *      - 'r+' read and write; throws when the file does not exist.
     *      - 'w' write only; the file is created when missing and truncated when existing.
     *      - 'w+' read and write; the file is created when missing.
     *      - 'a' write only, appending; the file is created when missing.
     *      - 'a+' read and write, appending; the file is created when missing.
     *
     *      The returned BufferedStream reads and writes text line by line; the line ending can be set through the EOL property.
     *      @param fname the file name
     *      @param flags the open mode, "r" (read only) by default
     *      @return the opened file object
     *
     */
    function openTextStreamSync(fname: string, flags?: string): Class_BufferedStream;

    /**
     * @description Opens a text file for reading, writing, or both
     *
     *      The flags parameter supports:
     *      - 'r' read only; throws when the file does not exist.
     *      - 'r+' read and write; throws when the file does not exist.
     *      - 'w' write only; the file is created when missing and truncated when existing.
     *      - 'w+' read and write; the file is created when missing.
     *      - 'a' write only, appending; the file is created when missing.
     *      - 'a+' read and write, appending; the file is created when missing.
     *
     *      The returned BufferedStream reads and writes text line by line; the line ending can be set through the EOL property.
     *      @param fname the file name
     *      @param flags the open mode, "r" (read only) by default
     *      @return the opened file object
     *
     */
    function openTextStreamAsync(fname: string, flags?: string): Promise<Class_BufferedStreamPromise>;

    /**
     * @description Opens a text file and reads its content
     *
     *      The content is decoded as utf-8 and returned.
     *      @param fname the file name
     *      @return the text content of the file
     *
     */
    function readTextFile(fname: string): string;

    function readTextFile(fname: string, callback: (err: Error | undefined | null, retVal: string)=>any): void;

    /**
     * @description Opens a text file and reads its content
     *
     *      The content is decoded as utf-8 and returned.
     *      @param fname the file name
     *      @return the text content of the file
     *
     */
    function readTextFileSync(fname: string): string;

    /**
     * @description Opens a text file and reads its content
     *
     *      The content is decoded as utf-8 and returned.
     *      @param fname the file name
     *      @return the text content of the file
     *
     */
    function readTextFileAsync(fname: string): Promise<string>;

    /**
     * @description Reads the whole content of a file, by its file descriptor or by its name
     *
     *      options supports the following options:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "encoding": "utf8" // specify the encoding, default is utf8.
     *      })
     *      ```
     *
     *      Example — read the same file as a string and as a Buffer:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-read-'));
     *      const file = path.join(dir, 'data.txt');
     *      fs.writeFile(file, 'plain text');
     *
     *      console.log(fs.readFile(file, 'utf8')); // plain text
     *      console.log(Buffer.isBuffer(fs.readFile(file))); // true
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *
     *      An encoding string is empty by default, nothing is decoded and a Buffer object is returned;
     *      when an encoding is given, the decoded string is returned. Consistent with Node.js: a file
     *      descriptor is not closed after reading, and reading starts at the current position of the
     *      descriptor and advances it.
     *
     *      fname may be the file name, an integer file descriptor, or a FileHandle object.
     *      options may be the decoding string, or the read options object; a descriptor read with an options object defaults to utf8, the other forms return a Buffer unless an encoding is given.
     *      @param fname the file to read
     *      @param options the decoding or the read options
     *      @return the file content
     *
     */
    function readFile(fname: Class_FileHandle | Class_FileHandlePromise | string | number, options?: FIBJS.GeneralObject | string): any;

    function readFile(fname: Class_FileHandle | Class_FileHandlePromise | string | number, options?: FIBJS.GeneralObject | string, callback: (err: Error | undefined | null, retVal: any)=>any): void;

    /**
     * @description Reads the whole content of a file, by its file descriptor or by its name
     *
     *      options supports the following options:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "encoding": "utf8" // specify the encoding, default is utf8.
     *      })
     *      ```
     *
     *      Example — read the same file as a string and as a Buffer:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-read-'));
     *      const file = path.join(dir, 'data.txt');
     *      fs.writeFile(file, 'plain text');
     *
     *      console.log(fs.readFile(file, 'utf8')); // plain text
     *      console.log(Buffer.isBuffer(fs.readFile(file))); // true
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *
     *      An encoding string is empty by default, nothing is decoded and a Buffer object is returned;
     *      when an encoding is given, the decoded string is returned. Consistent with Node.js: a file
     *      descriptor is not closed after reading, and reading starts at the current position of the
     *      descriptor and advances it.
     *
     *      fname may be the file name, an integer file descriptor, or a FileHandle object.
     *      options may be the decoding string, or the read options object; a descriptor read with an options object defaults to utf8, the other forms return a Buffer unless an encoding is given.
     *      @param fname the file to read
     *      @param options the decoding or the read options
     *      @return the file content
     *
     */
    function readFileSync(fname: Class_FileHandle | Class_FileHandlePromise | string | number, options?: FIBJS.GeneralObject | string): any;

    /**
     * @description Reads the whole content of a file, by its file descriptor or by its name
     *
     *      options supports the following options:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "encoding": "utf8" // specify the encoding, default is utf8.
     *      })
     *      ```
     *
     *      Example — read the same file as a string and as a Buffer:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-read-'));
     *      const file = path.join(dir, 'data.txt');
     *      fs.writeFile(file, 'plain text');
     *
     *      console.log(fs.readFile(file, 'utf8')); // plain text
     *      console.log(Buffer.isBuffer(fs.readFile(file))); // true
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *
     *      An encoding string is empty by default, nothing is decoded and a Buffer object is returned;
     *      when an encoding is given, the decoded string is returned. Consistent with Node.js: a file
     *      descriptor is not closed after reading, and reading starts at the current position of the
     *      descriptor and advances it.
     *
     *      fname may be the file name, an integer file descriptor, or a FileHandle object.
     *      options may be the decoding string, or the read options object; a descriptor read with an options object defaults to utf8, the other forms return a Buffer unless an encoding is given.
     *      @param fname the file to read
     *      @param options the decoding or the read options
     *      @return the file content
     *
     */
    function readFileAsync(fname: Class_FileHandle | Class_FileHandlePromise | string | number, options?: FIBJS.GeneralObject | string): Promise<any>;

    /**
     * @description Opens a file and reads a set of text lines into an array; the line ending follows the EOL property: "\n" on posix and "\r\n" on windows by default
     *
     *      Reads up to maxlines lines; a trailing line ending does not produce an extra empty entry.
     *      @param fname the file name
     *      @param maxlines the maximum number of lines to read, all lines by default
     *      @return the array of text lines read; an empty array when the file is empty or has no readable data
     *
     */
    function readLines(fname: string, maxlines?: number): string[];

    /**
     * @description Writes content into a file by its file descriptor
     *
     *      length defaults to -1, which writes all the remaining data of buffer from offset. position defaults to -1, which writes from the current file position.
     *
     *      fd may be an integer descriptor or a FileHandle object; position addresses that descriptor's file position.
     *      @param fd the file descriptor
     *      @param buffer the Buffer object to write
     *      @param offset the read offset in the Buffer, 0 by default
     *      @param length the number of bytes to write, -1 by default
     *      @param position the write position, the current file position by default
     *      @return the number of bytes actually written
     *
     */
    function write(fd: number | Class_FileHandle | Class_FileHandlePromise, buffer: Class_Buffer, offset?: number, length?: number, position?: number): number;

    function write(fd: number | Class_FileHandle | Class_FileHandlePromise, buffer: Class_Buffer, offset?: number, length?: number, position?: number, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Writes content into a file by its file descriptor
     *
     *      length defaults to -1, which writes all the remaining data of buffer from offset. position defaults to -1, which writes from the current file position.
     *
     *      fd may be an integer descriptor or a FileHandle object; position addresses that descriptor's file position.
     *      @param fd the file descriptor
     *      @param buffer the Buffer object to write
     *      @param offset the read offset in the Buffer, 0 by default
     *      @param length the number of bytes to write, -1 by default
     *      @param position the write position, the current file position by default
     *      @return the number of bytes actually written
     *
     */
    function writeSync(fd: number | Class_FileHandle | Class_FileHandlePromise, buffer: Class_Buffer, offset?: number, length?: number, position?: number): number;

    /**
     * @description Writes content into a file by its file descriptor
     *
     *      length defaults to -1, which writes all the remaining data of buffer from offset. position defaults to -1, which writes from the current file position.
     *
     *      fd may be an integer descriptor or a FileHandle object; position addresses that descriptor's file position.
     *      @param fd the file descriptor
     *      @param buffer the Buffer object to write
     *      @param offset the read offset in the Buffer, 0 by default
     *      @param length the number of bytes to write, -1 by default
     *      @param position the write position, the current file position by default
     *      @return the number of bytes actually written
     *
     */
    function writeAsync(fd: number | Class_FileHandle | Class_FileHandlePromise, buffer: Class_Buffer, offset?: number, length?: number, position?: number): Promise<number>;

    /**
     * @description Writes content into a file by its file descriptor
     *
     *      position defaults to -1, which writes from the current file position. The string is encoded with encoding before writing.
     *
     *      fd may be an integer descriptor or a FileHandle object; position addresses that descriptor's file position.
     *      @param fd the file descriptor
     *      @param string the string to write
     *      @param position the write position, the current file position by default
     *      @param encoding the decoding, utf8 by default
     *      @return the number of bytes actually written
     *
     */
    function write(fd: number | Class_FileHandle | Class_FileHandlePromise, string: string, position?: number, encoding?: string): number;

    function write(fd: number | Class_FileHandle | Class_FileHandlePromise, string: string, position?: number, encoding?: string, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Writes content into a file by its file descriptor
     *
     *      position defaults to -1, which writes from the current file position. The string is encoded with encoding before writing.
     *
     *      fd may be an integer descriptor or a FileHandle object; position addresses that descriptor's file position.
     *      @param fd the file descriptor
     *      @param string the string to write
     *      @param position the write position, the current file position by default
     *      @param encoding the decoding, utf8 by default
     *      @return the number of bytes actually written
     *
     */
    function writeSync(fd: number | Class_FileHandle | Class_FileHandlePromise, string: string, position?: number, encoding?: string): number;

    /**
     * @description Writes content into a file by its file descriptor
     *
     *      position defaults to -1, which writes from the current file position. The string is encoded with encoding before writing.
     *
     *      fd may be an integer descriptor or a FileHandle object; position addresses that descriptor's file position.
     *      @param fd the file descriptor
     *      @param string the string to write
     *      @param position the write position, the current file position by default
     *      @param encoding the decoding, utf8 by default
     *      @return the number of bytes actually written
     *
     */
    function writeAsync(fd: number | Class_FileHandle | Class_FileHandlePromise, string: string, position?: number, encoding?: string): Promise<number>;

    /**
     * @description Creates a text file and writes content into it
     *
     *      The file is opened for overwriting; existing content is truncated.
     *      @param fname the file name
     *      @param txt the string to write
     *      @return the number of bytes actually written
     *
     */
    function writeTextFile(fname: string, txt: string): number;

    function writeTextFile(fname: string, txt: string, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Creates a text file and writes content into it
     *
     *      The file is opened for overwriting; existing content is truncated.
     *      @param fname the file name
     *      @param txt the string to write
     *      @return the number of bytes actually written
     *
     */
    function writeTextFileSync(fname: string, txt: string): number;

    /**
     * @description Creates a text file and writes content into it
     *
     *      The file is opened for overwriting; existing content is truncated.
     *      @param fname the file name
     *      @param txt the string to write
     *      @return the number of bytes actually written
     *
     */
    function writeTextFileAsync(fname: string, txt: string): Promise<number>;

    /**
     * @description Writes content into a file, by its file descriptor or by its name
     *
     *      The file is opened for overwriting, existing content is truncated. opt is the encoding of text data, utf8 by default; an options object carries the write options instead:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "encoding": "utf8", // specify the encoding, default is utf8.
     *          "mode": 0666, // specify the file mode. Default: 0666
     *          "flag": "w" // specify the open flag. Default: w
     *      })
     *      ```
     *
     *      Example — overwriting an existing file:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-write-'));
     *      const file = path.join(dir, 'data.txt');
     *
     *      fs.writeFile(file, 'first');
     *      fs.writeFile(file, 'second'); // replaces the previous content
     *      console.log(fs.readFile(file, 'utf8')); // second
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *
     *      A file descriptor ignores the options object and encodes text data as utf8. Unlike Node.js,
     *      which writes at the current position of a descriptor, the fibjs descriptor form seeks to the
     *      beginning and truncates the file first.
     *
     *      fname may be the file name, an integer file descriptor, or a FileHandle object.
     *      @param fname the file to write
     *      @param data the data to write
     *      @param opt the encoding of text data or the write options
     *      @return the number of bytes actually written
     *
     */
    function writeFile(fname: Class_FileHandle | Class_FileHandlePromise | string | number, data: Class_Buffer | string, opt?: FIBJS.GeneralObject | string): number;

    function writeFile(fname: Class_FileHandle | Class_FileHandlePromise | string | number, data: Class_Buffer | string, opt?: FIBJS.GeneralObject | string, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Writes content into a file, by its file descriptor or by its name
     *
     *      The file is opened for overwriting, existing content is truncated. opt is the encoding of text data, utf8 by default; an options object carries the write options instead:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "encoding": "utf8", // specify the encoding, default is utf8.
     *          "mode": 0666, // specify the file mode. Default: 0666
     *          "flag": "w" // specify the open flag. Default: w
     *      })
     *      ```
     *
     *      Example — overwriting an existing file:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-write-'));
     *      const file = path.join(dir, 'data.txt');
     *
     *      fs.writeFile(file, 'first');
     *      fs.writeFile(file, 'second'); // replaces the previous content
     *      console.log(fs.readFile(file, 'utf8')); // second
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *
     *      A file descriptor ignores the options object and encodes text data as utf8. Unlike Node.js,
     *      which writes at the current position of a descriptor, the fibjs descriptor form seeks to the
     *      beginning and truncates the file first.
     *
     *      fname may be the file name, an integer file descriptor, or a FileHandle object.
     *      @param fname the file to write
     *      @param data the data to write
     *      @param opt the encoding of text data or the write options
     *      @return the number of bytes actually written
     *
     */
    function writeFileSync(fname: Class_FileHandle | Class_FileHandlePromise | string | number, data: Class_Buffer | string, opt?: FIBJS.GeneralObject | string): number;

    /**
     * @description Writes content into a file, by its file descriptor or by its name
     *
     *      The file is opened for overwriting, existing content is truncated. opt is the encoding of text data, utf8 by default; an options object carries the write options instead:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "encoding": "utf8", // specify the encoding, default is utf8.
     *          "mode": 0666, // specify the file mode. Default: 0666
     *          "flag": "w" // specify the open flag. Default: w
     *      })
     *      ```
     *
     *      Example — overwriting an existing file:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-write-'));
     *      const file = path.join(dir, 'data.txt');
     *
     *      fs.writeFile(file, 'first');
     *      fs.writeFile(file, 'second'); // replaces the previous content
     *      console.log(fs.readFile(file, 'utf8')); // second
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *
     *      A file descriptor ignores the options object and encodes text data as utf8. Unlike Node.js,
     *      which writes at the current position of a descriptor, the fibjs descriptor form seeks to the
     *      beginning and truncates the file first.
     *
     *      fname may be the file name, an integer file descriptor, or a FileHandle object.
     *      @param fname the file to write
     *      @param data the data to write
     *      @param opt the encoding of text data or the write options
     *      @return the number of bytes actually written
     *
     */
    function writeFileAsync(fname: Class_FileHandle | Class_FileHandlePromise | string | number, data: Class_Buffer | string, opt?: FIBJS.GeneralObject | string): Promise<number>;

    /**
     * @description Appends content to a file, by its file descriptor or by its name
     *
     *      The file is created when it does not exist. options is the encoding of the data to append; an options object carries the write options instead:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "encoding": "utf8", // specify the encoding of string data. Default: utf8
     *          "mode": 0666, // specify the file mode. Default: 0666
     *          "flag": "a" // specify the open flag. Default: a
     *      })
     *      ```
     *      Consistent with Node.js, `flag` defaults to 'a' (append) and may be 'w'/'wx'/'ax' and so on. The encoding of an options object only validates the label, the data is appended as it is; a file descriptor ignores the options object and appends string data as utf8, at the current position of the descriptor rather than necessarily at the end of the file.
     *
     *      fname may be the file name, an integer file descriptor, or a FileHandle object.
     *      @param fname the file to append to
     *      @param data the data to write
     *      @param options the encoding or the write options
     *      @return the number of bytes actually written
     *
     */
    function appendFile(fname: Class_FileHandle | Class_FileHandlePromise | string | number, data: Class_Buffer | string, options?: FIBJS.GeneralObject | string): number;

    function appendFile(fname: Class_FileHandle | Class_FileHandlePromise | string | number, data: Class_Buffer | string, options?: FIBJS.GeneralObject | string, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Appends content to a file, by its file descriptor or by its name
     *
     *      The file is created when it does not exist. options is the encoding of the data to append; an options object carries the write options instead:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "encoding": "utf8", // specify the encoding of string data. Default: utf8
     *          "mode": 0666, // specify the file mode. Default: 0666
     *          "flag": "a" // specify the open flag. Default: a
     *      })
     *      ```
     *      Consistent with Node.js, `flag` defaults to 'a' (append) and may be 'w'/'wx'/'ax' and so on. The encoding of an options object only validates the label, the data is appended as it is; a file descriptor ignores the options object and appends string data as utf8, at the current position of the descriptor rather than necessarily at the end of the file.
     *
     *      fname may be the file name, an integer file descriptor, or a FileHandle object.
     *      @param fname the file to append to
     *      @param data the data to write
     *      @param options the encoding or the write options
     *      @return the number of bytes actually written
     *
     */
    function appendFileSync(fname: Class_FileHandle | Class_FileHandlePromise | string | number, data: Class_Buffer | string, options?: FIBJS.GeneralObject | string): number;

    /**
     * @description Appends content to a file, by its file descriptor or by its name
     *
     *      The file is created when it does not exist. options is the encoding of the data to append; an options object carries the write options instead:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "encoding": "utf8", // specify the encoding of string data. Default: utf8
     *          "mode": 0666, // specify the file mode. Default: 0666
     *          "flag": "a" // specify the open flag. Default: a
     *      })
     *      ```
     *      Consistent with Node.js, `flag` defaults to 'a' (append) and may be 'w'/'wx'/'ax' and so on. The encoding of an options object only validates the label, the data is appended as it is; a file descriptor ignores the options object and appends string data as utf8, at the current position of the descriptor rather than necessarily at the end of the file.
     *
     *      fname may be the file name, an integer file descriptor, or a FileHandle object.
     *      @param fname the file to append to
     *      @param data the data to write
     *      @param options the encoding or the write options
     *      @return the number of bytes actually written
     *
     */
    function appendFileAsync(fname: Class_FileHandle | Class_FileHandlePromise | string | number, data: Class_Buffer | string, options?: FIBJS.GeneralObject | string): Promise<number>;

    /**
     * @description Sets a zip virtual file mapping
     *
     *      The zip data is mapped onto the given path; file accesses to that path are then read from the mapped zip. Entries inside the zip are reached by appending `$` to the mapping path, for example `/archive.zip$/dir/file.txt`.
     *
     *      data may be a Buffer holding the zip, or a string; a string is encoded as utf8.
     *      @param fname the mapping path, a string is encoded as utf8
     *      @param data the zip data to map
     *
     */
    function setZipFS(fname: string, data: Class_Buffer | string): void;

    /**
     * @description Clears zip virtual file mappings
     *
     *      When fname is omitted every mapping is cleared; afterwards accesses to those paths fall back
     *      to the real file system.
     *      @param fname the mapping path, all caches are cleared by default
     *
     */
    function clearZipFS(fname?: string): void;

    /**
     * @description Watches a file and returns the corresponding watcher object
     *
     *      Equivalent to watch(fname, {}, callback) without a callback; attach the handler with
     *      `watcher.on('change', ...)` or pass it to another overload.
     *
     *      The options object supports:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "persistent": true, // keep the process running while files are watched
     *          "recursive": false, // watch subdirectories too, false by default
     *          "encoding": "utf8", // file name encoding; 'buffer' passes a Buffer
     *      })
     *      ```
     *
     *      On Linux the recursive option is only stable on win32/darwin; it is forwarded to the uv
     *      backend but the handler may be invoked at times you do not expect. Use watchFile when the
     *      platform notification service is unreliable.
     *
     *      Example — watch a directory and stop after the first change:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-watch-'));
     *      let closed = false;
     *      const watcher = fs.watch(dir, (eventType, filename) => {
     *          console.log(eventType, filename);
     *          if (closed) return;
     *          closed = true;
     *          watcher.close();
     *          fs.rmSync(dir, { recursive: true, force: true });
     *      });
     *      fs.writeFile(path.join(dir, 'trigger.txt'), 'x');
     *      ```
     *      @param fname the file to watch
     *      @return the FSWatcher object
     *
     */
    function watch(fname: string): Class_FSWatcher;

    /**
     * @description Watches a file and returns the corresponding watcher object
     *
     *      The callback receives `(eventType, filename)`, where eventType is 'change' or 'rename' and
     *      filename may be null when the platform does not report it; it is called for every event.
     *      @param fname the file to watch
     *      @param callback `(evtType: 'change' | 'rename', filename: string) => any` the handler called when the file changes
     *      @return the FSWatcher object
     *
     */
    function watch(fname: string, callback: (eventType: string, filename: string | Class_Buffer)=>void): Class_FSWatcher;

    /**
     * @description Watches a file and returns the corresponding watcher object
     *
     *      The options (persistent, recursive, encoding) are described on the first overload.
     *      @param fname the file to watch
     *      @param options the watch options
     *      @return the FSWatcher object
     *
     */
    function watch(fname: string, options: FIBJS.GeneralObject): Class_FSWatcher;

    /**
     * @description Watches a file and returns the corresponding watcher object
     *
     *      options supports the following options:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "persistent": true, // keep the process running while files are watched
     *          "recursive": false, // watch subdirectories too, false by default
     *          "encoding": "utf8", // file name encoding; 'buffer' passes a Buffer
     *      })
     *      ```
     *      @param fname the file to watch
     *      @param options the watch options
     *      @param callback `(evtType: 'change' | 'rename', filename: string) => any` the handler called when the file changes
     *      @return the FSWatcher object
     *
     */
    function watch(fname: string, options: FIBJS.GeneralObject, callback: (eventType: string, filename: string | Class_Buffer)=>void): Class_FSWatcher;

    /**
     * @description Watches a file and returns the corresponding StatsWatcher object
     *
     *      The file status is checked periodically; the callback is called when it changes, receiving the Stat objects before and after the change. Returns a StatsWatcher deriving from EventEmitter; fs.unwatchFile(fname) or StatsWatcher.close() stops watching.
     *
     *      The options object supports:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "persistent": true, // keep the process running while files are watched
     *          "bigint": false, // accepted for Node.js compatibility, not implemented
     *          "interval": 5007 // poll period in milliseconds. Default: 5007
     *      })
     *      ```
     *      An interval smaller than 20 milliseconds falls back to the default.
     *      @param fname the file to watch
     *      @param callback `(curStats: Stats, prevStats: Stats) => any` the handler called when the stats of the file change
     *      @return the StatsWatcher object
     *
     */
    function watchFile(fname: string, callback: (curStats: Class_Stat, prevStats: Class_Stat)=>void): Class_StatsWatcher;

    /**
     * @description Watches a file and returns the corresponding StatsWatcher object
     *
     *      The options (persistent, bigint, interval) are described on the first overload; watchFile
     *      uses stat polling, fs.watch is preferred when the platform notification service is available.
     *      @param fname the file to watch
     *      @param options the watch options
     *      @param callback `(curStats: Stats, prevStats: Stats) => any` the handler called when the stats of the file change
     *      @return the StatsWatcher object
     *
     */
    function watchFile(fname: string, options: FIBJS.GeneralObject, callback: (curStats: Class_Stat, prevStats: Class_Stat)=>void): Class_StatsWatcher;

    /**
     * @description Removes all watch event handlers from the StatsWatcher watching fname
     *
     *      Has no effect when the file is not being watched.
     *      @param fname the file to watch
     *
     */
    function unwatchFile(fname: string): void;

    /**
     * @description Removes the `callback` handler from the watch event handlers of the StatsWatcher watching fname
     *
     *      No error is raised even when callback is not among the watch event handlers of the StatsWatcher.
     *      @param fname the file to watch
     *      @param callback the handler to remove
     *
     */
    function unwatchFile(fname: string, callback: (curStats: Class_Stat, prevStats: Class_Stat)=>void): void;

}

