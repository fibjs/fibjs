var fs = require('fs');
var path = require('path');

var { mkdirp } = require('../../fibjs/scripts/internal/helpers/fs');

/**
 * @description record idljson to output directory
 * 
 * @param {Record<string, import('../../idl/ir').IIDLDefinition>} defs
 */
module.exports = function (defs) {
    const entries = Object.entries(defs);
    const moduleCount = entries.filter(([, def]) => def.declare.module).length;
    const interfaceCount = entries.length - moduleCount;
    
    console.log(`   📝 Recording ${entries.length} definitions (${moduleCount} modules, ${interfaceCount} interfaces)...`);
    
    entries.forEach(([kname, def]) => {
        const ismodule = def.declare.module;
        const basedir = path.resolve(__dirname, `../../out/idljson/${ismodule ? 'module' : 'interface'}`);
        mkdirp(basedir);

        fs.writeFileSync(path.join(basedir, `${kname}.json`), JSON.stringify(def, null, '  '));
    });
    
    console.log(`   ✅ Recorded JSON definitions to out/idljson/`);
}