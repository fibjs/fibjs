var test = require("test");
test.setup();

var coroutine = require('coroutine');

describe("v8 bug", () => {
    it("not hungup", () => {
        for (var i = 0; i < 100; i++)
            coroutine.sleep(1);
    });

    it('local varible error', () => {
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
});

repl && test.run(console.DEBUG);