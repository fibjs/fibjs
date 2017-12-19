var fs = require('fs');
var path = require('path');
var encoding = require('encoding');

var txts = [];
var base = path.join(__dirname, "../fibjs/scripts");

function enc_folder(dir) {
    fs.readdir(path.join(base, dir)).forEach(f => {
        if (fs.stat(path.join(base, dir, f)).isDirectory()) {
            enc_folder(path.join(dir, f));
        } else if (f.substr(0, 1) !== '.') {
            console.log("encoding", path.join(dir, f));
            var code = fs.readTextFile(path.join(base, dir, f));

            var pos = 0;
            var sz = 20;

            var hex = new Buffer(code).hex();

            var cnt = 0;
            hex = '"' + hex.replace(/../g, s => {
                cnt++;
                if (cnt == 30) {
                    cnt = 0;
                    return "\\x" + s + '"\n    "';
                }
                return "\\x" + s;
            }) + '"';

            txts.push('"' + path.join(dir, path.basename(f, ".js")) + '",\n    ' + hex + ',');
        }
    });
}

enc_folder("");

fs.writeFile(path.join(__dirname, "../fibjs/src/base/opt_tools.cpp"), `/*
* opt_tools.cpp
*
*  Created on: Oct 15, 2017
*      Author: lion
*/

#include "utils.h"

namespace fibjs {
const char* opt_tools[] = {
    ${txts.join('\n    ')}
    NULL
};
}`);