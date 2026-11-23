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
 * The global layer and the prefixed module aliases live in `dts/_builtin/`:
 * tools/util/gen_compat_types.js writes them as part of the corpus, and this
 * tool embeds them verbatim. `"[Symbol.iterator]"` members are normalized
 * here because dts-dom cannot emit computed names.
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

// The global declaration file and the prefixed module aliases are generated
// by tools/util/gen_compat_types.js and read back by buildFiles() below.

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
// idl/types/Buffer.d.ts copied into the corpus by gen_dts).
function normalizeBufferBase(rel, text) {
    if (rel !== BUFFER_FILE)
        return text;

    // The corpus slot now comes from the hand-written idl/types/Buffer.d.ts
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

    // The global layer and the `fibjs:`/`node:` aliases are part of the
    // corpus; tools/util/gen_compat_types.js writes them before this tool
    // runs. Read every file in `_builtin` so a future `nodejs.d.ts` is picked
    // up without another generator change.
    const builtinDir = path.join(dts, '_builtin');

    if (!fs.existsSync(builtinDir))
        throw new Error(`the built-in type overlay is missing: ${builtinDir} (run tools/idlc.js first)`);

    for (const f of fs.readdir(builtinDir).filter(f => f.endsWith('.d.ts')).sort())
        add(rel(path.join(builtinDir, f)), fs.readTextFile(path.join(builtinDir, f)));

    for (const required of [GLOBALS_FILE, PREFIXED_MODULES_FILE])
        if (files[required] === undefined)
            throw new Error(`the built-in type overlay is missing ${required} (run tools/idlc.js first)`);

    const globals = Array.from(
        files[GLOBALS_FILE].matchAll(/^declare var ([A-Za-z_$][A-Za-z0-9_$]*):/gm)
    ).map(m => m[1]);

    return { files, bytesIn, sources, globals };
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
        ' *                  to the program; the same corpus is published as',
        ' *                  @fibjs/types (globals/prefixed generated by',
        ' *                  tools/util/gen_compat_types.js).',
        ' *   fibjs --man    parses the same map, the IDL comments are the manual.',
        ' *',
        ' * source : npm/types (version ' + version + ')',
        ' * files  : ' + sources + ' corpus files (including dts/_builtin/*.d.ts)',
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
        ' corpus files) from ' + stats.bytesIn + ' B, ' + stats.globals + ' fibjs globals');
    console.log('gen_builtin_types: ' + stats.outFile + ' (' + stats.chars + ' chars, ' +
        stats.bytes + ' B, deflated ' + stats.deflated + ' B) in ' + stats.build + ' ms');
}

// run as a tool, but stay silent when idlc.js requires the generator
if (process.argv[1] && path.basename(process.argv[1]) === path.basename(__filename))
    main();

module.exports = { generate, buildFiles, GLOBALS_FILE, PREFIXED_MODULES_FILE };
