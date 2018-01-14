var json_format = (function () {
    var p = [],
        indentConfig = {
            tab: {
                char: '\t',
                size: 1
            },
            space: {
                char: ' ',
                size: 4
            }
        },
        configDefault = {
            type: 'tab'
        },
        push = function (m) {
            return '\\' + p.push(m) + '\\';
        },
        pop = function (m, i) {
            return p[i - 1]
        },
        tabs = function (count, indentType) {
            return new Array(count + 1).join(indentType);
        };

    function JSONFormat(json, indentType) {
        p = [];
        var out = "",
            indent = 0;

        // Extract backslashes and strings
        json = json
            .replace(/\\./g, push)
            .replace(/(".*?"|'.*?')/g, push)
            .replace(/\s+/, '');

        // Indent and insert newlines
        for (var i = 0; i < json.length; i++) {
            var c = json.charAt(i);

            switch (c) {
                case '{':
                case '[':
                    out += c + "\n" + tabs(++indent, indentType);
                    break;
                case '}':
                case ']':
                    out += "\n" + tabs(--indent, indentType) + c;
                    break;
                case ',':
                    out += ",\n" + tabs(indent, indentType);
                    break;
                case ':':
                    out += ": ";
                    break;
                default:
                    out += c;
                    break;
            }
        }

        // Strip whitespace from numeric arrays and put backslashes 
        // and strings back in
        out = out
            .replace(/\[[\d,\s]+?\]/g, function (m) {
                return m.replace(/\s/g, '');
            })
            .replace(/\\(\d+)\\/g, pop) // strings
            .replace(/\\(\d+)\\/g, pop); // backslashes in strings

        return out;
    };

    return function (json, config) {
        config = config || configDefault;
        var indent = indentConfig[config.type];

        if (indent == null) {
            throw new Error('Unrecognized indent type: "' + config.type + '"');
        }
        var indentType = new Array((config.size || indent.size) + 1).join(indent.char);
        return JSONFormat(JSON.stringify(json), indentType);
    }
})();

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
        "license": 'ISC',
        "dependencies": [],
        "devDependencies": []
    }
}

["name", "version", "description", "repository", "keywords", "author", "license"].forEach(n => {
    var v = console.readLine(`${n}: ` + (info[n] ? `(${info[n]}) ` : ''));
    if (v !== '')
        info[n] = v;
});

var f = path.join(process.cwd(), 'package.json');
console.log(`About to write to $[f}:`);

var txt = json_format(info, {
    type: 'space',
    size: 2
});

console.log(txt);

var p = console.readLine('Is this ok? (yes)');
if (p === '' || p === 'yes')
    fs.writeFile(f, txt);