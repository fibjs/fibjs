var test = require("test");
test.setup();

var dns = require("dns");
var resolver, result;

describe('dns', () => {

    it('new Resolver', () => {
        resolver = new dns.Resolver();
        assert.equal(typeof resolver.resolve, 'function');
    });

    it('resolve', () => {
        result = dns.resolve('fibjs.org');
        assert.ok(result.length > 0);

        result = resolver.resolve('fibjs.org');
        assert.ok(result.length > 0);

        dns.resolve('fibjs.org', function(err, result){
            assert.notOk(err);
            assert.ok(result.length > 0);
        });
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

        dns.resolveSoa('dns.he.net', function(err, result){
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

        dns.resolveSoa('fibjs', function(err, result){
            assert.ok(err);
            assert.notOk(result);
        });
    });

    it('resolveMx', () => {
        result = dns.resolveMx('qq.com');
        assert.ok(result.length > 0);
        assert.ok(typeof result[0].exchange === 'string');
        assert.ok(typeof result[0].priority === 'number');

        dns.resolveMx('qq.com', function(err, result){
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

        dns.resolveMx('fibjs', function(err, result){
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

        dns.resolveSrv('_jabber._tcp.google.com', function(err, result){
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

        dns.resolveSrv('fibjs', function(err, result){
            assert.ok(err);
            assert.notOk(result);
        });
    });

    it('resolveTxt', () => {
        result = dns.resolveTxt('google.com');
        assert.ok(result.length > 0);
        assert.ok(result[0].length > 0);
        assert.ok(result[0][0].indexOf('all') !== -1);

        dns.resolveTxt('google.com', function(err, result){
            assert.notOk(err);
            assert.ok(result.length > 0);
            assert.ok(result[0].length > 0);
            assert.ok(result[0][0].indexOf('all') !== -1);
        });

        result = dns.resolve('google.com', 'TXT');
        assert.ok(result.length > 0);
        assert.ok(result[0].length > 0);
        assert.ok(result[0][0].indexOf('all') !== -1);

        result = resolver.resolveTxt('google.com');
        assert.ok(result.length > 0);
        assert.ok(result[0].length > 0);
        assert.ok(result[0][0].indexOf('all') !== -1);

        assert.throws(() => {
            dns.resolveTxt('fibjs');
        });

        dns.resolveTxt('fibjs', function(err, result){
            assert.ok(err);
            assert.notOk(result);
        });
    });

    it('resolveCname', () => {

        result = dns.resolveCname('www.microsoft.com');
        assert.ok(result.length > 0);
        assert.ok(result[0].indexOf('microsoft') !== -1);

        dns.resolveCname('www.microsoft.com', function(err, result){
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

        dns.resolveCname('fibjs', function(err, result){
            assert.ok(err);
            assert.notOk(result);
        });
    });

    it('resolve4', () => {
        result = dns.resolve4('qq.com');
        assert.ok(result.length > 0);

        dns.resolve4('qq.com', function(err, result){
            assert.notOk(err);
            assert.ok(result.length > 0);
        });

        result = dns.resolve('qq.com', 'A');
        assert.ok(result.length > 0);

        result = resolver.resolve4('qq.com');
        assert.ok(result.length > 0);

        result = dns.resolve4('qq.com', {ttl: true});
        assert.ok(result.length > 0);
        assert.ok(typeof result[0].address === 'string');
        assert.ok(typeof result[0].ttl === 'number');

        dns.resolve4('qq.com', {ttl: true}, function(err, result){
            assert.notOk(err);
            assert.ok(result.length > 0);
            assert.ok(typeof result[0].address === 'string');
            assert.ok(typeof result[0].ttl === 'number');
        });

        result = resolver.resolve4('qq.com', {ttl: true});
        assert.ok(result.length > 0);
        assert.ok(typeof result[0].address === 'string');
        assert.ok(typeof result[0].ttl === 'number');

        assert.throws(() => {
            dns.resolve4('fibjs');
        });

        dns.resolve4('fibjs', function(err, result){
            assert.ok(err);
            assert.notOk(result);
        });
    });

    it('resolve6', () => {
        result = dns.resolve6('tv6.ustc.edu.cn');
        assert.ok(result.length > 0);

        dns.resolve6('tv6.ustc.edu.cn', function(err, result){
            assert.notOk(err);
            assert.ok(result.length > 0);
        });

        result = dns.resolve('tv6.ustc.edu.cn', 'AAAA');
        assert.ok(result.length > 0);

        result = resolver.resolve6('tv6.ustc.edu.cn');
        assert.ok(result.length > 0);

        result = dns.resolve6('tv6.ustc.edu.cn', {ttl: true});
        assert.ok(result.length > 0);
        assert.ok(typeof result[0].address === 'string');
        assert.ok(typeof result[0].ttl === 'number');

        dns.resolve6('tv6.ustc.edu.cn', {ttl: true}, function(err, result){
            assert.notOk(err);
            assert.ok(result.length > 0);
            assert.ok(typeof result[0].address === 'string');
            assert.ok(typeof result[0].ttl === 'number');
        });

        result = resolver.resolve6('tv6.ustc.edu.cn', {ttl: true});
        assert.ok(result.length > 0);
        assert.ok(typeof result[0].address === 'string');
        assert.ok(typeof result[0].ttl === 'number');

        assert.throws(() => {
            dns.resolve6('fibjs');
        });

        dns.resolve6('fibjs', function(err, result){
            assert.ok(err);
            assert.notOk(result);
        });
    });

    it('resolveNs', () => {
        result = dns.resolveNs('rackspace.com');
        assert.ok(result.length > 0);
        assert.ok(result[0].indexOf('rackspace.com') !== -1);

        dns.resolveNs('rackspace.com', function(err, result){
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

        dns.resolveNs('fibjs', function(err, result){
            assert.ok(err);
            assert.notOk(result);
        });
    });

    it('resolvePtr', () => {
        result = dns.resolvePtr('8.8.8.8.in-addr.arpa');
        assert.ok(result.length > 0);
        assert.ok(result[0].indexOf('google') !== -1);

        dns.resolvePtr('8.8.8.8.in-addr.arpa', function(err, result){
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

        dns.resolvePtr('fibjs', function(err, result){
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

        dns.resolveNaptr('sip2sip.info', function(err, result){
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

        dns.resolveNaptr('fibjs', function(err, result){
            assert.ok(err);
            assert.notOk(result);
        });
    });
});

repl && test.run(console.DEBUG);
