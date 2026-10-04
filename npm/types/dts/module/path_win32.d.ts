/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description File path processing module
 *
 *  Usage:
 *  ```JavaScript
 *  var path = require('path').win32;
 *  ```
 *
 */
declare module 'path_win32' {
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
     *      pathObject supports the following fields:
     *      ```JavaScript
     *      {
     *          "dir": "", // specify the directory of the path
     *          "root": "", // specify the root of the path
     *          "base": "", // specify the base of the path, it's the combination of name and ext
     *          "name": "", // specify the name of the path
     *          "ext": "", // specify the ext of the path
     *      }
     *      ```
     *      @param pathObject the path object
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
    function parse(path: string): {
        root: string;
        dir: string;
        base: string;
        ext: string;
        name: string;
    };

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
    export const sep: "\\";

    /**
     * @description The multi-path delimiter of the current system: ':' on posix, ';' on Windows
     *
     */
    export const delimiter: ";";

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

