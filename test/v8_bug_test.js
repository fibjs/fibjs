var { describe, it, before, after, beforeEach, afterEach } = require('node:test');
var assert = require('assert');

var coroutine = require('coroutine');
var util = require('util');
var path = require('path');
var process = require('process');

describe("v8 bug", () => {
    it("not hangup", () => {
        for (var i = 0; i < 100; i++)
            coroutine.sleep(1);
    });

    it('local variable error', () => {
        function find(data) {
            var rs = {
                billmoney: 0
            };

            if (rs.billmoney)
                return rs.billmoney;

            rs.billmoney = data * 0.1;
            return 0;
        }

        assert.equal(find(99), 0);
        assert.equal(find(99), 0);
        assert.equal(find(99), 0);
    });

    it('toLocaleDateString', () => {
        new Date().toLocaleDateString();
    });

    // The fixed-notation branch of Number.prototype.toPrecision calls
    // SimpleStringBuilder::AddPadding('0', count), and count can be negative
    // when the significand is longer than the integer part: 1.5 with p = 3
    // gives decimal_point = 1, decimal_rep_length = 3, so count = -2.
    // The helper is documented to add nothing for a non-positive count, but it
    // used to hand the count to std::fill_n. libstdc++ 9 - the aarch64 cross
    // toolchain - computes the end iterator as `__first + __n` in its char
    // specialization, so the negative count becomes a huge size_t length for
    // memset and the process dies with SIGSEGV on linux/arm64.
    // Crash condition: the result matches /^-?[1-9]\d*\.\d+$/ (fixed notation,
    // fraction digits, non-zero integer part).
    it('toPrecision fixed notation with fraction digits', () => {
        var cases = [
            // fixed notation with fraction digits: the crashing shape
            [1, 2, '1.0'],
            [1.5, 3, '1.50'],
            [42.5, 3, '42.5'],
            [46327, 6, '46327.0'],
            [100.5, 5, '100.50'],
            [1234.5678, 9, '1234.56780'],
            [10, 3, '10.0'],
            [0.9999, 2, '1.0'],
            [-42.5, 3, '-42.5'],
            // fixed notation without fraction digits: no padding
            [1, 1, '1'],
            [46327, 5, '46327'],
            [9.99, 2, '10'],
            [42.5, 2, '43'],
            // zero integer part / exponential notation
            [0.5, 3, '0.500'],
            [0.000123, 21, '0.000123000000000000008198'],
            [1e21, 3, '1.00e+21'],
        ];

        for (var i = 0; i < cases.length; i++) {
            var value = cases[i][0];
            var precision = cases[i][1];
            assert.equal(value.toPrecision(precision), cases[i][2],
                value + '.toPrecision(' + precision + ')');
        }
    });

    it('snapshot', () => {
        assert.isTrue(util.buildInfo().vender['v8-snapshot']);
    })

    it('for (var n in {}) {}', () => {
        for (var n in {}) { }
    });
});

