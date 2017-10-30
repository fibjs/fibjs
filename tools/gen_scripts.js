var fs = require('fs');
var path = require('path');
var encoding = require('encoding');

var txts = [];

function c_encode(s) {
    return s.replace(/[\\\n'"]/g, (c) => {
        if (c == '\n')
            return "\\n"
        return "\\" + c;
    });
}

fs.readdir(path.join(__dirname, "../fibjs/scripts")).forEach(f => {
    console.log("encoding", f);
    var code = fs.readTextFile(path.join(__dirname, "../fibjs/scripts", f));
    var src = [];

    var pos = 0;
    var sz = 80;

    while (pos < code.length - sz) {
        src.push(c_encode(code.substr(pos, sz)));
        pos += sz;
    }

    src.push(c_encode(code.substr(pos)));

    txts.push('"--' + path.basename(f, ".js") + '", "' + src.join('"\n        "') + '",');
});

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