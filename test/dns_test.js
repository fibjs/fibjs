const dns = require('dns');
const test = require('test');
test.setup();

describe('dns', () => {
    it('dns.resolve', () => {
        dns.resolve('fibjs.org');
    });

    it('FIX: crash in dns.resolve when host is unknown', () => {
        assert.throws(() => {
            dns.resolve('999.999.999.999');
        });
    });

    it('dns.lookup', () => {
        dns.lookup('fibjs.org');
    });

    it('FIX: error result in dns.lookup when host is unknown', () => {
        assert.throws(() => {
            dns.lookup('999.999.999.999');
        });
    });
});

require.main === module && test.run(console.DEBUG);