var { describe, it, before, after, beforeEach, afterEach } = require('node:test');
var assert = require('assert');

const dns = require('dns');
const net = require('net');
const os = require('os');

var has_ipv6 = dns.lookup('ipv6.com', {
    all: true
}).find((c) => c.family == 6) != null;

describe('dns', () => {
    describe('lookup', () => {
        it('localhost', () => {
            dns.lookup('localhost');
        });

        describe('option', () => {
            it("family", () => {
                const ips = [
                    '198.178.249.201',
                    '2602:fa20:1:40::201'
                ]
                var result = dns.lookup('ipv6.com');
                assert.ok(ips.includes(result));

                var result = dns.lookup('ipv6.com', {
                    family: 0
                });
                assert.ok(ips.includes(result));

                var result = dns.lookup('ipv6.com', {
                    family: 4
                });
                assert.equal(result, '198.178.249.201');

                var result = dns.lookup('ipv6.com', {
                    family: "IPv4"
                });
                assert.equal(result, '198.178.249.201');

                if (has_ipv6) {
                    var result = dns.lookup('ipv6.com', {
                        family: 6
                    });
                    assert.equal(result, '2602:fa20:1:40::201');

                    var result = dns.lookup('ipv6.com', {
                        family: "IPv6"
                    });
                    assert.equal(result, '2602:fa20:1:40::201');
                }
            });

            it("all", () => {
                if (has_ipv6) {
                    var result = dns.lookup('ipv6.com', {
                        all: true
                    }).sort((a, b) => a.family - b.family);
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
                } else {
                    var result = dns.lookup('ipv6.com', {
                        all: true
                    }).sort((a, b) => a.family - b.family);
                    assert.deepEqual(result, [
                        {
                            "address": "198.178.249.201",
                            "family": 4
                        }
                    ]);
                }

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

            if (has_ipv6) {
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
            }
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
