/**
 * Generate bundled tsc check script with embedded lib.d.ts files
 * 
 * Downloads TypeScript directly from npm registry - no local installation needed.
 * Strips emit/transform/watch/build code to produce a check-only bundle.
 * 
 * Run: fibjs tools/gen_tsc.js
 */

const fs = require('fs');
const path = require('path');
const zlib = require('zlib');
const http = require('http');

const outputFile = path.join(__dirname, '../fibjs/scripts/opt_tools/check.js');

// TypeScript version to bundle. Pinned to the 6.x line on purpose:
// the same-name shadowing patch below (and the check-only bundle format)
// targets the 6.x JS compiler structure. TypeScript 7.x ships a native
// Go compiler whose package layout has no lib/_tsc.js, so it is not
// supported yet. Override with TSC_VERSION if needed.
const TSC_VERSION = process.env.TSC_VERSION || '6.0.2';

// Download and extract TypeScript from npm
function downloadTypeScript() {
    console.log(`Fetching TypeScript ${TSC_VERSION} package info from npm...`);
    
    // Get package info
    const res = http.getSync('https://registry.npmjs.org/typescript/' + TSC_VERSION);
    const pkgInfo = res.json();
    const version = pkgInfo.version;
    const tarballUrl = pkgInfo.dist.tarball;
    
    console.log(`TypeScript version: ${version}`);
    console.log(`Downloading from: ${tarballUrl}`);
    
    // Download tarball
    const tarRes = http.getSync(tarballUrl);
    const tarGz = tarRes.readAll();
    
    console.log(`Downloaded ${tarGz.length} bytes`);
    
    // Extract tarball (tar.gz format)
    const tar = zlib.gunzip(tarGz);
    
    // Parse tar and extract needed files
    const files = {};
    let pos = 0;
    
    while (pos < tar.length) {
        // Tar header is 512 bytes
        if (pos + 512 > tar.length) break;
        
        const header = tar.slice(pos, pos + 512);
        const name = header.slice(0, 100).toString().replace(/\0/g, '').trim();
        
        if (!name) break;
        
        // File size is at offset 124, 12 bytes, octal
        const sizeStr = header.slice(124, 136).toString().replace(/\0/g, '').trim();
        const size = parseInt(sizeStr, 8) || 0;
        
        pos += 512; // Skip header
        
        if (size > 0) {
            const content = tar.slice(pos, pos + size);
            
            // Extract lib/*.d.ts and lib/_tsc.js
            const relativePath = name.replace(/^package\//, '');
            if (relativePath === 'lib/_tsc.js' || 
                (relativePath.startsWith('lib/lib.') && relativePath.endsWith('.d.ts'))) {
                const fileName = path.basename(relativePath);
                files[fileName] = content.toString();
                console.log(`  Extracted: ${fileName} (${content.length} bytes)`);
            }
            
            // Skip to next 512-byte boundary
            pos += Math.ceil(size / 512) * 512;
        }
    }
    
    return { version, files };
}

/**
 * Strip emit/transform/build code from tsc source.
 * These sections are not needed for type-check-only mode.
 * 
 * Removes:
 *   - src/compiler/transformers/* (all JS transforms, ~23200 lines)
 *   - src/compiler/tsbuild.ts, tsbuildPublic.ts (~1737 lines)
 * 
 * Keeps (needed by executeCommandLine / program):
 *   - src/compiler/transformer.ts (getTransformers, referenced by program)
 *   - src/compiler/emitter.ts (emitFiles, getCommonSourceDirectory, etc.)
 *   - src/compiler/watchUtilities.ts (closeFileWatcherOf, etc.)
 *   - src/compiler/watch.ts (createDiagnosticReporter, emitFilesAndReportErrors)
 *   - src/compiler/watchPublic.ts (createWatchProgram, referenced by executeCommandLine)
 */
function stripEmitCode(tscCode) {
    const lines = tscCode.split('\n');
    const totalBefore = lines.length;

    // Sections to strip: [startMarker, endMarker] (endMarker is the NEXT section start)
    // Only strip sections whose exports are NOT referenced by kept code at check-time
    const sectionsToStrip = [
        // All transformer implementations (~23200 lines) - only called during emit
        ['// src/compiler/transformers/utilities.ts', '// src/compiler/transformer.ts'],
        // tsbuild* (~1737 lines) - only called for `tsc -b` mode
        ['// src/compiler/tsbuild.ts', '// src/compiler/tsbuildPublic.ts'],
        ['// src/compiler/tsbuildPublic.ts', '// src/compiler/executeCommandLine.ts'],
    ];

    // Find line indices for each marker
    const markerLines = {};
    for (let i = 0; i < lines.length; i++) {
        const trimmed = lines[i].trim();
        for (const [start, end] of sectionsToStrip) {
            if (trimmed === start) markerLines[start] = i;
            if (trimmed === end) markerLines[end] = i;
        }
    }

    // Build set of lines to remove (mark for removal)
    const removeSet = new Set();
    let totalStripped = 0;
    for (const [startMarker, endMarker] of sectionsToStrip) {
        const startLine = markerLines[startMarker];
        const endLine = markerLines[endMarker];
        if (startLine !== undefined && endLine !== undefined && endLine > startLine) {
            for (let i = startLine; i < endLine; i++) {
                removeSet.add(i);
            }
            const count = endLine - startLine;
            totalStripped += count;
            console.log(`  Stripped: ${startMarker} (${count} lines)`);
        } else {
            console.log(`  WARNING: Could not find section ${startMarker} -> ${endMarker}`);
        }
    }

    const result = [];
    for (let i = 0; i < lines.length; i++) {
        if (!removeSet.has(i)) {
            result.push(lines[i]);
        }
    }

    console.log(`\n  Total stripped: ${totalStripped} lines (${(totalStripped * 100 / totalBefore).toFixed(1)}%)`);
    console.log(`  Lines before: ${totalBefore}, after: ${result.length}`);

    // Add stubs for functions from stripped transformers section that are
    // referenced by the checker at type-check time
    const stubs = `
// Stubs for functions from stripped transformers section (used by checker)
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
    return stubs + result.join('\n');
}

function main() {
    const { version, files } = downloadTypeScript();
    
    let tscCode = files['_tsc.js'];
    if (!tscCode) {
        throw new Error('_tsc.js not found in package');
    }
    
    // Collect lib.d.ts files
    const libFiles = {};
    for (const [name, content] of Object.entries(files)) {
        if (name.startsWith('lib.') && name.endsWith('.d.ts')) {
            libFiles[name] = content;
        }
    }
    
    console.log(`\nPacked ${Object.keys(libFiles).length} lib.d.ts files`);
    
    // Embed lib files as JSON (will be compressed by gen_scripts.js)
    const libJson = JSON.stringify(libFiles);
    console.log(`Lib files total: ${libJson.length} bytes`);

    // Strip emit/transform/watch/build code for check-only mode
    console.log('\nStripping emit/transform/watch/build code...');
    tscCode = stripEmitCode(tscCode);

    // Patch out the same-name shadowing rule: stock tsc drops a .js file from
    // the program when a same-named .ts/.tsx file exists in the project (it
    // assumes the .js is the emitted output of the .ts). fibjs --check runs
    // with --allowJs by default and checks both files, so the shadowing is
    // removed here. The patch neutralizes the two calls in
    // getFileNamesFromConfigSpecs() that implement the priority-extension
    // filtering during tsconfig include collection.
    console.log('Patching out same-name shadowing rule...');
    const shadowingPatch = [
        '      // fibjs patch: skip the same-name shadowing check, so a .js file is',
        '      // still included even when a same-named .ts/.tsx file is in the project',
        '      // (stock tsc treats the .js as the emitted output of the .ts and drops it).',
        '      if (false && hasFileWithHigherPriorityExtension(file, literalFileMap, wildcardFileMap, supportedExtensions, keyMapper)) {',
        '        continue;',
        '      }',
        '      // (removed) removeWildcardFilesWithLowerPriorityExtension(file, wildcardFileMap, supportedExtensions, keyMapper);',
        '      const key = keyMapper(file);',
    ].join('\n');
    const shadowingOriginal = [
        '      if (hasFileWithHigherPriorityExtension(file, literalFileMap, wildcardFileMap, supportedExtensions, keyMapper)) {',
        '        continue;',
        '      }',
        '      removeWildcardFilesWithLowerPriorityExtension(file, wildcardFileMap, supportedExtensions, keyMapper);',
        '      const key = keyMapper(file);',
    ].join('\n');
    if (!tscCode.includes(shadowingOriginal)) {
        console.error('WARNING: could not find same-name shadowing code to patch!');
    }
    else {
        tscCode = tscCode.replace(shadowingOriginal, shadowingPatch);
    }

    // Replace the final executeCommandLine call to force --noEmit, --allowJs
    // and --allowImportingTsExtensions for check mode.
    // NOTE: In fibjs, process.argv is a read-only getter that returns a new array
    // on each access, so we cannot modify it. Instead we inject the flags directly
    // into sys.args before calling executeCommandLine.
    const checkModeFlags = `// Force --noEmit, --allowImportingTsExtensions and --allowJs for check mode
// (fibjs process.argv is a read-only getter, so we patch sys.args directly)
if (!sys.args.some(function(a) { return a === "--noEmit"; })) {
    sys.args.unshift("--noEmit");
}
if (!sys.args.some(function(a) { return a === "--allowImportingTsExtensions"; })) {
    sys.args.unshift("--allowImportingTsExtensions");
}
if (!sys.args.some(function(a) { return a === "--allowJs"; })) {
    sys.args.unshift("--allowJs");
}
executeCommandLine(sys, noop, sys.args);
`;
    tscCode = tscCode.replace(
        'executeCommandLine(sys, noop, sys.args);',
        checkModeFlags
    );

    // Generate the check.js file with header that patches fs module
    const header = `#!/usr/bin/env fibjs
/**
 * TypeScript type checker for fibjs
 * 
 * Bundled TypeScript version: ${version}
 * Generated by tools/gen_tsc.js
 * 
 * TypeScript is licensed under Apache License 2.0
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * 
 * Usage: fibjs --check [options] <files...>
 *
 * The TypeScript checker runs with --allowJs enabled by default, so both
 * .ts and .js files are checked (syntax and type errors). The bundled
 * compiler is patched to skip the same-name shadowing rule (stock tsc
 * drops a .js file when a same-named .ts file exists in the project).
 */

(function() {
"use strict";

const fs = require('fs');

// Embedded lib.d.ts files
const libFiles = ${libJson};

// Helper to extract basename
function getBasename(p) {
    return p.replace(/\\\\/g, '/').split('/').pop();
}

// Patch fs.readFileSync to check embedded lib files first
const originalReadFileSync = fs.readFileSync.bind(fs);
fs.readFileSync = function(path, options) {
    const basename = getBasename(path);
    if (basename && libFiles[basename]) {
        return Buffer.from(libFiles[basename]);
    }
    return originalReadFileSync(path, options);
};

// Patch fs.existsSync to check embedded lib files first  
const originalExistsSync = fs.existsSync.bind(fs);
fs.existsSync = function(path) {
    const basename = getBasename(path);
    if (basename && libFiles[basename]) {
        return true;
    }
    return originalExistsSync(path);
};

// Patch fs.statSync to handle embedded lib files and options parameter
const originalStatSync = fs.statSync.bind(fs);
fs.statSync = function(path, options) {
    const basename = getBasename(path);
    if (basename && libFiles[basename]) {
        // Return a fake stat object for lib files
        return {
            isFile: () => true,
            isDirectory: () => false,
            isSymbolicLink: () => false,
            size: libFiles[basename].length,
            mtime: new Date(),
            mode: 0o644
        };
    }
    // fibjs doesn't support options parameter, handle throwIfNoEntry
    try {
        return originalStatSync(path);
    } catch (e) {
        if (options && options.throwIfNoEntry === false) {
            return undefined;
        }
        throw e;
    }
};

})();

// ============== TypeScript Compiler (check-only) ==============
// The code below is from Microsoft TypeScript, licensed under Apache License 2.0
// https://github.com/microsoft/TypeScript
// Emit/transform/watch/build code has been stripped for check-only mode.

`;

    const checkJs = header + tscCode;
    fs.writeTextFile(outputFile, checkJs);
    console.log(`\nGenerated ${outputFile}`);
    console.log(`Total size: ${checkJs.length} bytes`);
}

main();