var fs = require('fs');
var path = require('path');
var encoding = require('encoding');

var txts = [];

fs.readdir(path.join(__dirname, "../fibjs/scripts")).forEach(f => {
    console.log("encoding", f);
    var code = fs.readTextFile(path.join(__dirname, "../fibjs/scripts", f));

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

    txts.push('"--' + path.basename(f, ".js") + '",\n    ' + hex + ',');
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