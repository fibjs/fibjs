var test = require("test");
test.setup();

var coroutine = require('coroutine');
var util = require('util');
var path = require('path');
var process = require('process');

describe("v8 test", () => {
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

    it('snapshot', () => {
        assert.isTrue(util.buildInfo().vender['v8-snapshot']);
    })

    it('for (var n in {}) {}', () => {
        for (var n in {}) {}
    });

    if (global.full_test)
        it('hash seed', () => {
            assert.notEqual(process.open(process.execPath, [path.join(__dirname, 'v8_seed_test.js')]).readLine(),
                process.open(process.execPath, [path.join(__dirname, 'v8_seed_test.js')]).readLine());
        });
});

require.main === module && test.run(console.DEBUG);