var parser = require('./util/parser');
var gen_code = require('./util/gen_code');

var baseFolder = "../fibjs/include/ifs/";

var defs = parser(baseFolder);
gen_code(defs, baseFolder);