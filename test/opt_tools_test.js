var { describe, it, before, beforeEach, afterEach } = require('node:test');
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
        return child_process.spawnSync(process.execPath, ['--install', pkgPath], {
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
        return child_process.spawnSync(npm, ['install', pkgPath], {
            cwd: targetDir,
            stdio: 'pipe',
            shell: process.platform === 'win32',
            env: { ...process.env, ...(opts && opts.env || {}) }
        });
    }
}

function install_from_pkgjson(targetDir, opts) {
    if (isFibjs) {
        return child_process.spawnSync(process.execPath, ['--install'], {
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
        return child_process.spawnSync(npm, ['install'], {
            cwd: targetDir,
            stdio: 'pipe',
            shell: process.platform === 'win32',
            env: { ...process.env, ...(opts && opts.env || {}) }
        });
    }
}

/**
 * @description whether the local package reached node_modules: as a link to the
 *              fixture where the filesystem can host one, as the copy the
 *              installer falls back to where it cannot (see LINKS_USED)
 */
function checkInstalled(targetDir, pkgName) {
    var p = path.join(targetDir, 'node_modules', pkgName);

    try {
        if (!fs.existsSync(p)) return false;

        if (LINKS_USED)
            return fs.lstatSync(p).isSymbolicLink();

        return fs.lstatSync(p).isDirectory() && fs.existsSync(path.join(p, 'package.json'));
    } catch (e) {
        return false;
    }
}

function checkFile(targetDir, pkgName, filename) {
    return fs.existsSync(path.join(targetDir, 'node_modules', pkgName, filename));
}

/**
 * @description list the entries of a directory, marking the links: a failed
 *              installation is mostly about what node_modules does (not) contain
 */
function listDir(dir) {
    if (!fs.existsSync(dir)) return '(missing)';

    try {
        var names = fs.readdirSync(dir);
        if (!names.length) return '(empty)';

        return names.map(function (n) {
            var p = path.join(dir, n);
            var kind;

            try {
                kind = fs.lstatSync(p).isSymbolicLink() ? 'link' : (fs.lstatSync(p).isDirectory() ? 'dir' : 'file');
            } catch (e) {
                kind = 'unreadable';
            }

            return n + ' [' + kind + ']';
        }).join(', ');
    } catch (e) {
        return '(unreadable: ' + e.message + ')';
    }
}

/**
 * @description describe a failed installation: what the installer printed (which
 *              is nothing when the install never started), and what it produced
 */
function diag(targetDir, res) {
    var out = '';

    if (res) {
        if (res.stdout) out += String(res.stdout);
        if (res.stderr) out += String(res.stderr);
    }

    var exit = res ? res.status : 'not captured';
    if (res && res.error) exit += ' (spawn error: ' + res.error.message + ')';

    return '\n  install target : ' + targetDir +
        '\n  exit code      : ' + exit +
        '\n  node_modules   : ' + listDir(path.join(targetDir, 'node_modules')) +
        '\n  install output :\n' + (out.trim() || '(nothing)').replace(/^/gm, '    ');
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

// ---------- environment check ----------

/**
 * @description whether TMP_DIR can host a link to a directory. A filesystem that
 *              cannot (a mapped network drive, a VM shared folder or a FAT/exFAT
 *              volume on Windows refuses reparse points) makes the installer fall
 *              back to copying the package, which is what the assertions then
 *              expect
 */
function probeLinkSupport() {
    fs.mkdirSync(TMP_DIR, { recursive: true });

    var probeTarget = path.join(TMP_DIR, 'link_probe_target');
    var probeLink = path.join(TMP_DIR, 'link_probe');

    function cleanup() {
        try { if (fs.existsSync(probeLink)) fs.unlinkSync(probeLink); } catch (e) { /* ignore */ }
        try { if (fs.existsSync(probeTarget)) fs.rmdirSync(probeTarget); } catch (e) { /* ignore */ }
    }

    cleanup();

    var supported = false;
    try {
        fs.mkdirSync(probeTarget, { recursive: true });

        if (isFibjs) {
            if (process.platform === 'win32')
                fs.symlink(probeTarget, probeLink, 'junction');
            else
                fs.symlink(probeTarget, probeLink);
        } else {
            fs.symlinkSync(probeTarget, probeLink, process.platform === 'win32' ? 'junction' : 'dir');
        }

        supported = fs.lstatSync(probeLink).isSymbolicLink();
    } catch (e) {
        supported = false;
    }
    cleanup();

    return supported;
}

// the installer links a local package, and copies it where the filesystem cannot
// host a link, so the assertions follow what this filesystem can do
const LINKS_USED = probeLinkSupport();

// ---------- test data ----------

var localInstallTests = [
    {
        description: 'should create symlink for local package',
        fixture: 'pkg-no-scripts',
        skipInNode: true,
        verify: function (targetDir, res) {
            assert.ok(checkInstalled(targetDir, 'test-pkg-no-scripts'), diag(targetDir, res));
        }
    },
    {
        description: 'should resolve relative path (../) for local package',
        fixture: 'pkg-no-scripts',
        relative: true,
        skipInNode: true,
        verify: function (targetDir, res) {
            assert.ok(checkInstalled(targetDir, 'test-pkg-no-scripts'), diag(targetDir, res));
        }
    },
    {
        description: 'should install dependencies of local package',
        fixture: 'pkg-with-deps',
        skipInNode: true,
        verify: function (targetDir, res) {
            assert.ok(checkInstalled(targetDir, 'test-pkg-with-deps'), diag(targetDir, res));
            // `pkg-with-deps` depends on `test-pkg-install@file:../pkg-with-install`: the
            // spec is relative to the package itself, not to the installation target
            assert.ok(checkInstalled(targetDir, 'test-pkg-install'), diag(targetDir, res));
        }
    },
];

var fileProtocolTests = [
    {
        description: 'should resolve file: dependency in package.json',
        pkgJsonContent: { name: 'root', version: '1.0.0', dependencies: { 'test-pkg-no-scripts': 'file:../../fixtures/pkg-no-scripts' } },
        verify: function (targetDir, res) {
            assert.ok(checkInstalled(targetDir, 'test-pkg-no-scripts'), diag(targetDir, res));
        }
    },
];

// peer dependencies are installed like npm (>= 7) does: every peer a package
// declares is fetched, except the ones marked optional
var peerDependencyTests = [
    {
        description: 'should install the peer dependency of a local package',
        fixture: 'pkg-with-peer',
        // npm skips the peers of a linked (file:) package, so this is fibjs only
        skipInNode: true,
        verify: function (targetDir, res) {
            assert.ok(checkInstalled(targetDir, 'test-pkg-with-peer'), diag(targetDir, res));
            // `pkg-with-peer` peers on `test-pkg-peer@file:../pkg-peer`: the spec is
            // relative to the package itself, and the peer is hoisted to the root
            assert.ok(checkInstalled(targetDir, 'test-pkg-peer'), diag(targetDir, res));
        }
    },
    {
        description: 'should skip a peer dependency marked optional',
        fixture: 'pkg-with-opt-peer',
        skipInNode: true,
        verify: function (targetDir, res) {
            assert.ok(checkInstalled(targetDir, 'test-pkg-with-opt-peer'), diag(targetDir, res));
            assert.ok(!checkInstalled(targetDir, 'test-pkg-opt-peer'),
                'peerDependenciesMeta.optional peer must not be installed: ' + diag(targetDir, res));
        }
    },
    {
        description: 'should leave a workspace: peer to the workspace',
        fixture: 'pkg-with-ws-peer',
        skipInNode: true,
        verify: function (targetDir, res) {
            assert.ok(checkInstalled(targetDir, 'test-pkg-with-ws-peer'), diag(targetDir, res));
            assert.ok(!checkInstalled(targetDir, 'test-pkg-ws-peer'),
                'a workspace: peer must not be looked up in the registry: ' + diag(targetDir, res));
        }
    },
];

var peerProtocolTests = [
    {
        description: 'should install a peer dependency declared by the root package',
        pkgJsonContent: { name: 'root', version: '1.0.0', peerDependencies: { 'test-pkg-no-scripts': 'file:../../fixtures/pkg-no-scripts' } },
        verify: function (targetDir, res) {
            assert.ok(checkInstalled(targetDir, 'test-pkg-no-scripts'), diag(targetDir, res));
        }
    },
    {
        description: 'should reuse the peer dependency satisfied one level up',
        pkgJsonContent: {
            name: 'root',
            version: '1.0.0',
            dependencies: {
                'test-pkg-peer': 'file:../../fixtures/pkg-peer',
                'test-pkg-with-peer': 'file:../../fixtures/pkg-with-peer'
            }
        },
        verify: function (targetDir, res) {
            assert.ok(checkInstalled(targetDir, 'test-pkg-peer'), diag(targetDir, res));
            assert.ok(checkInstalled(targetDir, 'test-pkg-with-peer'), diag(targetDir, res));
            // the root already provides the peer: no second copy inside the dependent
            assert.ok(!fs.existsSync(path.join(targetDir, 'node_modules', 'test-pkg-with-peer', 'node_modules')),
                'peer satisfied one level up must not be installed again: ' + diag(targetDir, res));
        }
    },
];

var lifecycleTests = [
    {
        description: 'should run scripts.install',
        fixture: 'pkg-with-install',
        verify: function (targetDir, res) {
            assert.ok(checkFile(targetDir, 'test-pkg-install', '.install-ran'), diag(targetDir, res));
        },
        skipInNode: true,
    },
    {
        description: 'should run scripts.postinstall',
        fixture: 'pkg-with-postinstall',
        verify: function (targetDir, res) {
            assert.ok(checkFile(targetDir, 'test-pkg-postinstall', '.postinstall-ran'), diag(targetDir, res));
        },
        skipInNode: true,
    },
    {
        description: 'binding.gyp without install script should not error',
        fixture: 'pkg-with-binding-gyp',
        verify: function (targetDir, res) {
            assert.ok(checkInstalled(targetDir, 'test-pkg-binding-gyp'), diag(targetDir, res));
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

    // === should be local path (windows forms, recognised on every platform) ===
    { input: ['.\\fixtures\\pkg'], expectedType: 'local', desc: '.\\fixtures\\pkg → local (windows relative)' },
    { input: ['..\\sibling-pkg'], expectedType: 'local', desc: '..\\sibling-pkg → local (windows relative)' },
    { input: ['D:\\projects\\pkg'], expectedType: 'local', desc: 'D:\\projects\\pkg → local (windows drive)' },
    { input: ['D:/projects/pkg'], expectedType: 'local', desc: 'D:/projects/pkg → local (windows drive, forward slash)' },
    { input: ['\\\\server\\share\\pkg'], expectedType: 'local', desc: '\\\\server\\share\\pkg → local (UNC)' },
    { input: ['file:D:\\projects\\pkg'], expectedType: 'local', desc: 'file:D:\\projects\\pkg → local' },
    { input: ['file:///D:/projects/pkg'], expectedType: 'local', desc: 'file:///D:/projects/pkg → local (file url, drive)' },
    { input: ['file:///projects/pkg'], expectedType: 'local', desc: 'file:///projects/pkg → local (file url, absolute)' },

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

    before(function () {
        if (!LINKS_USED)
            console.log('  # local packages are installed as copies here: ' +
                'the tmp filesystem cannot host a directory link');
    });

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
            it(test.description, { skip: shouldSkip }, function () {
                var targetDir = makeTargetDir();
                var fixturePath = path.join(FIXTURES_DIR, test.fixture);

                // a relative spec must stay relative: `fibjs --install ../x` has to resolve
                // against the working directory, just like `fibjs --install ./x`
                if (test.relative)
                    fixturePath = path.relative(targetDir, fixturePath);

                // the installation runs verbosely: whatever it printed lands in the
                // failure message instead of the void
                test.verify(targetDir, install_pkg(targetDir, fixturePath));
            });
        });
    });

    // ===== Phase 1: file: protocol =====
    describe('file: protocol in package.json (Phase 1)', function () {
        fileProtocolTests.forEach(function (test) {
            it(test.description, { skip: test.skip }, function () {
                var targetDir = makeTargetDir();
                createPackageJson(targetDir, test.pkgJsonContent);

                test.verify(targetDir, install_from_pkgjson(targetDir));
            });
        });
    });

    // ===== Phase 3: lifecycle scripts =====
    describe('lifecycle scripts (Phase 3)', function () {
        lifecycleTests.forEach(function (test) {
            var shouldSkip = test.skip || (!isFibjs && test.skipInNode);
            it(test.description, { skip: shouldSkip }, function () {
                var targetDir = makeTargetDir();
                var fixturePath = path.join(FIXTURES_DIR, test.fixture);

                test.verify(targetDir, install_pkg(targetDir, fixturePath));
            });
        });
    });

    // ===== Phase 4: peer dependencies =====
    describe('peer dependencies (Phase 4)', function () {
        peerDependencyTests.forEach(function (test) {
            var shouldSkip = test.skip || (!isFibjs && test.skipInNode);
            it(test.description, { skip: shouldSkip }, function () {
                var targetDir = makeTargetDir();
                var fixturePath = path.join(FIXTURES_DIR, test.fixture);

                test.verify(targetDir, install_pkg(targetDir, fixturePath));
            });
        });

        peerProtocolTests.forEach(function (test) {
            it(test.description, { skip: test.skip }, function () {
                var targetDir = makeTargetDir();
                createPackageJson(targetDir, test.pkgJsonContent);

                test.verify(targetDir, install_from_pkgjson(targetDir));
            });
        });
    });
});
