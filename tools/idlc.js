const fs = require('fs');
const path = require('path');
const parser = require('./util/parser');
const gen_code = require('./util/gen_code');
const gen_ts_type = require('./util/gen_ts_type');

const idlLang = process.env.FIBJS_IDL_LANG || 'zh-cn'
const idlFolder = path.join(__dirname, `../idl/${idlLang}`);
const baseCodeFolder = path.join(__dirname, "../fibjs/include/ifs/");

const defs = parser(idlFolder);
gen_code(defs, baseCodeFolder);

const typesDistDir = process.env.FIBJS_IDL2TYPE_DIST

if (typesDistDir) {
    if (!fs.exists(typesDistDir)) {
        throw `${FIBJS_IDL2TYPE_DIST} doesn's exist.`
    }

    console.log(`would write *.d.ts to ${typesDistDir}`)
    gen_ts_type(defs, typesDistDir);
}
