/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description The path module is a core module that provides utilities for working with file and directory paths
 *
 * It does not check whether a path exists or is a valid path; it only provides methods for processing paths.
 *
 * The path module provides many methods; the most commonly used are:
 * - join(): joins the given path segments together into a normalized path.
 * - resolve(): resolves a sequence of paths or path segments into an absolute path.
 * - basename(): returns the last portion of a path.
 * - dirname(): returns the directory name of a path.
 * - extname(): returns the extension of the file in a path.
 *
 * Examples of these methods:
 * ```JavaScript
 * const path = require('path');
 *
 * // connect path segments using the platform-specific separator as a delimiter,
 * console.log(path.join('/usr', 'local', 'bin'));  // output: /usr/local/bin
 *
 * // resolve a sequence of paths or path segments into an absolute path
 * console.log(path.resolve('/foo/bar', './baz'));  // output: /foo/bar/baz
 *
 * // return the last portion of a path
 * console.log(path.basename('/foo/bar/baz'));  // output: baz
 *
 * // return the directory name of a path
 * console.log(path.dirname('/foo/bar/baz'));  // output: /foo/bar
 *
 * // return the extension of the path, from the last '.' to end of string in the last portion of the path
 * console.log(path.extname('/foo/bar/baz.txt'));  // output: .txt
 * ```
 *
 * Besides the methods above, the path module also provides many others, such as normalize(), delimiter,
 * posix and win32, for normalizing paths, handling path separators and platform-specific path formats.
 * They are also frequently used in practice.
 *
 * The path module provides many handy utilities for working with paths, making file and directory path
 * handling easier; it is an indispensable tool in development.
 *
 */
declare module 'path' {
    /**
     * @description Normalizes a path, resolving parent directory references
     *
     *      @param path the path to normalize
     *      @return the normalized path
     *
     */
    function normalize(path: string): string;

    /**
     * @description Returns the file name of a path, removing a matching extension when ext is given
     *
     *      @param path the path to query
     *      @param ext the extension to remove when the file name matches
     *      @return the file name
     *
     */
    function basename(path: string, ext?: string): string;

    /**
     * @description Returns the extension of the file in a path
     *
     *      @param path the path to query
     *      @return the extension
     *
     */
    function extname(path: string): string;

    /**
     * @description Formats an object into a path
     *
     *      pathObject supports the following properties:
     *      ```JavaScript
     *      {
     *         "root": "/",
     *         "dir": "/a/b",
     *         "base": "c.ext",
     *         "ext": ".ext",
     *         "name": "c"
     *      }
     *      ```
     *
     *      @param pathObject the path object
     *
     *      @return the formatted path
     *
     */
    function format(pathObject: FIBJS.GeneralObject): string;

    /**
     * @description Parses a path into a path object
     *
     *      @param path the path to parse
     *      @return the parsed path object
     *
     */
    function parse(path: string): FIBJS.GeneralObject;

    /**
     * @description Returns the directory name of a path
     *
     *      @param path the path to query
     *      @return the directory name
     *
     */
    function dirname(path: string): string;

    /**
     * @description Converts a path into a full path
     *
     *      @param path the path to convert
     *      @return the full path
     *
     */
    function fullpath(path: string): string;

    /**
     * @description Checks whether a path matches the given glob pattern
     *
     *      @param path the path to check
     *      @param pattern the glob pattern
     *      @return the match result
     *
     */
    function matchesGlob(path: string, pattern: string): boolean;

    /**
     * @description Checks whether a path is absolute
     *
     *      @param path the path to check
     *      @return true when the path is absolute
     *
     */
    function isAbsolute(path: string): boolean;

    /**
     * @description Joins a series of paths into a single path
     *
     *      @param ps one or more paths
     *      @return the joined path
     *
     */
    function join(...ps: any[]): string;

    /**
     * @description Resolves a series of paths into an absolute path
     *
     *      @param ps one or more paths
     *      @return the resolved path
     *
     */
    function resolve(...ps: any[]): string;

    /**
     * @description Returns the relative path from _from to to
     *
     *      @param _from the source path
     *      @param to the target path
     *      @return the relative path
     *
     */
    function relative(_from: string, to: string): string;

    /**
     * @description Converts a path into a namespace-prefixed path; only effective on Windows, other systems get the input back
     *     see: https://msdn.microsoft.com/library/windows/desktop/aa365247(v=vs.85).aspx#namespaces
     *      @param path the path to convert
     *      @return the converted path
     *
     */
    function toNamespacedPath(path?: any): any;

    /**
     * @description The path segment separator of the current system: '/' on posix, a backslash on Windows
     *
     */
    const sep: string;

    /**
     * @description The multi-path delimiter of the current system: ':' on posix, ';' on Windows
     *
     */
    const delimiter: string;

    /**
     * @description The posix implementation, see path_posix
     *
     */
    const posix: FIBJS.GeneralObject;

    /**
     * @description The Windows implementation, see path_win32
     *
     */
    const win32: FIBJS.GeneralObject;

}

