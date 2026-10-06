/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description A directory entry: the name and the type of one item inside a directory
 *
 *  A DirEntry answers "what is in this directory" without a stat call per item: the directory
 *  listing already reports the type of each entry, so the isXxx() predicates are free. Use it
 *  when files, directories and links must be told apart while listing; reach for Stat when size,
 *  timestamps, permissions or the target of a link are needed, because a DirEntry carries none
 *  of them.
 *
 *  Concepts:
 *
 *  - **Type from the listing**: the isXxx() predicates test the entry type reported by the
 *    directory scan (S_IF* bits), not the result of an extra stat call. A file system that
 *    reports the type as unknown makes every predicate false; on Windows only the file,
 *    directory and symbolic link types are populated.
 *  - **Paths**: name is the base name of the entry; parentPath is the directory that was
 *    scanned, spelled the way the scan was requested. fs.readdir and Dir keep the requested
 *    path (a relative path stays relative), while fs.glob with `withFileTypes` always reports
 *    the absolute directory of each match.
 *  - **Links are not followed**: isSymbolicLink() is true for the link itself and false for a
 *    link to a file or directory, exactly like fs.lstat; use fs.stat to describe the target.
 *  - **Not constructible**: the class is exposed as `fs.Dirent`; `new fs.Dirent()` throws. It
 *    is a plain value object, not a handle, so there is nothing to close.
 *
 *  Obtained from:
 *  - `fs.readdir(path, { withFileTypes: true })` — the elements of the returned array;
 *  - `fs.glob(pattern, { withFileTypes: true })` — the elements of the returned array;
 *  - `Dir#read()` and iteration over a Dir obtained from `fs.opendir` or `new fs.Dir(path)`.
 *
 *  Example 1 — classify the entries of a directory without stat calls:
 *  ```JavaScript
 *  const fs = require('fs');
 *  const os = require('os');
 *  const path = require('path');
 *
 *  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-dirent-'));
 *  fs.writeFile(path.join(dir, 'notes.txt'), 'note');
 *  fs.mkdir(path.join(dir, 'images'));
 *  fs.symlink(path.join(dir, 'notes.txt'), path.join(dir, 'shortcut'));
 *
 *  fs.readdir(dir, { withFileTypes: true }).forEach((entry) => {
 *      let kind = 'file';
 *      if (entry.isDirectory()) kind = 'dir';
 *      else if (entry.isSymbolicLink()) kind = 'link';
 *      console.log(entry.name, kind, entry.parentPath);
 *  });
 *
 *  fs.rmSync(dir, { recursive: true, force: true });
 *  ```
 *
 *  Example 2 — walk a tree with glob and never stat a match:
 *  ```JavaScript
 *  const fs = require('fs');
 *  const os = require('os');
 *  const path = require('path');
 *
 *  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-dirent-'));
 *  fs.mkdir(path.join(dir, 'src'));
 *  fs.writeFile(path.join(dir, 'src', 'app.js'), 'app');
 *  fs.writeFile(path.join(dir, 'README.md'), 'readme');
 *
 *  fs.glob('**', { cwd: dir, withFileTypes: true }).forEach((entry) => {
 *      console.log(entry.name, entry.isDirectory() ? 'dir' : 'file');
 *  });
 *
 *  fs.rmSync(dir, { recursive: true, force: true });
 *  ```
 *
 *  Same object type as Node.js fs.Dirent; Node also has the deprecated `path` property, fibjs
 *  provides parentPath only.
 *
 */
declare class Class_DirEntry extends Class_object {
    /**
     * @description Base name of the entry
     *
     *      The last path component, without the directory part: 'report.txt' for an entry of
     *      '/data/report.txt'. It is a name inside the directory, not a path; combine it with
     *      parentPath to build the full path. Same property as Node.js Dirent#name.
     *
     */
    readonly name: string;

    /**
     * @description Directory that contains the entry
     *
     *      The directory path that was scanned, spelled as it was requested: fs.readdir('.')
     *      reports '.', a relative Dir reports its relative path, and fs.glob with
     *      `withFileTypes` reports the absolute directory of each match (even for a relative
     *      pattern). Node.js Dirent also exposes this property; the older Node `path` alias is not
     *      provided.
     *
     */
    readonly parentPath: string;

    /**
     * @description Queries whether the entry is a block device
     *
     *      Tests the S_IFBLK type from the directory listing. Always false on Windows, where the
     *      listing does not report this type; no stat call is made.
     *      @return true if it describes a block device
     *
     */
    isBlockDevice(): boolean;

    /**
     * @description Queries whether the entry is a character device
     *
     *      Tests the S_IFCHR type from the directory listing; character devices include terminals
     *      and serial ports. Always false on Windows.
     *      @return true if it describes a character device
     *
     */
    isCharacterDevice(): boolean;

    /**
     * @description Queries whether the entry is a directory
     *
     *      Tests the S_IFDIR type from the directory listing. A symbolic link to a directory is
     *      reported as a link, not as a directory (see isSymbolicLink); use fs.stat when the target
     *      of a link is needed.
     *      @return true if it is a directory
     *
     */
    isDirectory(): boolean;

    /**
     * @description Queries whether the entry is a FIFO pipe
     *
     *      Tests the S_IFIFO type from the directory listing. Always false on Windows.
     *      @return true if it describes a FIFO pipe
     *
     */
    isFIFO(): boolean;

    /**
     * @description Queries whether the entry is a regular file
     *
     *      Tests the S_IFREG type from the directory listing; directories, devices and links are
     *      not files. The entry is not followed, so a link to a file reports false.
     *      @return true if it is a file
     *
     */
    isFile(): boolean;

    /**
     * @description Queries whether the entry is a symbolic link
     *
     *      Tests the S_IFLNK type from the directory listing; true for the link itself, matching
     *      lstat, and false for its target. On Windows the link type is populated for reparse
     *      points.
     *
     *      Example — find the symbolic links of a directory:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-link-'));
     *      fs.writeFile(path.join(dir, 'target.txt'), 'x');
     *      fs.symlink(path.join(dir, 'target.txt'), path.join(dir, 'link.txt'));
     *
     *      const link = fs.readdir(dir, { withFileTypes: true })
     *          .find((entry) => entry.name === 'link.txt');
     *      console.log(link.isSymbolicLink(), link.isFile()); // true false
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @return true if it is a symbolic link
     *
     */
    isSymbolicLink(): boolean;

    /**
     * @description Queries whether the entry is a UNIX domain socket
     *
     *      Tests the S_IFSOCK type from the directory listing. Always false on Windows.
     *      @return true if it is a Socket
     *
     */
    isSocket(): boolean;

}

