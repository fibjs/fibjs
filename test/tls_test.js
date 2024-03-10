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

            var ss = new tls.TLSSocket(ctx);
            ss.accept(s);

            buf = ss.read();
            ss.write(buf);

            try {
                while (buf = ss.read())
                    ss.write(buf);
            } finally {
                ss.close();
                s.close();
            }
        });
        svr.start();

        test_util.push(svr.socket);
    });

    it("echo", () => {
        const ctx = tls.createSecureContext({
            ca: ca
        });

        for (var i = 0; i < 10; i++) {
            var s1 = new net.Socket();
            s1.connect("127.0.0.1", 9080 + base_port);

            var ss = new tls.TLSSocket(ctx);
            ss.connect(s1);

            ss.write("GET / HTTP/1.0");
            assert.equal("GET / HTTP/1.0", ss.read());

            ss.close();
            s1.close();
        }
    });

});

require.main === module && test.run(console.DEBUG);
