/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/Stat.d.ts" />
/**
 * @description Stream object interface with a movable current pointer
 */
declare class Class_SeekableStream extends Class_Stream {
    /**
     * @description Moves the current file operation position
     *       @param offset the new position
     *       @param whence the position base, allowed values: SEEK_SET, SEEK_CUR, SEEK_END
     *
     */
    seek(offset: number, whence: number): void;

    /**
     * @description Queries the current stream position
     *      @return returns the current stream position
     *
     */
    tell(): number;

    /**
     * @description Moves the current position to the beginning of the stream
     */
    rewind(): void;

    /**
     * @description Queries the stream size
     *      @return returns the stream size
     *
     */
    size(): number;

    /**
     * @description Modifies the file size; if the new size is smaller than the original size, the file is truncated
     *       @param bytes the new file size
     *
     */
    truncate(bytes: number): void;

    truncate(bytes: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Modifies the file size; if the new size is smaller than the original size, the file is truncated
     *       @param bytes the new file size
     *
     */
    truncateSync(bytes: number): void;

    /**
     * @description Modifies the file size; if the new size is smaller than the original size, the file is truncated
     *       @param bytes the new file size
     *
     */
    truncateAsync(bytes: number): Promise<void>;

    /**
     * @description Queries whether the file is at the end
     *      @return returns True if at the end
     *
     */
    eof(): boolean;

    /**
     * @description Queries the basic information of the current file
     *      @return returns the Stat object describing the file information
     *
     */
    stat(): Class_Stat;

    stat(callback: (err: Error | undefined | null, retVal: Class_Stat)=>any): void;

    /**
     * @description Queries the basic information of the current file
     *      @return returns the Stat object describing the file information
     *
     */
    statSync(): Class_Stat;

    /**
     * @description Queries the basic information of the current file
     *      @return returns the Stat object describing the file information
     *
     */
    statAsync(): Promise<Class_Stat>;

}

