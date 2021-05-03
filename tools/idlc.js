var path = require('path');
var parser = require('./util/parser');
var gen_code = require('./util/gen_code');
var generator = require('./util/generator');
var record_idljson = require('./util/record_idljson');
var gen_dts = require('./util/gen_dts');

var idlLang = process.env.FIBJS_IDL_LANG || 'zh-cn'
var idlFolder = path.join(__dirname, `../idl/${idlLang}`);
var baseCodeFolder = path.join(__dirname, "../fibjs/include/ifs/");

var defs = parser(idlFolder);

record_idljson(defs);

// generate back
generator(defs, 'zh-CN', 'zh-cn');

gen_code(defs, baseCodeFolder);
gen_dts(defs);

module.exports = defs;
