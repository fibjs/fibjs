var test = require("test");
test.setup();

var uuid = require('uuid');
var encoding = require('encoding');

describe("uuid", () => {
    it("test", () => {
        var id = uuid.node();
        var id1 = uuid.random();

        assert.notEqual(id.toString(), id1.toString());
    });
});

// test.run(console.DEBUG);
