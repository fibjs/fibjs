/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description StreamReader 对象，兼容 WHATWG ReadableStreamDefaultReader 接口的轻量级读取器
 * 
 * StreamReader 将 fibjs 的 Stream 封装为兼容 Web Streams API 的 read() 方法。
 * 可以通过 Stream.getReader() 获取。
 * 
 * ```JavaScript
 * const response = await fetch('http://example.com');
 * const reader = response.body.getReader();
 * while (true) {
 *     const { done, value } = await reader.read();
 *     if (done) break;
 *     console.log(value);
 * }
 * ```
 * 
 */
declare class Class_StreamReader extends Class_object {
    /**
     * @description 从流中读取下一个数据块
     *      @return 返回一个包含 `done`（布尔值）和 `value`（Buffer 数据或 undefined）属性的对象
     *      
     */
    read(): Promise<[done: boolean, value: Buffer]>;

    /**
     * @description 从流中读取下一个数据块
     *      @return 返回一个包含 `done`（布尔值）和 `value`（Buffer 数据或 undefined）属性的对象
     *      
     */
    readSync(): [done: boolean, value: Buffer];

    /**
     * @description 从流中读取下一个数据块
     *      @return 返回一个包含 `done`（布尔值）和 `value`（Buffer 数据或 undefined）属性的对象
     *      
     */
    readAsync(): Promise<[done: boolean, value: Buffer]>;

    /**
     * @description 释放对流的锁定 
     */
    releaseLock(): void;

    /**
     * @description 取消流并释放锁定
     *      @param reason 可选的取消原因
     *      @return 返回一个 Promise，在取消完成时解析
     *      
     */
    cancel(reason?: string): Promise<void>;

    /**
     * @description 取消流并释放锁定
     *      @param reason 可选的取消原因
     *      @return 返回一个 Promise，在取消完成时解析
     *      
     */
    cancelSync(reason?: string): void;

    /**
     * @description 取消流并释放锁定
     *      @param reason 可选的取消原因
     *      @return 返回一个 Promise，在取消完成时解析
     *      
     */
    cancelAsync(reason?: string): Promise<void>;

    /**
     * @description 流关闭时解析的 Promise 
     */
    readonly closed: Promise;

}

