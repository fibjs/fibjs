const fs = require('fs')

const gen_ts_type = require('./util/gen_ts_type');
const defs = require('./idlc')

const typesDistDir = process.env.FIBJS_IDL2TYPE_DIST

if (!typesDistDir) {
    throw `no target dist for *.d.ts files to be generated.`
}

if (!fs.exists(typesDistDir)) {
    throw `${FIBJS_IDL2TYPE_DIST} doesn's exist.`
}

console.log(`would write *.d.ts to ${typesDistDir}`)
gen_ts_type(defs, typesDistDir);