var { describe, it, before, after } = require('node:test');
var assert = require('assert');

const dns = require('dns');
const dnsPromises = require('dns/promises');
const T = require('./dns_files/dns_helper');

function call(fn) {
    return new Promise((resolve, reject) => {
        fn((err, ...args) => err ? reject(err) : resolve(args));
    });
}

function callArgs(fn) {
    return new Promise((resolve) => {
        fn((...args) => resolve(args));
    });
}

function handler(query) {
    const name = query.name;

    if (name === 'nx.test')
        return { rcode: 3 };
    if (name === 'fail.test')
        return { rcode: 2 };
    if (name.startsWith('drop') && name.endsWith('.test'))
        return null;
    if (name === '89.67.45.123.in-addr.arpa')
        return null;
    if (name === 'empty.test')
        return { answers: [] };
    if (name === 'badcount.test')
        return {
            answers: [T.aRecord(name, '192.0.2.1', 60)],
            // c-ares >= 1.21 discards a response whose ANCOUNT disagrees
            // with the records it contains.
            ancount: 2
        };

    if (name === 'any0.test') {
        return {
            answers: [
                T.aRecord(name, '1.2.3.4', 123),
                T.aaaaRecord(name, '::42', 123),
                T.mxRecord(name, 'foobar.com', 42, 124),
                T.nsRecord(name, 'foobar.org', 457),
                T.txtRecord(name, ['v=spf1 ~all xyz\0foo'], 120),
                T.ptrRecord(name, 'baz.org', 987),
                T.soaRecord(name, 'ns1.example.com', 'admin.example.com',
                    156696742, 900, 900, 1800, 60, 120),
                T.caaRecord(name, 128, 'issue', 'platynum.ch', 120)
            ]
        };
    }

    if (name === 'any1.test') {
        return {
            answers: [
                T.aRecord(name, '192.0.2.1', 11),
                T.aaaaRecord(name, '2001:db8::1', 22),
                T.cnameRecord(name, 'target.test', 33),
                T.mxRecord(name, 'mail.test', 10, 44),
                T.nsRecord(name, 'ns1.test', 55),
                T.txtRecord(name, ['hello', 'world'], 66),
                T.srvRecord(name, 'srv.test', 8080, 1, 2, 77),
                T.ptrRecord(name, 'ptr.test', 88),
                T.naptrRecord(name, 100, 10, 's', 'SIP+D2U', '',
                    '_sip._udp.test', 99),
                T.soaRecord(name, 'ns1.test', 'host.test', 123, 3600, 600,
                    86400, 300, 111),
                T.caaRecord(name, 0, 'issue', 'letsencrypt.org', 122),
                T.tlsaRecord(name, 3, 1, 1, Buffer.from([1, 2, 3, 4]), 133)
            ]
        };
    }

    if (name === 'any2.test') {
        return {
            answers: [
                T.aRecord(name, '192.0.2.2', 11),
                T.cnameRecord(name, 'target2.test', 22)
            ]
        };
    }

    if (query.type === T.A)
        return {
            answers: name === 'multi.test'
                ? [T.aRecord(name, '192.0.2.10', 10),
                    T.aRecord(name, '192.0.2.11', 20)]
                : [T.aRecord(name, '127.0.0.1', 120)]
        };
    if (query.type === T.AAAA)
        return {
            answers: name === 'multi.test'
                ? [T.aaaaRecord(name, '2001:db8::10', 10),
                    T.aaaaRecord(name, '2001:db8::11', 20)]
                : [T.aaaaRecord(name,
                    name === 'v6.test' ? '2001:0db8:0000:0000:0000:0000:0000:0001' : '::1',
                    120)]
        };
    if (query.type === T.CNAME)
        return { answers: [T.cnameRecord(name, 'target.test', 120)] };
    if (query.type === T.NS)
        return { answers: [T.nsRecord(name, 'ns1.test', 120)] };
    if (query.type === T.PTR)
        return {
            answers: name === '2.2.0.192.in-addr.arpa'
                ? [T.ptrRecord(name, 'first.test', 120),
                    T.ptrRecord(name, 'second.test', 120)]
                : [T.ptrRecord(name, 'ptr.test', 120)]
        };
    if (query.type === T.MX)
        return {
            answers: name === 'multi-mx.test'
                ? [T.mxRecord(name, 'mail2.test', 20, 120),
                    T.mxRecord(name, 'mail1.test', 10, 120)]
                : [T.mxRecord(name, 'mail.test', 10, 120)]
        };
    if (query.type === T.TXT)
        return {
            answers: name === 'txt8.test'
                ? [T.txtRecord(name, ['a\0b', ''], 120)]
                : [T.txtRecord(name, ['v=spf1', '-all'], 120)]
        };
    if (query.type === T.SRV)
        return {
            answers: [T.srvRecord(name, 'srv.test', 8080, 1, 2, 120)]
        };
    if (query.type === T.NAPTR)
        return {
            answers: [T.naptrRecord(name, 100, 10, 's', 'SIP+D2U',
                '', '_sip._udp.test', 120)]
        };
    if (query.type === T.CAA)
        return {
            answers: [T.caaRecord(name, 0, 'issue', 'letsencrypt.org', 120)]
        };
    if (query.type === T.SOA)
        return {
            answers: [T.soaRecord(name, 'ns1.test', 'host.test', 2026010101,
                3600, 600, 86400, 300, 120)]
        };
    if (query.type === T.TLSA)
        return {
            answers: [T.tlsaRecord(name, 3, 1, 1, Buffer.from([1, 2, 3, 4]),
                120)]
        };
    if (query.type === T.ANY)
        return {
            answers: [
                T.aRecord(name, '127.0.0.1', 120),
                T.txtRecord(name, ['hello'], 120)
            ]
        };

    return { rcode: 3 };
}

describe('dns', () => {
    let server;
    let port;
    let originalServers;

    before(() => {
        server = T.startDnsServer(handler);
        port = server.port;
        originalServers = dns.getServers();
    });

    after(() => {
        if (server)
            server.close();
        if (originalServers)
            dns.setServers(originalServers);
    });

    function resolver() {
        const r = new dns.Resolver();
        r.setServers(['127.0.0.1:' + port]);
        return r;
    }

    describe('lookup', () => {
        it('callback carries address and family', async () => {
            const args = await callArgs((cb) => dns.lookup('127.0.0.1', cb));
            assert.equal(args.length, 3);
            assert.equal(args[0], null);
            assert.equal(args[1], '127.0.0.1');
            assert.equal(args[2], 4);
        });

        it('all returns an address array with a two-argument callback', async () => {
            const args = await callArgs((cb) =>
                dns.lookup('127.0.0.1', { all: true }, cb));
            assert.equal(args.length, 2);
            assert.deepEqual(args[1], [{ address: '127.0.0.1', family: 4 }]);
        });

        it('promise returns {address, family}', async () => {
            const result = await dnsPromises.lookup('127.0.0.1');
            assert.deepEqual(result, { address: '127.0.0.1', family: 4 });
        });

        it('promise all returns a bare array', async () => {
            const result = await dnsPromises.lookup('127.0.0.1', {
                all: true
            });
            assert.deepEqual(result, [{ address: '127.0.0.1', family: 4 }]);
        });

        it('numeric family overload', async () => {
            const args = await callArgs((cb) =>
                dns.lookup('127.0.0.1', 4, cb));
            assert.equal(args[2], 4);
        });

        it('family and IPv6 literal', () => {
            assert.deepEqual(dns.lookup('::1', { family: 6 }),
                { address: '::1', family: 6 });
        });

        it('string family, hints and order options', () => {
            assert.deepEqual(dns.lookup('127.0.0.1', { family: 'IPv4' }),
                { address: '127.0.0.1', family: 4 });
            assert.deepEqual(dns.lookup('127.0.0.1', {
                family: 4,
                hints: dns.ADDRCONFIG,
                order: 'ipv4first'
            }), { address: '127.0.0.1', family: 4 });
            assert.deepEqual(dns.lookup('127.0.0.1', { verbatim: false }),
                { address: '127.0.0.1', family: 4 });
        });

        it('empty hostname', async () => {
            assert.throws(() => dns.lookup(''),
                { code: 'ERR_INVALID_ARG_VALUE' });

            const err = await new Promise((resolve) =>
                dns.lookup('', (e) => resolve(e)));
            assert.equal(err.code, 'ERR_INVALID_ARG_VALUE');
            assert.match(err.message, /non-empty string/);
        });

        it('error fields', async () => {
            const args = await callArgs((cb) =>
                dns.lookup('999.999.999.999', cb));
            // Node truncates the callback to the error argument on failure.
            assert.equal(args.length, 1);
            const err = args[0];
            assert.ok(err instanceof Error);
            assert.equal(err.code, 'ENOTFOUND');
            assert.equal(err.errno, -3008);
            assert.equal(err.syscall, 'getaddrinfo');
            assert.equal(err.hostname, '999.999.999.999');
        });

        it('validates family, hints and order', () => {
            assert.throws(() => dns.lookup('127.0.0.1', { family: 5 }),
                { code: 'ERR_INVALID_ARG_VALUE' });
            assert.throws(() => dns.lookup('127.0.0.1', 5),
                { code: 'ERR_INVALID_ARG_VALUE' });
            assert.throws(() => dns.lookup('127.0.0.1', { hints: 1 << 20 }),
                { code: 'ERR_INVALID_ARG_VALUE' });
            assert.throws(() => dns.lookup('127.0.0.1', { order: 'bad' }),
                { code: 'ERR_INVALID_ARG_VALUE' });
        });

        it('IP literal ignores a conflicting family', () => {
            assert.deepEqual(dns.lookup('::1', { family: 4 }),
                { address: '::1', family: 6 });
            assert.deepEqual(dns.lookup('::1', { family: 4, all: true }),
                [{ address: '::1', family: 6 }]);
        });

        it('promise rejects an empty hostname', async () => {
            await assert.rejects(async () => dnsPromises.lookup(''),
                { code: 'ERR_INVALID_ARG_VALUE' });
        });
    });

    describe('lookupService', () => {
        it('callback carries hostname and service', async () => {
            const args = await callArgs((cb) =>
                dns.lookupService('127.0.0.1', 80, cb));
            assert.equal(args[0], null);
            assert.equal(typeof args[1], 'string');
            assert.equal(typeof args[2], 'string');
        });

        it('promise shape', async () => {
            const result = await dnsPromises.lookupService('127.0.0.1', 80);
            assert.equal(typeof result.hostname, 'string');
            assert.equal(typeof result.service, 'string');
        });

        it('IPv6 address and boundary ports', async () => {
            const v6 = await dnsPromises.lookupService('::1', 80);
            assert.equal(typeof v6.hostname, 'string');
            assert.ok(v6.service.length > 0);

            const zero = await dnsPromises.lookupService('127.0.0.1', 0);
            assert.equal(zero.service, '0');

            const max = await callArgs((cb) =>
                dns.lookupService('127.0.0.1', 65535, cb));
            assert.equal(max[0], null);
            assert.equal(max[2], '65535');
        });

        it('NI_NAMEREQD failure and invalid ports', async () => {
            await assert.rejects(
                async () => dnsPromises.lookupService('192.0.2.1', 80),
                { code: 'ENOTFOUND' });

            const args = await callArgs((cb) =>
                dns.lookupService('192.0.2.1', 80, cb));
            assert.equal(args.length, 1);
            assert.equal(args[0].code, 'ENOTFOUND');
            assert.equal(args[0].syscall, 'getnameinfo');
            assert.equal(args[0].hostname, '192.0.2.1');

            assert.throws(() => dns.lookupService('127.0.0.1', -1),
                { code: 'ERR_SOCKET_BAD_PORT' });
            assert.throws(() => dns.lookupService('127.0.0.1', 65536),
                { code: 'ERR_SOCKET_BAD_PORT' });
        });

        it('rejects non-IP addresses', () => {
            assert.throws(() => dns.lookupService('not-an-ip', 80),
                { code: 'ERR_INVALID_ARG_VALUE' });
            assert.throws(() => dns.lookupService('127.0.0.1', 80.5),
                { code: 'ERR_SOCKET_BAD_PORT' });
        });
    });

    describe('resolve', () => {
        it('resolve4 and ttl', async () => {
            const r = resolver();
            assert.deepEqual(await r.resolve4('a.test'), ['127.0.0.1']);
            assert.deepEqual((await r.resolve4('a.test', { ttl: true }))[0],
                { address: '127.0.0.1', ttl: 120 });

            assert.deepEqual(await r.resolve4('multi.test'),
                ['192.0.2.10', '192.0.2.11']);
            assert.deepEqual(await r.resolve4('multi.test', { ttl: true }),
                [{ address: '192.0.2.10', ttl: 10 },
                    { address: '192.0.2.11', ttl: 20 }]);
        });

        it('resolve6', async () => {
            assert.deepEqual(await resolver().resolve6('a.test'), ['::1']);
            // RFC 5952 canonical form: lowercase, longest zero run
            // compressed, no leading zeros.
            assert.deepEqual(await resolver().resolve6('v6.test'),
                ['2001:db8::1']);
            assert.deepEqual(await resolver().resolve6('multi.test',
                { ttl: true }), [
                { address: '2001:db8::10', ttl: 10 },
                { address: '2001:db8::11', ttl: 20 }
            ]);
        });

        it('resolveCname / resolveNs / resolvePtr', async () => {
            const r = resolver();
            assert.deepEqual(await r.resolveCname('c.test'), ['target.test']);
            assert.deepEqual(await r.resolveNs('n.test'), ['ns1.test']);
            assert.deepEqual(await r.resolvePtr('p.test'), ['ptr.test']);
            assert.deepEqual(await r.resolve('c.test', 'CNAME'),
                ['target.test']);
            assert.deepEqual(await r.resolve('n.test', 'NS'), ['ns1.test']);
            assert.deepEqual(await r.resolve('p.test', 'PTR'), ['ptr.test']);
        });

        it('resolveMx', async () => {
            const mx = await resolver().resolveMx('m.test');
            assert.deepEqual(mx, [{ exchange: 'mail.test', priority: 10 }]);
            assert.deepEqual(await resolver().resolveMx('multi-mx.test'), [
                { exchange: 'mail2.test', priority: 20 },
                { exchange: 'mail1.test', priority: 10 }
            ]);
        });

        it('resolveTxt', async () => {
            const txt = await resolver().resolveTxt('t.test');
            assert.deepEqual(txt, [['v=spf1', '-all']]);
            // Chunks are binary safe: NUL and an empty chunk survive.
            assert.deepEqual(await resolver().resolveTxt('txt8.test'),
                [['a\0b', '']]);
        });

        it('resolveSrv', async () => {
            const srv = await resolver().resolveSrv('s.test');
            assert.deepEqual(srv, [{
                name: 'srv.test',
                port: 8080,
                priority: 1,
                weight: 2
            }]);
        });

        it('resolveNaptr', async () => {
            const naptr = await resolver().resolveNaptr('n.test');
            assert.deepEqual(naptr, [{
                flags: 's',
                service: 'SIP+D2U',
                regexp: '',
                replacement: '_sip._udp.test',
                order: 100,
                preference: 10
            }]);
        });

        it('resolveCaa', async () => {
            const caa = await resolver().resolveCaa('ca.test');
            assert.deepEqual(caa, [{ critical: 0, issue: 'letsencrypt.org' }]);
        });

        it('resolveSoa', async () => {
            const soa = await resolver().resolveSoa('so.test');
            assert.equal(soa.nsname, 'ns1.test');
            assert.equal(soa.hostmaster, 'host.test');
            assert.equal(soa.serial, 2026010101);
            assert.equal(soa.minttl, 300);
            // Node returns numbers, not BigInt; JSON must stay usable.
            assert.equal(typeof soa.serial, 'number');
            assert.equal(typeof soa.refresh, 'number');
            assert.doesNotThrow(() => JSON.stringify(soa));

            const viaResolve = await resolver().resolve('so.test', 'SOA');
            assert.deepEqual(viaResolve, soa);
        });

        it('resolveTlsa', async () => {
            const tlsa = await resolver().resolveTlsa('tl.test');
            assert.equal(tlsa[0].certUsage, 3);
            assert.equal(tlsa[0].selector, 1);
            assert.equal(tlsa[0].match, 1);
            assert.equal(tlsa[0].data.byteLength, 4);
            assert.ok(tlsa[0].data instanceof ArrayBuffer);
            assert.deepEqual(Array.from(new Uint8Array(tlsa[0].data)),
                [1, 2, 3, 4]);
        });

        it('resolveAny follows Node record grouping', async () => {
            const r = resolver();
            const grouped = await r.resolveAny('any0.test');
            assert.deepEqual(grouped.map((item) => item.type), [
                'A', 'AAAA', 'MX', 'NS', 'TXT', 'PTR', 'SOA', 'CAA'
            ]);
            // TTL is only attached to A/AAAA records.
            assert.equal(grouped[0].ttl, 123);
            assert.equal(grouped[1].ttl, 123);
            assert.equal('ttl' in grouped[2], false);
            assert.equal(grouped[4].entries[0], 'v=spf1 ~all xyz\0foo');
            assert.equal(grouped[6].serial, 156696742);
            assert.equal(grouped[7].issue, 'platynum.ch');

            const any = await r.resolveAny('any1.test');
            assert.deepEqual(any.map((item) => item.type), [
                'CNAME', 'AAAA', 'MX', 'NS', 'TXT', 'SRV', 'PTR', 'NAPTR',
                'SOA', 'TLSA', 'CAA'
            ]);
            // A CNAME suppresses the A group; the address belongs to the
            // CNAME target, not to the queried name.
            assert.equal(any.some((item) => item.type === 'A'), false);
            assert.equal(any[0].value, 'target.test');
            assert.equal(any[1].address, '2001:db8::1');
            assert.deepEqual(any[4].entries, ['hello', 'world']);
            assert.equal(any[5].name, 'srv.test');
            assert.equal(any[8].serial, 123);
            assert.equal(typeof any[8].serial, 'number');
            assert.equal(any[9].type, 'TLSA');
            assert.deepEqual(Array.from(new Uint8Array(any[9].data)),
                [1, 2, 3, 4]);
            assert.equal(any[10].issue, 'letsencrypt.org');
            assert.doesNotThrow(() => JSON.stringify(any));

            const cnameOnly = await r.resolveAny('any2.test');
            assert.deepEqual(cnameOnly.map((item) => item.type), ['CNAME']);
            assert.deepEqual(await r.resolve('any1.test', 'ANY'), any);
        });

        it('resolveAny and rrtype ANY', async () => {
            const r = resolver();
            const any = await r.resolveAny('any.test');
            assert.deepEqual(any.map((item) => item.type), ['A', 'TXT']);
            assert.equal(any[0].address, '127.0.0.1');
            assert.deepEqual(any[1].entries, ['hello']);
            assert.deepEqual(await r.resolve('any.test', 'ANY'), any);
            const tlsa = await r.resolve('tl.test', 'TLSA');
            assert.equal(tlsa[0].certUsage, 3);
        });

        it('resolve defaults to A and rejects unknown types', async () => {
            assert.deepEqual(await resolver().resolve('a.test'), ['127.0.0.1']);
            await assert.rejects(async () =>
                dnsPromises.resolve('a.test', 'BOGUS'),
                { code: 'ERR_INVALID_ARG_VALUE' });
            await assert.rejects(async () => resolver().resolve('a.test', 'bogus'),
                { code: 'ERR_INVALID_ARG_VALUE' });
        });

        it('reverse', async () => {
            assert.deepEqual(await resolver().reverse('192.0.2.1'),
                ['ptr.test']);
            // Every PTR name is reported, and h_name is not duplicated.
            assert.deepEqual(await resolver().reverse('192.0.2.2'),
                ['first.test', 'second.test']);
        });

        it('errors', async () => {
            const r = resolver();
            await assert.rejects(async () => r.resolve4('nx.test'),
                { code: 'ENOTFOUND' });
            await assert.rejects(async () => r.resolve4('fail.test'),
                { code: 'ESERVFAIL' });
            await assert.rejects(async () => r.resolve4('empty.test'),
                { code: 'ENODATA' });
            await assert.rejects(async () => r.resolveAny('empty.test'),
                { code: 'ENODATA' });

            const args = await callArgs((cb) => r.resolve4('nx.test', cb));
            assert.equal(args.length, 1);
            assert.equal(args[0].code, 'ENOTFOUND');
            assert.equal(args[0].syscall, 'queryA');
            assert.equal(args[0].hostname, 'nx.test');
        });

        it('a malformed answer count is discarded', async () => {
            const r = new dns.Resolver({ timeout: 100, tries: 1 });
            r.setServers(['127.0.0.1:' + port]);

            let err;
            try {
                await r.resolveAny('badcount.test');
            } catch (e) {
                err = e;
            }
            assert.match(err.code, /^(EBADRESP|ETIMEOUT)$/);
            assert.equal(err.syscall, 'queryAny');
            assert.equal(err.hostname, 'badcount.test');
        });

        it('reverse validates the IP the way Node does', async () => {
            let err;
            try {
                await resolver().reverse('not-an-ip');
            } catch (e) {
                err = e;
            }
            assert.equal(err.code, 'EINVAL');
            assert.equal(err.syscall, 'getHostByAddr');
            assert.equal(err.message, 'getHostByAddr EINVAL not-an-ip');
        });

        it('concurrent queries on one resolver', async () => {
            const r = resolver();
            const values = await Promise.all(Array.from({ length: 32 },
                () => r.resolve4('a.test')));

            for (const value of values)
                assert.deepEqual(value, ['127.0.0.1']);
        });

        it('multiple resolvers are independent', async () => {
            const r1 = resolver();
            const r2 = resolver();
            r2.setServers(['127.0.0.1:' + port]);

            assert.deepEqual(r1.getServers(), r2.getServers());
            const values = await Promise.all([
                ...Array.from({ length: 16 }, () => r1.resolve4('a.test')),
                ...Array.from({ length: 16 }, () => r2.resolve4('a.test'))
            ]);
            for (const value of values)
                assert.deepEqual(value, ['127.0.0.1']);
        });

        it('an in-flight query keeps the channel alive after GC', async () => {
            let r = resolver();
            const promise = call((cb) => r.resolve4('a.test', cb));
            r = null;
            if (typeof gc === 'function')
                gc();
            assert.deepEqual(await promise, [['127.0.0.1']]);
        });

        it('queries are not cached', async () => {
            const r = resolver();
            const before = server.state.queries.length;
            await r.resolve4('a.test');
            await r.resolve4('a.test');
            assert.equal(server.state.queries.length, before + 2);
        });

        it('timeout', async () => {
            const r = new dns.Resolver({ timeout: 100, tries: 1 });
            r.setServers(['127.0.0.1:' + port]);
            await assert.rejects(async () => r.resolve4('drop.test'),
                { code: 'ETIMEOUT' });
        });

        it('cancel', async () => {
            const r = new dns.Resolver();
            r.setServers(['127.0.0.1:' + port]);

            const before = server.state.queries.length;
            const promise = callArgs((cb) => r.resolve4('drop.test', cb));
            // Wait until the server has seen this query, then cancel.
            for (let i = 0; i < 100 && server.state.queries.length === before; i++)
                await new Promise((resolve) => setTimeout(resolve, 10));
            r.cancel();
            const args = await promise;
            assert.equal(args[0].code, 'ECANCELLED');
        });

        it('cancel reports queryA for the pending query', async () => {
            const r = new dns.Resolver();
            r.setServers(['127.0.0.1:' + port]);

            const before = server.state.queries.length;
            const promise = callArgs((cb) => r.resolve4('drop.test', cb));
            for (let i = 0; i < 100 && server.state.queries.length === before; i++)
                await new Promise((resolve) => setTimeout(resolve, 10));
            r.cancel();

            const args = await promise;
            assert.equal(args[0].code, 'ECANCELLED');
            assert.equal(args[0].syscall, 'queryA');
            assert.equal(args[0].hostname, 'drop.test');
        });

        it('cancel a reverse query reports getHostByAddr', async () => {
            const r = resolver();

            const before = server.state.queries.length;
            const promise = callArgs((cb) => r.reverse('123.45.67.89', cb));
            for (let i = 0; i < 100 && server.state.queries.length === before; i++)
                await new Promise((resolve) => setTimeout(resolve, 10));
            r.cancel();

            const args = await promise;
            assert.equal(args[0].code, 'ECANCELLED');
            assert.equal(args[0].syscall, 'getHostByAddr');
            assert.equal(args[0].hostname, '123.45.67.89');
        });
    });

    describe('resolver state', () => {
        it('get/setServers round-trip', () => {
            const r = resolver();
            assert.deepEqual(r.getServers(), ['127.0.0.1:' + port]);
        });

        it('IPv6 server formatting and lenient ports', () => {
            const r = new dns.Resolver();
            r.setServers(['::1', '[2001:db8::1]:5353', '[::1]:53']);
            // c-ares/Node drop duplicate servers and omit the default port,
            // so '[::1]:53' is the same server as '::1'.
            assert.deepEqual(r.getServers(),
                ['::1', '[2001:db8::1]:5353']);

            // Node ignores a trailing non-port suffix, and c-ares truncates
            // the port to 16 bits (65536 becomes 0, i.e. the default port).
            r.setServers(['[::1]:65536', '[::1]:65537']);
            assert.deepEqual(r.getServers(), ['::1', '[::1]:1']);
            r.setServers(['[::1]:53:5353']);
            assert.deepEqual(r.getServers(), ['[::1]:5353']);
            r.setServers(['[::1]:65535']);
            assert.deepEqual(r.getServers(), ['[::1]:65535']);
        });

        it('module setServers only affects resolve', async () => {
            dns.setServers(['127.0.0.1:' + port]);
            assert.deepEqual(dns.getServers(), ['127.0.0.1:' + port]);
            assert.deepEqual(await dnsPromises.resolve4('a.test'),
                ['127.0.0.1']);
            assert.deepEqual(dns.lookup('127.0.0.1'),
                { address: '127.0.0.1', family: 4 });
            dns.setServers(originalServers);
        });

        it('module setServers replaces the default while a query is pending',
            async () => {
                const previous = dns.getServers();
                dns.setServers(['127.0.0.1:' + port]);
                try {
                    const promise = dnsPromises.resolve4('a.test');
                    // Node swaps a fresh default Resolver in, so pending
                    // queries on the old channel must not block this call.
                    assert.doesNotThrow(() =>
                        dns.setServers(['127.0.0.1:1']));
                    assert.deepEqual(dns.getServers(), ['127.0.0.1:1']);
                    assert.deepEqual(await promise, ['127.0.0.1']);
                } finally {
                    dns.setServers(previous);
                }
            });

        it('setServers rejects invalid input', () => {
            const r = resolver();
            const before = r.getServers();
            assert.throws(() => r.setServers(['not-an-ip']),
                { code: 'ERR_INVALID_IP_ADDRESS' });
            assert.deepEqual(r.getServers(), before);

            assert.throws(() => r.setServers([null]),
                { code: 'ERR_INVALID_ARG_TYPE' });
            assert.throws(() => r.setServers([1]),
                { code: 'ERR_INVALID_ARG_TYPE' });
            assert.throws(() => r.setServers('127.0.0.1'),
                { code: 'ERR_INVALID_ARG_TYPE' });
        });

        it('setServers fails while a query is pending', async () => {
            const r = new dns.Resolver({ timeout: 5000, tries: 1 });
            r.setServers(['127.0.0.1:' + port]);

            const before = server.state.queries.length;
            const promise = callArgs((cb) => r.resolve4('drop.test', cb));
            for (let i = 0; i < 100 && server.state.queries.length === before; i++)
                await new Promise((resolve) => setTimeout(resolve, 10));

            const old = r.getServers();
            let setErr;
            try {
                r.setServers(['127.0.0.1:1']);
            } catch (e) {
                setErr = e;
            }
            assert.equal(setErr.code, 'ERR_DNS_SET_SERVERS_FAILED');
            assert.match(setErr.message,
                /^c-ares failed to set servers: "There are pending queries\."/);
            assert.deepEqual(r.getServers(), old);

            r.cancel();
            const args = await promise;
            assert.equal(args[0].code, 'ECANCELLED');

            // Once the query is gone, updating the servers is allowed again.
            r.setServers(['127.0.0.1:' + port]);
            assert.deepEqual(r.getServers(), ['127.0.0.1:' + port]);
        });

        it('setServers accepts an empty list', () => {
            const r = resolver();
            r.setServers([]);
            assert.deepEqual(r.getServers(), []);
            r.setServers(['127.0.0.1:' + port]);
            assert.deepEqual(r.getServers(), ['127.0.0.1:' + port]);
        });

        it('default result order', () => {
            assert.equal(dns.getDefaultResultOrder(), 'verbatim');
            dns.setDefaultResultOrder('ipv4first');
            assert.equal(dns.getDefaultResultOrder(), 'ipv4first');
            dns.setDefaultResultOrder('ipv6first');
            assert.equal(dns.getDefaultResultOrder(), 'ipv6first');
            assert.throws(() => dns.setDefaultResultOrder('bad'),
                { code: 'ERR_INVALID_ARG_VALUE' });
            dns.setDefaultResultOrder('verbatim');
        });

        it('resolver options', () => {
            const r = new dns.Resolver({ timeout: 1000, tries: 2,
                maxTimeout: 5000 });
            assert.equal(typeof r, 'object');
            assert.throws(() => new dns.Resolver({ tries: 0 }),
                { code: 'ERR_OUT_OF_RANGE' });
            assert.throws(() => new dns.Resolver({ timeout: 1.5 }),
                { code: 'ERR_OUT_OF_RANGE' });
            assert.throws(() => new dns.Resolver({ tries: 1.5 }),
                { code: 'ERR_OUT_OF_RANGE' });
            assert.throws(() => new dns.Resolver({ maxTimeout: 1.5 }),
                { code: 'ERR_OUT_OF_RANGE' });
        });

        it('setLocalAddress still resolves', async () => {
            const r = resolver();
            r.setLocalAddress('127.0.0.1');
            assert.deepEqual(await r.resolve4('a.test'), ['127.0.0.1']);
        });

        it('setLocalAddress validates the address families', () => {
            const r = new dns.Resolver();
            r.setLocalAddress('127.0.0.1');
            r.setLocalAddress('::1');
            r.setLocalAddress('127.0.0.1', '::1');
            assert.throws(() => r.setLocalAddress('not-an-ip'),
                { code: 'ERR_INVALID_ARG_VALUE' });
            assert.throws(() => r.setLocalAddress('127.0.0.1', '127.0.0.2'),
                { code: 'ERR_INVALID_ARG_VALUE' });
            assert.throws(() => r.setLocalAddress('::1', '::2'),
                { code: 'ERR_INVALID_ARG_VALUE' });
        });

        it('cancel and setLocalAddress exist', () => {
            const r = new dns.Resolver();
            r.setLocalAddress('127.0.0.1');
            r.cancel();
        });        it('default order and servers are isolate scoped', async () => {
            const { Worker } = require('worker_threads');
            const previousOrder = dns.getDefaultResultOrder();
            const previousServers = dns.getServers();

            dns.setDefaultResultOrder('ipv4first');
            dns.setServers(['127.0.0.1:' + port]);
            try {
                const result = await new Promise((resolve, reject) => {
                    const worker = new Worker(`
                        const dns = require('dns');
                        const { parentPort } = require('worker_threads');
                        const beforeOrder = dns.getDefaultResultOrder();
                        const beforeServers = dns.getServers();
                        dns.setDefaultResultOrder('ipv6first');
                        dns.setServers(['127.0.0.1:1']);
                        parentPort.postMessage({
                            beforeOrder,
                            beforeServers,
                            afterOrder: dns.getDefaultResultOrder(),
                            afterServers: dns.getServers()
                        });
                    `, { eval: true });
                    worker.once('message', resolve);
                    worker.once('error', reject);
                });

                assert.equal(result.beforeOrder, 'verbatim');
                assert.equal(result.afterOrder, 'ipv6first');
                assert.deepEqual(result.afterServers, ['127.0.0.1:1']);
                assert.notDeepEqual(result.beforeServers,
                    ['127.0.0.1:' + port]);

                assert.equal(dns.getDefaultResultOrder(), 'ipv4first');
                assert.deepEqual(dns.getServers(), ['127.0.0.1:' + port]);
            } finally {
                dns.setDefaultResultOrder(previousOrder);
                dns.setServers(previousServers);
            }
        });
    });

    describe('module surface', () => {
        it('constants', () => {
            const codes = {
                NODATA: 'ENODATA',
                FORMERR: 'EFORMERR',
                SERVFAIL: 'ESERVFAIL',
                NOTFOUND: 'ENOTFOUND',
                NOTIMP: 'ENOTIMP',
                REFUSED: 'EREFUSED',
                BADQUERY: 'EBADQUERY',
                BADNAME: 'EBADNAME',
                BADFAMILY: 'EBADFAMILY',
                BADRESP: 'EBADRESP',
                CONNREFUSED: 'ECONNREFUSED',
                TIMEOUT: 'ETIMEOUT',
                EOF: 'EOF',
                FILE: 'EFILE',
                NOMEM: 'ENOMEM',
                DESTRUCTION: 'EDESTRUCTION',
                BADSTR: 'EBADSTR',
                BADFLAGS: 'EBADFLAGS',
                NONAME: 'ENONAME',
                BADHINTS: 'EBADHINTS',
                NOTINITIALIZED: 'ENOTINITIALIZED',
                LOADIPHLPAPI: 'ELOADIPHLPAPI',
                ADDRGETNETWORKPARAMS: 'EADDRGETNETWORKPARAMS',
                CANCELLED: 'ECANCELLED'
            };
            for (const key of Object.keys(codes))
                assert.equal(dns[key], codes[key], key);

            assert.equal(typeof dns.ADDRCONFIG, 'number');
            assert.equal(typeof dns.ALL, 'number');
            assert.equal(typeof dns.V4MAPPED, 'number');
        });

        it('exports the complete resolver surface', () => {
            for (const name of ['lookup', 'lookupService', 'resolve',
                'resolve4', 'resolve6', 'resolveAny', 'resolveCaa',
                'resolveCname', 'resolveMx', 'resolveNaptr', 'resolveNs',
                'resolvePtr', 'resolveSoa', 'resolveSrv', 'resolveTlsa',
                'resolveTxt', 'reverse', 'getServers', 'setServers',
                'getDefaultResultOrder', 'setDefaultResultOrder'])
                assert.equal(typeof dns[name], 'function', name);

            for (const name of ['lookup', 'lookupService', 'resolve',
                'resolve4', 'resolve6', 'resolveAny', 'resolveTlsa',
                'reverse', 'getServers', 'setServers'])
                assert.equal(typeof dnsPromises[name], 'function', name);
        });

        it('promises identity and Resolver', () => {
            assert.equal(dns.promises, dnsPromises);
            assert.equal(typeof dns.Resolver, 'function');
            assert.equal(typeof dns.promises.Resolver, 'function');
        });

        it('promise resolve methods', async () => {
            const r = resolver();
            assert.deepEqual(await r.resolve4('a.test'), ['127.0.0.1']);
            assert.equal(r.resolve4.length >= 0, true);
        });

        it('dns/promises Resolver owns a channel and servers', async () => {
            const r = new dnsPromises.Resolver({ timeout: 1000, tries: 1 });
            r.setServers(['127.0.0.1:' + port]);
            assert.deepEqual(r.getServers(), ['127.0.0.1:' + port]);
            assert.deepEqual(await r.resolve4('a.test'), ['127.0.0.1']);
        });
    });
});
