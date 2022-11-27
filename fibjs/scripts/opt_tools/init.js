var fs = require('fs');
var path = require('path');

console.log('Press ^C at any time to quit.');

var info;

try {
    info = JSON.parse(fs.readTextFile('package.json'));
} catch (e) {}

if (info === undefined) {
    info = {
        "name": path.basename(process.cwd()),
        "version": '1.0.0',
        "description": '',
        "repository": '',
        "keywords": '',
        "author": '',
        "license": 'ISC'
    }
}

["name", "version", "description", "repository", "keywords", "author", "license"].forEach(n => {
    var v = console.readLine(`${n}: ` + (info[n] ? `(${info[n]}) ` : ''));
    if (v !== '')
        info[n] = v;
});

var f = path.join(process.cwd(), 'package.json');
console.log(`About to write to $[f}:`);

var txt = JSON.stringify(info, null, "  ");

console.log(txt);

var p = console.readLine('Is this ok? (yes)');
if (p === '' || p === 'yes')
    fs.writeFile(f, txt);