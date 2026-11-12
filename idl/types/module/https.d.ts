/// <reference path="../_import/_fibjs.d.ts" />
// Hand-written `https` module (no IDL unit: fibjs/src/http/http.cpp registers
// it with DECLARE_MODULE_EX(https, http), i.e. the *same* class info and the
// same module object as `http`).
//
// Runtime check: `require('https') === require('http')` and both carry the same
// 63 keys. The declaration therefore re-exports `http` wholesale; there is no
// separate https class to describe (Node's https has its own Server, but fibjs
// serves TLS through the same HttpServer plus `http.createServer(options,
// handler)`).

declare module 'https' {
    import http = require('http');

    export = http;
}
