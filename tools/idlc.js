var path = require('path');
var parser = require('./util/parser');
var gen_code = require('./util/gen_code');
var generator = require('./util/generator');
var record_idljson = require('./util/record_idljson');
var gen_dts = require('./util/gen_dts');
var { mkdirp } = require('../fibjs/scripts/internal/helpers/fs');

var idlLang = process.env.FIBJS_IDL_LANG || 'zh-cn'
var idlFolder = path.join(__dirname, `../idl/${idlLang}`);
var baseCodeFolder = path.join(__dirname, "../fibjs/include/ifs/");

var defs = parser(idlFolder);

record_idljson(defs);

// generate back
generator(defs, 'zh-CN', 'zh-cn');

gen_code(defs, baseCodeFolder);

const DTS_DIST_DIR = path.resolve(__dirname, `../npm/types/dts/`);
mkdirp(DTS_DIST_DIR);
gen_dts(parser(idlFolder), { DTS_DIST_DIR });

module.exports = defs;
