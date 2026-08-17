/*
 * test.js
 *
 * fibjs --test support.
 *
 * Runs test files with the built-in `test` module, following the
 * conventions of `node --test`:
 *
 *   fibjs --test                          discover and run test files
 *   fibjs --test <file|dir|glob> ...      run the given files / directories
 *                                         (directories are searched with the
 *                                         default test-file patterns)
 *
 * Supported options (mirroring node):
 *
 *   --test-name-pattern=<regex>           (not supported yet, ignored with a
 *                                         warning)
 *   --test-skip-pattern=<regex>           (not supported yet, ignored with a
 *                                         warning)
 *   --test-reporter=<name>                only the default spec-like reporter
 *                                         is supported; other names are
 *                                         ignored with a warning
 *   --test-reporter-destination=<dest>    only stdout is supported
 *   --test-concurrency=<n>                accepted, execution is sequential
 *   --test-timeout=<ms>                   (not supported yet, ignored with a
 *                                         warning)
 *   --test-isolation=<mode>               accepted, files run in-process
 *
 * Test files are loaded in-process, one after another, exactly as if each
 * file had been run on its own (no wrapping suite is added). The built-in
 * test module runs all collected cases when the script finishes. The process
 * exits with code 1 if any test fails.
 */

'use strict';

const test = require('test');
const fs = require('fs');
const path = require('path');

// When this file is the opt_tools entry (fibjs --test), __filename is the
// embedded pseudo path "opt_tools/test.cjs", which does not exist on disk.
// If this file is discovered as a test file itself (its name matches
// **/test.js) and loaded from disk through run(), main() must not run again.
const isTestRunnerEntry = !fs.exists(__filename);

// ---------------------------------------------------------------------------
// Test file discovery (node --test compatible)
// ---------------------------------------------------------------------------

// Extensions considered by the default patterns (node: js/mjs/cjs).
const TEST_EXTENSIONS = ['js', 'mjs', 'cjs'];

// node's default glob is:
//   **/{test,test/**,test-*,*[._-]test}.{js,mjs,cjs}
// expanded here into a flat pattern list for fs.glob.
function defaultPatterns() {
    const patterns = [];

    for (const ext of TEST_EXTENSIONS) {
        patterns.push(
            `**/*.test.${ext}`,
            `**/*-test.${ext}`,
            `**/*_test.${ext}`,
            `**/test-*.${ext}`,
            `**/test.${ext}`,
            `**/test/**/*.${ext}`
        );
    }

    return patterns;
}

function hasGlobMagic(pattern) {
    return /[*?[\]{}()!+@]/.test(pattern);
}

function isDirectory(p) {
    try {
        return fs.stat(p).isDirectory();
    } catch (e) {
        return false;
    }
}

// Exclude anything under a node_modules directory (at any depth), like node.
function isInNodeModules(abs) {
    return abs.split(/[\\/]/).some(seg => seg === 'node_modules');
}

// node's test runner globs with exclude: (name) => name === 'node_modules',
// pruning the walk. fibjs glob supports exclude patterns, so prune here too.
const GLOB_OPTS = {
    exclude: ['**/node_modules']
};

function globFiles(pattern, cwd) {
    const opts = Object.assign({ cwd }, GLOB_OPTS);

    return fs.glob(pattern, opts);
}

// Expand a user supplied pattern into absolute test file paths.
// Files are always included (like node, explicitly named files run even when
// their name does not match the test-file conventions). Directories are
// searched recursively with the default patterns.
function expandUserPattern(pattern, cwd) {
    const files = [];

    let matches;
    try {
        matches = globFiles(pattern, cwd);
    } catch (e) {
        matches = [];
    }

    // fs.glob does not match "." (or paths ending in "/") by itself, so fall
    // back to treating an existing directory as a search root.
    if (!matches.length) {
        const dir = path.resolve(cwd, pattern);

        if (isDirectory(dir)) {
            collectFromDirectory(dir, files);
            return files;
        }
    }

    for (const m of matches) {
        const abs = path.resolve(cwd, m);

        if (isInNodeModules(abs))
            continue;

        if (isDirectory(abs)) {
            collectFromDirectory(abs, files);
        } else {
            files.push(abs);
        }
    }

    return files;
}

// Recursively collect test files from a directory using the default patterns.
function collectFromDirectory(dir, files) {
    let matches;
    try {
        matches = globFiles(defaultPatterns(), dir);
    } catch (e) {
        return;
    }

    for (const m of matches) {
        const abs = path.resolve(dir, m);

        if (isInNodeModules(abs))
            continue;

        if (isDirectory(abs))
            collectFromDirectory(abs, files);
        else
            files.push(abs);
    }
}

function buildTestFileList(patterns, cwd) {
    const hasUserSuppliedPattern = patterns.length > 0;

    if (!hasUserSuppliedPattern) {
        const files = [];

        collectFromDirectory(cwd, files);
        return Array.from(new Set(files)).sort();
    }

    const files = [];
    const noMagic = [];
    const missing = [];

    for (const p of patterns) {
        if (!hasGlobMagic(p))
            noMagic.push(p);

        const matched = expandUserPattern(p, cwd);

        if (!matched.length && !hasGlobMagic(p))
            missing.push(p);

        files.push(...matched);
    }

    const unique = Array.from(new Set(files)).sort();

    // node errors out when user supplied patterns match nothing and none of
    // them contain glob magic.
    if (unique.length === 0 && noMagic.length === patterns.length) {
        console.error(`Could not find '${patterns.join(', ')}'`);
        process.exit(1);
    }

    // node silently ignores patterns that match nothing when others match.
    // Warn about them instead, so a typo like a missing path is not hidden
    // (the exit code stays node-compatible).
    for (const p of missing) {
        if (isDirectory(path.resolve(cwd, p)))
            console.error(`No test files found in '${p}'`);
        else
            console.error(`Could not find '${p}'`);
    }

    return unique;
}

// ---------------------------------------------------------------------------
// Command line parsing
// ---------------------------------------------------------------------------

// options that take a value (--opt=value or --opt value)
const VALUE_OPTIONS = [
    'test-name-pattern',
    'test-skip-pattern',
    'test-reporter',
    'test-reporter-destination',
    'test-concurrency',
    'test-timeout',
    'test-isolation',
    'test-global-setup',
    'test-rerun-failures'
];

// boolean options
const BOOL_OPTIONS = [
    'test-force-exit',
    'test-coverage',
    'test-watch'
];

// options that are recognized but not supported: warn once and continue
const UNSUPPORTED = {
    'test-name-pattern': 'test name filtering is not supported yet',
    'test-skip-pattern': 'test skip filtering is not supported yet',
    'test-timeout': 'per-test timeouts are not supported yet',
    'test-coverage': 'coverage is not supported by the test runner (use --cov)',
    'test-watch': 'watch mode is not supported yet',
    'test-global-setup': 'global setup is not supported yet',
    'test-rerun-failures': 'rerun failures is not supported yet'
};

function parseArgs(argv) {
    const opts = {
        patterns: [],
        reporter: 'spec',
        reporterDestination: 'stdout',
        forceExit: false,
        concurrency: undefined,
        timeout: undefined,
        isolation: undefined,
        warnings: []
    };

    for (let i = 0; i < argv.length; i++) {
        const arg = argv[i];

        if (arg === '--test' || arg === '--')
            continue;

        if (arg === '--help' || arg === '-h') {
            printUsage();
            process.exit(0);
        }

        if (arg.startsWith('--')) {
            let name = arg.slice(2);
            let value;

            const eq = name.indexOf('=');
            if (eq >= 0) {
                value = name.slice(eq + 1);
                name = name.slice(0, eq);
            }

            if (VALUE_OPTIONS.indexOf(name) >= 0) {
                if (value === undefined) {
                    if (i + 1 >= argv.length) {
                        console.error(`--${name} requires a value`);
                        process.exit(1);
                    }
                    value = argv[++i];
                }

                applyValueOption(opts, name, value);
            } else if (BOOL_OPTIONS.indexOf(name) >= 0) {
                if (value !== undefined) {
                    console.error(`--${name} does not take a value`);
                    process.exit(1);
                }

                applyBoolOption(opts, name);
            } else {
                opts.warnings.push(`unknown option '--${name}' (ignored)`);
            }
        } else {
            opts.patterns.push(arg);
        }
    }

    return opts;
}

function applyValueOption(opts, name, value) {
    switch (name) {
        case 'test-reporter':
            opts.reporter = value;
            break;
        case 'test-reporter-destination':
            opts.reporterDestination = value;
            break;
        case 'test-concurrency':
            opts.concurrency = parseInt(value, 10);
            if (isNaN(opts.concurrency) || opts.concurrency < 1)
                opts.warnings.push(`invalid value for --test-concurrency: '${value}'`);
            break;
        case 'test-timeout':
            opts.timeout = parseInt(value, 10);
            if (isNaN(opts.timeout) || opts.timeout < 0)
                opts.warnings.push(`invalid value for --test-timeout: '${value}'`);
            break;
        case 'test-isolation':
            opts.isolation = value;
            break;
        default:
            break;
    }

    if (UNSUPPORTED[name])
        opts.warnings.push(`--${name} is not supported (${UNSUPPORTED[name]}); ignoring`);
}

function applyBoolOption(opts, name) {
    switch (name) {
        case 'test-force-exit':
            opts.forceExit = true;
            break;
        default:
            break;
    }

    if (UNSUPPORTED[name])
        opts.warnings.push(`--${name} is not supported (${UNSUPPORTED[name]}); ignoring`);
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------

function printUsage() {
    console.log(`Usage: fibjs --test [options] [files|dirs|globs]

Run test files with the built-in test module.

  fibjs --test                    discover and run test files matching the
                                  default patterns (**/*.test.js,
                                  **/test-*.js, **/test.js, **/*_test.js,
                                  **/test/**/*.js, and the .mjs/.cjs
                                  variants)
  fibjs --test <path>...          run the given files; directories are
                                  searched recursively for test files;
                                  glob patterns are also accepted

Options:
  --test-reporter=<name>          reporter name (only 'spec' is supported)
  --test-reporter-destination=<d> reporter destination (only 'stdout')
  --test-concurrency=<n>          accepted for compatibility; files run
                                  sequentially
  --test-isolation=<mode>         accepted for compatibility; files run
                                  in-process
`);
}

function main() {
    if (!isTestRunnerEntry)
        return;

    const argv = process.argv.slice(1);
    const opts = parseArgs(argv);

    if (opts.reporter !== 'spec') {
        opts.warnings.push(
            `--test-reporter=${opts.reporter} is not supported; using the default reporter`
        );
    }

    if (opts.reporterDestination !== 'stdout') {
        opts.warnings.push(
            `--test-reporter-destination=${opts.reporterDestination} is not supported; using stdout`
        );
    }

    for (const w of opts.warnings)
        console.error(`warning: ${w}`);

    const cwd = process.cwd();
    const files = buildTestFileList(opts.patterns, cwd);

    const runFile = (typeof run === 'function') ? run : global.run;

    // Load the test files directly, without wrapping them in a suite, so the
    // tests register exactly as if each file had been run on its own.
    for (const f of files) {
        try {
            runFile(f);
        } catch (e) {
            // The file failed to load: register a failing case so the run is
            // reported as failed and the process exits with code 1.
            test.it(`${f} failed to load`, () => {
                throw e;
            });
        }
    }
}

main();
