var { describe, it } = require('node:test');
var assert = require('assert');

// The error contract, asserted the way a downstream consumer would rely on it:
//   * `name` says which JS class was thrown;
//   * `code` says precisely what went wrong;
//   * `number` is deliberately NOT asserted, and is not even assumed to exist:
//     errors raised by the JS built-ins (`internal/errors.js`) carry `code`
//     without `number`, because they never pass through `result_t`. See
//     plans/error-codes.md section 2.1 -- pinning `number` here would freeze an
//     implementation detail of one of the two channels.
//
// When adding a row, quote the Node runtime's own code so a regression shows up
// as a changed string rather than a changed class only.

function capture(fn) {
    try {
        fn();
    } catch (e) {
        return e;
    }
    return null;
}

function check(label, fn, expectedName, expectedCode) {
    var e = capture(fn);
    assert.ok(e, label + ': expected a throw');
    assert.strictEqual(e.name, expectedName, label + ': name');
    assert.strictEqual(e.code, expectedCode, label + ': code');
}

describe('error contract', () => {
    var fs = require('fs');
    var crypto = require('crypto');

    describe('argument layer (P2)', () => {
        it('missing required argument', () => {
            check('readFileSync()', function () { return fs.readFileSync(); },
                'TypeError', 'ERR_INVALID_ARG_TYPE');
        });

        it('wrong argument type', () => {
            check('readFileSync(path, 2)',
                function () { return fs.readFileSync('/tmp/x', 2); },
                'TypeError', 'ERR_INVALID_ARG_TYPE');
        });

        it('bad option value', () => {
            check('readFileSync(path, {flag})',
                function () { return fs.readFileSync('/tmp/x', { flag: 'zz' }); },
                'TypeError', 'ERR_INVALID_ARG_VALUE');
        });

        it('out of range', () => {
            check('Buffer.alloc(-1)', function () { return Buffer.alloc(-1); },
                'RangeError', 'ERR_OUT_OF_RANGE');
        });

        it('unknown encoding', () => {
            check('Buffer.from(x, nope)',
                function () { return Buffer.from('x', 'nope'); },
                'TypeError', 'ERR_UNKNOWN_ENCODING');
        });
    });

    describe('closed-state family (P3)', () => {
        var tmp = '/tmp/fibjs_error_codes_test.txt';

        it('file stream after close', () => {
            if (typeof fs.openFile !== 'function')
                return;
            fs.writeFileSync(tmp, 'x');
            var f = fs.openFile(tmp, 'r');
            f.close();
            check('FileStream.write after close', function () { return f.write('x'); },
                'Error', 'EBADF');
        });

        it('zip file after close', () => {
            var zip;
            try { zip = require('zip'); } catch (e) { return; }

            // A closed handle is what is under test, so the archive need only
            // exist: write one through fs so the fixture does not depend on a
            // checked-in binary.
            var archive = '/tmp/fibjs_error_codes_test.zip';
            var z = zip.open(archive, 'w');
            z.write(Buffer.from('hello'), 'a.txt');
            z.close();

            z = zip.open(archive, 'r');
            z.close();
            check('ZipFile.namelist after close', function () { return z.namelist(); },
                'Error', 'ERR_FIBJS_ZIP_CLOSED');
        });

        it('database after close', () => {
            var db = require('db');
            var d = db.openSQLite('/tmp/fibjs_error_codes_test.db');
            d.close();
            check('SQLite.execute after close',
                function () { return d.execute('select 1'); },
                'Error', 'ERR_INVALID_STATE');
        });

        it('dgram socket after close', () => {
            var dgram;
            try { dgram = require('dgram'); } catch (e) { return; }
            var d = dgram.createSocket('udp4');
            d.bind(0);
            d.close();
            check('dgram.send after close',
                function () { return d.send('x', 1, '127.0.0.1'); },
                'Error', 'ERR_SOCKET_DGRAM_NOT_RUNNING');
        });

        it('dgram socket bound twice', () => {
            var dgram;
            try { dgram = require('dgram'); } catch (e) { return; }
            var d = dgram.createSocket('udp4');
            d.bind(0);
            try {
                check('dgram.bind twice', function () { return d.bind(0); },
                    'Error', 'ERR_SOCKET_ALREADY_BOUND');
            } finally {
                d.close();
            }
        });

        it('tcp server listened twice', () => {
            var net = require('net');
            var s = net.createServer(function (c) { c.end(); });
            s.listen(0);
            try {
                check('TcpServer.listen twice', function () { return s.listen(0); },
                    'Error', 'ERR_SERVER_ALREADY_LISTEN');
            } finally {
                s.close();
            }
        });
    });

    describe('unknown values', () => {
        it('unknown digest', () => {
            check('createHmac(nope)', function () { return crypto.createHmac('nope', 'k'); },
                'TypeError', 'ERR_CRYPTO_INVALID_DIGEST');
        });

        it('timingSafeEqual length mismatch', () => {
            check('timingSafeEqual(1B, 2B)',
                function () { return crypto.timingSafeEqual(Buffer.from('a'), Buffer.from('ab')); },
                'RangeError', 'ERR_CRYPTO_TIMING_SAFE_EQUAL_LENGTH');
        });
    });

    describe('number is a grouping, not a contract', () => {
        // The guarantee is one-directional: a given code always arrives with the
        // same number, but two different codes may share one (20002 and 20005
        // are both CALL_E_* channels that surface as code ERR_INVALID_ARG_TYPE).
        // Errors from the JS built-ins have no number at all.
        it('the same code keeps one number on the core channel', () => {
            var a = capture(function () { return fs.readFileSync(); });
            var b = capture(function () { return fs.readFileSync('/tmp/x', 2); });
            assert.strictEqual(a.code, b.code);
            assert.strictEqual(typeof a.number, 'number');
            assert.strictEqual(typeof b.number, 'number');
        });

        it('the built-in channel carries code without number', () => {
            var e = capture(function () { return Buffer.alloc(-1); });
            assert.strictEqual(e.code, 'ERR_OUT_OF_RANGE');
            assert.strictEqual(e.number, undefined);
        });
    });
});
