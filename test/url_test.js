var test = require("test");
test.setup();

const isWindows = process.platform === 'win32';

var url = require('url');

describe("url", () => {
    it("domain to ascii", () => {
        const domainWithASCII = [
            ['ıíd', 'xn--d-iga7r'],
            ['يٴ', 'xn--mhb8f'],
            ['www.ϧƽəʐ.com', 'www.xn--cja62apfr6c.com'],
            ['новини.com', 'xn--b1amarcd.com'],
            ['名がドメイン.com', 'xn--v8jxj3d1dzdz08w.com'],
            ['افغانستا.icom.museum', 'xn--mgbaal8b0b9b2b.icom.museum'],
            ['الجزائر.icom.fake', 'xn--lgbbat1ad8j.icom.fake'],
            ['भारत.org', 'xn--h2brj9c.org'],
        ];

        domainWithASCII.forEach((pair) => {
            const domain = pair[0];
            const ascii = pair[1];
            const domainConvertedToASCII = url.domainToASCII(domain);
            assert.strictEqual(domainConvertedToASCII, ascii);
            const asciiConvertedToUnicode = url.domainToUnicode(ascii);
            assert.strictEqual(asciiConvertedToUnicode, domain);
        });
    });

    describe('file url', () => {
        it("file url to path", () => {
            function testInvalidArgs(...args) {
                for (const arg of args) {
                    assert.throws(() => url.fileURLToPath(arg), {
                        code: 'ERR_INVALID_ARG_TYPE'
                    });
                }
            }

            // Input must be string or URL
            testInvalidArgs(null, undefined, 1, {}, true);

            // Input must be a file URL
            assert.throws(() => url.fileURLToPath('https://a/b/c'), {
                code: 'ERR_INVALID_URL_SCHEME'
            });

            {
                const withHost = new URL('file://host/a');

                if (isWindows) {
                    assert.strictEqual(url.fileURLToPath(withHost), '\\\\host\\a');
                } else {
                    assert.throws(() => url.fileURLToPath(withHost), {
                        code: 'ERR_INVALID_FILE_URL_HOST'
                    });
                }
            }

            {
                if (isWindows) {
                    assert.throws(() => url.fileURLToPath('file:///C:/a%2F/'), {
                        code: 'ERR_INVALID_FILE_URL_PATH'
                    });
                    assert.throws(() => url.fileURLToPath('file:///C:/a%5C/'), {
                        code: 'ERR_INVALID_FILE_URL_PATH'
                    });
                    assert.throws(() => url.fileURLToPath('file:///?:/'), {
                        code: 'ERR_INVALID_FILE_URL_PATH'
                    });
                } else {
                    assert.throws(() => url.fileURLToPath('file:///a%2F/'), {
                        code: 'ERR_INVALID_FILE_URL_PATH'
                    });
                }
            }

            const windowsTestCases = [
                // Lowercase ascii alpha
                { path: 'C:\\foo', fileURL: 'file:///C:/foo' },
                // Uppercase ascii alpha
                { path: 'C:\\FOO', fileURL: 'file:///C:/FOO' },
                // dir
                { path: 'C:\\dir\\foo', fileURL: 'file:///C:/dir/foo' },
                // trailing separator
                { path: 'C:\\dir\\', fileURL: 'file:///C:/dir/' },
                // dot
                { path: 'C:\\foo.mjs', fileURL: 'file:///C:/foo.mjs' },
                // space
                { path: 'C:\\foo bar', fileURL: 'file:///C:/foo%20bar' },
                // question mark
                { path: 'C:\\foo?bar', fileURL: 'file:///C:/foo%3Fbar' },
                // number sign
                { path: 'C:\\foo#bar', fileURL: 'file:///C:/foo%23bar' },
                // ampersand
                { path: 'C:\\foo&bar', fileURL: 'file:///C:/foo&bar' },
                // equals
                { path: 'C:\\foo=bar', fileURL: 'file:///C:/foo=bar' },
                // colon
                { path: 'C:\\foo:bar', fileURL: 'file:///C:/foo:bar' },
                // semicolon
                { path: 'C:\\foo;bar', fileURL: 'file:///C:/foo;bar' },
                // percent
                { path: 'C:\\foo%bar', fileURL: 'file:///C:/foo%25bar' },
                // backslash
                { path: 'C:\\foo\\bar', fileURL: 'file:///C:/foo/bar' },
                // backspace
                { path: 'C:\\foo\bbar', fileURL: 'file:///C:/foo%08bar' },
                // tab
                { path: 'C:\\foo\tbar', fileURL: 'file:///C:/foo%09bar' },
                // newline
                { path: 'C:\\foo\nbar', fileURL: 'file:///C:/foo%0Abar' },
                // carriage return
                { path: 'C:\\foo\rbar', fileURL: 'file:///C:/foo%0Dbar' },
                // latin1
                { path: 'C:\\fóóbàr', fileURL: 'file:///C:/f%C3%B3%C3%B3b%C3%A0r' },
                // Euro sign (BMP code point)
                { path: 'C:\\€', fileURL: 'file:///C:/%E2%82%AC' },
                // Rocket emoji (non-BMP code point)
                { path: 'C:\\🚀', fileURL: 'file:///C:/%F0%9F%9A%80' },
                // UNC path (see https://docs.microsoft.com/en-us/archive/blogs/ie/file-uris-in-windows)
                { path: '\\\\nas\\My Docs\\File.doc', fileURL: 'file://nas/My%20Docs/File.doc' },
            ];
            const posixTestCases = [
                // Lowercase ascii alpha
                { path: '/foo', fileURL: 'file:///foo' },
                // Uppercase ascii alpha
                { path: '/FOO', fileURL: 'file:///FOO' },
                // dir
                { path: '/dir/foo', fileURL: 'file:///dir/foo' },
                // trailing separator
                { path: '/dir/', fileURL: 'file:///dir/' },
                // dot
                { path: '/foo.mjs', fileURL: 'file:///foo.mjs' },
                // space
                { path: '/foo bar', fileURL: 'file:///foo%20bar' },
                // question mark
                { path: '/foo?bar', fileURL: 'file:///foo%3Fbar' },
                // number sign
                { path: '/foo#bar', fileURL: 'file:///foo%23bar' },
                // ampersand
                { path: '/foo&bar', fileURL: 'file:///foo&bar' },
                // equals
                { path: '/foo=bar', fileURL: 'file:///foo=bar' },
                // colon
                { path: '/foo:bar', fileURL: 'file:///foo:bar' },
                // semicolon
                { path: '/foo;bar', fileURL: 'file:///foo;bar' },
                // percent
                { path: '/foo%bar', fileURL: 'file:///foo%25bar' },
                // backslash
                { path: '/foo\\bar', fileURL: 'file:///foo%5Cbar' },
                // backspace
                { path: '/foo\bbar', fileURL: 'file:///foo%08bar' },
                // tab
                { path: '/foo\tbar', fileURL: 'file:///foo%09bar' },
                // newline
                { path: '/foo\nbar', fileURL: 'file:///foo%0Abar' },
                // carriage return
                { path: '/foo\rbar', fileURL: 'file:///foo%0Dbar' },
                // latin1
                { path: '/fóóbàr', fileURL: 'file:///f%C3%B3%C3%B3b%C3%A0r' },
                // Euro sign (BMP code point)
                { path: '/€', fileURL: 'file:///%E2%82%AC' },
                // Rocket emoji (non-BMP code point)
                { path: '/🚀', fileURL: 'file:///%F0%9F%9A%80' },
            ];

            for (const { path, fileURL } of windowsTestCases) {
                const fromString = url.fileURLToPath(fileURL, { windows: true });
                assert.strictEqual(fromString, path);
                const fromURL = url.fileURLToPath(new URL(fileURL), { windows: true });
                assert.strictEqual(fromURL, path);
            }

            for (const { path, fileURL } of posixTestCases) {
                const fromString = url.fileURLToPath(fileURL, { windows: false });
                assert.strictEqual(fromString, path);
                const fromURL = url.fileURLToPath(new URL(fileURL), { windows: false });
                assert.strictEqual(fromURL, path);
            }

            const defaultTestCases = isWindows ? windowsTestCases : posixTestCases;

            // Test when `options` is null
            const whenNullActual = url.fileURLToPath(new URL(defaultTestCases[0].fileURL), null);
            assert.strictEqual(whenNullActual, defaultTestCases[0].path);

            for (const { path, fileURL } of defaultTestCases) {
                const fromString = url.fileURLToPath(fileURL);
                assert.strictEqual(fromString, path);
                const fromURL = url.fileURLToPath(new URL(fileURL));
                assert.strictEqual(fromURL, path);
            }
        });

        it("file url short format", () => {
            const shortFormatTests = [
                // Short format lowercase
                { fileURL: 'file://d/foo', path: 'd:\\foo' },
                // Short format uppercase
                { fileURL: 'file://D/FOO', path: 'd:\\FOO' },
                // Short format with subdirectories
                { fileURL: 'file://e/dir/foo', path: 'e:\\dir\\foo' },
                // Short format with special characters
                { fileURL: 'file://f/foo bar/baz', path: 'f:\\foo bar\\baz' },
                // Short format with query and hash
                { fileURL: 'file://g/test/file?query#hash', path: 'g:\\test\\file' }
            ];

            for (const { fileURL, path } of shortFormatTests) {
                const actual = url.fileURLToPath(fileURL, { windows: true });
                assert.strictEqual(actual, path);
            }
        });

        it("path to file url", () => {

            {
                const fileURL = url.pathToFileURL('test/').href;
                assert.ok(fileURL.startsWith('file:///'));
                assert.ok(fileURL.endsWith('/'));
            }

            {
                const fileURL = url.pathToFileURL('test\\').href;
                assert.ok(fileURL.startsWith('file:///'));
                if (isWindows)
                    assert.ok(fileURL.endsWith('/'));
                else
                    assert.ok(fileURL.endsWith('%5C'));
            }

            {
                const fileURL = url.pathToFileURL('test/%').href;
                assert.ok(fileURL.includes('%25'));
            }

            {
                if (isWindows) {
                    // UNC path: \\server\share\resource

                    // Missing server:
                    assert.throws(() => url.pathToFileURL('\\\\\\no-server'), {
                        code: 'ERR_INVALID_ARG_VALUE',
                    });

                    // Missing share or resource:
                    assert.throws(() => url.pathToFileURL('\\\\host'), {
                        code: 'ERR_INVALID_ARG_VALUE',
                    });
                } else {
                    // UNC paths on posix are considered a single path that has backslashes:
                    const fileURL = url.pathToFileURL('\\\\nas\\share\\path.txt').href;
                    assert.isTrue(/file:\/\/.+%5C%5Cnas%5Cshare%5Cpath\.txt$/.test(fileURL));
                }
            }

            const windowsTestCases = [
                // Lowercase ascii alpha
                { path: 'C:\\foo', expected: 'file:///C:/foo' },
                // Uppercase ascii alpha
                { path: 'C:\\FOO', expected: 'file:///C:/FOO' },
                // dir
                { path: 'C:\\dir\\foo', expected: 'file:///C:/dir/foo' },
                // trailing separator
                { path: 'C:\\dir\\', expected: 'file:///C:/dir/' },
                // dot
                { path: 'C:\\foo.mjs', expected: 'file:///C:/foo.mjs' },
                // space
                { path: 'C:\\foo bar', expected: 'file:///C:/foo%20bar' },
                // question mark
                { path: 'C:\\foo?bar', expected: 'file:///C:/foo%3Fbar' },
                // number sign
                { path: 'C:\\foo#bar', expected: 'file:///C:/foo%23bar' },
                // ampersand
                { path: 'C:\\foo&bar', expected: 'file:///C:/foo&bar' },
                // equals
                { path: 'C:\\foo=bar', expected: 'file:///C:/foo=bar' },
                // colon
                { path: 'C:\\foo:bar', expected: 'file:///C:/foo:bar' },
                // semicolon
                { path: 'C:\\foo;bar', expected: 'file:///C:/foo;bar' },
                // percent
                { path: 'C:\\foo%bar', expected: 'file:///C:/foo%25bar' },
                // backslash
                { path: 'C:\\foo\\bar', expected: 'file:///C:/foo/bar' },
                // backspace
                { path: 'C:\\foo\bbar', expected: 'file:///C:/foo%08bar' },
                // tab
                { path: 'C:\\foo\tbar', expected: 'file:///C:/foo%09bar' },
                // newline
                { path: 'C:\\foo\nbar', expected: 'file:///C:/foo%0Abar' },
                // carriage return
                { path: 'C:\\foo\rbar', expected: 'file:///C:/foo%0Dbar' },
                // latin1
                { path: 'C:\\fóóbàr', expected: 'file:///C:/f%C3%B3%C3%B3b%C3%A0r' },
                // Euro sign (BMP code point)
                { path: 'C:\\€', expected: 'file:///C:/%E2%82%AC' },
                // Rocket emoji (non-BMP code point)
                { path: 'C:\\🚀', expected: 'file:///C:/%F0%9F%9A%80' },
                // UNC path (see https://docs.microsoft.com/en-us/archive/blogs/ie/file-uris-in-windows)
                { path: '\\\\nas\\My Docs\\File.doc', expected: 'file://nas/My%20Docs/File.doc' },
            ];
            const posixTestCases = [
                // Lowercase ascii alpha
                { path: '/foo', expected: 'file:///foo' },
                // Uppercase ascii alpha
                { path: '/FOO', expected: 'file:///FOO' },
                // dir
                { path: '/dir/foo', expected: 'file:///dir/foo' },
                // trailing separator
                { path: '/dir/', expected: 'file:///dir/' },
                // dot
                { path: '/foo.mjs', expected: 'file:///foo.mjs' },
                // space
                { path: '/foo bar', expected: 'file:///foo%20bar' },
                // question mark
                { path: '/foo?bar', expected: 'file:///foo%3Fbar' },
                // number sign
                { path: '/foo#bar', expected: 'file:///foo%23bar' },
                // ampersand
                { path: '/foo&bar', expected: 'file:///foo&bar' },
                // equals
                { path: '/foo=bar', expected: 'file:///foo=bar' },
                // colon
                { path: '/foo:bar', expected: 'file:///foo:bar' },
                // semicolon
                { path: '/foo;bar', expected: 'file:///foo;bar' },
                // percent
                { path: '/foo%bar', expected: 'file:///foo%25bar' },
                // backslash
                { path: '/foo\\bar', expected: 'file:///foo%5Cbar' },
                // backspace
                { path: '/foo\bbar', expected: 'file:///foo%08bar' },
                // tab
                { path: '/foo\tbar', expected: 'file:///foo%09bar' },
                // newline
                { path: '/foo\nbar', expected: 'file:///foo%0Abar' },
                // carriage return
                { path: '/foo\rbar', expected: 'file:///foo%0Dbar' },
                // latin1
                { path: '/fóóbàr', expected: 'file:///f%C3%B3%C3%B3b%C3%A0r' },
                // Euro sign (BMP code point)
                { path: '/€', expected: 'file:///%E2%82%AC' },
                // Rocket emoji (non-BMP code point)
                { path: '/🚀', expected: 'file:///%F0%9F%9A%80' },
            ];

            for (const { path, expected } of windowsTestCases) {
                const actual = url.pathToFileURL(path, { windows: true }).href;
                assert.strictEqual(actual, expected);
            }

            for (const { path, expected } of posixTestCases) {
                const actual = url.pathToFileURL(path, { windows: false }).href;
                assert.strictEqual(actual, expected);
            }

            const testCases = isWindows ? windowsTestCases : posixTestCases;

            // Test when `options` is null
            const whenNullActual = url.pathToFileURL(testCases[0].path, null);
            assert.strictEqual(whenNullActual.href, testCases[0].expected);

            for (const { path, expected } of testCases) {
                const actual = url.pathToFileURL(path).href;
                assert.strictEqual(actual, expected);
            }
        });

    });

    describe('format', () => {
        it('legacy format', () => {
            // Formatting tests to verify that it'll format slightly wonky content to a
            // valid URL.
            const formatTests = {
                'http://example.com?': {
                    href: 'http://example.com/?',
                    protocol: 'http:',
                    slashes: true,
                    host: 'example.com',
                    hostname: 'example.com',
                    search: '?',
                    query: {},
                    pathname: '/'
                },
                'http://example.com?foo=bar#frag': {
                    href: 'http://example.com/?foo=bar#frag',
                    protocol: 'http:',
                    host: 'example.com',
                    hostname: 'example.com',
                    hash: '#frag',
                    search: '?foo=bar',
                    query: 'foo=bar',
                    pathname: '/'
                },
                'http://example.com?foo=@bar#frag': {
                    href: 'http://example.com/?foo=@bar#frag',
                    protocol: 'http:',
                    host: 'example.com',
                    hostname: 'example.com',
                    hash: '#frag',
                    search: '?foo=@bar',
                    query: 'foo=@bar',
                    pathname: '/'
                },
                'http://example.com?foo=/bar/#frag': {
                    href: 'http://example.com/?foo=/bar/#frag',
                    protocol: 'http:',
                    host: 'example.com',
                    hostname: 'example.com',
                    hash: '#frag',
                    search: '?foo=/bar/',
                    query: 'foo=/bar/',
                    pathname: '/'
                },
                'http://example.com?foo=?bar/#frag': {
                    href: 'http://example.com/?foo=?bar/#frag',
                    protocol: 'http:',
                    host: 'example.com',
                    hostname: 'example.com',
                    hash: '#frag',
                    search: '?foo=?bar/',
                    query: 'foo=?bar/',
                    pathname: '/'
                },
                'http://example.com#frag=?bar/#frag': {
                    href: 'http://example.com/#frag=?bar/#frag',
                    protocol: 'http:',
                    host: 'example.com',
                    hostname: 'example.com',
                    hash: '#frag=?bar/#frag',
                    pathname: '/'
                },
                'http://a.com/a/b/c?s#h': {
                    href: 'http://a.com/a/b/c?s#h',
                    protocol: 'http',
                    host: 'a.com',
                    pathname: 'a/b/c',
                    hash: 'h',
                    search: 's'
                },
                'xmpp:isaacschlueter@jabber.org': {
                    href: 'xmpp:isaacschlueter@jabber.org',
                    protocol: 'xmpp:',
                    host: 'jabber.org',
                    auth: 'isaacschlueter',
                    hostname: 'jabber.org'
                },
                'http://atpass:foo%40bar@127.0.0.1/': {
                    href: 'http://atpass:foo%40bar@127.0.0.1/',
                    auth: 'atpass:foo@bar',
                    hostname: '127.0.0.1',
                    protocol: 'http:',
                    pathname: '/'
                },
                'http://atslash%2F%40:%2F%40@foo/': {
                    href: 'http://atslash%2F%40:%2F%40@foo/',
                    auth: 'atslash/@:/@',
                    hostname: 'foo',
                    protocol: 'http:',
                    pathname: '/'
                },
                'svn+ssh://foo/bar': {
                    href: 'svn+ssh://foo/bar',
                    hostname: 'foo',
                    protocol: 'svn+ssh:',
                    pathname: '/bar',
                    slashes: true
                },
                'dash-test://foo/bar': {
                    href: 'dash-test://foo/bar',
                    hostname: 'foo',
                    protocol: 'dash-test:',
                    pathname: '/bar',
                    slashes: true
                },
                'dash-test:foo/bar': {
                    href: 'dash-test:foo/bar',
                    hostname: 'foo',
                    protocol: 'dash-test:',
                    pathname: '/bar'
                },
                'dot.test://foo/bar': {
                    href: 'dot.test://foo/bar',
                    hostname: 'foo',
                    protocol: 'dot.test:',
                    pathname: '/bar',
                    slashes: true
                },
                'dot.test:foo/bar': {
                    href: 'dot.test:foo/bar',
                    hostname: 'foo',
                    protocol: 'dot.test:',
                    pathname: '/bar'
                },
                // IPv6 support
                'coap:u:p@[::1]:61616/.well-known/r?n=Temperature': {
                    href: 'coap:u:p@[::1]:61616/.well-known/r?n=Temperature',
                    protocol: 'coap:',
                    auth: 'u:p',
                    hostname: '::1',
                    port: '61616',
                    pathname: '/.well-known/r',
                    search: 'n=Temperature'
                },
                'coap:[fedc:ba98:7654:3210:fedc:ba98:7654:3210]:61616/s/stopButton': {
                    href: 'coap:[fedc:ba98:7654:3210:fedc:ba98:7654:3210]:61616/s/stopButton',
                    protocol: 'coap',
                    host: '[fedc:ba98:7654:3210:fedc:ba98:7654:3210]:61616',
                    pathname: '/s/stopButton'
                },
                'http://[::]/': {
                    href: 'http://[::]/',
                    protocol: 'http:',
                    hostname: '[::]',
                    pathname: '/'
                },

                // Encode context-specific delimiters in path and query, but do not touch
                // other non-delimiter chars like `%`.
                // <https://github.com/nodejs/node-v0.x-archive/issues/4082>

                // `#`,`?` in path
                '/path/to/%%23%3F+=&.txt?foo=theA1#bar': {
                    href: '/path/to/%%23%3F+=&.txt?foo=theA1#bar',
                    pathname: '/path/to/%#?+=&.txt',
                    query: {
                        foo: 'theA1'
                    },
                    hash: '#bar'
                },

                // `#`,`?` in path + `#` in query
                '/path/to/%%23%3F+=&.txt?foo=the%231#bar': {
                    href: '/path/to/%%23%3F+=&.txt?foo=the%231#bar',
                    pathname: '/path/to/%#?+=&.txt',
                    query: {
                        foo: 'the#1'
                    },
                    hash: '#bar'
                },

                // `#` in path end + `#` in query
                '/path/to/%%23?foo=the%231#bar': {
                    href: '/path/to/%%23?foo=the%231#bar',
                    pathname: '/path/to/%#',
                    query: {
                        foo: 'the#1'
                    },
                    hash: '#bar'
                },

                // `?` and `#` in path and search
                'http://ex.com/foo%3F100%m%23r?abc=the%231?&foo=bar#frag': {
                    href: 'http://ex.com/foo%3F100%m%23r?abc=the%231?&foo=bar#frag',
                    protocol: 'http:',
                    hostname: 'ex.com',
                    hash: '#frag',
                    search: '?abc=the#1?&foo=bar',
                    pathname: '/foo?100%m#r',
                },

                // `?` and `#` in search only
                'http://ex.com/fooA100%mBr?abc=the%231?&foo=bar#frag': {
                    href: 'http://ex.com/fooA100%mBr?abc=the%231?&foo=bar#frag',
                    protocol: 'http:',
                    hostname: 'ex.com',
                    hash: '#frag',
                    search: '?abc=the#1?&foo=bar',
                    pathname: '/fooA100%mBr',
                },

                // Multiple `#` in search
                'http://example.com/?foo=bar%231%232%233&abc=%234%23%235#frag': {
                    href: 'http://example.com/?foo=bar%231%232%233&abc=%234%23%235#frag',
                    protocol: 'http:',
                    slashes: true,
                    host: 'example.com',
                    hostname: 'example.com',
                    hash: '#frag',
                    search: '?foo=bar#1#2#3&abc=#4##5',
                    query: {},
                    pathname: '/'
                },

                // Greater than or equal to 63 characters after `.` in hostname
                [`http://www.${'z'.repeat(63)}example.com/node`]: {
                    href: `http://www.${'z'.repeat(63)}example.com/node`,
                    protocol: 'http:',
                    slashes: true,
                    host: `www.${'z'.repeat(63)}example.com`,
                    hostname: `www.${'z'.repeat(63)}example.com`,
                    pathname: '/node',
                    path: '/node'
                },

                // https://github.com/nodejs/node/issues/3361
                'file:///home/user': {
                    href: 'file:///home/user',
                    protocol: 'file',
                    pathname: '/home/user',
                    path: '/home/user'
                },

                // surrogate in auth
                'http://%F0%9F%98%80@www.example.com/': {
                    href: 'http://%F0%9F%98%80@www.example.com/',
                    protocol: 'http:',
                    auth: '\uD83D\uDE00',
                    hostname: 'www.example.com',
                    pathname: '/'
                },

                'http://xn--v8jxj3d1dzdz08w.com/test': {
                    href: 'http://xn--v8jxj3d1dzdz08w.com/test',
                    protocol: 'http:',
                    slashes: true,
                    hostname: '名がドメイン.com',
                    pathname: '/test'
                }
            };

            for (const u in formatTests) {
                const expect = formatTests[u].href;
                delete formatTests[u].href;
                const actual = url.format(u);
                const actualObj = url.format(formatTests[u]);
                assert.strictEqual(actual, expect);
                assert.strictEqual(actualObj, expect);
            }
        });

        todo('whatwg format', () => {
            const myURL = new URL('http://user:pass@xn--lck1c3crb1723bpq4a.com/a?a=b#c');

            assert.strictEqual(url.format(myURL), 'http://user:pass@xn--lck1c3crb1723bpq4a.com/a?a=b#c');
            assert.strictEqual(url.format(myURL, {}), 'http://user:pass@xn--lck1c3crb1723bpq4a.com/a?a=b#c');

            {
                [true, 1, 'test', Infinity].forEach((value) => {
                    assert.throws(
                        () => url.format(myURL, value),
                        {
                            code: 'ERR_INVALID_ARG_TYPE',
                            name: 'TypeError',
                            message: 'The "options" argument must be of type object.' +
                                common.invalidArgTypeHelper(value)
                        }
                    );
                });
            }

            const opt_tests = [
                [{ auth: false }, 'http://xn--lck1c3crb1723bpq4a.com/a?a=b#c'],
                [{ auth: '' }, 'http://xn--lck1c3crb1723bpq4a.com/a?a=b#c'],
                [{ auth: 0 }, 'http://xn--lck1c3crb1723bpq4a.com/a?a=b#c'],
                [{ auth: 1 }, 'http://user:pass@xn--lck1c3crb1723bpq4a.com/a?a=b#c'],
                [{ auth: {} }, 'http://user:pass@xn--lck1c3crb1723bpq4a.com/a?a=b#c'],
                [{ fragment: false }, 'http://user:pass@xn--lck1c3crb1723bpq4a.com/a?a=b'],
                [{ fragment: '' }, 'http://user:pass@xn--lck1c3crb1723bpq4a.com/a?a=b'],
                [{ fragment: 0 }, 'http://user:pass@xn--lck1c3crb1723bpq4a.com/a?a=b'],
                [{ fragment: 1 }, 'http://user:pass@xn--lck1c3crb1723bpq4a.com/a?a=b#c'],
                [{ fragment: {} }, 'http://user:pass@xn--lck1c3crb1723bpq4a.com/a?a=b#c'],
                [{ search: false }, 'http://user:pass@xn--lck1c3crb1723bpq4a.com/a#c'],
                [{ search: '' }, 'http://user:pass@xn--lck1c3crb1723bpq4a.com/a#c'],
                [{ search: 0 }, 'http://user:pass@xn--lck1c3crb1723bpq4a.com/a#c'],
                [{ search: 1 }, 'http://user:pass@xn--lck1c3crb1723bpq4a.com/a?a=b#c'],
                [{ search: {} }, 'http://user:pass@xn--lck1c3crb1723bpq4a.com/a?a=b#c'],
                [{ unicode: true }, 'http://user:pass@理容ナカムラ.com/a?a=b#c'],
                [{ unicode: 1 }, 'http://user:pass@理容ナカムラ.com/a?a=b#c'],
                [{ unicode: {} }, 'http://user:pass@理容ナカムラ.com/a?a=b#c'],
                [{ unicode: false }, 'http://user:pass@xn--lck1c3crb1723bpq4a.com/a?a=b#c'],
                [{ unicode: 0 }, 'http://user:pass@xn--lck1c3crb1723bpq4a.com/a?a=b#c']
            ];

            for (const [opt, expect] of opt_tests)
                assert.strictEqual(url.format(myURL, opt), expect);

            assert.strictEqual(url.format(new URL('http://user:pass@xn--0zwm56d.com:8080/path'), { unicode: true }), 'http://user:pass@测试.com:8080/path');
            assert.strictEqual(url.format(new URL('tel:123')), url.format(new URL('tel:123'), { unicode: true }));
        });
    });

    it('parse', () => {
        // URLs to parse, and expected data
        // { url : parsed }
        const parseTests = {
            "http:\\\\evil-phisher\\foo.html#h\\a\\s\\h": {
                "href": "http://evil-phisher/foo.html#h\\a\\s\\h",
                "origin": "http://evil-phisher",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "evil-phisher",
                "hostname": "evil-phisher",
                "port": "",
                "pathname": "/foo.html",
                "search": "",
                "hash": "#h\\a\\s\\h"
            },
            "http:\\\\evil-phisher\\foo.html?json=\"\\\"foo\\\"\"#h\\a\\s\\h": {
                "href": "http://evil-phisher/foo.html?json=%22\\%22foo\\%22%22#h\\a\\s\\h",
                "origin": "http://evil-phisher",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "evil-phisher",
                "hostname": "evil-phisher",
                "port": "",
                "pathname": "/foo.html",
                "search": "?json=%22\\%22foo\\%22%22",
                "hash": "#h\\a\\s\\h"
            },
            "http:\\\\evil-phisher\\foo.html#h\\a\\s\\h?blarg": {
                "href": "http://evil-phisher/foo.html#h\\a\\s\\h?blarg",
                "origin": "http://evil-phisher",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "evil-phisher",
                "hostname": "evil-phisher",
                "port": "",
                "pathname": "/foo.html",
                "search": "",
                "hash": "#h\\a\\s\\h?blarg"
            },
            "http:\\\\evil-phisher\\foo.html": {
                "href": "http://evil-phisher/foo.html",
                "origin": "http://evil-phisher",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "evil-phisher",
                "hostname": "evil-phisher",
                "port": "",
                "pathname": "/foo.html",
                "search": "",
                "hash": ""
            },
            "HTTP://www.example.com/": {
                "href": "http://www.example.com/",
                "origin": "http://www.example.com",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "www.example.com",
                "hostname": "www.example.com",
                "port": "",
                "pathname": "/",
                "search": "",
                "hash": ""
            },
            "HTTP://www.example.com": {
                "href": "http://www.example.com/",
                "origin": "http://www.example.com",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "www.example.com",
                "hostname": "www.example.com",
                "port": "",
                "pathname": "/",
                "search": "",
                "hash": ""
            },
            "http://www.ExAmPlE.com/": {
                "href": "http://www.example.com/",
                "origin": "http://www.example.com",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "www.example.com",
                "hostname": "www.example.com",
                "port": "",
                "pathname": "/",
                "search": "",
                "hash": ""
            },
            "http://user:pw@www.ExAmPlE.com/": {
                "href": "http://user:pw@www.example.com/",
                "origin": "http://www.example.com",
                "protocol": "http:",
                "username": "user",
                "password": "pw",
                "host": "www.example.com",
                "hostname": "www.example.com",
                "port": "",
                "pathname": "/",
                "search": "",
                "hash": ""
            },
            "http://USER:PW@www.ExAmPlE.com/": {
                "href": "http://USER:PW@www.example.com/",
                "origin": "http://www.example.com",
                "protocol": "http:",
                "username": "USER",
                "password": "PW",
                "host": "www.example.com",
                "hostname": "www.example.com",
                "port": "",
                "pathname": "/",
                "search": "",
                "hash": ""
            },
            "http://user@www.example.com/": {
                "href": "http://user@www.example.com/",
                "origin": "http://www.example.com",
                "protocol": "http:",
                "username": "user",
                "password": "",
                "host": "www.example.com",
                "hostname": "www.example.com",
                "port": "",
                "pathname": "/",
                "search": "",
                "hash": ""
            },
            "http://user%3Apw@www.example.com/": {
                "href": "http://user%3Apw@www.example.com/",
                "origin": "http://www.example.com",
                "protocol": "http:",
                "username": "user%3Apw",
                "password": "",
                "host": "www.example.com",
                "hostname": "www.example.com",
                "port": "",
                "pathname": "/",
                "search": "",
                "hash": ""
            },
            "http://x.com/path?that's#all, folks": {
                "href": "http://x.com/path?that%27s#all,%20folks",
                "origin": "http://x.com",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "x.com",
                "hostname": "x.com",
                "port": "",
                "pathname": "/path",
                "search": "?that%27s",
                "hash": "#all,%20folks"
            },
            "HTTP://X.COM/Y": {
                "href": "http://x.com/Y",
                "origin": "http://x.com",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "x.com",
                "hostname": "x.com",
                "port": "",
                "pathname": "/Y",
                "search": "",
                "hash": ""
            },
            " http://www.example.com/": {
                "href": "http://www.example.com/",
                "origin": "http://www.example.com",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "www.example.com",
                "hostname": "www.example.com",
                "port": "",
                "pathname": "/",
                "search": "",
                "hash": ""
            },
            "http://x.y.com+a/b/c": {
                "href": "http://x.y.com+a/b/c",
                "origin": "http://x.y.com+a",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "x.y.com+a",
                "hostname": "x.y.com+a",
                "port": "",
                "pathname": "/b/c",
                "search": "",
                "hash": ""
            },
            "HtTp://x.y.cOm;a/b/c?d=e#f g<h>i": {
                "href": "http://x.y.com;a/b/c?d=e#f%20g%3Ch%3Ei",
                "origin": "http://x.y.com;a",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "x.y.com;a",
                "hostname": "x.y.com;a",
                "port": "",
                "pathname": "/b/c",
                "search": "?d=e",
                "hash": "#f%20g%3Ch%3Ei"
            },
            "HtTp://x.y.cOm;A/b/c?d=e#f g<h>i": {
                "href": "http://x.y.com;a/b/c?d=e#f%20g%3Ch%3Ei",
                "origin": "http://x.y.com;a",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "x.y.com;a",
                "hostname": "x.y.com;a",
                "port": "",
                "pathname": "/b/c",
                "search": "?d=e",
                "hash": "#f%20g%3Ch%3Ei"
            },
            "http://x...y...#p": {
                "href": "http://x...y.../#p",
                "origin": "http://x...y...",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "x...y...",
                "hostname": "x...y...",
                "port": "",
                "pathname": "/",
                "search": "",
                "hash": "#p"
            },
            "http://x/p/\"quoted\"": {
                "href": "http://x/p/%22quoted%22",
                "origin": "http://x",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "x",
                "hostname": "x",
                "port": "",
                "pathname": "/p/%22quoted%22",
                "search": "",
                "hash": ""
            },
            "http://www.narwhaljs.org/blog/categories?id=news": {
                "href": "http://www.narwhaljs.org/blog/categories?id=news",
                "origin": "http://www.narwhaljs.org",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "www.narwhaljs.org",
                "hostname": "www.narwhaljs.org",
                "port": "",
                "pathname": "/blog/categories",
                "search": "?id=news",
                "hash": ""
            },
            "http://mt0.google.com/vt/lyrs=m@114&hl=en&src=api&x=2&y=2&z=3&s=": {
                "href": "http://mt0.google.com/vt/lyrs=m@114&hl=en&src=api&x=2&y=2&z=3&s=",
                "origin": "http://mt0.google.com",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "mt0.google.com",
                "hostname": "mt0.google.com",
                "port": "",
                "pathname": "/vt/lyrs=m@114&hl=en&src=api&x=2&y=2&z=3&s=",
                "search": "",
                "hash": ""
            },
            "http://mt0.google.com/vt/lyrs=m@114???&hl=en&src=api&x=2&y=2&z=3&s=": {
                "href": "http://mt0.google.com/vt/lyrs=m@114???&hl=en&src=api&x=2&y=2&z=3&s=",
                "origin": "http://mt0.google.com",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "mt0.google.com",
                "hostname": "mt0.google.com",
                "port": "",
                "pathname": "/vt/lyrs=m@114",
                "search": "???&hl=en&src=api&x=2&y=2&z=3&s=",
                "hash": ""
            },
            "http://user:pass@mt0.google.com/vt/lyrs=m@114???&hl=en&src=api&x=2&y=2&z=3&s=": {
                "href": "http://user:pass@mt0.google.com/vt/lyrs=m@114???&hl=en&src=api&x=2&y=2&z=3&s=",
                "origin": "http://mt0.google.com",
                "protocol": "http:",
                "username": "user",
                "password": "pass",
                "host": "mt0.google.com",
                "hostname": "mt0.google.com",
                "port": "",
                "pathname": "/vt/lyrs=m@114",
                "search": "???&hl=en&src=api&x=2&y=2&z=3&s=",
                "hash": ""
            },
            "file:///etc/passwd": {
                "href": "file:///etc/passwd",
                "origin": "null",
                "protocol": "file:",
                "username": "",
                "password": "",
                "host": "",
                "hostname": "",
                "port": "",
                "pathname": "/etc/passwd",
                "search": "",
                "hash": ""
            },
            "file://localhost/etc/passwd": {
                "href": "file:///etc/passwd",
                "origin": "null",
                "protocol": "file:",
                "username": "",
                "password": "",
                "host": "",
                "hostname": "",
                "port": "",
                "pathname": "/etc/passwd",
                "search": "",
                "hash": ""
            },
            "file://foo/etc/passwd": {
                "href": "file://foo/etc/passwd",
                "origin": "null",
                "protocol": "file:",
                "username": "",
                "password": "",
                "host": "foo",
                "hostname": "foo",
                "port": "",
                "pathname": "/etc/passwd",
                "search": "",
                "hash": ""
            },
            "file:///etc/node/": {
                "href": "file:///etc/node/",
                "origin": "null",
                "protocol": "file:",
                "username": "",
                "password": "",
                "host": "",
                "hostname": "",
                "port": "",
                "pathname": "/etc/node/",
                "search": "",
                "hash": ""
            },
            "file://localhost/etc/node/": {
                "href": "file:///etc/node/",
                "origin": "null",
                "protocol": "file:",
                "username": "",
                "password": "",
                "host": "",
                "hostname": "",
                "port": "",
                "pathname": "/etc/node/",
                "search": "",
                "hash": ""
            },
            "file://foo/etc/node/": {
                "href": "file://foo/etc/node/",
                "origin": "null",
                "protocol": "file:",
                "username": "",
                "password": "",
                "host": "foo",
                "hostname": "foo",
                "port": "",
                "pathname": "/etc/node/",
                "search": "",
                "hash": ""
            },
            "http:/baz/../foo/bar": {
                "href": "http://baz/foo/bar",
                "origin": "http://baz",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "baz",
                "hostname": "baz",
                "port": "",
                "pathname": "/foo/bar",
                "search": "",
                "hash": ""
            },
            "http://user:pass@example.com:8000/foo/bar?baz=quux#frag": {
                "href": "http://user:pass@example.com:8000/foo/bar?baz=quux#frag",
                "origin": "http://example.com:8000",
                "protocol": "http:",
                "username": "user",
                "password": "pass",
                "host": "example.com:8000",
                "hostname": "example.com",
                "port": "8000",
                "pathname": "/foo/bar",
                "search": "?baz=quux",
                "hash": "#frag"
            },
            "http:/foo/bar?baz=quux#frag": {
                "href": "http://foo/bar?baz=quux#frag",
                "origin": "http://foo",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "foo",
                "hostname": "foo",
                "port": "",
                "pathname": "/bar",
                "search": "?baz=quux",
                "hash": "#frag"
            },
            "mailto:foo@bar.com?subject=hello": {
                "href": "mailto:foo@bar.com?subject=hello",
                "origin": "null",
                "protocol": "mailto:",
                "username": "",
                "password": "",
                "host": "",
                "hostname": "",
                "port": "",
                "pathname": "foo@bar.com",
                "search": "?subject=hello",
                "hash": ""
            },
            "javascript:alert('hello');": {
                "href": "javascript:alert('hello');",
                "origin": "null",
                "protocol": "javascript:",
                "username": "",
                "password": "",
                "host": "",
                "hostname": "",
                "port": "",
                "pathname": "alert('hello');",
                "search": "",
                "hash": ""
            },
            "xmpp:isaacschlueter@jabber.org": {
                "href": "xmpp:isaacschlueter@jabber.org",
                "origin": "null",
                "protocol": "xmpp:",
                "username": "",
                "password": "",
                "host": "",
                "hostname": "",
                "port": "",
                "pathname": "isaacschlueter@jabber.org",
                "search": "",
                "hash": ""
            },
            "http://atpass:foo%40bar@127.0.0.1:8080/path?search=foo#bar": {
                "href": "http://atpass:foo%40bar@127.0.0.1:8080/path?search=foo#bar",
                "origin": "http://127.0.0.1:8080",
                "protocol": "http:",
                "username": "atpass",
                "password": "foo%40bar",
                "host": "127.0.0.1:8080",
                "hostname": "127.0.0.1",
                "port": "8080",
                "pathname": "/path",
                "search": "?search=foo",
                "hash": "#bar"
            },
            "svn+ssh://foo/bar": {
                "href": "svn+ssh://foo/bar",
                "origin": "null",
                "protocol": "svn+ssh:",
                "username": "",
                "password": "",
                "host": "foo",
                "hostname": "foo",
                "port": "",
                "pathname": "/bar",
                "search": "",
                "hash": ""
            },
            "dash-test://foo/bar": {
                "href": "dash-test://foo/bar",
                "origin": "null",
                "protocol": "dash-test:",
                "username": "",
                "password": "",
                "host": "foo",
                "hostname": "foo",
                "port": "",
                "pathname": "/bar",
                "search": "",
                "hash": ""
            },
            "dash-test:foo/bar": {
                "href": "dash-test:foo/bar",
                "origin": "null",
                "protocol": "dash-test:",
                "username": "",
                "password": "",
                "host": "",
                "hostname": "",
                "port": "",
                "pathname": "foo/bar",
                "search": "",
                "hash": ""
            },
            "dot.test://foo/bar": {
                "href": "dot.test://foo/bar",
                "origin": "null",
                "protocol": "dot.test:",
                "username": "",
                "password": "",
                "host": "foo",
                "hostname": "foo",
                "port": "",
                "pathname": "/bar",
                "search": "",
                "hash": ""
            },
            "dot.test:foo/bar": {
                "href": "dot.test:foo/bar",
                "origin": "null",
                "protocol": "dot.test:",
                "username": "",
                "password": "",
                "host": "",
                "hostname": "",
                "port": "",
                "pathname": "foo/bar",
                "search": "",
                "hash": ""
            },
            "http://www.日本語.com/": {
                "href": "http://www.xn--wgv71a119e.com/",
                "origin": "http://www.xn--wgv71a119e.com",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "www.xn--wgv71a119e.com",
                "hostname": "www.xn--wgv71a119e.com",
                "port": "",
                "pathname": "/",
                "search": "",
                "hash": ""
            },
            "http://example.Bücher.com/": {
                "href": "http://example.xn--bcher-kva.com/",
                "origin": "http://example.xn--bcher-kva.com",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "example.xn--bcher-kva.com",
                "hostname": "example.xn--bcher-kva.com",
                "port": "",
                "pathname": "/",
                "search": "",
                "hash": ""
            },
            "http://www.Äffchen.com/": {
                "href": "http://www.xn--ffchen-9ta.com/",
                "origin": "http://www.xn--ffchen-9ta.com",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "www.xn--ffchen-9ta.com",
                "hostname": "www.xn--ffchen-9ta.com",
                "port": "",
                "pathname": "/",
                "search": "",
                "hash": ""
            },
            "http://www.Äffchen.cOm;A/b/c?d=e#f g<h>i": {
                "href": "http://www.xn--ffchen-9ta.com;a/b/c?d=e#f%20g%3Ch%3Ei",
                "origin": "http://www.xn--ffchen-9ta.com;a",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "www.xn--ffchen-9ta.com;a",
                "hostname": "www.xn--ffchen-9ta.com;a",
                "port": "",
                "pathname": "/b/c",
                "search": "?d=e",
                "hash": "#f%20g%3Ch%3Ei"
            },
            "http://SÉLIER.COM/": {
                "href": "http://xn--slier-bsa.com/",
                "origin": "http://xn--slier-bsa.com",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "xn--slier-bsa.com",
                "hostname": "xn--slier-bsa.com",
                "port": "",
                "pathname": "/",
                "search": "",
                "hash": ""
            },
            "http://ليهمابتكلموشعربي؟.ي؟/": {
                "href": "http://xn--egbpdaj6bu4bxfgehfvwxn.xn--egb9f/",
                "origin": "http://xn--egbpdaj6bu4bxfgehfvwxn.xn--egb9f",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "xn--egbpdaj6bu4bxfgehfvwxn.xn--egb9f",
                "hostname": "xn--egbpdaj6bu4bxfgehfvwxn.xn--egb9f",
                "port": "",
                "pathname": "/",
                "search": "",
                "hash": ""
            },
            "http://➡.ws/➡": {
                "href": "http://xn--hgi.ws/%E2%9E%A1",
                "origin": "http://xn--hgi.ws",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "xn--hgi.ws",
                "hostname": "xn--hgi.ws",
                "port": "",
                "pathname": "/%E2%9E%A1",
                "search": "",
                "hash": ""
            },
            "http://bucket_name.s3.amazonaws.com/image.jpg": {
                "href": "http://bucket_name.s3.amazonaws.com/image.jpg",
                "origin": "http://bucket_name.s3.amazonaws.com",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "bucket_name.s3.amazonaws.com",
                "hostname": "bucket_name.s3.amazonaws.com",
                "port": "",
                "pathname": "/image.jpg",
                "search": "",
                "hash": ""
            },
            "git+http://github.com/joyent/node.git": {
                "href": "git+http://github.com/joyent/node.git",
                "origin": "null",
                "protocol": "git+http:",
                "username": "",
                "password": "",
                "host": "github.com",
                "hostname": "github.com",
                "port": "",
                "pathname": "/joyent/node.git",
                "search": "",
                "hash": ""
            },
            "coap://[FEDC:BA98:7654:3210:FEDC:BA98:7654:3210]": {
                "href": "coap://[fedc:ba98:7654:3210:fedc:ba98:7654:3210]",
                "origin": "null",
                "protocol": "coap:",
                "username": "",
                "password": "",
                "host": "[fedc:ba98:7654:3210:fedc:ba98:7654:3210]",
                "hostname": "[fedc:ba98:7654:3210:fedc:ba98:7654:3210]",
                "port": "",
                "pathname": "",
                "search": "",
                "hash": ""
            },
            "coap://[1080:0:0:0:8:800:200C:417A]:61616/": {
                "href": "coap://[1080::8:800:200c:417a]:61616/",
                "origin": "null",
                "protocol": "coap:",
                "username": "",
                "password": "",
                "host": "[1080::8:800:200c:417a]:61616",
                "hostname": "[1080::8:800:200c:417a]",
                "port": "61616",
                "pathname": "/",
                "search": "",
                "hash": ""
            },
            "http://user:password@[3ffe:2a00:100:7031::1]:8080": {
                "href": "http://user:password@[3ffe:2a00:100:7031::1]:8080/",
                "origin": "http://[3ffe:2a00:100:7031::1]:8080",
                "protocol": "http:",
                "username": "user",
                "password": "password",
                "host": "[3ffe:2a00:100:7031::1]:8080",
                "hostname": "[3ffe:2a00:100:7031::1]",
                "port": "8080",
                "pathname": "/",
                "search": "",
                "hash": ""
            },
            "coap://u:p@[::192.9.5.5]:61616/.well-known/r?n=Temperature": {
                "href": "coap://u:p@[::c009:505]:61616/.well-known/r?n=Temperature",
                "origin": "null",
                "protocol": "coap:",
                "username": "u",
                "password": "p",
                "host": "[::c009:505]:61616",
                "hostname": "[::c009:505]",
                "port": "61616",
                "pathname": "/.well-known/r",
                "search": "?n=Temperature",
                "hash": ""
            },
            "http://example.com:": {
                "href": "http://example.com/",
                "origin": "http://example.com",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "example.com",
                "hostname": "example.com",
                "port": "",
                "pathname": "/",
                "search": "",
                "hash": ""
            },
            "http://example.com:/a/b.html": {
                "href": "http://example.com/a/b.html",
                "origin": "http://example.com",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "example.com",
                "hostname": "example.com",
                "port": "",
                "pathname": "/a/b.html",
                "search": "",
                "hash": ""
            },
            "http://example.com:?a=b": {
                "href": "http://example.com/?a=b",
                "origin": "http://example.com",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "example.com",
                "hostname": "example.com",
                "port": "",
                "pathname": "/",
                "search": "?a=b",
                "hash": ""
            },
            "http://example.com:#abc": {
                "href": "http://example.com/#abc",
                "origin": "http://example.com",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "example.com",
                "hostname": "example.com",
                "port": "",
                "pathname": "/",
                "search": "",
                "hash": "#abc"
            },
            "http://[fe80::1]:/a/b?a=b#abc": {
                "href": "http://[fe80::1]/a/b?a=b#abc",
                "origin": "http://[fe80::1]",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "[fe80::1]",
                "hostname": "[fe80::1]",
                "port": "",
                "pathname": "/a/b",
                "search": "?a=b",
                "hash": "#abc"
            },
            "http://-lovemonsterz.tumblr.com/rss": {
                "href": "http://-lovemonsterz.tumblr.com/rss",
                "origin": "http://-lovemonsterz.tumblr.com",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "-lovemonsterz.tumblr.com",
                "hostname": "-lovemonsterz.tumblr.com",
                "port": "",
                "pathname": "/rss",
                "search": "",
                "hash": ""
            },
            "http://-lovemonsterz.tumblr.com:80/rss": {
                "href": "http://-lovemonsterz.tumblr.com/rss",
                "origin": "http://-lovemonsterz.tumblr.com",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "-lovemonsterz.tumblr.com",
                "hostname": "-lovemonsterz.tumblr.com",
                "port": "",
                "pathname": "/rss",
                "search": "",
                "hash": ""
            },
            "http://user:pass@-lovemonsterz.tumblr.com/rss": {
                "href": "http://user:pass@-lovemonsterz.tumblr.com/rss",
                "origin": "http://-lovemonsterz.tumblr.com",
                "protocol": "http:",
                "username": "user",
                "password": "pass",
                "host": "-lovemonsterz.tumblr.com",
                "hostname": "-lovemonsterz.tumblr.com",
                "port": "",
                "pathname": "/rss",
                "search": "",
                "hash": ""
            },
            "http://user:pass@-lovemonsterz.tumblr.com:80/rss": {
                "href": "http://user:pass@-lovemonsterz.tumblr.com/rss",
                "origin": "http://-lovemonsterz.tumblr.com",
                "protocol": "http:",
                "username": "user",
                "password": "pass",
                "host": "-lovemonsterz.tumblr.com",
                "hostname": "-lovemonsterz.tumblr.com",
                "port": "",
                "pathname": "/rss",
                "search": "",
                "hash": ""
            },
            "http://_jabber._tcp.google.com/test": {
                "href": "http://_jabber._tcp.google.com/test",
                "origin": "http://_jabber._tcp.google.com",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "_jabber._tcp.google.com",
                "hostname": "_jabber._tcp.google.com",
                "port": "",
                "pathname": "/test",
                "search": "",
                "hash": ""
            },
            "http://user:pass@_jabber._tcp.google.com/test": {
                "href": "http://user:pass@_jabber._tcp.google.com/test",
                "origin": "http://_jabber._tcp.google.com",
                "protocol": "http:",
                "username": "user",
                "password": "pass",
                "host": "_jabber._tcp.google.com",
                "hostname": "_jabber._tcp.google.com",
                "port": "",
                "pathname": "/test",
                "search": "",
                "hash": ""
            },
            "http://_jabber._tcp.google.com:80/test": {
                "href": "http://_jabber._tcp.google.com/test",
                "origin": "http://_jabber._tcp.google.com",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "_jabber._tcp.google.com",
                "hostname": "_jabber._tcp.google.com",
                "port": "",
                "pathname": "/test",
                "search": "",
                "hash": ""
            },
            "http://user:pass@_jabber._tcp.google.com:80/test": {
                "href": "http://user:pass@_jabber._tcp.google.com/test",
                "origin": "http://_jabber._tcp.google.com",
                "protocol": "http:",
                "username": "user",
                "password": "pass",
                "host": "_jabber._tcp.google.com",
                "hostname": "_jabber._tcp.google.com",
                "port": "",
                "pathname": "/test",
                "search": "",
                "hash": ""
            },
            "http://x:1/' <>\"`/{}|\\^~`/": {
                "href": "http://x:1/'%20%3C%3E%22%60/%7B%7D|/^~%60/",
                "origin": "http://x:1",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "x:1",
                "hostname": "x",
                "port": "1",
                "pathname": "/'%20%3C%3E%22%60/%7B%7D|/^~%60/",
                "search": "",
                "hash": ""
            },
            "http://a@b@c/": {
                "href": "http://a%40b@c/",
                "origin": "http://c",
                "protocol": "http:",
                "username": "a%40b",
                "password": "",
                "host": "c",
                "hostname": "c",
                "port": "",
                "pathname": "/",
                "search": "",
                "hash": ""
            },
            "http://a@b?@c": {
                "href": "http://a@b/?@c",
                "origin": "http://b",
                "protocol": "http:",
                "username": "a",
                "password": "",
                "host": "b",
                "hostname": "b",
                "port": "",
                "pathname": "/",
                "search": "?@c",
                "hash": ""
            },
            "http://a.b/\tbc\ndr\ref g\"hq'j<kl>?mn\\op^q=r`99{st|uv}wz": {
                "href": "http://a.b/bcdref%20g%22hq'j%3Ckl%3E?mn\\op^q=r`99{st|uv}wz",
                "origin": "http://a.b",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "a.b",
                "hostname": "a.b",
                "port": "",
                "pathname": "/bcdref%20g%22hq'j%3Ckl%3E",
                "search": "?mn\\op^q=r`99{st|uv}wz",
                "hash": ""
            },
            "http://a\r\" \t\n<'b:b@c\r\nd/e?f": {
                "href": "http://a%22%20%3C'b:b@cd/e?f",
                "origin": "http://cd",
                "protocol": "http:",
                "username": "a%22%20%3C'b",
                "password": "b",
                "host": "cd",
                "hostname": "cd",
                "port": "",
                "pathname": "/e",
                "search": "?f",
                "hash": ""
            },
            "https://*": {
                "href": "https://*/",
                "origin": "https://*",
                "protocol": "https:",
                "username": "",
                "password": "",
                "host": "*",
                "hostname": "*",
                "port": "",
                "pathname": "/",
                "search": "",
                "hash": ""
            },
            "javascript:alert(1);a='@white-listed.com'": {
                "href": "javascript:alert(1);a='@white-listed.com'",
                "origin": "null",
                "protocol": "javascript:",
                "username": "",
                "password": "",
                "host": "",
                "hostname": "",
                "port": "",
                "pathname": "alert(1);a='@white-listed.com'",
                "search": "",
                "hash": ""
            },
            "javAscript:alert(1);a='@white-listed.com'": {
                "href": "javascript:alert(1);a='@white-listed.com'",
                "origin": "null",
                "protocol": "javascript:",
                "username": "",
                "password": "",
                "host": "",
                "hostname": "",
                "port": "",
                "pathname": "alert(1);a='@white-listed.com'",
                "search": "",
                "hash": ""
            },
            "ws://www.example.com": {
                "href": "ws://www.example.com/",
                "origin": "ws://www.example.com",
                "protocol": "ws:",
                "username": "",
                "password": "",
                "host": "www.example.com",
                "hostname": "www.example.com",
                "port": "",
                "pathname": "/",
                "search": "",
                "hash": ""
            },
            "wss://www.example.com": {
                "href": "wss://www.example.com/",
                "origin": "wss://www.example.com",
                "protocol": "wss:",
                "username": "",
                "password": "",
                "host": "www.example.com",
                "hostname": "www.example.com",
                "port": "",
                "pathname": "/",
                "search": "",
                "hash": ""
            },
            "\bhttp://example.com/\b": {
                "href": "http://example.com/",
                "origin": "http://example.com",
                "protocol": "http:",
                "username": "",
                "password": "",
                "host": "example.com",
                "hostname": "example.com",
                "port": "",
                "pathname": "/",
                "search": "",
                "hash": ""
            },
            "https://evil.com$.example.com": {
                "href": "https://evil.com$.example.com/",
                "origin": "https://evil.com$.example.com",
                "protocol": "https:",
                "username": "",
                "password": "",
                "host": "evil.com$.example.com",
                "hostname": "evil.com$.example.com",
                "port": "",
                "pathname": "/",
                "search": "",
                "hash": ""
            },
            "x://0.0,1.1/": {
                "href": "x://0.0,1.1/",
                "origin": "null",
                "protocol": "x:",
                "username": "",
                "password": "",
                "host": "0.0,1.1",
                "hostname": "0.0,1.1",
                "port": "",
                "pathname": "/",
                "search": "",
                "hash": ""
            }
        };

        for (const u in parseTests) {
            let actual = url.parse(u);
            const spaced = url.parse(`     \t  ${u}\n\t`);

            for (const key in parseTests[u]) {
                assert.strictEqual(actual[key], parseTests[u][key]);
                assert.strictEqual(spaced[key], parseTests[u][key]);
            }
        }
    });

    it("resolve", () => {
        const relativeTests = [
            ['/foo/bar/baz', 'quux', '/foo/bar/quux'],
            ['/foo/bar/baz', 'quux/asdf', '/foo/bar/quux/asdf'],
            ['/foo/bar/baz', 'quux/baz', '/foo/bar/quux/baz'],
            ['/foo/bar/baz', '../quux/baz', '/foo/quux/baz'],
            ['/foo/bar/baz', '/bar', '/bar'],
            ['/foo/bar/baz/', 'quux', '/foo/bar/baz/quux'],
            ['/foo/bar/baz/', 'quux/baz', '/foo/bar/baz/quux/baz'],
            ['/foo/bar/baz', '../../../../../../../../quux/baz', '/quux/baz'],
            ['/foo/bar/baz', '../../../../../../../quux/baz', '/quux/baz'],
            ['/foo', '.', '/'],
            ['/foo/', '.', '/foo/'],
            ['/foo/', '..', '/'],
            ['/foo/bar', '.', '/foo/'],
            ['/foo/bar', '..', '/'],
            ['/foo/bar/', '.', '/foo/bar/'],
            ['/foo/bar/', '..', '/foo/'],
            ['http://example.com/b//c//d;p?q#blarg',
                'https:/p/a/t/h?s#hash2',
                'https://p/a/t/h?s#hash2'],
            ['http://example.com/b//c//d;p?q#blarg',
                'https://u:p@h.com/p/a/t/h?s#hash2',
                'https://u:p@h.com/p/a/t/h?s#hash2'],
            ['http://example.com/b//c//d;p?q#blarg',
                'https:/a/b/c/d',
                'https://a/b/c/d'],
            ['http://example.com/b//c//d;p?q#blarg',
                'http:#hash2',
                'http://example.com/b//c//d;p?q#hash2'],
            ['http://example.com/b//c//d;p?q#blarg',
                'http:/p/a/t/h?s#hash2',
                'http://example.com/p/a/t/h?s#hash2'],
            ['http://example.com/b//c//d;p?q#blarg',
                'http://u:p@h.com/p/a/t/h?s#hash2',
                'http://u:p@h.com/p/a/t/h?s#hash2'],
            ['http://example.com/b//c//d;p?q#blarg',
                'http:/a/b/c/d',
                'http://example.com/a/b/c/d'],
            ['/foo/bar/baz', '/../etc/passwd', '/etc/passwd'],
            ['http://localhost', 'file:///Users/foo', 'file:///Users/foo'],
            ['http://localhost', 'file://foo/Users', 'file://foo/Users'],
            ['https://registry.npmjs.org', '@foo/bar', 'https://registry.npmjs.org/@foo/bar'],
        ];
        for (let i = 0; i < relativeTests.length; i++) {
            const relativeTest = relativeTests[i];

            const a = url.resolve(relativeTest[0], relativeTest[1]);
            const e = relativeTest[2];
            assert.strictEqual(a, e);
        }
    });

    it("property", () => {
        const url_ = new url.URL('http://user:pass@foo.bar.com:21/aaa/zzz?l=24#test');
        const oldParams = url_.searchParams;

        // `href` is writable (not readonly) and is stringifier
        assert.strictEqual(url_.toString(), url_.href);
        url_.href = 'http://user:pass@foo.bar.com:21/aaa/zzz?l=25#test';
        assert.strictEqual(url_.href,
            'http://user:pass@foo.bar.com:21/aaa/zzz?l=25#test');
        assert.strictEqual(url_.toString(), url_.href);
        // Return true because it's configurable, but because the properties
        // are defined on the prototype per the spec, the deletion has no effect
        assert.strictEqual((delete url_.href), true);
        assert.strictEqual(url_.href,
            'http://user:pass@foo.bar.com:21/aaa/zzz?l=25#test');
        // assert.strictEqual(url_.searchParams, oldParams);  // [SameObject]

        // searchParams is readonly. Under strict mode setting a
        // non-writable property should throw.
        // Note: this error message is subject to change in V8 updates
        assert.throws(
            () => url_.origin = 'http://foo.bar.com:22',
            /^TypeError: Cannot set property origin of \[object URL\] which has only a getter$/
        );
        assert.strictEqual(url_.origin, 'http://foo.bar.com:21');
        assert.strictEqual(url_.toString(),
            'http://user:pass@foo.bar.com:21/aaa/zzz?l=25#test');
        assert.strictEqual((delete url_.origin), true);
        assert.strictEqual(url_.origin, 'http://foo.bar.com:21');

        // The following properties should be writable (not readonly)
        url_.protocol = 'https:';
        assert.strictEqual(url_.protocol, 'https:');
        assert.strictEqual(url_.toString(),
            'https://user:pass@foo.bar.com:21/aaa/zzz?l=25#test');
        assert.strictEqual((delete url_.protocol), true);
        assert.strictEqual(url_.protocol, 'https:');

        url_.username = 'user2';
        assert.strictEqual(url_.username, 'user2');
        assert.strictEqual(url_.toString(),
            'https://user2:pass@foo.bar.com:21/aaa/zzz?l=25#test');
        assert.strictEqual((delete url_.username), true);
        assert.strictEqual(url_.username, 'user2');

        url_.password = 'pass2';
        assert.strictEqual(url_.password, 'pass2');
        assert.strictEqual(url_.toString(),
            'https://user2:pass2@foo.bar.com:21/aaa/zzz?l=25#test');
        assert.strictEqual((delete url_.password), true);
        assert.strictEqual(url_.password, 'pass2');

        url_.host = 'foo.bar.net:22';
        assert.strictEqual(url_.host, 'foo.bar.net:22');
        assert.strictEqual(url_.toString(),
            'https://user2:pass2@foo.bar.net:22/aaa/zzz?l=25#test');
        assert.strictEqual((delete url_.host), true);
        assert.strictEqual(url_.host, 'foo.bar.net:22');

        url_.hostname = 'foo.bar.org';
        assert.strictEqual(url_.hostname, 'foo.bar.org');
        assert.strictEqual(url_.toString(),
            'https://user2:pass2@foo.bar.org:22/aaa/zzz?l=25#test');
        assert.strictEqual((delete url_.hostname), true);
        assert.strictEqual(url_.hostname, 'foo.bar.org');

        url_.port = '23';
        assert.strictEqual(url_.port, '23');
        assert.strictEqual(url_.toString(),
            'https://user2:pass2@foo.bar.org:23/aaa/zzz?l=25#test');
        assert.strictEqual((delete url_.port), true);
        assert.strictEqual(url_.port, '23');

        url_.pathname = '/aaa/bbb';
        assert.strictEqual(url_.pathname, '/aaa/bbb');
        assert.strictEqual(url_.toString(),
            'https://user2:pass2@foo.bar.org:23/aaa/bbb?l=25#test');
        assert.strictEqual((delete url_.pathname), true);
        assert.strictEqual(url_.pathname, '/aaa/bbb');

        url_.search = '?k=99';
        assert.strictEqual(url_.search, '?k=99');
        assert.strictEqual(url_.toString(),
            'https://user2:pass2@foo.bar.org:23/aaa/bbb?k=99#test');
        assert.strictEqual((delete url_.search), true);
        assert.strictEqual(url_.search, '?k=99');

        url_.hash = '#abcd';
        assert.strictEqual(url_.hash, '#abcd');
        assert.strictEqual(url_.toString(),
            'https://user2:pass2@foo.bar.org:23/aaa/bbb?k=99#abcd');
        assert.strictEqual((delete url_.hash), true);
        assert.strictEqual(url_.hash, '#abcd');

        // searchParams is readonly. Under strict mode setting a
        // non-writable property should throw.
        // Note: this error message is subject to change in V8 updates
        assert.throws(
            () => url_.searchParams = '?k=88',
            /^TypeError: Cannot set property searchParams of \[object URL\] which has only a getter$/
        );
        // assert.strictEqual(url_.searchParams, oldParams);
        assert.strictEqual(url_.toString(),
            'https://user2:pass2@foo.bar.org:23/aaa/bbb?k=99#abcd');
        assert.strictEqual((delete url_.searchParams), true);
        // assert.strictEqual(url_.searchParams, oldParams);

        // Test special origins
        [
            {
                expected: 'https://whatwg.org',
                url: 'blob:https://whatwg.org/d0360e2f-caee-469f-9a2f-87d5b0456f6f'
            },
            { expected: 'ftp://example.org', url: 'ftp://example.org/foo' },
            { expected: 'http://example.org', url: 'http://example.org/foo' },
            { expected: 'https://example.org', url: 'https://example.org/foo' },
            { expected: 'ws://example.org', url: 'ws://example.org/foo' },
            { expected: 'wss://example.org', url: 'wss://example.org/foo' },
            { expected: 'null', url: 'gopher://gopher.quux.org/1/' },
            { expected: 'null', url: 'file:///tmp/mock/path' },
            { expected: 'null', url: 'npm://nodejs/rules' },
        ].forEach((test) => {
            assert.strictEqual(new url.URL(test.url).origin, test.expected);
        });
    });

    it('override hostname', () => {
        const url = new (class extends URL { get hostname() { return 'bar.com'; } })('http://foo.com/');
        assert.strictEqual(url.href, 'http://foo.com/');
        assert.strictEqual(url.toString(), 'http://foo.com/');
        // assert.strictEqual(url.toJSON(), 'http://foo.com/');
        assert.strictEqual(url.hash, '');
        assert.strictEqual(url.host, 'foo.com');
        assert.strictEqual(url.hostname, 'bar.com');
        assert.strictEqual(url.origin, 'http://foo.com');
        assert.strictEqual(url.password, '');
        assert.strictEqual(url.protocol, 'http:');
        assert.strictEqual(url.username, '');
        assert.strictEqual(url.search, '');
        // assert.strictEqual(url.searchParams.toString(), '');
    });
});

