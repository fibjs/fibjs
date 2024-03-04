/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/X509Certificate.d.ts" />
/// <reference path="../interface/KeyObject.d.ts" />
/**
 * @description tls 安全上下文对象，用于多个 tls 连接共享基础配置
 * 
 *   SecureContext 对象是一个安全上下文对象，用于多个 tls 连接共享基础配置。SecureContext 对象可以通过 tls.createSecureContext 方法创建。
 *   ```JavaScript
 *     const tls = require('tls');
 *     const fs = require('fs');
 * 
 *     const options = {
 *       key: fs.readFileSync('server-key.pem'),
 *       cert: fs.readFileSync('server-cert.pem')
 *     };
 * 
 *     const context = tls.createSecureContext(options);
 *   ```
 *  
 */
declare class Class_SecureContext extends Class_object {
    readonly ca: Class_X509Certificate;

    readonly key: Class_KeyObject;

    readonly cert: Class_X509Certificate;

    readonly maxVersion: string;

    readonly minVersion: string;

    readonly secureProtocol: string;

    readonly rejectUnauthorized: boolean;

    readonly sessionTimeout: number;

}

