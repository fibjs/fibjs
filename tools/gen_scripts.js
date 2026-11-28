var fs = require('fs');
var path = require('path').posix;
var zlib = require('zlib');
var util = require('util');

var base = path.join(__dirname, "../fibjs/scripts");
var target = path.join(__dirname, "../fibjs/src/base/opt_tools.cpp");

/**
 * @description what the checked-in file already embeds. One deflate build is
 *              deterministic (the same source and zlib give the same bytes), but
 *              the format does not require a particular encoding: another zlib
 *              — an upgraded vendored one, a different level — emits different
 *              bytes for the same source. Comparing the decompressed content
 *              lets an unchanged module keep the array it already has, so a
 *              regeneration only moves the entries whose sources moved. The
 *              comparison is on the converted code, not on the source file's
 *              bytes, because a `.ts` source is embedded after `stripTypeScript`
 *
 *              What is compared is the *current* source against what the file
 *              embeds, not a previous run's state: a source an earlier run
 *              forgot (the embedded copy is older than the file) does not match
 *              and is compressed again. Nothing can stay stale across a run,
 *              and `--check` exists so a forgotten regeneration is noticed
 *              before it is committed
 */
function read_previous(file) {
    var previous = { lines: {}, entries: [] };

    if (!fs.exists(file))
        return previous;

    var text = fs.readTextFile(file);

    text.replace(/static const unsigned char dat_(\d+)\[\] = \{[\s\S]*?\};/g, function (all, idx) {
        previous.lines[idx] = all;
        return all;
    });

    text.replace(/\{"([^"]+)", (\d+), \(const char\*\)dat_(\d+)\},/g, function (all, name, len, idx) {
        previous.entries.push({ name: name, len: parseInt(len, 10), dat: idx });
        return all;
    });

    return previous;
}

var previous = read_previous(target);
var previous_by_name = {};

previous.entries.forEach(function (e) {
    previous_by_name[e.name] = e;
});

/**
 * @description is that embedded array the compression of `code`? The number in
 *              the table is the compressed length, so the two are compared
 *              through the decompressor rather than by size
 */
function same_content(entry, code) {
    var line = previous.lines[entry.dat];

    if (!line)
        return false;

    try {
        var bytes = [];

        line.replace(/0x([0-9a-fA-F]{2})/g, function (all, hex) {
            bytes.push(parseInt(hex, 16));
            return all;
        });

        return zlib.inflate(Buffer.from(bytes)).toString('utf8') === code;
    } catch (e) {
        // an array this build cannot inflate is not a reason to fail the build:
        // it is compressed again below
        return false;
    }
}

function encode(code) {
    // node 兼容：不能再依赖已废弃的 `new Buffer(string)`（见 plans/buffer-node-compat-*）
    var bin = zlib.deflate(Buffer.from(code, 'utf8'));
    var hex = bin.hex();

    var cnt = 0;
    hex = '{\n    ' + hex.replace(/../g, function (s) {
        cnt++;
        if (cnt == 20) {
            cnt = 0;
            return `0x${s},\n    `;
        }
        return `0x${s}, `;
    }) + '0}';

    return { bin: bin, hex: hex };
}

// the modules to embed, by the name they are required under. `fs.readdir` has
// no order of its own, so the walk is sorted: a new file lands in a predictable
// place instead of wherever the filesystem happened to list it
var sources = {};
var source_order = [];

function enc_folder(dir) {
    fs.readdir(path.join(base, dir)).sort().forEach(f => {
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

        // A `.d.ts` next to the JS it describes is a declaration file for the
        // type checker (read by tools/util/gen_dts.js), never a module to embed.
        // Without this guard `indexOf('.ts')` matches, `stripTypeScript` blanks
        // the text and `basename(f, '.ts')` turns `stream.d.ts` into `stream.d`,
        // which used to register an empty module silently.
        if (f.endsWith('.d.ts')) {
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
            console.log("converting", path.join(dir, f));
            code = util.stripTypeScript(code);
        }

        // Remove extension (.js or .ts) for the internal name
        var baseName = isTypeScript ? path.basename(f, ".ts") : path.basename(f, ".js");
        var name = path.join(dir, baseName);

        if (sources[name] === undefined)
            source_order.push(name);

        sources[name] = code;
    });
}

enc_folder("");

// the checked-in table order is kept: an entry that is still there keeps its
// array index, so editing or adding a module does not renumber the ones after
// it. New names are appended, sorted; a deleted one is simply dropped
var names = [];
var seen = {};

previous.entries.forEach(function (e) {
    if (sources[e.name] !== undefined && !seen[e.name]) {
        seen[e.name] = true;
        names.push(e.name);
    }
});

source_order.slice().sort().forEach(function (name) {
    if (!seen[name]) {
        seen[name] = true;
        names.push(name);
    }
});

var datas = [];
var txts = [];
var stats = { unchanged: 0, updated: 0, added: 0, dropped: 0 };

names.forEach(function (name) {
    var code = sources[name];
    var old = previous_by_name[name];
    var line, len;

    if (old && same_content(old, code)) {
        // byte for byte what the file already had. The array index is written
        // from the position it takes here, so a dropped entry renumbers the
        // ones after it without touching their contents
        line = previous.lines[old.dat].replace(/^static const unsigned char dat_\d+\[\]/,
            'static const unsigned char dat_' + datas.length + '[]');
        len = old.len;
        stats.unchanged++;
    } else {
        var encoded = encode(code);
        line = `static const unsigned char dat_${datas.length}[] = ` + encoded.hex + ';';
        len = encoded.bin.length;

        // an old entry that does not match is exactly a source an earlier run
        // missed, and it is repaired here. A missing one is a new module
        if (old) {
            console.log("updating", name);
            stats.updated++;
        } else {
            console.log("adding", name);
            stats.added++;
        }
    }

    var idx = datas.length;
    datas.push(line);
    txts.push('{"' + name + '", ' + len + `, (const char*)dat_${idx}},`);
});

var dropped = [];

previous.entries.forEach(function (e) {
    if (sources[e.name] === undefined && dropped.indexOf(e.name) < 0)
        dropped.push(e.name);
});

stats.dropped = dropped.length;
dropped.forEach(function (name) {
    console.log("dropping", name);
});

console.log(`opt_tools.cpp: ${stats.unchanged} unchanged, ${stats.updated} updated, ` +
    `${stats.added} added, ${stats.dropped} dropped`);

var out = `/*
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
}`;

// the build watches this file: writing the same bytes again would only make it
// recompile and relink for nothing
var up_to_date = fs.exists(target) && fs.readTextFile(target) === out;

if (process.argv.indexOf('--check') > -1) {
    // a forgotten regeneration is the one failure mode this tool can have: the
    // binary keeps embedding an old copy of a module and nothing says so. A CI
    // job runs this mode and fails instead of committing the stale file
    if (up_to_date) {
        console.log("opt_tools.cpp is up to date");
    } else {
        console.error("opt_tools.cpp is stale: run `fibjs tools/gen_scripts.js`");
        process.exit(1);
    }
} else if (up_to_date) {
    console.log("opt_tools.cpp is unchanged");
} else {
    fs.writeFile(target, out);
    console.log("opt_tools.cpp written");
}