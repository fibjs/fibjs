var os = require('os');
var fs = require('fs');
var util = require('util');

fs.writeFile("jsc_test_" + os.arch + ".jsc",
    util.compile("jsc_test.js", "module.exports = {a : 100};"));