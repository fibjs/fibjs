var { describe, it, before, after, beforeEach, afterEach } = require('node:test');
var assert = require('assert');

describe("class test", () => {
    it("toString", () => {
        o = Buffer.from('aaa');

        assert.equal(Object.prototype.toString.call(o), "[object Uint8Array]");

        assert.equal(Object.prototype.toString.call(process), "[object process]");
        assert.equal(Object.prototype.toString.call(console), "[object console]");
    });

    // Tests for the ClassInfo NamedPropertyHandler fix:
    // native C++ instance properties are now reported as own enumerable,
    // so Object.assign / spread / Object.keys can see them on any native object.
    describe("native object property enumerability", () => {
        it("Object.assign should copy URL instance properties", () => {
            const u = new URL('https://user:pass@example.com:8080/path/to?key=value#frag');
            const obj = Object.assign({}, u);
            assert.strictEqual(obj.href, u.href);
            assert.strictEqual(obj.protocol, 'https:');
            assert.strictEqual(obj.hostname, 'example.com');
            assert.strictEqual(obj.port, '8080');
            assert.strictEqual(obj.pathname, '/path/to');
            assert.strictEqual(obj.search, '?key=value');
            assert.strictEqual(obj.hash, '#frag');
        });

        it("spread operator should copy URL instance properties", () => {
            const u = new URL('https://example.com/path');
            const obj = { ...u };
            assert.strictEqual(obj.href, u.href);
            assert.strictEqual(obj.hostname, 'example.com');
            assert.strictEqual(obj.pathname, '/path');
        });

        it("Object.keys should include native instance property names", () => {
            // URL: check all expected WHATWG URL properties are enumerable
            const u = new URL('http://example.com/');
            const keys = Object.keys(u);
            const expected = ['href', 'origin', 'protocol', 'username', 'password',
                'host', 'hostname', 'port', 'pathname', 'search', 'searchParams', 'hash'];
            for (const key of expected)
                assert.ok(keys.includes(key), `Object.keys should include '${key}'`);
        });

        it("for...in should enumerate native instance properties", () => {
            const u = new URL('http://example.com/');
            const keys = [];
            for (const k in u) keys.push(k);
            assert.ok(keys.includes('href'), "for..in should include 'href'");
            assert.ok(keys.includes('hostname'), "for..in should include 'hostname'");
            assert.ok(keys.includes('pathname'), "for..in should include 'pathname'");
        });

        it("propertyIsEnumerable should return true for native instance properties", () => {
            const u = new URL('http://example.com/');
            assert.strictEqual(u.propertyIsEnumerable('href'), true);
            assert.strictEqual(u.propertyIsEnumerable('hostname'), true);
            assert.strictEqual(u.propertyIsEnumerable('pathname'), true);
            assert.strictEqual(u.propertyIsEnumerable('searchParams'), true);
        });

        it("Object.getOwnPropertyDescriptor should return a descriptor for native properties", () => {
            const u = new URL('http://example.com/');
            const desc = Object.getOwnPropertyDescriptor(u, 'href');
            assert.ok(desc !== undefined, 'descriptor for href should exist');
            assert.strictEqual(desc.enumerable, true, 'href should be enumerable');
        });

        it("Object.assign with URL should work as http.request options (node-fetch pattern)", () => {
            // Reproduces the node-fetch getNodeRequestOptions() pattern that was broken:
            //   Object.assign({}, new URL(request.url), { method, headers, agent })
            const u = new URL('http://api.example.com:3000/v1/users');
            const opts = Object.assign({}, u, { method: 'POST' });
            assert.strictEqual(opts.hostname, 'api.example.com');
            assert.strictEqual(opts.port, '3000');
            assert.strictEqual(opts.pathname, '/v1/users');
            assert.strictEqual(opts.protocol, 'http:');
            assert.strictEqual(opts.method, 'POST');
        });

        it("non-URL native objects should not be affected", () => {
            // process and console are native singletons; their own-property sets
            // are distinct from URL -- just verify they still behave correctly.
            assert.equal(Object.prototype.toString.call(process), "[object process]");
            assert.equal(Object.prototype.toString.call(console), "[object console]");
        });
    });

    describe("IsJSObject accepts null-prototype objects", () => {
        var url = require('url');

        it("Object.create(null) should be accepted where Object is expected", () => {
            // Regression: IsJSObject previously required proto === Object.prototype,
            // rejecting Object.create(null) and causing overload dispatch to fail.
            var opts = Object.create(null);
            opts.protocol = 'http:';
            opts.hostname = 'example.com';
            opts.pathname = '/path';
            // url.format(Object) — if IsJSObject rejects null-proto, this throws TYPEMISMATCH
            assert.strictEqual(url.format(opts), 'http://example.com/path');
        });

        it("null-proto object with all URL fields formats correctly", () => {
            var opts = Object.create(null);
            opts.protocol = 'https:';
            opts.hostname = 'api.example.com';
            opts.port = '8080';
            opts.pathname = '/v1/users';
            opts.search = '?limit=10';
            assert.strictEqual(url.format(opts), 'https://api.example.com:8080/v1/users?limit=10');
        });

        it("null-proto and plain-object produce identical url.format output", () => {
            var fields = { protocol: 'http:', hostname: 'example.com', pathname: '/test' };
            var nullProto = Object.assign(Object.create(null), fields);
            assert.strictEqual(url.format(nullProto), url.format(fields));
        });
    });
});

