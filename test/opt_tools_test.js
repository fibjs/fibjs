var { describe, it, before, after, beforeEach, afterEach } = require('node:test');
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
 * @description run the installer with explicit arguments. The `fibjs --install`
 *              command line (its flags, and what it does with a flag it does not
 *              know) has no npm equivalent, so every case built on this helper is
 *              marked skipInNode
 */
function runInstaller(targetDir, args, opts) {
    return child_process.spawnSync(process.execPath, args, {
        cwd: targetDir,
        stdio: 'pipe',
        env: { ...process.env, ...(opts && opts.env || {}) }
    });
}

/**
 * @description whatever a run printed, wherever it printed it: the installer warns
 *              on stderr for some things and logs on stdout for others, and a test
 *              should not care which
 */
function outputOf(res) {
    var out = '';

    if (res) {
        if (res.stdout) out += String(res.stdout);
        if (res.stderr) out += String(res.stderr);
    }

    return out;
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

/**
 * @description copy a directory tree, links included: an installed tree has links
 *              in it, and reading one as a file is not a copy
 */
function copyDir(src, dest) {
    fs.mkdirSync(dest, { recursive: true });
    fs.readdirSync(src).forEach(function (n) {
        var s = path.join(src, n);
        var d = path.join(dest, n);
        var st = fs.lstatSync(s);

        if (st.isSymbolicLink()) {
            var target = path.resolve(path.dirname(s), fs.readlinkSync(s));
            if (process.platform === 'win32') fs.symlinkSync(target, d, 'junction');
            else fs.symlinkSync(target, d);
            return;
        }

        if (st.isDirectory()) copyDir(s, d);
        else fs.writeFileSync(d, fs.readFileSync(s));
    });
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

    // ===== Phase 5: frozen install from a lockfile =====
    // the frozen install is fibjs's own command (`npm ci` is npm's answer to the
    // same problem, and the npm side of the suite already uses it), so these cases
    // only make sense under fibjs
    if (isFibjs) describe('lockfile / ci (Phase 5)', function () {
        // the fixture carries a real npm generated lock: three registry packages,
        // one of them dev only, a `file:` link and a foreign platform optional
        var LOCK_FIXTURE = path.join(FIXTURES_DIR, 'lock-basic-v3');

        function makeLockProject() {
            var targetDir = makeTargetDir();
            copyDir(LOCK_FIXTURE, targetDir);
            return targetDir;
        }

        function treeOf(dir) {
            var nm = path.join(dir, 'node_modules');
            if (!fs.existsSync(nm)) return '(no node_modules)';

            return fs.readdirSync(nm).sort().filter(function (n) {
                // npm writes a hidden lockfile into node_modules, and leaves an empty
                // directory behind for a scope whose only package was skipped for this
                // platform; fibjs produces neither (the hidden lock is not implemented,
                // see the plan) — neither is part of the installed tree
                if (n === '.package-lock.json') return false;

                var p = path.join(nm, n);
                if (n.charAt(0) === '@' && fs.lstatSync(p).isDirectory())
                    return fs.readdirSync(p).some(function (s) {
                        return fs.existsSync(path.join(p, s, 'package.json'));
                    });

                return true;
            }).map(function (n) {
                var p = path.join(nm, n);
                var st = fs.lstatSync(p);
                if (st.isSymbolicLink()) return n + ' link';
                var pj = path.join(p, 'package.json');
                if (fs.existsSync(pj)) {
                    try { return n + '@' + (JSON.parse(fs.readFileSync(pj, 'utf8')).version || '?'); } catch (e) { }
                }
                return n + ' dir';
            }).join(', ');
        }

        function writeFile(dir, rel, content) {
            var p = path.join(dir, rel);
            fs.mkdirSync(path.dirname(p), { recursive: true });
            fs.writeFileSync(p, content);
        }

        /**
         * @description is there a file with that name anywhere under the directory?
         *              links are followed: a local package is linked, not copied
         */
        function findFile(dir, name) {
            if (!fs.existsSync(dir)) return false;

            var found = false;
            fs.readdirSync(dir).forEach(function (n) {
                if (found) return;

                var p = path.join(dir, n);
                var st;
                try { st = fs.statSync(p); } catch (e) { return; }
                if (st.isDirectory()) found = findFile(p, name);
                else if (n === name) found = true;
            });

            return found;
        }

        /**
         * @description a project whose only dependency is a local package that asks
         *              for a prebuilt addon. The lockfile cannot describe the addon
         *              (npm has no such field), so the installer has to read it from
         *              the package itself
         */
        function makeBinaryProject(binary) {
            var targetDir = makeTargetDir();

            writeFile(targetDir, 'package.json', JSON.stringify({
                name: 'binary-proj', version: '1.0.0', dependencies: { binpkg: 'file:binpkg' },
            }));
            writeFile(targetDir, 'package-lock.json', JSON.stringify({
                name: 'binary-proj', version: '1.0.0', lockfileVersion: 3, requires: true,
                packages: {
                    '': { name: 'binary-proj', version: '1.0.0', dependencies: { binpkg: 'file:binpkg' } },
                    'binpkg': { name: 'binpkg', version: '1.0.0' },
                    'node_modules/binpkg': { resolved: 'binpkg', link: true },
                },
            }, null, 2));
            writeFile(targetDir, 'binpkg/package.json', JSON.stringify({
                name: 'binpkg', version: '1.0.0', main: 'index.js', binary: binary,
            }, null, 2));

            return targetDir;
        }

        it('installs what the lockfile says and leaves the lockfile alone', function () {
            var targetDir = makeLockProject();
            var before = fs.readFileSync(path.join(targetDir, 'package-lock.json'), 'utf8');
            var res = runInstaller(targetDir, ['--install', '--ci']);

            assert.equal(res.status, 0, diag(targetDir, res));
            assert.equal(fs.readFileSync(path.join(targetDir, 'package-lock.json'), 'utf8'), before,
                'a frozen install never writes the lockfile');

            var tree = treeOf(targetDir);
            assert.ok(tree.indexOf('is-number@6.0.0') > -1, 'locked version, not the newest: ' + tree);
            assert.ok(tree.indexOf('is-odd@3.0.1') > -1, 'transitive: ' + tree);
            assert.ok(tree.indexOf('is-negative@2.1.0') > -1, 'dev dependency is installed: ' + tree);
            assert.ok(tree.indexOf('local-pkg link') > -1, 'the file: dependency is linked: ' + tree);
            assert.ok(tree.indexOf('@esbuild') === -1, 'a foreign platform optional stays out: ' + tree);
        });

        it('refuses a package.json that moved past the lockfile, without touching the disk', function () {
            var targetDir = makeLockProject();
            var pkgjson = JSON.parse(fs.readFileSync(path.join(targetDir, 'package.json'), 'utf8'));
            pkgjson.dependencies['is-number'] = '^7.0.0';
            fs.writeFileSync(path.join(targetDir, 'package.json'), JSON.stringify(pkgjson, null, 2));

            var res = runInstaller(targetDir, ['--install', '--ci']);
            var out = outputOf(res);

            assert.equal(res.status, 1, diag(targetDir, res));
            assert.ok(out.indexOf('Invalid: lock file\'s is-number@6.0.0') > -1, out);
            assert.equal(fs.existsSync(path.join(targetDir, 'node_modules')), false,
                'the check runs before node_modules is emptied');
        });

        it('refuses a dependency the lockfile does not have', function () {
            var targetDir = makeLockProject();
            var pkgjson = JSON.parse(fs.readFileSync(path.join(targetDir, 'package.json'), 'utf8'));
            pkgjson.dependencies['is-positive'] = '^3.0.0';
            fs.writeFileSync(path.join(targetDir, 'package.json'), JSON.stringify(pkgjson, null, 2));

            var res = runInstaller(targetDir, ['--install', '--ci']);
            var out = outputOf(res);

            assert.equal(res.status, 1, diag(targetDir, res));
            assert.ok(out.indexOf('Missing: is-positive@^3.0.0 from lock file') > -1, out);
        });

        it('refuses to run without a lockfile', function () {
            var targetDir = makeTargetDir();
            createPackageJson(targetDir, { name: 'no-lock', version: '1.0.0' });

            var res = runInstaller(targetDir, ['--install', '--ci']);
            var out = outputOf(res);

            assert.equal(res.status, 1, diag(targetDir, res));
            assert.ok(out.indexOf('needs a package-lock.json') > -1, out);
        });

        it('refuses a package argument, and --no-package-lock', function () {
            var targetDir = makeLockProject();

            var withPkg = runInstaller(targetDir, ['--install', '--ci', 'is-number']);
            assert.equal(withPkg.status, 1, diag(targetDir, withPkg));
            assert.ok(outputOf(withPkg).indexOf('takes no package argument') > -1, outputOf(withPkg));

            var withNoLock = runInstaller(targetDir, ['--install', '--ci', '--no-package-lock']);
            assert.equal(withNoLock.status, 1, diag(targetDir, withNoLock));
            assert.ok(outputOf(withNoLock).indexOf('cannot be combined with --ci') > -1, outputOf(withNoLock));
        });

        it('--dry-run checks without writing anything', function () {
            var targetDir = makeLockProject();
            var res = runInstaller(targetDir, ['--install', '--ci', '--dry-run']);

            assert.equal(res.status, 0, diag(targetDir, res));
            assert.ok(outputOf(res).indexOf('nothing was written') > -1, outputOf(res));
            assert.equal(fs.existsSync(path.join(targetDir, 'node_modules')), false);
        });

        it('empties node_modules before installing, like npm ci', function () {
            var targetDir = makeLockProject();
            var leftovers = path.join(targetDir, 'node_modules', 'leftover');
            fs.mkdirSync(leftovers, { recursive: true });
            fs.writeFileSync(path.join(leftovers, 'package.json'), JSON.stringify({ name: 'leftover', version: '1.0.0' }));

            var res = runInstaller(targetDir, ['--install', '--ci']);

            assert.equal(res.status, 0, diag(targetDir, res));
            assert.equal(fs.existsSync(leftovers), false, 'an entry that is not in the lockfile goes');
        });

        it('fetches the prebuilt addon a package declares', { skip: !process.env.FIBJS_TEST_NETWORK }, function () {
            // the artifact has to come from an https host, and it is fetched without
            // an integrity to check against (the plan's §7 R10): a real tarball keeps
            // this case honest, and the registry keeps it reachable
            var targetDir = makeBinaryProject({
                module_name: 'is-number',
                module_path: 'build/Release',
                host: 'https://registry.npmjs.org',
                remote_path: 'is-number/-',
                package_name: 'is-number-6.0.0.tgz',
            });

            var res = runInstaller(targetDir, ['--install', '--ci']);

            assert.equal(res.status, 0, diag(targetDir, res));
            assert.ok(outputOf(res).indexOf('[install addon]') > -1, outputOf(res));
            assert.ok(fs.existsSync(path.join(targetDir, 'node_modules', 'binpkg', 'build', 'Release', 'index.js')),
                'the addon is unpacked into the package itself, not into a path built from an absolute module_path' +
                diag(targetDir, res));
        });

        it('ignores a binary field that describes nothing usable', function () {
            // a partial node-pre-gyp config is the package's problem: the install
            // goes on without the prebuilt addon instead of failing
            var targetDir = makeBinaryProject({ module_path: 'build/Release' });

            var res = runInstaller(targetDir, ['--install', '--ci']);

            assert.equal(res.status, 0, diag(targetDir, res));
            assert.ok(outputOf(res).indexOf('ignoring an unusable binary field') > -1, outputOf(res));
            assert.ok(checkInstalled(targetDir, 'binpkg'), 'the package itself is installed');
        });

        it('installs from a v1 lockfile, converting the legacy tree in memory', function () {
            var targetDir = makeTargetDir();

            writeFile(targetDir, 'package.json', JSON.stringify({
                name: 'v1-proj', version: '1.0.0', dependencies: { binpkg: 'file:binpkg' },
            }));
            // v1 has no path table and no `link` flag: a local package is recognised
            // by its `file:` version, the way npm converts it when it reads v1
            writeFile(targetDir, 'package-lock.json', JSON.stringify({
                name: 'v1-proj', version: '1.0.0', lockfileVersion: 1, requires: true,
                dependencies: { binpkg: { version: 'file:binpkg' } },
            }, null, 2));
            writeFile(targetDir, 'binpkg/package.json', JSON.stringify({ name: 'binpkg', version: '1.0.0' }));

            var res = runInstaller(targetDir, ['--install', '--ci']);

            assert.equal(res.status, 0, diag(targetDir, res));
            assert.ok(checkInstalled(targetDir, 'binpkg'), 'the local package is linked' + diag(targetDir, res));
        });

        it('prefers npm-shrinkwrap.json over a stale package-lock.json', function () {
            var targetDir = makeTargetDir();

            writeFile(targetDir, 'package.json', JSON.stringify({
                name: 'shrinkwrap-proj', version: '1.0.0', dependencies: { binpkg: 'file:binpkg' },
            }));
            // the shrinkwrap knows the dependency, the lockfile next to it does not:
            // npm prefers the shrinkwrap, and so does the frozen install
            writeFile(targetDir, 'npm-shrinkwrap.json', JSON.stringify({
                name: 'shrinkwrap-proj', version: '1.0.0', lockfileVersion: 3, requires: true,
                packages: {
                    '': { name: 'shrinkwrap-proj', version: '1.0.0', dependencies: { binpkg: 'file:binpkg' } },
                    'binpkg': { name: 'binpkg', version: '1.0.0' },
                    'node_modules/binpkg': { resolved: 'binpkg', link: true },
                },
            }, null, 2));
            writeFile(targetDir, 'package-lock.json', JSON.stringify({
                name: 'shrinkwrap-proj', version: '1.0.0', lockfileVersion: 3, requires: true,
                packages: { '': { name: 'shrinkwrap-proj', version: '1.0.0' } },
            }, null, 2));
            writeFile(targetDir, 'binpkg/package.json', JSON.stringify({ name: 'binpkg', version: '1.0.0' }));

            var res = runInstaller(targetDir, ['--install', '--ci']);

            assert.equal(res.status, 0, diag(targetDir, res));
            assert.ok(outputOf(res).indexOf('npm-shrinkwrap.json is in sync') > -1, outputOf(res));
            assert.ok(checkInstalled(targetDir, 'binpkg'));
        });

        it('refuses a tampered tarball integrity', { skip: !process.env.FIBJS_TEST_NETWORK }, function () {
            // the registry is needed to fetch the tarball, so this case only runs
            // where the network layer is asked for
            var targetDir = makeLockProject();
            var lock = JSON.parse(fs.readFileSync(path.join(targetDir, 'package-lock.json'), 'utf8'));
            var entry = lock.packages['node_modules/is-number'];
            entry.integrity = entry.integrity.replace(/^sha512-..../, 'sha512-0000');
            fs.writeFileSync(path.join(targetDir, 'package-lock.json'), JSON.stringify(lock, null, 2));

            var res = runInstaller(targetDir, ['--install', '--ci']);
            var out = outputOf(res);

            assert.notEqual(res.status, 0, 'a tarball that does not match the lockfile must fail' + diag(targetDir, res));
            assert.ok(out.indexOf('integrity mismatch') > -1, out);
        });

        it('produces the same tree as npm ci', { skip: !process.env.FIBJS_TEST_NETWORK }, function () {
            var targetDir = makeLockProject();

            var res = runInstaller(targetDir, ['--install', '--ci']);
            assert.equal(res.status, 0, diag(targetDir, res));

            var ours = treeOf(targetDir);

            // npm's own answer for the same lockfile, in a second copy
            var npmDir = makeLockProject();
            var npm = process.platform === 'win32' ? 'npm.cmd' : 'npm';
            var npmRes = child_process.spawnSync(npm, ['ci', '--no-audit', '--no-fund'], {
                cwd: npmDir,
                stdio: 'pipe',
                shell: process.platform === 'win32',
            });

            assert.equal(npmRes.status, 0, String(npmRes.stderr || ''));
            assert.equal(ours, treeOf(npmDir), 'the frozen install has to match npm ci');
        });
    });

    // ===== Phase 6: lock-first install and the write back =====
    // offline: a project whose dependencies are local packages, so the lockfile
    // round trip needs no registry
    if (isFibjs) describe('lockfile / write back (Phase 6)', function () {
        function writeFile(dir, rel, content) {
            var p = path.join(dir, rel);
            fs.mkdirSync(path.dirname(p), { recursive: true });
            fs.writeFileSync(p, content);
        }

        /**
         * @description a project with one linked dependency and one linked dev
         *              dependency, and no lockfile yet
         */
        function makeProject() {
            var targetDir = makeTargetDir();

            writeFile(targetDir, 'package.json', JSON.stringify({
                name: 'write-proj', version: '1.0.0',
                dependencies: { linkpkg: 'file:linkpkg' },
                devDependencies: { devpkg: 'file:devpkg' },
            }, null, 2));
            writeFile(targetDir, 'linkpkg/package.json', JSON.stringify({ name: 'linkpkg', version: '1.0.0' }, null, 2));
            writeFile(targetDir, 'devpkg/package.json', JSON.stringify({ name: 'devpkg', version: '2.0.0' }, null, 2));

            return targetDir;
        }

        function lockOf(dir) {
            return JSON.parse(fs.readFileSync(path.join(dir, 'package-lock.json'), 'utf8'));
        }

        it('writes a lockfile npm would write: root, link entry, link target', function () {
            var targetDir = makeProject();
            var res = runInstaller(targetDir, ['--install']);

            assert.equal(res.status, 0, diag(targetDir, res));

            var lock = lockOf(targetDir);
            assert.equal(lock.lockfileVersion, 3);
            assert.equal(lock.requires, true);
            assert.equal(lock.name, 'write-proj');

            // npm writes a link as the target plus the flag, nothing else
            assert.deepEqual(lock.packages['node_modules/linkpkg'], { resolved: 'linkpkg', link: true });
            assert.deepEqual(lock.packages['linkpkg'], { name: 'linkpkg', version: '1.0.0' });
            assert.equal(lock.packages[''].dependencies.linkpkg, 'file:linkpkg');
        });

        it('describes the dev dependencies, and installs them like npm does', function () {
            var targetDir = makeProject();
            var res = runInstaller(targetDir, ['--install']);

            assert.equal(res.status, 0, diag(targetDir, res));

            var lock = lockOf(targetDir);
            assert.equal(lock.packages['node_modules/devpkg'].dev, true,
                'a lockfile describes the whole graph: ' + JSON.stringify(lock.packages['node_modules/devpkg']));
            assert.ok(checkInstalled(targetDir, 'devpkg'),
                'dev dependencies are installed by default, as npm does' + diag(targetDir, res));
        });

        it('--omit=dev keeps the dev dependencies out of the disk, not out of the lockfile', function () {
            var targetDir = makeProject();
            var res = runInstaller(targetDir, ['--install', '--omit=dev']);

            assert.equal(res.status, 0, diag(targetDir, res));
            assert.equal(lockOf(targetDir).packages['node_modules/devpkg'].dev, true,
                'the lockfile still describes them (npm: omit is about the disk)');
            assert.equal(fs.existsSync(path.join(targetDir, 'node_modules', 'devpkg')), false,
                'and they are not installed');
        });

        it('keeps the lockfile when it already covers package.json', function () {
            var targetDir = makeProject();
            runInstaller(targetDir, ['--install']);

            var before = fs.readFileSync(path.join(targetDir, 'package-lock.json'), 'utf8');
            var res = runInstaller(targetDir, ['--install']);

            assert.equal(res.status, 0, diag(targetDir, res));
            assert.ok(outputOf(res).indexOf('covers package.json') > -1,
                'the lockfile is the plan, it is not resolved again' + diag(targetDir, res));
            assert.equal(fs.readFileSync(path.join(targetDir, 'package-lock.json'), 'utf8'), before,
                'and it is not rewritten either');
        });

        it('is idempotent: writing it twice gives the same bytes', function () {
            var targetDir = makeProject();
            runInstaller(targetDir, ['--install']);

            var first = fs.readFileSync(path.join(targetDir, 'package-lock.json'), 'utf8');

            // a fresh install, from nothing, has to describe the same thing
            rmdirSync(path.join(targetDir, 'node_modules'));
            var res = runInstaller(targetDir, ['--install']);

            assert.equal(res.status, 0, diag(targetDir, res));
            assert.equal(fs.readFileSync(path.join(targetDir, 'package-lock.json'), 'utf8'), first);
        });

        it('updates the lockfile when package.json grew a dependency', function () {
            var targetDir = makeProject();
            runInstaller(targetDir, ['--install']);

            writeFile(targetDir, 'extrapkg/package.json', JSON.stringify({ name: 'extrapkg', version: '3.0.0' }, null, 2));

            var pkgjson = JSON.parse(fs.readFileSync(path.join(targetDir, 'package.json'), 'utf8'));
            pkgjson.dependencies.extrapkg = 'file:extrapkg';
            fs.writeFileSync(path.join(targetDir, 'package.json'), JSON.stringify(pkgjson, null, 2));

            var res = runInstaller(targetDir, ['--install']);

            assert.equal(res.status, 0, diag(targetDir, res));
            assert.deepEqual(lockOf(targetDir).packages['node_modules/extrapkg'], { resolved: 'extrapkg', link: true },
                'the new dependency is in the lockfile now');
        });

        it('leaves the lockfile alone when a package is installed without --save', function () {
            var targetDir = makeProject();
            runInstaller(targetDir, ['--install']);

            writeFile(targetDir, 'extrapkg/package.json', JSON.stringify({ name: 'extrapkg', version: '3.0.0' }, null, 2));

            var before = fs.readFileSync(path.join(targetDir, 'package-lock.json'), 'utf8');
            var res = runInstaller(targetDir, ['--install', './extrapkg']);

            assert.equal(fs.existsSync(path.join(targetDir, 'node_modules', 'extrapkg')), true,
                'the package itself is installed: ' + diag(targetDir, res));
            assert.equal(fs.readFileSync(path.join(targetDir, 'package-lock.json'), 'utf8'), before,
                'npm does not record a package it was told not to save, and neither does this');
        });

        it('writes a lockfile npm ci accepts', { skip: !process.env.FIBJS_TEST_NETWORK }, function () {
            var targetDir = makeTargetDir();
            copyDir(path.join(FIXTURES_DIR, 'lock-basic-v3'), targetDir);
            fs.unlinkSync(path.join(targetDir, 'package-lock.json'));

            var res = runInstaller(targetDir, ['--install']);
            assert.equal(res.status, 0, diag(targetDir, res));

            // npm's own answer to "is this lockfile usable?": npm ci refuses one that
            // does not describe package.json, dev dependencies included
            var npmDir = makeTargetDir();
            copyDir(targetDir, npmDir);

            var npm = process.platform === 'win32' ? 'npm.cmd' : 'npm';
            var npmRes = child_process.spawnSync(npm, ['ci', '--no-audit', '--no-fund'], {
                cwd: npmDir,
                stdio: 'pipe',
                shell: process.platform === 'win32',
            });

            assert.equal(npmRes.status, 0, 'npm ci refused the lockfile fibjs wrote:\n' +
                String(npmRes.stderr || '') + String(npmRes.stdout || ''));
        });
    });

    // ===== Phase 5: .npmrc, a private registry and its certificate =====
    // a local https registry: the registry URL, the auth token and the certificate
    // handling are all exercised for real, and nothing leaves the machine
    if (isFibjs) describe('.npmrc + registry auth (Phase 5)', function () {
        var crypto = require('crypto');
        var http = require('http');
        var tls = require('tls');

        var server = null;
        var port = 0;
        var certPem = '';
        var requests = [];
        var tarball = null;
        var integrity = '';
        var shasum = '';
        var fixtureDir = '';

        before(function () {
            // a package packed the way npm packs one: `package/…` at the root
            fixtureDir = path.join(TMP_DIR, 'registry_' + Date.now() + '_' + Math.random().toString(36).slice(2));

            var pkgDir = path.join(fixtureDir, 'package');
            fs.mkdirSync(pkgDir, { recursive: true });
            fs.writeFileSync(path.join(pkgDir, 'package.json'), JSON.stringify({ name: 'fake-pkg', version: '1.0.0' }));
            fs.writeFileSync(path.join(pkgDir, 'index.js'), 'module.exports = 1;\n');

            var file = path.join(fixtureDir, 'fake-pkg-1.0.0.tgz');
            var res = child_process.spawnSync('tar', ['czf', file, '-C', fixtureDir, 'package'], { stdio: 'pipe' });
            assert.equal(res.status, 0, 'building the fixture needs tar: ' + String(res.stderr || ''));

            tarball = fs.readFileSync(file);
            integrity = 'sha512-' + crypto.createHash('sha512').update(tarball).digest('base64');
            shasum = crypto.createHash('sha1').update(tarball).digest('hex');

            var keys = crypto.generateKeyPairSync('rsa', { modulusLength: 2048 });
            var subject = { C: 'CN', O: 'fibjs', CN: 'localhost' };
            var cert = crypto.createCertificateRequest({ key: keys.privateKey, subject: subject })
                .issue({
                    key: keys.privateKey, issuer: subject,
                    notBefore: new Date(), notAfter: new Date(Date.now() + 86400000),
                });

            certPem = cert.pem;

            var keyPem = keys.privateKey.export({ format: 'pem' });

            function handler(r) {
                requests.push({ path: r.address, authorization: r.headers.get('authorization') });

                if (r.address === '/fake-pkg') {
                    r.response.write(JSON.stringify({
                        name: 'fake-pkg',
                        'dist-tags': { latest: '1.0.0' },
                        versions: {
                            '1.0.0': {
                                name: 'fake-pkg', version: '1.0.0',
                                dist: {
                                    tarball: 'https://localhost:' + port + '/fake-pkg/-/fake-pkg-1.0.0.tgz',
                                    integrity: integrity,
                                    shasum: shasum,
                                },
                            },
                        },
                    }));
                    return;
                }

                if (r.address === '/fake-pkg/-/fake-pkg-1.0.0.tgz') {
                    r.response.write(tarball);
                    return;
                }

                r.response.statusCode = 404;
                r.response.write('not found');
            }

            // a port that is free: `listen` throws when it is not, and another one is
            // a second away
            var context = tls.createSecureContext({ cert: certPem, key: keyPem, requestCert: false });

            for (var attempt = 0; attempt < 20 && !server; attempt++) {
                var candidate = 41000 + Math.floor(Math.random() * 4000);
                var candidateServer = http.createServer(context, handler);

                try {
                    candidateServer.listen(candidate);
                    port = candidate;
                    server = candidateServer;
                } catch (e) {
                    try { candidateServer.stop(); } catch (e2) { /* ignore */ }
                }
            }

            assert.ok(server, 'no free port for the local registry');
        });

        after(function () {
            if (server) server.stop();
            try { rmdirSync(fixtureDir); } catch (e) { /* ignore */ }
        });

        beforeEach(function () {
            requests = [];
        });

        /**
         * @description a project that depends on the local registry only. The proxy
         *              variables are cleared: this is a loopback connection
         */
        function makePrivateProject(npmrc) {
            var targetDir = makeTargetDir();

            fs.writeFileSync(path.join(targetDir, 'package.json'), JSON.stringify({
                name: 'private-proj', version: '1.0.0', dependencies: { 'fake-pkg': '1.0.0' },
            }, null, 2));
            fs.writeFileSync(path.join(targetDir, '.npmrc'), npmrc);

            return targetDir;
        }

        function runPrivate(targetDir) {
            return runInstaller(targetDir, ['--install'], {
                env: { HTTP_PROXY: '', HTTPS_PROXY: '', http_proxy: '', https_proxy: '', NO_PROXY: '*' },
            });
        }

        it('installs from the registry .npmrc names, trusting the certificate it gives', function () {
            var certFile = path.join(fixtureDir, 'ca.pem');
            fs.writeFileSync(certFile, certPem);

            var targetDir = makePrivateProject([
                'registry=https://localhost:' + port + '/',
                'cafile=' + certFile,
                '//localhost:' + port + '/:_authToken=secret-token',
            ].join('\n') + '\n');

            var res = runPrivate(targetDir);

            assert.equal(res.status, 0, diag(targetDir, res));
            assert.ok(fs.existsSync(path.join(targetDir, 'node_modules', 'fake-pkg', 'index.js')),
                'the package came from the private registry' + diag(targetDir, res));
            assert.ok(requests.length >= 2, 'metadata and tarball were fetched: ' + JSON.stringify(requests));
            requests.forEach(function (r) {
                assert.equal(r.authorization, 'Bearer secret-token',
                    'every request carries the token: ' + JSON.stringify(r));
            });
        });

        it('installs without a certificate when strict-ssl is off', function () {
            var targetDir = makePrivateProject([
                'registry=https://localhost:' + port + '/',
                'strict-ssl=false',
            ].join('\n') + '\n');

            var res = runPrivate(targetDir);

            assert.equal(res.status, 0, diag(targetDir, res));
            assert.ok(fs.existsSync(path.join(targetDir, 'node_modules', 'fake-pkg', 'index.js')), diag(targetDir, res));
        });

        it('refuses a certificate it was not told to trust', function () {
            var targetDir = makePrivateProject('registry=https://localhost:' + port + '/\n');

            var res = runPrivate(targetDir);

            assert.notEqual(res.status, 0, 'a self signed certificate is not trusted by default' + diag(targetDir, res));
            assert.ok(outputOf(res).indexOf('certificate verify failed') > -1, outputOf(res));
        });
    });

    // ===== Phase 7: command semantics =====
    // offline: every dependency is a local package, so what `omit` leaves on the
    // disk can be read off without a registry
    if (isFibjs) describe('cli semantics (Phase 7)', function () {
        function writeFile(dir, rel, content) {
            var p = path.join(dir, rel);
            fs.mkdirSync(path.dirname(p), { recursive: true });
            fs.writeFileSync(p, content);
        }

        function makeProject() {
            var targetDir = makeTargetDir();

            writeFile(targetDir, 'package.json', JSON.stringify({
                name: 'semantics-proj', version: '1.0.0',
                dependencies: { prodpkg: 'file:prodpkg' },
                devDependencies: { devpkg: 'file:devpkg' },
                optionalDependencies: { optpkg: 'file:optpkg' },
            }, null, 2));

            ['prodpkg', 'devpkg', 'optpkg'].forEach(function (n) {
                writeFile(targetDir, n + '/package.json', JSON.stringify({ name: n, version: '1.0.0' }, null, 2));
            });

            return targetDir;
        }

        function installed(targetDir, name) {
            return fs.existsSync(path.join(targetDir, 'node_modules', name));
        }

        it('installs devDependencies by default, like npm', function () {
            var targetDir = makeProject();
            var res = runInstaller(targetDir, ['--install']);

            assert.equal(res.status, 0, diag(targetDir, res));
            assert.ok(installed(targetDir, 'prodpkg'), diag(targetDir, res));
            assert.ok(installed(targetDir, 'devpkg'), 'dev dependencies are installed by default' + diag(targetDir, res));
            assert.ok(installed(targetDir, 'optpkg'), 'and so are the optional ones' + diag(targetDir, res));
        });

        it('leaves the dev dependencies out with --omit=dev', function () {
            var targetDir = makeProject();
            var res = runInstaller(targetDir, ['--install', '--omit=dev']);

            assert.equal(res.status, 0, diag(targetDir, res));
            assert.ok(installed(targetDir, 'prodpkg'), diag(targetDir, res));
            assert.equal(installed(targetDir, 'devpkg'), false);
            assert.ok(installed(targetDir, 'optpkg'), 'only dev was omitted' + diag(targetDir, res));
        });

        it('reads NODE_ENV=production as --omit=dev', function () {
            var targetDir = makeProject();
            var res = runInstaller(targetDir, ['--install'], { env: { NODE_ENV: 'production' } });

            assert.equal(res.status, 0, diag(targetDir, res));
            assert.equal(installed(targetDir, 'devpkg'), false);
            assert.ok(installed(targetDir, 'prodpkg'), diag(targetDir, res));
        });

        it('lets --include take a kind back', function () {
            var targetDir = makeProject();
            var res = runInstaller(targetDir, ['--install', '--omit=dev', '--include=dev']);

            assert.equal(res.status, 0, diag(targetDir, res));
            assert.ok(installed(targetDir, 'devpkg'), 'the last word on a kind wins' + diag(targetDir, res));
        });

        it('omits the optional dependencies with --omit=optional', function () {
            var targetDir = makeProject();
            var res = runInstaller(targetDir, ['--install', '--omit=optional']);

            assert.equal(res.status, 0, diag(targetDir, res));
            assert.equal(installed(targetDir, 'optpkg'), false);
            assert.ok(installed(targetDir, 'devpkg'), diag(targetDir, res));
        });

        it('warns about a dependency type it does not know', function () {
            var targetDir = makeProject();
            var res = runInstaller(targetDir, ['--install', '--omit=deev']);

            assert.equal(res.status, 0, diag(targetDir, res));
            assert.ok(outputOf(res).indexOf('unknown dependency type') > -1, outputOf(res));
        });

        it('--lockfile-only writes the lockfile and installs nothing', function () {
            var targetDir = makeProject();
            var res = runInstaller(targetDir, ['--install', '--lockfile-only']);

            assert.equal(res.status, 0, diag(targetDir, res));
            assert.ok(fs.existsSync(path.join(targetDir, 'package-lock.json')),
                'the lockfile is written' + diag(targetDir, res));
            assert.equal(fs.existsSync(path.join(targetDir, 'node_modules')), false,
                'and nothing is installed');
        });

        it('--dry-run reports what it would write and writes nothing', function () {
            var targetDir = makeProject();
            var res = runInstaller(targetDir, ['--install', '--dry-run']);

            assert.equal(res.status, 0, diag(targetDir, res));
            assert.ok(outputOf(res).indexOf('would write') > -1, outputOf(res));
            assert.equal(fs.existsSync(path.join(targetDir, 'package-lock.json')), false);
            assert.equal(fs.existsSync(path.join(targetDir, 'node_modules')), false);
        });

        it('--save records a local package with a file: spec', function () {
            // the local package is registered under its own name and recorded the way
            // npm records it, not under the path the caller typed
            var targetDir = makeProject();

            // a lockfile the single package install must not touch: it resolves only
            // that package, so a lockfile written from its tree would lose the rest
            runInstaller(targetDir, ['--install']);
            var lockBefore = fs.readFileSync(path.join(targetDir, 'package-lock.json'), 'utf8');

            var res = runInstaller(targetDir, ['--install', '--save', './prodpkg']);

            assert.equal(res.status, 0, diag(targetDir, res));

            var pkgjson = JSON.parse(fs.readFileSync(path.join(targetDir, 'package.json'), 'utf8'));
            assert.equal(pkgjson.dependencies.prodpkg, 'file:./prodpkg',
                'npm writes a local dependency as file:<path>: ' + JSON.stringify(pkgjson.dependencies));

            assert.equal(fs.readFileSync(path.join(targetDir, 'package-lock.json'), 'utf8'), lockBefore,
                'the lockfile is left for `fibjs --install` to update' + diag(targetDir, res));
            assert.ok(outputOf(res).indexOf('lockfile was left alone') > -1, outputOf(res));
        });

        it('brings the lockfile root entry back in sync with package.json', function () {
            // `--save` moves a spec in package.json; the tree does not move, so the
            // next install keeps every entry and only follows the root one
            var targetDir = makeProject();
            runInstaller(targetDir, ['--install', '--save', './prodpkg']);

            var res = runInstaller(targetDir, ['--install']);

            assert.equal(res.status, 0, diag(targetDir, res));

            var lock = JSON.parse(fs.readFileSync(path.join(targetDir, 'package-lock.json'), 'utf8'));
            assert.equal(lock.packages[''].dependencies.prodpkg, 'file:./prodpkg',
                'the root entry mirrors package.json: ' + JSON.stringify(lock.packages[''].dependencies));
        });

        it('agrees with npm about what --omit=dev leaves out', function () {            var ours = makeProject();
            var res = runInstaller(ours, ['--install', '--omit=dev']);
            assert.equal(res.status, 0, diag(ours, res));

            // npm's own answer, for the same project, in a second copy
            var theirs = makeProject();
            var npm = process.platform === 'win32' ? 'npm.cmd' : 'npm';
            var npmRes = child_process.spawnSync(npm, ['install', '--omit=dev', '--no-audit', '--no-fund'], {
                cwd: theirs,
                stdio: 'pipe',
                shell: process.platform === 'win32',
            });

            assert.equal(npmRes.status, 0, String(npmRes.stderr || ''));
            assert.equal(installed(ours, 'devpkg'), installed(theirs, 'devpkg'),
                'fibjs and npm have to agree about dev dependencies');
            assert.equal(installed(ours, 'prodpkg'), installed(theirs, 'prodpkg'),
                'and about production ones');
        });
    });

    // ===== Phase 8: the gaps npm's own test suite pointed at =====
    // (see plans/installer-lockfile-plan.md §12: npm/cli test/lib/commands/ci.js,
    // test/lib/utils/validate-lockfile.js, arborist's shrinkwrap.js and reify.js)
    if (isFibjs) describe('npm parity (Phase 8)', function () {
        function writeFile(dir, rel, content) {
            var p = path.join(dir, rel);
            fs.mkdirSync(path.dirname(p), { recursive: true });
            fs.writeFileSync(p, content);
        }

        function linkProject(pkgjson, extra) {
            var targetDir = makeTargetDir();

            fs.writeFileSync(path.join(targetDir, 'package.json'), JSON.stringify(pkgjson, null, 2));
            writeFile(targetDir, 'linkpkg/package.json', JSON.stringify({ name: 'linkpkg', version: '1.0.0' }, null, 2));

            if (extra)
                extra(targetDir);

            return targetDir;
        }

        function lockfileOf(targetDir) {
            return JSON.parse(fs.readFileSync(path.join(targetDir, 'package-lock.json'), 'utf8'));
        }

        it('runs the root preinstall before installing, like npm', function () {
            var targetDir = linkProject({
                name: 'pre-proj', version: '1.0.0',
                dependencies: { linkpkg: 'file:linkpkg' },
                scripts: {
                    preinstall: 'node -e "require(\'fs\').writeFileSync(\'marker.txt\', require(\'fs\').existsSync(\'node_modules\') ? \'after\' : \'before\')"',
                },
            });

            var res = runInstaller(targetDir, ['--install']);

            assert.equal(res.status, 0, diag(targetDir, res));
            assert.equal(fs.readFileSync(path.join(targetDir, 'marker.txt'), 'utf8'), 'before',
                'preinstall runs before the tree is built' + diag(targetDir, res));
        });

        it('stops when the root preinstall fails', function () {
            var targetDir = linkProject({
                name: 'pre-proj', version: '1.0.0',
                dependencies: { linkpkg: 'file:linkpkg' },
                scripts: { preinstall: 'node -e "process.exit(3)"' },
            });

            var res = runInstaller(targetDir, ['--install']);

            assert.notEqual(res.status, 0, 'a failing preinstall stops the install' + diag(targetDir, res));
            assert.equal(fs.existsSync(path.join(targetDir, 'node_modules')), false,
                'and nothing was installed' + diag(targetDir, res));
        });

        it('--ignore-scripts skips the preinstall entirely', function () {
            var targetDir = linkProject({
                name: 'pre-proj', version: '1.0.0',
                dependencies: { linkpkg: 'file:linkpkg' },
                scripts: { preinstall: 'node -e "require(\'fs\').writeFileSync(\'marker.txt\', \'ran\')"' },
            });

            var res = runInstaller(targetDir, ['--install', '--ignore-scripts']);

            assert.equal(res.status, 0, diag(targetDir, res));
            assert.equal(fs.existsSync(path.join(targetDir, 'marker.txt')), false, outputOf(res));
            assert.ok(checkInstalled(targetDir, 'linkpkg'), 'the install itself happened' + diag(targetDir, res));
        });

        it('keeps a lockfile entry nothing needs, without complaining', function () {
            // npm validates the tree against the lockfile, not the other way round
            var targetDir = linkProject({
                name: 'extra-proj', version: '1.0.0',
                dependencies: { linkpkg: 'file:linkpkg' },
            }, function (dir) {
                writeFile(dir, 'package-lock.json', JSON.stringify({
                    name: 'extra-proj', version: '1.0.0', lockfileVersion: 3, requires: true,
                    packages: {
                        '': { name: 'extra-proj', version: '1.0.0', dependencies: { linkpkg: 'file:linkpkg' } },
                        'linkpkg': { name: 'linkpkg', version: '1.0.0' },
                        'node_modules/linkpkg': { resolved: 'linkpkg', link: true },
                        'node_modules/ghost': { version: '9.9.9', resolved: 'https://registry.npmjs.org/ghost/-/ghost-9.9.9.tgz' },
                    },
                }, null, 2));
            });

            var res = runInstaller(targetDir, ['--install', '--ci']);

            assert.equal(res.status, 0, 'an extra lockfile entry is not an error' + diag(targetDir, res));
            assert.ok(checkInstalled(targetDir, 'linkpkg'), diag(targetDir, res));
        });

        it('refuses a lockfileVersion it does not know', function () {
            var targetDir = linkProject({ name: 'v9-proj', version: '1.0.0' }, function (dir) {
                writeFile(dir, 'package-lock.json', JSON.stringify({
                    name: 'v9-proj', version: '1.0.0', lockfileVersion: 9, packages: {},
                }, null, 2));
            });

            var res = runInstaller(targetDir, ['--install', '--ci']);

            assert.notEqual(res.status, 0, diag(targetDir, res));
            assert.ok(outputOf(res).indexOf('lockfileVersion 9 is not supported') > -1, outputOf(res));
        });

        it('does not touch node_modules when the lockfile is missing', function () {
            var targetDir = linkProject({ name: 'nolock-proj', version: '1.0.0' }, function (dir) {
                writeFile(dir, 'node_modules/keepme/package.json',
                    JSON.stringify({ name: 'keepme', version: '1.0.0' }, null, 2));
            });

            var res = runInstaller(targetDir, ['--install', '--ci']);

            assert.notEqual(res.status, 0, diag(targetDir, res));
            assert.ok(fs.existsSync(path.join(targetDir, 'node_modules', 'keepme', 'package.json')),
                'node_modules is left alone' + diag(targetDir, res));
        });

        it('writes a lockfile for a project without dependencies', function () {
            var targetDir = makeTargetDir();
            fs.writeFileSync(path.join(targetDir, 'package.json'),
                JSON.stringify({ name: 'empty-proj', version: '1.0.0' }, null, 2));

            var res = runInstaller(targetDir, ['--install']);

            assert.equal(res.status, 0, diag(targetDir, res));
            assert.deepEqual(Object.keys(lockfileOf(targetDir).packages), [''],
                'only the root entry: ' + JSON.stringify(lockfileOf(targetDir).packages));
        });

        it('ignores a nested npm-shrinkwrap.json', function () {
            var targetDir = linkProject({
                name: 'nested-proj', version: '1.0.0',
                dependencies: { linkpkg: 'file:linkpkg' },
            }, function (dir) {
                writeFile(dir, 'linkpkg/npm-shrinkwrap.json', JSON.stringify({
                    name: 'linkpkg', version: '9.9.9', lockfileVersion: 3, packages: {},
                }, null, 2));
            });

            var res = runInstaller(targetDir, ['--install']);

            assert.equal(res.status, 0, 'only the project root has a lockfile' + diag(targetDir, res));
            assert.ok(checkInstalled(targetDir, 'linkpkg'), diag(targetDir, res));
        });

        it('follows the indentation of package.json when writing the lockfile', function () {
            var targetDir = makeTargetDir();

            fs.writeFileSync(path.join(targetDir, 'package.json'),
                JSON.stringify({ name: 'indent-proj', version: '1.0.0' }, null, '\t') + '\n');
            writeFile(targetDir, 'linkpkg/package.json', JSON.stringify({ name: 'linkpkg', version: '1.0.0' }, null, 2));

            var res = runInstaller(targetDir, ['--install']);

            assert.equal(res.status, 0, diag(targetDir, res));
            var text = fs.readFileSync(path.join(targetDir, 'package-lock.json'), 'utf8');
            assert.ok(/\n\t"/.test(text), 'the lockfile is indented like package.json:\n' + text.slice(0, 200));
        });

        it('omits peer dependencies on request', function () {
            var targetDir = linkProject({
                name: 'peer-proj', version: '1.0.0',
                peerDependencies: { linkpkg: 'file:linkpkg' },
            });

            var withPeer = runInstaller(targetDir, ['--install']);
            assert.equal(withPeer.status, 0, diag(targetDir, withPeer));
            assert.ok(checkInstalled(targetDir, 'linkpkg'),
                'a peer dependency is installed by default' + diag(targetDir, withPeer));

            rmdirSync(path.join(targetDir, 'node_modules'));

            // a lockfile cannot say which entries are peers (npm does not mark them),
            // so this is about the resolve path
            var without = runInstaller(targetDir, ['--install', '--no-package-lock', '--omit=peer']);
            assert.equal(without.status, 0, diag(targetDir, without));
            assert.equal(fs.existsSync(path.join(targetDir, 'node_modules', 'linkpkg')), false,
                '--omit=peer leaves it out' + diag(targetDir, without));
        });

        it('skips an optional dependency for another libc', function () {
            if (process.versions.musl)
                return;     // the case is written for glibc hosts

            var targetDir = makeTargetDir();

            fs.writeFileSync(path.join(targetDir, 'package.json'), JSON.stringify({
                name: 'libc-proj', version: '1.0.0',
                optionalDependencies: { 'musl-pkg': '1.0.0' },
            }, null, 2));
            writeFile(targetDir, 'package-lock.json', JSON.stringify({
                name: 'libc-proj', version: '1.0.0', lockfileVersion: 3, requires: true,
                packages: {
                    '': { name: 'libc-proj', version: '1.0.0', optionalDependencies: { 'musl-pkg': '1.0.0' } },
                    'node_modules/musl-pkg': {
                        version: '1.0.0', optional: true, libc: ['musl'],
                        resolved: 'https://registry.npmjs.org/musl-pkg/-/musl-pkg-1.0.0.tgz',
                    },
                },
            }, null, 2));

            var res = runInstaller(targetDir, ['--install', '--ci']);

            assert.equal(res.status, 0, diag(targetDir, res));
            assert.equal(fs.existsSync(path.join(targetDir, 'node_modules', 'musl-pkg')), false,
                'the lockfile says musl and this is not musl' + diag(targetDir, res));
        });

        it('reports devOptional for a package both dev and optional need', function () {
            var targetDir = linkProject({
                name: 'devopt-proj', version: '1.0.0',
                devDependencies: { linkpkg: 'file:linkpkg' },
                optionalDependencies: { linkpkg: 'file:linkpkg' },
            });

            var res = runInstaller(targetDir, ['--install']);

            assert.equal(res.status, 0, diag(targetDir, res));

            var entry = lockfileOf(targetDir).packages['node_modules/linkpkg'];
            assert.equal(entry.devOptional, true, JSON.stringify(entry));
            assert.equal(entry.dev, undefined,
                'npm does not mark dev when the entry is also optional: ' + JSON.stringify(entry));
        });

        it('--omit=dev twice does not move the lockfile the second time', function () {
            var targetDir = linkProject({
                name: 'stable-proj', version: '1.0.0',
                dependencies: { linkpkg: 'file:linkpkg' },
                devDependencies: { devpkg: 'file:devpkg' },
            }, function (dir) {
                writeFile(dir, 'devpkg/package.json', JSON.stringify({ name: 'devpkg', version: '2.0.0' }, null, 2));
            });

            var first = runInstaller(targetDir, ['--install', '--omit=dev']);
            assert.equal(first.status, 0, diag(targetDir, first));

            var before = fs.readFileSync(path.join(targetDir, 'package-lock.json'), 'utf8');
            var second = runInstaller(targetDir, ['--install', '--omit=dev']);

            assert.equal(second.status, 0, diag(targetDir, second));
            assert.equal(fs.readFileSync(path.join(targetDir, 'package-lock.json'), 'utf8'), before,
                'omitting a type is about the disk, so the lockfile does not move');
        });

        it('--ci --ignore-scripts installs without running anything', function () {
            var targetDir = linkProject({
                name: 'noscript-proj', version: '1.0.0',
                dependencies: { scriptpkg: 'file:scriptpkg' },
                scripts: { preinstall: 'node -e "require(\'fs\').writeFileSync(\'root-ran.txt\', \'1\')"' },
            }, function (dir) {
                writeFile(dir, 'scriptpkg/package.json', JSON.stringify({
                    name: 'scriptpkg', version: '1.0.0',
                    scripts: { postinstall: 'node -e "require(\'fs\').writeFileSync(\'pkg-ran.txt\', \'1\')"' },
                }, null, 2));
                writeFile(dir, 'package-lock.json', JSON.stringify({
                    name: 'noscript-proj', version: '1.0.0', lockfileVersion: 3, requires: true,
                    packages: {
                        '': { name: 'noscript-proj', version: '1.0.0', dependencies: { scriptpkg: 'file:scriptpkg' } },
                        'scriptpkg': { name: 'scriptpkg', version: '1.0.0', hasInstallScript: true },
                        'node_modules/scriptpkg': { resolved: 'scriptpkg', link: true },
                    },
                }, null, 2));
            });

            var res = runInstaller(targetDir, ['--install', '--ci', '--ignore-scripts']);

            assert.equal(res.status, 0, diag(targetDir, res));
            assert.equal(fs.existsSync(path.join(targetDir, 'root-ran.txt')), false, outputOf(res));
            assert.equal(fs.existsSync(path.join(targetDir, 'scriptpkg', 'pkg-ran.txt')), false, outputOf(res));
            assert.ok(checkInstalled(targetDir, 'scriptpkg'), diag(targetDir, res));
        });

        it('links the binaries of an installed package', function () {
            var targetDir = linkProject({
                name: 'bin-proj', version: '1.0.0',
                dependencies: { binpkg: 'file:binpkg' },
            }, function (dir) {
                writeFile(dir, 'binpkg/package.json', JSON.stringify({
                    name: 'binpkg', version: '1.0.0',
                    bin: { 'binpkg-cli': 'cli.js' },
                }, null, 2));
                writeFile(dir, 'binpkg/cli.js', '#!/usr/bin/env node\nconsole.log("hi");\n');
            });

            var res = runInstaller(targetDir, ['--install']);

            assert.equal(res.status, 0, diag(targetDir, res));

            var binDir = path.join(targetDir, 'node_modules', '.bin');
            assert.ok(fs.existsSync(path.join(binDir, 'binpkg-cli')) ||
                fs.existsSync(path.join(binDir, 'binpkg-cli.cmd')),
                'the binary is linked into node_modules/.bin: ' + listDir(binDir) + diag(targetDir, res));
        });

        it('installs a workspace member from the lockfile', function () {
            var targetDir = makeTargetDir();

            fs.writeFileSync(path.join(targetDir, 'package.json'), JSON.stringify({
                name: 'ws-proj', version: '1.0.0', workspaces: ['packages/*'],
            }, null, 2));
            writeFile(targetDir, 'packages/member/package.json', JSON.stringify({
                name: 'ws-member', version: '1.0.0', dependencies: { linkpkg: 'file:../../linkpkg' },
            }, null, 2));
            writeFile(targetDir, 'linkpkg/package.json', JSON.stringify({ name: 'linkpkg', version: '1.0.0' }, null, 2));
            writeFile(targetDir, 'package-lock.json', JSON.stringify({
                name: 'ws-proj', version: '1.0.0', lockfileVersion: 3, requires: true,
                packages: {
                    '': { name: 'ws-proj', version: '1.0.0', workspaces: ['packages/*'] },
                    'linkpkg': { name: 'linkpkg', version: '1.0.0' },
                    'node_modules/linkpkg': { resolved: 'linkpkg', link: true },
                    'node_modules/ws-member': { resolved: 'packages/member', link: true },
                    'packages/member': {
                        name: 'ws-member', version: '1.0.0',
                        dependencies: { linkpkg: 'file:../../linkpkg' },
                    },
                },
            }, null, 2));

            var res = runInstaller(targetDir, ['--install', '--ci']);

            assert.equal(res.status, 0, diag(targetDir, res));
            assert.ok(fs.existsSync(path.join(targetDir, 'node_modules', 'ws-member')),
                'the workspace member is linked' + diag(targetDir, res));
            assert.ok(fs.existsSync(path.join(targetDir, 'node_modules', 'linkpkg')),
                'and so is what it depends on' + diag(targetDir, res));
        });
    });

    // ===== Phase 0: command line =====
    describe('cli arguments (Phase 0)', function () {
        var cliArgumentTests = [
            {
                description: 'an unknown option is rejected instead of ignored',
                args: ['--install', '--whatever'],
                expectStatus: 1,
                expectOutput: 'unknown option: --whatever',
            },
            {
                // the point of the whole exercise: a flag nobody implements must not
                // look like it worked. `--frozen-lockfile` is implemented now, so it
                // says what it needs instead of pretending
                description: '--frozen-lockfile asks for a lockfile when there is none',
                args: ['--install', '--frozen-lockfile'],
                expectStatus: 1,
                expectOutput: 'needs a package-lock.json',
            },
            {
                description: '--ci is an alias of --frozen-lockfile',
                args: ['--install', '--ci'],
                expectStatus: 1,
                expectOutput: 'needs a package-lock.json',
            },
            {
                description: '--target without a value is an error',
                args: ['--install', '--target'],
                expectStatus: 1,
                expectOutput: 'needs a value',
            },
            {
                description: 'npm only flags are accepted and ignored with a notice',
                args: ['--install', '--no-audit', '--no-fund', '--force', '--silent'],
                expectStatus: 0,
                expectOutput: 'ignored',
            },
            {
                description: '--production is accepted (npm compatibility)',
                args: ['--install', '--production'],
                expectStatus: 0,
            },
            {
                description: '--ignore-scripts is accepted',
                args: ['--install', '--ignore-scripts'],
                expectStatus: 0,
            },
            {
                // the flag plumbing: without a package the installer records nothing,
                // and must not fail on the flag either
                description: '--save is accepted',
                args: ['--install', '--save'],
                expectStatus: 0,
                verify: function (targetDir) {
                    var pkgjson = JSON.parse(fs.readFileSync(path.join(targetDir, 'package.json'), 'utf8'));
                    assert.equal(pkgjson.dependencies, undefined,
                        'nothing is recorded when no package is given');
                },
            },
            {
                description: '-D (--save-dev) is accepted',
                args: ['--install', '-D'],
                expectStatus: 0,
                verify: function (targetDir) {
                    var pkgjson = JSON.parse(fs.readFileSync(path.join(targetDir, 'package.json'), 'utf8'));
                    assert.equal(pkgjson.devDependencies, undefined,
                        'nothing is recorded when no package is given');
                },
            },
        ];

        cliArgumentTests.forEach(function (test) {
            // the command line under test is fibjs's own: npm parses its arguments
            // differently, so these cases never run on the npm side
            var shouldSkip = test.skip || !isFibjs;

            it(test.description, { skip: shouldSkip }, function () {
                var targetDir = makeTargetDir();
                createPackageJson(targetDir, { name: 'cli-args-target', version: '1.0.0' });

                var args = test.args;
                var res = runInstaller(targetDir, args);
                var out = outputOf(res);

                assert.equal(res.status, test.expectStatus,
                    'exit code for ' + args.join(' ') + diag(targetDir, res));

                if (test.expectOutput)
                    assert.ok(out.indexOf(test.expectOutput) > -1,
                        'expected "' + test.expectOutput + '" in the output' + diag(targetDir, res));

                if (test.verify)
                    test.verify(targetDir, res);
            });
        });
    });
});

// ---------- lockfile helper (Phase 0, fibjs-only) ----------
//
// the helper is embedded in the binary like the installer itself, so it is loaded
// by its internal name: that is also the copy that ships

if (isFibjs) (function () {
    var lockfile = require('internal/helpers/lockfile');

    var SRI_ABC_SHA512 = 'sha512-3a81oZNherrMQXNJriBBMRLm+k6JqX6iCp7u5ktV05ohkpkqJ0/BqDa6PCOj/uu9RU1EI2Q86A4qmslPpUyknw==';
    var SRI_ABC_SHA1 = 'sha1-qZk+NkcGgWq6PiVxeFDCbJzQ2J0=';
    var SRI_ABC_SHA1_HEX = 'sha1-a9993e364706816aba3e25717850c26c9cd0d89d';

    function buf(s) {
        return Buffer.from ? Buffer.from(s) : new Buffer(s);
    }

    describe('lockfile helper: integrity (Phase 0)', function () {
        it('accepts a matching sha512 integrity', function () {
            var res = lockfile.verify_integrity(buf('abc'), SRI_ABC_SHA512);
            assert.equal(res.checked, true);
            assert.equal(res.ok, true, res.expected + ' vs ' + res.actual);
            assert.equal(res.algorithm, 'sha512');
        });

        it('rejects a tampered digest', function () {
            var res = lockfile.verify_integrity(buf('abc'), SRI_ABC_SHA512.replace('3a81', '0000'));
            assert.equal(res.ok, false);
        });

        it('checks only the strongest algorithm present', function () {
            // the sha1 digest is wrong on purpose: an entry that also carries a
            // sha512 is verified against the sha512, the way npm does it
            var res = lockfile.verify_integrity(buf('abc'),
                SRI_ABC_SHA1_HEX.replace('a999', '0000') + ' ' + SRI_ABC_SHA512);
            assert.equal(res.algorithm, 'sha512');
            assert.equal(res.ok, true);
        });

        it('accepts a hex digest (the dist.shasum fallback)', function () {
            assert.equal(lockfile.verify_integrity(buf('abc'), SRI_ABC_SHA1_HEX).ok, true);
        });

        it('accepts a base64 sha1 digest', function () {
            assert.equal(lockfile.verify_integrity(buf('abc'), SRI_ABC_SHA1).ok, true);
        });

        it('ignores the ?options suffix', function () {
            assert.equal(lockfile.verify_integrity(buf('abc'), SRI_ABC_SHA512 + '?foo=bar').ok, true);
        });

        it('reports nothing to check when the entry carries no integrity', function () {
            assert.equal(lockfile.verify_integrity(buf('abc'), '').checked, false);
            assert.equal(lockfile.verify_integrity(buf('abc'), undefined).checked, false);
        });
    });

    describe('lockfile helper: registry replacement (Phase 0)', function () {
        var tarball = 'https://registry.npmjs.org/is-number/-/is-number-6.0.0.tgz';

        it('serves the default registry url from the configured registry', function () {
            assert.equal(
                lockfile.apply_registry_replace(tarball, 'https://registry.npmmirror.com'),
                'https://registry.npmmirror.com/is-number/-/is-number-6.0.0.tgz');
        });

        it('keeps a url whose host is not the magic one', function () {
            var other = 'https://example.com/is-number/-/is-number-6.0.0.tgz';
            assert.equal(lockfile.apply_registry_replace(other, 'https://registry.npmmirror.com'), other);
        });

        it('never replaces with policy never', function () {
            assert.equal(
                lockfile.apply_registry_replace(tarball, 'https://registry.npmmirror.com', 'never'),
                tarball);
        });

        it('replaces any host with policy always', function () {
            assert.equal(
                lockfile.apply_registry_replace('https://example.com/x.tgz', 'https://mirror.test', 'always'),
                'https://mirror.test/x.tgz');
        });

        it('replaces the host a policy names', function () {
            assert.equal(
                lockfile.apply_registry_replace('https://npm.example.com/x.tgz', 'https://mirror.test', 'npm.example.com'),
                'https://mirror.test/x.tgz');
        });

        it('leaves git and file entries alone', function () {
            var git = 'git+ssh://git@github.com/o/r.git#abc';
            var file = 'packages/a';

            assert.equal(lockfile.apply_registry_replace(git, 'https://mirror.test', 'always'), git);
            assert.equal(lockfile.apply_registry_replace(file, 'https://mirror.test', 'always'), file);
        });
    });

    describe('lockfile helper: reading (Phase 0)', function () {
        var lockDirs = [];

        function writeLock(content, filename) {
            var dir = path.join(TMP_DIR, 'lock_' + Date.now() + '_' + Math.random().toString(36).slice(2));
            fs.mkdirSync(dir, { recursive: true });
            lockDirs.push(dir);
            fs.writeFileSync(path.join(dir, filename || 'package-lock.json'), JSON.stringify(content));
            return dir;
        }

        function makeEmptyDir() {
            var dir = path.join(TMP_DIR, 'lock_empty_' + Date.now() + '_' + Math.random().toString(36).slice(2));
            fs.mkdirSync(dir, { recursive: true });
            lockDirs.push(dir);
            return dir;
        }

        afterEach(function () {
            lockDirs.forEach(function (d) { try { rmdirSync(d); } catch (e) { /* ignore */ } });
            lockDirs = [];
        });

        it('reads a v3 lock (path table, no legacy tree)', function () {
            var dir = writeLock({
                name: 'x',
                lockfileVersion: 3,
                requires: true,
                packages: { '': { name: 'x' }, 'node_modules/a': { version: '1.0.0' } }
            });
            var lock = lockfile.read_lockfile(dir);

            assert.equal(lock.lockfileVersion, 3);
            assert.equal(lock.packages['node_modules/a'].version, '1.0.0');
            assert.equal(lock.legacy, null);
            assert.equal(lock.root.name, 'x');
        });

        it('reads a v2 lock (path table plus the legacy tree)', function () {
            var dir = writeLock({
                name: 'x',
                lockfileVersion: 2,
                packages: { 'node_modules/a': { version: '1.0.0' } },
                dependencies: { a: { version: '1.0.0' } }
            });
            var lock = lockfile.read_lockfile(dir);

            assert.equal(lock.lockfileVersion, 2);
            assert.equal(lock.packages['node_modules/a'].version, '1.0.0');
            assert.equal(lock.legacy.a.version, '1.0.0');
        });

        it('reads a v1 lock (legacy tree only)', function () {
            var dir = writeLock({ name: 'x', lockfileVersion: 1, dependencies: { a: { version: '1.0.0' } } });
            var lock = lockfile.read_lockfile(dir);

            assert.equal(lock.lockfileVersion, 1);
            assert.equal(lock.legacy.a.version, '1.0.0');
            assert.deepEqual(Object.keys(lock.packages), []);
        });

        it('prefers npm-shrinkwrap.json over package-lock.json', function () {
            var dir = writeLock({ name: 'x', lockfileVersion: 3, packages: {} }, 'npm-shrinkwrap.json');
            fs.writeFileSync(path.join(dir, 'package-lock.json'),
                JSON.stringify({ name: 'x', lockfileVersion: 3, packages: {} }));

            assert.equal(lockfile.read_lockfile(dir).filename, 'npm-shrinkwrap.json');
        });

        it('returns null when the project has no lock', function () {
            assert.equal(lockfile.read_lockfile(makeEmptyDir()), null);
        });

        it('reports a lock that is not valid JSON', function () {
            var dir = makeEmptyDir();
            fs.writeFileSync(path.join(dir, 'package-lock.json'), '{ not json');

            assert.throws(function () { lockfile.read_lockfile(dir); }, /not valid JSON/);
        });

        it('accepts the versions it can read and refuses the rest', function () {
            assert.equal(lockfile.supported_version(1), true);
            assert.equal(lockfile.supported_version(2), true);
            assert.equal(lockfile.supported_version(3), true);
            assert.equal(lockfile.supported_version(0), false);
            assert.equal(lockfile.supported_version(4), false);
        });
    });
    describe('lockfile helper: path table (Phase 1)', function () {
        it('derives names from the paths and skips the root entry', function () {
            var map = lockfile.to_path_map({
                lockfileVersion: 3,
                packages: {
                    '': { name: 'root' },
                    'node_modules/a': { version: '1.0.0' },
                    'node_modules/@s/b': { version: '2.0.0' },
                    'node_modules/a/node_modules/c': { version: '3.0.0' },
                    'packages/local': { name: 'local', version: '1.0.0' },
                },
            });

            assert.equal(map[''], undefined, 'the root entry is not a package');
            assert.equal(map['node_modules/a'].name, 'a');
            assert.equal(map['node_modules/@s/b'].name, '@s/b');
            assert.equal(map['node_modules/a/node_modules/c'].name, 'c');
            assert.equal(map['node_modules/a'].target_only, false);
            assert.equal(map['packages/local'].target_only, true, 'a link target installs nothing');
        });

        it('converts a v1 lock into a path table', function () {
            var map = lockfile.to_path_map({
                lockfileVersion: 1,
                dependencies: {
                    a: {
                        version: '1.0.0',
                        requires: { c: '^3.0.0' },
                        dependencies: { c: { version: '3.0.0' } },
                    },
                },
            });

            assert.equal(map['node_modules/a'].version, '1.0.0');
            assert.equal(map['node_modules/a/node_modules/c'].version, '3.0.0');
            // v1 keeps the declared ranges under `requires`
            assert.deepEqual(map['node_modules/a'].dependencies, { c: '^3.0.0' });
        });

        it('keeps link entries as links', function () {
            var map = lockfile.to_path_map({
                lockfileVersion: 3,
                packages: {
                    'node_modules/test-pkg': { resolved: 'fixtures/pkg', link: true },
                    'fixtures/pkg': { name: 'test-pkg', version: '1.0.0' },
                },
            });

            assert.equal(map['node_modules/test-pkg'].link, true);
            assert.equal(map['node_modules/test-pkg'].name, 'test-pkg');
            assert.equal(map['node_modules/test-pkg'].resolved, 'fixtures/pkg');
        });
    });

    describe('lockfile helper: lookup and spec satisfaction (Phase 1)', function () {
        var semver = require('internal/helpers/semver');

        function map_of(packages) {
            return lockfile.to_path_map({ lockfileVersion: 3, packages: packages });
        }

        it('prefers the root entry, then the shallowest satisfying nested one', function () {
            var packages = map_of({
                'node_modules/b': { version: '1.0.0' },
                'node_modules/a/node_modules/b': { version: '2.0.0' },
            });

            assert.equal(lockfile.lookup_entry(packages, 'b', '^1.0.0', semver).version, '1.0.0');
            assert.equal(lockfile.lookup_entry(packages, 'b', '^2.0.0', semver).version, '2.0.0');
        });

        it('returns the root entry so a mismatch is reported as Invalid', function () {
            var packages = map_of({ 'node_modules/b': { version: '1.0.0' } });
            var entry = lockfile.lookup_entry(packages, 'b', '^2.0.0', semver);

            assert.equal(entry.version, '1.0.0');
            assert.equal(lockfile.spec_satisfied('^2.0.0', entry, semver).ok, false);
        });

        it('returns nothing when the package is absent', function () {
            assert.equal(lockfile.lookup_entry(map_of({}), 'b', '^1.0.0', semver), null);
        });

        it('checks a semver range against the locked version', function () {
            var entry = map_of({ 'node_modules/b': { version: '1.4.0' } })['node_modules/b'];

            assert.equal(lockfile.spec_satisfied('^1.0.0', entry, semver).ok, true);
            assert.equal(lockfile.spec_satisfied('~1.0.0', entry, semver).ok, false);
        });

        it('checks an npm: alias by name and range', function () {
            var map = map_of({ 'node_modules/alias': { name: 'real', version: '1.2.0' } });
            var entry = map['node_modules/alias'];

            assert.equal(lockfile.spec_satisfied('npm:real@^1.0.0', entry, semver).ok, true);
            assert.equal(lockfile.spec_satisfied('npm:other@^1.0.0', entry, semver).ok, false);
            assert.equal(lockfile.spec_satisfied('npm:real@^2.0.0', entry, semver).ok, false);
        });

        it('checks a file: spec against the target the link points at', function () {
            var map = map_of({
                'node_modules/local': { resolved: 'fixtures/pkg-local', link: true },
                'fixtures/pkg-local': { name: 'local', version: '1.0.0' },
            });
            var entry = map['node_modules/local'];

            assert.equal(lockfile.spec_satisfied('file:fixtures/pkg-local', entry, semver).ok, true);
            assert.equal(lockfile.spec_satisfied('file:./fixtures/pkg-local', entry, semver).ok, true);
            assert.equal(lockfile.spec_satisfied('file:fixtures/moved-away', entry, semver).ok, false,
                'a stale target must not look in sync');
            assert.equal(lockfile.spec_satisfied('./fixtures/pkg-local', entry, semver).ok, true);
        });

        it('requires a link for a workspace: spec', function () {
            var link = map_of({ 'node_modules/w': { resolved: 'packages/w', link: true } })['node_modules/w'];
            var plain = map_of({ 'node_modules/w': { version: '1.0.0' } })['node_modules/w'];

            assert.equal(lockfile.spec_satisfied('workspace:*', link, semver).ok, true);
            assert.equal(lockfile.spec_satisfied('workspace:*', plain, semver).ok, false);
        });

        it('checks a git spec against the pinned repository', function () {
            var sha = '98e8ff1da1a89f93d1397a24d7413ed15421c139';
            var pinned = map_of({
                'node_modules/g': { version: '7.0.0', resolved: 'git+ssh://git@github.com/o/r.git#' + sha },
            })['node_modules/g'];
            var branch = map_of({
                'node_modules/g': { version: '7.0.0', resolved: 'git+ssh://git@github.com/o/r.git#main' },
            })['node_modules/g'];

            assert.equal(lockfile.spec_satisfied('github:o/r', pinned, semver).ok, true);
            assert.equal(lockfile.spec_satisfied('github:o/r#semver:^7.0.0', pinned, semver).ok, true);
            assert.equal(lockfile.spec_satisfied('github:other/r', pinned, semver).ok, false);
            assert.equal(lockfile.spec_satisfied('github:o/r', branch, semver).ok, false,
                'an entry pinned to a branch is not frozen');
        });

        it('accepts a dist-tag on presence alone', function () {
            var entry = map_of({ 'node_modules/t': { version: '1.0.0' } })['node_modules/t'];
            assert.equal(lockfile.spec_satisfied('latest', entry, semver).ok, true);
        });
    });

    describe('lockfile helper: sync check (Phase 1)', function () {
        var semver = require('internal/helpers/semver');

        function lock_of(packages, extra) {
            return Object.assign({ lockfileVersion: 3, packages: packages, legacy: null }, extra || {});
        }

        var clean = lock_of({
            '': { name: 'root', dependencies: { a: '^1.0.0' } },
            'node_modules/a': { version: '1.5.0', resolved: 'https://registry.npmjs.org/a/-/a-1.5.0.tgz' },
        });
        var clean_pkgjson = { name: 'root', dependencies: { a: '^1.0.0' } };

        it('accepts a lock that matches package.json', function () {
            var res = lockfile.check_sync(clean_pkgjson, clean, { semver: semver });
            assert.equal(res.ok, true, JSON.stringify(res.errors));
            assert.deepEqual(res.errors, []);
        });

        it('reports Invalid when the range moved past the lock', function () {
            var res = lockfile.check_sync({ name: 'root', dependencies: { a: '^2.0.0' } }, clean, { semver: semver });

            assert.equal(res.ok, false);
            assert.equal(res.errors[0].kind, 'Invalid');
            assert.equal(res.errors[0].name, 'a');
            assert.equal(res.errors[0].locked, '1.5.0');
        });

        it('reports Missing when package.json grew a dependency', function () {
            var res = lockfile.check_sync(
                { name: 'root', dependencies: { a: '^1.0.0', b: '^1.0.0' } }, clean, { semver: semver });

            assert.equal(res.ok, false);
            assert.equal(res.errors[0].kind, 'Missing');
            assert.equal(res.errors[0].name, 'b');
        });

        it('reports Missing when the lock lost an entry its parent declares', function () {
            var broken = lock_of({
                'node_modules/a': { version: '1.5.0', dependencies: { c: '^3.0.0' } },
            });
            var res = lockfile.check_sync({ name: 'root', dependencies: { a: '^1.0.0' } }, broken, { semver: semver });

            assert.equal(res.ok, false);
            assert.equal(res.errors[0].kind, 'Missing');
            assert.equal(res.errors[0].name, 'c');
            assert.equal(res.errors[0].via, 'node_modules/a');
        });

        it('only warns about an optional dependency the lock lost', function () {
            var broken = lock_of({
                'node_modules/a': { version: '1.5.0', optionalDependencies: { c: '^3.0.0' } },
            });
            var res = lockfile.check_sync({ name: 'root', dependencies: { a: '^1.0.0' } }, broken, { semver: semver });

            assert.equal(res.ok, true, JSON.stringify(res.errors));
            assert.equal(res.warnings.length, 1);
            assert.equal(res.warnings[0].name, 'c');
        });

        it('checks the workspace members too', function () {
            var lock = lock_of({ 'node_modules/a': { version: '1.5.0' } });
            var res = lockfile.check_sync({ name: 'root' }, lock, {
                semver: semver,
                workspaces: [{ name: 'w', path: 'packages/w', pkgjson: { name: 'w', dependencies: { a: '^2.0.0' } } }],
            });

            assert.equal(res.ok, false);
            assert.equal(res.errors[0].via, 'workspace w');
            assert.equal(res.errors[0].kind, 'Invalid');
        });

        it('skips bundled dependencies (they live inside the tarball)', function () {
            var bundled = lock_of({
                'node_modules/a': { version: '1.5.0', inBundle: true, dependencies: { c: '^3.0.0' } },
            });
            var res = lockfile.check_sync({ name: 'root', dependencies: { a: '^1.0.0' } }, bundled, { semver: semver });

            assert.equal(res.ok, true, JSON.stringify(res.errors));
        });

        it('reports a v1 closure miss as a warning, not as a failure', function () {
            // a v1 `requires` mixes dependency kinds, so it cannot block the install
            var v1 = { lockfileVersion: 1, packages: {}, legacy: { a: { version: '1.5.0', requires: { c: '^3.0.0' } } } };
            var res = lockfile.check_sync({ name: 'root', dependencies: { a: '^1.0.0' } }, v1, { semver: semver });

            assert.equal(res.ok, true, JSON.stringify(res.errors));
            assert.equal(res.warnings.length, 1);
            assert.equal(res.warnings[0].name, 'c');
        });
    });

    describe('lockfile helper: .npmrc (Phase 1)', function () {
        var npmrcDirs = [];

        function makeDir() {
            var dir = path.join(TMP_DIR, 'npmrc_' + Date.now() + '_' + Math.random().toString(36).slice(2));
            fs.mkdirSync(dir, { recursive: true });
            npmrcDirs.push(dir);
            return dir;
        }

        afterEach(function () {
            npmrcDirs.forEach(function (d) { try { rmdirSync(d); } catch (e) { /* ignore */ } });
            npmrcDirs = [];
        });

        it('reads the registry, the scope registries and the tokens', function () {
            var dir = makeDir();
            fs.writeFileSync(path.join(dir, '.npmrc'), [
                '; a comment',
                'registry=https://registry.npmmirror.com',
                '@scope:registry=https://npm.example.com/',
                '//npm.example.com/:_authToken=secret-token',
            ].join('\n'));

            var npmrc = lockfile.read_npmrc(dir, { env: { HOME: path.join(dir, 'no-home') } });

            assert.equal(npmrc.registry, 'https://registry.npmmirror.com');
            assert.equal(npmrc.scoped['@scope'], 'https://npm.example.com/');
            assert.deepEqual(npmrc.auth['//npm.example.com/'], { authToken: 'secret-token' });
        });

        it('expands ${VAR} and strips quotes', function () {
            var dir = makeDir();
            fs.writeFileSync(path.join(dir, '.npmrc'), '//npm.example.com/:_authToken="${MY_TOKEN}"\n');

            var npmrc = lockfile.read_npmrc(dir, { env: { HOME: dir, MY_TOKEN: 'from-env' } });
            assert.equal(npmrc.auth['//npm.example.com/'].authToken, 'from-env');
        });

        it('lets the environment override the files', function () {
            var dir = makeDir();
            fs.writeFileSync(path.join(dir, '.npmrc'), 'registry=https://from-file\n');

            var npmrc = lockfile.read_npmrc(dir, {
                env: { HOME: dir, npm_config_registry: 'https://from-env' },
            });

            assert.equal(npmrc.registry, 'https://from-env');
        });

        it('lets the project file override the user file', function () {
            var home = makeDir();
            var project = makeDir();
            fs.writeFileSync(path.join(home, '.npmrc'), 'registry=https://from-user\n');
            fs.writeFileSync(path.join(project, '.npmrc'), 'registry=https://from-project\n');

            assert.equal(lockfile.read_npmrc(project, { env: { HOME: home } }).registry, 'https://from-project');
        });

        it('matches a token by the longest url prefix', function () {
            var auth = {
                '//npm.example.com/': { authToken: 'host-token' },
                '//npm.example.com/scoped/': { authToken: 'path-token' },
            };

            assert.equal(lockfile.auth_for_url('https://npm.example.com/a/-/a-1.0.0.tgz', auth).authToken, 'host-token');
            assert.equal(lockfile.auth_for_url('https://npm.example.com/scoped/a/-/a-1.0.0.tgz', auth).authToken, 'path-token');
            assert.equal(lockfile.auth_for_url('https://other.example.com/a.tgz', auth), null);
        });

        it('builds the authorization header', function () {
            var auth = { '//npm.example.com/': { authToken: 't' } };
            assert.deepEqual(lockfile.auth_header_for_url('https://npm.example.com/a.tgz', auth),
                { authorization: 'Bearer t' });

            var basic = { '//npm.example.com/': { username: 'u', password: 'p' } };
            assert.deepEqual(lockfile.auth_header_for_url('https://npm.example.com/a.tgz', basic),
                { authorization: 'Basic ' + Buffer.from('u:p').toString('base64') });
        });
    });
})(); // end lockfile helper tests
