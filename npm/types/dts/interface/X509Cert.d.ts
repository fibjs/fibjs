/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/PKey.d.ts" />
/**
 * @description x509 证书对象
 * 
 *  X509Cert 对象属于 crypto 模块，创建：
 *  ```JavaScript
 *  var k = new crypto.X509Cert();
 *  ```
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
     * @description X509Cert 构造函数，加载一个 CRT/PEM/TXT 格式的证书
     * 
     *     load 加载 mozilla 的 certdata,txt， 可于 http://hg.mozilla.org/releases/mozilla-release/raw-file/default/security/nss/lib/ckfw/builtins/certdata.txt 下载使用
     *      @param txtCert CRT/PEM/TXT 格式的证书
     *     
     */
    constructor(txtCert: string);

    /**
     * @description 加载一个 DER 格式的证书，可多次调用
     *      @param derCert DER 格式的证书
     *     
     */
    load(derCert: Class_Buffer): void;

    /**
     * @description 加载一个 CRT/PEM/TXT 格式的证书，可多次调用
     * 
     *     load 加载 mozilla 的 certdata,txt， 可于 http://hg.mozilla.org/releases/mozilla-release/raw-file/default/security/nss/lib/ckfw/builtins/certdata.txt 下载使用
     *      @param txtCert CRT/PEM/TXT 格式的证书
     *     
     */
    load(txtCert: string): void;

    /**
     * @description 加载一个 CRT/PEM/DER/TXT 格式的证书，可多次调用
     * 
     *     loadFile 加载 mozilla 的 certdata,txt， 可于 http://hg.mozilla.org/releases/mozilla-release/raw-file/default/security/nss/lib/ckfw/builtins/certdata.txt 下载使用
     *      @param filename 证书文件名
     *     
     */
    loadFile(filename: string): void;

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
     * @description 导出已经加载的证书
     *      @param pem 指定输出 PEM 格式的撤销证书，缺省为 true
     *      @return 以数组方式导出证书链
     *     
     */
    dump(pem?: boolean): any[];

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
     * @description 获取证书的公钥 
     */
    readonly publicKey: Class_PKey;

    /**
     * @description 获取证书链中得下一个证书 
     */
    readonly next: Class_X509Cert;

}

