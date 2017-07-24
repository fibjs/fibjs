var os = require('os');
var fs = require('fs');
var path = require('path');
var util = require('util');

fs.writeFile(path.join(__dirname, "jsc_test_" + os.arch() + ".jsc"),
    util.compile("jsc_test.js", "module.exports = {a : 100};"));