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

        s.setRecvBufferSize(1024);
        assert.equal(s.getRecvBufferSize(), 1024);

        s.setRecvBufferSize(2048);
        assert.equal(s.getRecvBufferSize(), 2048);
    });

    it('setSendBufferSize', () => {
        const s = dgram.createSocket('udp4');

        s.setSendBufferSize(1024);
        assert.equal(s.getSendBufferSize(), 1024);

        s.setSendBufferSize(2048);
        assert.equal(s.getSendBufferSize(), 2048);
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

});

repl && test.run(console.DEBUG);