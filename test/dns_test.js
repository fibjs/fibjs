const dns = require('dns');
const net = require('net');
const test = require('test');
test.setup();

describe('dns', () => {
    describe('lookup', () => {
        it('localhost', () => {
            dns.lookup('localhost');
        });

        describe('option', () => {

            it("family", () => {
                var result = dns.lookup('ipv6.com');
                assert.equal(result, '198.178.249.201');

                var result = dns.lookup('ipv6.com', {
                    family: 0
                });
                assert.equal(result, '198.178.249.201');

                var result = dns.lookup('ipv6.com', {
                    family: 4
                });
                assert.equal(result, '198.178.249.201');

                var result = dns.lookup('ipv6.com', {
                    family: 6
                });
                assert.equal(result, '2602:fa20:1:40::201');

                var result = dns.lookup('ipv6.com', {
                    family: "IPv4"
                });
                assert.equal(result, '198.178.249.201');

                var result = dns.lookup('ipv6.com', {
                    family: "IPv6"
                });
                assert.equal(result, '2602:fa20:1:40::201');
            });

            it("all", () => {
                var result = dns.lookup('ipv6.com', {
                    all: true
                });
                assert.deepEqual(result, [
                    {
                        "address": "198.178.249.201",
                        "family": 4
                    },
                    {
                        "address": "2602:fa20:1:40::201",
                        "family": 6
                    }
                ]);

                var result = dns.lookup('ipv6.com', {
                    family: 4,
                    all: true
                });
                assert.deepEqual(result, [
                    {
                        "address": "198.178.249.201",
                        "family": 4
                    }
                ]);
            });

            var result = dns.lookup('ipv6.com', {
                family: 6,
                all: true
            });
            assert.deepEqual(result, [
                {
                    "address": "2602:fa20:1:40::201",
                    "family": 6
                }
            ]);
        });

        it('FIX: error result in dns.lookup when host is unknown', () => {
            assert.throws(() => {
                dns.lookup('999.999.999.999');
            });
        });
    });

    it('dns.resolve', () => {
        dns.resolve('localhost');
    });

    it('FIX: crash in dns.resolve when host is unknown', () => {
        assert.throws(() => {
            dns.resolve('999.999.999.999');
        });
    });

    it('net.resolve', () => {
        net.resolve('localhost');
    });

    it('FIX: crash in net.resolve when host is unknown', () => {
        assert.throws(() => {
            net.resolve('999.999.999.999');
        });
    });
});
