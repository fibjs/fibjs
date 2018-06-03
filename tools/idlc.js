var path = require('path');
var parser = require('./util/parser');
var gen_code = require('./util/gen_code');

var idlLang = process.env.FIBJS_IDL_LANG || 'zh-cn'
var idlFolder = path.join(__dirname, `../idl/${idlLang}`);
var baseCodeFolder = path.join(__dirname, "../fibjs/include/ifs/");

var defs = parser(idlFolder);
gen_code(defs, baseCodeFolder);

module.exports = defs;
