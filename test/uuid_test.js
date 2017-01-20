var test = require("test");
test.setup();

var uuid = require('uuid');
var encoding = require('encoding');

describe("uuid", () => {
    it("test", () => {
        var id = uuid.node();
        var id1 = uuid.uuid(id);
        var id2 = uuid.uuid(id.data());

        assert.equal(id.toString(), id1.toString());
        assert.equal(id.toString(), id2.toString());
    });
});

//test.run(console.DEBUG);
