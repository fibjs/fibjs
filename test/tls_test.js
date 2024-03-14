var test = require("test");
test.setup();

var test_util = require('./test_util');

var ssl = require("ssl");
var tls = require("tls");
var crypto = require("crypto");
var fs = require('fs');
var path = require('path');
var net = require('net');
var coroutine = require('coroutine');

var base_port = coroutine.vmid * 10000;

var pk = crypto.generateKeyPair('rsa', {
    modulusLength: 2048
});
var pk1 = crypto.generateKeyPair('rsa', {
    modulusLength: 2048
});

var ca = crypto.createCertificateRequest({
    key: pk.privateKey,
    subject: {
        CN: "fibjs.org"
    }
}).issue({
    key: pk.privateKey,
    ca: true,
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
    issuer: {
        CN: "fibjs.org"
    }
});

describe('tls', () => {
    const ctx = tls.createSecureContext({
        ca: ca
    });

    after(test_util.cleanup);

    describe('createSecureContext', () => {
        it('default', () => {
            var ctx = tls.createSecureContext();
            assert.equal(ctx.sessionTimeout, 7200);
            assert.equal(ctx.requestCert, true);
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
    });

    it("root ca", () => {
        var cert = new crypto.X509Certificate(fs.readFile(path.join(__dirname, 'cert_files', 'ca-bundle.crt')));
        var s = cert.pem;

        var s1 = tls.secureContext.ca.pem;

        assert.equal(s, s1);
    });

    it("echo server", () => {
        const ctx = tls.createSecureContext({
            key: pk1.privateKey,
            cert: crt
        }, true);

        var svr = new net.TcpServer(9080 + base_port, (s) => {
            var ss;
            var buf;

            test_util.push(s);
            var ss = new tls.TLSSocket(ctx);

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
            s1.connect("127.0.0.1", 9080 + base_port);
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
    });

    describe('verification', () => {
        function connect() {
            var s1 = new net.Socket();
            s1.connect("127.0.0.1", 9080 + base_port);
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

        it('not verify', () => {
            var ss = new tls.TLSSocket({
                requestCert: false
            });
            ss.connect(connect());

            var ss = new tls.TLSSocket({
                ca: ca,
                requestCert: false
            });
            ss.connect(connect(), "fibjs.org");
        });
    });

    describe('tls.connect', () => {
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

        it('default context', () => {
            assert.throws(() => {
                tls.connect(`ssl://localhost:${9080 + base_port}`);
            });
        });
    });
});

require.main === module && test.run(console.DEBUG);
