/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description crypto 模块常用常量定义模块
 * 
 *  引用方法：
 *  ```JavaScript
 *  var constants = require('crypto').constants
 *  ```
 *  
 */
declare module 'crypto_constants' {
    export const RSA_PKCS1_PADDING: 1;

    export const RSA_NO_PADDING: 3;

    export const RSA_PKCS1_OAEP_PADDING: 4;

    export const RSA_X931_PADDING: 5;

    export const RSA_PKCS1_PSS_PADDING: 6;

    export const RSA_PSS_SALTLEN_DIGEST: -1;

    export const RSA_PSS_SALTLEN_MAX_SIGN: -2;

    export const RSA_PSS_SALTLEN_AUTO: -2;

}

