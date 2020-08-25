var fs = require('fs');
var path = require('path');
var parser = require('./util/parser');
var generator = require('./util/generator');

var idlLang = process.env.FIBJS_IDL_LANG || 'zh-cn'
var idlFolder = path.join(__dirname, `../idl/${idlLang}`);

var defs = parser(idlFolder);

// dump defs as json;
var snapshotsDir = path.resolve(__dirname, '../idl/__snapshots__');
try { fs.mkdir(snapshotsDir) } catch (error) { };

fs.writeTextFile(
    path.resolve(snapshotsDir, `./defs_${idlLang}.json`),
    JSON.stringify(defs, null, '  ')
)

// validate back
generator(defs, 'zh-CN', 'zh-cn');
generator(defs, 'en', 'us-en');
