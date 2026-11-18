var { describe, it, after } = require('node:test');
var assert = require('assert');
const child_process = require('child_process');
const fs = require('fs');
const os = require('os');
const path = require('path');

// The same body runs the fibjs CLI under fibjs and `npx` under node, so the
// expectations below are the npx contract in both runtimes:
//   fibjs <command>   vs   npx --offline --no-install <command>
// Every fixture is local; --offline keeps npx from touching the registry and
// --cache points at a scratch directory, so the suite never writes ~/.npm/_npx.
const isFibjs = !!(process.versions && process.versions.fibjs);
const isWindows = process.platform === 'win32';

const FIXTURES = path.join(__dirname, 'npx_files');
const PROJECT = path.join(FIXTURES, 'project');
const DEEP = path.join(PROJECT, 'sub', 'deep');
const OWN = path.join(FIXTURES, 'own');
const OWN_DEEP = path.join(OWN, 'sub', 'deep');
const STRING_BIN = path.join(FIXTURES, 'stringbin', 'sub', 'deep');
const BINPATH_DEEP = path.join(FIXTURES, 'binpath', 'sub', 'deep');
const ARRAYBIN = path.join(FIXTURES, 'arraybin');
const PKGNAME_DEEP = path.join(FIXTURES, 'pkgname', 'sub', 'deep');
const EMPTY = path.join(FIXTURES, 'empty');
const GLOBAL_PREFIX = path.join(FIXTURES, 'global', 'prefix');

const npxCommand = isWindows ? 'npx.cmd' : 'npx';
const npxAvailable = isFibjs || child_process.spawnSync(npxCommand, ['--version'], {
    encoding: 'utf8'
}).status === 0;

let cacheDir;
function cache() {
    if (!cacheDir)
        cacheDir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-npx-test-'));
    return cacheDir;
}

function run(args, cwd, env) {
    const command = isFibjs ? process.execPath : npxCommand;
    const commandArgs = isFibjs
        ? args
        : ['--offline', '--no-install', '--cache', cache(), ...args];

    return child_process.spawnSync(command, commandArgs, {
        cwd,
        env: Object.assign({}, process.env, env || {}),
        encoding: 'utf8'
    });
}

function output(result) {
    assert.equal(result.status, 0,
        `exit ${result.status}\nstdout: ${result.stdout}\nstderr: ${result.stderr}`);
    return result.stdout.trim();
}

describe('npx lookup paths', { skip: isWindows || !npxAvailable }, () => {
    after(() => {
        if (cacheDir)
            fs.rmSync(cacheDir, { recursive: true, force: true });
    });

    it('runs a bin installed above the working directory', () => {
        const out = output(run(['root-tool'], DEEP));
        assert.equal(out, 'ROOT-TOOL');
    });

    it('prefers the nearest ancestor node_modules/.bin', () => {
        const out = output(run(['dup-tool'], DEEP));
        assert.equal(out, 'SUB-DUP');
    });

    it('exposes every ancestor node_modules/.bin on PATH', () => {
        const entries = output(run(['path-echo'], DEEP)).split(path.delimiter);

        // npm prepends the directory the command itself came from.
        assert.equal(entries[0], path.join(PROJECT, 'node_modules', '.bin'));

        for (const dir of [DEEP, path.join(PROJECT, 'sub'), PROJECT]) {
            const bin = path.join(dir, 'node_modules', '.bin');
            assert.ok(entries.includes(bin),
                `expected ${bin} on PATH, got:\n${entries.join('\n')}`);
        }
    });

    it('puts the resolved bin directory before nearer .bin dirs', () => {
        const out = output(run(['root-pref'], DEEP));
        assert.equal(out, 'ROOT-HELPER');
    });

    it('exports INIT_CWD for the command', () => {
        const out = output(run(['init-cwd'], DEEP));
        assert.equal(out, fs.realpathSync(DEEP));
    });

    it('lets a bin resolve a tool from a sibling ancestor .bin', () => {
        const out = output(run(['root-nested'], DEEP));
        assert.equal(out, 'SUB-TOOL');
    });

    it("runs the package's own bin without a .bin link", () => {
        const out = output(run(['own-only'], OWN_DEEP));
        assert.equal(out, 'OWN-ONLY');
    });

    it("prefers the package's own bin over a .bin link", () => {
        const out = output(run(['own-tool'], PROJECT));
        assert.equal(out, 'OWN-TOOL');
    });

    it('normalizes a string bin field to the unscoped package name', () => {
        const out = output(run(['string-bin-fixture'], STRING_BIN));
        assert.equal(out, 'STRING-BIN');
    });

    it('keeps relative and absolute bin targets inside the package', () => {
        assert.equal(output(run(['binpath-rel'], BINPATH_DEEP)), 'CLAMPED');
        assert.equal(output(run(['binpath-abs'], BINPATH_DEEP)), 'CLAMPED');
    });

    it('exposes legacy array bin entries under their basename', () => {
        const out = output(run(['array-tool.js'], ARRAYBIN));
        assert.equal(out, 'ARRAY-BIN');
    });

    it('translates an installed package name to its only bin', () => {
        assert.equal(output(run(['dep-pkg'], PKGNAME_DEEP)), 'LOCAL-DEP');
    });

    it('translates a package name to the bin named after it', () => {
        assert.equal(output(run(['multi-pkg'], PKGNAME_DEEP)), 'MULTI-PKG');
    });

    it('translates a scoped package name', () => {
        assert.equal(output(run(['@scope/scoped-pkg'], PKGNAME_DEEP)), 'SCOPED-PKG');
    });

    it('finds a bin under the configured global prefix', () => {
        const out = output(run(['global-tool'], EMPTY, {
            npm_config_prefix: GLOBAL_PREFIX
        }));
        assert.equal(out, 'GLOBAL-TOOL');
    });

    it('puts the global bin directory first for a global command', () => {
        const out = output(run(['global-nested'], EMPTY, {
            npm_config_prefix: GLOBAL_PREFIX
        }));
        assert.equal(out, 'GLOBAL-HELPER');
    });

    it('puts the global bin directory first for a shell command', () => {
        const out = output(run(['global-shell'], EMPTY, {
            npm_config_prefix: GLOBAL_PREFIX
        }));
        assert.equal(out, 'GLOBAL-HELPER');
    });
});
