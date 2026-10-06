/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description The path module provides utilities for working with file and directory paths; it is
 *  platform-aware and keeps path handling out of ad-hoc string concatenation
 *
 *  Main capabilities:
 *
 *  - **Building paths**: `join`, `resolve`, `normalize`, `fullpath` (fibjs extension);
 *  - **Breaking paths down**: `parse`, `format`, `basename`, `dirname`, `extname`;
 *  - **Comparing paths**: `relative`, `isAbsolute`, `matchesGlob`;
 *  - **Platform rule sets**: `posix`, `win32` and the `sep` / `delimiter` constants.
 *
 *  Concepts:
 *
 *  - **Two rule sets, one API**: these functions never touch the file system. The POSIX rule set
 *    uses '/' as the separator and treats '\' as an ordinary character; the Windows rule set
 *    accepts both '/' and '\', and recognises drive letters, UNC shares and device namespaces.
 *    The `posix` and `win32` properties expose both rule sets on every platform, so parsing a
 *    foreign path format does not depend on the host; on POSIX hosts the module itself applies
 *    the posix rules (`path === path.posix`), on Windows the win32 rules.
 *  - **join vs resolve vs fullpath**: `join` merges segments with the rule set separator and then
 *    normalizes; a later absolute segment is appended as an ordinary segment (`join('a', '/b')`
 *    is `'a/b'`). `resolve` works from right to left and restarts at the rightmost absolute
 *    segment, returning an absolute path anchored at the working directory. `fullpath` is a
 *    fibjs extension that anchors a relative path to the working directory and normalizes it,
 *    without touching the file system and without resolving symbolic links.
 *  - **Normalization**: '.' segments are dropped, '..' cancels the previous segment where
 *    possible, repeated separators collapse, and a trailing separator is preserved (except on
 *    the root). Relative paths keep leading '..' segments; absolute paths are clamped at the
 *    root. Normalizing an empty string returns '.'.
 *  - **Empty strings and roots**: `join()` and `join('')` return '.'; `resolve('')` returns the
 *    working directory; `basename('/')` and `basename('')` are ''; `dirname('foo')` and
 *    `dirname('')` are '.', while `dirname('/')` is '/'.
 *  - **Extensions and dotfiles**: `extname` returns the text from the last dot of the last
 *    segment, so '.bashrc' has no extension, '.env.local' has '.local' and 'index.' has '.'.
 *    `basename(path, ext)` strips `ext` as a plain suffix, which does not need to start with a
 *    dot.
 *  - **Windows specifics**: the win32 rule set recognises drive-absolute ('C:\'),
 *    drive-relative ('C:'), UNC ('\\server\share') and device ('\\?\C:\') forms;
 *    `isAbsolute('C:')` is false. Glob matching compares drive letters case-insensitively and
 *    paths on different drives never match. `toNamespacedPath` adds the '\\?\' prefix only
 *    under the win32 rule set.
 *  - **sep and delimiter**: `sep` separates path segments ('/' or '\'), while `delimiter`
 *    separates entries in PATH-style lists (':' or ';'). Both belong to the rule set, not to the
 *    host operating system.
 *
 *  Import:
 *  ```JavaScript
 *  const path = require('path');
 *  ```
 *
 *  Example 1 — joining, resolving and comparing paths:
 *  ```JavaScript
 *  const path = require('path');
 *
 *  // join() concatenates segments with the platform separator
 *  console.log(path.join('src', 'app', '..', 'index.js')); // src/index.js
 *
 *  // resolve() builds an absolute path; the rightmost absolute segment wins
 *  console.log(path.resolve('/srv', 'www', '/etc')); // /etc
 *
 *  // relative() computes how to get from one path to another
 *  console.log(path.relative('/srv/www', '/srv/log/app.log')); // ../log/app.log
 *  console.log(path.relative('/srv/www', '/srv/www')); // ''
 *  ```
 *
 *  Example 2 — parsing paths and extracting names:
 *  ```JavaScript
 *  const path = require('path');
 *
 *  const info = path.parse('/var/log/app.tar.gz');
 *  console.log(info.dir, info.name, info.ext); // /var/log app.tar .gz
 *
 *  // format() accepts the object returned by parse()
 *  console.log(path.format(info)); // /var/log/app.tar.gz
 *
 *  // a leading dot starts a dotfile, not an extension
 *  console.log(path.extname('/home/user/.bashrc')); // ''
 *
 *  // trailing separators are ignored when a name is extracted
 *  console.log(path.basename('/foo/bar/')); // bar
 *  console.log(path.dirname('/foo/bar/'));  // /foo
 *  ```
 *
 *  Example 3 — one input through the three rule sets:
 *  ```JavaScript
 *  const path = require('path');
 *  const posix = require('path/posix');
 *  const win32 = require('path/win32');
 *
 *  const input = 'C:\\temp\\\\foo\\..';
 *
 *  // the platform default applies the rules of the current system
 *  console.log(JSON.stringify(path.normalize(input)));  // "C:\\temp\\\\foo\\.."
 *
 *  // the posix rule set treats the backslash as an ordinary character
 *  console.log(JSON.stringify(posix.normalize(input))); // "C:\\temp\\\\foo\\.."
 *
 *  // the win32 rule set recognises the drive and cancels foo\..
 *  console.log(JSON.stringify(win32.normalize(input))); // "C:\\temp"
 *  ```
 *
 *  Notes:
 *
 *  - Node.js offers the same API. fibjs adds `fullpath`; `path.win32.fullpath` is implemented
 *    only on Windows and throws elsewhere.
 *  - `toNamespacedPath` is more tolerant than Node.js: a non-string value is returned unchanged
 *    and `null` becomes `undefined`.
 *  - `path/posix` and `path/win32` are the runtime entry points of the posix and win32 rule
 *    sets, matching the Node.js subpath requires; `path_posix` / `path_win32` are only the names
 *    of their manual pages.
 *
 */
declare module 'path' {
    /**
     * @description Normalizes a path, resolving '.' and '..' segments and collapsing repeated separators
     *
     *      A pure string transformation: the path does not need to exist and no file system access is
     *      performed. Relative paths keep leading '..' segments; absolute paths are clamped at the
     *      root, so '/../' becomes '/'. A trailing separator is preserved ('a//b/' becomes 'a/b/'),
     *      except on the root. In the posix rule set a backslash is an ordinary character; in the
     *      win32 rule set both '/' and '\' separate segments and drive letters and UNC roots are
     *      preserved. An empty string becomes '.'.
     *
     *      Example — normalize mixed segments:
     *      ```JavaScript
     *      const path = require('path');
     *
     *      console.log(path.normalize('a//b/../c'));  // a/c
     *      console.log(path.normalize('/../a/b/..')); // /a
     *      console.log(path.normalize(''));           // .
     *      console.log(path.normalize('..'));         // ..
     *      ```
     *
     *      @param path the path to normalize
     *      @return the normalized path
     *
     */
    function normalize(path: string): string;

    /**
     * @description Returns the last portion of a path, removing a matching extension when ext is given
     *
     *      Trailing separators are ignored, so basename('/foo/bar/') is 'bar' and basename('/') is ''.
     *      The optional ext is stripped as a plain suffix of the result, not necessarily starting with
     *      a dot: basename('/a/b.txt', 'txt') returns 'b.'. When ext does not match, the name is
     *      returned unchanged. The separator rules follow the module rule set: in the posix rule set a
     *      backslash is an ordinary character, while the win32 rule set treats it as a separator.
     *      Same name in Node.js.
     *
     *      @param path the path to query
     *      @param ext the extension to remove when the file name matches
     *      @return the file name
     *
     */
    function basename(path: string, ext?: string): string;

    /**
     * @description Returns the extension of the file in a path, from the last '.' in the last segment
     *
     *      A leading dot starts a dotfile, not an extension: extname('.bashrc') is '', while
     *      extname('.env.local') is '.local'. extname('index.') is '.', and the result is '' for '..'
     *      and for paths that end with a separator. A dot in a parent directory name is ignored. In
     *      the posix rule set a backslash is an ordinary character, in the win32 rule set it
     *      separates segments. Same name in Node.js.
     *
     *      @param path the path to query
     *      @return the extension
     *
     */
    function extname(path: string): string;

    /**
     * @description Formats a path object into a path string, the inverse of parse
     *
     *      All fields are optional; typically either dir or root is used together with base, or with
     *      name and ext. Base wins over name + ext when both are present, and dir wins over root
     *      unless dir is empty. When dir equals root no separator is inserted between them; otherwise
     *      the rule set separator joins dir and base. A dir-only object produces a trailing separator
     *      ('/a' becomes '/a/') and an empty object produces ''. Same name in Node.js.
     *
     *      pathObject supports the following properties:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "root": "/",     // the root of the path, e.g. '/' or 'C:\\'
     *          "dir": "/a/b",   // the directory; wins over root when both are set
     *          "base": "c.ext", // the full last segment; wins over name + ext
     *          "ext": ".ext",   // the extension, including the leading dot
     *          "name": "c"      // the name without the extension
     *      })
     *      ```
     *
     *      Example — build a path from a parsed object and from scratch:
     *      ```JavaScript
     *      const path = require('path');
     *
     *      const parts = path.parse('/srv/www/index.html');
     *      console.log(path.format(parts)); // /srv/www/index.html
     *
     *      console.log(path.format({ dir: '/a', name: 'b', ext: '.txt' })); // /a/b.txt
     *      console.log(path.format({ root: '/', name: 'b', ext: '.txt' })); // /b.txt
     *      console.log(path.format({ dir: '/a' }));                         // /a/
     *      console.log(path.format({}));                                    // ''
     *      ```
     *
     *      @param pathObject the path object
     *      @return the formatted path
     *
     */
    function format(pathObject: FIBJS.GeneralObject): string;

    /**
     * @description Parses a path into an object with root, dir, base, ext and name fields
     *
     *      The object always contains all five fields as strings, so format(parse(p)) rebuilds the
     *      path. A leading '/' sets root to '/'; a trailing separator is ignored for base; a leading
     *      dot starts a dotfile ('.bashrc' has base and name '.bashrc' and no extension), while
     *      'index.' has ext '.'. The win32 rule set additionally recognises drive letters ('C:'),
     *      drive-absolute paths ('C:\'), UNC shares ('\\server\share\') and '\\?\' device paths,
     *      accepting both separators. An empty string produces five empty fields. Same name in
     *      Node.js.
     *
     *      Example — inspect a parsed path:
     *      ```JavaScript
     *      const path = require('path');
     *
     *      const parts = path.parse('/srv/www/index.html');
     *      console.log(parts.root, parts.dir, parts.base); // / /srv/www index.html
     *      console.log(parts.ext, parts.name);             // .html index
     *
     *      console.log(path.parse('.bashrc').ext); // ''
     *      ```
     *
     *      @param path the path to parse
     *      @return the parsed path object
     *
     */
    function parse(path: string): {
        root: string;
        dir: string;
        base: string;
        ext: string;
        name: string;
    };

    /**
     * @description Returns the directory name of a path, dropping the last segment
     *
     *      Trailing separators are ignored: dirname('/foo/bar/') is '/foo', dirname('/foo') is '/',
     *      dirname('foo') is '.', dirname('') is '.' and dirname('/') is '/'. In the win32 rule set
     *      the root is kept for drive-absolute paths ('C:\foo' becomes 'C:\') and UNC shares, while
     *      a drive-relative path keeps its drive ('c:foo' becomes 'c:'). Same name in Node.js.
     *
     *      @param path the path to query
     *      @return the directory name
     *
     */
    function dirname(path: string): string;

    /**
     * @description Converts a path into an absolute path anchored at the current working directory
     *
     *      A fibjs extension, not part of Node.js. An absolute input is normalized; a relative input
     *      is prefixed with process.cwd() and then normalized. The function is a pure string operation
     *      and does not touch the file system: it does not resolve symbolic links or check existence,
     *      and it keeps the separators of the rule set. Unlike resolve, an empty string becomes the
     *      working directory with a trailing separator. Not implemented for path.win32 outside
     *      Windows: it throws there.
     *
     *      Example — anchor a relative path to the working directory:
     *      ```JavaScript
     *      const path = require('path');
     *
     *      console.log(path.fullpath('a/../b') === path.join(process.cwd(), 'b')); // true
     *      console.log(path.fullpath('/a//b/../c')); // /a/c
     *      ```
     *
     *      @param path the path to convert
     *      @return the full path
     *
     */
    function fullpath(path: string): string;

    /**
     * @description Checks whether a path matches a glob pattern
     *
     *      Supports the common glob syntax: '*' (any characters except a separator), '**' (zero or
     *      more path segments), '?' (one character), character classes '[abc]' / '[a-z]' / '[!abc]',
     *      brace expansion '{a,b}' and the extglob forms '@(...)', '?(...)', '+(...)', '*(...)' and
     *      '!(...)'. Patterns are anchored as a whole. Dotfiles are matched only by an explicit dot:
     *      matchesGlob('.gitignore', '*') is false. In the posix rule set a backslash in the pattern
     *      escapes the next character while a backslash in the tested path is an ordinary character;
     *      in the win32 rule set both separators are accepted, drive letters are compared
     *      case-insensitively and paths on different drives never match. Node.js exposes the same
     *      function since v22.5.
     *
     *      Example — select source files with a pattern:
     *      ```JavaScript
     *      const path = require('path');
     *
     *      // '**' matches zero or more segments; the pattern is built from two literals
     *      const pattern = '**' + '/*.js';
     *
     *      console.log(path.matchesGlob('src/app.js', pattern)); // true
     *      console.log(path.matchesGlob('src/app.md', pattern)); // false
     *      console.log(path.matchesGlob('.gitignore', '*'));     // false
     *      ```
     *
     *      @param path the path to check
     *      @param pattern the glob pattern
     *      @return true when the path matches the pattern
     *
     */
    function matchesGlob(path: string, pattern: string): boolean;

    /**
     * @description Checks whether a path is absolute under the rule set of the module
     *
     *      The posix rule set returns true only for a path starting with '/'; the win32 rule set also
     *      accepts a leading '\' or '/', UNC paths such as '\\server\share' and a drive letter
     *      followed by a separator ('C:\' or 'C:/'), but reports false for a drive-relative path such
     *      as 'C:temp'. The empty string is never absolute, and the check is purely textual: the path
     *      does not need to exist. Same name in Node.js.
     *
     *      @param path the path to check
     *      @return true when the path is absolute
     *
     */
    function isAbsolute(path: string): boolean;

    /**
     * @description Joins path segments into a single normalized path using the rule set separator
     *
     *      Empty segments are ignored; when the result would be empty, '.' is returned. Later absolute
     *      segments do not reset the accumulated path the way resolve does: join('a', '/b') is 'a/b'.
     *      A '..' segment cancels a preceding segment when possible, but a leading '..' survives. In
     *      the win32 rule set a leading '//' or '\\' pair can form a UNC share and a drive letter is
     *      kept: join('c:', 'file') is 'c:\file'. Same name in Node.js.
     *
     *      Example — join segments with embedded '..':
     *      ```JavaScript
     *      const path = require('path');
     *
     *      console.log(path.join('a', 'b', '..', 'c')); // a/c
     *      console.log(path.join('a', '', 'b'));        // a/b
     *      console.log(path.join());                    // .
     *      console.log(path.join('a', '/b'));           // a/b
     *      ```
     *
     *      @param ps one or more paths
     *      @return the joined path
     *
     */
    function join(...ps: any[]): string;

    /**
     * @description Resolves path segments into an absolute path, anchored at the working directory
     *
     *      Segments are processed from right to left until an absolute one is found, so the rightmost
     *      absolute segment wins: resolve('/srv', 'www', '/etc') is '/etc'. The result is normalized
     *      and never ends with a separator. With no arguments, or with only empty segments, the
     *      current working directory is returned. In the win32 rule set the drive of the working
     *      directory is kept for relative segments, while a later drive-absolute segment replaces it.
     *      Same name in Node.js.
     *
     *      Example — resolve relative, absolute and empty segments:
     *      ```JavaScript
     *      const path = require('path');
     *
     *      console.log(path.resolve('a', '/b', 'c'));          // /b/c
     *      console.log(path.resolve() === process.cwd());       // true
     *      console.log(path.resolve(process.cwd(), 'x/../y')); // <cwd>/y
     *      ```
     *
     *      @param ps one or more paths
     *      @return the resolved path
     *
     */
    function resolve(...ps: any[]): string;

    /**
     * @description Returns the relative path from _from to to
     *
     *      Both arguments are resolved against the working directory first, so the result does not
     *      depend on whether they are relative or absolute. An empty string is returned when both
     *      describe the same location; otherwise the result is the shortest path from the first to
     *      the second, using '..' segments as needed and no trailing separator. In the win32 rule set
     *      paths are compared case-insensitively, and a target on another drive is returned unchanged
     *      because no relative path can cross drives. Same name in Node.js.
     *
     *      Example — compute relative paths:
     *      ```JavaScript
     *      const path = require('path');
     *
     *      console.log(path.relative('/srv/www', '/srv/log/app.log')); // ../log/app.log
     *      console.log(path.relative('/srv/www', '/srv/www'));         // ''
     *      ```
     *
     *      @param _from the source path
     *      @param to the target path
     *      @return the relative path
     *
     */
    function relative(_from: string, to: string): string;

    /**
     * @description Converts a path into the Windows namespace-prefixed form under the win32 rule set
     *
     *      The posix rule set returns the input unchanged, as do non-string values. The win32 rule
     *      set resolves the path and then prefixes it: a drive path gains the '\\?\' prefix ('C:\tmp'
     *      becomes '\\?\C:\tmp') and a UNC path becomes '\\?\UNC\...', with forward slashes
     *      converted to backslashes. An existing '\\?\' prefix is not duplicated. The function is a
     *      pure string operation and works on every platform, although the prefix is only meaningful
     *      on Windows. Node.js returns null unchanged for a null argument, fibjs converts it to
     *      undefined.
     *      See https://msdn.microsoft.com/library/windows/desktop/aa365247(v=vs.85).aspx#namespaces
     *
     *      @param path the path to convert
     *      @return the converted path
     *
     */
    function toNamespacedPath(path?: any): any;

    /**
     * @description The path segment separator of the rule set
     *
     *      '/' for path and path.posix, '\' for path.win32, regardless of the host system. Use it to
     *      build or split path strings instead of hard-coding a separator.
     *
     */
    const sep: string;

    /**
     * @description The separator between entries of PATH-style environment lists
     *
     *      ':' for path and path.posix, ';' for path.win32, regardless of the host system. It is not
     *      a path separator; use sep between path segments.
     *
     */
    const delimiter: string;

    /**
     * @description The posix rule set of the path module, identical to require('path/posix')
     *
     *      Use it to process POSIX paths (forward slashes, backslash as an ordinary character) on any
     *      platform. On POSIX hosts path === path.posix. See the path_posix module for details.
     *
     */
    const posix: FIBJS.GeneralObject;

    /**
     * @description The win32 rule set of the path module, identical to require('path/win32')
     *
     *      Use it to process Windows paths (drive letters, UNC shares, both separators) on any
     *      platform. On Windows hosts path === path.win32. See the path_win32 module for details.
     *
     */
    const win32: FIBJS.GeneralObject;

}

