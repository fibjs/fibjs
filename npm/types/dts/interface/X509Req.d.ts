/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/PKey.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/X509Cert.d.ts" />
/**
 * @description X509Req 对象是用于创建 x509 证书请求的对象，属于 crypto 模块
 * 
 * 可以通过它的构造函数创建 X509Req 的实例，在构造函数中可以传入参数指定证书的主题可分辨名称和公钥，例如：
 * 
 * ```JavaScript
 * var crypto = require('crypto');
 * 
 * let pky = crypto.PKey.from(private_pem);
 * let req = new crypto.X509Req("CN=localhost,O=fibjs", pky);
 * ```
 * 
 * 可以通过该实例调用其父类的方法和属性来获取证书请求对象的信息，例如：
 * 
 * ```JavaScript
 * // return the DER format of the certificate request
 * let derReq = req.der();
 * 
 * // return the PEM format of the certificate request
 * let pemReq = req.pem();
 * ```
 * 
 * 也可以对 X509Req 对象进行签名，生成正式的证书对象，例如：
 * 
 * ```JavaScript
 * let opt = {
 *   notBefore: new Date('2019-01-01') // valid from 2019-01-01
 *  ,notAfter:  new Date('2029-12-31') // valid to 2029-12-31
 * };
 * let crt = req.sign("CN=myy.mkx", pky, opt); 
 * // CN=myy.mkx is the issuer of the certificate
 * ```
 * 
 * 需要注意的是，X509Req 对象的作用是创建 x509 证书请求，而不是证书本身，要想获得有效的证书还需要对其进行签名。同时，签名证书所使用的公钥必须和证书请求中使用的公钥一致。
 *  
 */
declare class Class_X509Req extends Class_object {
    /**
     * @description X509Req 构造函数 
     */
    constructor();

    /**
     * @description X509Req 构造函数，根据给定的信息创建一个证书请求
     *      @param subject 证书的主题可分辨名称
     *      @param key 证书的公钥
     *      @param hash 证书摘要算法，缺省为 hash.SHA256
     *     
     */
    constructor(subject: string, key: Class_PKey, hash: number);

    /**
     * @description X509Req 构造函数，加载一个 DER 格式的证书请求
     *      @param derReq DER 格式的证书请求
     *     
     */
    constructor(derReq: Class_Buffer);

    /**
     * @description X509Req 构造函数，加载一个 PEM 格式的证书请求
     *      @param pemReq DER 格式的证书请求
     *     
     */
    constructor(pemReq: string);

    /**
     * @description 加载一个 DER 格式的证书请求
     *      @param derReq DER 格式的证书请求
     *     
     */
    import(derReq: Class_Buffer): void;

    /**
     * @description 加载一个 PEM 格式的证书请求
     *      @param pemReq PEM 格式的证书请求
     *     
     */
    import(pemReq: string): void;

    /**
     * @description 返回当前证书请求的 PEM 格式编码
     *      @return 当前证书请求的 PEM 格式编码
     *     
     */
    pem(): string;

    /**
     * @description 返回当前证书请求的 DER 格式编码
     *      @return 当前证书请求的 DER 格式编码
     *     
     */
    der(): Class_Buffer;

    /**
     * @description 签名当前证书请求为正式证书
     * 
     *     opts 接收的字段如下：
     *     ```JavaScript
     *     {
     *         ca: false,      // specify if the certificate is a CA certificate, default is false
     *         pathlen: -1,    // specify the path length of the certificate, default is -1
     *         notBefore: "",  // specify the certificate valid from date, default is current date
     *         notAfter: "",   // specify the certificate valid to date, default is 365 days after current date
     *         usage: "",      // specify the certificate usage, accept: digitalSignature, nonRepudiation, keyEncipherment, dataEncipherment, keyAgreement, keyCertSign, cRLSign
     *         type: ""        // specify the certificate Netscape certificate type, accept: client, server, email, objsign, reserved, sslCA, emailCA, objCA
     *     }
     *     ```
     *      @param issuer 签名机构的可分辨名称
     *      @param key 签名机构的私钥
     *      @param opts 其他可选参数
     *      @return 返回签名后的正式证书
     *      
     */
    sign(issuer: string, key: Class_PKey, opts?: FIBJS.GeneralObject): Class_X509Cert;

    sign(issuer: string, key: Class_PKey, opts?: FIBJS.GeneralObject, callback?: (err: Error | undefined | null, retVal: Class_X509Cert)=>any): void;

    /**
     * @description 获取证书的主题可分辨名称 
     */
    readonly subject: string;

    /**
     * @description 获取证书的公钥 
     */
    readonly publicKey: Class_PKey;

    /**
     * @description 获取证书的摘要算法 
     */
    readonly sig_md: number;

    /**
     * @description 获取证书的签名算法 
     */
    readonly sig_pk: number;

}

