/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description The posix rule set of the path module: it processes POSIX paths on every platform
 *
 *  This definition is the manual page of the rule set reachable at runtime as
 *  `require('path').posix` or `require('path/posix')`; it is not itself a require-able module
 *  name. All functions are pure string operations and never touch the file system.
 *
 *  Main capabilities:
 *
 *  - **Building paths**: `join`, `resolve`, `normalize`, `fullpath` (fibjs extension);
 *  - **Breaking paths down**: `parse`, `format`, `basename`, `dirname`, `extname`;
 *  - **Comparing paths**: `relative`, `isAbsolute`, `matchesGlob`;
 *  - **Constants**: `sep` ('/') and `delimiter` (':').
 *
 *  Concepts:
 *
 *  - **POSIX rule set**: '/' separates path segments and a backslash is an ordinary character,
 *    so 'a\\b.txt' is a single file name. A path starting with '/' is absolute; the root is '/'
 *    and '..' is clamped there. On POSIX hosts the path module itself applies these rules
 *    (`path === path.posix`); on Windows use this rule set to process paths received from a
 *    POSIX system. See the path module for the platform default and the path_win32 module for
 *    the Windows rule set.
 *  - **join vs resolve vs fullpath**: join merges segments and normalizes, keeping a later
 *    absolute segment as a plain segment; resolve restarts at the rightmost absolute segment and
 *    anchors the result at the working directory; fullpath (fibjs extension) anchors a relative
 *    path to the working directory and normalizes it, without touching the file system.
 *  - **Normalization**: '.' segments are dropped, '..' cancels the previous segment where
 *    possible and repeated separators collapse; a trailing separator survives ('a//b/' becomes
 *    'a/b/'), a leading '..' is kept in relative paths, and normalize('') is '.'.
 *  - **Extensions and dotfiles**: extname('.bashrc') is '' because a leading dot starts a
 *    dotfile; extname('.env.local') is '.local' and extname('index.') is '.'. basename(path, ext)
 *    strips ext as a plain suffix and ignores trailing separators.
 *  - **Glob matching**: matchesGlob supports '*', '**', '?', character classes, brace expansion
 *    and extglob forms; a backslash in the pattern is treated as a path separator, while a
 *    backslash in the tested path is an ordinary character.
 *
 *  Import:
 *  ```JavaScript
 *  // path_posix is the manual-page name; the runtime entry points are:
 *  const posix = require('path').posix;
 *  const posixAgain = require('path/posix');
 *  ```
 *
 *  Example 1 — the posix rule set treats backslashes as ordinary characters:
 *  ```JavaScript
 *  const posix = require('path').posix;
 *
 *  console.log(posix.basename('a\\b.txt'));      // a\b.txt
 *  console.log(posix.normalize('a//b/./../c')); // a/c
 *
 *  // dotfiles have no extension unless another dot follows
 *  console.log(posix.extname('.bashrc'));     // ''
 *  console.log(posix.extname('.env.local')); // .local
 *  ```
 *
 *  Example 2 — building and comparing posix paths:
 *  ```JavaScript
 *  const posix = require('path').posix;
 *
 *  console.log(posix.join('/usr', 'local', 'bin')); // /usr/local/bin
 *  console.log(posix.join('a', '../b'));            // b
 *  console.log(posix.resolve('/srv', 'www'));       // /srv/www
 *  console.log(posix.relative('/a/b', '/a/c'));     // ../c
 *  ```
 *
 *  Example 3 — parse and format round trip:
 *  ```JavaScript
 *  const posix = require('path').posix;
 *
 *  const parts = posix.parse('/etc/nginx/nginx.conf');
 *  console.log(parts.dir, parts.base, parts.ext); // /etc/nginx nginx.conf .conf
 *
 *  console.log(posix.format({ dir: parts.dir, name: 'site', ext: '.conf' }));
 *  // /etc/nginx/site.conf
 *  ```
 *
 *  Notes:
 *
 *  - Node.js exposes the same rule set as require('path').posix; fibjs also provides the
 *    require('path/posix') subpath form and documents the rule set as the path_posix definition.
 *  - fibjs adds fullpath to the rule set; see the path module for the differences from Node.js.
 *
 */
declare module 'path_posix' {
    /**
     * @description Normalizes a posix path, resolving '.' and '..' and collapsing '/' separators
     *
     *      A pure string transformation over the posix rule set: '/' separates segments and a
     *      backslash is an ordinary character. Relative paths keep leading '..' segments; absolute
     *      paths are clamped at the root, so '/../' becomes '/'. A trailing separator is preserved
     *      ('a//b/' becomes 'a/b/') and normalize('') is '.'. See the path module for the shared
     *      normalization rules.
     *
     *      Example — normalize a posix path:
     *      ```JavaScript
     *      const posix = require('path').posix;
     *
     *      console.log(posix.normalize('a//b/./../c'));         // a/c
     *      console.log(posix.normalize('/usr//local/../bin/')); // /usr/bin/
     *      console.log(posix.normalize(''));                    // .
     *      ```
     *
     *      @param path the path to normalize
     *      @return the normalized path
     *
     */
    function normalize(path: string): string;

    /**
     * @description Returns the last portion of a posix path, removing a matching extension
     *
     *      Trailing '/' separators are ignored (basename('foo/') is 'foo'), basename('/') is '' and a
     *      backslash is an ordinary character (basename('a\\b.txt') is 'a\\b.txt'). The optional ext
     *      is stripped as a plain suffix, not necessarily starting with a dot:
     *      basename('/a/b.txt', 'txt') is 'b.'.
     *
     *      @param path the path to query
     *      @param ext the extension to remove when the file name matches
     *      @return the file name
     *
     */
    function basename(path: string, ext?: string): string;

    /**
     * @description Returns the extension from the last '.' of the last posix segment
     *
     *      A leading dot starts a dotfile, not an extension: extname('.bashrc') is '',
     *      extname('.env.local') is '.local' and extname('index.') is '.'. The result is '' for '..'
     *      and for paths ending with '/'. A dotted parent directory is ignored, and a backslash is an
     *      ordinary character, so extname('a\\b.txt') is '.txt'.
     *
     *      @param path the path to query
     *      @return the extension
     *
     */
    function extname(path: string): string;

    /**
     * @description Formats a path object into a posix path string, the inverse of parse
     *
     *      All fields are optional. Base wins over name + ext, and dir wins over root unless dir is
     *      empty; when dir equals root no separator is inserted. '/' is always used as the separator,
     *      even on Windows, and a dir-only object produces a trailing '/' ('some/dir' becomes
     *      'some/dir/'). The full field list is documented in the path module.
     *
     *      pathObject supports the following properties:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "root": "/",       // the root of the path, always '/' for posix
     *          "dir": "some/dir", // the directory; wins over root when both are set
     *          "base": "c.ext",   // the full last segment; wins over name + ext
     *          "ext": ".ext",     // the extension, including the leading dot
     *          "name": "c"        // the name without the extension
     *      })
     *      ```
     *
     *      Example — build posix paths from objects:
     *      ```JavaScript
     *      const posix = require('path').posix;
     *
     *      console.log(posix.format({ dir: 'some/dir', name: 'index', ext: '.html' }));
     *      // some/dir/index.html
     *      console.log(posix.format({ root: '/' }));       // /
     *      console.log(posix.format({ dir: 'some/dir' })); // some/dir/
     *      ```
     *
     *      @param pathObject the path object
     *      @return the formatted path
     *
     */
    function format(pathObject: FIBJS.GeneralObject): string;

    /**
     * @description Parses a posix path into an object with root, dir, base, ext and name fields
     *
     *      The object always contains all five fields as strings and can be passed to format. root is
     *      '/' for absolute paths and '' otherwise; a trailing '/' is ignored for base; a leading dot
     *      starts a dotfile ('.bashrc' has no extension), while 'index.' has ext '.'. A backslash is
     *      an ordinary character. The field semantics are shared with the path module.
     *
     *      Example — inspect a parsed posix path:
     *      ```JavaScript
     *      const posix = require('path').posix;
     *
     *      const parts = posix.parse('/var/log/app.tar.gz');
     *      console.log(parts.dir, parts.name, parts.ext); // /var/log app.tar .gz
     *
     *      console.log(posix.parse('/a/b/').base); // b
     *      console.log(posix.parse('.').base);     // .
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
     * @description Returns the directory name of a posix path, dropping the last segment
     *
     *      Trailing '/' separators are ignored: dirname('/a/b/') is '/a', dirname('/a') is '/',
     *      dirname('foo') is '.' and dirname('') is '.'. dirname('/') stays '/'. A backslash is an
     *      ordinary character, so dirname('a\\b.txt') is '.'.
     *
     *      @param path the path to query
     *      @return the directory name
     *
     */
    function dirname(path: string): string;

    /**
     * @description Converts a posix path into an absolute path anchored at the working directory
     *
     *      A fibjs extension, not part of Node.js. An absolute input is normalized; a relative input
     *      is prefixed with process.cwd() and then normalized with the posix rule set. The function
     *      is a pure string operation, does not touch the file system and never checks existence.
     *      Unlike resolve, an empty string becomes the working directory plus a trailing separator.
     *
     *      Example — anchor a relative path to the working directory:
     *      ```JavaScript
     *      const posix = require('path').posix;
     *
     *      console.log(posix.fullpath('a/../b') === posix.join(process.cwd(), 'b')); // true
     *      console.log(posix.fullpath('')); // <cwd>/ (the working directory plus a separator)
     *      ```
     *
     *      @param path the path to convert
     *      @return the full path
     *
     */
    function fullpath(path: string): string;

    /**
     * @description Checks whether a path matches a glob pattern using the posix rule set
     *
     *      Supports '*', '**', '?', character classes, brace expansion and the extglob forms;
     *      patterns are anchored as a whole and dotfiles need an explicit dot. Under the posix rule
     *      set a backslash in the pattern is treated as a path separator, while a backslash in the
     *      tested path is an ordinary character: matchesGlob('a/b', 'a\\b') is true, but
     *      matchesGlob('a\\b', 'a/b') is false. The path module documents the full syntax.
     *
     *      @param path the path to check
     *      @param pattern the glob pattern
     *      @return true when the path matches the pattern
     *
     */
    function matchesGlob(path: string, pattern: string): boolean;

    /**
     * @description Checks whether a posix path is absolute
     *
     *      Returns true only when the path starts with '/'; the check is purely textual, so the path
     *      does not need to exist. The empty string is false, and a Windows drive path such as
     *      'C:\\dir' is not absolute under the posix rule set because it does not start with '/'.
     *
     *      @param path the path to check
     *      @return true when the path is absolute
     *
     */
    function isAbsolute(path: string): boolean;

    /**
     * @description Joins segments into a normalized posix path using '/' as the separator
     *
     *      Empty segments are ignored and '.' is returned when the result would be empty. A later
     *      absolute segment is appended as an ordinary segment: join('a', '/b') is 'a/b'. A '..'
     *      cancels a preceding segment where possible, but a leading '..' survives. See the path
     *      module for the join/resolve/fullpath comparison.
     *
     *      Example — join posix segments:
     *      ```JavaScript
     *      const posix = require('path').posix;
     *
     *      console.log(posix.join('/usr', 'local', 'bin')); // /usr/local/bin
     *      console.log(posix.join('a', '../b'));            // b
     *      console.log(posix.join(''));                     // .
     *      ```
     *
     *      @param ps one or more paths
     *      @return the joined path
     *
     */
    function join(...ps: any[]): string;

    /**
     * @description Resolves segments into an absolute posix path, anchored at the working directory
     *
     *      Segments are processed from right to left until an absolute one is found, so the rightmost
     *      absolute segment wins; the result is normalized and has no trailing separator. With no
     *      arguments, or with only empty segments, the working directory is returned. Paths are
     *      resolved with the posix rule set even when the host is Windows.
     *
     *      Example — resolve posix segments:
     *      ```JavaScript
     *      const posix = require('path').posix;
     *
     *      console.log(posix.resolve('/srv', 'www'));          // /srv/www
     *      console.log(posix.resolve('/srv', 'www', '/etc'));  // /etc
     *      ```
     *
     *      @param ps one or more paths
     *      @return the resolved path
     *
     */
    function resolve(...ps: any[]): string;

    /**
     * @description Returns the relative posix path from _from to to
     *
     *      Both arguments are resolved against the working directory first, so the result does not
     *      depend on whether they are relative or absolute. '' is returned for the same location;
     *      otherwise the result uses '..' segments as needed and has no trailing separator.
     *
     *      @param _from the source path
     *      @param to the target path
     *      @return the relative path
     *
     */
    function relative(_from: string, to: string): string;

    /**
     * @description Returns the input unchanged; the namespace prefix only applies to the win32 rule set
     *
     *      The posix rule set has no namespace-prefixed form, so strings and non-string values are
     *      returned as-is on every platform.
     *
     *      @param path the path to convert
     *      @return the input value
     *
     */
    function toNamespacedPath(path?: any): any;

    /**
     * @description The path segment separator of the posix rule set: '/'
     *
     */
    export const sep: "/";

    /**
     * @description The PATH-list delimiter of the posix rule set: ':'
     *
     */
    export const delimiter: ":";

    /**
     * @description The posix rule set itself
     *
     *      A self reference to the object returned by require('path').posix, provided for API
     *      symmetry with path_win32.
     *
     */
    const posix: FIBJS.GeneralObject;

    /**
     * @description The win32 rule set of the path module, see path_win32
     *
     *      Use it to process Windows paths (drive letters, UNC shares, both separators) from a posix
     *      context.
     *
     */
    const win32: FIBJS.GeneralObject;

}

