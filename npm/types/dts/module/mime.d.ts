/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description MIME parsing module
 */
declare module 'mime' {
    /**
     * @description parses the corresponding MIME type according to the provided file name
     *
     *      @param fname specifies the file name to parse
     *      @return returns the parsed MIME type
     *
     */
    function getType(fname: string): string;

    /**
     * @description adds a MIME type
     *
     *      @param ext the specified extension
     *      @param type the specified MIME type
     *
     */
    function addType(ext: string, type: string): void;

}

