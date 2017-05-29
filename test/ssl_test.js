var test = require("test");
test.setup();

var ssl = require("ssl");
var crypto = require("crypto");
var net = require("net");
var fs = require('fs');
var os = require('os');
var coroutine = require('coroutine');

var base_port = coroutine.vmid * 10000;

var net_config = {
    family: net.AF_INET6,
    address: '::1'
};

if (os.type() == 'Windows' && os.version < "6.0")
    net_config = {
        family: net.AF_INET,
        address: '127.0.0.1'
    };

var pk = new crypto.PKey();
pk.genRsaKey(1024);

var crt = new crypto.X509Req("CN=localhost", pk).sign("CN=baoz.me", pk);
var ca = new crypto.X509Req("CN=baoz.me", pk).sign("CN=baoz.me", pk, {
    ca: true
});

var ca_pem = ca.dump()[0];

function del(f) {
    try {
        fs.unlink(f);
    } catch (e) { }
}

describe('ssl', () => {
    var sss;

    var ss = [];

    after(() => {
        ss.forEach((s) => {
            s.close();
        });
    });

    it("echo server", () => {
        sss = new ssl.Socket(crt, pk);
        sss.verification = ssl.VERIFY_NONE;

        var svr = new net.TcpServer(9080 + base_port, (s) => {
            var ss;
            var buf;

            try {
                ss = sss.accept(s);
            } catch (e) {
                s.close();
                return;
            }

            try {
                while (buf = ss.read())
                    ss.write(buf);
            } finally {
                ss.close();
                s.close();
            }
        });
        svr.asyncRun();

        ss.push(svr.socket);
    });

    function test_handshake() {
        var s = new net.Socket();
        s.connect("127.0.0.1", 9080 + base_port);

        var ss = new ssl.Socket();
        ss.connect(s);
        ss.close();
        s.close();
    }

    function test_client_cert() {
        var s = new net.Socket();
        s.connect("127.0.0.1", 9080 + base_port);

        var ss = new ssl.Socket(crt, pk);
        ss.connect(s);
        ss.close();
        s.close();
    }

    it("client verify", () => {
        ssl.verification = ssl.VERIFY_OPTIONAL;
        test_handshake();

        ssl.verification = ssl.VERIFY_REQUIRED;
        assert.throws(test_handshake);

        ssl.ca.load(ca_pem);
        test_handshake();
    });

    it("server verify", () => {
        sss.verification = ssl.VERIFY_REQUIRED;
        assert.throws(test_client_cert);

        sss.ca.load(ca_pem);
        test_client_cert();

        assert.throws(test_handshake);
    });

    it("echo", () => {
        sss.verification = ssl.VERIFY_NONE;

        for (var i = 0; i < 10; i++) {
            var s1 = new net.Socket();
            s1.connect("127.0.0.1", 9080 + base_port);

            var ss = new ssl.Socket();
            ss.connect(s1);

            ss.write("GET / HTTP/1.0");
            assert.equal("GET / HTTP/1.0", ss.read());

            ss.close();
            s1.close();
        }
    });

    it("ssl.connect", () => {
        var ss = ssl.connect('ssl://localhost:' + (9080 + base_port));

        ss.write("GET / HTTP/1.0");
        assert.equal("GET / HTTP/1.0", ss.read());

        ss.close();
    });

    it("net.connect", () => {
        var ss = net.connect('ssl://localhost:' + (9080 + base_port));

        ss.write("GET / HTTP/1.0");
        assert.equal("GET / HTTP/1.0", ss.read());

        ss.close();
    });

    it("copyTo", () => {
        var str = "012345678901234567890123456789";

        for (var i = 0; i < 10; i++)
            str = str + str;

        var sss = new ssl.Socket(crt, pk);
        sss.verification = ssl.VERIFY_NONE;

        var svr = new net.TcpServer(9082 + base_port, (s) => {
            var ss = sss.accept(s);

            fs.writeFile('net_temp_000001' + base_port, str);
            var f = fs.open('net_temp_000001' + base_port);
            assert.equal(f.copyTo(ss), str.length);

            f.close();
            ss.close();
            s.close();
        });
        ss.push(svr.socket);
        svr.asyncRun();

        function t_conn() {
            var c1 = new net.Socket();
            c1.connect('127.0.0.1', 9082 + base_port);

            var ss = new ssl.Socket();
            ss.connect(c1);

            var f1 = fs.open('net_temp_000002' + base_port, 'w');
            assert.equal(ss.copyTo(f1), str.length);

            ss.close();
            c1.close();
            f1.close();

            assert.equal(str, fs.readFile('net_temp_000002' + base_port));
        }

        for (var i = 0; i < 10; i++) {
            str = str + str;
            t_conn();
        }

        del('net_temp_000001' + base_port);
        del('net_temp_000002' + base_port);
    });

    it("Handler", () => {
        var svr = new net.TcpServer(9083 + base_port, new ssl.Handler(crt, pk, (s) => {
            var buf;

            while (buf = s.read())
                s.write(buf);
        }));
        ss.push(svr.socket);
        svr.asyncRun();

        for (var i = 0; i < 10; i++) {
            var s1 = new net.Socket();
            s1.connect("127.0.0.1", 9083 + base_port);

            var cs = new ssl.Socket();
            cs.connect(s1);

            cs.write("GET / HTTP/1.0");
            assert.equal("GET / HTTP/1.0", cs.read());

            cs.close();
            s1.close();
        }
    });

    it("Server", () => {
        var svr = new ssl.Server(crt, pk, 9084 + base_port, (s) => {
            var buf;

            while (buf = s.read())
                s.write(buf);
        });
        ss.push(svr.socket);
        svr.asyncRun();

        for (var i = 0; i < 10; i++) {
            var s1 = new net.Socket();
            s1.connect("127.0.0.1", 9084 + base_port);

            var cs = new ssl.Socket();
            cs.connect(s1);

            cs.write("GET / HTTP/1.0");
            assert.equal("GET / HTTP/1.0", cs.read());

            cs.close();
            s1.close();
        }
    });
});

//test.run(console.DEBUG);
