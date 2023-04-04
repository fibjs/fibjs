/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/PKey.d.ts" />
/**
 * @description X509Cert 是 crypto 模块中表示 x509 证书的对象，这个对象主要包括了从一个 DER 或者 PEM 格式的证书数据以及证书链数据构建出来的证书信息和方法，可以进行证书的解析和验证等操作
 * 
 * 通过 X509Cert 对象可以获取到证书的主题名称，颁发者名称，通过时间，到期时间，ca 验证等等信息，还可以导出证书的 DER 编码，获取证书的公钥以及签名算法等等信息。
 * 
 * 如果想要加载一个证书，可以使用如下代码从 DER/PEM 格式的证书数据中创建 X509Cert 对象：
 * 
 * ```JavaScript
 * const crypto = require('crypto');
 * 
 * const certData = require('fs').readFile('server.crt');
 * const cert = new crypto.X509Cert(certData);
 * ```
 * 
 * 当然，X509Cert 对象也提供了直接导入证书的方法：
 * 
 * ```JavaScript
 * const cert = crypto.X509Cert.import(certData);
 * ```
 * 
 * 然后你就可以通过 cert 进行各种证书的操作，例如获取证书的有效期时间：
 * 
 * ```JavaScript
 * var strDate = cert.notBefore.toLocaleTimeString() + " " + cert.notBefore.toLocaleDateString() + " ~ " + cert.notAfter.toLocaleTimeString() + " " + cert.notAfter.toLocaleDateString();
 * console.log(`证书有效时间: ${strDate}`)
 * ```
 *  
 */
declare class Class_X509Cert extends Class_object {
    /**
     * @description X509Cert 构造函数 
     */
    constructor();

    /**
     * @description X509Cert 构造函数，加载一个 DER 格式的证书
     *      @param derCert DER 格式的证书
     *     
     */
    constructor(derCert: Class_Buffer);

    /**
     * @description X509Cert 构造函数，加载一个 CRT/PEM 格式的证书
     *      @param txtCert CRT/PEM 格式的证书
     *     
     */
    constructor(txtCert: string);

    /**
     * @description 加载一个 DER 格式的证书，可多次调用
     *      @param derCert DER 格式的证书
     *     
     */
    import(derCert: Class_Buffer): void;

    /**
     * @description 加载一个 CRT/PEM 格式的证书，可多次调用
     *      @param txtCert CRT/PEM 格式的证书
     *     
     */
    import(txtCert: string): void;

    /**
     * @description 加载自带的缺省根证书
     *     此证书内容源自：http://hg.mozilla.org/releases/mozilla-release/raw-file/default/security/nss/lib/ckfw/builtins/certdata.txt 
     */
    loadRootCerts(): void;

    /**
     * @description 使用当前证书链验证给定的证书
     *      @param cert 给定需要验证的证书
     *      @return 如果验证成功则返回 True
     *     
     */
    verify(cert: Class_X509Cert): boolean;

    verify(cert: Class_X509Cert, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description 以 PEM 格式导出已经加载的证书
     *      @param all 指定是否输出全部证书，缺省为 true
     *      @return 以数组方式导出证书链
     *     
     */
    pem(all?: boolean): string;

    /**
     * @description 以 DER 格式导出已经加载的证书
     *      @return 以数组方式导出证书链
     *     
     */
    der(): Class_Buffer;

    /**
     * @description 清空已经加载的证书 
     */
    clear(): void;

    /**
     * @description 获取证书的版本 
     */
    readonly version: number;

    /**
     * @description 获取证书的序列号 
     */
    readonly serial: string;

    /**
     * @description 获取证书颁发者的可分辨名称 
     */
    readonly issuer: string;

    /**
     * @description 获取证书的主题可分辨名称 
     */
    readonly subject: string;

    /**
     * @description 获取证书的生效时间 
     */
    readonly notBefore: typeof Date;

    /**
     * @description 获取证书的到期时间 
     */
    readonly notAfter: typeof Date;

    /**
     * @description 获取证书是否是 ca 证书 
     */
    readonly ca: boolean;

    /**
     * @description 获取证书的 pathlen 
     */
    readonly pathlen: number;

    /**
     * @description 获取证书的使用范围
     * 
     *     结果为全部或部分以下内容：digitalSignature, nonRepudiation, keyEncipherment, dataEncipherment, keyAgreement, keyCertSign, cRLSign
     *     
     */
    readonly usage: string;

    /**
     * @description 获取证书的 Netscape 证书类型
     * 
     *     结果为全部或部分以下内容：client, server, email, objsign, reserved, sslCA, emailCA, objCA
     *      
     */
    readonly type: string;

    /**
     * @description 获取证书的摘要算法 
     */
    readonly sig_md: number;

    /**
     * @description 获取证书的签名算法 
     */
    readonly sig_pk: number;

    /**
     * @description 获取证书的公钥 
     */
    readonly publicKey: Class_PKey;

    /**
     * @description 获取证书链中得下一个证书 
     */
    readonly next: Class_X509Cert;

}

