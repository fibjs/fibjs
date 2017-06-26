var path = require('path');
var parser = require('./util/parser');
var gen_code = require('./util/gen_code');

var idlFolder = path.join(__dirname, "../idl/zh-cn");
var baseFolder = path.join(__dirname, "../fibjs/include/ifs/");

var defs = parser(idlFolder);
gen_code(defs, baseFolder);