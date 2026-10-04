#!/usr/bin/env fibjs
/*
 * gen_builtin_types.js
 *
 * Build-time generator for the built-in types the runtime carries.
 *
 *   fibjs tools/gen_builtin_types.js [corpus-dir] [out-file]
 *
 * Defaults:  corpus  npm/types       (the committed @fibjs/types corpus)
 *            out     fibjs/scripts/internal/fibjs-types.js
 *
 * The generated module is ONE payload in the format the checker consumes -
 * a map of virtual path to file text, the same shape as the bundled lib.d.ts
 * files of check.js:
 *
 *   module.exports = { version, files: { 'dts/module/http.d.ts': '...', ... } }
 *
 * `fibjs --check` serves the map through its fs layer under
 * <cwd>/node_modules/@fibjs/types/ and attaches the files to the program;
 * that is the interface tsc needs, so the corpus travels as the very .d.ts
 * text tsc reads.
 *
 * `fibjs --man` reads the SAME map: the IDL comments are the manual, so the
 * payload keeps them and both commands are guaranteed to describe the same
 * version of the runtime. man.js parses the few files a query needs.
 *
 * Two generated additions to the corpus:
 *   - `dts/_builtin/globals.d.ts` carries the fibjs globals the default lib
 *     does not provide (`declare const X: typeof import('global').X`) plus
 *     the `Buffer` type alias; it and `dts/module/global.d.ts` are dropped at
 *     read time when the project loads @types/node, whose globals must win.
 *   - `"[Symbol.iterator]"` string members are unquoted: dts-dom cannot emit
 *     computed names and the iteration protocols need the real ones.
 *
 * Run this before tools/gen_scripts.js, which embeds the generated module as
 * `internal/fibjs-types`. tools/idlc.js calls it after regenerating the
 * corpus, and fibjs/scripts/opt_tools/man.js plus the `--check` wrapper read
 * it at run time.
 */

const fs = require('fs');
const path = require('path');
const zlib = require('zlib');

const GLOBALS_FILE = 'dts/_builtin/globals.d.ts';
const PREFIXED_MODULES_FILE = 'dts/_builtin/prefixed-modules.d.ts';
const BUFFER_FILE = 'dts/interface/Buffer.d.ts';

// The files that provide the globals: dropped when the project has @types/node.
// The prefixed modules go with them: an @types/node project resolves `node:fs`
// to the node definitions, and two ambient modules of one name would merge.
const GLOBALS_SOURCES = ['dts/module/global.d.ts', GLOBALS_FILE, PREFIXED_MODULES_FILE];

// Globals the *es* lib chain declares as values (`globalThis`); everything else
// the `global` module exports is declared below with the fibjs type. The
// checker's default lib is es-only on purpose: lib.dom would shadow WebSocket,
// URL, fetch, console, ... with browser types that do not match the runtime
// (plans/idl-event-types-2026-10-03.md §10).
const LIB_PROVIDED = new Set([
    'globalThis',
]);

// ---------------------------------------------------------------------------
// corpus reading / normalizing
// ---------------------------------------------------------------------------

function corpusVersion(corpusDir) {
    try {
        return JSON.parse(fs.readTextFile(path.join(corpusDir, 'package.json'))).version || '';
    } catch (e) {
        return '';
    }
}

// dts-dom cannot emit computed member names, so `[Symbol.iterator]` members
// arrive as the *string* member "[Symbol.iterator]", which does not satisfy
// the iteration protocols. Unquote them.
function normalizeSymbolMembers(text) {
    return text
        .replace(/"\[Symbol\.(iterator|asyncIterator)\]"/g, '[Symbol.$1]')
        .replace(/"\[Symbol\.asynclterator\]"/g, '[Symbol.asyncIterator]');
}

// The IDL declares Buffer as a bare object, but the runtime Buffer *is* a
// Uint8Array: Buffer.from('x') instanceof Uint8Array is true and the prototype
// chain goes through Uint8Array.prototype. Align the base class, so the
// typed-array surface (buffer/byteOffset/length/subarray/map/filter/...,
// assignments to and from Uint8Array) typechecks.
//
// Overriding a typed-array member incompatibly would make the class - and
// every assignment of a Buffer to a Uint8Array - invalid. TypeScript checks
// each overriding declaration against the base signature (parameters
// bivariantly, and a base member returning `this` must be overridden with
// `this`), so the members the corpus still shapes differently are rewritten
// into base-compatible forms:
//
//   slice    (start) + (start, end) -> (start?, end?)
//   toString (codec, offset?, end?) with a *required* codec, which the base's
//            toString() rejects -> codec optional
//
// `fill` needs no handling: it is not an IDL method at all (the native
// overloads were removed from idl/Buffer.idl and Buffer.cpp), the number form
// comes from Uint8Array and the string/buffer forms are implemented by the JS
// layer (fibjs/scripts/internal/buffer.js, `class Buffer extends Uint8Array`).
//
// A no-op once the corpus declares the base itself (the hand-written
// tools/handwritten/Buffer.d.ts copied into the corpus by gen_dts).
function normalizeBufferBase(rel, text) {
    if (rel !== BUFFER_FILE)
        return text;

    // The corpus slot now comes from the hand-written tools/handwritten/Buffer.d.ts
    // (copied by tools/util/gen_dts.js), which already declares the Uint8Array base
    // and base-compatible members. The reshaping below only applies to a legacy
    // generated corpus; each step is skipped when its input is not present.
    const collapse = (from, to) => {
        if (text.indexOf(from[0] + '\n') < 0)
            return;

        text = text.split(from[0] + '\n').join(to + '\n');

        for (const line of from.slice(1))
            text = text.split(line + '\n').join('');
    };

    text = text.replace(/^declare class Class_Buffer extends Class_object \{$/m,
        'declare class Class_Buffer extends Uint8Array {');

    collapse([
        '    slice(start?: number): Class_Buffer;',
        '    slice(start: number, end: number): Class_Buffer;'
    ], '    slice(start?: number, end?: number): Class_Buffer;');

    collapse([
        '    toString(codec: string, offset?: number, end?: number): string;',
        '    toString(codec: string, offset?: number): string;'
    ], '    toString(codec?: string, offset?: number, end?: number): string;');

    return text;
}

// everything the check and the manual see goes through here, so both agree
function normalizeCorpusFile(rel, text) {
    return normalizeBufferBase(rel, normalizeSymbolMembers(text));
}

// fibjs globals: everything the 'global' module exports that the default lib
// does not already declare, plus the CommonJS wrapper names and the type
// aliases of the class-valued ones.
//
// `declare var` (not `const`): a global declared with `var` is a property of
// `globalThis`, which is how the runtime installs it - `globalThis.fetch` has
// to resolve. The value is still the runtime's (the checker never emits, so
// nothing can assign it).
//
// A global whose value is a fibjs class also owns the *type* name, and the type
// is an `interface ... extends Class_X`, not a `type X = Class_X` alias: a
// project (or a dependency such as @types/react) that declares a placeholder
// `interface X {}` merges with an interface - the alias would be a duplicate
// identifier and take the value down with it (TS2451, `X` no longer usable as a
// value). See plans/idl-event-types-2026-10-03.md §22.
function globalsFile(globalDts) {
    const exported = new Set();

    for (const m of globalDts.matchAll(/^\s*(?:const|function)\s+([A-Za-z_$][A-Za-z0-9_$]*)\s*[:<(]/gm))
        exported.add(m[1]);

    const kept = Array.from(exported).filter(n => !LIB_PROVIDED.has(n));

    const classAliases = [];
    for (const m of globalDts.matchAll(/^\s*const\s+([A-Za-z_$][A-Za-z0-9_$]*)\s*:\s*typeof\s+(Class_[A-Za-z0-9_$]+)\s*;/gm))
        if (kept.indexOf(m[1]) >= 0)
            classAliases.push(`interface ${m[1]} extends ${m[2]} {}`);

    return {
        kept: kept,
        aliased: classAliases.length,
        text: [
            '// fibjs globals, generated by tools/gen_builtin_types.js.',
            '// Dropped at read time when the project loads @types/node.',
            '',
            kept.map(n => `declare var ${n}: typeof import('global').${n};`).join('\n'),
            '',
            '// the class-valued globals also own their type name; an interface\n' +
            '// merges with the placeholders a project may declare',
            classAliases.join('\n'),
            '',
            '// fibjs specific globals without an IDL declaration',
            'declare function gc(): void;',
            '',
            '// the `import.meta` members the module loader installs',
            '// (fibjs/src/base/v8_api/utils.cpp: initImportMeta)',
            'interface ImportMeta {',
            '    url: string;',
            '    dirname: string;',
            '    filename: string;',
            '}',
            '',
            'declare const __filename: string;',
            'declare const __dirname: string;',
            'declare var module: { exports: any };',
            'declare var exports: any;',
            ''
        ].join('\n')
    };
}

/**
 * The runtime registers every root module under its own name plus the `fibjs:`
 * and `node:` prefixes - `SandBox::installRootModules()` - and the Node compat
 * layer adds the rest (`node:module`, `node:buffer`, `node:stream`, ...). The
 * corpus mirrors that with a re-exporting environment module per module whose
 * prefixed spelling resolves, so the Node-flavoured form typechecks:
 *
 *     import fs from 'node:fs/promises';
 *
 * Which names qualify is decided by asking the runtime itself: the generator
 * runs inside fibjs, so `require('node:<name>')` is the same question the user
 * asks. The IMPORT_MODULE list of fibjs/src/base/modules.cpp is not enough -
 * `module`, `buffer`, `stream` and `events` come from the compat layer.
 */
function prefixedModuleSupported(name) {
    try {
        require('node:' + name);
        return true;
    } catch (e) {
        return false;
    }
}

function prefixedModulesFile(files) {
    const aliases = [];

    Object.keys(files)
        .filter(rel => /^dts\/module\/[^/]+\.d\.ts$/.test(rel))
        .sort()
        .forEach(rel => {
            const declared = /^declare module '([^']+)'/m.exec(files[rel]);

            if (!declared)
                return;

            const name = declared[1];

            if (!prefixedModuleSupported(name))
                return;

            // An `export =` module (the callable ones: test, assert, ...) cannot
            // be re-exported with `export *` (TS2498); bind and re-export the
            // entity itself - the shape @types/node gives `node:assert`.
            if (/^\s*export\s*=/m.test(files[rel])) {
                // the local binding needs an identifier: `assert/strict` binds
                // as `assert_strict`
                const bind = name.replace(/[^\w$]/g, '_');

                if (!/^[A-Za-z_$][\w$]*$/.test(bind))
                    throw new Error(`the export = module '${name}' needs an identifier to re-export under fibjs:/node:`);

                aliases.push(`declare module "fibjs:${name}" { import ${bind} = require("${name}"); export = ${bind}; }`);
                aliases.push(`declare module "node:${name}" { import ${bind} = require("${name}"); export = ${bind}; }`);
                return;
            }

            aliases.push(`declare module "fibjs:${name}" { export * from "${name}"; }`);
            aliases.push(`declare module "node:${name}" { export * from "${name}"; }`);
        });

    return {
        count: aliases.length,
        text: [
            '// fibjs: / node: aliases of the root modules, generated by tools/gen_builtin_types.js.',
            '// The runtime registers them in SandBox::installRootModules(); dropped at read time',
            '// when the project loads @types/node (the node definitions own the prefixed names then).',
            '',
            aliases.join('\n'),
            ''
        ].join('\n')
    };
}

function buildFiles(corpusDir) {
    const dts = path.join(corpusDir, 'dts');
    const files = {};
    let bytesIn = 0;
    let sources = 0;

    const add = (rel, text) => {
        files[rel] = normalizeCorpusFile(rel, text);
        bytesIn += text.length;
        sources++;
    };

    const rel = p => 'dts/' + path.relative(dts, p).replace(/\\/g, '/');

    add(rel(path.join(dts, '_import/_fibjs.d.ts')), fs.readTextFile(path.join(dts, '_import/_fibjs.d.ts')));

    for (const tag of ['interface', 'module'])
        for (const f of fs.readdir(path.join(dts, tag)).filter(f => f.endsWith('.d.ts')).sort())
            add(rel(path.join(dts, tag, f)), fs.readTextFile(path.join(dts, tag, f)));

    const globals = globalsFile(files['dts/module/global.d.ts']);

    files[GLOBALS_FILE] = globals.text;

    const prefixed = prefixedModulesFile(files);

    files[PREFIXED_MODULES_FILE] = prefixed.text;

    return { files, bytesIn, sources, globals: globals.kept, aliases: prefixed.count };
}

// ---------------------------------------------------------------------------
// generate
// ---------------------------------------------------------------------------

// The payload is written out with real newlines so the repository copy stays
// greppable and diffable: every corpus file is emitted verbatim inside a
// template literal. The embedded copy is deflated by tools/gen_scripts.js, so
// the formatting costs no binary size. Only the sequences that would end or
// interpolate the literal are escaped.
function escapeTemplateText(text) {
    return text.replace(/[\\`]|\$\{/g, (m) => '\\' + m);
}

function formatPayload(version, files) {
    const entries = Object.keys(files).map((rel) =>
        '        ' + JSON.stringify(rel) + ': `' + escapeTemplateText(files[rel]) + '`,');

    return [
        '{',
        '    version: ' + JSON.stringify(version) + ',',
        '    files: {',
        entries.join('\n'),
        '    }',
        '}'
    ].join('\n');
}

function generate(corpusDir, outFile) {
    corpusDir = corpusDir || path.join(__dirname, '../npm/types');
    outFile = outFile || path.join(__dirname, '../fibjs/scripts/internal/fibjs-types.js');

    const t0 = Date.now();
    const version = corpusVersion(corpusDir);
    const { files, bytesIn, sources, globals } = buildFiles(corpusDir);
    const tBuild = Date.now() - t0;

    const payload = formatPayload(version, files);
    const bytes = Buffer.from(payload);

    fs.writeFile(outFile, [
        '/*',
        ' * fibjs-types.js -- generated by tools/gen_builtin_types.js, do not edit.',
        ' *',
        ' * The built-in types of the runtime, from the @fibjs/types corpus, in the',
        ' * format the checker consumes: a map of virtual path to .d.ts text.',
        ' *',
        ' * The corpus files are emitted verbatim inside template literals (real',
        ' * newlines), so this repository copy stays greppable and diffable; the',
        ' * embedded copy is deflated by tools/gen_scripts.js anyway.',
        ' *',
        ' *   fibjs --check  serves the map under',
        ' *                  <cwd>/node_modules/@fibjs/types/ and attaches the files',
        ' *                  to the program (' + GLOBALS_SOURCES.join(', ') + ' are',
        ' *                  dropped when the project loads @types/node).',
        ' *   fibjs --man    parses the same map, the IDL comments are the manual.',
        ' *',
        ' * source : npm/types (version ' + version + ')',
        ' * files  : ' + sources + ' corpus files + ' + GLOBALS_FILE,
        ' * globals: ' + globals.join(' '),
        ' */',
        'module.exports = ' + payload + ';',
        ''
    ].join('\n'));

    return {
        version: version,
        chars: payload.length,
        bytes: bytes.length,
        deflated: zlib.deflate(bytes).length,
        files: Object.keys(files).length,
        sources: sources,
        bytesIn: bytesIn,
        globals: globals.length,
        build: tBuild,
        outFile: outFile
    };
}

function main() {
    const stats = generate(process.argv[2], process.argv[3]);

    console.log('gen_builtin_types: ' + stats.files + ' files (' + stats.sources +
        ' corpus + globals) from ' + stats.bytesIn + ' B, ' + stats.globals + ' fibjs globals');
    console.log('gen_builtin_types: ' + stats.outFile + ' (' + stats.chars + ' chars, ' +
        stats.bytes + ' B, deflated ' + stats.deflated + ' B) in ' + stats.build + ' ms');
}

// run as a tool, but stay silent when idlc.js requires the generator
if (process.argv[1] && path.basename(process.argv[1]) === path.basename(__filename))
    main();

module.exports = { generate, buildFiles, GLOBALS_FILE, PREFIXED_MODULES_FILE, GLOBALS_SOURCES };
