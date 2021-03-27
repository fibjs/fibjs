const dgram = require('dgram');
const coroutine = require('coroutine');
const os = require('os');

const test = require('test');
test.setup();

var base_port = coroutine.vmid * 10000;
const test_util = require('./test_util');

var has_ipv6 = false;

var ni = os.networkInterfaces();

for (var n in ni) {
    ni[n].forEach((c) => {
        if (c.family == 'IPv6')
            has_ipv6 = true;
    });
}

describe('dgram', () => {
    it('create/close', () => {
        var t = false;
        const s = dgram.createSocket('udp4');
        s.close(() => {
            t = true;
        });

        coroutine.sleep(100);
        assert.isTrue(t);
    });

    it('throw close on closed socket', () => {
        const s = dgram.createSocket('udp4');
        s.close();

        assert.throws(() => {
            s.close();
        });
    });

    it('bind', () => {
        const s = dgram.createSocket('udp4');
        s.bind(base_port + 1000);
        s.close();
    });

    it('throw bind on bound socket', () => {
        const s = dgram.createSocket('udp4');
        s.bind(base_port + 1001);

        assert.throws(() => {
            s.bind(base_port + 1002);
        });

        s.close();
    });

    it('setRecvBufferSize', () => {
        const s = dgram.createSocket('udp4');
        s.bind(0);

        s.setRecvBufferSize(5120);

        var sz = s.getRecvBufferSize();
        s.setRecvBufferSize(1024);
        assert.notEqual(s.getRecvBufferSize(), sz);

        sz = s.getRecvBufferSize();
        s.setRecvBufferSize(4096);
        assert.notEqual(s.getRecvBufferSize(), sz);

        s.close();
    });

    it('setSendBufferSize', () => {
        const s = dgram.createSocket('udp4');
        s.bind(0);

        s.setSendBufferSize(5120);

        var sz = s.getSendBufferSize();
        s.setSendBufferSize(1024);
        assert.notEqual(s.getSendBufferSize(), sz);

        sz = s.getSendBufferSize();
        s.setSendBufferSize(4096);
        assert.notEqual(s.getSendBufferSize(), sz);

        s.close();
    });

    it('option', () => {
        const s = dgram.createSocket('udp4');
        s.bind(0);
        var recv_size = s.getRecvBufferSize();
        var send_size = s.getSendBufferSize();
        s.close();

        const socket = dgram.createSocket({
            type: 'udp4',
            recvBufferSize: 1234,
            sendBufferSize: 1234
        });

        socket.bind(0);

        assert.notEqual(socket.getRecvBufferSize(), recv_size);
        assert.notEqual(socket.getSendBufferSize(), send_size);

        socket.close();
    });

    it('address', done => {
        const socket = dgram.createSocket('udp4');

        socket.on('listening', () => {
            try {
                const address = socket.address();

                assert.strictEqual(address.address, "127.0.0.1");
                assert.strictEqual(typeof address.port, 'number');
                assert.ok(address.port > 0);
                assert.strictEqual(address.family, 'IPv4');

                done();
            } catch (e) {
                done(e);
            } finally {
                socket.close();
            }
        });

        socket.bind(0, "127.0.0.1");
    });

    function test_message(name, value, port) {
        it(`send ${name}`, done => {
            var t = false;
            const s = dgram.createSocket('udp4');
            s.on('message', (msg, addr) => {
                s.off('message');
                s.on('message', (msg, addr) => {
                    c.close();
                    s.close();

                    done();
                });

                assert.equal(msg.toString(), value);
                s.send(msg, addr.port, addr.address);
            });

            s.bind(base_port + port);

            const c = dgram.createSocket('udp4');
            c.on('message', (msg, addr) => {
                assert.equal(msg.toString(), value);
                c.send(msg, addr.port, addr.address);
            });

            c.send(value, base_port + port);
        });
    }

    test_message('message', "123456", 1002);
    test_message('empty message', "", 1003);
    test_message('big message', new Buffer(4000).hex(), 1004);

    if (has_ipv6)
        describe("ipv6", () => {
            it('ipv6 address', done => {
                const socket = dgram.createSocket('udp6');

                socket.on('listening', () => {
                    try {
                        const address = socket.address();

                        assert.strictEqual(address.address, "::1");
                        assert.strictEqual(typeof address.port, 'number');
                        assert.ok(address.port > 0);
                        assert.strictEqual(address.family, 'IPv6');

                        done();
                    } catch (e) {
                        done(e);
                    } finally {
                        socket.close();
                    }
                });

                socket.bind(0, "::1");
            });

            it('send/message ipv6', done => {
                var t = false;
                const s = dgram.createSocket({
                    type: 'udp6',
                    ipv6Only: true
                });
                s.on('message', (msg, addr) => {
                    s.off('message');
                    s.on('message', (msg, addr) => {
                        c.close();
                        s.close();

                        done();
                    });

                    assert.equal(msg.toString(), "0123456");
                    s.send(msg, addr.port, addr.address);
                });

                s.bind(base_port + 1005);

                const c = dgram.createSocket('udp6');
                c.on('message', (msg, addr) => {
                    assert.equal(msg.toString(), "0123456");
                    c.send(msg, addr.port, addr.address);
                });

                c.send("0123456", base_port + 1005);
            });
        });

    it("broadcast", () => {
        var t = false;
        const s = dgram.createSocket('udp4');
        s.on('message', (msg, addr) => {
            assert.equal(msg.toString(), '123456');
            t = true;
        });

        s.bind(base_port + 1006);

        const c = dgram.createSocket('udp4');

        assert.throws(() => {
            c.send('123456', base_port + 1006, "255.255.255.255");
        });

        c.setBroadcast(true);
        c.send('123456', base_port + 1006, "255.255.255.255");

        coroutine.sleep(100);

        c.close();
        s.close();

        assert.isTrue(t);
    });

    it("gc", () => {
        test_util.gc();

        var n = test_util.countObject("DgramSocket");

        var s = dgram.createSocket('udp4');
        s.bind(0);
        s.close();
        s = undefined;

        test_util.gc();

        assert.equal(n, test_util.countObject("DgramSocket"));
    });

    it("not keep unclosed socket in gc", () => {
        test_util.gc();

        var n = test_util.countObject("DgramSocket");

        var s = dgram.createSocket('udp4');
        s = undefined;

        test_util.gc();

        assert.equal(n, test_util.countObject("DgramSocket"));
    });

    it("hold bind socket in gc", () => {
        test_util.gc();

        var c = dgram.createSocket('udp4');

        var n = test_util.countObject("DgramSocket");

        var s = dgram.createSocket('udp4');
        s.on('message', function (msg) {
            this.close();
        })
        s.bind(base_port + 1006);
        s = undefined;

        test_util.gc();
        assert.equal(n + 1, test_util.countObject("DgramSocket"));

        c.send('', base_port + 1006);
        coroutine.sleep(100);

        test_util.gc();
        assert.equal(n, test_util.countObject("DgramSocket"));
        c.close();
    });

    it('FIX: crash in send', () => {
        const c = dgram.createSocket('udp4');
        c.send('123456', 1, 2, base_port + 1008);
        c.close();
    });
});

require.main === module && test.run(console.DEBUG);