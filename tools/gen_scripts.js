var fs = require('fs');
var path = require('path').posix;
var encoding = require('encoding');
var zlib = require('zlib');
var util = require('util');

var datas = [];
var txts = [];
var base = path.join(__dirname, "../fibjs/scripts");

function enc_folder(dir) {
    fs.readdir(path.join(base, dir)).forEach(f => {
        var stat = fs.stat(path.join(base, dir, f))
        if (stat.isDirectory()) {
            enc_folder(path.join(dir, f));
        }

        if (!stat.isFile()) {
            return
        }

        if (f.substr(0, 1) === '.') {
            return
        }

        var isTypeScript = f.indexOf('.ts') !== -1;
        var isJavaScript = f.indexOf('.js') !== -1;
        
        if (!isJavaScript && !isTypeScript) {
            return
        }

        var code = fs.readTextFile(path.join(base, dir, f));

        // Convert TypeScript to JavaScript if needed
        if (isTypeScript) {
            console.log("encoding", path.join(dir, f), "(converting TypeScript to JavaScript)");
            code = util.stripTypeScript(code);
        } else {
            console.log("encoding", path.join(dir, f));
        }

        var pos = 0;
        var sz = 20;

        var bin = zlib.deflate(new Buffer(code));
        var hex = bin.hex();

        var cnt = 0;
        hex = '{\n    ' + hex.replace(/../g, s => {
            cnt++;
            if (cnt == 20) {
                cnt = 0;
                return `0x${s},\n    `;
            }
            return `0x${s}, `;
        }) + '0}';

        var idx = datas.length;
        datas.push(`static const unsigned char dat_${idx}[] = ` + hex + ';');
        
        // Remove extension (.js or .ts) for the internal name
        var baseName = isTypeScript ? path.basename(f, ".ts") : path.basename(f, ".js");
        txts.push('{"' + path.join(dir, baseName) + '", ' + bin.length + `, (const char*)dat_${idx}},`);
    });
}

enc_folder("");

fs.writeFile(path.join(__dirname, "../fibjs/src/base/opt_tools.cpp"), `/*
* opt_tools.cpp
*
*  Created on: Oct 15, 2017
*      Author: lion
*/

#include "options.h"

namespace fibjs {

${datas.join('\n\n')}

const OptData opt_tools[] = {
    ${txts.join('\n    ')}
    {NULL, 0, NULL}
};
}`);