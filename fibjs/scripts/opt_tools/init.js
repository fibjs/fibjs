var fs = require('fs');
var path = require('path');

function usage_text() {
    return [
        'Usage: fibjs --init',
        '',
        'Write a package.json in the current directory. Values already present in an',
        'existing package.json are offered as defaults, and an empty answer keeps',
        'them. There are no options: the file is built by answering the prompts.',
        '',
        'Options:',
        '  -h, --help                  print this message',
        '',
        'Run `fibjs --help` for the global options.',
    ].join('\n');
}

var args = process.argv.slice(2);

if (args.indexOf('--help') >= 0 || args.indexOf('-h') >= 0) {
    console.log(usage_text());
    process.exit(0);
}

var unknown = args.filter(function (a) { return a.charAt(0) === '-'; });
if (unknown.length > 0) {
    console.error('fibjs --init: unknown option: ' + unknown[0]);
    console.error(usage_text());
    process.exit(1);
}

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
console.log(`About to write to ${f}:`);

var txt = JSON.stringify(info, null, "  ");

console.log(txt);

var p = console.readLine('Is this ok? (yes)');
if (p === '' || p === 'yes')
    fs.writeFile(f, txt);