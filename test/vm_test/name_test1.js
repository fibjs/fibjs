var assert = require("assert");

var err = new Error();
var a = err.stack.split("\n");
var re = /test:/g;

assert.isTrue(re.test(a[1]));
assert.isFalse(re.test(a[2]));