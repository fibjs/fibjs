var test = require("test");
test.setup();

var dns = require("dns");
var coroutine = require("coroutine");
var resolver, result;

var ipReg = "^((25[0-5]|2[0-4]\\d|1\\d{2}|[1-9]?\\d|\\d)\\.){3}(25[0-5]|2[0-4]\\d|1\\d{2}|[1-9]?\\d|\\d)$"

describe('dns', () => {
    function testIp(ip) {
        assert.ok(new RegExp(ipReg).test(ip))
    }
    it('new Resolver', () => {
        resolver = new dns.Resolver();
        assert.equal(typeof resolver.resolve, 'function');
    });

    it('resolve', () => {
        dns.resolve('fibjs.org', function (err, result) {
            assert.notOk(err);
            assert.ok(result.length > 0);
            testIp(result[0])
        });
        //reentry
        result = dns.resolve('fibjs.org');
        assert.ok(result.length > 0);

        resolver.resolve('fibjs.org', function (err, result) {
            assert.notOk(err);
            assert.ok(result.length > 0);
        });
        //reentry
        result = resolver.resolve('fibjs.org');
        assert.ok(result.length > 0);
    });

    it('resolveSoa', () => {
        result = dns.resolveSoa('dns.he.net');
        assert.ok(!Array.isArray(result));
        assert.ok(typeof result.nsname === 'string');
        assert.ok(typeof result.hostmaster === 'string');
        assert.ok(typeof result.serial === 'number');
        assert.ok(typeof result.refresh === 'number');
        assert.ok(typeof result.retry === 'number');
        assert.ok(typeof result.expire === 'number');
        assert.ok(typeof result.minttl === 'number');

        dns.resolveSoa('dns.he.net', function (err, result) {
            assert.notOk(err);
            assert.ok(!Array.isArray(result));
            assert.ok(typeof result.nsname === 'string');
            assert.ok(typeof result.hostmaster === 'string');
            assert.ok(typeof result.serial === 'number');
            assert.ok(typeof result.refresh === 'number');
            assert.ok(typeof result.retry === 'number');
            assert.ok(typeof result.expire === 'number');
            assert.ok(typeof result.minttl === 'number');
        });

        result = dns.resolve('dns.he.net', 'SOA');
        assert.ok(!Array.isArray(result));
        assert.ok(typeof result.nsname === 'string');
        assert.ok(typeof result.hostmaster === 'string');
        assert.ok(typeof result.serial === 'number');
        assert.ok(typeof result.refresh === 'number');
        assert.ok(typeof result.retry === 'number');
        assert.ok(typeof result.expire === 'number');
        assert.ok(typeof result.minttl === 'number');

        result = resolver.resolveSoa('dns.he.net');
        assert.ok(!Array.isArray(result));
        assert.ok(typeof result.nsname === 'string');
        assert.ok(typeof result.hostmaster === 'string');
        assert.ok(typeof result.serial === 'number');
        assert.ok(typeof result.refresh === 'number');
        assert.ok(typeof result.retry === 'number');
        assert.ok(typeof result.expire === 'number');
        assert.ok(typeof result.minttl === 'number');

        assert.throws(() => {
            dns.resolveSoa('fibjs');
        });

        dns.resolveSoa('fibjs', function (err, result) {
            assert.ok(err);
            assert.notOk(result);
        });
    });

    it('resolveMx', () => {
        result = dns.resolveMx('qq.com');
        assert.ok(result.length > 0);
        assert.ok(typeof result[0].exchange === 'string');
        assert.ok(typeof result[0].priority === 'number');

        dns.resolveMx('qq.com', function (err, result) {
            assert.notOk(err);
            assert.ok(result.length > 0);
            assert.ok(typeof result[0].exchange === 'string');
            assert.ok(typeof result[0].priority === 'number');
        });

        result = dns.resolve('qq.com', 'MX');
        assert.ok(result.length > 0);
        assert.ok(typeof result[0].exchange === 'string');
        assert.ok(typeof result[0].priority === 'number');

        result = resolver.resolveMx('qq.com');
        assert.ok(result.length > 0);
        assert.ok(typeof result[0].exchange === 'string');
        assert.ok(typeof result[0].priority === 'number');

        assert.throws(() => {
            dns.resolveMx('fibjs');
        });

        dns.resolveMx('fibjs', function (err, result) {
            assert.ok(err);
            assert.notOk(result);
        });
    });

    it('resolveSrv', () => {
        result = dns.resolveSrv('_jabber._tcp.google.com');
        assert.ok(result.length > 0);
        assert.ok(typeof result[0].name === 'string');
        assert.ok(typeof result[0].port === 'number');
        assert.ok(typeof result[0].priority === 'number');
        assert.ok(typeof result[0].weight === 'number');

        dns.resolveSrv('_jabber._tcp.google.com', function (err, result) {
            assert.notOk(err);
            assert.ok(result.length > 0);
            assert.ok(typeof result[0].name === 'string');
            assert.ok(typeof result[0].port === 'number');
            assert.ok(typeof result[0].priority === 'number');
            assert.ok(typeof result[0].weight === 'number');
        });

        result = dns.resolve('_jabber._tcp.google.com', 'SRV');
        assert.ok(result.length > 0);
        assert.ok(typeof result[0].name === 'string');
        assert.ok(typeof result[0].port === 'number');
        assert.ok(typeof result[0].priority === 'number');
        assert.ok(typeof result[0].weight === 'number');

        result = resolver.resolveSrv('_jabber._tcp.google.com');
        assert.ok(result.length > 0);
        assert.ok(typeof result[0].name === 'string');
        assert.ok(typeof result[0].port === 'number');
        assert.ok(typeof result[0].priority === 'number');
        assert.ok(typeof result[0].weight === 'number');

        assert.throws(() => {
            dns.resolveSrv('fibjs');
        });

        dns.resolveSrv('fibjs', function (err, result) {
            assert.ok(err);
            assert.notOk(result);
        });
    });

    it('resolveTxt', () => {
        result = dns.resolveTxt('google.com');
        assert.ok(result.length > 0);
        assert.ok(result[0].length > 0);
        assert.ok(result.some((re) => re[0].indexOf('all') > -1));

        dns.resolveTxt('google.com', function (err, result) {
            assert.notOk(err);
            assert.ok(result.length > 0);
            assert.ok(result[0].length > 0);
            assert.ok(result.some((re) => re[0].indexOf('all') > -1));
        });

        result = dns.resolve('google.com', 'TXT');
        assert.ok(result.length > 0);
        assert.ok(result[0].length > 0);
        assert.ok(result.some((re) => re[0].indexOf('all') > -1));

        result = resolver.resolveTxt('google.com');
        assert.ok(result.length > 0);
        assert.ok(result[0].length > 0);
        assert.ok(result.some((re) => re[0].indexOf('all') > -1));

        assert.throws(() => {
            dns.resolveTxt('fibjs');
        });

        dns.resolveTxt('fibjs', function (err, result) {
            assert.ok(err);
            assert.notOk(result);
        });
    });

    it('resolveCname', () => {

        result = dns.resolveCname('www.microsoft.com');
        assert.ok(result.length > 0);
        assert.ok(result[0].indexOf('microsoft') !== -1);

        dns.resolveCname('www.microsoft.com', function (err, result) {
            assert.notOk(err);
            assert.ok(result.length > 0);
            assert.ok(result[0].indexOf('microsoft') !== -1);
        });

        result = dns.resolve('www.microsoft.com', 'CNAME');
        assert.ok(result.length > 0);
        assert.ok(result[0].indexOf('microsoft') !== -1);

        result = resolver.resolveCname('www.microsoft.com');
        assert.ok(result.length > 0);
        assert.ok(result[0].indexOf('microsoft') !== -1);

        assert.throws(() => {
            dns.resolveCname('fibjs');
        });

        dns.resolveCname('fibjs', function (err, result) {
            assert.ok(err);
            assert.notOk(result);
        });
    });

    it('resolve4', () => {
        result = dns.resolve4('qq.com');
        assert.ok(result.length > 0);

        dns.resolve4('qq.com', function (err, result) {
            assert.notOk(err);
            assert.ok(result.length > 0);
        });

        result = dns.resolve('qq.com', 'A');
        assert.ok(result.length > 0);

        result = resolver.resolve4('qq.com');
        assert.ok(result.length > 0);

        result = dns.resolve4('qq.com', { ttl: true });
        assert.ok(result.length > 0);
        assert.ok(typeof result[0].address === 'string');
        assert.ok(typeof result[0].ttl === 'number');

        dns.resolve4('qq.com', { ttl: true }, function (err, result) {
            assert.notOk(err);
            assert.ok(result.length > 0);
            assert.ok(typeof result[0].address === 'string');
            assert.ok(typeof result[0].ttl === 'number');
        });

        result = resolver.resolve4('qq.com', { ttl: true });
        assert.ok(result.length > 0);
        assert.ok(typeof result[0].address === 'string');
        assert.ok(typeof result[0].ttl === 'number');

        assert.throws(() => {
            dns.resolve4('fibjs');
        });

        dns.resolve4('fibjs', function (err, result) {
            assert.ok(err);
            assert.notOk(result);
        });
        // parallel
        var resolvers = [new dns.Resolver()]
        resolvers.push(new dns.Resolver())
        resolvers.push(new dns.Resolver())
        resolvers.push(new dns.Resolver())
        coroutine.parallel(resolvers, (resolver) => {
            result = resolver.resolve4('qq.com')
            assert.ok(result.length > 0);
        })
    });

    it('resolve6', () => {
        result = dns.resolve6('tv6.ustc.edu.cn');
        assert.ok(result.length > 0);

        dns.resolve6('tv6.ustc.edu.cn', function (err, result) {
            assert.notOk(err);
            assert.ok(result.length > 0);
        });

        result = dns.resolve('tv6.ustc.edu.cn', 'AAAA');
        assert.ok(result.length > 0);

        result = resolver.resolve6('tv6.ustc.edu.cn');
        assert.ok(result.length > 0);

        result = dns.resolve6('tv6.ustc.edu.cn', { ttl: true });
        assert.ok(result.length > 0);
        assert.ok(typeof result[0].address === 'string');
        assert.ok(typeof result[0].ttl === 'number');

        dns.resolve6('tv6.ustc.edu.cn', { ttl: true }, function (err, result) {
            assert.notOk(err);
            assert.ok(result.length > 0);
            assert.ok(typeof result[0].address === 'string');
            assert.ok(typeof result[0].ttl === 'number');
        });

        result = resolver.resolve6('tv6.ustc.edu.cn', { ttl: true });
        assert.ok(result.length > 0);
        assert.ok(typeof result[0].address === 'string');
        assert.ok(typeof result[0].ttl === 'number');

        assert.throws(() => {
            dns.resolve6('fibjs');
        });

        dns.resolve6('fibjs', function (err, result) {
            assert.ok(err);
            assert.notOk(result);
        });
    });

    it('resolveNs', () => {
        result = dns.resolveNs('rackspace.com');
        assert.ok(result.length > 0);
        assert.ok(result[0].indexOf('rackspace.com') !== -1);

        dns.resolveNs('rackspace.com', function (err, result) {
            assert.notOk(err);
            assert.ok(result.length > 0);
            assert.ok(result[0].indexOf('rackspace.com') !== -1);
        });

        result = dns.resolve('rackspace.com', 'NS');
        assert.ok(result.length > 0);
        assert.ok(result[0].indexOf('rackspace.com') !== -1);

        result = resolver.resolveNs('rackspace.com');
        assert.ok(result.length > 0);
        assert.ok(result[0].indexOf('rackspace.com') !== -1);

        assert.throws(() => {
            dns.resolveNs('fibjs');
        });

        dns.resolveNs('fibjs', function (err, result) {
            assert.ok(err);
            assert.notOk(result);
        });
    });

    it('resolvePtr', () => {
        result = dns.resolvePtr('8.8.8.8.in-addr.arpa');
        assert.ok(result.length > 0);
        assert.ok(result[0].indexOf('google') !== -1);

        dns.resolvePtr('8.8.8.8.in-addr.arpa', function (err, result) {
            assert.notOk(err);
            assert.ok(result.length > 0);
            assert.ok(result[0].indexOf('google') !== -1);
        });

        result = dns.resolve('8.8.8.8.in-addr.arpa', 'PTR');
        assert.ok(result.length > 0);
        assert.ok(result[0].indexOf('google') !== -1);

        result = resolver.resolvePtr('8.8.8.8.in-addr.arpa');
        assert.ok(result.length > 0);
        assert.ok(result[0].indexOf('google') !== -1);

        assert.throws(() => {
            dns.resolvePtr('fibjs');
        });

        dns.resolvePtr('fibjs', function (err, result) {
            assert.ok(err);
            assert.notOk(result);
        });
    });

    it('resolveNaptr', () => {

        result = dns.resolveNaptr('sip2sip.info');
        assert.ok(result.length > 0);
        assert.ok(typeof result[0].flags === 'string');
        assert.ok(typeof result[0].service === 'string');
        assert.ok(typeof result[0].regexp === 'string');
        assert.ok(typeof result[0].replacement === 'string');
        assert.ok(typeof result[0].order === 'number');
        assert.ok(typeof result[0].preference === 'number');

        dns.resolveNaptr('sip2sip.info', function (err, result) {
            assert.notOk(err);
            assert.ok(result.length > 0);
            assert.ok(typeof result[0].flags === 'string');
            assert.ok(typeof result[0].service === 'string');
            assert.ok(typeof result[0].regexp === 'string');
            assert.ok(typeof result[0].replacement === 'string');
            assert.ok(typeof result[0].order === 'number');
            assert.ok(typeof result[0].preference === 'number');
        });

        result = dns.resolve('sip2sip.info', 'NAPTR');
        assert.ok(result.length > 0);
        assert.ok(typeof result[0].flags === 'string');
        assert.ok(typeof result[0].service === 'string');
        assert.ok(typeof result[0].regexp === 'string');
        assert.ok(typeof result[0].replacement === 'string');
        assert.ok(typeof result[0].order === 'number');
        assert.ok(typeof result[0].preference === 'number');

        result = resolver.resolveNaptr('sip2sip.info');
        assert.ok(result.length > 0);
        assert.ok(typeof result[0].flags === 'string');
        assert.ok(typeof result[0].service === 'string');
        assert.ok(typeof result[0].regexp === 'string');
        assert.ok(typeof result[0].replacement === 'string');
        assert.ok(typeof result[0].order === 'number');
        assert.ok(typeof result[0].preference === 'number');

        assert.throws(() => {
            dns.resolveNaptr('fibjs');
        });

        dns.resolveNaptr('fibjs', function (err, result) {
            assert.ok(err);
            assert.notOk(result);
        });
    });

    it('resolveAny', () => {
        result = dns.resolveAny('fibjs.org');
        assert.ok(result.length > 0);
        var types = ["A", "AAAA", "MX", "TXT", "SRV", "NS", "CNAME", "PTR", "NAPTR", "SOA"]
        var validator = [(data) => {
            assert.equal(data.type, 'A')
            assert.ok(typeof data.address === 'string');
            assert.ok(typeof data.ttl === 'number');
        }, (data) => {
            assert.equal(data.type, 'AAAA')
            assert.ok(typeof data.address === 'string');
            assert.ok(typeof data.ttl === 'number');
        }, (data) => {
            assert.equal(data.type, 'MX')
            assert.ok(typeof data.exchange === 'string');
            assert.ok(typeof data.priority === 'number');
        }, (data) => {
            assert.equal(data.type, 'TXT')
            assert.ok(typeof data.entries === 'object');
            assert.equal(data.entries.length, 1);
        }, (data) => {
            assert.equal(data.type, 'SRV')
            assert.ok(typeof data.name === 'string');
            assert.ok(typeof data.port === 'number');
            assert.ok(typeof data.priority === 'number');
            assert.ok(typeof data.weight === 'number');
        }, (data) => {
            assert.equal(data.type, 'NS')
            assert.ok(typeof data.value === 'string');
        }, (data) => {
            assert.equal(data.type, 'CNAME')
            assert.ok(typeof data.value === 'string');
        }, (data) => {
            assert.equal(data.type, 'PTR')
            assert.ok(typeof data.value === 'string');
        }, (data) => {
            assert.equal(data.type, 'NAPTR')
            assert.ok(typeof data.flags === 'string');
            assert.ok(typeof data.service === 'string');
            assert.ok(typeof data.regexp === 'string');
            assert.ok(typeof data.replacement === 'string');
            assert.ok(typeof data.order === 'number');
            assert.ok(typeof data.preference === 'number');
        }, (data) => {
            assert.equal(data.type, 'SOA')
            assert.ok(typeof data.nsname === 'string');
            assert.ok(typeof data.hostmaster === 'string');
            assert.ok(typeof data.serial === 'number');
            assert.ok(typeof data.refresh === 'number');
            assert.ok(typeof data.retry === 'number');
            assert.ok(typeof data.expire === 'number');
            assert.ok(typeof data.minttl === 'number');
        }]
        result = dns.resolveAny('google.com')
        assert.ok(result.length > 0)
        result.forEach((r) => {
            validator[types.indexOf(r.type)](r)
        })
    })

    it('reverse', ()=>{
        try {
            dns.reverse('bogus ip')
        } catch(e) {
            assert.ok(e.message.indexOf('Invalid procedure call') > -1)
        }

        try {
            dns.reverse('203.0.113.0')
        } catch(e) {
            assert.ok(e.message.indexOf('Domain name not found') > -1)
        }
    })

    it('getServers & setServers', () => {
        const existing = dns.getServers();
        assert.ok(existing.length > 0);

        // Verify that setServers() handles arrays with holes and other oddities
        {
            const servers = [];

            servers[0] = '127.0.0.1';
            servers[2] = '0.0.0.0';
            dns.setServers(servers);

            assert.deepEqual(dns.getServers(), ['127.0.0.1', '0.0.0.0']);
        }

        {
            const servers = ['127.0.0.1', '192.168.1.1'];

            servers[3] = '127.1.0.1';
            servers[4] = '127.1.0.1';
            servers[5] = '127.1.1.1';

            Object.defineProperty(servers, 2, {
                enumerable: true,
                get: () => {
                    servers.length = 3;
                    return '0.0.0.0';
                }
            });

            dns.setServers(servers);
            assert.deepEqual(dns.getServers(), [
                '127.0.0.1',
                '192.168.1.1',
                '0.0.0.0'
            ]);
        }

        {
            // Various invalidities, all of which should throw a clean error.
            const invalidServers = [
                ' ',
                '\n',
                '\0',
                '1'.repeat(3 * 4),
                // Check for REDOS issues.
                ':'.repeat(100000),
                '['.repeat(100000),
                '['.repeat(100000) + ']'.repeat(100000) + 'a'
            ];
            invalidServers.forEach((serv) => {
                assert.throws(
                    () => {
                        dns.setServers([serv]);
                    }
                );
            });
        }

        const goog = [
            '8.8.8.8',
            '8.8.4.4',
        ];
        dns.setServers(goog);
        assert.deepEqual(dns.getServers(), goog);

        assert.throws(
            () => {
                dns.setServers(['foobar'])
            }
        );

        assert.throws(
            () => {
                dns.setServers(['127.0.0.1:va'])
            }
        );

        assert.deepEqual(dns.getServers(), goog);

        const goog6 = [
            '2001:4860:4860::8888',
            '2001:4860:4860::8844',
        ];
        dns.setServers(goog6);
        assert.deepEqual(dns.getServers(), goog6);

        goog6.push('4.4.4.4');
        dns.setServers(goog6);
        assert.deepEqual(dns.getServers(), goog6);

        const ports = [
            '4.4.4.4:53',
            '[2001:4860:4860::8888]:53',
            '103.238.225.181:666',
            '[fe80::483a:5aff:fee6:1f04]:666'
        ];
        const portsExpected = [
            '4.4.4.4',
            '2001:4860:4860::8888',
            '103.238.225.181:666',
            '[fe80::483a:5aff:fee6:1f04]:666'
        ];
        dns.setServers(ports);
        assert.deepEqual(dns.getServers(), portsExpected);

        dns.setServers([]);
        assert.deepEqual(dns.getServers(), []);

    })
});

require.main === module && test.run(console.DEBUG);
