var { describe, it, xit, before, after, afterEach } = require('node:test');
var assert = require('assert');

var test_util = require('./test_util');

var net = require('net');
var fs = require('fs');
var path = require('path');
var os = require('os');
var io = require('io');
var coroutine = require('coroutine');

var base_port = coroutine.vmid * 10000;

const isAndroid = process.platform === 'android';

var net_config = {
    family: net.AF_INET6,
    address: '::1',
    host: '[::1]'
};

var has_ipv6 = false;

var ni = os.networkInterfaces();

for (var n in ni) {
    ni[n].forEach((c) => {
        if (c.family == 'IPv6')
            has_ipv6 = true;
    });
}

if (!has_ipv6)
    net_config = {
        family: net.AF_INET,
        address: '127.0.0.1',
        host: '127.0.0.1'
    };

var backend = {
    "win32": "IOCP",
    "darwin": "KQueue",
    "freebsd": "KQueue",
    "linux": "EPoll",
    "android": "EPoll",
    "ios": "KQueue"
}[process.platform];


function del(f) {
    try {
        fs.unlink(f);
    } catch (e) { }
}

// Use /tmp on iOS to avoid path truncation (Unix socket sun_path limit is 104 bytes)
function unixSocketDir() {
    return process.platform === 'ios' ? '/tmp' : os.tmpdir();
}

function test_net(eng, use_uv) {
    var now_port = use_uv ? 9080 : 8080;

    function getPort() {
        return now_port++ + base_port;
    }

    describe("net " + eng, () => {
        before(() => {
            net.use_uv_socket = use_uv;
        })

        after(() => {
            test_util.cleanup();
            net.use_uv_socket = false;
        });

        it("backend", () => {
            assert.equal(net.backend(), backend);
        });

        describe("echo server", () => {
            var s, _port;

            function connect(c) {
                console.log(c.remoteAddress, c.remotePort, "->",
                    c.localAddress, c.localPort);
                try {
                    var b;

                    while (b = c.recv())
                        c.send(b);
                } finally {
                    c.close();
                }
            }

            function accept(s) {
                try {
                    while (1)
                        coroutine.start(connect, s.accept());
                } catch (e) { }
            }

            before(() => {
                s = new net.Socket(net_config.family);
                test_util.push(s);

                _port = getPort();

                s.bind(_port);
                s.listen();
                assert.equal(s.localPort, _port);
                coroutine.start(accept, s);
            });

            it("socket.connect(port, address)", () => {
                var s1 = new net.Socket(net_config.family);
                s1.connect(_port, net_config.address);
                console.log(s1.remoteAddress, s1.remotePort, "<-",
                    s1.localAddress, s1.localPort);
                s1.send(new Buffer("GET / HTTP/1.0"));
                assert.equal("GET / HTTP/1.0", s1.recv());
                s1.close();
            });

            it("net.connect(url)", () => {
                var s1 = net.connect('tcp://' + net_config.host + ':' + (_port));
                console.log(s1.remoteAddress, s1.remotePort, "<-",
                    s1.localAddress, s1.localPort);
                assert.equal(s1.remotePort, _port);
                s1.send(new Buffer("GET / HTTP/1.0"));
                assert.equal("GET / HTTP/1.0", s1.recv());
                s1.close();
            });

            it("socket.connect({ host, port })", () => {
                var s1 = new net.Socket(net_config.family);
                s1.connect({ host: net_config.address, port: _port });
                console.log(s1.remoteAddress, s1.remotePort, "<-",
                    s1.localAddress, s1.localPort);
                assert.equal(s1.remotePort, _port);
                s1.send(new Buffer("GET / HTTP/1.0"));
                assert.equal("GET / HTTP/1.0", s1.recv());
                s1.close();
            });

            it("net.connect(port, host)", () => {
                var s1 = net.connect(_port, net_config.address);
                console.log(s1.remoteAddress, s1.remotePort, "<-",
                    s1.localAddress, s1.localPort);
                assert.equal(s1.remotePort, _port);
                s1.send(new Buffer("GET / HTTP/1.0"));
                assert.equal("GET / HTTP/1.0", s1.recv());
                s1.close();
            });

            it("net.connect({ host, port })", () => {
                var s1 = net.connect({ host: net_config.address, port: _port });
                console.log(s1.remoteAddress, s1.remotePort, "<-",
                    s1.localAddress, s1.localPort);
                assert.equal(s1.remotePort, _port);
                s1.send(new Buffer("GET / HTTP/1.0"));
                assert.equal("GET / HTTP/1.0", s1.recv());
                s1.close();
            });

            it("async connect with events", () => {
                var connectEvent = new coroutine.Event();
                var connected = false;
                var receivedData = null;

                var s1 = new net.Socket(net_config.family);
                s1.on('connect', function () {
                    console.log(this.remoteAddress, this.remotePort, "<-",
                        this.localAddress, this.localPort);
                    connected = true;
                    this.send(new Buffer("GET / HTTP/1.0"));
                });
                s1.on('data', function (data) {
                    receivedData = data.toString();
                    connectEvent.set();
                });
                s1.on('error', function (err) {
                    console.log('error:', err);
                    connectEvent.set();
                });

                s1.connect(_port, net_config.address);
                console.log('connecting...');

                connectEvent.wait();
                assert.ok(connected);
                assert.equal(receivedData, "GET / HTTP/1.0");
                s1.close();
            });

            it("on data after connect success", () => {
                var dataEvent = new coroutine.Event();
                var receivedData = null;

                var s1 = new net.Socket(net_config.family);
                s1.connect(_port, net_config.address);
                console.log(s1.remoteAddress, s1.remotePort, "<-",
                    s1.localAddress, s1.localPort);

                // Register on data after connect success
                s1.on('data', function (data) {
                    receivedData = data.toString();
                    dataEvent.set();
                });

                s1.send(new Buffer("GET / HTTP/1.0"));

                dataEvent.wait();
                assert.equal(receivedData, "GET / HTTP/1.0");
                s1.close();
            });

            it("socket.connect(port, host, connectListener)", () => {
                var connectEvent = new coroutine.Event();
                var connected = false;

                var s1 = new net.Socket(net_config.family);
                s1.connect(_port, net_config.address, function () {
                    connected = true;
                    connectEvent.set();
                });

                connectEvent.wait();
                assert.ok(connected);
                console.log(s1.remoteAddress, s1.remotePort, "<-",
                    s1.localAddress, s1.localPort);
                s1.send(new Buffer("GET / HTTP/1.0"));
                assert.equal("GET / HTTP/1.0", s1.recv());
                s1.close();
            });

            it("socket.connect(port, host, timeout, connectListener)", () => {
                var connectEvent = new coroutine.Event();
                var connected = false;

                var s1 = new net.Socket(net_config.family);
                s1.connect(_port, net_config.address, 5000, function () {
                    connected = true;
                    connectEvent.set();
                });

                connectEvent.wait();
                assert.ok(connected);
                console.log(s1.remoteAddress, s1.remotePort, "<-",
                    s1.localAddress, s1.localPort);
                s1.send(new Buffer("GET / HTTP/1.0"));
                assert.equal("GET / HTTP/1.0", s1.recv());
                s1.close();
            });

            it("socket.connect(options, connectListener)", () => {
                var connectEvent = new coroutine.Event();
                var connected = false;

                var s1 = new net.Socket(net_config.family);
                s1.connect({ host: net_config.address, port: _port }, function () {
                    connected = true;
                    connectEvent.set();
                });

                connectEvent.wait();
                assert.ok(connected);
                console.log(s1.remoteAddress, s1.remotePort, "<-",
                    s1.localAddress, s1.localPort);
                s1.send(new Buffer("GET / HTTP/1.0"));
                assert.equal("GET / HTTP/1.0", s1.recv());
                s1.close();
            });

            it("net.connect(port, host, connectListener)", () => {
                var connectEvent = new coroutine.Event();
                var connected = false;
                var s1;

                s1 = net.connect(_port, net_config.address, function () {
                    connected = true;
                    connectEvent.set();
                });

                connectEvent.wait();
                assert.ok(connected);
                console.log(s1.remoteAddress, s1.remotePort, "<-",
                    s1.localAddress, s1.localPort);
                s1.send(new Buffer("GET / HTTP/1.0"));
                assert.equal("GET / HTTP/1.0", s1.recv());
                s1.close();
            });

            it("net.connect(port, host, timeout, connectListener)", () => {
                var connectEvent = new coroutine.Event();
                var connected = false;
                var s1;

                s1 = net.connect(_port, net_config.address, 5000, function () {
                    connected = true;
                    connectEvent.set();
                });

                connectEvent.wait();
                assert.ok(connected);
                console.log(s1.remoteAddress, s1.remotePort, "<-",
                    s1.localAddress, s1.localPort);
                s1.send(new Buffer("GET / HTTP/1.0"));
                assert.equal("GET / HTTP/1.0", s1.recv());
                s1.close();
            });

            it("net.connect(options, connectListener)", () => {
                var connectEvent = new coroutine.Event();
                var connected = false;
                var s1;

                s1 = net.connect({ host: net_config.address, port: _port }, function () {
                    connected = true;
                    connectEvent.set();
                });

                connectEvent.wait();
                assert.ok(connected);
                console.log(s1.remoteAddress, s1.remotePort, "<-",
                    s1.localAddress, s1.localPort);
                s1.send(new Buffer("GET / HTTP/1.0"));
                assert.equal("GET / HTTP/1.0", s1.recv());
                s1.close();
            });

            it("async connect error when connection refused", () => {
                test_util.gc();
                var socketCount = test_util.countObject('Socket');

                var errorEvent = new coroutine.Event();
                var errorReceived = null;

                // Use a port that is not listening
                var unusedPort = 59999;

                var s1 = new net.Socket(net_config.family);
                s1.on('connect', function () {
                    errorEvent.set();
                });
                s1.on('data', function (data) {
                    errorEvent.set();
                });
                s1.on('error', function (err) {
                    errorReceived = err;
                    errorEvent.set();
                });

                s1.connect(unusedPort, net_config.address);

                errorEvent.wait();
                assert.ok(errorReceived !== null);

                // Verify socket is released after error
                s1 = null;
                errorReceived = null;
                test_util.gc();
                assert.equal(socketCount, test_util.countObject('Socket'));
            });

            it("no leak when on data but no connect", () => {
                test_util.gc();
                var socketCount = test_util.countObject('Socket');

                var s1 = new net.Socket(net_config.family);
                s1.on('data', function (data) { });

                s1 = null;
                test_util.gc();
                assert.equal(socketCount, test_util.countObject('Socket'));
            });

            it("no leak when just create socket", () => {
                test_util.gc();
                var socketCount = test_util.countObject('Socket');

                var s1 = new net.Socket(net_config.family);

                s1 = null;
                test_util.gc();
                assert.equal(socketCount, test_util.countObject('Socket'));
            });
        });

        it("write and send return value validation", () => {
            function connect(c) {
                try {
                    var receivedData = '';
                    var b;

                    while (b = c.recv()) {
                        receivedData += b.toString();
                        c.send(b);
                    }
                } finally {
                    c.close();
                }
            }

            function accept(s) {
                try {
                    while (1)
                        coroutine.start(connect, s.accept());
                } catch (e) { }
            }

            var s = new net.Socket(net_config.family);
            test_util.push(s);

            var _port = getPort();

            s.bind(_port);
            s.listen();
            coroutine.start(accept, s);

            // Test write return value with string
            var s1 = new net.Socket(net_config.family);
            s1.connect(_port, net_config.address);

            var testData = 'Hello Network World!';
            var bytesWritten = s1.write(testData);
            assert.equal(bytesWritten, true);

            // Test write return value with Buffer
            var testBuffer = new Buffer('Network Buffer Data');
            bytesWritten = s1.write(testBuffer);
            assert.equal(bytesWritten, true);

            // Test write return value with empty string
            bytesWritten = s1.write('');
            assert.equal(bytesWritten, true);

            // Test send return value with string
            var sendData = 'Send Test Data';
            var bytesSent = s1.send(sendData);
            assert.equal(bytesSent, sendData.length);

            // Test send return value with Buffer
            var sendBuffer = new Buffer('Send Buffer Data');
            bytesSent = s1.send(sendBuffer);
            assert.equal(bytesSent, sendBuffer.length);

            // Test send return value with empty Buffer
            var emptyBuffer = new Buffer('');
            bytesSent = s1.send(emptyBuffer);
            assert.equal(bytesSent, 0);

            // Verify all data was transmitted correctly
            var expectedResponse = testData + testBuffer.toString() + sendData + sendBuffer.toString();
            var response = '';
            var chunk;
            while (chunk = s1.recv()) {
                response += chunk.toString();
                if (response.length >= expectedResponse.length) break;
            }
            assert.equal(response, expectedResponse);

            s1.close();
        });

        var str = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789";

        for (var i = 0; i < 8; i++)
            str = str + str;

        it("multi chunk read", () => {
            function accept1(s) {
                try {
                    while (true) {
                        var c = s.accept();
                        c.write(str);
                        c.close();
                    }
                } catch (e) { }
            }

            var _port = getPort();

            var s1 = new net.Socket(net_config.family);
            test_util.push(s1);

            s1.bind(_port);
            s1.listen();
            coroutine.start(accept1, s1);

            var c1 = new net.Socket();
            c1.connect(_port, '127.0.0.1');
            var data = c1.read(5000);
            assert.equal(data.length, 5000);
            assert.equal(data.toString(), str.substr(0, 5000));
        });

        it("multi read", () => {
            function accept1(s) {
                try {
                    while (true) {
                        var c = s.accept();
                        c.write(str);
                        c.close();
                    }
                } catch (e) { }
            }

            var _port = getPort();

            var s1 = new net.Socket(net_config.family);
            test_util.push(s1);

            s1.bind(_port);
            s1.listen();
            coroutine.start(accept1, s1);

            var c1 = new net.Socket();
            c1.connect(_port, '127.0.0.1');

            coroutine.sleep(10);

            const cnt = 5;
            for (var i = 0; i < cnt; i++) {
                var data = c1.read(5000);
                assert.equal(data.length, 5000);
                assert.equal(data.toString(), str.substr(5000 * i, 5000));
                coroutine.sleep(10);
            }
        });

        it("parallel read", () => {
            var ev0 = new coroutine.Event();

            function accept1(s) {
                try {
                    while (true) {
                        var c = s.accept();

                        ev0.wait();
                        c.write(str);

                        c.close();
                    }
                } catch (e) { }
            }

            var _port = getPort();

            var s1 = new net.Socket(net_config.family);
            test_util.push(s1);

            s1.bind(_port);
            s1.listen();
            coroutine.start(accept1, s1);

            var c1 = new net.Socket();
            c1.connect(_port, '127.0.0.1');

            const cnt = 5;
            var evs = [];
            var datas = [];

            function start_job(n) {
                evs[n] = new coroutine.Event();
                c1.read(5000, (err, data) => {
                    datas[n] = data;
                    evs[n].set();
                });
                coroutine.sleep(10);
            }

            for (var i = 0; i < cnt; i++)
                start_job(i);

            ev0.set();

            coroutine.sleep(10);

            for (var i = 0; i < cnt; i++) {
                evs[i].wait();
                assert.equal(datas[i].length, 5000);
                assert.equal(datas[i].toString(), str.substr(5000 * i, 5000));
            }
        });

        it("copyTo", () => {
            function accept1(s) {
                try {
                    while (true) {
                        var c = s.accept();

                        // Use MemoryStream instead of file
                        var f = new io.MemoryStream();
                        f.write(str);
                        f.rewind();
                        assert.equal(f.copyTo(c), str.length);
                        f.close();
                        c.close();
                    }
                } catch (e) { }
            }

            var _port = getPort();

            var s1 = new net.Socket(net_config.family);
            test_util.push(s1);

            s1.bind(_port);
            s1.listen();
            coroutine.start(accept1, s1);

            function t_conn() {
                var c1 = new net.Socket();
                c1.connect(_port, '127.0.0.1');

                // Use MemoryStream instead of file
                var f1 = new io.MemoryStream();
                assert.equal(c1.copyTo(f1), str.length);
                c1.close();

                f1.rewind();
                assert.equal(str, f1.readAll().toString());
                f1.close();
            }

            for (var i = 0; i < 100; i++)
                t_conn();

            for (var i = 0; i < 10; i++)
                str = str + str;

            t_conn();

            str = undefined;
        });

        it("read & recv", () => {
            function accept2(s) {
                try {
                    while (true) {
                        var c = s.accept();

                        c.write('a');
                        coroutine.sleep(100);
                        c.write('a');
                        coroutine.sleep(100);
                        c.write('b');
                        coroutine.sleep(100);
                        c.write('c');
                        coroutine.sleep(100);
                        c.write('d');
                        coroutine.sleep(100);

                        c.close();
                    }
                } catch (e) { }
            }

            var s2 = new net.Socket(net_config.family);
            test_util.push(s2);

            var _port = getPort();

            s2.bind(_port);
            s2.listen();
            coroutine.start(accept2, s2);

            var c1 = new net.Socket();
            c1.connect(_port, '127.0.0.1');
            assert.equal('a', c1.recv(100));
            assert.equal('ab', c1.read(2));
            assert.equal('c', c1.read(1));
            assert.equal('d', c1.read(3));
            assert.equal(null, c1.read(3));
            assert.equal(null, c1.read(3));
        });

        it("data event", () => {
            function accept3(s) {
                try {
                    while (true) {
                        var c = s.accept();

                        // Send HTTP response data
                        c.write('HTTP/1.1 200 OK\r\n');
                        coroutine.sleep(50);
                        c.write('Content-Type: text/plain\r\n');
                        coroutine.sleep(50);
                        c.write('Content-Length: 13\r\n');
                        coroutine.sleep(50);
                        c.write('\r\n');
                        coroutine.sleep(50);
                        c.write('Hello, World!');

                        coroutine.sleep(100);
                        c.close();
                    }
                } catch (e) { }
            }

            var s3 = new net.Socket(net_config.family);
            test_util.push(s3);

            var _port = getPort();

            s3.bind(_port);
            s3.listen();
            coroutine.start(accept3, s3);

            var c1 = new net.Socket();
            c1.connect(_port, '127.0.0.1');

            var receivedData = [];
            var dataEvent = new coroutine.Event();
            var closeEvent = new coroutine.Event();

            // Register data event handler
            c1.on('data', (data) => {
                receivedData.push(data.toString());
            });

            // Register close event handler  
            c1.on('close', () => {
                closeEvent.set();
            });

            // Send HTTP request
            c1.write('GET / HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n');

            // Wait for connection to close
            closeEvent.wait();

            // Verify received data
            var fullResponse = receivedData.join('');
            assert.ok(fullResponse.includes('HTTP/1.1 200 OK'));
            assert.ok(fullResponse.includes('Content-Type: text/plain'));
            assert.ok(fullResponse.includes('Hello, World!'));
            assert.ok(receivedData.length > 0);
        });

        describe("re-entrant", () => {

            it("accept", () => {
                var t = 0;

                function accept2(s, n) {
                    test_util.push(s.accept());
                    t = n;
                }

                var s2 = new net.Socket(net_config.family);
                test_util.push(s2);

                var _port = getPort();

                s2.bind(_port);
                s2.listen();

                coroutine.start(accept2, s2, 1);
                coroutine.sleep(10);
                coroutine.start(accept2, s2, 2);
                coroutine.sleep(10);

                assert.equal(t, 0);

                var c1 = new net.Socket();
                c1.connect(_port, '127.0.0.1');
                c1.close();
                coroutine.sleep(10);
                assert.equal(t, 1);

                var c1 = new net.Socket();
                c1.connect(_port, '127.0.0.1');
                c1.close();
                coroutine.sleep(10);
                assert.equal(t, 2);

            });

            it("recv", () => {
                var t = 0;

                function recv2(s, d, n) {
                    var d = s.recv();
                    if (d.toString() == d)
                        t = n;
                }

                function accept2(s) {
                    var c = s.accept();
                    coroutine.start(recv2, c, "1234", 1);
                    coroutine.sleep(10);
                    coroutine.start(recv2, c, "4567", 2);
                }

                var s2 = new net.Socket(net_config.family);
                test_util.push(s2);

                var _port = getPort();

                s2.bind(_port);
                s2.listen();

                coroutine.start(accept2, s2);

                var c1 = new net.Socket();
                c1.connect(_port, '127.0.0.1');
                coroutine.sleep(100);

                c1.send('1234');
                coroutine.sleep(10);
                assert.equal(t, 1);

                c1.send('4567');
                coroutine.sleep(10);
                assert.equal(t, 2);
            });

            it("send", () => {

            });
        });

        describe("timeout", () => {
            it("basic timeout", () => {
                function accept4(s) {
                    try {
                        while (true) {
                            test_util.push(s.accept());
                        }
                    } catch (e) { }
                }

                var s2 = new net.Socket(net_config.family);
                test_util.push(s2);

                var _port = getPort();

                s2.bind(_port);
                s2.listen();
                coroutine.start(accept4, s2);

                var c1 = new net.Socket();

                test_util.gc();

                c1.connect(_port, '127.0.0.1');

                var t1 = new Date();
                c1.timeout = 300;
                assert.throws(() => {
                    c1.recv();
                });

                var t2 = new Date();

                assert.greaterThan(t2 - t1, 250);
                assert.lessThan(t2 - t1, 1000);

                var c2 = new net.Socket();
                var t1 = new Date();
                assert.throws(() => {
                    c2.connect(8086 + base_port, '192.166.166.166', 300);
                });
                var t2 = new Date();

                assert.greaterThan(t2 - t1, 100);
                assert.lessThan(t2 - t1, 1000);
            });

            it("socket remains open after recv timeout", () => {
                // Note: This test validates the new timeout behavior for ev/iocp backends
                // UVSocket has different timeout implementation
                if (use_uv) {
                    // Skip for UV backend - it has different timeout behavior
                    return;
                }

                function accept_keep_alive(s) {
                    try {
                        while (true) {
                            var c = s.accept();
                            test_util.push(c);
                            // Keep connection open but don't send data
                        }
                    } catch (e) { }
                }

                var svr = new net.Socket(net_config.family);
                test_util.push(svr);

                var _port = getPort();

                svr.bind(_port);
                svr.listen();
                coroutine.start(accept_keep_alive, svr);

                var c1 = new net.Socket();
                c1.connect(_port, '127.0.0.1');
                c1.timeout = 200;

                // First recv should timeout
                var t1 = new Date();
                assert.throws(() => {
                    c1.recv();
                });
                var t2 = new Date();

                assert.greaterThan(t2 - t1, 150);
                assert.lessThan(t2 - t1, 500);

                // Socket should still be open - verify by accessing properties
                assert.ok(c1.remoteAddress);
                assert.ok(c1.remotePort > 0);
                assert.ok(c1.localAddress);
                assert.ok(c1.localPort > 0);

                // Should be able to close manually
                c1.close();
            });

            it("socket can retry after timeout", () => {
                function accept_delayed(s) {
                    try {
                        while (true) {
                            var c = s.accept();
                            test_util.push(c);
                            // Wait a bit then send data
                            coroutine.sleep(400);
                            c.send('delayed response');
                            c.close();
                        }
                    } catch (e) { }
                }

                var svr = new net.Socket(net_config.family);
                test_util.push(svr);

                var _port = getPort();

                svr.bind(_port);
                svr.listen();
                coroutine.start(accept_delayed, svr);

                var c1 = new net.Socket();
                c1.connect(_port, '127.0.0.1');

                // First recv with short timeout should timeout
                c1.timeout = 200;
                assert.throws(() => {
                    c1.recv();
                });

                // Socket should still be connected
                assert.ok(c1.remotePort > 0);

                // Second recv with longer timeout should succeed
                c1.timeout = 500;
                var data = c1.recv();
                assert.equal(data.toString(), 'delayed response');

                c1.close();
            });

            it("multiple consecutive timeouts", () => {
                function accept_silent(s) {
                    try {
                        while (true) {
                            var c = s.accept();
                            test_util.push(c);
                            // Never send data
                        }
                    } catch (e) { }
                }

                var svr = new net.Socket(net_config.family);
                test_util.push(svr);

                var _port = getPort();

                svr.bind(_port);
                svr.listen();
                coroutine.start(accept_silent, svr);

                var c1 = new net.Socket();
                c1.connect(_port, '127.0.0.1');
                c1.timeout = 200;

                // Multiple timeouts should all work
                for (var i = 0; i < 3; i++) {
                    var t1 = new Date();
                    assert.throws(() => {
                        c1.recv();
                    });
                    var t2 = new Date();
                    assert.greaterThan(t2 - t1, 150);
                    assert.lessThan(t2 - t1, 500);
                }

                // Socket should still be valid
                assert.ok(c1.remotePort > 0);
                c1.close();
            });

            it("concurrent reads with timeout", () => {
                var svr = new net.Socket(net_config.family);
                test_util.push(svr);

                var _port = getPort();

                svr.bind(_port);
                svr.listen();

                var serverConn = null;
                coroutine.start(() => {
                    serverConn = svr.accept();
                    test_util.push(serverConn);
                });

                var c1 = new net.Socket();
                c1.connect(_port, '127.0.0.1');

                // Wait for server to accept
                while (!serverConn) coroutine.sleep(10);

                var timeoutResult = null;
                var normalResult = null;

                // Start a read with short timeout
                coroutine.start(() => {
                    try {
                        c1.timeout = 200;
                        c1.recv();
                        timeoutResult = "success";
                    } catch (e) {
                        timeoutResult = e.number;
                    }
                });

                // Start a read without timeout (will wait in queue)
                coroutine.start(() => {
                    try {
                        c1.timeout = 0;  // No timeout
                        normalResult = c1.recv();
                    } catch (e) {
                        normalResult = e;
                    }
                });

                // Wait for first read to timeout
                coroutine.sleep(500);
                assert.equal(timeoutResult, 20021);  // CALL_E_TIMEOUT (absolute value)

                // Now send data - should be received by the second read
                serverConn.send(new Buffer("hello"));

                // Wait for second read to complete
                coroutine.sleep(100);
                assert.ok(Buffer.isBuffer(normalResult));
                assert.equal(normalResult.toString(), "hello");

                c1.close();
            });

            it("connect timeout does not close socket", () => {
                // Use a non-routable IP for connect timeout
                var c1 = new net.Socket();

                var t1 = new Date();
                assert.throws(() => {
                    // 192.0.2.1 is TEST-NET-1, guaranteed to be non-routable
                    c1.connect(80, '192.0.2.1', 300);
                });
                var t2 = new Date();

                assert.greaterThan(t2 - t1, 250);
                assert.lessThan(t2 - t1, 1000);

                // Should be able to close manually
                c1.close();
            });

            it("timeout error contains proper message", () => {
                function accept_silent(s) {
                    try {
                        while (true) {
                            test_util.push(s.accept());
                        }
                    } catch (e) { }
                }

                var svr = new net.Socket(net_config.family);
                test_util.push(svr);

                var _port = getPort();

                svr.bind(_port);
                svr.listen();
                coroutine.start(accept_silent, svr);

                var c1 = new net.Socket();
                c1.connect(_port, '127.0.0.1');
                c1.timeout = 200;

                var errorCaught = false;
                var errorMessage = '';

                try {
                    c1.recv();
                } catch (e) {
                    errorCaught = true;
                    errorMessage = e.message;
                }

                assert.ok(errorCaught);
                // Should contain timeout-related message
                assert.ok(errorMessage.includes('time') || errorMessage.includes('exceeded'));

                c1.close();
            });

            it("different operations can have different timeouts", () => {
                function accept_varied(s) {
                    try {
                        while (true) {
                            var c = s.accept();
                            test_util.push(c);
                            // Delayed response
                            coroutine.sleep(300);
                            c.send('response');
                            c.close();
                        }
                    } catch (e) { }
                }

                var svr = new net.Socket(net_config.family);
                test_util.push(svr);

                var _port = getPort();

                svr.bind(_port);
                svr.listen();
                coroutine.start(accept_varied, svr);

                var c1 = new net.Socket();
                c1.connect(_port, '127.0.0.1');

                // Short timeout should fail
                c1.timeout = 100;
                assert.throws(() => {
                    c1.recv();
                });

                // Change timeout and try again
                c1.timeout = 500;
                var data = c1.recv();
                assert.equal(data.toString(), 'response');

                c1.close();
            });

            it("zero timeout means no timeout", () => {
                function accept_slow(s) {
                    try {
                        while (true) {
                            var c = s.accept();
                            test_util.push(c);
                            coroutine.sleep(100);
                            c.send('data');
                            c.close();
                        }
                    } catch (e) { }
                }

                var svr = new net.Socket(net_config.family);
                test_util.push(svr);

                var _port = getPort();

                svr.bind(_port);
                svr.listen();
                coroutine.start(accept_slow, svr);

                var c1 = new net.Socket();
                c1.connect(_port, '127.0.0.1');

                // Zero timeout should wait indefinitely
                c1.timeout = 0;
                var data = c1.recv();
                assert.equal(data.toString(), 'data');

                c1.close();
            });

            it("write timeout", () => {
                // This test verifies write timeout works when socket blocks

                var svr = new net.Socket(net_config.family);
                test_util.push(svr);
                var _port = getPort();

                svr.bind(_port);
                svr.listen();

                var serverConn = null;
                coroutine.start(() => {
                    serverConn = svr.accept();
                    test_util.push(serverConn);
                    // Don't read anything - let send buffer fill up
                });

                var c1 = new net.Socket();
                c1.connect(_port, '127.0.0.1');

                while (!serverConn) coroutine.sleep(10);

                // Set short timeout
                c1.timeout = 200;

                var writeResult = null;
                var largeData = new Buffer(16 * 1024 * 1024);  // 16MB to fill buffer

                // Try to write large data that should block and timeout
                try {
                    for (var i = 0; i < 10; i++) {
                        c1.send(largeData);
                    }
                    writeResult = "success";
                } catch (e) {
                    writeResult = e.number;
                }

                // Either timed out (20021) or completed successfully (large buffer)
                assert.ok(writeResult === 20021 || writeResult === "success",
                    "Expected timeout (20021) or success, got: " + writeResult);

                c1.close();
            });
        });

        it("bind same port", () => {
            var _port = getPort();
            var svr = new net.TcpServer(_port, (c) => { });
            assert.throws(() => {
                new net.TcpServer(_port, (c) => { });
            });
            test_util.push(svr.socket);
        });

        describe("close Pending I/O", () => {
            function close_it(s) {
                coroutine.sleep(50);
                s.close();
            }

            it("close connect", () => {
                var c1 = new net.Socket();
                coroutine.start(close_it, c1);
                assert.throws(() => {
                    c1.connect(80, '12.0.0.1');
                });
            });

            it("close accept", () => {
                var c1 = new net.Socket();
                c1.bind(getPort());
                c1.listen();

                coroutine.start(close_it, c1);

                assert.throws(() => {
                    c1.accept();
                });
            });

            it("close read", () => {
                // Create a temporary server to connect to
                var svr = new net.Socket(net_config.family);
                var svrPort = getPort();
                svr.bind(svrPort, '127.0.0.1');
                svr.listen();
                test_util.push(svr);

                var c1 = new net.Socket();
                c1.connect(svrPort, '127.0.0.1');
                console.log(c1.remoteAddress, c1.remotePort, "->",
                    c1.localAddress, c1.localPort);
                coroutine.start(close_it, c1);
                assert.throws(() => {
                    c1.read();
                });
            });
        });

        describe("abort", () => {
            it("abort read", () => {
                var svr = new net.Socket(net_config.family);
                test_util.push(svr);
                var _port = getPort();

                svr.bind(_port);
                svr.listen();

                var serverConn = null;
                coroutine.start(() => {
                    serverConn = svr.accept();
                    test_util.push(serverConn);
                });

                var c1 = new net.Socket();
                c1.connect(_port, '127.0.0.1');

                // Wait for server to accept
                while (!serverConn) coroutine.sleep(10);

                var readResult = null;

                // Start a read that will be aborted
                coroutine.start(() => {
                    try {
                        c1.recv();
                        readResult = "success";
                    } catch (e) {
                        readResult = e.number;
                    }
                });

                // Give the read time to start
                coroutine.sleep(50);

                // Abort pending operations
                c1.abort();

                // Wait for the read to complete
                coroutine.sleep(50);
                assert.equal(readResult, 20022);  // CALL_E_ABORT (absolute value)

                // Socket should still be valid
                assert.ok(c1.remotePort > 0);

                c1.close();
            });

            it("read after abort", () => {
                var svr = new net.Socket(net_config.family);
                test_util.push(svr);
                var _port = getPort();

                svr.bind(_port);
                svr.listen();

                var serverConn = null;
                coroutine.start(() => {
                    serverConn = svr.accept();
                    test_util.push(serverConn);
                });

                var c1 = new net.Socket();
                c1.connect(_port, '127.0.0.1');

                while (!serverConn) coroutine.sleep(10);

                var readResult = null;

                // Start a read that will be aborted
                coroutine.start(() => {
                    try {
                        c1.recv();
                    } catch (e) {
                        readResult = e.number;
                    }
                });

                coroutine.sleep(50);
                c1.abort();
                coroutine.sleep(50);
                assert.equal(readResult, 20022);  // CALL_E_ABORT

                // Should be able to read again after abort
                coroutine.start(() => {
                    serverConn.send(new Buffer("hello"));
                });

                var data = c1.recv();
                assert.equal(data.toString(), "hello");

                c1.close();
            });

            it("abort multiple concurrent reads", () => {
                // ev mode only supports one recv at a time, skip this test
                if (!use_uv) return;

                var svr = new net.Socket(net_config.family);
                test_util.push(svr);
                var _port = getPort();

                svr.bind(_port);
                svr.listen();

                var serverConn = null;
                coroutine.start(() => {
                    serverConn = svr.accept();
                    test_util.push(serverConn);
                });

                var c1 = new net.Socket();
                c1.connect(_port, '127.0.0.1');

                while (!serverConn) coroutine.sleep(10);

                var results = [];

                // Start multiple reads
                for (var i = 0; i < 3; i++) {
                    coroutine.start(() => {
                        try {
                            c1.recv();
                            results.push("success");
                        } catch (e) {
                            results.push(e.number);
                        }
                    });
                }

                coroutine.sleep(50);

                // Abort all pending reads
                c1.abort();

                coroutine.sleep(50);

                // All reads should be aborted
                assert.equal(results.length, 3);
                for (var i = 0; i < results.length; i++) {
                    assert.equal(results[i], 20022);  // CALL_E_ABORT
                }

                c1.close();
            });

            it("write abort", () => {
                // This test verifies write abort works when write is blocked

                var svr = new net.Socket(net_config.family);
                test_util.push(svr);
                var _port = getPort();

                svr.bind(_port);
                svr.listen();

                var serverConn = null;
                coroutine.start(() => {
                    serverConn = svr.accept();
                    test_util.push(serverConn);
                    // Don't read - let buffer fill up
                });

                var c1 = new net.Socket();
                c1.connect(_port, '127.0.0.1');

                while (!serverConn) coroutine.sleep(10);

                c1.timeout = 0;  // No timeout

                var writeResult = null;
                var largeData = new Buffer(16 * 1024 * 1024);  // 16MB to fill buffer

                // Start a write that should block
                coroutine.start(() => {
                    try {
                        // Keep writing until blocked
                        for (var i = 0; i < 10; i++) {
                            c1.send(largeData);
                        }
                        writeResult = "success";
                    } catch (e) {
                        writeResult = e.number;
                    }
                });

                // Give the write time to start and block
                coroutine.sleep(100);

                // Abort pending operations
                c1.abort();

                // Wait for the write to complete
                coroutine.sleep(100);

                // Either aborted (20022) or completed successfully
                assert.ok(writeResult === 20022 || writeResult === "success",
                    "Expected abort (20022) or success, got: " + writeResult);

                c1.close();
            });

            it("abort concurrent read and write", () => {
                var svr = new net.Socket(net_config.family);
                test_util.push(svr);
                var _port = getPort();

                svr.bind(_port);
                svr.listen();

                var serverConn = null;
                coroutine.start(() => {
                    serverConn = svr.accept();
                    test_util.push(serverConn);
                    // Don't read or write - let operations block
                });

                var c1 = new net.Socket();
                c1.connect(_port, '127.0.0.1');

                while (!serverConn) coroutine.sleep(10);

                c1.timeout = 0;  // No timeout

                var readResult = null;
                var writeResult = null;
                var largeData = new Buffer(16 * 1024 * 1024);  // 16MB

                // Start a read that will block
                coroutine.start(() => {
                    try {
                        c1.recv();
                        readResult = "success";
                    } catch (e) {
                        readResult = e.number;
                    }
                });

                // Start a write that will block
                coroutine.start(() => {
                    try {
                        for (var i = 0; i < 10; i++) {
                            c1.send(largeData);
                        }
                        writeResult = "success";
                    } catch (e) {
                        writeResult = e.number;
                    }
                });

                // Give operations time to start and block
                coroutine.sleep(100);

                // Abort both pending operations
                c1.abort();

                // Wait for operations to complete
                coroutine.sleep(100);

                // Read should be aborted
                assert.equal(readResult, 20022, "Read should be aborted");

                // Write either aborted or completed
                assert.ok(writeResult === 20022 || writeResult === "success",
                    "Expected write abort (20022) or success, got: " + writeResult);

                // Socket should still be valid
                assert.ok(c1.remotePort > 0);

                c1.close();
            });
        });

        describe("socket reusability after timeout/abort", () => {
            it("socket remains usable after timeout", () => {
                function echo_server(s) {
                    try {
                        while (true) {
                            var c = s.accept();
                            test_util.push(c);
                            var data = c.recv();
                            if (data)
                                c.send(data);
                            c.close();
                        }
                    } catch (e) { }
                }

                var svr = new net.Socket(net_config.family);
                test_util.push(svr);
                var _port = getPort();
                svr.bind(_port);
                svr.listen();
                coroutine.start(echo_server, svr);

                var c1 = new net.Socket();
                c1.connect(_port, '127.0.0.1');

                // First operation: timeout
                c1.timeout = 100;
                assert.throws(() => {
                    c1.recv();
                });

                // Socket should still be connected
                assert.ok(c1.remoteAddress);

                // Second operation: should work with longer timeout
                c1.timeout = 1000;
                c1.send('hello');
                var response = c1.recv();
                assert.equal(response.toString(), 'hello');

                c1.close();
            });

            it("socket remains usable after abort via close", () => {
                function delayed_echo(s) {
                    try {
                        while (true) {
                            var c = s.accept();
                            test_util.push(c);
                            coroutine.sleep(200);
                            var data = c.recv();
                            if (data)
                                c.send(data);
                            c.close();
                        }
                    } catch (e) { }
                }

                var svr = new net.Socket(net_config.family);
                test_util.push(svr);
                var _port = getPort();
                svr.bind(_port);
                svr.listen();
                coroutine.start(delayed_echo, svr);

                var c1 = new net.Socket();
                c1.connect(_port, '127.0.0.1');

                // Start a read operation in background
                var readError = null;
                coroutine.start(() => {
                    try {
                        c1.recv();
                    } catch (e) {
                        readError = e;
                    }
                });

                // Abort by closing (this will trigger abort internally)
                coroutine.sleep(50);
                c1.close();

                coroutine.sleep(100);
                assert.ok(readError);

                // Socket should be closed now
                assert.throws(() => {
                    c1.send('test');
                });
            });

            it("can perform multiple operations after timeout", () => {
                function slow_responder(s) {
                    try {
                        while (true) {
                            var c = s.accept();
                            test_util.push(c);

                            // First request - respond immediately
                            var data1 = c.recv();
                            if (data1)
                                c.send('fast: ' + data1.toString());

                            // Second request - respond immediately
                            var data2 = c.recv();
                            if (data2)
                                c.send('fast: ' + data2.toString());

                            c.close();
                        }
                    } catch (e) { }
                }

                var svr = new net.Socket(net_config.family);
                test_util.push(svr);
                var _port = getPort();
                svr.bind(_port);
                svr.listen();
                coroutine.start(slow_responder, svr);

                var c1 = new net.Socket();
                c1.connect(_port, '127.0.0.1');

                // Operation 1: timeout
                c1.timeout = 50;
                assert.throws(() => {
                    c1.recv();
                });

                // Operation 2: successful with proper timeout
                c1.timeout = 1000;
                c1.send('req1');
                var resp1 = c1.recv();
                assert.equal(resp1.toString(), 'fast: req1');

                // Operation 3: another successful operation
                c1.send('req2');
                var resp2 = c1.recv();
                assert.equal(resp2.toString(), 'fast: req2');

                c1.close();
            });

            it("abort increments version and invalidates old operations", () => {
                function hanging_server(s) {
                    try {
                        while (true) {
                            var c = s.accept();
                            test_util.push(c);
                            // Keep connection open but don't respond
                            // Read will block until client closes
                            try {
                                c.recv();
                            } catch (e) {
                                // Client closed, cleanup and continue
                            }
                        }
                    } catch (e) { }
                }

                var svr = new net.Socket(net_config.family);
                test_util.push(svr);
                var _port = getPort();
                svr.bind(_port);
                svr.listen();
                coroutine.start(hanging_server, svr);

                var c1 = new net.Socket();
                c1.connect(_port, '127.0.0.1');

                // Start operation 1
                var error1 = null;
                coroutine.start(() => {
                    try {
                        c1.recv();
                    } catch (e) {
                        error1 = e;
                    }
                });

                coroutine.sleep(50);

                // Close socket (triggers abort)
                c1.close();

                coroutine.sleep(100);

                // Operation should have been aborted
                assert.ok(error1);
            });
        });

        it("isIP", () => {
            assert.strictEqual(net.isIP('127.0.0.1'), 4);
            assert.strictEqual(net.isIP('x127.0.0.1'), 0);
            assert.strictEqual(net.isIP('example.com'), 0);
            assert.strictEqual(net.isIP('0000:0000:0000:0000:0000:0000:0000:0000'), 6);
            assert.strictEqual(net.isIP('0000:0000:0000:0000:0000:0000:0000:0000::0000'), 0);
            assert.strictEqual(net.isIP('1050:0:0:0:5:600:300c:326b'), 6);
            assert.strictEqual(net.isIP('2001:252:0:1::2008:6'), 6);
            assert.strictEqual(net.isIP('2001:dead:beef:1::2008:6'), 6);
            assert.strictEqual(net.isIP('2001::'), 6);
            assert.strictEqual(net.isIP('2001:dead::'), 6);
            assert.strictEqual(net.isIP('2001:dead:beef::'), 6);
            assert.strictEqual(net.isIP('2001:dead:beef:1::'), 6);
            assert.strictEqual(net.isIP('ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff'), 6);
            assert.strictEqual(net.isIP(':2001:252:0:1::2008:6:'), 0);
            assert.strictEqual(net.isIP(':2001:252:0:1::2008:6'), 0);
            assert.strictEqual(net.isIP('2001:252:0:1::2008:6:'), 0);
            assert.strictEqual(net.isIP('2001:252::1::2008:6'), 0);
            assert.strictEqual(net.isIP('::2001:252:1:2008:6'), 6);
            assert.strictEqual(net.isIP('::2001:252:1:1.1.1.1'), 6);
            assert.strictEqual(net.isIP('::2001:252:1:255.255.255.255'), 6);
            assert.strictEqual(net.isIP('::2001:252:1:255.255.255.255.76'), 0);
            assert.strictEqual(net.isIP('::anything'), 0);
            assert.strictEqual(net.isIP('::1'), 6);
            assert.strictEqual(net.isIP('::'), 6);
            assert.strictEqual(net.isIP('0000:0000:0000:0000:0000:0000:12345:0000'), 0);
            assert.strictEqual(net.isIP('0'), 0);
            assert.strictEqual(net.isIP(), 0);
            assert.strictEqual(net.isIP(''), 0);
            assert.strictEqual(net.isIP(null), 0);
            assert.strictEqual(net.isIP(123), 0);
            assert.strictEqual(net.isIP(true), 0);
            assert.strictEqual(net.isIP({}), 0);
            assert.strictEqual(net.isIP({
                toString: () => '::2001:252:1:255.255.255.255'
            }), 6);
            assert.strictEqual(net.isIP({
                toString: () => '127.0.0.1'
            }), 4);
            assert.strictEqual(net.isIP({
                toString: () => 'bla'
            }), 0);
        });

        it("isIPv4", () => {
            assert.strictEqual(net.isIPv4('127.0.0.1'), true);
            assert.strictEqual(net.isIPv4('example.com'), false);
            assert.strictEqual(net.isIPv4('2001:252:0:1::2008:6'), false);
            assert.strictEqual(net.isIPv4(), false);
            assert.strictEqual(net.isIPv4(''), false);
            assert.strictEqual(net.isIPv4(null), false);
            assert.strictEqual(net.isIPv4(123), false);
            assert.strictEqual(net.isIPv4(true), false);
            assert.strictEqual(net.isIPv4({}), false);
            assert.strictEqual(net.isIPv4({
                toString: () => '::2001:252:1:255.255.255.255'
            }), false);
            assert.strictEqual(net.isIPv4({
                toString: () => '127.0.0.1'
            }), true);
            assert.strictEqual(net.isIPv4({
                toString: () => 'bla'
            }), false);
        });

        it("isIPv6", () => {
            assert.strictEqual(net.isIPv6('127.0.0.1'), false);
            assert.strictEqual(net.isIPv6('example.com'), false);
            assert.strictEqual(net.isIPv6('2001:252:0:1::2008:6'), true);
            assert.strictEqual(net.isIPv6(), false);
            assert.strictEqual(net.isIPv6(''), false);
            assert.strictEqual(net.isIPv6(null), false);
            assert.strictEqual(net.isIPv6(123), false);
            assert.strictEqual(net.isIPv6(true), false);
            assert.strictEqual(net.isIPv6({}), false);
            assert.strictEqual(net.isIPv6({
                toString: () => '::2001:252:1:255.255.255.255'
            }), true);
            assert.strictEqual(net.isIPv6({
                toString: () => '127.0.0.1'
            }), false);
            assert.strictEqual(net.isIPv6({
                toString: () => 'bla'
            }), false);
        });

        it("Memory Leak detect", () => {
            var ss, no1;

            test_util.gc();
            no1 = test_util.countObject('Socket');

            ss = new net.TcpServer(getPort(), (c) => { });
            ss.start();

            coroutine.sleep(50);
            ss.stop();
            ss = undefined;

            test_util.gc();
            assert.equal(no1, test_util.countObject('Socket'));

            (() => {
                var s = new net.TcpServer(getPort(), () => { });
            })();

            test_util.gc();
            assert.equal(no1, test_util.countObject('Socket'));
        });

        (isAndroid ? describe.skip : describe)("unix socket", () => {
            it("echo", () => {
                function connect(c) {
                    try {
                        var b;

                        while (b = c.recv())
                            c.send(b);
                    } finally {
                        c.close();
                    }
                }

                function accept(s) {
                    try {
                        while (1)
                            coroutine.start(connect, s.accept());
                    } catch (e) { }
                }

                var s = new net.Socket(net.AF_UNIX);
                test_util.push(s);

                var _port = getPort();
                var _path = process.platform === 'win32' ? "//./pipe/port_" + _port : unixSocketDir() + '/port_' + _port;

                del(_path);
                s.bind(_path);
                s.listen();
                // Skip localAddress check if path is too long (Unix socket sun_path limit is 104 bytes)
                if (process.platform !== 'win32' && _path.length < 104)
                    assert.equal(s.localAddress, _path);
                coroutine.start(accept, s);

                function conn_socket() {
                    var s1 = new net.Socket(net.AF_UNIX);
                    s1.connect(_path);
                    s1.send(new Buffer("GET / HTTP/1.0"));
                    assert.equal("GET / HTTP/1.0", s1.recv());
                    s1.close();
                }

                function conn() {
                    var s1 = net.connect('unix:' + _path);
                    // Skip remoteAddress check if path is too long (Unix socket sun_path limit is 104 bytes)
                    if (process.platform !== 'win32' && _path.length < 104)
                        assert.equal(s1.remoteAddress, _path);
                    s1.send(new Buffer("GET / HTTP/1.0"));
                    assert.equal("GET / HTTP/1.0", s1.recv());
                    s1.close();
                }

                conn_socket();
                conn();

                assert.throws(() => {
                    var s1 = new net.Socket(net.AF_UNIX);
                    s1.connect(0, "999.999.999.999");
                });
            });

            it("Server", () => {
                var svr;

                var _port = getPort();
                var _path = process.platform === 'win32' ? "//./pipe/port_" + _port : unixSocketDir() + '/port_' + _port;

                del(_path);
                svr = new net.TcpServer(_path, (c) => {
                    try {
                        var b;

                        while (b = c.recv())
                            c.send(b);
                    } finally {
                        c.close();
                    }
                });
                test_util.push(svr.socket);
                svr.start();

                var s1 = net.connect('unix:' + _path);
                // Skip remoteAddress check if path is too long (Unix socket sun_path limit is 104 bytes)
                if (process.platform !== 'win32' && _path.length < 104)
                    assert.equal(s1.remoteAddress, _path);
                s1.send(new Buffer("GET / HTTP/1.0"));
                assert.equal("GET / HTTP/1.0", s1.recv());
                s1.close();
            });

            it("FIX: multi bind", () => {
                var _port = getPort();
                var _path = process.platform === 'win32' ? "//./pipe/port1_" + _port : unixSocketDir() + '/port1_' + _port;

                var s = new net.Socket(net.AF_UNIX);
                test_util.push(s);

                del(_path);
                s.bind(_path);

                var s1 = new net.Socket(net.AF_UNIX);
                assert.throws(() => {
                    s1.bind(_path);
                });
            });
        });

        if (global.full_test)
            describe("Smtp", () => {
                var s;

                it("new & connect", () => {
                    s = new net.Smtp();
                    s.connect("tcp://smtp.ym.163.com:25");
                    s.socket.close();
                });

                it("net.openSmtp", () => {
                    s = net.openSmtp("tcp://smtp.exmail.qq.com:25");
                });

                it("command", () => {
                    assert.equal(s.command("HELO", "baoz.me").substr(0, 4),
                        "250 ");

                    assert.throws(() => {
                        s.command("FUCK", "baoz.me");
                    });
                });

                it("hello", () => {
                    s.hello();
                });

                it("quit", () => {
                    s.quit();
                });
            });

        it("FIX: net.Smtp results in a segmentation fault", () => {
            new net.Smtp().socket;
        })

        // ─────────────────────────────────────────────────────────────────────
        // TcpServer EventEmitter events
        // ─────────────────────────────────────────────────────────────────────
        describe("TcpServer EventEmitter events", () => {
            var svr;

            afterEach(() => {
                if (svr) {
                    svr.stop();
                    svr = null;
                }
            });

            it("emits 'listening' after start()", () => {
                var fired = false;
                svr = new net.TcpServer(getPort(), (sock) => { sock.close(); });
                svr.on('listening', () => { fired = true; });
                svr.start();
                test_util.push(svr.socket);
                coroutine.sleep(0);
                assert.strictEqual(fired, true);
            });

            it("onlistening shorthand works", () => {
                var fired = false;
                svr = new net.TcpServer(getPort(), (sock) => { sock.close(); });
                svr.onlistening = () => { fired = true; };
                svr.start();
                test_util.push(svr.socket);
                coroutine.sleep(0);
                assert.strictEqual(fired, true);
            });

            it("emits 'connection' when a client connects", () => {
                var conns = 0;
                var p = getPort();
                svr = new net.TcpServer(p, (sock) => { sock.close(); });
                svr.on('connection', () => { conns++; });
                svr.start();
                test_util.push(svr.socket);

                var c = net.connect(p, '127.0.0.1');
                c.close();
                for (var i = 0; i < 10 && conns < 1; i++)
                    coroutine.sleep(0);
                assert.ok(conns >= 1);
            });

            it("emits 'close' after stop()", () => {
                var closed = false;
                svr = new net.TcpServer(getPort(), (sock) => { sock.close(); });
                svr.on('close', () => { closed = true; });
                svr.start();
                test_util.push(svr.socket);
                svr.stop();
                for (var i = 0; i < 10 && !closed; i++)
                    coroutine.sleep(0);
                svr = null;
                assert.strictEqual(closed, true);
            });
        });

        // TcpServer listen() mode — no-port constructor + listen()
        // ─────────────────────────────────────────────────────────
        describe("TcpServer listen() mode", () => {
            var svr;

            afterEach(() => {
                if (svr) {
                    svr.stop();
                    svr = null;
                }
            });

            it("listen(port) binds and starts the server", () => {
                var p = getPort();
                svr = new net.TcpServer((sock) => { sock.close(); });
                svr.listen(p);
                test_util.push(svr.socket);

                var c = net.connect(p, '127.0.0.1');
                c.close();
            });

            it("listen(port, addr) binds to specific address", () => {
                var p = getPort();
                svr = new net.TcpServer((sock) => { sock.close(); });
                svr.listen(p, '127.0.0.1');
                test_util.push(svr.socket);

                var c = net.connect(p, '127.0.0.1');
                c.close();
            });

            it("emits 'listening' event after listen()", () => {
                var fired = false;
                svr = new net.TcpServer((sock) => { sock.close(); });
                svr.on('listening', () => { fired = true; });
                svr.listen(getPort());
                test_util.push(svr.socket);
                coroutine.sleep(0);
                assert.strictEqual(fired, true);
            });

            it("double listen() throws CALL_E_INVALID_CALL", () => {
                var p = getPort();
                svr = new net.TcpServer((sock) => { sock.close(); });
                svr.listen(p);
                test_util.push(svr.socket);
                assert.throws(() => { svr.listen(getPort()); });
            });
        });

        describe("TcpServer Object options constructor", () => {
            var svr;

            afterEach(() => {
                if (svr) {
                    svr.stop();
                    svr = null;
                }
            });

            it("new TcpServer({port}, handler) binds immediately", () => {
                var p = getPort();
                svr = new net.TcpServer({ port: p }, (conn) => {
                    conn.write("opts-port");
                    conn.close();
                });
                svr.start();
                test_util.push(svr.socket);

                var c = net.connect(p, '127.0.0.1');
                assert.equal(c.read().toString(), "opts-port");
                c.close();
            });

            it("new TcpServer({address, port}, handler) binds to specific address", () => {
                var p = getPort();
                svr = new net.TcpServer({ address: net_config.address, port: p }, (conn) => {
                    conn.write("opts-addr-port");
                    conn.close();
                });
                svr.start();
                test_util.push(svr.socket);

                var s = new net.Socket(net_config.family);
                s.connect(p, net_config.address);
                assert.equal(s.read().toString(), "opts-addr-port");
                s.close();
            });

            it("new TcpServer({}, handler) defers — requires listen()", () => {
                var p = getPort();
                svr = new net.TcpServer({}, (conn) => {
                    conn.write("opts-deferred");
                    conn.close();
                });
                svr.listen(p);
                test_util.push(svr.socket);

                var c = net.connect(p, '127.0.0.1');
                assert.equal(c.read().toString(), "opts-deferred");
                c.close();
            });
        });

        describe("TcpServer address()", () => {
            var svr;

            afterEach(() => {
                if (svr) {
                    try {
                        svr.stop();
                    } catch (e) { }
                    svr = null;
                }
            });

            it("returns {address, family, port} after start()", () => {
                var p = getPort();
                svr = new net.TcpServer(p, (sock) => { sock.close(); });
                svr.start();
                test_util.push(svr.socket);

                var addr = svr.address();
                assert.strictEqual(typeof addr.address, 'string');
                assert.strictEqual(typeof addr.family, 'string');
                assert.strictEqual(typeof addr.port, 'number');
                assert.strictEqual(addr.port, p);
                assert.ok(addr.family === 'IPv4' || addr.family === 'IPv6');
            });

            it("returns correct port after listen(0)", () => {
                svr = new net.TcpServer((sock) => { sock.close(); });
                svr.listen(0);
                test_util.push(svr.socket);

                var addr = svr.address();
                assert.ok(addr.port > 0);
                assert.strictEqual(addr.family, 'IPv4');
            });

            it("returns bound address for 127.0.0.1", () => {
                var p = getPort();
                svr = new net.TcpServer('127.0.0.1', p, (sock) => { sock.close(); });
                svr.start();
                test_util.push(svr.socket);

                var addr = svr.address();
                assert.strictEqual(addr.address, '127.0.0.1');
                assert.strictEqual(addr.port, p);
                assert.strictEqual(addr.family, 'IPv4');
            });

            it("throws before socket is bound", () => {
                svr = new net.TcpServer((sock) => { sock.close(); });
                assert.throws(() => { svr.address(); });
            });
        });

        describe("socket.connect options form", () => {
            // Regression: sock.connect({host, port}) after pre-registering on('connect')
            // must fire the 'data' event. Without startConnectEvent() being called in the
            // sync phase of the options overload, m_state never reaches 0 and the
            // AsyncStreamReader never starts — so 'data' silently drops.
            it("data event fires when connect({host, port}) is called after on('connect')", () => {
                var p = getPort();
                var svr = net.createServer((conn) => {
                    conn.write("hello");
                    conn.close();
                });
                svr.listen(p);
                test_util.push(svr.socket);

                var received = null;
                var done = new coroutine.Event();
                var timedOut = false;

                var sock = new net.Socket();
                sock.on('connect', () => {
                    sock.on('data', (chunk) => {
                        received = chunk.toString();
                        done.set();
                    });
                    sock.resume();
                });
                sock.connect({ host: '127.0.0.1', port: p });

                setTimeout(() => {
                    timedOut = true;
                    done.set();
                }, 3000);

                done.wait();
                sock.close();

                assert.equal(timedOut, false, "timed out waiting for data event");
                assert.equal(received, "hello");
            });
        });

        describe("stream writable/readable/_readableState", () => {
            it("new Socket has writable, readable, _readableState", () => {
                var sock = new net.Socket();
                assert.strictEqual(typeof sock.writable, 'boolean');
                assert.strictEqual(typeof sock.readable, 'boolean');
                assert.ok(sock._readableState);
                assert.strictEqual(sock._readableState.ended, false);
            });

            it("writable is true before close, false after close", () => {
                var p = getPort();
                var svr = net.createServer((conn) => {
                    conn.write("hi");
                    conn.close();
                });
                svr.listen(p);
                test_util.push(svr.socket);

                var sock = new net.Socket();
                assert.strictEqual(sock.writable, true);

                var done = new coroutine.Event();
                sock.on('close', () => {
                    assert.strictEqual(sock.writable, false);
                    assert.strictEqual(sock._readableState.ended, true);
                    done.set();
                });
                sock.connect({ host: '127.0.0.1', port: p });
                sock.resume();

                done.wait();
            });
        });

        describe("net.createServer", () => {
            var svr;

            afterEach(() => {
                if (svr) {
                    svr.stop();
                    svr = null;
                }
            });

            it("createServer(handler) returns unbound TcpServer", () => {
                var p = getPort();
                svr = net.createServer((conn) => {
                    conn.write("cs-handler");
                    conn.close();
                });
                svr.listen(p);
                test_util.push(svr.socket);

                var c = net.connect(p, '127.0.0.1');
                assert.equal(c.read().toString(), "cs-handler");
                c.close();
            });

            it("createServer({port}, handler) binds immediately", () => {
                var p = getPort();
                svr = net.createServer({ port: p }, (conn) => {
                    conn.write("cs-opts-port");
                    conn.close();
                });
                svr.start();
                test_util.push(svr.socket);

                var c = net.connect(p, '127.0.0.1');
                assert.equal(c.read().toString(), "cs-opts-port");
                c.close();
            });

            it("createServer({}, handler) defers — requires listen()", () => {
                var p = getPort();
                svr = net.createServer({}, (conn) => {
                    conn.write("cs-opts-deferred");
                    conn.close();
                });
                svr.listen(p);
                test_util.push(svr.socket);

                var c = net.connect(p, '127.0.0.1');
                assert.equal(c.read().toString(), "cs-opts-deferred");
                c.close();
            });
        });
    });
}

test_net("ev", false);
test_net("uv", true);
