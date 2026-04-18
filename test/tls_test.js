var { describe, odescribe, it, after, afterEach } = require('node:test');
var assert = require('assert');

var test_util = require('./test_util');

var android = process.platform === 'android';

var tls = require("tls");
var crypto = require("crypto");
var fs = require('fs');
var io = require('io');
var path = require('path');
var net = require('net');
var coroutine = require('coroutine');

var base_port = coroutine.vmid * 10000;

describe('tls', () => {
    function test_tls(name, pk, pk1) {
        describe(name, () => {
            var ca = crypto.createCertificateRequest({
                key: pk.privateKey,
                subject: {
                    CN: "fibjs.org"
                }
            }).issue({
                key: pk.privateKey,
                ca: true,
                validFrom: new Date(new Date() - 1000),
                issuer: {
                    CN: "fibjs.org"
                }
            });

            var crt = crypto.createCertificateRequest({
                key: pk1.privateKey,
                subject: {
                    CN: "localhost"
                }
            }).issue({
                key: pk.privateKey,
                validFrom: new Date(new Date() - 1000),
                issuer: {
                    CN: "fibjs.org"
                }
            });

            var caPemBuffer = Buffer.from(ca.pem);
            var crtPemBuffer = Buffer.from(crt.pem);

            var resolve_cnt = 0;
            function sni_resolver(domain) {
                resolve_cnt++;

                if (domain === "no_cert")
                    return;

                var cert = crypto.createCertificateRequest({
                    key: pk.privateKey,
                    subject: {
                        CN: domain
                    }
                }).issue({
                    key: pk.privateKey,
                    validFrom: new Date(new Date() - 1000),
                    issuer: {
                        CN: "fibjs.org"
                    }
                });

                return tls.createSecureContext({
                    key: pk.privateKey,
                    cert: cert
                });
            }

            const ctx = tls.createSecureContext({
                ca: ca
            });

            const ctx_svr = tls.createSecureContext({
                key: pk1.privateKey,
                cert: crt
            }, true);

            after(test_util.cleanup);

            describe('createSecureContext', () => {
                it('default', () => {
                    var ctx = tls.createSecureContext();
                    assert.equal(ctx.sessionTimeout, 7200);
                    assert.equal(ctx.requestCert, true);
                    assert.equal(ctx.rejectUnverified, true);
                    assert.equal(ctx.rejectUnauthorized, true);
                    assert.isUndefined(ctx.minVersion);
                    assert.isUndefined(ctx.maxVersion);
                    assert.isUndefined(ctx.cert);
                    assert.isUndefined(ctx.key);

                    var ctx = tls.createSecureContext(true);
                    assert.equal(ctx.requestCert, true);
                    assert.equal(ctx.rejectUnauthorized, false);
                });

                it('version', () => {
                    var ctx = tls.createSecureContext({
                        minVersion: "TLSv1.2",
                        maxVersion: "TLSv1.3"
                    });

                    assert.equal(ctx.minVersion, "TLSv1.2");
                    assert.equal(ctx.maxVersion, "TLSv1.3");
                });

                it('cert/key', () => {
                    var ctx = tls.createSecureContext({
                        key: pk1.privateKey,
                        cert: crt
                    });

                    assert.equal(ctx.cert, crt);
                    assert.ok(ctx.key.equals(pk1.privateKey));
                });

                it('cert/ca accept Buffer inputs', () => {
                    var ctx = tls.createSecureContext({
                        key: pk1.privateKey,
                        cert: crtPemBuffer,
                        ca: caPemBuffer
                    });

                    assert.equal(ctx.cert.subject, crt.subject);
                    assert.equal(ctx.ca.subject, ca.subject);
                    assert.ok(ctx.key.equals(pk1.privateKey));
                });

                it('cert accepts Buffer array chain', () => {
                    var ctx = tls.createSecureContext({
                        key: pk1.privateKey,
                        cert: [crtPemBuffer, caPemBuffer],
                        ca: [caPemBuffer]
                    });

                    assert.equal(ctx.cert.subject, crt.subject);
                    assert.equal(ctx.cert.next().subject, ca.subject);
                    assert.equal(ctx.ca.subject, ca.subject);
                });

                it('key/cert undefined — ignored, no error thrown', () => {
                    // {key: undefined} must be treated as absent (same as Node.js)
                    var ctx = tls.createSecureContext({
                        key: undefined,
                        cert: undefined,
                        ca: undefined
                    });
                    assert.isUndefined(ctx.key);
                    assert.isUndefined(ctx.cert);
                });

                it('key/cert null — ignored, no error thrown', () => {
                    // {key: null} must also be treated as absent
                    var ctx = tls.createSecureContext({
                        key: null,
                        cert: null,
                        ca: null
                    });
                    assert.isUndefined(ctx.key);
                    assert.isUndefined(ctx.cert);
                });

                it('requestCert/rejectUnauthorized', () => {
                    var ctx = tls.createSecureContext({
                        requestCert: false,
                        rejectUnauthorized: false
                    });

                    assert.equal(ctx.requestCert, false);
                    assert.equal(ctx.rejectUnauthorized, false);

                    var ctx = tls.createSecureContext({
                        requestCert: false,
                        rejectUnauthorized: true
                    });

                    assert.equal(ctx.requestCert, false);
                    assert.equal(ctx.rejectUnauthorized, false);

                    var ctx = tls.createSecureContext({
                        requestCert: true,
                        rejectUnauthorized: false
                    });

                    assert.equal(ctx.requestCert, true);
                    assert.equal(ctx.rejectUnauthorized, false);

                    var ctx = tls.createSecureContext({
                        requestCert: true,
                        rejectUnauthorized: true
                    });

                    assert.equal(ctx.requestCert, true);
                    assert.equal(ctx.rejectUnauthorized, true);
                });

                it('rejectUnverified', () => {
                    var ctx = tls.createSecureContext({
                        rejectUnverified: false
                    });

                    assert.equal(ctx.rejectUnverified, false);
                });

                describe('SNIContext', () => {
                    it('set/get', () => {
                        var ctx = tls.createSecureContext(true);
                        ctx.setSNIContext("test", sni_resolver("test"));
                        ctx.setSNIContext("test1", sni_resolver("test1"));

                        assert.equal(ctx.getSNIContext("test").cert.subject, 'CN=test');
                        assert.equal(ctx.getSNIContext("test1").cert.subject, 'CN=test1');
                    });

                    it('resolver', () => {
                        var ctx = tls.createSecureContext({
                            "SNIResolver": sni_resolver
                        }, true);

                        assert.equal(ctx.getSNIContext("test", true).cert.subject, 'CN=test');
                        assert.equal(ctx.getSNIContext("test1", true).cert.subject, 'CN=test1');

                        var n = resolve_cnt;
                        assert.equal(ctx.getSNIContext("test", true).cert.subject, 'CN=test');
                        assert.equal(n, resolve_cnt);
                    });

                    it('delete', () => {
                        var ctx = tls.createSecureContext(true);
                        ctx.setSNIContext("test", sni_resolver("test"));
                        ctx.setSNIContext("test1", sni_resolver("test1"));

                        ctx.removeSNIContext("test");

                        assert.equal(ctx.getSNIContext("test"), undefined);
                        assert.equal(ctx.getSNIContext("test1").cert.subject, 'CN=test1');
                    });

                    it('throw error in resolver', () => {
                        var ctx = tls.createSecureContext({
                            "SNIResolver": (domain) => {
                                throw new Error("test");
                            }
                        }, true);

                        assert.equal(ctx.getSNIContext("test", true), undefined);
                    });

                    it('return nothing in resolver', () => {
                        var ctx = tls.createSecureContext({
                            "SNIResolver": (domain) => {
                            }
                        }, true);

                        assert.equal(ctx.getSNIContext("test", true), undefined);
                    });

                    it('deleted during resolve', () => {
                        var ctx = tls.createSecureContext({
                            "SNIResolver": (domain) => {
                                ctx.removeSNIContext(domain);
                                return sni_resolver(domain);
                            }
                        }, true);

                        assert.equal(ctx.getSNIContext("test", true).cert.subject, 'CN=test');
                        assert.equal(ctx.getSNIContext("test"), undefined);
                    });

                    it('deleted during resolve return nothing', () => {
                        var ctx = tls.createSecureContext({
                            "SNIResolver": (domain) => {
                                ctx.removeSNIContext(domain);
                                return;
                            }
                        }, true);

                        assert.equal(ctx.getSNIContext("test", true), undefined);
                        assert.equal(ctx.getSNIContext("test"), undefined);
                    });

                    it('size', () => {
                        var ctx = tls.createSecureContext({
                            "SNICacheSize": 2
                        }, true);
                        ctx.setSNIContext("test", sni_resolver("test"));
                        ctx.setSNIContext("test1", sni_resolver("test1"));
                        ctx.setSNIContext("test2", sni_resolver("test2"));

                        assert.equal(ctx.getSNIContext("test"), undefined);
                        assert.equal(ctx.getSNIContext("test1").cert.subject, 'CN=test1');
                        assert.equal(ctx.getSNIContext("test2").cert.subject, 'CN=test2');
                    });

                    it('SNICacheIdleTimeout', () => {
                        var ctx = tls.createSecureContext({
                            "SNICacheIdleTimeout": 1
                        }, true);
                        ctx.setSNIContext("test", sni_resolver("test"));

                        for (var i = 0; i < 10; i++) {
                            assert.notEqual(ctx.getSNIContext("test"), undefined);
                            coroutine.sleep(150);
                        }

                        assert.notEqual(ctx.getSNIContext("test"), undefined);
                    });

                    it('SNICacheTimeout', () => {
                        var ctx = tls.createSecureContext({
                            "SNICacheTimeout": 1
                        }, true);
                        ctx.setSNIContext("test", sni_resolver("test"));

                        for (var i = 0; i < 10; i++) {
                            ctx.getSNIContext("test");
                            coroutine.sleep(150);
                        }

                        assert.equal(ctx.getSNIContext("test"), undefined);
                    });

                    it('memory recycling', () => {
                        var ctx = tls.createSecureContext({
                            "SNIResolver": sni_resolver,
                            "SNICacheSize": 2
                        }, true);

                        ctx.getSNIContext("test", true);
                        ctx.getSNIContext("test1", true);

                        test_util.gc();
                        const cnt = test_util.countObject("SecureContext");

                        for (var i = 0; i < 10; i++)
                            ctx.getSNIContext("test" + i, true);

                        test_util.gc();
                        const cnt1 = test_util.countObject("SecureContext");
                        assert.equal(cnt, cnt1);
                    });
                });
            });

            it("root ca", () => {
                var cert = new crypto.X509Certificate(fs.readFile(path.join(__dirname, 'cert_files', 'ca-bundle.crt')));
                var s = cert.pem;

                var s1 = tls.secureContext.ca.pem;

                assert.equal(s, s1);
            });

            it("echo server", () => {
                var svr = new net.TcpServer(9080 + base_port, (s) => {
                    var ss;
                    var buf;

                    test_util.push(s);
                    var ss = new tls.TLSSocket(ctx_svr);

                    try {
                        ss.accept(s);

                        buf = ss.read();
                        if (buf.toString() === "double") {
                            ss.write("1234567890");
                            ss.write("1234567890");
                        } else if (buf.toString() === "double sleep") {
                            ss.write("1234567890");
                            coroutine.sleep(10);
                            ss.write("1234567890");
                        } else if (buf.toString() === "no_close") {
                            s.close();
                            return;
                        } else
                            ss.write(buf);

                        ss.close();
                    } catch (e) { }
                });
                svr.start();

                test_util.push(svr.socket);
            });

            describe('TLSSocket', () => {
                function connect() {
                    var s1 = new net.Socket();
                    s1.connect(9080 + base_port, "127.0.0.1");
                    test_util.push(s1);

                    var ss = new tls.TLSSocket(ctx);
                    ss.connect(s1);
                    return ss;
                }

                it("echo", () => {
                    for (var i = 0; i < 30; i++) {
                        var ss = connect();
                        ss.write("GET / HTTP/1.0");
                        assert.equal("GET / HTTP/1.0", ss.read());
                    }
                });

                it("async connect with events", () => {
                    var connectEvent = new coroutine.Event();
                    var connected = false;
                    var receivedData = null;

                    var s1 = new net.Socket();
                    s1.connect(9080 + base_port, "127.0.0.1");
                    test_util.push(s1);

                    var ss = new tls.TLSSocket(ctx);
                    ss.on('connect', function () {
                        connected = true;
                        this.write("GET / HTTP/1.0");
                    });
                    ss.on('data', function (data) {
                        receivedData = data.toString();
                        connectEvent.set();
                    });
                    ss.on('error', function (err) {
                        console.log('error:', err);
                        connectEvent.set();
                    });

                    ss.connect(s1);

                    connectEvent.wait();
                    assert.ok(connected);
                    assert.equal(receivedData, "GET / HTTP/1.0");
                    ss.close();
                    ss = null;
                });

                it("on data after connect success", () => {
                    var dataEvent = new coroutine.Event();
                    var receivedData = null;

                    var s1 = new net.Socket();
                    s1.connect(9080 + base_port, "127.0.0.1");
                    test_util.push(s1);

                    var ss = new tls.TLSSocket(ctx);
                    ss.connect(s1);

                    // Register on data after connect success
                    ss.on('data', function (data) {
                        receivedData = data.toString();
                        dataEvent.set();
                    });

                    ss.write("GET / HTTP/1.0");

                    dataEvent.wait();
                    assert.equal(receivedData, "GET / HTTP/1.0");
                    ss.close();
                    ss = null;
                });

                it("async connect error when handshake fails", () => {
                    test_util.gc();
                    var tlsSocketCount = test_util.countObject('TLSSocket');

                    var errorEvent = new coroutine.Event();
                    var errorReceived = null;

                    // Create a raw TCP connection to a TLS server, then connect TLSSocket
                    // with wrong certificates - this will cause handshake failure
                    var s1 = new net.Socket();
                    s1.connect(9080 + base_port, "127.0.0.1");
                    test_util.push(s1);

                    // Create TLS socket with wrong context (no proper CA)
                    var wrongCtx = tls.createSecureContext({});
                    var ss = new tls.TLSSocket(wrongCtx);
                    ss.on('connect', function () {
                        errorEvent.set();
                    });
                    ss.on('data', function (data) {
                        errorEvent.set();
                    });
                    ss.on('error', function (err) {
                        errorReceived = err;
                        errorEvent.set();
                    });

                    ss.connect(s1);

                    errorEvent.wait();
                    assert.ok(errorReceived !== null);

                    // Close the underlying socket to trigger server side error
                    s1.close();

                    // Wait for server side to finish processing
                    coroutine.sleep(100);

                    // Verify TLSSocket is released after error
                    ss = null;
                    errorReceived = null;
                    test_util.gc();
                    assert.equal(tlsSocketCount, test_util.countObject('TLSSocket'));
                });

                it("no leak when on data but no connect", () => {
                    test_util.gc();
                    var tlsSocketCount = test_util.countObject('TLSSocket');

                    var ss = new tls.TLSSocket(ctx);
                    ss.on('data', function (data) { });

                    ss = null;
                    test_util.gc();
                    assert.equal(tlsSocketCount, test_util.countObject('TLSSocket'));
                });

                it("no leak when just create TLSSocket", () => {
                    test_util.gc();
                    var tlsSocketCount = test_util.countObject('TLSSocket');

                    var ss = new tls.TLSSocket(ctx);

                    ss = null;
                    test_util.gc();
                    assert.equal(tlsSocketCount, test_util.countObject('TLSSocket'));
                });

                it("read specific bytes", () => {
                    var ss = connect();
                    ss.write("GET / HTTP/1.0");
                    assert.equal("GET /", ss.read(5));
                });

                it("read left bytes", () => {
                    var ss = connect();
                    ss.write("GET / HTTP/1.0");
                    assert.equal("GET /", ss.read(5));
                    assert.equal(" HTTP/1.0", ss.read());
                });

                it("read specific bytes to end", () => {
                    var ss = connect();
                    ss.write("GET / HTTP/1.0");
                    assert.equal("GET / HTTP/1.0", ss.read(100));
                });

                it("read multi package", () => {
                    var ss = connect();
                    ss.write("double");
                    coroutine.sleep(10);
                    assert.equal("123456789012345", ss.read(15));
                    assert.equal("67890", ss.read(15));

                    var ss = connect();
                    ss.write("double sleep");
                    assert.equal("123456789012345", ss.read(15));
                    assert.equal("67890", ss.read(15));
                });

                it("read specific bytes from end", () => {
                    var ss = connect();
                    ss.write("GET / HTTP/1.0");
                    assert.equal("GET / HTTP/1.0", ss.read(14));
                    assert.equal(null, ss.read(100));
                });

                it("read specific bytes from nonclosed end", () => {
                    var ss = connect();
                    ss.write("no_close");
                    assert.equal(null, ss.read(100));
                });

                it("read from end", () => {
                    var ss = connect();
                    ss.write("GET / HTTP/1.0");
                    assert.equal("GET / HTTP/1.0", ss.read(14));
                    assert.equal(null, ss.read());
                });

                it("read from nonclosed end", () => {
                    var ss = connect();
                    ss.write("no_close");
                    assert.equal(null, ss.read());
                });

                it("write return value validation", () => {
                    var ss = connect();

                    // Test write return value with string (write now returns Boolean)
                    var testData = 'Hello TLS World!';
                    var bytesWritten = ss.write(testData);
                    assert.equal(bytesWritten, true);

                    // Test write return value with Buffer
                    var testBuffer = new Buffer('TLS Buffer Data');
                    bytesWritten = ss.write(testBuffer);
                    assert.equal(bytesWritten, true);

                    // Test write return value with empty string
                    bytesWritten = ss.write('');
                    assert.equal(bytesWritten, true);

                    ss.close();
                });

                it("connect with connectListener", () => {
                    var connectEvent = new coroutine.Event();
                    var connected = false;
                    var connectError = null;

                    var s1 = new net.Socket();
                    s1.connect(9080 + base_port, "127.0.0.1");
                    test_util.push(s1);

                    var ss = new tls.TLSSocket(ctx);
                    ss.on('error', function (err) {
                        connectError = err;
                        connectEvent.set();
                    });
                    ss.connect(s1, function () {
                        connected = true;
                        connectEvent.set();
                    });

                    connectEvent.wait();
                    assert.ok(!connectError, connectError ? connectError.message : '');
                    assert.ok(connected);

                    ss.write("GET / HTTP/1.0");
                    assert.equal("GET / HTTP/1.0", ss.read());
                    ss.close();
                });

                it("connect with server_name and connectListener", () => {
                    var connectEvent = new coroutine.Event();
                    var connected = false;
                    var connectError = null;

                    var s1 = new net.Socket();
                    s1.connect(9080 + base_port, "127.0.0.1");
                    test_util.push(s1);

                    var ss = new tls.TLSSocket(ctx);
                    ss.on('error', function (err) {
                        connectError = err;
                        connectEvent.set();
                    });
                    ss.connect(s1, "localhost", function () {
                        connected = true;
                        connectEvent.set();
                    });

                    connectEvent.wait();
                    assert.ok(!connectError, connectError ? connectError.message : '');
                    assert.ok(connected);

                    ss.write("GET / HTTP/1.0");
                    assert.equal("GET / HTTP/1.0", ss.read());
                    ss.close();
                });

                // ...existing code...
            });

            describe('verification', () => {
                function connect() {
                    var s1 = new net.Socket();
                    s1.connect(9080 + base_port, "127.0.0.1");
                    test_util.push(s1);
                    return s1;
                }

                it('root ca', () => {
                    var ss = new tls.TLSSocket();
                    assert.throws(() => {
                        ss.connect(connect());
                    });
                });

                it('custom ca', () => {
                    var ss = new tls.TLSSocket({
                        ca: ca
                    });
                    ss.connect(connect());
                });

                it('verify server name', () => {
                    var ss = new tls.TLSSocket({
                        ca: ca
                    });
                    ss.connect(connect(), "localhost");

                    var ss = new tls.TLSSocket({
                        ca: ca
                    });
                    assert.throws(() => {
                        ss.connect(connect(), "fibjs.org");
                    });
                });

                it('not request cert', () => {
                    var ss = new tls.TLSSocket({
                        requestCert: false
                    });
                    ss.connect(connect());
                    assert.equal(ss.getPeerX509Certificate().pem, crt.pem);

                    var ss = new tls.TLSSocket({
                        ca: ca,
                        requestCert: false
                    });
                    ss.connect(connect(), "fibjs.org");
                    assert.equal(ss.getPeerX509Certificate().pem, crt.pem);
                });

                it('not verify cert', () => {
                    var ss = new tls.TLSSocket({
                        rejectUnverified: false
                    });
                    ss.connect(connect());
                    assert.equal(ss.getPeerX509Certificate().pem, crt.pem);

                    var ss = new tls.TLSSocket({
                        ca: ca,
                        rejectUnverified: false
                    });
                    ss.connect(connect(), "fibjs.org");
                    assert.equal(ss.getPeerX509Certificate().pem, crt.pem);
                });
            });

            (android ? describe.skip : describe)('tls.connect', () => {
                it('with context', () => {
                    var ss = tls.connect(`ssl://localhost:${9080 + base_port}`, ctx);
                    ss.write("GET / HTTP/1.0");
                    assert.equal("GET / HTTP/1.0", ss.read());
                });

                it('with options', () => {
                    var ss = tls.connect(`ssl://localhost:${9080 + base_port}`, {
                        ca: ca
                    });
                    ss.write("GET / HTTP/1.0");
                    assert.equal("GET / HTTP/1.0", ss.read());
                });

                it('with context in options', () => {
                    var ss = tls.connect(`ssl://localhost:${9080 + base_port}`, {
                        secureContext: ctx
                    });
                    ss.write("GET / HTTP/1.0");
                    assert.equal("GET / HTTP/1.0", ss.read());
                });

                it('default context', () => {
                    assert.throws(() => {
                        tls.connect(`ssl://localhost:${9080 + base_port}`);
                    });
                });

                it('with port, host, options', () => {
                    var ss = tls.connect(9080 + base_port, "localhost", {
                        secureContext: ctx
                    });
                    ss.write("GET / HTTP/1.0");
                    assert.equal("GET / HTTP/1.0", ss.read());
                });

                it('with options object containing port and host', () => {
                    var ss = tls.connect({
                        port: 9080 + base_port,
                        host: "localhost",
                        secureContext: ctx
                    });
                    ss.write("GET / HTTP/1.0");
                    assert.equal("GET / HTTP/1.0", ss.read());
                });

                it('with url and connectListener', () => {
                    var connectEvent = new coroutine.Event();
                    var connected = false;
                    var connectError = null;

                    var ss = tls.connect(`ssl://localhost:${9080 + base_port}`, ctx, function () {
                        connected = true;
                        connectEvent.set();
                    });
                    ss.on('error', function (err) {
                        connectError = err;
                        connectEvent.set();
                    });

                    connectEvent.wait();
                    assert.ok(!connectError, connectError ? connectError.message : '');
                    assert.ok(connected);

                    ss.write("GET / HTTP/1.0");
                    assert.equal("GET / HTTP/1.0", ss.read());
                    ss.close();
                });

                it('with url, timeout and connectListener', () => {
                    var connectEvent = new coroutine.Event();
                    var connected = false;
                    var connectError = null;

                    var ss = tls.connect(`ssl://localhost:${9080 + base_port}`, ctx, 0, function () {
                        connected = true;
                        connectEvent.set();
                    });
                    ss.on('error', function (err) {
                        connectError = err;
                        connectEvent.set();
                    });

                    connectEvent.wait();
                    assert.ok(!connectError, connectError ? connectError.message : '');
                    assert.ok(connected);

                    ss.write("GET / HTTP/1.0");
                    assert.equal("GET / HTTP/1.0", ss.read());
                    ss.close();
                });

                it('with url, options and connectListener', () => {
                    var connectEvent = new coroutine.Event();
                    var connected = false;
                    var connectError = null;

                    var ss = tls.connect(`ssl://localhost:${9080 + base_port}`, {
                        ca: ca
                    }, function () {
                        connected = true;
                        connectEvent.set();
                    });
                    ss.on('error', function (err) {
                        connectError = err;
                        connectEvent.set();
                    });

                    connectEvent.wait();
                    assert.ok(!connectError, connectError ? connectError.message : '');
                    assert.ok(connected);

                    ss.write("GET / HTTP/1.0");
                    assert.equal("GET / HTTP/1.0", ss.read());
                    ss.close();
                });

                it('with port, host, options and connectListener', () => {
                    var connectEvent = new coroutine.Event();
                    var connected = false;
                    var connectError = null;

                    var ss = tls.connect(9080 + base_port, "localhost", {
                        secureContext: ctx
                    }, function () {
                        connected = true;
                        connectEvent.set();
                    });
                    ss.on('error', function (err) {
                        connectError = err;
                        connectEvent.set();
                    });

                    connectEvent.wait();
                    assert.ok(!connectError, connectError ? connectError.message : '');
                    assert.ok(connected);

                    ss.write("GET / HTTP/1.0");
                    assert.equal("GET / HTTP/1.0", ss.read());
                    ss.close();
                });

                it('with port, options and connectListener', () => {
                    var connectEvent = new coroutine.Event();
                    var connected = false;
                    var connectError = null;

                    var ss = tls.connect(9080 + base_port, {
                        host: "localhost",
                        secureContext: ctx
                    }, function () {
                        connected = true;
                        connectEvent.set();
                    });
                    ss.on('error', function (err) {
                        connectError = err;
                        connectEvent.set();
                    });

                    connectEvent.wait();
                    assert.ok(!connectError, connectError ? connectError.message : '');
                    assert.ok(connected);

                    ss.write("GET / HTTP/1.0");
                    assert.equal("GET / HTTP/1.0", ss.read());
                    ss.close();
                });

                it('with options and connectListener', () => {
                    var connectEvent = new coroutine.Event();
                    var connected = false;
                    var connectError = null;

                    var ss = tls.connect({
                        port: 9080 + base_port,
                        host: "localhost",
                        secureContext: ctx
                    }, function () {
                        connected = true;
                        connectEvent.set();
                    });
                    ss.on('error', function (err) {
                        connectError = err;
                        connectEvent.set();
                    });

                    connectEvent.wait();
                    assert.ok(!connectError, connectError ? connectError.message : '');
                    assert.ok(connected);

                    ss.write("GET / HTTP/1.0");
                    assert.equal("GET / HTTP/1.0", ss.read());
                    ss.close();
                });
            });

            it("copyTo", () => {
                var str = "012345678901234567890123456789";

                for (var i = 0; i < 10; i++)
                    str = str + str;

                var svr = new net.TcpServer(9082 + base_port, (s) => {
                    var ss = new tls.TLSSocket(ctx_svr);
                    ss.accept(s);

                    var strm = new io.MemoryStream();
                    strm.write(str);
                    strm.seek(0, io.SEEK_SET);
                    assert.equal(strm.copyTo(ss), str.length);

                    ss.close();
                    s.close();
                });
                test_util.push(svr.socket);
                svr.start();

                function t_conn() {
                    var c1 = new net.Socket();
                    c1.connect(9082 + base_port, "127.0.0.1");

                    var ss = new tls.TLSSocket(ctx);
                    ss.connect(c1);

                    var strm = new io.MemoryStream();
                    assert.equal(ss.copyTo(strm), str.length);

                    ss.close();
                    c1.close();

                    strm.seek(0, io.SEEK_SET);
                    assert.equal(str, strm.readAll().toString());
                }

                for (var i = 0; i < 5; i++) {
                    str = str + str;
                    t_conn();
                }

                str = undefined;
            });

            it("Handler", () => {
                var svr = new net.TcpServer(9083 + base_port, new tls.Handler(ctx_svr, (s) => {
                    var buf;

                    while (buf = s.read())
                        s.write(buf);
                }));
                test_util.push(svr.socket);
                svr.start();

                for (var i = 0; i < 10; i++) {
                    var s1 = new net.Socket();
                    s1.connect(9083 + base_port, "127.0.0.1");

                    var cs = new tls.TLSSocket(ctx);
                    cs.connect(s1);

                    cs.write("GET / HTTP/1.0");
                    assert.equal("GET / HTTP/1.0", cs.read());

                    cs.close();
                    s1.close();
                }
            });

            it("Server", () => {
                var svr = new tls.Server(ctx_svr, 9084 + base_port, (s) => {
                    var buf;

                    while (buf = s.read())
                        s.write(buf);
                });
                test_util.push(svr.socket);
                svr.start();

                for (var i = 0; i < 10; i++) {
                    var s1 = new net.Socket();
                    s1.connect(9084 + base_port, "127.0.0.1");

                    var cs = new tls.TLSSocket(ctx);
                    cs.connect(s1);

                    cs.write("GET / HTTP/1.0");
                    assert.equal("GET / HTTP/1.0", cs.read());

                    cs.close();
                    s1.close();
                }
            });

            const ctx_sni = tls.createSecureContext({
                key: pk1.privateKey,
                cert: crt,
                SNIResolver: sni_resolver
            }, true);

            it("SNI Server", () => {
                var svr = new tls.Server(ctx_sni, 9086 + base_port, (s) => {
                    var buf;

                    while (buf = s.read())
                        s.write(buf);
                });
                test_util.push(svr.socket);
                svr.start();

                var s1 = new net.Socket();
                s1.connect(9086 + base_port, "127.0.0.1");

                var cs = new tls.TLSSocket(ctx);
                cs.connect(s1, "test1");

                var cert = cs.getPeerX509Certificate();
                assert.equal(cert.subject, 'CN=test1');

                cs.close();

                var s2 = new net.Socket();
                s2.connect(9086 + base_port, "127.0.0.1");
                var cs1 = new tls.TLSSocket(ctx);

                assert.throws(() => {
                    cs1.connect(s2, "no_cert");
                });

                cs1.close();
            });

            // TLSServer inherits TcpServer which inherits EventEmitter
            describe("TLSServer EventEmitter events", () => {
                var svr;

                afterEach(() => {
                    if (svr) {
                        svr.stop();
                        svr = null;
                    }
                });

                it("emits 'listening' after start()", () => {
                    var fired = false;
                    svr = new tls.Server(ctx_svr, 9088 + base_port, (s) => { s.close(); });
                    svr.on('listening', () => { fired = true; });
                    svr.start();
                    test_util.push(svr.socket);
                    coroutine.sleep(0);
                    assert.strictEqual(fired, true);
                });

                it("emits 'connection' when a client connects", () => {
                    var conns = 0;
                    svr = new tls.Server(ctx_svr, 9089 + base_port, (s) => { s.close(); });
                    svr.on('connection', () => { conns++; });
                    svr.start();
                    test_util.push(svr.socket);

                    var s1 = new net.Socket();
                    s1.connect(9089 + base_port, '127.0.0.1');
                    var cs = new tls.TLSSocket(ctx);
                    try { cs.connect(s1); } catch (e) { }
                    cs.close();
                    s1.close();

                    for (var i = 0; i < 10 && conns < 1; i++)
                        coroutine.sleep(0);
                    assert.ok(conns >= 1);
                });

                it("emits 'close' after stop()", () => {
                    var closed = false;
                    svr = new tls.Server(ctx_svr, 9090 + base_port, (s) => { s.close(); });
                    svr.on('close', () => { closed = true; });
                    svr.start();
                    test_util.push(svr.socket);
                    svr.stop();
                    for (var i = 0; i < 10 && !closed; i++)
                        coroutine.sleep(0);
                    svr = null;
                    assert.strictEqual(closed, true);
                });

                it("onlistening shorthand works", () => {
                    var fired = false;
                    svr = new tls.Server(ctx_svr, 9091 + base_port, (s) => { s.close(); });
                    svr.onlistening = () => { fired = true; };
                    svr.start();
                    test_util.push(svr.socket);
                    for (var i = 0; i < 10 && !fired; i++)
                        coroutine.sleep(0);
                    assert.strictEqual(fired, true);
                });
            });

            // TLSServer listen() mode — no-port constructor + listen()
            describe("TLSServer listen() mode", () => {
                var svr;

                afterEach(() => {
                    if (svr) {
                        svr.stop();
                        svr = null;
                    }
                });

                it("listen(port) binds and accepts TLS connections", () => {
                    var p = 9092 + base_port;
                    svr = new tls.Server(ctx_svr, (s) => { s.close(); });
                    svr.listen(p);
                    test_util.push(svr.socket);

                    var s1 = new net.Socket();
                    s1.connect(p, '127.0.0.1');
                    var cs = new tls.TLSSocket(ctx);
                    try { cs.connect(s1); } catch (e) { }
                    cs.close();
                    s1.close();
                });

                it("listen() emits 'listening' event", () => {
                    var fired = false;
                    svr = new tls.Server(ctx_svr, (s) => { s.close(); });
                    svr.on('listening', () => { fired = true; });
                    svr.listen(9093 + base_port);
                    test_util.push(svr.socket);
                    coroutine.sleep(0);
                    assert.strictEqual(fired, true);
                });

                it("double listen() throws", () => {
                    svr = new tls.Server(ctx_svr, (s) => { s.close(); });
                    svr.listen(9094 + base_port);
                    test_util.push(svr.socket);
                    assert.throws(() => { svr.listen(9095 + base_port); });
                });
            });

            describe("createServer", () => {
                var svr;

                afterEach(() => {
                    if (svr) {
                        svr.stop();
                        svr = null;
                    }
                });

                it("tls.createServer(options, handler) returns TLSServer", () => {
                    svr = tls.createServer({ key: pk1.privateKey, cert: crt }, (s) => {
                        var buf = s.read();
                        s.write(buf);
                        s.close();
                    });
                    svr.listen(9096 + base_port);
                    test_util.push(svr.socket);

                    var ss = tls.connect('ssl://localhost:' + (9096 + base_port), ctx);
                    ss.write('tls-cs');
                    assert.equal(ss.read().toString(), 'tls-cs');
                    ss.close();
                });

                it("tls.createServer(context, handler) returns TLSServer", () => {
                    svr = tls.createServer(ctx_svr, (s) => {
                        var buf = s.read();
                        s.write(buf);
                        s.close();
                    });
                    svr.listen(9097 + base_port);
                    test_util.push(svr.socket);

                    var ss = tls.connect('ssl://localhost:' + (9097 + base_port), ctx);
                    ss.write('tls-ctx');
                    assert.equal(ss.read().toString(), 'tls-ctx');
                    ss.close();
                });
            });

            describe("TLSServer Object options without port", () => {
                var svr;

                afterEach(() => {
                    if (svr) {
                        svr.stop();
                        svr = null;
                    }
                });

                it("new TLSServer({key, cert}, handler) defers — requires listen()", () => {
                    svr = new tls.Server({ key: pk1.privateKey, cert: crt }, (s) => {
                        var buf = s.read();
                        s.write(buf);
                        s.close();
                    });
                    svr.listen(9098 + base_port);
                    test_util.push(svr.socket);

                    var ss = tls.connect('ssl://localhost:' + (9098 + base_port), ctx);
                    ss.write('tls-deferred');
                    assert.equal(ss.read().toString(), 'tls-deferred');
                    ss.close();
                });

                it("new TLSServer({key, cert, port}, handler) binds immediately", () => {
                    svr = new tls.Server({ key: pk1.privateKey, cert: crt, port: 9099 + base_port }, (s) => {
                        var buf = s.read();
                        s.write(buf);
                        s.close();
                    });
                    svr.start();
                    test_util.push(svr.socket);

                    var ss = tls.connect('ssl://localhost:' + (9099 + base_port), ctx);
                    ss.write('tls-immediate');
                    assert.equal(ss.read().toString(), 'tls-immediate');
                    ss.close();
                });
            });

            describe('stream close event', () => {
                it("EOF autoDestroy triggers close", () => {
                    var svr = new net.TcpServer(0, (s) => {
                        var ss = new tls.TLSSocket(ctx_svr);
                        try {
                            ss.accept(s);
                            ss.write('hi');
                            ss.close();
                        } catch (e) { }
                        s.close();
                    });
                    svr.start();
                    var port = svr.socket.localPort;
                    test_util.push(svr.socket);

                    var s1 = new net.Socket();
                    s1.connect(port, '127.0.0.1');
                    var ss = new tls.TLSSocket(ctx);
                    ss.connect(s1);

                    var events = [];
                    var done = new coroutine.Event();
                    ss.on('data', () => { events.push('data'); });
                    ss.on('end', () => { events.push('end'); });
                    ss.on('close', () => { events.push('close'); done.set(); });
                    ss.resume();
                    done.wait();
                    assert.ok(events.indexOf('end') >= 0);
                    assert.ok(events.indexOf('close') >= 0);
                    assert.ok(events.indexOf('end') < events.indexOf('close'));
                    svr.stop();
                });

                it("destroy() triggers close", () => {
                    var svr = new net.TcpServer(0, (s) => {
                        var ss = new tls.TLSSocket(ctx_svr);
                        try {
                            ss.accept(s);
                            ss.write('hi');
                            coroutine.sleep(5000);
                            ss.close();
                        } catch (e) { }
                        s.close();
                    });
                    svr.start();
                    var port = svr.socket.localPort;
                    test_util.push(svr.socket);

                    var s1 = new net.Socket();
                    s1.connect(port, '127.0.0.1');
                    var ss = new tls.TLSSocket(ctx);
                    ss.connect(s1);

                    var done = new coroutine.Event();
                    var closed = false;
                    ss.on('close', () => { closed = true; done.set(); });
                    ss.destroy();
                    done.wait();
                    assert.strictEqual(closed, true);
                    svr.stop();
                });

                it("destroy(err) emits error then close", () => {
                    var svr = new net.TcpServer(0, (s) => {
                        var ss = new tls.TLSSocket(ctx_svr);
                        try {
                            ss.accept(s);
                            ss.write('hi');
                            coroutine.sleep(5000);
                            ss.close();
                        } catch (e) { }
                        s.close();
                    });
                    svr.start();
                    var port = svr.socket.localPort;
                    test_util.push(svr.socket);

                    var s1 = new net.Socket();
                    s1.connect(port, '127.0.0.1');
                    var ss = new tls.TLSSocket(ctx);
                    ss.connect(s1);

                    var events = [];
                    var done = new coroutine.Event();
                    ss.on('error', () => { events.push('error'); });
                    ss.on('close', () => { events.push('close'); done.set(); });
                    ss.destroy(new Error('test'));
                    done.wait();
                    assert.deepStrictEqual(events, ['error', 'close']);
                    svr.stop();
                });
            });
        });
    }

    test_tls("rsa", crypto.generateKeyPair('rsa', {
        modulusLength: 2048
    }), crypto.generateKeyPair('rsa', {
        modulusLength: 2048
    }));

    test_tls("ec", crypto.generateKeyPair('ec', {
        namedCurve: 'secp256r1'
    }), crypto.generateKeyPair('ec', {
        namedCurve: 'secp256r1'
    }));

    test_tls("sm2", crypto.generateKeyPair('sm2'), crypto.generateKeyPair('sm2'));
});


