var { describe, it, beforeEach, afterEach } = require('node:test');
var assert = require('assert');
var fs = require('fs');
var path = require('path');
var child_process = require('child_process');

// Detect runtime: fibjs or node
const isFibjs = typeof process !== 'undefined' && process.versions && process.versions.fibjs;

const FIXTURES_DIR = path.join(__dirname, 'opt_tools_test_files', 'fixtures');
const TMP_DIR = path.join(__dirname, 'opt_tools_test_files', 'tmp');

// ---------- cross-runtime helpers ----------

function install_pkg(targetDir, pkgPath, opts) {
    if (isFibjs) {
        child_process.spawnSync(process.execPath, ['--install', pkgPath], {
            cwd: targetDir,
            stdio: 'pipe',
            env: {
                ...process.env,
                FIBJS_SILENT_INSALL: opts && opts.silent ? '1' : process.env.FIBJS_SILENT_INSALL,
                ...(opts && opts.env || {})
            }
        });
    } else {
        const npm = process.platform === 'win32' ? 'npm.cmd' : 'npm';
        child_process.spawnSync(npm, ['install', pkgPath], {
            cwd: targetDir,
            stdio: 'pipe',
            shell: process.platform === 'win32',
            env: { ...process.env, ...(opts && opts.env || {}) }
        });
    }
}

function install_from_pkgjson(targetDir, opts) {
    if (isFibjs) {
        child_process.spawnSync(process.execPath, ['--install'], {
            cwd: targetDir,
            stdio: 'pipe',
            env: {
                ...process.env,
                FIBJS_SILENT_INSALL: opts && opts.silent ? '1' : process.env.FIBJS_SILENT_INSALL,
                ...(opts && opts.env || {})
            }
        });
    } else {
        const npm = process.platform === 'win32' ? 'npm.cmd' : 'npm';
        child_process.spawnSync(npm, ['install'], {
            cwd: targetDir,
            stdio: 'pipe',
            shell: process.platform === 'win32',
            env: { ...process.env, ...(opts && opts.env || {}) }
        });
    }
}

function checkSymlink(targetDir, pkgName) {
    const linkPath = path.join(targetDir, 'node_modules', pkgName);
    try {
        return fs.existsSync(linkPath) && fs.lstatSync(linkPath).isSymbolicLink();
    } catch (e) { return false; }
}

function checkFile(targetDir, pkgName, filename) {
    return fs.existsSync(path.join(targetDir, 'node_modules', pkgName, filename));
}

function rmdirSync(dirPath) {
    if (!fs.existsSync(dirPath)) return;
    var files = fs.readdirSync(dirPath);
    files.forEach(function (file) {
        var fullPath = path.join(dirPath, file);
        if (fs.lstatSync(fullPath).isDirectory())
            rmdirSync(fullPath);
        else
            fs.unlinkSync(fullPath);
    });
    fs.rmdirSync(dirPath);
}

// ---------- test data ----------

var localInstallTests = [
    {
        description: 'should create symlink for local package',
        fixture: 'pkg-no-scripts',
        skipInNode: true,
        verify: function (targetDir) {
            assert.ok(checkSymlink(targetDir, 'test-pkg-no-scripts'));
        }
    },
    {
        description: 'should resolve relative path (../) for local package',
        fixture: 'pkg-no-scripts',
        skipInNode: true,
        verify: function (targetDir) {
            assert.ok(checkSymlink(targetDir, 'test-pkg-no-scripts'));
        }
    },
    {
        description: 'should install dependencies of local package',
        fixture: 'pkg-with-deps',
        skipInNode: true,
        verify: function (targetDir) {
            assert.ok(checkSymlink(targetDir, 'test-pkg-with-deps'));
        }
    },
];

var fileProtocolTests = [
    {
        description: 'should resolve file: dependency in package.json',
        pkgJsonContent: { name: 'root', version: '1.0.0', dependencies: { 'test-pkg-no-scripts': 'file:../../fixtures/pkg-no-scripts' } },
        verify: function (targetDir) {
            assert.ok(checkSymlink(targetDir, 'test-pkg-no-scripts'));
        }
    },
];

var lifecycleTests = [
    {
        description: 'should run scripts.install',
        fixture: 'pkg-with-install',
        verify: function (targetDir) {
            assert.ok(checkFile(targetDir, 'test-pkg-install', '.install-ran'));
        },
        skipInNode: true,
    },
    {
        description: 'should run scripts.postinstall',
        fixture: 'pkg-with-postinstall',
        verify: function (targetDir) {
            assert.ok(checkFile(targetDir, 'test-pkg-postinstall', '.postinstall-ran'));
        },
        skipInNode: true,
    },
    {
        description: 'binding.gyp without install script should not error',
        fixture: 'pkg-with-binding-gyp',
        verify: function (targetDir) {
            assert.ok(checkSymlink(targetDir, 'test-pkg-binding-gyp'));
        },
        skipInNode: true,
    },
];

// ---------- parse_pkg_installname unit tests (fibjs-only) ----------

if (isFibjs) (function () {
var parse_pkg_installname = require('internal/helpers/package').parse_pkg_installname;

var parsingTests = [
    // === should be local path ===
    { input: ['./fixtures/pkg'], expectedType: 'local', desc: './fixtures/pkg → local' },
    { input: ['', './fixtures/pkg'], expectedType: 'local', desc: "('', './fixtures/pkg') → local" },
    { input: ['../sibling-pkg'], expectedType: 'local', desc: '../sibling-pkg → local' },
    { input: ['/abs/path/pkg'], expectedType: 'local', desc: '/abs/path/pkg → local' },
    { input: ['~/projects/pkg'], expectedType: 'local', desc: '~/projects/pkg → local' },
    { input: ['file:./fixtures/pkg'], expectedType: 'local', desc: 'file:./fixtures/pkg → local' },
    { input: ['', 'file:../foo'], expectedType: 'local', desc: "file:../foo → local" },
    { input: ['./@scope/pkg'], expectedType: 'local', desc: './@scope/pkg → local (scoped dir)' },

    // === should NOT be local path (semver ranges) ===
    { input: ['~1.0.0'], expectedType: '!local', desc: '~1.0.0 → not local (semver range)' },
    { input: ['^2.0.0'], expectedType: '!local', desc: '^2.0.0 → not local (semver range)' },
    { input: ['>=1.0.0'], expectedType: '!local', desc: '>=1.0.0 → not local (semver range)' },
    { input: ['1.0.0'], expectedType: '!local', desc: '1.0.0 → not local (exact version)' },
    { input: ['*'], expectedType: '!local', desc: '* → not local' },
    { input: ['latest'], expectedType: '!local', desc: 'latest → not local' },
    { input: ['1.0.0 - 2.0.0'], expectedType: '!local', desc: '1.0.0 - 2.0.0 → not local (semver range)' },
    { input: ['', '~1.1.0'], expectedType: '!local', desc: "('', '~1.1.0') → not local" },

    // === should be registry ===
    { input: ['foo'], expectedType: 'registry', desc: 'foo → registry' },
    { input: ['@scope/foo'], expectedType: 'registry', desc: '@scope/foo → registry' },
    { input: ['foo@1.0.0'], expectedType: 'registry', desc: 'foo@1.0.0 → registry' },
    { input: ['@scope/foo@^2.0'], expectedType: 'registry', desc: '@scope/foo@^2.0 → registry' },

    // === should be git ===
    { input: ['user/repo'], expectedType: 'git', desc: 'user/repo → git' },
    { input: ['user/repo#branch'], expectedType: 'git', desc: 'user/repo#branch → git' },
];

describe('parse_pkg_installname', function () {
    parsingTests.forEach(function (t) {
        it(t.desc, function () {
            var result = parse_pkg_installname.apply(null, t.input);
            if (t.expectedType === '!local') {
                assert.notEqual(result.type, 'local', 'should not be local');
            } else {
                assert.equal(result.type, t.expectedType);
            }
        });
    });
});

})(); // end isFibjs guard

// ---------- tests ----------

describe('opt_tools install lifecycle', function () {
    var testTargets = [];

    function makeTargetDir() {
        var testId = Date.now() + '_' + Math.random().toString(36).slice(2);
        var targetDir = path.join(TMP_DIR, 'test_' + testId);
        fs.mkdirSync(targetDir, { recursive: true });
        testTargets.push(targetDir);
        return targetDir;
    }

    function createPackageJson(dir, content) {
        fs.writeFileSync(path.join(dir, 'package.json'), JSON.stringify(content));
    }

    beforeEach(function () {
        testTargets = [];
    });

    afterEach(function () {
        testTargets.forEach(function (t) {
            try {
                var nm = path.join(t, 'node_modules');
                if (fs.existsSync(nm)) rmdirSync(nm);
                ['package.json', 'package-lock.json'].forEach(function (f) {
                    var fp = path.join(t, f);
                    if (fs.existsSync(fp)) fs.unlinkSync(fp);
                });
                fs.rmdirSync(t);
            } catch (e) { /* ignore cleanup errors */ }
        });
        testTargets = [];
    });

    // ===== Phase 1: local directory install =====
    describe('local directory install (Phase 1)', function () {
        localInstallTests.forEach(function (test) {
            var shouldSkip = test.skip || (!isFibjs && test.skipInNode);
            (shouldSkip ? it.skip : it)(test.description, function () {
                var targetDir = makeTargetDir();
                var fixturePath = path.join(FIXTURES_DIR, test.fixture);

                install_pkg(targetDir, fixturePath, { silent: true });

                test.verify(targetDir);
            });
        });
    });

    // ===== Phase 1: file: protocol =====
    describe('file: protocol in package.json (Phase 1)', function () {
        fileProtocolTests.forEach(function (test) {
            (test.skip ? it.skip : it)(test.description, function () {
                var targetDir = makeTargetDir();
                createPackageJson(targetDir, test.pkgJsonContent);

                install_from_pkgjson(targetDir, { silent: true });

                test.verify(targetDir);
            });
        });
    });

    // ===== Phase 3: lifecycle scripts =====
    describe('lifecycle scripts (Phase 3)', function () {
        lifecycleTests.forEach(function (test) {
            var shouldSkip = test.skip || (!isFibjs && test.skipInNode);
            (shouldSkip ? it.skip : it)(test.description, function () {
                var targetDir = makeTargetDir();
                var fixturePath = path.join(FIXTURES_DIR, test.fixture);

                install_pkg(targetDir, fixturePath, { silent: true });

                test.verify(targetDir);
            });
        });
    });
});
