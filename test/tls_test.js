var test = require("test");
test.setup();

var test_util = require('./test_util');

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

function del(f) {
    try {
        fs.unlink(f);
    } catch (e) { }
}

describe('tls', () => {
    function sni_resolver(domain) {
        if (domain === "no_cert")
            return;

        var cert = crypto.createCertificateRequest({
            key: pk.privateKey,
            subject: {
                CN: domain
            }
        }).issue({
            key: pk.privateKey,
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
                        coroutine.sleep(100);
                        return sni_resolver(domain);
                    }
                }, true);

                setImmediate(() => {
                    ctx.removeSNIContext("test");
                });

                assert.equal(ctx.getSNIContext("test", true).cert.subject, 'CN=test');
                assert.equal(ctx.getSNIContext("test"), undefined);
            });

            it('deleted during resolve return nothing', () => {
                var ctx = tls.createSecureContext({
                    "SNIResolver": (domain) => {
                        coroutine.sleep(100);
                        return;
                    }
                }, true);

                setImmediate(() => {
                    ctx.removeSNIContext("test");
                });

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

            it('timeout', () => {
                var ctx = tls.createSecureContext({
                    "SNICacheTimeout": 1
                }, true);
                ctx.setSNIContext("test", sni_resolver("test"));

                coroutine.sleep(1500);
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

    it("copyTo", () => {
        var str = "012345678901234567890123456789";

        for (var i = 0; i < 10; i++)
            str = str + str;

        var svr = new net.TcpServer(9082 + base_port, (s) => {
            var ss = new tls.TLSSocket(ctx_svr);
            ss.accept(s);

            fs.writeFile(path.join(__dirname, 'net_temp_000001' + base_port), str);
            var f = fs.openFile(path.join(__dirname, 'net_temp_000001' + base_port));
            assert.equal(f.copyTo(ss), str.length);

            f.close();
            ss.close();
            s.close();
        });
        test_util.push(svr.socket);
        svr.start();

        function t_conn() {
            var c1 = new net.Socket();
            c1.connect('127.0.0.1', 9082 + base_port);

            var ss = new tls.TLSSocket(ctx);
            ss.connect(c1);

            var f1 = fs.openFile(path.join(__dirname, 'net_temp_000002' + base_port), 'w');
            assert.equal(ss.copyTo(f1), str.length);

            ss.close();
            c1.close();
            f1.close();

            assert.equal(str, fs.readFile(path.join(__dirname, 'net_temp_000002' + base_port)));
        }

        for (var i = 0; i < 5; i++) {
            str = str + str;
            t_conn();
        }

        str = undefined;

        del(path.join(__dirname, 'net_temp_000001' + base_port));
        del(path.join(__dirname, 'net_temp_000002' + base_port));
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
            s1.connect("127.0.0.1", 9083 + base_port);

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
            s1.connect("127.0.0.1", 9084 + base_port);

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
        s1.connect("127.0.0.1", 9086 + base_port);

        var cs = new tls.TLSSocket(ctx);
        cs.connect(s1, "test1");

        var cert = cs.getPeerX509Certificate();
        assert.equal(cert.subject, 'CN=test1');

        cs.close();

        var s2 = new net.Socket();
        s2.connect("127.0.0.1", 9086 + base_port);
        var cs1 = new tls.TLSSocket(ctx);

        assert.throws(() => {
            cs1.connect(s2, "no_cert");
        });

        cs1.close();
    });
});

require.main === module && test.run(console.DEBUG);
