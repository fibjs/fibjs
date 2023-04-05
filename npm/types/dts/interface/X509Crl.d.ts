/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description X509Crl 对象是 crypto 模块中的一个对象，用来表示 X509 撤销证书，即证书的吊销列表，与 CA 相关的操作如 X509 证书链的验证都会用到撤销列表信息
 * 
 *  X509Crl 在 crypto 模块声明，故可以通过下面的代码来引入：
 * 
 * ```JavaScript
 * const crypto = require('crypto');
 * ```
 * 
 * 用户可以通过新建 X509Crl 类对象，调用对象的 import 方法加载一个 DER 或 PEM 格式的证书。可以通过 clear 方法清除已经加载的撤销证书列表信息，控制加载状态，以支持重复的证书加载工作。然后可以使用 nextUpdate 方法获取撤销列表的下次更新时间，调用 serials 获取证书吊销序列号列表等。
 * 
 * 下面这个示例演示如何从文件中导入一个撤销证 ，并将其列表在控制台输出：
 * 
 * ```JavaScript
 * const crypto = require('crypto');
 * const fs = require('fs');
 * 
 * // load a PEM format CRL
 * var pemFile = './mycrl.pem';
 * var pemText = fs.readFile(pemFile).toString();
 * var crl = new crypto.X509Crl(pemText);
 * 
 * // output serials of revoked certificates
 * for (var i = 0; i < crl.serials.length; i++) {
 *     console.log(crl.serials[i]);
 * }
 * ```
 *  
 */
declare class Class_X509Crl extends Class_object {
    /**
     * @description X509Crl 构造函数 
     */
    constructor();

    /**
     * @description X509Crl 构造函数,加载一个 DER 格式的撤销证书
     *      @param derCrl DER 格式的撤销证书
     *     
     */
    constructor(derCrl: Class_Buffer);

    /**
     * @description X509Crl 构造函数,加载一个 PEM 格式的撤销证书
     *      @param pemCrl PEM 格式的撤销证书
     *     
     */
    constructor(pemCrl: string);

    /**
     * @description 加载一个 DER 格式的撤销证书，可多次调用
     *      @param derCrl DER 格式的撤销证书
     *     
     */
    import(derCrl: Class_Buffer): void;

    /**
     * @description 加载一个 PEM 格式的撤销证书，可多次调用
     *      @param pemCrl PEM 格式的撤销证书
     *     
     */
    import(pemCrl: string): void;

    /**
     * @description 以 PEM 格式导出已经加载的撤销证书
     *      @param all 指定是否输出全部证书，缺省为 true
     *      @return 以数组方式导出撤销证书链
     *     
     */
    pem(all?: boolean): string;

    /**
     * @description 以 DER 格式导出已经加载的撤销证书
     *      @return 以数组方式导出撤销证书链
     *     
     */
    der(): Class_Buffer;

    /**
     * @description 清空已经加载的撤销证书
     *     
     */
    clear(): void;

    /**
     * @description 获取证书的版本 
     */
    readonly version: number;

    /**
     * @description 获取证书颁发者的可分辨名称 
     */
    readonly issuer: string;

    /**
     * @description 获取证书吊销序列号列表 
     */
    readonly serials: any[];

    /**
     * @description 获取证书的本次更新时间 
     */
    readonly thisUpdate: typeof Date;

    /**
     * @description 获取证书的下次更新时间 
     */
    readonly nextUpdate: typeof Date;

    /**
     * @description 获取证书链中得下一个证书 
     */
    readonly next: Class_X509Crl;

}

