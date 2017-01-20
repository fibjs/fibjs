var assert = require("assert");

exports.func = () => {
    assert.equal(__sbname, "test");
}
