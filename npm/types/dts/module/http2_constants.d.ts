/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description http2 模块常用常量定义模块
 * 
 *  引用方法：
 *  ```JavaScript
 *  var constants = require('http2').constants
 *  ```
 *  
 */
declare module 'http2_constants' {
    /**
     * @description 默认是否启用 CONNECT 协议扩展 
     */
    export const DEFAULT_SETTINGS_ENABLE_CONNECT_PROTOCOL: 0;

    /**
     * @description 默认是否启用服务端推送 
     */
    export const DEFAULT_SETTINGS_ENABLE_PUSH: 1;

    /**
     * @description 默认头部表大小（字节） 
     */
    export const DEFAULT_SETTINGS_HEADER_TABLE_SIZE: 4096;

    /**
     * @description 默认初始流窗口大小（字节） 
     */
    export const DEFAULT_SETTINGS_INITIAL_WINDOW_SIZE: 65535;

    /**
     * @description 默认最大并发流数（无限制） 
     */
    export const DEFAULT_SETTINGS_MAX_CONCURRENT_STREAMS: 4294967295;

    /**
     * @description 默认最大帧大小（字节） 
     */
    export const DEFAULT_SETTINGS_MAX_FRAME_SIZE: 16384;

    /**
     * @description 默认最大头部列表大小（字节） 
     */
    export const DEFAULT_SETTINGS_MAX_HEADER_LIST_SIZE: 65535;

    /**
     * @description HTTP 状态码：Continue 
     */
    export const HTTP_STATUS_CONTINUE: 100;

    /**
     * @description HTTP 状态码：Switching Protocols 
     */
    export const HTTP_STATUS_SWITCHING_PROTOCOLS: 101;

    /**
     * @description HTTP 状态码：Processing 
     */
    export const HTTP_STATUS_PROCESSING: 102;

    /**
     * @description HTTP 状态码：Early Hints 
     */
    export const HTTP_STATUS_EARLY_HINTS: 103;

    /**
     * @description HTTP 状态码：OK 
     */
    export const HTTP_STATUS_OK: 200;

    /**
     * @description HTTP 状态码：Created 
     */
    export const HTTP_STATUS_CREATED: 201;

    /**
     * @description HTTP 状态码：Accepted 
     */
    export const HTTP_STATUS_ACCEPTED: 202;

    /**
     * @description HTTP 状态码：Non-Authoritative Information 
     */
    export const HTTP_STATUS_NON_AUTHORITATIVE_INFORMATION: 203;

    /**
     * @description HTTP 状态码：No Content 
     */
    export const HTTP_STATUS_NO_CONTENT: 204;

    /**
     * @description HTTP 状态码：Reset Content 
     */
    export const HTTP_STATUS_RESET_CONTENT: 205;

    /**
     * @description HTTP 状态码：Partial Content 
     */
    export const HTTP_STATUS_PARTIAL_CONTENT: 206;

    /**
     * @description HTTP 状态码：Multi-Status 
     */
    export const HTTP_STATUS_MULTI_STATUS: 207;

    /**
     * @description HTTP 状态码：Already Reported 
     */
    export const HTTP_STATUS_ALREADY_REPORTED: 208;

    /**
     * @description HTTP 状态码：IM Used 
     */
    export const HTTP_STATUS_IM_USED: 226;

    /**
     * @description HTTP 状态码：Multiple Choices 
     */
    export const HTTP_STATUS_MULTIPLE_CHOICES: 300;

    /**
     * @description HTTP 状态码：Moved Permanently 
     */
    export const HTTP_STATUS_MOVED_PERMANENTLY: 301;

    /**
     * @description HTTP 状态码：Found 
     */
    export const HTTP_STATUS_FOUND: 302;

    /**
     * @description HTTP 状态码：See Other 
     */
    export const HTTP_STATUS_SEE_OTHER: 303;

    /**
     * @description HTTP 状态码：Not Modified 
     */
    export const HTTP_STATUS_NOT_MODIFIED: 304;

    /**
     * @description HTTP 状态码：Use Proxy 
     */
    export const HTTP_STATUS_USE_PROXY: 305;

    /**
     * @description HTTP 状态码：Temporary Redirect 
     */
    export const HTTP_STATUS_TEMPORARY_REDIRECT: 307;

    /**
     * @description HTTP 状态码：Permanent Redirect 
     */
    export const HTTP_STATUS_PERMANENT_REDIRECT: 308;

    /**
     * @description HTTP 状态码：Bad Request 
     */
    export const HTTP_STATUS_BAD_REQUEST: 400;

    /**
     * @description HTTP 状态码：Unauthorized 
     */
    export const HTTP_STATUS_UNAUTHORIZED: 401;

    /**
     * @description HTTP 状态码：Payment Required 
     */
    export const HTTP_STATUS_PAYMENT_REQUIRED: 402;

    /**
     * @description HTTP 状态码：Forbidden 
     */
    export const HTTP_STATUS_FORBIDDEN: 403;

    /**
     * @description HTTP 状态码：Not Found 
     */
    export const HTTP_STATUS_NOT_FOUND: 404;

    /**
     * @description HTTP 状态码：Method Not Allowed 
     */
    export const HTTP_STATUS_METHOD_NOT_ALLOWED: 405;

    /**
     * @description HTTP 状态码：Not Acceptable 
     */
    export const HTTP_STATUS_NOT_ACCEPTABLE: 406;

    /**
     * @description HTTP 状态码：Proxy Authentication Required 
     */
    export const HTTP_STATUS_PROXY_AUTHENTICATION_REQUIRED: 407;

    /**
     * @description HTTP 状态码：Request Timeout 
     */
    export const HTTP_STATUS_REQUEST_TIMEOUT: 408;

    /**
     * @description HTTP 状态码：Conflict 
     */
    export const HTTP_STATUS_CONFLICT: 409;

    /**
     * @description HTTP 状态码：Gone 
     */
    export const HTTP_STATUS_GONE: 410;

    /**
     * @description HTTP 状态码：Length Required 
     */
    export const HTTP_STATUS_LENGTH_REQUIRED: 411;

    /**
     * @description HTTP 状态码：Precondition Failed 
     */
    export const HTTP_STATUS_PRECONDITION_FAILED: 412;

    /**
     * @description HTTP 状态码：Payload Too Large 
     */
    export const HTTP_STATUS_PAYLOAD_TOO_LARGE: 413;

    /**
     * @description HTTP 状态码：URI Too Long 
     */
    export const HTTP_STATUS_URI_TOO_LONG: 414;

    /**
     * @description HTTP 状态码：Unsupported Media Type 
     */
    export const HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE: 415;

    /**
     * @description HTTP 状态码：Range Not Satisfiable 
     */
    export const HTTP_STATUS_RANGE_NOT_SATISFIABLE: 416;

    /**
     * @description HTTP 状态码：Expectation Failed 
     */
    export const HTTP_STATUS_EXPECTATION_FAILED: 417;

    /**
     * @description HTTP 状态码：I'm a Teapot 
     */
    export const HTTP_STATUS_TEAPOT: 418;

    /**
     * @description HTTP 状态码：Misdirected Request 
     */
    export const HTTP_STATUS_MISDIRECTED_REQUEST: 421;

    /**
     * @description HTTP 状态码：Unprocessable Entity 
     */
    export const HTTP_STATUS_UNPROCESSABLE_ENTITY: 422;

    /**
     * @description HTTP 状态码：Locked 
     */
    export const HTTP_STATUS_LOCKED: 423;

    /**
     * @description HTTP 状态码：Failed Dependency 
     */
    export const HTTP_STATUS_FAILED_DEPENDENCY: 424;

    /**
     * @description HTTP 状态码：Too Early 
     */
    export const HTTP_STATUS_TOO_EARLY: 425;

    /**
     * @description HTTP 状态码：Upgrade Required 
     */
    export const HTTP_STATUS_UPGRADE_REQUIRED: 426;

    /**
     * @description HTTP 状态码：Precondition Required 
     */
    export const HTTP_STATUS_PRECONDITION_REQUIRED: 428;

    /**
     * @description HTTP 状态码：Too Many Requests 
     */
    export const HTTP_STATUS_TOO_MANY_REQUESTS: 429;

    /**
     * @description HTTP 状态码：Request Header Fields Too Large 
     */
    export const HTTP_STATUS_REQUEST_HEADER_FIELDS_TOO_LARGE: 431;

    /**
     * @description HTTP 状态码：Unavailable For Legal Reasons 
     */
    export const HTTP_STATUS_UNAVAILABLE_FOR_LEGAL_REASONS: 451;

    /**
     * @description HTTP 状态码：Internal Server Error 
     */
    export const HTTP_STATUS_INTERNAL_SERVER_ERROR: 500;

    /**
     * @description HTTP 状态码：Not Implemented 
     */
    export const HTTP_STATUS_NOT_IMPLEMENTED: 501;

    /**
     * @description HTTP 状态码：Bad Gateway 
     */
    export const HTTP_STATUS_BAD_GATEWAY: 502;

    /**
     * @description HTTP 状态码：Service Unavailable 
     */
    export const HTTP_STATUS_SERVICE_UNAVAILABLE: 503;

    /**
     * @description HTTP 状态码：Gateway Timeout 
     */
    export const HTTP_STATUS_GATEWAY_TIMEOUT: 504;

    /**
     * @description HTTP 状态码：HTTP Version Not Supported 
     */
    export const HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED: 505;

    /**
     * @description HTTP 状态码：Variant Also Negotiates 
     */
    export const HTTP_STATUS_VARIANT_ALSO_NEGOTIATES: 506;

    /**
     * @description HTTP 状态码：Insufficient Storage 
     */
    export const HTTP_STATUS_INSUFFICIENT_STORAGE: 507;

    /**
     * @description HTTP 状态码：Loop Detected 
     */
    export const HTTP_STATUS_LOOP_DETECTED: 508;

    /**
     * @description HTTP 状态码：Bandwidth Limit Exceeded 
     */
    export const HTTP_STATUS_BANDWIDTH_LIMIT_EXCEEDED: 509;

    /**
     * @description HTTP 状态码：Not Extended 
     */
    export const HTTP_STATUS_NOT_EXTENDED: 510;

    /**
     * @description HTTP 状态码：Network Authentication Required 
     */
    export const HTTP_STATUS_NETWORK_AUTHENTICATION_REQUIRED: 511;

    /**
     * @description HTTP/2 伪头部：:authority 
     */
    export const HTTP2_HEADER_AUTHORITY: ":authority";

    /**
     * @description HTTP/2 头部：accept 
     */
    export const HTTP2_HEADER_ACCEPT: "accept";

    /**
     * @description HTTP/2 头部：accept-charset 
     */
    export const HTTP2_HEADER_ACCEPT_CHARSET: "accept-charset";

    /**
     * @description HTTP/2 头部：accept-encoding 
     */
    export const HTTP2_HEADER_ACCEPT_ENCODING: "accept-encoding";

    /**
     * @description HTTP/2 头部：accept-language 
     */
    export const HTTP2_HEADER_ACCEPT_LANGUAGE: "accept-language";

    /**
     * @description HTTP/2 头部：accept-ranges 
     */
    export const HTTP2_HEADER_ACCEPT_RANGES: "accept-ranges";

    /**
     * @description HTTP/2 头部：access-control-allow-credentials 
     */
    export const HTTP2_HEADER_ACCESS_CONTROL_ALLOW_CREDENTIALS: "access-control-allow-credentials";

    /**
     * @description HTTP/2 头部：access-control-allow-headers 
     */
    export const HTTP2_HEADER_ACCESS_CONTROL_ALLOW_HEADERS: "access-control-allow-headers";

    /**
     * @description HTTP/2 头部：access-control-allow-methods 
     */
    export const HTTP2_HEADER_ACCESS_CONTROL_ALLOW_METHODS: "access-control-allow-methods";

    /**
     * @description HTTP/2 头部：access-control-allow-origin 
     */
    export const HTTP2_HEADER_ACCESS_CONTROL_ALLOW_ORIGIN: "access-control-allow-origin";

    /**
     * @description HTTP/2 头部：access-control-expose-headers 
     */
    export const HTTP2_HEADER_ACCESS_CONTROL_EXPOSE_HEADERS: "access-control-expose-headers";

    /**
     * @description HTTP/2 头部：access-control-max-age 
     */
    export const HTTP2_HEADER_ACCESS_CONTROL_MAX_AGE: "access-control-max-age";

    /**
     * @description HTTP/2 头部：access-control-request-headers 
     */
    export const HTTP2_HEADER_ACCESS_CONTROL_REQUEST_HEADERS: "access-control-request-headers";

    /**
     * @description HTTP/2 头部：access-control-request-method 
     */
    export const HTTP2_HEADER_ACCESS_CONTROL_REQUEST_METHOD: "access-control-request-method";

    /**
     * @description HTTP/2 头部：age 
     */
    export const HTTP2_HEADER_AGE: "age";

    /**
     * @description HTTP/2 头部：allow 
     */
    export const HTTP2_HEADER_ALLOW: "allow";

    /**
     * @description HTTP/2 头部：alt-svc 
     */
    export const HTTP2_HEADER_ALT_SVC: "alt-svc";

    /**
     * @description HTTP/2 头部：authorization 
     */
    export const HTTP2_HEADER_AUTHORIZATION: "authorization";

    /**
     * @description HTTP/2 头部：cache-control 
     */
    export const HTTP2_HEADER_CACHE_CONTROL: "cache-control";

    /**
     * @description HTTP/2 头部：connection 
     */
    export const HTTP2_HEADER_CONNECTION: "connection";

    /**
     * @description HTTP/2 头部：content-disposition 
     */
    export const HTTP2_HEADER_CONTENT_DISPOSITION: "content-disposition";

    /**
     * @description HTTP/2 头部：content-encoding 
     */
    export const HTTP2_HEADER_CONTENT_ENCODING: "content-encoding";

    /**
     * @description HTTP/2 头部：content-language 
     */
    export const HTTP2_HEADER_CONTENT_LANGUAGE: "content-language";

    /**
     * @description HTTP/2 头部：content-length 
     */
    export const HTTP2_HEADER_CONTENT_LENGTH: "content-length";

    /**
     * @description HTTP/2 头部：content-location 
     */
    export const HTTP2_HEADER_CONTENT_LOCATION: "content-location";

    /**
     * @description HTTP/2 头部：content-md5 
     */
    export const HTTP2_HEADER_CONTENT_MD5: "content-md5";

    /**
     * @description HTTP/2 头部：content-range 
     */
    export const HTTP2_HEADER_CONTENT_RANGE: "content-range";

    /**
     * @description HTTP/2 头部：content-security-policy 
     */
    export const HTTP2_HEADER_CONTENT_SECURITY_POLICY: "content-security-policy";

    /**
     * @description HTTP/2 头部：content-type 
     */
    export const HTTP2_HEADER_CONTENT_TYPE: "content-type";

    /**
     * @description HTTP/2 头部：cookie 
     */
    export const HTTP2_HEADER_COOKIE: "cookie";

    /**
     * @description HTTP/2 头部：date 
     */
    export const HTTP2_HEADER_DATE: "date";

    /**
     * @description HTTP/2 头部：dnt 
     */
    export const HTTP2_HEADER_DNT: "dnt";

    /**
     * @description HTTP/2 头部：early-data 
     */
    export const HTTP2_HEADER_EARLY_DATA: "early-data";

    /**
     * @description HTTP/2 头部：etag 
     */
    export const HTTP2_HEADER_ETAG: "etag";

    /**
     * @description HTTP/2 伪头部：:method 
     */
    export const HTTP2_HEADER_METHOD: ":method";

    /**
     * @description HTTP/2 头部：expect 
     */
    export const HTTP2_HEADER_EXPECT: "expect";

    /**
     * @description HTTP/2 头部：expect-ct 
     */
    export const HTTP2_HEADER_EXPECT_CT: "expect-ct";

    /**
     * @description HTTP/2 头部：expires 
     */
    export const HTTP2_HEADER_EXPIRES: "expires";

    /**
     * @description HTTP/2 头部：forwarded 
     */
    export const HTTP2_HEADER_FORWARDED: "forwarded";

    /**
     * @description HTTP/2 头部：from 
     */
    export const HTTP2_HEADER_FROM: "from";

    /**
     * @description HTTP/2 头部：host 
     */
    export const HTTP2_HEADER_HOST: "host";

    /**
     * @description HTTP/2 头部：http2-settings 
     */
    export const HTTP2_HEADER_HTTP2_SETTINGS: "http2-settings";

    /**
     * @description HTTP/2 头部：if-match 
     */
    export const HTTP2_HEADER_IF_MATCH: "if-match";

    /**
     * @description HTTP/2 头部：if-modified-since 
     */
    export const HTTP2_HEADER_IF_MODIFIED_SINCE: "if-modified-since";

    /**
     * @description HTTP/2 头部：if-none-match 
     */
    export const HTTP2_HEADER_IF_NONE_MATCH: "if-none-match";

    /**
     * @description HTTP/2 头部：if-range 
     */
    export const HTTP2_HEADER_IF_RANGE: "if-range";

    /**
     * @description HTTP/2 头部：if-unmodified-since 
     */
    export const HTTP2_HEADER_IF_UNMODIFIED_SINCE: "if-unmodified-since";

    /**
     * @description HTTP/2 头部：keep-alive 
     */
    export const HTTP2_HEADER_KEEP_ALIVE: "keep-alive";

    /**
     * @description HTTP/2 头部：last-modified 
     */
    export const HTTP2_HEADER_LAST_MODIFIED: "last-modified";

    /**
     * @description HTTP/2 头部：link 
     */
    export const HTTP2_HEADER_LINK: "link";

    /**
     * @description HTTP/2 头部：location 
     */
    export const HTTP2_HEADER_LOCATION: "location";

    /**
     * @description HTTP/2 头部：max-forwards 
     */
    export const HTTP2_HEADER_MAX_FORWARDS: "max-forwards";

    /**
     * @description HTTP/2 头部：origin 
     */
    export const HTTP2_HEADER_ORIGIN: "origin";

    /**
     * @description HTTP/2 伪头部：:path 
     */
    export const HTTP2_HEADER_PATH: ":path";

    /**
     * @description HTTP/2 头部：prefer 
     */
    export const HTTP2_HEADER_PREFER: "prefer";

    /**
     * @description HTTP/2 头部：priority 
     */
    export const HTTP2_HEADER_PRIORITY: "priority";

    /**
     * @description HTTP/2 伪头部：:protocol 
     */
    export const HTTP2_HEADER_PROTOCOL: ":protocol";

    /**
     * @description HTTP/2 头部：proxy-authenticate 
     */
    export const HTTP2_HEADER_PROXY_AUTHENTICATE: "proxy-authenticate";

    /**
     * @description HTTP/2 头部：proxy-authorization 
     */
    export const HTTP2_HEADER_PROXY_AUTHORIZATION: "proxy-authorization";

    /**
     * @description HTTP/2 头部：proxy-connection 
     */
    export const HTTP2_HEADER_PROXY_CONNECTION: "proxy-connection";

    /**
     * @description HTTP/2 头部：purpose 
     */
    export const HTTP2_HEADER_PURPOSE: "purpose";

    /**
     * @description HTTP/2 头部：range 
     */
    export const HTTP2_HEADER_RANGE: "range";

    /**
     * @description HTTP/2 头部：referer 
     */
    export const HTTP2_HEADER_REFERER: "referer";

    /**
     * @description HTTP/2 头部：refresh 
     */
    export const HTTP2_HEADER_REFRESH: "refresh";

    /**
     * @description HTTP/2 头部：retry-after 
     */
    export const HTTP2_HEADER_RETRY_AFTER: "retry-after";

    /**
     * @description HTTP/2 伪头部：:scheme 
     */
    export const HTTP2_HEADER_SCHEME: ":scheme";

    /**
     * @description HTTP/2 头部：server 
     */
    export const HTTP2_HEADER_SERVER: "server";

    /**
     * @description HTTP/2 头部：set-cookie 
     */
    export const HTTP2_HEADER_SET_COOKIE: "set-cookie";

    /**
     * @description HTTP/2 伪头部：:status 
     */
    export const HTTP2_HEADER_STATUS: ":status";

    /**
     * @description HTTP/2 头部：strict-transport-security 
     */
    export const HTTP2_HEADER_STRICT_TRANSPORT_SECURITY: "strict-transport-security";

    /**
     * @description HTTP/2 头部：te 
     */
    export const HTTP2_HEADER_TE: "te";

    /**
     * @description HTTP/2 头部：timing-allow-origin 
     */
    export const HTTP2_HEADER_TIMING_ALLOW_ORIGIN: "timing-allow-origin";

    /**
     * @description HTTP/2 头部：tk 
     */
    export const HTTP2_HEADER_TK: "tk";

    /**
     * @description HTTP/2 头部：trailer 
     */
    export const HTTP2_HEADER_TRAILER: "trailer";

    /**
     * @description HTTP/2 头部：transfer-encoding 
     */
    export const HTTP2_HEADER_TRANSFER_ENCODING: "transfer-encoding";

    /**
     * @description HTTP/2 头部：upgrade 
     */
    export const HTTP2_HEADER_UPGRADE: "upgrade";

    /**
     * @description HTTP/2 头部：upgrade-insecure-requests 
     */
    export const HTTP2_HEADER_UPGRADE_INSECURE_REQUESTS: "upgrade-insecure-requests";

    /**
     * @description HTTP/2 头部：user-agent 
     */
    export const HTTP2_HEADER_USER_AGENT: "user-agent";

    /**
     * @description HTTP/2 头部：vary 
     */
    export const HTTP2_HEADER_VARY: "vary";

    /**
     * @description HTTP/2 头部：via 
     */
    export const HTTP2_HEADER_VIA: "via";

    /**
     * @description HTTP/2 头部：warning 
     */
    export const HTTP2_HEADER_WARNING: "warning";

    /**
     * @description HTTP/2 头部：www-authenticate 
     */
    export const HTTP2_HEADER_WWW_AUTHENTICATE: "www-authenticate";

    /**
     * @description HTTP/2 头部：x-content-type-options 
     */
    export const HTTP2_HEADER_X_CONTENT_TYPE_OPTIONS: "x-content-type-options";

    /**
     * @description HTTP/2 头部：x-forwarded-for 
     */
    export const HTTP2_HEADER_X_FORWARDED_FOR: "x-forwarded-for";

    /**
     * @description HTTP/2 头部：x-frame-options 
     */
    export const HTTP2_HEADER_X_FRAME_OPTIONS: "x-frame-options";

    /**
     * @description HTTP/2 头部：x-xss-protection 
     */
    export const HTTP2_HEADER_X_XSS_PROTECTION: "x-xss-protection";

    /**
     * @description HTTP/2 方法：ACL 
     */
    export const HTTP2_METHOD_ACL: "ACL";

    /**
     * @description HTTP/2 方法：BASELINE-CONTROL 
     */
    export const HTTP2_METHOD_BASELINE_CONTROL: "BASELINE-CONTROL";

    /**
     * @description HTTP/2 方法：BIND 
     */
    export const HTTP2_METHOD_BIND: "BIND";

    /**
     * @description HTTP/2 方法：CHECKIN 
     */
    export const HTTP2_METHOD_CHECKIN: "CHECKIN";

    /**
     * @description HTTP/2 方法：CHECKOUT 
     */
    export const HTTP2_METHOD_CHECKOUT: "CHECKOUT";

    /**
     * @description HTTP/2 方法：CONNECT 
     */
    export const HTTP2_METHOD_CONNECT: "CONNECT";

    /**
     * @description HTTP/2 方法：COPY 
     */
    export const HTTP2_METHOD_COPY: "COPY";

    /**
     * @description HTTP/2 方法：DELETE 
     */
    export const HTTP2_METHOD_DELETE: "DELETE";

    /**
     * @description HTTP/2 方法：GET 
     */
    export const HTTP2_METHOD_GET: "GET";

    /**
     * @description HTTP/2 方法：HEAD 
     */
    export const HTTP2_METHOD_HEAD: "HEAD";

    /**
     * @description HTTP/2 方法：LABEL 
     */
    export const HTTP2_METHOD_LABEL: "LABEL";

    /**
     * @description HTTP/2 方法：LINK 
     */
    export const HTTP2_METHOD_LINK: "LINK";

    /**
     * @description HTTP/2 方法：LOCK 
     */
    export const HTTP2_METHOD_LOCK: "LOCK";

    /**
     * @description HTTP/2 方法：MERGE 
     */
    export const HTTP2_METHOD_MERGE: "MERGE";

    /**
     * @description HTTP/2 方法：MKACTIVITY 
     */
    export const HTTP2_METHOD_MKACTIVITY: "MKACTIVITY";

    /**
     * @description HTTP/2 方法：MKCALENDAR 
     */
    export const HTTP2_METHOD_MKCALENDAR: "MKCALENDAR";

    /**
     * @description HTTP/2 方法：MKCOL 
     */
    export const HTTP2_METHOD_MKCOL: "MKCOL";

    /**
     * @description HTTP/2 方法：MKREDIRECTREF 
     */
    export const HTTP2_METHOD_MKREDIRECTREF: "MKREDIRECTREF";

    /**
     * @description HTTP/2 方法：MKWORKSPACE 
     */
    export const HTTP2_METHOD_MKWORKSPACE: "MKWORKSPACE";

    /**
     * @description HTTP/2 方法：MOVE 
     */
    export const HTTP2_METHOD_MOVE: "MOVE";

    /**
     * @description HTTP/2 方法：OPTIONS 
     */
    export const HTTP2_METHOD_OPTIONS: "OPTIONS";

    /**
     * @description HTTP/2 方法：ORDERPATCH 
     */
    export const HTTP2_METHOD_ORDERPATCH: "ORDERPATCH";

    /**
     * @description HTTP/2 方法：PATCH 
     */
    export const HTTP2_METHOD_PATCH: "PATCH";

    /**
     * @description HTTP/2 方法：POST 
     */
    export const HTTP2_METHOD_POST: "POST";

    /**
     * @description HTTP/2 方法：PRI 
     */
    export const HTTP2_METHOD_PRI: "PRI";

    /**
     * @description HTTP/2 方法：PROPFIND 
     */
    export const HTTP2_METHOD_PROPFIND: "PROPFIND";

    /**
     * @description HTTP/2 方法：PROPPATCH 
     */
    export const HTTP2_METHOD_PROPPATCH: "PROPPATCH";

    /**
     * @description HTTP/2 方法：PUT 
     */
    export const HTTP2_METHOD_PUT: "PUT";

    /**
     * @description HTTP/2 方法：REBIND 
     */
    export const HTTP2_METHOD_REBIND: "REBIND";

    /**
     * @description HTTP/2 方法：REPORT 
     */
    export const HTTP2_METHOD_REPORT: "REPORT";

    /**
     * @description HTTP/2 方法：SEARCH 
     */
    export const HTTP2_METHOD_SEARCH: "SEARCH";

    /**
     * @description HTTP/2 方法：TRACE 
     */
    export const HTTP2_METHOD_TRACE: "TRACE";

    /**
     * @description HTTP/2 方法：UNBIND 
     */
    export const HTTP2_METHOD_UNBIND: "UNBIND";

    /**
     * @description HTTP/2 方法：UNCHECKOUT 
     */
    export const HTTP2_METHOD_UNCHECKOUT: "UNCHECKOUT";

    /**
     * @description HTTP/2 方法：UNLINK 
     */
    export const HTTP2_METHOD_UNLINK: "UNLINK";

    /**
     * @description HTTP/2 方法：UNLOCK 
     */
    export const HTTP2_METHOD_UNLOCK: "UNLOCK";

    /**
     * @description HTTP/2 方法：UPDATE 
     */
    export const HTTP2_METHOD_UPDATE: "UPDATE";

    /**
     * @description HTTP/2 方法：UPDATEREDIRECTREF 
     */
    export const HTTP2_METHOD_UPDATEREDIRECTREF: "UPDATEREDIRECTREF";

    /**
     * @description HTTP/2 方法：VERSION-CONTROL 
     */
    export const HTTP2_METHOD_VERSION_CONTROL: "VERSION-CONTROL";

    /**
     * @description 初始窗口大小最大值 
     */
    export const MAX_INITIAL_WINDOW_SIZE: 2147483647;

    /**
     * @description 帧大小最大值 
     */
    export const MAX_MAX_FRAME_SIZE: 16777215;

    /**
     * @description 帧大小最小值 
     */
    export const MIN_MAX_FRAME_SIZE: 16384;

    /**
     * @description NGHTTP2 错误：无错误 
     */
    export const NGHTTP2_NO_ERROR: 0;

    /**
     * @description NGHTTP2 错误：协议错误 
     */
    export const NGHTTP2_PROTOCOL_ERROR: 1;

    /**
     * @description NGHTTP2 错误：内部错误 
     */
    export const NGHTTP2_INTERNAL_ERROR: 2;

    /**
     * @description NGHTTP2 错误：流量控制错误 
     */
    export const NGHTTP2_FLOW_CONTROL_ERROR: 3;

    /**
     * @description NGHTTP2 错误：流已关闭 
     */
    export const NGHTTP2_STREAM_CLOSED: 5;

    /**
     * @description NGHTTP2 错误：帧大小错误 
     */
    export const NGHTTP2_FRAME_SIZE_ERROR: 6;

    /**
     * @description NGHTTP2 错误：流被拒绝 
     */
    export const NGHTTP2_REFUSED_STREAM: 7;

    /**
     * @description NGHTTP2 错误：取消 
     */
    export const NGHTTP2_CANCEL: 8;

    /**
     * @description NGHTTP2 错误：压缩错误 
     */
    export const NGHTTP2_COMPRESSION_ERROR: 9;

    /**
     * @description NGHTTP2 错误：连接错误 
     */
    export const NGHTTP2_CONNECT_ERROR: 10;

    /**
     * @description NGHTTP2 错误：请降速 
     */
    export const NGHTTP2_ENHANCE_YOUR_CALM: 11;

    /**
     * @description NGHTTP2 错误：安全性不足 
     */
    export const NGHTTP2_INADEQUATE_SECURITY: 12;

    /**
     * @description NGHTTP2 错误：需要 HTTP/1.1 
     */
    export const NGHTTP2_HTTP_1_1_REQUIRED: 13;

    /**
     * @description NGHTTP2 内部错误：帧大小错误 
     */
    export const NGHTTP2_ERR_FRAME_SIZE_ERROR: -522;

    /**
     * @description NGHTTP2 Flag：无标志 
     */
    export const NGHTTP2_FLAG_NONE: 0;

    /**
     * @description NGHTTP2 Flag：ACK 
     */
    export const NGHTTP2_FLAG_ACK: 1;

    /**
     * @description NGHTTP2 Flag：END_STREAM 
     */
    export const NGHTTP2_FLAG_END_STREAM: 1;

    /**
     * @description NGHTTP2 Flag：END_HEADERS 
     */
    export const NGHTTP2_FLAG_END_HEADERS: 4;

    /**
     * @description NGHTTP2 Flag：PADDED 
     */
    export const NGHTTP2_FLAG_PADDED: 8;

    /**
     * @description NGHTTP2 Flag：PRIORITY 
     */
    export const NGHTTP2_FLAG_PRIORITY: 32;

    /**
     * @description NGHTTP2 默认权重 
     */
    export const NGHTTP2_DEFAULT_WEIGHT: 16;

    /**
     * @description NGHTTP2 会话类型：服务端 
     */
    export const NGHTTP2_SESSION_SERVER: 0;

    /**
     * @description NGHTTP2 会话类型：客户端 
     */
    export const NGHTTP2_SESSION_CLIENT: 1;

    /**
     * @description NGHTTP2 设置：头部表大小 
     */
    export const NGHTTP2_SETTINGS_HEADER_TABLE_SIZE: 1;

    /**
     * @description NGHTTP2 设置：是否启用推送 
     */
    export const NGHTTP2_SETTINGS_ENABLE_PUSH: 2;

    /**
     * @description NGHTTP2 设置：最大并发流 
     */
    export const NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS: 3;

    /**
     * @description NGHTTP2 设置：初始窗口大小 
     */
    export const NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE: 4;

    /**
     * @description NGHTTP2 设置：超时 
     */
    export const NGHTTP2_SETTINGS_TIMEOUT: 4;

    /**
     * @description NGHTTP2 设置：最大帧大小 
     */
    export const NGHTTP2_SETTINGS_MAX_FRAME_SIZE: 5;

    /**
     * @description NGHTTP2 设置：最大头部列表大小 
     */
    export const NGHTTP2_SETTINGS_MAX_HEADER_LIST_SIZE: 6;

    /**
     * @description NGHTTP2 设置：启用 CONNECT 协议扩展 
     */
    export const NGHTTP2_SETTINGS_ENABLE_CONNECT_PROTOCOL: 8;

    /**
     * @description NGHTTP2 流状态：idle 
     */
    export const NGHTTP2_STREAM_STATE_IDLE: 1;

    /**
     * @description NGHTTP2 流状态：open 
     */
    export const NGHTTP2_STREAM_STATE_OPEN: 2;

    /**
     * @description NGHTTP2 流状态：reserved local 
     */
    export const NGHTTP2_STREAM_STATE_RESERVED_LOCAL: 3;

    /**
     * @description NGHTTP2 流状态：reserved remote 
     */
    export const NGHTTP2_STREAM_STATE_RESERVED_REMOTE: 4;

    /**
     * @description NGHTTP2 流状态：half closed local 
     */
    export const NGHTTP2_STREAM_STATE_HALF_CLOSED_LOCAL: 5;

    /**
     * @description NGHTTP2 流状态：half closed remote 
     */
    export const NGHTTP2_STREAM_STATE_HALF_CLOSED_REMOTE: 6;

    /**
     * @description NGHTTP2 流状态：closed 
     */
    export const NGHTTP2_STREAM_STATE_CLOSED: 7;

    /**
     * @description 填充策略：不填充 
     */
    export const PADDING_STRATEGY_NONE: 0;

    /**
     * @description 填充策略：对齐 
     */
    export const PADDING_STRATEGY_ALIGNED: 1;

    /**
     * @description 填充策略：回调（等同于 ALIGNED） 
     */
    export const PADDING_STRATEGY_CALLBACK: 1;

    /**
     * @description 填充策略：最大填充 
     */
    export const PADDING_STRATEGY_MAX: 2;

}

