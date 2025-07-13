var path = require('path');
var parser = require('./util/parser');
var gen_code = require('./util/gen_code');
var generator = require('./util/generator');
var record_idljson = require('./util/record_idljson');
var gen_dts = require('./util/gen_dts');

var idlLang = process.env.FIBJS_IDL_LANG || 'zh-cn'
var idlFolder = path.resolve(__dirname, `../idl/${idlLang}`);
var baseCodeFolder = path.resolve(__dirname, "../fibjs/include/ifs/");

console.log('🚀 Starting fibjs IDL compilation...');
console.log(`📂 IDL Language: ${idlLang}`);
console.log(`📁 IDL Folder: ${idlFolder}`);
console.log(`📁 Code Output: ${baseCodeFolder}`);

console.log('\n📖 Parsing IDL definitions...');
var defs = parser(idlFolder);
console.log(`✅ Parsed ${Object.keys(defs).length} IDL definitions`);

console.log('\n💾 Recording IDL JSON...');
record_idljson(defs);
console.log('✅ IDL JSON recorded');

console.log('\n🔄 Generating IDL files...');
// generate back
generator(defs, 'zh-CN', 'zh-cn');
console.log('✅ IDL files generated');

console.log('\n⚡ Generating C++ code...');
gen_code(defs, baseCodeFolder);
console.log('✅ C++ code generated');

console.log('\n📝 Generating TypeScript definitions...');
const DTS_DIST_DIR = path.resolve(__dirname, `../npm/types/dts/`);
require('fs').mkdirSync(DTS_DIST_DIR, { recursive: true });
gen_dts(parser(idlFolder), { DTS_DIST_DIR });
console.log('✅ TypeScript definitions generated');

console.log('\n🎉 IDL compilation completed successfully!');

module.exports = defs;
