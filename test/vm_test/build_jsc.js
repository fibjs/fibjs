var os = require('os');
var fs = require('fs');
var path = require('path');
var util = require('util');

const jscContent = util.compile("jsc_test.js", "module.exports = {a : 100};");

;[
    `jsc_test_${os.arch()}.jsc`,
    `jsc_test_${os.arch()}-${os.platform()}.jsc`,
].forEach(fname => {
    fs.writeFile(path.join(__dirname, fname), jscContent);
    console.notice(`[build_jsc] jsc file ${fname} generated`);
});
