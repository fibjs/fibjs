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

var pk = crypto.generateKeyPair('ec', {
    namedCurve: 'P-256'
});
var pk1 = crypto.generateKeyPair('ec', {
    namedCurve: 'P-256'
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
        CN: "test.fibjs.org"
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

    it("root ca", () => {
        var cert = new crypto.X509Certificate(fs.readFile(path.join(__dirname, 'cert_files', 'ca-bundle.crt')));
        var s = cert.pem;

        var s1 = tls.secureContext.ca.pem;

        assert.equal(s, s1);
    });


    it("echo server", () => {
        const ctx = tls.createSecureContext({
            key: pk1.privateKey,
            cert: crt,
            rejectUnauthorized: false
        });

        var svr = new net.TcpServer(9080 + base_port, (s) => {
            var ss;
            var buf;

            test_util.push(s);
            var ss = new tls.TLSSocket(ctx);
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
});

require.main === module && test.run(console.DEBUG);
