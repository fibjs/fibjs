var test = require("test");
test.setup();

const isWindows = process.platform === 'win32';

var net = require('net');
var url = require('url');

describe("url", () => {
    var parseTests = {
        '//some_path': {
            'href': '//some_path',
            'slashes': false,
            'hostname': '',
            'pathname': '//some_path',
            'path': '///some_path'
        },

        'HTTP://www.example.com/': {
            'href': 'http://www.example.com/',
            'protocol': 'http:',
            'slashes': true,
            'host': 'www.example.com',
            'hostname': 'www.example.com',
            'pathname': '/',
            'path': '/'
        },

        'http://www.ExAmPlE.com/': {
            'href': 'http://www.example.com/',
            'protocol': 'http:',
            'slashes': true,
            'host': 'www.example.com',
            'hostname': 'www.example.com',
            'pathname': '/',
            'path': '/'
        },

        'http://user:pw@www.ExAmPlE.com/': {
            'href': 'http://user:pw@www.example.com/',
            'protocol': 'http:',
            'slashes': true,
            'username': 'user',
            'password': 'pw',
            'host': 'www.example.com',
            'hostname': 'www.example.com',
            'pathname': '/',
            'path': '/'
        },

        'http://USER:PW@www.ExAmPlE.com/': {
            'href': 'http://USER:PW@www.example.com/',
            'protocol': 'http:',
            'slashes': true,
            'username': 'USER',
            'password': 'PW',
            'host': 'www.example.com',
            'hostname': 'www.example.com',
            'pathname': '/',
            'path': '/'
        },

        'http://user@www.example.com/': {
            'href': 'http://user@www.example.com/',
            'protocol': 'http:',
            'slashes': true,
            'username': 'user',
            'host': 'www.example.com',
            'hostname': 'www.example.com',
            'pathname': '/',
            'path': '/'
        },

        'http://user:pw@www.example.com/': {
            'href': 'http://user:pw@www.example.com/',
            'protocol': 'http:',
            'slashes': true,
            'username': 'user',
            'password': 'pw',
            'host': 'www.example.com',
            'hostname': 'www.example.com',
            'pathname': '/',
            'path': '/'
        },

        'http://x.com/path?that\'s#all, folks': {
            'href': 'http://x.com/path?that%27s#all,%20folks',
            'protocol': 'http:',
            'slashes': true,
            'host': 'x.com',
            'hostname': 'x.com',
            'search': '?that%27s',
            'query': 'that%27s',
            'pathname': '/path',
            'hash': '#all,%20folks',
            'path': '/path?that%27s'
        },

        'HTTP://X.COM/Y': {
            'href': 'http://x.com/Y',
            'protocol': 'http:',
            'slashes': true,
            'host': 'x.com',
            'hostname': 'x.com',
            'pathname': '/Y',
            'path': '/Y'
        },

        // an unexpected invalid char in the hostname.
        'HtTp://x.y.cOm*a/b/c?d=e#f g<h>i': {
            'href': 'http://x.y.com/*a/b/c?d=e#f%20g%3Ch%3Ei',
            'protocol': 'http:',
            'slashes': true,
            'host': 'x.y.com',
            'hostname': 'x.y.com',
            'pathname': '/*a/b/c',
            'search': '?d=e',
            'query': 'd=e',
            'hash': '#f%20g%3Ch%3Ei',
            'path': '/*a/b/c?d=e'
        },

        // make sure that we don't accidentally lcast the
        // path parts.
        'HtTp://x.y.cOm*A/b/c?d=e#f g<h>i': {
            'href': 'http://x.y.com/*A/b/c?d=e#f%20g%3Ch%3Ei',
            'protocol': 'http:',
            'slashes': true,
            'host': 'x.y.com',
            'hostname': 'x.y.com',
            'pathname': '/*A/b/c',
            'search': '?d=e',
            'query': 'd=e',
            'hash': '#f%20g%3Ch%3Ei',
            'path': '/*A/b/c?d=e'
        },

        'http://x...y...#p': {
            'href': 'http://x...y.../#p',
            'protocol': 'http:',
            'slashes': true,
            'host': 'x...y...',
            'hostname': 'x...y...',
            'hash': '#p',
            'pathname': '/',
            'path': '/'
        },

        'http://x/p/"quoted"': {
            'href': 'http://x/p/%22quoted%22',
            'protocol': 'http:',
            'slashes': true,
            'host': 'x',
            'hostname': 'x',
            'pathname': '/p/%22quoted%22',
            'path': '/p/%22quoted%22'
        },

        '<http://goo.corn/bread> Is a URL!': {
            'href': '%3Chttp://goo.corn/bread%3E%20Is%20a%20URL!',
            'pathname': '%3Chttp://goo.corn/bread%3E%20Is%20a%20URL!',
            'path': '%3Chttp://goo.corn/bread%3E%20Is%20a%20URL!'
        },

        'http://www.narwhaljs.org/blog/categories?id=news': {
            'href': 'http://www.narwhaljs.org/blog/categories?id=news',
            'protocol': 'http:',
            'slashes': true,
            'host': 'www.narwhaljs.org',
            'hostname': 'www.narwhaljs.org',
            'search': '?id=news',
            'query': 'id=news',
            'pathname': '/blog/categories',
            'path': '/blog/categories?id=news'
        },

        'http://mt0.google.com/vt/lyrs=m@114&hl=en&src=api&x=2&y=2&z=3&s=': {
            'href': 'http://mt0.google.com/vt/lyrs=m@114&hl=en&src=api&x=2&y=2&z=3&s=',
            'protocol': 'http:',
            'slashes': true,
            'host': 'mt0.google.com',
            'hostname': 'mt0.google.com',
            'pathname': '/vt/lyrs=m@114&hl=en&src=api&x=2&y=2&z=3&s=',
            'path': '/vt/lyrs=m@114&hl=en&src=api&x=2&y=2&z=3&s='
        },

        'http://mt0.google.com/vt/lyrs=m@114???&hl=en&src=api&x=2&y=2&z=3&s=': {
            'href': 'http://mt0.google.com/vt/lyrs=m@114???&hl=en&src=api' + '&x=2&y=2&z=3&s=',
            'protocol': 'http:',
            'slashes': true,
            'host': 'mt0.google.com',
            'hostname': 'mt0.google.com',
            'search': '???&hl=en&src=api&x=2&y=2&z=3&s=',
            'query': '??&hl=en&src=api&x=2&y=2&z=3&s=',
            'pathname': '/vt/lyrs=m@114',
            'path': '/vt/lyrs=m@114???&hl=en&src=api&x=2&y=2&z=3&s='
        },

        'http://user:pass@mt0.google.com/vt/lyrs=m@114???&hl=en&src=api&x=2&y=2&z=3&s=': {
            'href': 'http://user:pass@mt0.google.com/vt/lyrs=m@114???' + '&hl=en&src=api&x=2&y=2&z=3&s=',
            'protocol': 'http:',
            'slashes': true,
            'host': 'mt0.google.com',
            'username': 'user',
            'password': 'pass',
            'hostname': 'mt0.google.com',
            'search': '???&hl=en&src=api&x=2&y=2&z=3&s=',
            'query': '??&hl=en&src=api&x=2&y=2&z=3&s=',
            'pathname': '/vt/lyrs=m@114',
            'path': '/vt/lyrs=m@114???&hl=en&src=api&x=2&y=2&z=3&s='
        },

        'file:///etc/passwd': {
            'href': 'file:///etc/passwd',
            'slashes': true,
            'protocol': 'file:',
            'pathname': '/etc/passwd',
            'hostname': '',
            'host': '',
            'path': '/etc/passwd'
        },

        'file://localhost/etc/passwd': {
            'href': 'file://localhost/etc/passwd',
            'protocol': 'file:',
            'slashes': true,
            'pathname': '/etc/passwd',
            'hostname': 'localhost',
            'host': 'localhost',
            'path': '/etc/passwd'
        },

        'file://foo/etc/passwd': {
            'href': 'file://foo/etc/passwd',
            'protocol': 'file:',
            'slashes': true,
            'pathname': '/etc/passwd',
            'hostname': 'foo',
            'host': 'foo',
            'path': '/etc/passwd'
        },

        'file:///etc/node/': {
            'href': 'file:///etc/node/',
            'slashes': true,
            'protocol': 'file:',
            'pathname': '/etc/node/',
            'hostname': '',
            'host': '',
            'path': '/etc/node/'
        },

        'file://localhost/etc/node/': {
            'href': 'file://localhost/etc/node/',
            'protocol': 'file:',
            'slashes': true,
            'pathname': '/etc/node/',
            'hostname': 'localhost',
            'host': 'localhost',
            'path': '/etc/node/'
        },

        'file://foo/etc/node/': {
            'href': 'file://foo/etc/node/',
            'protocol': 'file:',
            'slashes': true,
            'pathname': '/etc/node/',
            'hostname': 'foo',
            'host': 'foo',
            'path': '/etc/node/'
        },

        'http:/baz/../foo/bar': {
            'href': 'http:/baz/../foo/bar',
            'protocol': 'http:',
            'pathname': '/baz/../foo/bar',
            'path': '/baz/../foo/bar'
        },

        'http://user:pass@example.com:8000/foo/bar?baz=quux#frag': {
            'href': 'http://user:pass@example.com:8000/foo/bar?baz=quux#frag',
            'protocol': 'http:',
            'slashes': true,
            'host': 'example.com:8000',
            'username': 'user',
            'password': 'pass',
            'port': '8000',
            'hostname': 'example.com',
            'hash': '#frag',
            'search': '?baz=quux',
            'query': 'baz=quux',
            'pathname': '/foo/bar',
            'path': '/foo/bar?baz=quux'
        },

        '//user:pass@example.com:8000/foo/bar?baz=quux#frag': {
            'href': '//user:pass@example.com:8000/foo/bar?baz=quux#frag',
            'slashes': true,
            'host': 'example.com:8000',
            'username': 'user',
            'password': 'pass',
            'port': '8000',
            'hostname': 'example.com',
            'hash': '#frag',
            'search': '?baz=quux',
            'query': 'baz=quux',
            'pathname': '/foo/bar',
            'path': '/foo/bar?baz=quux'
        },

        '/foo/bar?baz=quux#frag': {
            'href': '/foo/bar?baz=quux#frag',
            'hash': '#frag',
            'search': '?baz=quux',
            'query': 'baz=quux',
            'pathname': '/foo/bar',
            'path': '/foo/bar?baz=quux'
        },

        'http:/foo/bar?baz=quux#frag': {
            'href': 'http:/foo/bar?baz=quux#frag',
            'protocol': 'http:',
            'hash': '#frag',
            'search': '?baz=quux',
            'query': 'baz=quux',
            'pathname': '/foo/bar',
            'path': '/foo/bar?baz=quux'
        },

        'mailto:foo@bar.com?subject=hello': {
            'href': 'mailto:foo@bar.com?subject=hello',
            'protocol': 'mailto:',
            'host': 'bar.com',
            'username': 'foo',
            'hostname': 'bar.com',
            'search': '?subject=hello',
            'query': 'subject=hello',
            'path': '?subject=hello'
        },

        'javascript:alert(\'hello\');': {
            'href': 'javascript:alert(\'hello\');',
            'protocol': 'javascript:',
            'pathname': 'alert(\'hello\');',
            'path': 'alert(\'hello\');'
        },

        'xmpp:isaacschlueter@jabber.org': {
            'href': 'xmpp:isaacschlueter@jabber.org',
            'protocol': 'xmpp:',
            'host': 'jabber.org',
            'username': 'isaacschlueter',
            'hostname': 'jabber.org'
        },

        'http://atpass:foo%40bar@127.0.0.1:8080/path?search=foo#bar': {
            'href': 'http://atpass:foo%40bar@127.0.0.1:8080/path?search=foo#bar',
            'protocol': 'http:',
            'slashes': true,
            'host': '127.0.0.1:8080',
            'username': 'atpass',
            'password': 'foo@bar',
            'hostname': '127.0.0.1',
            'port': '8080',
            'pathname': '/path',
            'search': '?search=foo',
            'query': 'search=foo',
            'hash': '#bar',
            'path': '/path?search=foo'
        },

        'svn+ssh://foo/bar': {
            'href': 'svn+ssh://foo/bar',
            'host': 'foo',
            'hostname': 'foo',
            'protocol': 'svn+ssh:',
            'pathname': '/bar',
            'path': '/bar',
            'slashes': true
        },

        'dash-test://foo/bar': {
            'href': 'dash-test://foo/bar',
            'host': 'foo',
            'hostname': 'foo',
            'protocol': 'dash-test:',
            'pathname': '/bar',
            'path': '/bar',
            'slashes': true
        },

        'dash-test:foo/bar': {
            'href': 'dash-test:foo/bar',
            'host': 'foo',
            'hostname': 'foo',
            'protocol': 'dash-test:',
            'pathname': '/bar',
            'path': '/bar'
        },

        'dot.test://foo/bar': {
            'href': 'dot.test://foo/bar',
            'host': 'foo',
            'hostname': 'foo',
            'protocol': 'dot.test:',
            'pathname': '/bar',
            'path': '/bar',
            'slashes': true
        },

        'dot.test:foo/bar': {
            'href': 'dot.test:foo/bar',
            'host': 'foo',
            'hostname': 'foo',
            'protocol': 'dot.test:',
            'pathname': '/bar',
            'path': '/bar'
        },

        'git+http://github.com/joyent/node.git': {
            protocol: 'git+http:',
            slashes: true,
            host: 'github.com',
            hostname: 'github.com',
            pathname: '/joyent/node.git',
            path: '/joyent/node.git',
            href: 'git+http://github.com/joyent/node.git'
        },

        // if local1@domain1 is uses as a relative URL it
        // may
        // be parse into auth@hostname, but here there is no
        // way to make it work in url.parse, I add the test
        // to be explicit
        'local1@domain1': {
            'pathname': 'local1@domain1',
            'path': 'local1@domain1',
            'href': 'local1@domain1'
        },

        // While this may seem counter-intuitive, a browser
        // will parse
        // <a href='www.google.com'> as a path.
        'www.example.com': {
            'href': 'www.example.com',
            'pathname': 'www.example.com',
            'path': 'www.example.com'
        },

        // ipv6 support
        '[fe80::1]': {
            'href': '[fe80::1]',
            'pathname': '[fe80::1]',
            'path': '[fe80::1]'
        },

        'coap://[FEDC:BA98:7654:3210:FEDC:BA98:7654:3210]/': {
            'protocol': 'coap:',
            'slashes': true,
            'host': '[fedc:ba98:7654:3210:fedc:ba98:7654:3210]',
            'hostname': 'fedc:ba98:7654:3210:fedc:ba98:7654:3210',
            'href': 'coap://[fedc:ba98:7654:3210:fedc:ba98:7654:3210]/',
            'pathname': '/',
            'path': '/'
        },

        'coap://[1080:0:0:0:8:800:200C:417A]:61616/': {
            'protocol': 'coap:',
            'slashes': true,
            'host': '[1080:0:0:0:8:800:200c:417a]:61616',
            'port': '61616',
            'hostname': '1080:0:0:0:8:800:200c:417a',
            'href': 'coap://[1080:0:0:0:8:800:200c:417a]:61616/',
            'pathname': '/',
            'path': '/'
        },

        'http://user:password@[3ffe:2a00:100:7031::1]:8080': {
            'protocol': 'http:',
            'slashes': true,
            'username': 'user',
            'password': 'password',
            'host': '[3ffe:2a00:100:7031::1]:8080',
            'port': '8080',
            'hostname': '3ffe:2a00:100:7031::1',
            'href': 'http://user:password@[3ffe:2a00:100:7031::1]:8080/',
            'pathname': '/',
            'path': '/'
        },

        'coap://u:p@[::192.9.5.5]:61616/.well-known/r?n=Temperature': {
            'protocol': 'coap:',
            'slashes': true,
            'username': 'u',
            'password': 'p',
            'host': '[::192.9.5.5]:61616',
            'port': '61616',
            'hostname': '::192.9.5.5',
            'href': 'coap://u:p@[::192.9.5.5]:61616/.well-known/r?n=Temperature',
            'search': '?n=Temperature',
            'query': 'n=Temperature',
            'pathname': '/.well-known/r',
            'path': '/.well-known/r?n=Temperature'
        },

        // empty port
        'http://example.com:': {
            'protocol': 'http:',
            'slashes': true,
            'host': 'example.com',
            'hostname': 'example.com',
            'href': 'http://example.com/',
            'pathname': '/',
            'path': '/'
        },

        'http://example.com:/a/b.html': {
            'protocol': 'http:',
            'slashes': true,
            'host': 'example.com',
            'hostname': 'example.com',
            'href': 'http://example.com/a/b.html',
            'pathname': '/a/b.html',
            'path': '/a/b.html'
        },

        'http://example.com:?a=b': {
            'protocol': 'http:',
            'slashes': true,
            'host': 'example.com',
            'hostname': 'example.com',
            'href': 'http://example.com/?a=b',
            'search': '?a=b',
            'query': 'a=b',
            'pathname': '/',
            'path': '/?a=b'
        },

        'http://example.com:#abc': {
            'protocol': 'http:',
            'slashes': true,
            'host': 'example.com',
            'hostname': 'example.com',
            'href': 'http://example.com/#abc',
            'hash': '#abc',
            'pathname': '/',
            'path': '/'
        },

        'http://[fe80::1]:/a/b?a=b#abc': {
            'protocol': 'http:',
            'slashes': true,
            'host': '[fe80::1]',
            'hostname': 'fe80::1',
            'href': 'http://[fe80::1]/a/b?a=b#abc',
            'search': '?a=b',
            'query': 'a=b',
            'hash': '#abc',
            'pathname': '/a/b',
            'path': '/a/b?a=b'
        },

        'http://example.com?': {
            'href': 'http://example.com/',
            'protocol': 'http:',
            'slashes': true,
            'host': 'example.com',
            'hostname': 'example.com',
            'search': '?',
            'query': {},
            'pathname': '/'
        },
        'http://example.com?foo=bar#frag': {
            'href': 'http://example.com/?foo=bar#frag',
            'protocol': 'http:',
            'host': 'example.com',
            'hostname': 'example.com',
            'hash': '#frag',
            'search': '?foo=bar',
            'query': 'foo=bar',
            'pathname': '/'
        },
        'http://example.com?foo=@bar#frag': {
            'href': 'http://example.com/?foo=@bar#frag',
            'protocol': 'http:',
            'host': 'example.com',
            'hostname': 'example.com',
            'hash': '#frag',
            'search': '?foo=@bar',
            'query': 'foo=@bar',
            'pathname': '/'
        },
        'http://example.com?foo=/bar/#frag': {
            'href': 'http://example.com/?foo=/bar/#frag',
            'protocol': 'http:',
            'host': 'example.com',
            'hostname': 'example.com',
            'hash': '#frag',
            'search': '?foo=/bar/',
            'query': 'foo=/bar/',
            'pathname': '/'
        },
        'http://example.com?foo=?bar/#frag': {
            'href': 'http://example.com/?foo=?bar/#frag',
            'protocol': 'http:',
            'host': 'example.com',
            'hostname': 'example.com',
            'hash': '#frag',
            'search': '?foo=?bar/',
            'query': 'foo=?bar/',
            'pathname': '/'
        },
        'http://example.com#frag=?bar/#frag': {
            'href': 'http://example.com/#frag=?bar/#frag',
            'protocol': 'http:',
            'host': 'example.com',
            'hostname': 'example.com',
            'hash': '#frag=?bar/#frag',
            'pathname': '/'
        },
        'http://google.com" onload="alert(42)/': {
            'href': 'http://google.com/%22%20onload=%22alert(42)/',
            'protocol': 'http:',
            'hostname': 'google.com',
            'pathname': '/%22%20onload=%22alert(42)/'
        },
        'http://a.com/a/b/c?s#h': {
            'href': 'http://a.com/a/b/c?s#h',
            'protocol': 'http',
            'hostname': 'a.com',
            'pathname': 'a/b/c',
            'hash': 'h',
            'query': 's'
        },
        'https://a.com:447/a/b/c?s#h': {
            'href': 'https://a.com:447/a/b/c?s#h',
            'protocol': 'https',
            'hostname': 'a.com',
            'pathname': 'a/b/c',
            'port': 447,
            'hash': 'h',
            'query': 's'
        },
        'http://atpass:foo%40bar@127.0.0.1/': {
            'href': 'http://atpass:foo%40bar@127.0.0.1/',
            'username': 'atpass',
            password: 'foo@bar',
            'hostname': '127.0.0.1',
            'protocol': 'http:',
            'pathname': '/'
        },
        'http://atslash%2F%40:%2F%40@foo/': {
            'href': 'http://atslash%2F%40:%2F%40@foo/',
            'username': 'atslash/@',
            password: '/@',
            'hostname': 'foo',
            'protocol': 'http:',
            'pathname': '/'
        },
        // ipv6 support
        'coap:u:p@[::1]:61616/.well-known/r?n=Temperature': {
            'href': 'coap:u:p@[::1]:61616/.well-known/r?n=Temperature',
            'protocol': 'coap:',
            'username': 'u',
            password: 'p',
            'hostname': '::1',
            'port': '61616',
            'pathname': '/.well-known/r',
            'query': 'n=Temperature'
        },
        'coap:[fedc:ba98:7654:3210:fedc:ba98:7654:3210]:61616/s/stopButton': {
            'href': 'coap:[fedc:ba98:7654:3210:fedc:ba98:7654:3210]:61616/s/stopButton',
            'protocol': 'coap',
            'hostname': 'fedc:ba98:7654:3210:fedc:ba98:7654:3210',
            port: '61616',
            'pathname': '/s/stopButton'
        },
        '\t\r\n\f\u00a0\ufeffhttp://test.com\\r?n=t': {
            'href': 'http://test.com/r?n=t',
            'protocol': 'http',
            'hostname': 'test.com',
            'pathname': '/r',
            'path': '/r?n=t',
            "query": "n=t"
        },
        'http://中文.com': {
            "pathname": "/",
            "path": "/",
            "hostname": "xn--fiq228c.com",
            "host": "xn--fiq228c.com",
            "protocol": "http:",
            "href": "http://xn--fiq228c.com/",
        },
        'http://%2fvar%2Frun%2Fdocker.sock/v1.41/images/json': {
            "pathname": "/v1.41/images/json",
            "path": "/v1.41/images/json",
            "hostname": "/var/run/docker.sock",
            "host": "/var/run/docker.sock",
            "protocol": "http:",
            "href": "http://%2Fvar%2Frun%2Fdocker.sock/v1.41/images/json",
        },
        'http://%2f%2F.%2Fpipe%2Fdocker_engine/v1.41/images/json': {
            "pathname": "/v1.41/images/json",
            "path": "/v1.41/images/json",
            "hostname": "//./pipe/docker_engine",
            "host": "//./pipe/docker_engine",
            "protocol": "http:",
            "href": "http://%2F%2F.%2Fpipe%2Fdocker_engine/v1.41/images/json",
        }
    };

    it("format", () => {
        var url = new net.Url();

        for (var u in parseTests) {
            url.format(parseTests[u]);
            assert.equal(url.href, parseTests[u].href);
        }
    });

    it("url.format", () => {
        for (var u in parseTests) {
            var href = url.format(parseTests[u]);
            assert.equal(href, parseTests[u].href);
        }
    });

    it("parse", () => {
        var url = new net.Url();

        for (var u in parseTests) {
            url.parse(u);
            assert.equal(url.href, parseTests[u].href);
        }
    });

    it("url.parse", () => {
        for (var u in parseTests) {
            var uo = url.parse(u);
            assert.equal(uo.href, parseTests[u].href);
        }
    });

    it("url.parse querystring", () => {
        assert.deepEqual(url.parse("http://a.com/test?a=100&b=200", true).query.toJSON(), {
            a: "100",
            b: "200"
        });
    });

    it("hostname", () => {
        var url = new net.Url();

        url.hostname = 'a.com';
        assert.equal(url.host, 'a.com');

        url.port = 100;
        assert.equal(url.host, 'a.com:100');

        url.host = 'b.com:20';
        assert.equal(url.hostname, 'b.com');
        assert.equal(url.port, '20');
    });

    it("resolve", () => {
        var relativeTests = [
            ['/foo/bar/baz', 'quux', '/foo/bar/quux'],
            ['/foo/bar/baz', 'quux/asdf',
                '/foo/bar/quux/asdf'
            ],
            ['/foo/bar/baz', 'quux/baz',
                '/foo/bar/quux/baz'
            ],
            ['/foo/bar/baz', '../quux/baz',
                '/foo/quux/baz'
            ],
            ['/foo/bar/baz', '/bar', '/bar'],
            ['/foo/bar/baz/', 'quux', '/foo/bar/baz/quux'],
            ['/foo/bar/baz/', 'quux/baz',
                '/foo/bar/baz/quux/baz'
            ],
            ['/foo/bar/baz',
                '../../../../../../../../quux/baz',
                '/quux/baz'
            ],
            ['/foo/bar/baz',
                '../../../../../../../quux/baz',
                '/quux/baz'
            ],
            ['foo/bar', '../../../baz', '../../baz'],
            ['foo/bar/', '../../../baz', '../baz'],
            ['http://example.com/b//c//d;p?q#blarg',
                'https:#hash2', 'https:#hash2'
            ],
            ['http://example.com/b//c//d;p?q#blarg',
                'https:/p/a/t/h?s#hash2',
                'https:/p/a/t/h?s#hash2'
            ],
            ['http://example.com/b//c//d;p?q#blarg',
                'https://u:p@h.com/p/a/t/h?s#hash2',
                'https://u:p@h.com/p/a/t/h?s#hash2'
            ],
            ['http://example.com/b//c//d;p?q#blarg',
                'https:/a/b/c/d', 'https:/a/b/c/d'
            ],
            ['http://example.com/b//c//d;p?q#blarg',
                'http:#hash2',
                'http://example.com/b//c//d;p?q#hash2'
            ],
            ['http://example.com/b//c//d;p?q#blarg',
                'http:/p/a/t/h?s#hash2',
                'http://example.com/p/a/t/h?s#hash2'
            ],
            ['http://example.com/b//c//d;p?q#blarg',
                'http://u:p@h.com/p/a/t/h?s#hash2',
                'http://u:p@h.com/p/a/t/h?s#hash2'
            ],
            ['http://example.com/b//c//d;p?q#blarg',
                'http:/a/b/c/d',
                'http://example.com/a/b/c/d'
            ],
            ['/foo/bar/baz', '/../etc/passwd',
                '/etc/passwd'
            ]
        ];

        var url = new net.Url();
        relativeTests.forEach((relativeTest) => {
            url.parse(relativeTest[0]);
            var url1 = url.resolve(relativeTest[1]);
            assert.equal(relativeTest[2], url1.href);
        });
    });

    it("pathToFileURL", () => {
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
                assert.throws(() => url.pathToFileURL('\\\\\\no-server'));

                // Missing share or resource:
                assert.throws(() => url.pathToFileURL('\\\\host'));
            } else {
                // UNC paths on posix are considered a single path that has backslashes:
                const fileURL = url.pathToFileURL('\\\\nas\\share\\path.txt').href;
                assert.ok(/file:\/\/.+%5C%5Cnas%5Cshare%5Cpath\.txt$/.test(fileURL));
            }
        }

        {
            let testCases;
            if (isWindows) {
                testCases = [
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
            } else {
                testCases = [
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
            }

            for (const testCase of testCases) {
                const u = url.pathToFileURL(testCase.path);
                const actual = u.href;
                assert.strictEqual(actual, testCase.expected);
            }
        }
    });

    it("fileURLToPath", () => {
        function testInvalidArgs(...args) {
            for (const arg of args) {
                assert.throws(() => url.fileURLToPath(arg));
            }
        }

        // Input must be string or URL
        testInvalidArgs(null, undefined, 1, {}, true);

        // Input must be a file URL
        assert.throws(() => url.fileURLToPath('https://a/b/c'));

        {
            const withHost = new URL('file://host/a');

            if (isWindows) {
                assert.strictEqual(url.fileURLToPath(withHost), '\\\\host\\a');
            } else {
                assert.throws(() => url.fileURLToPath(withHost));
            }
        }

        {
            if (isWindows) {
                assert.throws(() => url.fileURLToPath('file:///C:/a%2F/'));
                assert.throws(() => url.fileURLToPath('file:///C:/a%5C/'));
                assert.throws(() => url.fileURLToPath('file:///?:/'));
            } else {
                assert.throws(() => url.fileURLToPath('file:///a%2F/'));
            }
        }

        {
            let testCases;
            if (isWindows) {
                testCases = [
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
            } else {
                testCases = [
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
            }

            for (const { path, fileURL } of testCases) {
                const fromString = url.fileURLToPath(fileURL);
                assert.strictEqual(fromString, path);
                const fromURL = url.fileURLToPath(new URL(fileURL));
                assert.strictEqual(fromURL, path);
            }
        }
    });
});

require.main === module && test.run(console.DEBUG);