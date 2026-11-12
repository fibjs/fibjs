// Regression tests: the hand-written declarations must describe what the
// runtime really exports.
//
// The pure-JS compatibility layer (fibjs/scripts/**) has no IDL, so its
// declarations are hand-written: `fibjs/scripts/**/<name>.d.ts` next to the
// implementation, and `idl/types/module/<unit>.d.ts` for a module with no JS of
// its own. Nothing generates them from the implementation, so a member added to
// the JS without its declaration (or the reverse) goes unnoticed until a TS user
// hits it - `timers/promises.setInterval` and `readline/promises.Interface` were
// both missing that way.
//
// This suite compares the two directions for every such module. It is
// deliberately conservative about what counts as declared: a class declaration
// publishes its prototype and inherited members, and `export =` republishes the
// bound class, so those count too.

var { describe, it } = require('node:test');
var assert = require('assert');
var fs = require('fs');
var path = require('path');

var DTS = path.join(__dirname, '..', 'npm', 'types', 'dts', 'module');
var SCRIPTS = path.join(__dirname, '..', 'fibjs', 'scripts');

// module id -> corpus unit name under npm/types/dts/module
var MODULES = [
    { mod: 'stream', unit: 'stream' },
    { mod: 'stream/promises', unit: 'stream.promises' },
    { mod: 'stream/web', unit: 'stream_web' },
    { mod: 'readline', unit: 'readline' },
    { mod: 'readline/promises', unit: 'readline.promises' },
    { mod: 'timers/promises', unit: 'timers.promises' },
    { mod: 'diagnostics_channel', unit: 'diagnostics_channel' },
    { mod: 'events', unit: 'events' },
    { mod: 'buffer', unit: 'buffer' }
];

// Modules that re-export another module wholesale (`import http = require('http');
// export = http;`): their members are declared by that other module, so listing
// them here would only restate its file. The test asserts the `export =` form is
// kept instead.
var EXPORT_EQUALS_OTHER = {
    https: 'http'
};

function declaredNames(text) {
    var names = {};
    var re = /^[ \t]*(?:export\s+)?(?:declare\s+)?(?:function|const|let|var|class|namespace)\s+([A-Za-z_$][\w$]*)/gm;
    var m;

    while ((m = re.exec(text)) !== null)
        names[m[1]] = true;

    return names;
}

function hasExportEquals(text) {
    return /^[ \t]*export[ \t]*=/m.test(text);
}

// Every own and inherited property name of a class value, plus its prototypes'.
function classSurface(fn) {
    var keys = {};

    if (typeof fn !== 'function')
        return keys;

    for (var o = fn; o && o !== Function.prototype; o = Object.getPrototypeOf(o))
        Object.getOwnPropertyNames(o).forEach(function (k) { keys[k] = true; });

    for (var p = fn.prototype; p; p = Object.getPrototypeOf(p))
        Object.getOwnPropertyNames(p).forEach(function (k) { keys[k] = true; });

    return keys;
}

describe('hand-written declarations match the runtime', () => {
    MODULES.forEach(entry => {
        var mod = entry.mod;
        var unit = entry.unit;

        it(mod + ': every runtime export is declared, and vice versa', () => {
            var file = path.join(DTS, unit + '.d.ts');

            assert.ok(fs.existsSync(file), 'missing corpus file for ' + mod + ': ' + file);

            var text = fs.readTextFile(file);
            var rt = require(mod);
            var runtimeKeys = Object.keys(rt);
            var declared = declaredNames(text);

            // The declaration may publish a class's members rather than list
            // them at the top level, in two ways:
            //   - `export = <class>` republishes the bound class;
            //   - the module object *is* the class (`events`), so its instance
            //     API rides the prototype and the statics are own properties.
            // In both cases the reachable surface is the class chain.
            var surface = (hasExportEquals(text) || typeof rt === 'function')
                ? classSurface(rt)
                : {};

            var runtimeOnly = runtimeKeys.filter(function (k) {
                return !declared[k] && !surface[k];
            }).sort();

            var declaredOnly = Object.keys(declared).filter(function (k) {
                return runtimeKeys.indexOf(k) < 0;
            }).sort();

            assert.deepStrictEqual(runtimeOnly, [],
                mod + ': exported at runtime but not declared (add it to the .d.ts)');
            assert.deepStrictEqual(declaredOnly, [],
                mod + ': declared but not exported at runtime (remove it or add the runtime member)');
        });
    });

    Object.keys(EXPORT_EQUALS_OTHER).forEach(mod => {
        it(mod + ': keeps the re-export form of ' + EXPORT_EQUALS_OTHER[mod], () => {
            var text = fs.readTextFile(path.join(DTS, mod + '.d.ts'));

            assert.ok(hasExportEquals(text),
                mod + ' is expected to declare `export = ' + EXPORT_EQUALS_OTHER[mod] + '`');
        });
    });
});

// The declaration files that describe a pure-JS module must live beside it, so
// that changing the JS and its types stays one edit in one directory.
describe('co-located declarations', () => {
    var CO_LOCATED = [
        'stream.d.ts',
        'stream/promises.d.ts',
        'stream/web.d.ts',
        'readline.d.ts',
        'readline/promises.d.ts',
        'timers/promises.d.ts',
        'diagnostics_channel.d.ts'
    ];

    CO_LOCATED.forEach(rel => {
        it(rel + ' sits next to its implementation and starts with declare module', () => {
            var file = path.join(SCRIPTS, rel);
            var impl = path.join(SCRIPTS, rel.replace(/\.d\.ts$/, '.js'));

            assert.ok(fs.existsSync(file),
                'expected a co-located declaration at ' + file);
            assert.ok(fs.existsSync(impl),
                'a co-located declaration needs its implementation beside it: ' + impl);

            var text = fs.readTextFile(file);

            // The corpus slot is resolved from this line, so a file without it
            // cannot be matched (tools/util/gen_dts.js fails the build on it).
            assert.ok(/^declare module '[^']+'/m.test(text),
                rel + " must start with declare module '<runtime name>'");
        });
    });

    it('no declaration is embedded as a module by gen_scripts', () => {
        // `.d.ts` under fibjs/scripts is a declaration for the type checker, not
        // a module; tools/gen_scripts.js skips it. A regression there registers a
        // bogus `<name>.d` module, which is what this guards.
        var opt = path.join(__dirname, '..', 'fibjs', 'src', 'base', 'opt_tools.cpp');

        if (!fs.existsSync(opt))
            return;

        var text = fs.readTextFile(opt);

        assert.ok(text.indexOf('"stream.d"') < 0 && text.indexOf('"web.d"') < 0 &&
            text.indexOf('"promises.d"') < 0,
            'a .d.ts was packed into opt_tools.cpp as a module (see tools/gen_scripts.js)');
    });
});
