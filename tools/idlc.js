var path = require('path');
var parser = require('./util/parser');
var gen_code = require('./util/gen_code');

var baseFolder = path.join(__dirname, "../fibjs/include/ifs/");

var defs = parser(baseFolder);
gen_code(defs, baseFolder);