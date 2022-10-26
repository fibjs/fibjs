const dns = require('dns');
const net = require('net');
const test = require('test');
test.setup();

describe('dns', () => {
    it('dns.resolve', () => {
        dns.resolve('www.icann.org');
    });

    it('FIX: crash in dns.resolve when host is unknown', () => {
        assert.throws(() => {
            dns.resolve('999.999.999.999');
        });
    });

    it('dns.lookup', () => {
        dns.lookup('www.icann.org');
    });

    it('FIX: error result in dns.lookup when host is unknown', () => {
        assert.throws(() => {
            dns.lookup('999.999.999.999');
        });
    });

    it('net.resolve', () => {
        net.resolve('www.icann.org');
    });

    it('FIX: crash in net.resolve when host is unknown', () => {
        assert.throws(() => {
            net.resolve('999.999.999.999');
        });
    });
});

require.main === module && test.run(console.DEBUG);