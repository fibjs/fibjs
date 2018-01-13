const dgram = require('dgram');
const coroutine = require('coroutine');
const test = require('test');
test.setup();

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
        s.bind(10000);
        s.close();
    });

    it('throw bind on bound socket', () => {
        const s = dgram.createSocket('udp4');
        s.bind(10001);

        assert.throws(() => {
            s.bind(10002);
        });

        s.close();
    });

    it('setRecvBufferSize', () => {
        const s = dgram.createSocket('udp4');

        s.setRecvBufferSize(5120);

        var sz = s.getRecvBufferSize();
        s.setRecvBufferSize(1024);
        assert.notEqual(s.getRecvBufferSize(), sz);

        sz = s.getRecvBufferSize();
        s.setRecvBufferSize(4096);
        assert.notEqual(s.getRecvBufferSize(), sz);
    });

    it('setSendBufferSize', () => {
        const s = dgram.createSocket('udp4');

        s.setSendBufferSize(5120);

        var sz = s.getSendBufferSize();
        s.setSendBufferSize(1024);
        assert.notEqual(s.getSendBufferSize(), sz);

        sz = s.getSendBufferSize();
        s.setSendBufferSize(4096);
        assert.notEqual(s.getSendBufferSize(), sz);
    });

    it('send/message', () => {
        var t = false;
        const s = dgram.createSocket('udp4');
        s.on('message', (msg, addr) => {
            assert.equal(msg.toString(), '123456');
            t = true;
        });

        s.bind(10002);

        const c = dgram.createSocket('udp4');
        c.send('123456', 10002);

        coroutine.sleep(100);
        assert.isTrue(t);

        c.close();
        s.close();
    });

    xit('send/message ipv6', () => {
        var t = false;
        const s = dgram.createSocket('udp6');
        s.on('message', (msg, addr) => {
            assert.equal(msg.toString(), '123456');
            t = true;
        });

        s.bind(10003);

        const c = dgram.createSocket('udp6');
        c.send('123456', 10003);

        coroutine.sleep(100);
        assert.isTrue(t);

        c.close();
        s.close();
    });
});

require.main === module && test.run(console.DEBUG);