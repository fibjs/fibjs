var fs = require('fs');
var path = require('path');

var { mkdirp } = require('../../fibjs/scripts/internal/helpers/fs');

/**
 * @description record idljson to output directory
 * 
 * @param {Record<string, import('../../idl/ir').IIDLDefinition>} defs
 */
module.exports = function (defs) {
    Object.entries(defs).forEach(([kname, def]) => {
        const ismodule = def.declare.module;
        const basedir = path.resolve(__dirname, `../../out/idljson/${ismodule ? 'module' : 'interface'}`);
        mkdirp(basedir);

        fs.writeTextFile(path.join(basedir, `${kname}.json`), JSON.stringify(def, null, '  '));

        // console.log(`generated idl IR for ${ismodule ? 'module' : 'interface'}: ${kname}`)
    });
}