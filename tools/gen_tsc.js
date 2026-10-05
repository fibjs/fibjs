#!/usr/bin/env fibjs
/**
 * gen_tsc.js — build the trimmed TypeScript compiler the fibjs checker runs on.
 *
 *   fibjs tools/gen_tsc.js
 *
 * Output (generated, committed):
 *
 *   fibjs/scripts/internal/tsc-libs.js   the trimmed compiler, exported as a
 *                                        module, together with the package's
 *                                        lib.*.d.ts files the checker's host
 *                                        serves as data. It lives under
 *                                        internal/ so it is not part of the
 *                                        `--<command>` lookup (opt_tools/*)
 *                                        and stays out of the module list
 *                                        user code sees.
 *
 * The checker is fibjs/scripts/opt_tools/check.js, written by hand on top of
 * this module: it owns every fibjs specific behavior (embedded built-in types,
 * es-only default lib, project references dispatch, diagnostics filtering,
 * ...). This generator therefore:
 *
 *   - downloads the pinned TypeScript from the npm registry,
 *   - strips the emit/transform/build code (check-only mode does not emit),
 *   - applies the one compiler behavior fibjs needs changed (the same-name
 *     .js/.ts shadowing rule, see patchShadowingRule),
 *   - removes the CLI entry call so the bundle can be require()d,
 *   - exports the compiler API surface check.js uses, plus the lib files.
 *
 * The strip markers and the patch anchor are exact snippets of the pinned
 * release: every one of them is verified, so a version bump fails the
 * generation loudly instead of shipping a broken checker (set TSC_VERSION to
 * bundle another 6.x release).
 *
 * Run this before tools/gen_scripts.js, which embeds fibjs/scripts/** into the
 * binary.
 */

const fs = require('fs');
const path = require('path');
const zlib = require('zlib');
const http = require('http');

const outputFile = path.join(__dirname, '../fibjs/scripts/internal/tsc-libs.js');

// Pinned to the 6.x line: the strip markers and the shadowing patch target that
// compiler structure. 7.x ships a native Go compiler with no lib/_tsc.js.
const TSC_VERSION = process.env.TSC_VERSION || '6.0.2';

// The compiler API surface check.js uses. Every name is verified against the
// generated text, so a TypeScript upgrade that renames or strips one fails the
// generation instead of shipping a broken checker.
const EXPORTS = [
    'version',
    'sys',
    'Diagnostics',
    'parseCommandLine',
    'getParsedCommandLineOfConfigFile',
    'createCompilerHost',
    'createProgram',
    'createSourceFile',
    'getDefaultLibFileName',
    'getLineAndCharacterOfPosition',
    'createCompilerDiagnostic',
    'flattenDiagnosticMessageText',
    'formatDiagnosticsWithColorAndContext',
    'optionDeclarations',
    'printHelp',
    // the compiler's own module resolution: `--check` serves a
    // `resolveModuleNameLiterals` host callback built on these, so the import
    // graph it reports is the one the program was built with. The compiler's
    // own resolved-module iterators (`forEachResolvedModule`,
    // `getResolvedModule`) are not usable here: the first throws on cache
    // entries it did not memoize, the second is a no-op in the trimmed build.
    'resolveModuleName',
    'getModeForUsageLocation',
    // the type-reference side of the same story: `--check` serves a
    // `resolveTypeReferenceDirectiveReferences` host callback built on this,
    // so the node type references (a dependency directive, a `types` entry,
    // the `"*"` scan) can be refused - a checked program never carries
    // @types/node (check.js createHost); every other reference resolves
    // through the compiler.
    'resolveTypeReferenceDirective',
];

function downloadTypeScript() {
    console.log(`Fetching TypeScript ${TSC_VERSION} package info from npm...`);

    const pkgInfo = http.getSync('https://registry.npmjs.org/typescript/' + TSC_VERSION).json();
    const tarballUrl = pkgInfo.dist.tarball;
    console.log(`TypeScript version: ${pkgInfo.version}`);
    console.log(`Downloading ${tarballUrl}...`);

    const tarGz = http.getSync(tarballUrl).readAll();
    console.log(`Downloaded ${tarGz.length} bytes`);

    const tar = zlib.gunzip(tarGz);
    const files = {};
    let pos = 0;

    while (pos + 512 <= tar.length) {
        const header = tar.slice(pos, pos + 512);
        const name = header.slice(0, 100).toString().replace(/\0/g, '').trim();

        if (!name)
            break;

        // File size is at offset 124, 12 bytes, octal
        const size = parseInt(header.slice(124, 136).toString().replace(/\0/g, '').trim(), 8) || 0;

        pos += 512; // skip the header

        if (size > 0) {
            const relativePath = name.replace(/^package\//, '');

            if (relativePath === 'lib/_tsc.js' ||
                (relativePath.startsWith('lib/lib.') && relativePath.endsWith('.d.ts'))) {
                files[path.basename(relativePath)] = tar.slice(pos, pos + size).toString();
            }

            pos += Math.ceil(size / 512) * 512; // skip to the next 512-byte boundary
        }
    }

    return { version: pkgInfo.version, files };
}

/**
 * Strip emit/transform/build code: a check-only checker does not emit, and the
 * transformers/tsbuild sections are the bulk of the compiler.
 *
 * Removes:
 *   - src/compiler/transformers/* (all JS transforms, ~23200 lines)
 *   - src/compiler/tsbuild.ts, tsbuildPublic.ts (~1737 lines)
 *
 * Keeps (referenced by the kept code at check time):
 *   - src/compiler/transformer.ts, emitter.ts, watchUtilities.ts, watch.ts,
 *     watchPublic.ts (createDiagnosticReporter, ...)
 */
function stripEmitCode(tscCode) {
    const lines = tscCode.split('\n');
    const totalBefore = lines.length;

    // Sections to strip: [startMarker, endMarker] (endMarker is the NEXT section start)
    const sectionsToStrip = [
        ['// src/compiler/transformers/utilities.ts', '// src/compiler/transformer.ts'],
        ['// src/compiler/tsbuild.ts', '// src/compiler/tsbuildPublic.ts'],
        ['// src/compiler/tsbuildPublic.ts', '// src/compiler/executeCommandLine.ts'],
    ];

    const markerLines = {};
    for (let i = 0; i < lines.length; i++) {
        const trimmed = lines[i].trim();
        for (const [start, end] of sectionsToStrip) {
            if (trimmed === start) markerLines[start] = i;
            if (trimmed === end) markerLines[end] = i;
        }
    }

    const removeSet = new Set();
    let totalStripped = 0;

    for (const [startMarker, endMarker] of sectionsToStrip) {
        const startLine = markerLines[startMarker];
        const endLine = markerLines[endMarker];

        if (startLine === undefined || endLine === undefined || endLine < startLine)
            throw new Error(`cannot strip ${startMarker}: marker(s) not found in the bundled tsc`);

        for (let i = startLine; i < endLine; i++)
            removeSet.add(i);

        totalStripped += endLine - startLine;
        console.log(`  Stripped: ${startMarker} (${endLine - startLine} lines)`);
    }

    const kept = lines.filter((_, i) => !removeSet.has(i));

    console.log(`\n  Total stripped: ${totalStripped} lines (${(totalStripped * 100 / totalBefore).toFixed(1)}%)`);
    console.log(`  Lines before: ${totalBefore}, after: ${kept.length}`);

    // Stubs for functions from the stripped sections that the kept code still
    // references at type-check time.
    const stubs = `
// Stubs for functions from stripped sections (referenced by the kept code)
function isCompoundAssignment(kind) {
  return kind >= 65 && kind <= 79;
}
function isInitializedProperty(member) {
  return member.kind === 173 && member.initializer !== void 0;
}
function classHasDeclaredOrExplicitlyAssignedName(node) {
  return !!node.name;
}
function getDeclarationDiagnostics(host, resolver, file) {
  return emptyArray;
}
`;
    return stubs + kept.join('\n');
}

/**
 * Patch out the same-name shadowing rule: stock tsc drops a .js file from the
 * program when a same-named .ts/.tsx file exists in the project (it assumes the
 * .js is the emitted output of the .ts). fibjs --check runs with --allowJs by
 * default and checks both files, so the shadowing is removed here. The patch
 * neutralizes the two calls in getFileNamesFromConfigSpecs() that implement the
 * priority-extension filtering during tsconfig include collection.
 */
function patchShadowingRule(tscCode) {
    console.log('Patching out the same-name shadowing rule...');

    const original = [
        '      if (hasFileWithHigherPriorityExtension(file, literalFileMap, wildcardFileMap, supportedExtensions, keyMapper)) {',
        '        continue;',
        '      }',
        '      removeWildcardFilesWithLowerPriorityExtension(file, wildcardFileMap, supportedExtensions, keyMapper);',
        '      const key = keyMapper(file);',
    ].join('\n');
    const patched = [
        '      // fibjs patch: skip the same-name shadowing check, so a .js file is',
        '      // still included even when a same-named .ts/.tsx file is in the project',
        '      // (stock tsc treats the .js as the emitted output of the .ts and drops it).',
        '      if (false && hasFileWithHigherPriorityExtension(file, literalFileMap, wildcardFileMap, supportedExtensions, keyMapper)) {',
        '        continue;',
        '      }',
        '      // (removed) removeWildcardFilesWithLowerPriorityExtension(file, wildcardFileMap, supportedExtensions, keyMapper);',
        '      const key = keyMapper(file);',
    ].join('\n');

    if (!tscCode.includes(original))
        throw new Error('cannot patch the same-name shadowing rule: anchor not found in the bundled tsc');

    return tscCode.replace(original, patched);
}

/**
 * The bundle ends with the tsc CLI entry call. check.js drives the compiler
 * itself, so requiring this module must not run a CLI: the call is replaced.
 */
function neutraliseCliEntry(tscCode) {
    const anchor = 'executeCommandLine(sys, noop, sys.args);';

    if (!tscCode.includes(anchor))
        throw new Error('cannot neutralise the tsc CLI entry: anchor not found in the bundled tsc');

    return tscCode.replace(
        anchor,
        '// fibjs: the CLI entry is not called; check.js drives the compiler as a library.'
    );
}

function verifyExports(tscCode) {
    EXPORTS.forEach(name => {
        const definition = new RegExp(
            '(^|\\n)(function ' + name + '\\(|var ' + name + ' =|const ' + name + ' =|let ' + name + ' =)'
        );

        if (!definition.test(tscCode))
            throw new Error('cannot export ' + name + ': not defined in the trimmed tsc');
    });
}

function main() {
    const { version, files } = downloadTypeScript();

    let tscCode = files['_tsc.js'];
    if (!tscCode)
        throw new Error('lib/_tsc.js not found in the package');

    const libs = {};
    for (const name of Object.keys(files)) {
        if (name !== '_tsc.js')
            libs[name] = files[name];
    }

    const libBytes = Object.keys(libs).reduce((n, name) => n + libs[name].length, 0);
    console.log(`\nPacked ${Object.keys(libs).length} lib.d.ts files (${libBytes} bytes)`);

    console.log('\nStripping emit/transform/watch/build code...');
    tscCode = stripEmitCode(tscCode);
    tscCode = patchShadowingRule(tscCode);
    tscCode = neutraliseCliEntry(tscCode);
    verifyExports(tscCode);

    const out = [];
    out.push(`/*!
 * tsc-libs.js — TypeScript ${version}, trimmed for fibjs --check, plus the
 * lib.*.d.ts files the checker serves.
 *
 * Generated by tools/gen_tsc.js — do not edit.
 *
 * TypeScript is licensed under Apache License 2.0
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * https://github.com/microsoft/TypeScript
 *
 * The compiler is exported as \`tsc\`: the API surface
 * fibjs/scripts/opt_tools/check.js drives. The lib files are exported as
 * \`libs\`, keyed by file name, as the data a compiler host serves.
 *
 * Emit/transform/watch/build code is stripped and the CLI entry is removed, so
 * requiring this module only loads the compiler.
 */

"use strict";
`);
    out.push(tscCode);
    out.push('\n// fibjs: the compiler API surface check.js uses, plus the lib files.\nmodule.exports = {\n  tsc: {\n');
    out.push(EXPORTS.map(name => '    ' + name + ': ' + name).join(',\n'));
    out.push('\n  },\n  libs: {\n');
    out.push(Object.keys(libs).sort()
        .map(name => '    ' + JSON.stringify(name) + ': ' + JSON.stringify(libs[name]))
        .join(',\n'));
    out.push('\n  }\n};\n');

    const content = out.join('');

    if (!fs.existsSync(path.dirname(outputFile)))
        fs.mkdirSync(path.dirname(outputFile), { recursive: true });

    fs.writeTextFile(outputFile, content);
    console.log(`\nWrote ${outputFile} (${content.length} bytes)`);
}

main();
