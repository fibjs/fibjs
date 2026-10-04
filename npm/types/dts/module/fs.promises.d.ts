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
 * The promise variant of the fs module: async members return a Promise as their primary form.
 */
declare module 'fs/promises' {
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
     */
    const constants: typeof import ('fs_constants');

    /**
     * @description The alias of the Stat class, see Stat
     */
    const Stats: typeof Class_Stat;

    /**
     * @description The alias of the DirEntry class, see DirEntry
     */
    const Dirent: typeof Class_DirEntry;

    /**
     * @description The alias of the Dir class, see Dir
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
    function exists(path: string): Promise<boolean>;

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
    function exists(path: string, options: FIBJS.GeneralObject): Promise<boolean>;

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
    function access(path: string, mode?: number): Promise<void>;

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
     *      @param oldPath the source file
     *      @param newPath the file to create
     *
     */
    function link(oldPath: string, newPath: string): Promise<void>;

    /**
     * @description Creates a hard link; not supported on Windows
     *      @param oldPath the source file
     *      @param newPath the file to create
     *
     */
    function linkSync(oldPath: string, newPath: string): void;

    /**
     * @description Creates a hard link; not supported on Windows
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
    function unlink(path: string): Promise<void>;

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
     *      mode specifies the directory permissions and is ignored on Windows; an existing directory throws, unless the recursive option is used to create parent directories. A string mode is an octal number (such as '755', '0755', '0o755'), consistent with Node.js; an invalid mode throws.
     *
     *      The options object may contain:
     *      ```JavaScript
     *      {
     *          recursive: false, // specify whether parent directories should be created. Default: false
     *          mode: 0777 // specify the file mode. Default: 0777
     *      }
     *      ```
     *
     *      When recursive is true, the path of the first created directory is returned, consistent with Node.js; when the directory already exists, undefined is returned.
     *      @param path the directory to create
     *      @param mode the file mode or the creation options: a number, an octal string, or an options object
     *      @return the path of the first created directory when recursive is true and a directory was actually created
     *
     */
    function mkdir(path: string, mode?: number | FIBJS.GeneralObject | any): Promise<any>;

    /**
     * @description Creates a directory
     *
     *      mode specifies the directory permissions and is ignored on Windows; an existing directory throws, unless the recursive option is used to create parent directories. A string mode is an octal number (such as '755', '0755', '0o755'), consistent with Node.js; an invalid mode throws.
     *
     *      The options object may contain:
     *      ```JavaScript
     *      {
     *          recursive: false, // specify whether parent directories should be created. Default: false
     *          mode: 0777 // specify the file mode. Default: 0777
     *      }
     *      ```
     *
     *      When recursive is true, the path of the first created directory is returned, consistent with Node.js; when the directory already exists, undefined is returned.
     *      @param path the directory to create
     *      @param mode the file mode or the creation options: a number, an octal string, or an options object
     *      @return the path of the first created directory when recursive is true and a directory was actually created
     *
     */
    function mkdirSync(path: string, mode?: number | FIBJS.GeneralObject | any): any;

    /**
     * @description Creates a directory
     *
     *      mode specifies the directory permissions and is ignored on Windows; an existing directory throws, unless the recursive option is used to create parent directories. A string mode is an octal number (such as '755', '0755', '0o755'), consistent with Node.js; an invalid mode throws.
     *
     *      The options object may contain:
     *      ```JavaScript
     *      {
     *          recursive: false, // specify whether parent directories should be created. Default: false
     *          mode: 0777 // specify the file mode. Default: 0777
     *      }
     *      ```
     *
     *      When recursive is true, the path of the first created directory is returned, consistent with Node.js; when the directory already exists, undefined is returned.
     *      @param path the directory to create
     *      @param mode the file mode or the creation options: a number, an octal string, or an options object
     *      @return the path of the first created directory when recursive is true and a directory was actually created
     *
     */
    function mkdirAsync(path: string, mode?: number | FIBJS.GeneralObject | any): Promise<any>;

    /**
     * @description Creates a unique temporary directory
     *
     *      The directory is created under the system temporary directory, its name starts with prefix and ends with a random suffix.
     *      @param prefix the prefix of the temporary directory name
     *      @return the path of the created temporary directory
     *
     */
    function mkdtemp(prefix: string): Promise<string>;

    /**
     * @description Creates a unique temporary directory
     *
     *      The directory is created under the system temporary directory, its name starts with prefix and ends with a random suffix.
     *      @param prefix the prefix of the temporary directory name
     *      @return the path of the created temporary directory
     *
     */
    function mkdtempSync(prefix: string): string;

    /**
     * @description Creates a unique temporary directory
     *
     *      The directory is created under the system temporary directory, its name starts with prefix and ends with a random suffix.
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
     *      {
     *          recursive: false // specify whether all subdirectories and files should be removed. Default: false
     *      }
     *      ```
     *      @param path the directory to remove
     *      @param opt the removal options
     *
     */
    function rmdir(path: string, opt?: FIBJS.GeneralObject): Promise<void>;

    /**
     * @description Removes a directory
     *
     *      The options may contain:
     *      ```JavaScript
     *      {
     *          recursive: false // specify whether all subdirectories and files should be removed. Default: false
     *      }
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
     *      {
     *          recursive: false // specify whether all subdirectories and files should be removed. Default: false
     *      }
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
     *      {
     *          recursive: false, // specify whether all subdirectories and files should be removed. Default: false
     *          force: false // whether to ignore nonexistent paths. Default: false
     *      }
     *      ```
     *
     *      When recursive is false, only files and symbolic links can be removed; removing a directory throws EISDIR. When recursive is true, the directory and all its content are removed recursively; a symbolic link is removed itself without following the target. A nonexistent path throws ENOENT, unless force is true, which ignores nonexistent paths.
     *      @param path the directory to remove
     *      @param opt the removal options
     *
     */
    function rm(path: string, opt?: FIBJS.GeneralObject): Promise<void>;

    /**
     * @description Removes a file or directory
     *
     *      The options may contain:
     *      ```JavaScript
     *      {
     *          recursive: false, // specify whether all subdirectories and files should be removed. Default: false
     *          force: false // whether to ignore nonexistent paths. Default: false
     *      }
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
     *      {
     *          recursive: false, // specify whether all subdirectories and files should be removed. Default: false
     *          force: false // whether to ignore nonexistent paths. Default: false
     *      }
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
    function rename(from: string, to: string): Promise<void>;

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
    function copyFile(from: string, to: string, mode?: number): Promise<void>;

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
     *      {
     *          recursive: false, // recursively copy directories. Default: false
     *          force: true, // overwrite existing files or directories. Default: true
     *          mode: 0 // modifiers for copy operation. Default: 0
     *      }
     *      ```
     *      @param src the source path to copy
     *      @param dest the target path to copy to
     *      @param opts the copy options
     *
     */
    function cp(src: string, dest: string, opts?: FIBJS.GeneralObject): Promise<void>;

    /**
     * @description Copies src to dest asynchronously, including subdirectories and files.
     *
     *      When src is a directory, it is not copied recursively by default; set recursive to true for that.
     *
     *      opts supports the following options:
     *      ```JavaScript
     *      {
     *          recursive: false, // recursively copy directories. Default: false
     *          force: true, // overwrite existing files or directories. Default: true
     *          mode: 0 // modifiers for copy operation. Default: 0
     *      }
     *      ```
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
     *      {
     *          recursive: false, // recursively copy directories. Default: false
     *          force: true, // overwrite existing files or directories. Default: true
     *          mode: 0 // modifiers for copy operation. Default: 0
     *      }
     *      ```
     *      @param src the source path to copy
     *      @param dest the target path to copy to
     *      @param opts the copy options
     *
     */
    function cpAsync(src: string, dest: string, opts?: FIBJS.GeneralObject): Promise<void>;

    /**
     * @description Sets the access permissions of the given file; not supported on Windows
     *
     *      mode may be a number or an octal string (such as '755', '0755', '0o755'), consistent with Node.js.
     *      @param path the file to operate on, a string is encoded as utf8
     *      @param mode the access permissions to set
     *
     */
    function chmod(path: string, mode: number | any): Promise<void>;

    /**
     * @description Sets the access permissions of the given file; not supported on Windows
     *
     *      mode may be a number or an octal string (such as '755', '0755', '0o755'), consistent with Node.js.
     *      @param path the file to operate on, a string is encoded as utf8
     *      @param mode the access permissions to set
     *
     */
    function chmodSync(path: string, mode: number | any): void;

    /**
     * @description Sets the access permissions of the given file; not supported on Windows
     *
     *      mode may be a number or an octal string (such as '755', '0755', '0o755'), consistent with Node.js.
     *      @param path the file to operate on, a string is encoded as utf8
     *      @param mode the access permissions to set
     *
     */
    function chmodAsync(path: string, mode: number | any): Promise<void>;

    /**
     * @description Sets the access permissions of the given file without changing the target of a symbolic link; available on macOS and BSD platforms only
     *      @param path the file to operate on, a string is encoded as utf8
     *      @param mode the access permissions to set
     *
     */
    function lchmod(path: string, mode: number | any): Promise<void>;

    /**
     * @description Sets the access permissions of the given file without changing the target of a symbolic link; available on macOS and BSD platforms only
     *      @param path the file to operate on, a string is encoded as utf8
     *      @param mode the access permissions to set
     *
     */
    function lchmodSync(path: string, mode: number | any): void;

    /**
     * @description Sets the access permissions of the given file without changing the target of a symbolic link; available on macOS and BSD platforms only
     *      @param path the file to operate on, a string is encoded as utf8
     *      @param mode the access permissions to set
     *
     */
    function lchmodAsync(path: string, mode: number | any): Promise<void>;

    /**
     * @description Sets the owner of the given file; not supported on Windows
     *      @param path the file to set
     *      @param uid the user id of the owner
     *      @param gid the group id of the owner
     *
     */
    function chown(path: string, uid: number, gid: number): Promise<void>;

    /**
     * @description Sets the owner of the given file; not supported on Windows
     *      @param path the file to set
     *      @param uid the user id of the owner
     *      @param gid the group id of the owner
     *
     */
    function chownSync(path: string, uid: number, gid: number): void;

    /**
     * @description Sets the owner of the given file; not supported on Windows
     *      @param path the file to set
     *      @param uid the user id of the owner
     *      @param gid the group id of the owner
     *
     */
    function chownAsync(path: string, uid: number, gid: number): Promise<void>;

    /**
     * @description Sets the owner of the given file without changing the target of a symbolic link; not supported on Windows
     *      @param path the file to set
     *      @param uid the user id of the owner
     *      @param gid the group id of the owner
     *
     */
    function lchown(path: string, uid: number, gid: number): Promise<void>;

    /**
     * @description Sets the owner of the given file without changing the target of a symbolic link; not supported on Windows
     *      @param path the file to set
     *      @param uid the user id of the owner
     *      @param gid the group id of the owner
     *
     */
    function lchownSync(path: string, uid: number, gid: number): void;

    /**
     * @description Sets the owner of the given file without changing the target of a symbolic link; not supported on Windows
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
    function utimes(path: string, atime: any, mtime: any): Promise<void>;

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
    function lutimes(path: string, atime: any, mtime: any): Promise<void>;

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
     *      Throws when the path does not exist.
     *      @param path the file to query
     *      @return the basic information of the file
     *
     */
    function stat(path: string): Promise<Class_Stat>;

    /**
     * @description Queries the basic information of the given file
     *
     *      Throws when the path does not exist.
     *      @param path the file to query
     *      @return the basic information of the file
     *
     */
    function statSync(path: string): Class_Stat;

    /**
     * @description Queries the basic information of the given file
     *
     *      Throws when the path does not exist.
     *      @param path the file to query
     *      @return the basic information of the file
     *
     */
    function statAsync(path: string): Promise<Class_Stat>;

    /**
     * @description Queries the basic information of the given file
     *
     *      options supports the following options:
     *      ```JavaScript
     *      {
     *          "throwIfNoEntry": true // whether a nonexistent path throws; returns undefined when false. Default: true
     *      }
     *      ```
     *
     *      `throwIfNoEntry` works like Node.js and only takes effect for synchronous (no callback) calls; the asynchronous form always throws.
     *      @param path the file to query
     *      @param options the query options
     *      @return the basic information of the file, or undefined when `throwIfNoEntry` is false and the path does not exist
     *
     */
    function stat(path: string, options: FIBJS.GeneralObject): Promise<Class_Stat>;

    /**
     * @description Queries the basic information of the given file
     *
     *      options supports the following options:
     *      ```JavaScript
     *      {
     *          "throwIfNoEntry": true // whether a nonexistent path throws; returns undefined when false. Default: true
     *      }
     *      ```
     *
     *      `throwIfNoEntry` works like Node.js and only takes effect for synchronous (no callback) calls; the asynchronous form always throws.
     *      @param path the file to query
     *      @param options the query options
     *      @return the basic information of the file, or undefined when `throwIfNoEntry` is false and the path does not exist
     *
     */
    function statSync(path: string, options: FIBJS.GeneralObject): Class_Stat;

    /**
     * @description Queries the basic information of the given file
     *
     *      options supports the following options:
     *      ```JavaScript
     *      {
     *          "throwIfNoEntry": true // whether a nonexistent path throws; returns undefined when false. Default: true
     *      }
     *      ```
     *
     *      `throwIfNoEntry` works like Node.js and only takes effect for synchronous (no callback) calls; the asynchronous form always throws.
     *      @param path the file to query
     *      @param options the query options
     *      @return the basic information of the file, or undefined when `throwIfNoEntry` is false and the path does not exist
     *
     */
    function statAsync(path: string, options: FIBJS.GeneralObject): Promise<Class_Stat>;

    /**
     * @description Queries the basic information of the given file; unlike stat, when path is a symbolic link, the information of the link itself is returned instead of its target
     *      @param path the file to query
     *      @return the basic information of the file
     *
     */
    function lstat(path: string): Promise<Class_Stat>;

    /**
     * @description Queries the basic information of the given file; unlike stat, when path is a symbolic link, the information of the link itself is returned instead of its target
     *      @param path the file to query
     *      @return the basic information of the file
     *
     */
    function lstatSync(path: string): Class_Stat;

    /**
     * @description Queries the basic information of the given file; unlike stat, when path is a symbolic link, the information of the link itself is returned instead of its target
     *      @param path the file to query
     *      @return the basic information of the file
     *
     */
    function lstatAsync(path: string): Promise<Class_Stat>;

    /**
     * @description Queries the basic information of the given file; unlike stat, when path is a symbolic link, the information of the link itself is returned instead of its target
     *
     *      options supports the following options:
     *      ```JavaScript
     *      {
     *          "throwIfNoEntry": true // whether a nonexistent path throws; returns undefined when false. Default: true
     *      }
     *      ```
     *
     *      `throwIfNoEntry` works like Node.js and only takes effect for synchronous (no callback) calls; the asynchronous form always throws.
     *      @param path the file to query
     *      @param options the query options
     *      @return the basic information of the file, or undefined when `throwIfNoEntry` is false and the path does not exist
     *
     */
    function lstat(path: string, options: FIBJS.GeneralObject): Promise<Class_Stat>;

    /**
     * @description Queries the basic information of the given file; unlike stat, when path is a symbolic link, the information of the link itself is returned instead of its target
     *
     *      options supports the following options:
     *      ```JavaScript
     *      {
     *          "throwIfNoEntry": true // whether a nonexistent path throws; returns undefined when false. Default: true
     *      }
     *      ```
     *
     *      `throwIfNoEntry` works like Node.js and only takes effect for synchronous (no callback) calls; the asynchronous form always throws.
     *      @param path the file to query
     *      @param options the query options
     *      @return the basic information of the file, or undefined when `throwIfNoEntry` is false and the path does not exist
     *
     */
    function lstatSync(path: string, options: FIBJS.GeneralObject): Class_Stat;

    /**
     * @description Queries the basic information of the given file; unlike stat, when path is a symbolic link, the information of the link itself is returned instead of its target
     *
     *      options supports the following options:
     *      ```JavaScript
     *      {
     *          "throwIfNoEntry": true // whether a nonexistent path throws; returns undefined when false. Default: true
     *      }
     *      ```
     *
     *      `throwIfNoEntry` works like Node.js and only takes effect for synchronous (no callback) calls; the asynchronous form always throws.
     *      @param path the file to query
     *      @param options the query options
     *      @return the basic information of the file, or undefined when `throwIfNoEntry` is false and the path does not exist
     *
     */
    function lstatAsync(path: string, options: FIBJS.GeneralObject): Promise<Class_Stat>;

    /**
     * @description Queries the basic information of the given file
     *      @param fd the file descriptor object
     *      @return the basic information of the file
     *
     */
    function fstat(fd: Class_FileHandle | Class_FileHandlePromise): Promise<Class_Stat>;

    /**
     * @description Queries the basic information of the given file
     *      @param fd the file descriptor object
     *      @return the basic information of the file
     *
     */
    function fstatSync(fd: Class_FileHandle | Class_FileHandlePromise): Class_Stat;

    /**
     * @description Queries the basic information of the given file
     *      @param fd the file descriptor object
     *      @return the basic information of the file
     *
     */
    function fstatAsync(fd: Class_FileHandle | Class_FileHandlePromise): Promise<Class_Stat>;

    /**
     * @description Queries the basic information of the given file
     *
     *      options currently has no effective option and is kept for Node.js compatibility only.
     *      @param fd the file descriptor object
     *      @param options the query options
     *      @return the basic information of the file
     *
     */
    function fstat(fd: Class_FileHandle | Class_FileHandlePromise, options: FIBJS.GeneralObject): Promise<Class_Stat>;

    /**
     * @description Queries the basic information of the given file
     *
     *      options currently has no effective option and is kept for Node.js compatibility only.
     *      @param fd the file descriptor object
     *      @param options the query options
     *      @return the basic information of the file
     *
     */
    function fstatSync(fd: Class_FileHandle | Class_FileHandlePromise, options: FIBJS.GeneralObject): Class_Stat;

    /**
     * @description Queries the basic information of the given file
     *
     *      options currently has no effective option and is kept for Node.js compatibility only.
     *      @param fd the file descriptor object
     *      @param options the query options
     *      @return the basic information of the file
     *
     */
    function fstatAsync(fd: Class_FileHandle | Class_FileHandlePromise, options: FIBJS.GeneralObject): Promise<Class_Stat>;

    /**
     * @description Reads the given symbolic link and returns the target path it points to; not supported on Windows
     *      @param path the symbolic link to read
     *      @return the file name the symbolic link points to
     *
     */
    function readlink(path: string): Promise<any>;

    /**
     * @description Reads the given symbolic link and returns the target path it points to; not supported on Windows
     *      @param path the symbolic link to read
     *      @return the file name the symbolic link points to
     *
     */
    function readlinkSync(path: string): any;

    /**
     * @description Reads the given symbolic link and returns the target path it points to; not supported on Windows
     *      @param path the symbolic link to read
     *      @return the file name the symbolic link points to
     *
     */
    function readlinkAsync(path: string): Promise<any>;

    /**
     * @description Reads the given symbolic link and returns the target path it points to; not supported on Windows
     *
     *      options supports the following options, or a string is used as the encoding directly:
     *      ```JavaScript
     *      {
     *          "encoding": "utf8" // the encoding of the returned value; 'buffer' returns a Buffer. Default: utf8
     *      }
     *      ```
     *      @param path the symbolic link to read
     *      @param options the read options, or the encoding of the returned value; 'buffer' returns a Buffer
     *      @return the decoded string when an encoding is given, or a Buffer for 'buffer'
     *
     */
    function readlink(path: string, options: FIBJS.GeneralObject | string): Promise<any>;

    /**
     * @description Reads the given symbolic link and returns the target path it points to; not supported on Windows
     *
     *      options supports the following options, or a string is used as the encoding directly:
     *      ```JavaScript
     *      {
     *          "encoding": "utf8" // the encoding of the returned value; 'buffer' returns a Buffer. Default: utf8
     *      }
     *      ```
     *      @param path the symbolic link to read
     *      @param options the read options, or the encoding of the returned value; 'buffer' returns a Buffer
     *      @return the decoded string when an encoding is given, or a Buffer for 'buffer'
     *
     */
    function readlinkSync(path: string, options: FIBJS.GeneralObject | string): any;

    /**
     * @description Reads the given symbolic link and returns the target path it points to; not supported on Windows
     *
     *      options supports the following options, or a string is used as the encoding directly:
     *      ```JavaScript
     *      {
     *          "encoding": "utf8" // the encoding of the returned value; 'buffer' returns a Buffer. Default: utf8
     *      }
     *      ```
     *      @param path the symbolic link to read
     *      @param options the read options, or the encoding of the returned value; 'buffer' returns a Buffer
     *      @return the decoded string when an encoding is given, or a Buffer for 'buffer'
     *
     */
    function readlinkAsync(path: string, options: FIBJS.GeneralObject | string): Promise<any>;

    /**
     * @description Returns the absolute path of the given path, unfolding relative segments and resolving symbolic links
     *      @param path the path to read
     *      @return the resolved absolute path
     *
     */
    function realpath(path: string): Promise<any>;

    /**
     * @description Returns the absolute path of the given path, unfolding relative segments and resolving symbolic links
     *      @param path the path to read
     *      @return the resolved absolute path
     *
     */
    function realpathSync(path: string): any;

    /**
     * @description Returns the absolute path of the given path, unfolding relative segments and resolving symbolic links
     *      @param path the path to read
     *      @return the resolved absolute path
     *
     */
    function realpathAsync(path: string): Promise<any>;

    /**
     * @description Returns the absolute path of the given path, unfolding relative segments and resolving symbolic links
     *
     *      options supports the following options, or a string is used as the encoding directly:
     *      ```JavaScript
     *      {
     *          "encoding": "utf8" // the encoding of the returned value; 'buffer' returns a Buffer. Default: utf8
     *      }
     *      ```
     *      @param path the path to read
     *      @param options the read options, or the encoding of the returned value; 'buffer' returns a Buffer
     *      @return the decoded string when an encoding is given, or a Buffer for 'buffer'
     *
     */
    function realpath(path: string, options: FIBJS.GeneralObject | string): Promise<any>;

    /**
     * @description Returns the absolute path of the given path, unfolding relative segments and resolving symbolic links
     *
     *      options supports the following options, or a string is used as the encoding directly:
     *      ```JavaScript
     *      {
     *          "encoding": "utf8" // the encoding of the returned value; 'buffer' returns a Buffer. Default: utf8
     *      }
     *      ```
     *      @param path the path to read
     *      @param options the read options, or the encoding of the returned value; 'buffer' returns a Buffer
     *      @return the decoded string when an encoding is given, or a Buffer for 'buffer'
     *
     */
    function realpathSync(path: string, options: FIBJS.GeneralObject | string): any;

    /**
     * @description Returns the absolute path of the given path, unfolding relative segments and resolving symbolic links
     *
     *      options supports the following options, or a string is used as the encoding directly:
     *      ```JavaScript
     *      {
     *          "encoding": "utf8" // the encoding of the returned value; 'buffer' returns a Buffer. Default: utf8
     *      }
     *      ```
     *      @param path the path to read
     *      @param options the read options, or the encoding of the returned value; 'buffer' returns a Buffer
     *      @return the decoded string when an encoding is given, or a Buffer for 'buffer'
     *
     */
    function realpathAsync(path: string, options: FIBJS.GeneralObject | string): Promise<any>;

    /**
     * @description Creates a symbolic link
     *      @param target the target, which may be a file, a directory or a nonexistent path
     *      @param linkpath the symbolic link to create
     *      @param type the type of the symbolic link: 'file', 'dir' or 'junction', 'file' by default; this parameter is only effective on Windows, and for 'junction' the target path linkpath must be absolute, while target is converted to an absolute path automatically.
     *
     */
    function symlink(target: string, linkpath: string, type?: string): Promise<void>;

    /**
     * @description Creates a symbolic link
     *      @param target the target, which may be a file, a directory or a nonexistent path
     *      @param linkpath the symbolic link to create
     *      @param type the type of the symbolic link: 'file', 'dir' or 'junction', 'file' by default; this parameter is only effective on Windows, and for 'junction' the target path linkpath must be absolute, while target is converted to an absolute path automatically.
     *
     */
    function symlinkSync(target: string, linkpath: string, type?: string): void;

    /**
     * @description Creates a symbolic link
     *      @param target the target, which may be a file, a directory or a nonexistent path
     *      @param linkpath the symbolic link to create
     *      @param type the type of the symbolic link: 'file', 'dir' or 'junction', 'file' by default; this parameter is only effective on Windows, and for 'junction' the target path linkpath must be absolute, while target is converted to an absolute path automatically.
     *
     */
    function symlinkAsync(target: string, linkpath: string, type?: string): Promise<void>;

    /**
     * @description Changes the size of a file; when the given length is larger than the source file, it is padded with '\0', otherwise the exceeding content is lost
     *      @param path the path of the file to change
     *      @param len the new size of the file
     *
     */
    function truncate(path: string, len: number): Promise<void>;

    /**
     * @description Changes the size of a file; when the given length is larger than the source file, it is padded with '\0', otherwise the exceeding content is lost
     *      @param path the path of the file to change
     *      @param len the new size of the file
     *
     */
    function truncateSync(path: string, len: number): void;

    /**
     * @description Changes the size of a file; when the given length is larger than the source file, it is padded with '\0', otherwise the exceeding content is lost
     *      @param path the path of the file to change
     *      @param len the new size of the file
     *
     */
    function truncateAsync(path: string, len: number): Promise<void>;

    /**
     * @description Reads the content of a file by its file descriptor
     *
     *      length defaults to 0, which reads no data; a length must be given explicitly to read. position defaults to -1, which reads from the current file position; when position is given, the file pointer is moved there before reading.
     *      @param fd the file descriptor object
     *      @param buffer the Buffer the result is written into
     *      @param offset the write offset in the Buffer, 0 by default
     *      @param length the number of bytes to read, 0 by default
     *      @param position the read position, the current file position by default
     *      @return the number of bytes actually read
     *
     */
    function read(fd: Class_FileHandle | Class_FileHandlePromise, buffer: Class_Buffer, offset?: number, length?: number, position?: number): Promise<number>;

    /**
     * @description Reads the content of a file by its file descriptor
     *
     *      length defaults to 0, which reads no data; a length must be given explicitly to read. position defaults to -1, which reads from the current file position; when position is given, the file pointer is moved there before reading.
     *      @param fd the file descriptor object
     *      @param buffer the Buffer the result is written into
     *      @param offset the write offset in the Buffer, 0 by default
     *      @param length the number of bytes to read, 0 by default
     *      @param position the read position, the current file position by default
     *      @return the number of bytes actually read
     *
     */
    function readSync(fd: Class_FileHandle | Class_FileHandlePromise, buffer: Class_Buffer, offset?: number, length?: number, position?: number): number;

    /**
     * @description Reads the content of a file by its file descriptor
     *
     *      length defaults to 0, which reads no data; a length must be given explicitly to read. position defaults to -1, which reads from the current file position; when position is given, the file pointer is moved there before reading.
     *      @param fd the file descriptor object
     *      @param buffer the Buffer the result is written into
     *      @param offset the write offset in the Buffer, 0 by default
     *      @param length the number of bytes to read, 0 by default
     *      @param position the read position, the current file position by default
     *      @return the number of bytes actually read
     *
     */
    function readAsync(fd: Class_FileHandle | Class_FileHandlePromise, buffer: Class_Buffer, offset?: number, length?: number, position?: number): Promise<number>;

    /**
     * @description Changes the file mode by its file descriptor. Effective on POSIX systems only.
     *      @param fd the file descriptor object
     *      @param mode the file mode
     *
     */
    function fchmod(fd: Class_FileHandle | Class_FileHandlePromise, mode: number): Promise<void>;

    /**
     * @description Changes the file mode by its file descriptor. Effective on POSIX systems only.
     *      @param fd the file descriptor object
     *      @param mode the file mode
     *
     */
    function fchmodSync(fd: Class_FileHandle | Class_FileHandlePromise, mode: number): void;

    /**
     * @description Changes the file mode by its file descriptor. Effective on POSIX systems only.
     *      @param fd the file descriptor object
     *      @param mode the file mode
     *
     */
    function fchmodAsync(fd: Class_FileHandle | Class_FileHandlePromise, mode: number): Promise<void>;

    /**
     * @description Changes the owner by the file descriptor. Effective on POSIX systems only.
     *      @param fd the file descriptor object
     *      @param uid the user id
     *      @param gid the group id
     *
     */
    function fchown(fd: Class_FileHandle | Class_FileHandlePromise, uid: number, gid: number): Promise<void>;

    /**
     * @description Changes the owner by the file descriptor. Effective on POSIX systems only.
     *      @param fd the file descriptor object
     *      @param uid the user id
     *      @param gid the group id
     *
     */
    function fchownSync(fd: Class_FileHandle | Class_FileHandlePromise, uid: number, gid: number): void;

    /**
     * @description Changes the owner by the file descriptor. Effective on POSIX systems only.
     *      @param fd the file descriptor object
     *      @param uid the user id
     *      @param gid the group id
     *
     */
    function fchownAsync(fd: Class_FileHandle | Class_FileHandlePromise, uid: number, gid: number): Promise<void>;

    /**
     * @description Changes the access and modification time of a file by its file descriptor
     *
     *      The time arguments may be a Date object, a Unix timestamp in seconds, or a date string, consistent with Node.js.
     *      @param fd the file descriptor object
     *      @param atime the last access time: a Date object, a Unix timestamp in seconds, or a date string
     *      @param mtime the last modification time: a Date object, a Unix timestamp in seconds, or a date string
     *
     */
    function futimes(fd: Class_FileHandle | Class_FileHandlePromise, atime: any, mtime: any): Promise<void>;

    /**
     * @description Changes the access and modification time of a file by its file descriptor
     *
     *      The time arguments may be a Date object, a Unix timestamp in seconds, or a date string, consistent with Node.js.
     *      @param fd the file descriptor object
     *      @param atime the last access time: a Date object, a Unix timestamp in seconds, or a date string
     *      @param mtime the last modification time: a Date object, a Unix timestamp in seconds, or a date string
     *
     */
    function futimesSync(fd: Class_FileHandle | Class_FileHandlePromise, atime: any, mtime: any): void;

    /**
     * @description Changes the access and modification time of a file by its file descriptor
     *
     *      The time arguments may be a Date object, a Unix timestamp in seconds, or a date string, consistent with Node.js.
     *      @param fd the file descriptor object
     *      @param atime the last access time: a Date object, a Unix timestamp in seconds, or a date string
     *      @param mtime the last modification time: a Date object, a Unix timestamp in seconds, or a date string
     *
     */
    function futimesAsync(fd: Class_FileHandle | Class_FileHandlePromise, atime: any, mtime: any): Promise<void>;

    /**
     * @description Synchronizes data to disk by the file descriptor
     *
     *      Only the file data is synchronized, not the metadata, which costs less than fsync.
     *      @param fd the file descriptor object
     *
     */
    function fdatasync(fd: Class_FileHandle | Class_FileHandlePromise): Promise<void>;

    /**
     * @description Synchronizes data to disk by the file descriptor
     *
     *      Only the file data is synchronized, not the metadata, which costs less than fsync.
     *      @param fd the file descriptor object
     *
     */
    function fdatasyncSync(fd: Class_FileHandle | Class_FileHandlePromise): void;

    /**
     * @description Synchronizes data to disk by the file descriptor
     *
     *      Only the file data is synchronized, not the metadata, which costs less than fsync.
     *      @param fd the file descriptor object
     *
     */
    function fdatasyncAsync(fd: Class_FileHandle | Class_FileHandlePromise): Promise<void>;

    /**
     * @description Synchronizes data to disk by the file descriptor
     *
     *      Synchronizes both the file data and the metadata, making sure the written content is persisted.
     *      @param fd the file descriptor object
     *
     */
    function fsync(fd: Class_FileHandle | Class_FileHandlePromise): Promise<void>;

    /**
     * @description Synchronizes data to disk by the file descriptor
     *
     *      Synchronizes both the file data and the metadata, making sure the written content is persisted.
     *      @param fd the file descriptor object
     *
     */
    function fsyncSync(fd: Class_FileHandle | Class_FileHandlePromise): void;

    /**
     * @description Synchronizes data to disk by the file descriptor
     *
     *      Synchronizes both the file data and the metadata, making sure the written content is persisted.
     *      @param fd the file descriptor object
     *
     */
    function fsyncAsync(fd: Class_FileHandle | Class_FileHandlePromise): Promise<void>;

    /**
     * @description Changes the size of a file by its file descriptor
     *
     *      Consistent with Node.js: a length of 0 empties the file, and negative values are treated as 0.
     *      @param fd the file descriptor object
     *      @param len the new size of the file, 0 by default
     *
     */
    function ftruncate(fd: Class_FileHandle | Class_FileHandlePromise, len?: number): Promise<void>;

    /**
     * @description Changes the size of a file by its file descriptor
     *
     *      Consistent with Node.js: a length of 0 empties the file, and negative values are treated as 0.
     *      @param fd the file descriptor object
     *      @param len the new size of the file, 0 by default
     *
     */
    function ftruncateSync(fd: Class_FileHandle | Class_FileHandlePromise, len?: number): void;

    /**
     * @description Changes the size of a file by its file descriptor
     *
     *      Consistent with Node.js: a length of 0 empties the file, and negative values are treated as 0.
     *      @param fd the file descriptor object
     *      @param len the new size of the file, 0 by default
     *
     */
    function ftruncateAsync(fd: Class_FileHandle | Class_FileHandlePromise, len?: number): Promise<void>;

    /**
     * @description Queries the information of the file system
     *
     *      The returned object contains the type, bsize, blocks, bfree, bavail, files and ffree fields, consistent with Node.js.
     *      @param path the path to query
     *      @return the file system information object
     *
     */
    function statfs(path: string): Promise<{
        type: number;
        bsize: number;
        blocks: number;
        bfree: number;
        bavail: number;
        files: number;
        ffree: number;
    }>;

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
     *      @param path the directory to query
     *      @return the array of directory entries
     *
     */
    function readdir(path: string): Promise<any[]>;

    /**
     * @description Reads the entries of the given directory
     *
     *      Returns an array of file names under the directory, without the content of subdirectories.
     *      @param path the directory to query
     *      @return the array of directory entries
     *
     */
    function readdirSync(path: string): any[];

    /**
     * @description Reads the entries of the given directory
     *
     *      Returns an array of file names under the directory, without the content of subdirectories.
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
    function opendir(path: string): Promise<Class_DirPromise>;

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
     *      {
     *          "recursive": false, // specify whether all subdirectories should be watched or only the current directory
     *          "withFileTypes": false, // specify whether to return DirEntry objects. Default: false
     *          "encoding": "utf8" // specify the encoding of the file names, 'buffer' returns Buffer objects. Default: utf8
     *      }
     *      ```
     *
     *      When withFileTypes is true an array of DirEntry objects is returned, otherwise an array of file names. A string encoding is equivalent to passing it in the options; 'buffer' returns an array of Buffer objects, consistent with Node.js.
     *      @param path the directory to query
     *      @param opts the options, or the encoding of the returned file names
     *      @return the array of directory entries
     *
     */
    function readdir(path: string, opts?: FIBJS.GeneralObject | string): Promise<any[]>;

    /**
     * @description Reads the entries of the given directory
     *
     *      The opts parameter supports the following options, or a string is used as the encoding of the file names directly:
     *      ```JavaScript
     *      {
     *          "recursive": false, // specify whether all subdirectories should be watched or only the current directory
     *          "withFileTypes": false, // specify whether to return DirEntry objects. Default: false
     *          "encoding": "utf8" // specify the encoding of the file names, 'buffer' returns Buffer objects. Default: utf8
     *      }
     *      ```
     *
     *      When withFileTypes is true an array of DirEntry objects is returned, otherwise an array of file names. A string encoding is equivalent to passing it in the options; 'buffer' returns an array of Buffer objects, consistent with Node.js.
     *      @param path the directory to query
     *      @param opts the options, or the encoding of the returned file names
     *      @return the array of directory entries
     *
     */
    function readdirSync(path: string, opts?: FIBJS.GeneralObject | string): any[];

    /**
     * @description Reads the entries of the given directory
     *
     *      The opts parameter supports the following options, or a string is used as the encoding of the file names directly:
     *      ```JavaScript
     *      {
     *          "recursive": false, // specify whether all subdirectories should be watched or only the current directory
     *          "withFileTypes": false, // specify whether to return DirEntry objects. Default: false
     *          "encoding": "utf8" // specify the encoding of the file names, 'buffer' returns Buffer objects. Default: utf8
     *      }
     *      ```
     *
     *      When withFileTypes is true an array of DirEntry objects is returned, otherwise an array of file names. A string encoding is equivalent to passing it in the options; 'buffer' returns an array of Buffer objects, consistent with Node.js.
     *      @param path the directory to query
     *      @param opts the options, or the encoding of the returned file names
     *      @return the array of directory entries
     *
     */
    function readdirAsync(path: string, opts?: FIBJS.GeneralObject | string): Promise<any[]>;

    /**
     * @description Searches the given directory for files matching a name pattern
     *
     *      The opts parameter supports the following options:
     *      ```JavaScript
     *      {
     *          "cwd": "", // specify a different working directory, default to current directory
     *          "withFileTypes": false // specify whether to return Dirent objects. Default: false
     *      }
     *      ```
     *
     *      The pattern supports the `*`, `?`, `**` and other wildcards; the absolute paths of the matching files are returned.
     *      @param pattern the file name pattern
     *      @param opts the options
     *      @return the file list
     *
     */
    function glob(pattern: string, opts?: FIBJS.GeneralObject): Promise<any[]>;

    /**
     * @description Searches the given directory for files matching a name pattern
     *
     *      The opts parameter supports the following options:
     *      ```JavaScript
     *      {
     *          "cwd": "", // specify a different working directory, default to current directory
     *          "withFileTypes": false // specify whether to return Dirent objects. Default: false
     *      }
     *      ```
     *
     *      The pattern supports the `*`, `?`, `**` and other wildcards; the absolute paths of the matching files are returned.
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
     *      {
     *          "cwd": "", // specify a different working directory, default to current directory
     *          "withFileTypes": false // specify whether to return Dirent objects. Default: false
     *      }
     *      ```
     *
     *      The pattern supports the `*`, `?`, `**` and other wildcards; the absolute paths of the matching files are returned.
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
     *      {
     *          "cwd": "", // specify a different working directory, default to current directory
     *          "withFileTypes": false // specify whether to return Dirent objects. Default: false
     *      }
     *      ```
     *
     *      The matches of all patterns are merged; a duplicate file appears only once.
     *      @param patterns the file name patterns
     *      @param opts the options
     *      @return the file list
     *
     */
    function glob(patterns: string[], opts?: FIBJS.GeneralObject): Promise<any[]>;

    /**
     * @description Searches the given directory for files matching a set of name patterns
     *
     *      The opts parameter supports the following options:
     *      ```JavaScript
     *      {
     *          "cwd": "", // specify a different working directory, default to current directory
     *          "withFileTypes": false // specify whether to return Dirent objects. Default: false
     *      }
     *      ```
     *
     *      The matches of all patterns are merged; a duplicate file appears only once.
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
     *      {
     *          "cwd": "", // specify a different working directory, default to current directory
     *          "withFileTypes": false // specify whether to return Dirent objects. Default: false
     *      }
     *      ```
     *
     *      The matches of all patterns are merged; a duplicate file appears only once.
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
     *      {
     *          "flags": "r",      // the open mode, "r" (read only) by default
     *          "start": 0,        // the start position of the read
     *          "end": undefined    // the end position of the read (inclusive), the end of the file by default
     *      }
     *      ```
     *
     *      When start or end is given, the returned stream only covers the [start, end] range (boundaries included).
     *      @param fname the file name
     *      @param options the read options
     *      @return the file stream object
     *
     */
    function createReadStream(fname: string, options?: FIBJS.GeneralObject): Promise<Class_SeekableStreamPromise>;

    /**
     * @description Creates a readable file stream
     *
     *      options supports the following options:
     *      ```JavaScript
     *      {
     *          "flags": "r",      // the open mode, "r" (read only) by default
     *          "start": 0,        // the start position of the read
     *          "end": undefined    // the end position of the read (inclusive), the end of the file by default
     *      }
     *      ```
     *
     *      When start or end is given, the returned stream only covers the [start, end] range (boundaries included).
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
     *      {
     *          "flags": "r",      // the open mode, "r" (read only) by default
     *          "start": 0,        // the start position of the read
     *          "end": undefined    // the end position of the read (inclusive), the end of the file by default
     *      }
     *      ```
     *
     *      When start or end is given, the returned stream only covers the [start, end] range (boundaries included).
     *      @param fname the file name
     *      @param options the read options
     *      @return the file stream object
     *
     */
    function createReadStreamAsync(fname: string, options?: FIBJS.GeneralObject): Promise<Class_SeekableStreamPromise>;

    /**
     * @description Opens a file and creates a writable stream
     *      @param fname the file name
     *      @param options the write options, supporting flags ('w' by default)
     *      @return the file stream object
     *
     */
    function createWriteStream(fname: string, options?: FIBJS.GeneralObject): Promise<Class_SeekableStreamPromise>;

    /**
     * @description Opens a file and creates a writable stream
     *      @param fname the file name
     *      @param options the write options, supporting flags ('w' by default)
     *      @return the file stream object
     *
     */
    function createWriteStreamSync(fname: string, options?: FIBJS.GeneralObject): Class_SeekableStream;

    /**
     * @description Opens a file and creates a writable stream
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
     *      @param fname the file name
     *      @param flags the open mode: integer fs.constants flags, or a string, "r" (read only) by default
     *      @return the opened file object
     *
     */
    function openFile(fname: string, flags?: string | number): Promise<Class_SeekableStreamPromise>;

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
     *      @param fname the file name
     *      @param flags the open mode: integer fs.constants flags, or a string, "r" (read only) by default
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
     *      @param fname the file name
     *      @param flags the open mode: integer fs.constants flags, or a string, "r" (read only) by default
     *      @return the opened file object
     *
     */
    function openFileAsync(fname: string, flags?: string | number): Promise<Class_SeekableStreamPromise>;

    /**
     * @description Opens a file descriptor, using integer fs.constants flags
     *
     *      @param fname the file name
     *      @param flags integer flags, a combination of fs.constants values (such as fs.constants.O_WRONLY | fs.constants.O_CREAT)
     *      @param mode the file mode when the file is created, 0666 by default
     *      @return the opened file descriptor
     *
     */
    function open(fname: string, flags: number, mode?: number): Promise<Class_FileHandlePromise>;

    /**
     * @description Opens a file descriptor, using integer fs.constants flags
     *
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
    function open(fname: string, flags: string, mode: any): Promise<Class_FileHandlePromise>;

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
    function open(fname: string, flags?: string, mode?: number): Promise<Class_FileHandlePromise>;

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
     *      @param fd the file descriptor object
     *
     */
    function close(fd: Class_FileHandle | Class_FileHandlePromise): Promise<void>;

    /**
     * @description Closes the file descriptor
     *      @param fd the file descriptor object
     *
     */
    function closeSync(fd: Class_FileHandle | Class_FileHandlePromise): void;

    /**
     * @description Closes the file descriptor
     *      @param fd the file descriptor object
     *
     */
    function closeAsync(fd: Class_FileHandle | Class_FileHandlePromise): Promise<void>;

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
    function openTextStream(fname: string, flags?: string): Promise<Class_BufferedStreamPromise>;

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
    function readTextFile(fname: string): Promise<string>;

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
     *      {
     *          "encoding": "utf8" // specify the encoding, default is utf8.
     *      }
     *      ```
     *
     *      An encoding string is empty by default, nothing is decoded and a Buffer object is returned; when an encoding is given, the decoded string is returned. Consistent with Node.js: a file descriptor is not closed after reading and the current file position is not changed.
     *      @param fname the file name, or the file descriptor object
     *      @param options the decoding, or the read options; a descriptor read with an options object defaults to utf8, the other forms return a Buffer unless an encoding is given
     *      @return the file content
     *
     */
    function readFile(fname: Class_FileHandle | Class_FileHandlePromise | string, options?: FIBJS.GeneralObject | string): Promise<any>;

    /**
     * @description Reads the whole content of a file, by its file descriptor or by its name
     *
     *      options supports the following options:
     *      ```JavaScript
     *      {
     *          "encoding": "utf8" // specify the encoding, default is utf8.
     *      }
     *      ```
     *
     *      An encoding string is empty by default, nothing is decoded and a Buffer object is returned; when an encoding is given, the decoded string is returned. Consistent with Node.js: a file descriptor is not closed after reading and the current file position is not changed.
     *      @param fname the file name, or the file descriptor object
     *      @param options the decoding, or the read options; a descriptor read with an options object defaults to utf8, the other forms return a Buffer unless an encoding is given
     *      @return the file content
     *
     */
    function readFileSync(fname: Class_FileHandle | Class_FileHandlePromise | string, options?: FIBJS.GeneralObject | string): any;

    /**
     * @description Reads the whole content of a file, by its file descriptor or by its name
     *
     *      options supports the following options:
     *      ```JavaScript
     *      {
     *          "encoding": "utf8" // specify the encoding, default is utf8.
     *      }
     *      ```
     *
     *      An encoding string is empty by default, nothing is decoded and a Buffer object is returned; when an encoding is given, the decoded string is returned. Consistent with Node.js: a file descriptor is not closed after reading and the current file position is not changed.
     *      @param fname the file name, or the file descriptor object
     *      @param options the decoding, or the read options; a descriptor read with an options object defaults to utf8, the other forms return a Buffer unless an encoding is given
     *      @return the file content
     *
     */
    function readFileAsync(fname: Class_FileHandle | Class_FileHandlePromise | string, options?: FIBJS.GeneralObject | string): Promise<any>;

    /**
     * @description Opens a file and reads a set of text lines into an array; the line ending follows the EOL property: "\n" on posix and "\r\n" on windows by default
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
     *      @param fd the file descriptor object
     *      @param buffer the Buffer object to write
     *      @param offset the read offset in the Buffer, 0 by default
     *      @param length the number of bytes to write, -1 by default
     *      @param position the write position, the current file position by default
     *      @return the number of bytes actually written
     *
     */
    function write(fd: Class_FileHandle | Class_FileHandlePromise, buffer: Class_Buffer, offset?: number, length?: number, position?: number): Promise<number>;

    /**
     * @description Writes content into a file by its file descriptor
     *
     *      length defaults to -1, which writes all the remaining data of buffer from offset. position defaults to -1, which writes from the current file position.
     *      @param fd the file descriptor object
     *      @param buffer the Buffer object to write
     *      @param offset the read offset in the Buffer, 0 by default
     *      @param length the number of bytes to write, -1 by default
     *      @param position the write position, the current file position by default
     *      @return the number of bytes actually written
     *
     */
    function writeSync(fd: Class_FileHandle | Class_FileHandlePromise, buffer: Class_Buffer, offset?: number, length?: number, position?: number): number;

    /**
     * @description Writes content into a file by its file descriptor
     *
     *      length defaults to -1, which writes all the remaining data of buffer from offset. position defaults to -1, which writes from the current file position.
     *      @param fd the file descriptor object
     *      @param buffer the Buffer object to write
     *      @param offset the read offset in the Buffer, 0 by default
     *      @param length the number of bytes to write, -1 by default
     *      @param position the write position, the current file position by default
     *      @return the number of bytes actually written
     *
     */
    function writeAsync(fd: Class_FileHandle | Class_FileHandlePromise, buffer: Class_Buffer, offset?: number, length?: number, position?: number): Promise<number>;

    /**
     * @description Writes content into a file by its file descriptor
     *
     *      position defaults to -1, which writes from the current file position. The string is encoded with encoding before writing.
     *      @param fd the file descriptor object
     *      @param string the string to write
     *      @param position the write position, the current file position by default
     *      @param encoding the decoding, utf8 by default
     *      @return the number of bytes actually written
     *
     */
    function write(fd: Class_FileHandle | Class_FileHandlePromise, string: string, position?: number, encoding?: string): Promise<number>;

    /**
     * @description Writes content into a file by its file descriptor
     *
     *      position defaults to -1, which writes from the current file position. The string is encoded with encoding before writing.
     *      @param fd the file descriptor object
     *      @param string the string to write
     *      @param position the write position, the current file position by default
     *      @param encoding the decoding, utf8 by default
     *      @return the number of bytes actually written
     *
     */
    function writeSync(fd: Class_FileHandle | Class_FileHandlePromise, string: string, position?: number, encoding?: string): number;

    /**
     * @description Writes content into a file by its file descriptor
     *
     *      position defaults to -1, which writes from the current file position. The string is encoded with encoding before writing.
     *      @param fd the file descriptor object
     *      @param string the string to write
     *      @param position the write position, the current file position by default
     *      @param encoding the decoding, utf8 by default
     *      @return the number of bytes actually written
     *
     */
    function writeAsync(fd: Class_FileHandle | Class_FileHandlePromise, string: string, position?: number, encoding?: string): Promise<number>;

    /**
     * @description Creates a text file and writes content into it
     *
     *      The file is opened for overwriting; existing content is truncated.
     *      @param fname the file name
     *      @param txt the string to write
     *      @return the number of bytes actually written
     *
     */
    function writeTextFile(fname: string, txt: string): Promise<number>;

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
     *      {
     *          "encoding": "utf8", // specify the encoding, default is utf8.
     *          "mode": 0666, // specify the file mode. Default: 0666
     *          "flag": "w" // specify the open flag. Default: w
     *      }
     *      ```
     *
     *      A file descriptor ignores the options object and encodes text data as utf8.
     *      @param fname the file name, or the file descriptor object
     *      @param data the data to write
     *      @param opt the encoding of text data, utf8 by default, or the write options
     *      @return the number of bytes actually written
     *
     */
    function writeFile(fname: Class_FileHandle | Class_FileHandlePromise | string, data: Class_Buffer | string, opt?: FIBJS.GeneralObject | string): Promise<number>;

    /**
     * @description Writes content into a file, by its file descriptor or by its name
     *
     *      The file is opened for overwriting, existing content is truncated. opt is the encoding of text data, utf8 by default; an options object carries the write options instead:
     *      ```JavaScript
     *      {
     *          "encoding": "utf8", // specify the encoding, default is utf8.
     *          "mode": 0666, // specify the file mode. Default: 0666
     *          "flag": "w" // specify the open flag. Default: w
     *      }
     *      ```
     *
     *      A file descriptor ignores the options object and encodes text data as utf8.
     *      @param fname the file name, or the file descriptor object
     *      @param data the data to write
     *      @param opt the encoding of text data, utf8 by default, or the write options
     *      @return the number of bytes actually written
     *
     */
    function writeFileSync(fname: Class_FileHandle | Class_FileHandlePromise | string, data: Class_Buffer | string, opt?: FIBJS.GeneralObject | string): number;

    /**
     * @description Writes content into a file, by its file descriptor or by its name
     *
     *      The file is opened for overwriting, existing content is truncated. opt is the encoding of text data, utf8 by default; an options object carries the write options instead:
     *      ```JavaScript
     *      {
     *          "encoding": "utf8", // specify the encoding, default is utf8.
     *          "mode": 0666, // specify the file mode. Default: 0666
     *          "flag": "w" // specify the open flag. Default: w
     *      }
     *      ```
     *
     *      A file descriptor ignores the options object and encodes text data as utf8.
     *      @param fname the file name, or the file descriptor object
     *      @param data the data to write
     *      @param opt the encoding of text data, utf8 by default, or the write options
     *      @return the number of bytes actually written
     *
     */
    function writeFileAsync(fname: Class_FileHandle | Class_FileHandlePromise | string, data: Class_Buffer | string, opt?: FIBJS.GeneralObject | string): Promise<number>;

    /**
     * @description Appends content to a file, by its file descriptor or by its name
     *
     *      The file is created when it does not exist. options is the encoding of the data to append; an options object carries the write options instead:
     *      ```JavaScript
     *      {
     *          "encoding": "utf8", // specify the encoding of string data. Default: utf8
     *          "mode": 0666, // specify the file mode. Default: 0666
     *          "flag": "a" // specify the open flag. Default: a
     *      }
     *      ```
     *      Consistent with Node.js, `flag` defaults to 'a' (append) and may be 'w'/'wx'/'ax' and so on. The encoding of an options object only validates the label, the data is appended as it is; a file descriptor ignores the options object and appends string data as utf8.
     *      @param fname the file name, or the file descriptor object
     *      @param data the data to write
     *      @param options the encoding of the data, or the write options
     *      @return the number of bytes actually written
     *
     */
    function appendFile(fname: Class_FileHandle | Class_FileHandlePromise | string, data: Class_Buffer | string, options?: FIBJS.GeneralObject | string): Promise<number>;

    /**
     * @description Appends content to a file, by its file descriptor or by its name
     *
     *      The file is created when it does not exist. options is the encoding of the data to append; an options object carries the write options instead:
     *      ```JavaScript
     *      {
     *          "encoding": "utf8", // specify the encoding of string data. Default: utf8
     *          "mode": 0666, // specify the file mode. Default: 0666
     *          "flag": "a" // specify the open flag. Default: a
     *      }
     *      ```
     *      Consistent with Node.js, `flag` defaults to 'a' (append) and may be 'w'/'wx'/'ax' and so on. The encoding of an options object only validates the label, the data is appended as it is; a file descriptor ignores the options object and appends string data as utf8.
     *      @param fname the file name, or the file descriptor object
     *      @param data the data to write
     *      @param options the encoding of the data, or the write options
     *      @return the number of bytes actually written
     *
     */
    function appendFileSync(fname: Class_FileHandle | Class_FileHandlePromise | string, data: Class_Buffer | string, options?: FIBJS.GeneralObject | string): number;

    /**
     * @description Appends content to a file, by its file descriptor or by its name
     *
     *      The file is created when it does not exist. options is the encoding of the data to append; an options object carries the write options instead:
     *      ```JavaScript
     *      {
     *          "encoding": "utf8", // specify the encoding of string data. Default: utf8
     *          "mode": 0666, // specify the file mode. Default: 0666
     *          "flag": "a" // specify the open flag. Default: a
     *      }
     *      ```
     *      Consistent with Node.js, `flag` defaults to 'a' (append) and may be 'w'/'wx'/'ax' and so on. The encoding of an options object only validates the label, the data is appended as it is; a file descriptor ignores the options object and appends string data as utf8.
     *      @param fname the file name, or the file descriptor object
     *      @param data the data to write
     *      @param options the encoding of the data, or the write options
     *      @return the number of bytes actually written
     *
     */
    function appendFileAsync(fname: Class_FileHandle | Class_FileHandlePromise | string, data: Class_Buffer | string, options?: FIBJS.GeneralObject | string): Promise<number>;

    /**
     * @description Sets a zip virtual file mapping; a string data is encoded as utf8
     *
     *      The zip data is mapped onto the given path; file accesses to that path are then read from the mapped zip.
     *      @param fname the mapping path, a string is encoded as utf8
     *      @param data the zip data to map, a string is encoded as utf8
     *
     */
    function setZipFS(fname: string, data: Class_Buffer | string): void;

    /**
     * @description Clears zip virtual file mappings
     *      @param fname the mapping path, all caches are cleared by default
     *
     */
    function clearZipFS(fname?: string): void;

    /**
     * @description Watches a file and returns the corresponding watcher object
     *      @param fname the file to watch
     *      @return the FSWatcher object
     *
     */
    function watch(fname: string): Class_FSWatcher;

    /**
     * @description Watches a file and returns the corresponding watcher object
     *      @param fname the file to watch
     *      @param callback `(evtType: 'change' | 'rename', filename: string) => any` the handler called when the file changes
     *      @return the FSWatcher object
     *
     */
    function watch(fname: string, callback: (eventType: string, filename: string | Class_Buffer)=>void): Class_FSWatcher;

    /**
     * @description Watches a file and returns the corresponding watcher object
     *
     *      options supports the following options:
     *      ```JavaScript
     *      {
     *          "persistent": true, // specify whether the process should continue to run as long as files are being watched
     *          "recursive": false, // specify whether all subdirectories should be watched or only the current directory
     *          "encoding": "utf8", // specify the encoding, default is utf8.
     *      }
     *      ```
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
     *      {
     *          "persistent": true, // specify whether the process should continue to run as long as files are being watched
     *          "recursive": false, // specify whether all subdirectories should be watched or only the current directory
     *          "encoding": "utf8", // specify the encoding, default is utf8.
     *      }
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
     *      The file status is checked periodically; the callback is called when it changes, receiving the Stat objects before and after the change.
     *      @param fname the file to watch
     *      @param callback `(curStats: Stats, prevStats: Stats) => any` the handler called when the stats of the file change
     *      @return the StatsWatcher object
     *
     */
    function watchFile(fname: string, callback: (curStats: Class_Stat, prevStats: Class_Stat)=>void): Class_StatsWatcher;

    /**
     * @description Watches a file and returns the corresponding StatsWatcher object
     *
     *      options supports the following options:
     *      ```JavaScript
     *      {
     *          "persistent": true, // specify whether the process should continue to run as long as files are being watched
     *          "bigint": false, // specify whether the numeric values in the returned Stat objects should be bigint. Default: false
     *          "interval": 100 // specify the time interval in milliseconds at which the file's stats should be polled. Default: 100
     *      }
     *      ```
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


declare module "fs" {
    const promises: typeof import("fs/promises");
}
