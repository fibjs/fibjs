var { describe, it, before, after, beforeEach, afterEach } = require('node:test');
var assert = require('assert');
var fs = require('fs');
var path = require('path');
var child_process = require('child_process');

// Detect runtime: fibjs or node
const isFibjs = typeof process !== 'undefined' && process.versions && process.versions.fibjs;
const isWin32 = process.platform === 'win32';

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

            // the fixture's optional carries os:["linux"], cpu:["x64"]: a foreign
            // platform optional everywhere but linux/x64, and there it lands at its
            // scoped path (node_modules/@esbuild/linux-x64, never a doubled scope
            // such as @esbuild/@esbuild/linux-x64, which treeOf would hide)
            if (process.platform === 'linux' && process.arch === 'x64')
                assert.ok(fs.existsSync(path.join(targetDir, 'node_modules', '@esbuild', 'linux-x64', 'package.json')),
                    'a matching platform optional installs into its scope: ' + tree + ' | ' + diag(targetDir, res));
            else
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

            // and the directory it points at as its manifest projected: npm leaves
            // `name` out where the path already says it (`packages/linkpkg` is
            // `linkpkg`), and carries the flags over from the link
            assert.deepEqual(lock.packages['linkpkg'], { version: '1.0.0' });
            assert.equal(lock.packages[''].dependencies.linkpkg, 'file:linkpkg');
        });

        it('describes the dev dependencies, and installs them like npm does', function () {
            var targetDir = makeProject();
            var res = runInstaller(targetDir, ['--install']);

            assert.equal(res.status, 0, diag(targetDir, res));

            var lock = lockOf(targetDir);

            // npm marks a link's *target*, not the link: `metaFromNode` returns
            // `{ resolved, link }` for one and `calc-dep-flags` copies the flags onto
            // the directory it points at
            assert.equal(lock.packages['node_modules/devpkg'].dev, undefined,
                'npm does not mark the link itself: ' + JSON.stringify(lock.packages['node_modules/devpkg']));
            assert.equal(lock.packages['devpkg'].dev, true,
                'a lockfile describes the whole graph: ' + JSON.stringify(lock.packages['devpkg']));
            assert.ok(checkInstalled(targetDir, 'devpkg'),
                'dev dependencies are installed by default, as npm does' + diag(targetDir, res));
        });

        it('--omit=dev keeps the dev dependencies out of the disk, not out of the lockfile', function () {
            var targetDir = makeProject();
            var res = runInstaller(targetDir, ['--install', '--omit=dev']);

            assert.equal(res.status, 0, diag(targetDir, res));
            assert.equal(lockOf(targetDir).packages['devpkg'].dev, true,
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

        // the defects reported on 2026-11-08, all four of which made a lockfile this
        // installer wrote unusable by itself or by npm
        it('describes a workspace member with the manifest npm projects into it', function () {
            var targetDir = makeTargetDir();

            writeFile(targetDir, 'package.json', JSON.stringify({
                name: 'ws-write', version: '1.0.0', private: true, workspaces: ['packages/*'],
            }, null, 2));
            writeFile(targetDir, 'packages/alpha/package.json', JSON.stringify({
                name: '@repro/alpha', version: '0.1.0', private: true,
                dependencies: { linkpkg: 'file:../../linkpkg' },
            }, null, 2));
            writeFile(targetDir, 'linkpkg/package.json', JSON.stringify({ name: 'linkpkg', version: '1.0.0' }, null, 2));

            var res = runInstaller(targetDir, ['--install']);
            assert.equal(res.status, 0, diag(targetDir, res));

            var member = lockOf(targetDir).packages['packages/alpha'];

            // npm writes the member's manifest into the entry: its dependencies are
            // what a reader of the lockfile — npm itself, `--ci`, a graph walk — reads
            assert.deepEqual(member.dependencies, { linkpkg: 'file:../../linkpkg' },
                'the member describes what it depends on: ' + JSON.stringify(member));
            assert.equal(member.name, '@repro/alpha', 'and the name its path does not imply');
            assert.equal(member.version, '0.1.0');

            // A member whose manifest moved is written by a second, *resolving* install,
            // and that is the run that used to describe the member with the installer's
            // own bookkeeping: `to_path_map` adds `path`, `target_only`, `raw_entry`
            // and the boolean flags, `target_entry` kept every one of them, and
            // `raw_entry` was a snapshot of the file *before* this run — so the old
            // dependency range stayed inside the entry next to the new one
            writeFile(targetDir, 'packages/alpha/package.json', JSON.stringify({
                name: '@repro/alpha', version: '0.2.0', private: true,
                dependencies: { linkpkg2: 'file:../../linkpkg2' },
            }, null, 2));
            writeFile(targetDir, 'linkpkg2/package.json', JSON.stringify({ name: 'linkpkg2', version: '1.0.0' }, null, 2));

            var moved = runInstaller(targetDir, ['--install']);
            assert.equal(moved.status, 0, diag(targetDir, moved));

            member = lockOf(targetDir).packages['packages/alpha'];

            // npm's entry for a member is *the manifest* and nothing else
            var keys = Object.keys(member);

            assert.deepEqual(keys.filter(k => ['path', 'target_only', 'raw_entry',
                'link', 'in_bundle', 'legacy', 'peer', 'dev_optional',
                'has_install_script'].indexOf(k) > -1), [],
                'nothing but npm fields: ' + keys.join(', '));
            assert.deepEqual(member.dependencies, { linkpkg2: 'file:../../linkpkg2' },
                'and the manifest is the one the directory has now: ' + JSON.stringify(member));
            assert.equal(member.version, '0.2.0', 'with its version: ' + JSON.stringify(member));
        });

        it('names a workspace member the way npm does when its manifest has no name', function () {
            var targetDir = makeTargetDir();

            writeFile(targetDir, 'package.json', JSON.stringify({
                name: 'ws-nameless', version: '1.0.0', private: true, workspaces: ['packages/*'],
            }, null, 2));
            // npm derives the name from the directory rather than skipping the member:
            // skipping one left the resolver blind to it while the tree on disk held it
            writeFile(targetDir, 'packages/beta/package.json', JSON.stringify({
                version: '0.1.0', private: true,
                dependencies: { linkpkg: 'file:../../linkpkg' },
            }, null, 2));
            writeFile(targetDir, 'linkpkg/package.json', JSON.stringify({ name: 'linkpkg', version: '1.0.0' }, null, 2));

            var res = runInstaller(targetDir, ['--install']);
            assert.equal(res.status, 0, diag(targetDir, res));

            var packages = lockOf(targetDir).packages;

            assert.ok(fs.lstatSync(path.join(targetDir, 'node_modules/beta')).isSymbolicLink(),
                'the member is linked under its directory name: ' +
                listDir(path.join(targetDir, 'node_modules')));
            assert.deepEqual(packages['node_modules/beta'], { resolved: 'packages/beta', link: true },
                'and the lockfile says so: ' + JSON.stringify(packages['node_modules/beta']));
            assert.deepEqual(packages['packages/beta'].dependencies, { linkpkg: 'file:../../linkpkg' },
                'with the manifest it declares: ' + JSON.stringify(packages['packages/beta']));
        });

        it('records where a package on disk came from, so `--ci` can install it', function () {
            // the report's second defect: a lockfile derived from an existing
            // node_modules carried versions and nothing else, and the `--ci` that
            // followed refused the file this installer had just written
            var targetDir = makeTargetDir();

            writeFile(targetDir, 'package.json', JSON.stringify({
                name: 'derive-proj', version: '1.0.0', private: true,
                dependencies: { linkpkg: 'file:linkpkg' },
            }, null, 2));
            writeFile(targetDir, 'linkpkg/package.json', JSON.stringify({ name: 'linkpkg', version: '1.0.0' }, null, 2));

            // a first install writes both the lockfile and the note npm keeps
            var first = runInstaller(targetDir, ['--install']);
            assert.equal(first.status, 0, diag(targetDir, first));

            assert.ok(fs.existsSync(path.join(targetDir, 'node_modules', '.package-lock.json')),
                'the note npm keeps is written as well: ' + listDir(path.join(targetDir, 'node_modules')));

            // now the lockfile goes away and has to be rebuilt from the tree on disk
            fs.unlinkSync(path.join(targetDir, 'package-lock.json'));

            var second = runInstaller(targetDir, ['--install']);
            assert.equal(second.status, 0, diag(targetDir, second));

            var packages = lockOf(targetDir).packages;

            assert.deepEqual(packages['node_modules/linkpkg'], { resolved: 'linkpkg', link: true },
                'the link survived the rebuild: ' + JSON.stringify(packages['node_modules/linkpkg']));

            var ci = runInstaller(targetDir, ['--install', '--ci', '--dry-run']);
            assert.equal(ci.status, 0, 'and the file it wrote is one its own --ci accepts:\n' + outputOf(ci));
        });

        it('keeps the source of a registry package the hidden lockfile recorded', function () {
            // the same defect in its registry form: the version is in the directory,
            // but `resolved` and `integrity` only ever lived in the lockfile — which is
            // why npm keeps them in node_modules/.package-lock.json as well
            var targetDir = makeTargetDir();
            var url = 'https://registry.npmjs.org/linkpkg/-/linkpkg-1.0.0.tgz';

            writeFile(targetDir, 'package.json', JSON.stringify({
                name: 'hidden-proj', version: '1.0.0', private: true,
                dependencies: { linkpkg: '1.0.0' },
            }, null, 2));

            writeFile(targetDir, 'node_modules/linkpkg/package.json', JSON.stringify({
                name: 'linkpkg', version: '1.0.0', license: 'MIT',
            }, null, 2));

            // npm's note about the tree it installed
            writeFile(targetDir, 'node_modules/.package-lock.json', JSON.stringify({
                name: 'hidden-proj', version: '1.0.0', lockfileVersion: 3, requires: true,
                packages: {
                    'node_modules/linkpkg': { version: '1.0.0', resolved: url, integrity: 'sha512-' + 'B'.repeat(86) + '==' },
                },
            }, null, 2));

            var res = runInstaller(targetDir, ['--install']);
            assert.equal(res.status, 0, diag(targetDir, res));

            var entry = lockOf(targetDir).packages['node_modules/linkpkg'];

            assert.equal(entry.resolved, url, 'the source came from the note: ' + JSON.stringify(entry));
            assert.equal(entry.integrity, 'sha512-' + 'B'.repeat(86) + '==',
                'and so did the integrity: ' + JSON.stringify(entry));
        });

        it('ignores a note that describes another tree', function () {
            var targetDir = makeTargetDir();

            writeFile(targetDir, 'package.json', JSON.stringify({
                name: 'stale-proj', version: '1.0.0', private: true,
                dependencies: { stale: '2.0.0' },
            }, null, 2));

            // the directory says 2.0.0, the note says 1.0.0: the note is about a tree
            // that is not there any more, and believing it would describe the wrong thing
            writeFile(targetDir, 'node_modules/stale/package.json', JSON.stringify({
                name: 'stale', version: '2.0.0',
            }, null, 2));
            writeFile(targetDir, 'node_modules/.package-lock.json', JSON.stringify({
                name: 'stale-proj', version: '1.0.0', lockfileVersion: 3, requires: true,
                packages: {
                    'node_modules/stale': {
                        version: '1.0.0',
                        resolved: 'https://registry.example.invalid/stale/-/stale-1.0.0.tgz',
                        integrity: 'sha512-' + 'C'.repeat(86) + '==',
                    },
                },
            }, null, 2));

            // `--lockfile-only`: the verdict is what this is about, and a registry
            // lookup for `stale` is not
            var res = runInstaller(targetDir, ['--install', '--lockfile-only']);
            assert.equal(res.status, 0, diag(targetDir, res));

            var entry = JSON.parse(fs.readFileSync(path.join(targetDir, 'package-lock.json'), 'utf8'))
                .packages['node_modules/stale'];

            assert.equal(entry.version, '2.0.0', 'the directory decides the version: ' + JSON.stringify(entry));
            assert.ok(!entry.resolved || entry.resolved.indexOf('stale-2.0.0.tgz') > -1,
                'and a stale source is not borrowed for it: ' + JSON.stringify(entry));
        });

        it('refuses an entry it cannot verify before touching node_modules', function () {
            // the report's third defect: `--ci` emptied node_modules and then failed on
            // a download, which left a project with neither its old tree nor a new one
            var targetDir = makeTargetDir();

            writeFile(targetDir, 'package.json', JSON.stringify({
                name: 'unsigned-proj', version: '1.0.0', private: true,
                dependencies: { linkpkg: '1.0.0' },
            }, null, 2));
            writeFile(targetDir, 'package-lock.json', JSON.stringify({
                name: 'unsigned-proj', version: '1.0.0', lockfileVersion: 3, requires: true,
                packages: {
                    '': { name: 'unsigned-proj', version: '1.0.0', dependencies: { linkpkg: '1.0.0' } },
                    // a version and a source, and nothing to check the bytes against
                    'node_modules/linkpkg': {
                        version: '1.0.0',
                        resolved: 'https://registry.npmjs.org/linkpkg/-/linkpkg-1.0.0.tgz',
                    },
                },
            }, null, 2));

            // a tree that must survive the refusal
            writeFile(targetDir, 'node_modules/keepme/package.json',
                JSON.stringify({ name: 'keepme', version: '1.0.0' }, null, 2));

            var res = runInstaller(targetDir, ['--install', '--ci']);
            var out = outputOf(res);

            assert.notEqual(res.status, 0, 'the entry is refused' + diag(targetDir, res));
            assert.ok(out.indexOf('the lockfile carries no integrity') > -1, out);
            assert.ok(out.indexOf('--no-strict-integrity') > -1, 'and the way out is offered:\n' + out);
            assert.ok(fs.existsSync(path.join(targetDir, 'node_modules', 'keepme')),
                'node_modules was left as it was: ' + listDir(path.join(targetDir, 'node_modules')));
        });

        it('stops on a tarball the registry does not serve, with a code and a name', function () {
            // what actually left the report's tree half installed: a non-200 answer
            // ended the process from inside a download fiber. `process.exit()` with no
            // code is 0, so the run that installed two packages out of five reported
            // success; and the retry loop never got a second chance
            var targetDir = makeTargetDir();

            writeFile(targetDir, 'package.json', JSON.stringify({
                name: 'missing-proj', version: '1.0.0', private: true,
                dependencies: { ghost: '9.9.9' },
            }, null, 2));
            writeFile(targetDir, 'package-lock.json', JSON.stringify({
                name: 'missing-proj', version: '1.0.0', lockfileVersion: 3, requires: true,
                packages: {
                    '': { name: 'missing-proj', version: '1.0.0', dependencies: { ghost: '9.9.9' } },
                    // `ghost` is published, `9.9.9` never was: the registry answers 404
                    'node_modules/ghost': {
                        version: '9.9.9',
                        resolved: 'https://registry.npmjs.org/ghost/-/ghost-9.9.9.tgz',
                        integrity: 'sha512-' + 'D'.repeat(86) + '==',
                    },
                },
            }, null, 2));

            var res = runInstaller(targetDir, ['--install', '--ci']);
            var out = outputOf(res);

            assert.notEqual(res.status, 0, 'a package that could not be fetched fails the run' + diag(targetDir, res));
            assert.ok(out.indexOf('could not be installed') > -1, 'and the run says what is missing:\n' + out);
            assert.ok(out.indexOf('ghost') > -1, out);
            assert.ok(out.indexOf('HTTP 404') > -1, 'with the reason:\n' + out);
        });

        it('completes the source of an entry the tree only knows the version of', function () {
            // the report's second defect, end to end: a lockfile whose registry
            // entries carry a version and nothing else. `--install` used to write it
            // back unchanged and the `--ci` that followed refused it — a file the tool
            // insisted on and then could not use
            var targetDir = makeTargetDir();

            writeFile(targetDir, 'package.json', JSON.stringify({
                name: 'source-proj', version: '1.0.0', private: true,
                dependencies: { 'is-number': '6.0.0' },
            }, null, 2));
            writeFile(targetDir, 'package-lock.json', JSON.stringify({
                name: 'source-proj', version: '1.0.0', lockfileVersion: 3, requires: true,
                packages: {
                    '': { name: 'source-proj', version: '1.0.0', dependencies: { 'is-number': '6.0.0' } },
                    'node_modules/is-number': { version: '6.0.0' },
                },
            }, null, 2));

            var res = runInstaller(targetDir, ['--install']);
            assert.equal(res.status, 0, diag(targetDir, res));
            assert.ok(outputOf(res).indexOf('gained the source') > -1,
                'the source was completed, and the run says so:\n' + outputOf(res));

            var entry = lockOf(targetDir).packages['node_modules/is-number'];

            assert.ok(entry.resolved && entry.resolved.indexOf('is-number-6.0.0.tgz') > -1,
                'the entry names the tarball it was installed from: ' + JSON.stringify(entry));
            assert.ok(entry.integrity, 'and carries a digest to check it against: ' + JSON.stringify(entry));

            var ci = runInstaller(targetDir, ['--install', '--ci', '--dry-run']);
            assert.equal(ci.status, 0, 'the file it healed is one its own --ci accepts:\n' + outputOf(ci));
        });

        it('drops an entry an older install left beside the link that provides it', function () {
            // the debris the report's fourth defect left behind: a member linked at
            // `node_modules/@scope/member` *and* described as a registry package under
            // its directory name. Installing that entry asked a registry for a tarball
            // nobody published, so the tree could not be installed at all
            var targetDir = makeTargetDir();

            writeFile(targetDir, 'package.json', JSON.stringify({
                name: 'debris-proj', version: '1.0.0', private: true,
                workspaces: ['packages/*'],
            }, null, 2));
            writeFile(targetDir, 'packages/member/package.json', JSON.stringify({
                name: '@repro/member', version: '0.1.0', private: true,
            }, null, 2));
            writeFile(targetDir, 'package-lock.json', JSON.stringify({
                name: 'debris-proj', version: '1.0.0', lockfileVersion: 3, requires: true,
                packages: {
                    '': { name: 'debris-proj', version: '1.0.0', workspaces: ['packages/*'] },
                    'node_modules/@repro/member': { resolved: 'packages/member', link: true },
                    'packages/member': { name: '@repro/member', version: '0.1.0' },
                    // what an older installer added: the member as a registry package,
                    // keyed by the directory it sits in
                    'node_modules/member': { name: '@repro/member', version: '0.1.0' },
                },
            }, null, 2));

            var res = runInstaller(targetDir, ['--install']);
            var out = outputOf(res);

            assert.equal(res.status, 0, diag(targetDir, res));
            assert.equal(out.indexOf('registry.npmjs.org/member') > -1, false,
                'the member was not fetched from a registry:\n' + out);

            var packages = lockOf(targetDir).packages;

            assert.equal(packages['node_modules/member'], undefined,
                'the entry beside the link is gone: ' + Object.keys(packages).join(', '));
            assert.deepEqual(packages['node_modules/@repro/member'], { resolved: 'packages/member', link: true },
                'and the link that provides the package is kept: ' +
                JSON.stringify(packages['node_modules/@repro/member']));
            assert.ok(packages['packages/member'], 'with the directory it points at');
        });

        it('links a workspace member with a relative target, the way npm does', function () {
            // an absolute target survives neither a renamed project directory nor a
            // copy of it: moved it is simply gone, and copied it still resolves — to
            // the *original* directory, so every edit in the copy is invisible and
            // nothing says so. npm writes the relative form for exactly that reason
            var targetDir = makeTargetDir();

            writeFile(targetDir, 'package.json', JSON.stringify({
                name: 'rel-proj', version: '1.0.0', private: true,
                workspaces: ['packages/*'],
            }, null, 2));
            writeFile(targetDir, 'packages/core/package.json', JSON.stringify({
                name: '@z/core', version: '0.1.0', private: true,
            }, null, 2));
            writeFile(targetDir, 'packages/core/index.js', 'module.exports = "CORE";\n');

            var res = runInstaller(targetDir, ['--install']);
            assert.equal(res.status, 0, diag(targetDir, res));

            var link = path.join(targetDir, 'node_modules', '@z', 'core');

            if (!LINKS_USED) {
                // a filesystem that cannot host a link installs a copy instead
                assert.ok(fs.existsSync(path.join(link, 'package.json')), listDir(path.join(targetDir, 'node_modules')));
                return;
            }

            var target = fs.readlinkSync(link);

            // A Windows junction stores an absolute target and cannot store anything
            // else: libuv refuses a relative one (`UV_EINVAL`). npm passes the relative
            // form and still ends up with an absolute one there, because Node's
            // `fs.symlink` resolves it before the junction is written. The property
            // that has to hold everywhere is that the link resolves to the member
            if (process.platform !== 'win32') {
                assert.equal(path.isAbsolute(target), false,
                    'the link target is relative (' + target + '), so the tree can move');
                assert.equal(target, path.join('..', '..', 'packages', 'core'),
                    'and names the package the way npm spells it');
            }

            assert.equal(fs.realpathSync(link), fs.realpathSync(path.join(targetDir, 'packages', 'core')),
                'while still resolving to the member');
        });

        it('rewrites a member entry when its package.json changed', function () {
            // a member's entry in the file *is* its current manifest projected. Writing
            // only the entries the file lacked left every member at whatever the first
            // install saw — a version, a name or a `license` that changed afterwards
            // never reached the file, and `--ci` then reported a lockfile in sync with a
            // package.json that said something else
            var targetDir = makeTargetDir();

            writeFile(targetDir, 'package.json', JSON.stringify({
                name: 'member-refresh-proj', version: '1.0.0', private: true,
                workspaces: ['packages/*'],
            }, null, 2));
            writeFile(targetDir, 'packages/member/package.json', JSON.stringify({
                name: '@r/old', version: '0.1.0',
                dependencies: { 'is-odd': '3.0.1' },
            }, null, 2));

            var first = runInstaller(targetDir, ['--install']);
            assert.equal(first.status, 0, diag(targetDir, first));

            var before = lockOf(targetDir).packages['packages/member'];

            assert.equal(before.version, '0.1.0');
            assert.equal(before.name, '@r/old');

            writeFile(targetDir, 'packages/member/package.json', JSON.stringify({
                name: '@r/new', version: '0.2.0', license: 'MIT',
                dependencies: { 'is-odd': '3.0.1' },
            }, null, 2));

            var again = runInstaller(targetDir, ['--install']);
            assert.equal(again.status, 0, diag(targetDir, again));

            var after = lockOf(targetDir).packages['packages/member'];

            assert.equal(after.version, '0.2.0', 'the version follows package.json');
            assert.equal(after.name, '@r/new', 'so does the name');
            assert.equal(after.license, 'MIT', 'and every other manifest field npm projects');

            // nothing of the installer's own bookkeeping may reach the file: an entry
            // that carries `target_only` or `raw_entry` is not something npm can read
            ['target_only', 'raw_entry', 'legacy', 'path', 'link', 'dev_optional',
                'in_bundle', 'peer', 'has_install_script'].forEach(k => {
                    assert.equal(after[k], undefined, 'the entry is a lockfile entry, not a node: ' + k);
                });

            assert.equal(lockOf(targetDir).packages['node_modules/@r/old'], undefined,
                'and the link the old name made is gone: ' +
                Object.keys(lockOf(targetDir).packages).join(', '));
        });

        it('repoints a link an older install wrote with an absolute target', function () {
            var targetDir = makeTargetDir();

            writeFile(targetDir, 'package.json', JSON.stringify({
                name: 'repoint-proj', version: '1.0.0', private: true,
                workspaces: ['packages/*'],
            }, null, 2));
            writeFile(targetDir, 'packages/core/package.json', JSON.stringify({
                name: '@z/core', version: '0.1.0', private: true,
            }, null, 2));
            writeFile(targetDir, 'packages/core/index.js', 'module.exports = "CORE";\n');

            if (!LINKS_USED)
                return;     // a copy is not a link, so there is nothing to repoint

            // the shape an earlier build left: an absolute target. A Windows run can
            // only create that as a junction — a directory symlink needs a privilege a
            // plain user does not have — and a junction already stores an absolute
            // target, which is the same defect
            var present = path.join(targetDir, 'node_modules', '@z', 'core');

            fs.mkdirSync(path.join(targetDir, 'node_modules', '@z'), { recursive: true });

            if (process.platform === 'win32')
                fs.symlinkSync(path.join(targetDir, 'packages', 'core'), present, 'junction');
            else
                fs.symlinkSync(path.join(targetDir, 'packages', 'core'), present, 'dir');

            var res = runInstaller(targetDir, ['--install']);
            assert.equal(res.status, 0, diag(targetDir, res));

            var target = fs.readlinkSync(present);

            // the same platform difference as above: a junction always stores an
            // absolute target, so a Windows run cannot see the relative form — it
            // sees a link that points at the member, which is the property that matters
            if (process.platform === 'win32') {
                assert.equal(fs.realpathSync(present),
                    fs.realpathSync(path.join(targetDir, 'packages', 'core')),
                    'the link still points at the member: ' + target);
            } else {
                assert.equal(path.isAbsolute(target), false,
                    'a plain --install repoints it, no rm -rf needed: ' + target);
            }
        });

        it('leaves a healthy relative link alone', function () {
            var targetDir = makeTargetDir();

            writeFile(targetDir, 'package.json', JSON.stringify({
                name: 'stable-link-proj', version: '1.0.0', private: true,
                workspaces: ['packages/*'],
            }, null, 2));
            writeFile(targetDir, 'packages/core/package.json', JSON.stringify({
                name: '@z/core', version: '0.1.0', private: true,
            }, null, 2));

            runInstaller(targetDir, ['--install']);

            if (!LINKS_USED)
                return;

            var link = path.join(targetDir, 'node_modules', '@z', 'core');
            var before = fs.lstatSync(link).ino;

            var res = runInstaller(targetDir, ['--install']);
            assert.equal(res.status, 0, diag(targetDir, res));

            assert.equal(fs.lstatSync(link).ino, before,
                'a link that already resolves where it should is not churned');
            assert.equal(outputOf(res).indexOf('link: ') > -1, false,
                'and the run says nothing about relinking:\n' + outputOf(res));
        });

        it('refuses two workspace members of the same name, naming both', function () {
            // a link can only point at one directory, so one of the two packages is
            // installed and the other is dropped — silently, and only on some machines,
            // because it is the directory order that decides which one. npm refuses the
            // project (`EDUPLICATEWORKSPACE`); this answers the same way
            var targetDir = makeTargetDir();

            writeFile(targetDir, 'package.json', JSON.stringify({
                name: 'dup-proj', version: '1.0.0', private: true,
                workspaces: ['packages/*'],
            }, null, 2));
            writeFile(targetDir, 'packages/p1/package.json', JSON.stringify({
                name: '@f/dup', version: '0.1.0', private: true,
            }, null, 2));
            writeFile(targetDir, 'packages/p2/package.json', JSON.stringify({
                name: '@f/dup', version: '0.2.0', private: true,
            }, null, 2));

            var res = runInstaller(targetDir, ['--install']);
            var out = outputOf(res);

            assert.notEqual(res.status, 0, 'a project two members share a name in is refused' + diag(targetDir, res));
            assert.ok(out.indexOf('must not have multiple workspaces with the same name') > -1, out);
            assert.ok(out.indexOf("@f/dup") > -1, 'the name is named:\n' + out);
            assert.ok(out.indexOf('p1') > -1 && out.indexOf('p2') > -1,
                'and so is every directory that claims it:\n' + out);

            assert.equal(fs.existsSync(path.join(targetDir, 'node_modules')), false,
                'nothing was installed' + diag(targetDir, res));
            assert.equal(fs.existsSync(path.join(targetDir, 'package-lock.json')), false,
                'and no lockfile was written for a project that cannot be installed');
        });

        it('refuses the duplicate under --ci as well', function () {
            // the case the report worried about most: a lockfile that describes one of
            // the two members made `--ci` pass, so the clash never surfaced in CI
            var targetDir = makeTargetDir();

            writeFile(targetDir, 'package.json', JSON.stringify({
                name: 'dup-ci-proj', version: '1.0.0', private: true,
                workspaces: ['packages/*'],
            }, null, 2));
            writeFile(targetDir, 'packages/p1/package.json', JSON.stringify({
                name: '@f/dup', version: '0.1.0', private: true,
            }, null, 2));
            writeFile(targetDir, 'packages/p2/package.json', JSON.stringify({
                name: '@f/dup', version: '0.2.0', private: true,
            }, null, 2));
            // a lockfile that happens to describe just one of them
            writeFile(targetDir, 'package-lock.json', JSON.stringify({
                name: 'dup-ci-proj', version: '1.0.0', lockfileVersion: 3, requires: true,
                packages: {
                    '': { name: 'dup-ci-proj', version: '1.0.0', workspaces: ['packages/*'] },
                    'node_modules/@f/dup': { resolved: 'packages/p2', link: true },
                    'packages/p2': { name: '@f/dup', version: '0.2.0' },
                },
            }, null, 2));

            var res = runInstaller(targetDir, ['--install', '--ci']);
            var out = outputOf(res);

            assert.notEqual(res.status, 0, 'the frozen install refuses it too' + diag(targetDir, res));
            assert.ok(out.indexOf('multiple workspaces with the same name') > -1, out);
        });

        it('accepts two patterns that name the same member once', function () {
            // npm keys a member by its path: `packages/*` and `packages/core` naming the
            // same directory is one member, not two of the same name
            var targetDir = makeTargetDir();

            writeFile(targetDir, 'package.json', JSON.stringify({
                name: 'overlap-proj', version: '1.0.0', private: true,
                workspaces: ['packages/*', 'packages/core'],
            }, null, 2));
            writeFile(targetDir, 'packages/core/package.json', JSON.stringify({
                name: '@g/core', version: '0.1.0', private: true,
            }, null, 2));

            var res = runInstaller(targetDir, ['--install']);
            assert.equal(res.status, 0, diag(targetDir, res));
            assert.ok(checkInstalled(targetDir, '@g/core'), diag(targetDir, res));
        });

        it('honours an excluded workspace pattern, the way npm does', function () {
            // `!pattern` is npm's way of keeping a member out of the tree — a package
            // used only for publishing, say. It used to be globbed like any other
            // pattern, so the excluded member was linked and written into the lockfile
            var targetDir = makeTargetDir();

            writeFile(targetDir, 'package.json', JSON.stringify({
                name: 'excl-proj', version: '1.0.0', private: true,
                workspaces: ['packages/*', '!packages/skip'],
            }, null, 2));
            writeFile(targetDir, 'packages/keep/package.json', JSON.stringify({
                name: '@a/keep', version: '0.1.0', private: true,
            }, null, 2));
            writeFile(targetDir, 'packages/skip/package.json', JSON.stringify({
                name: '@a/skip', version: '0.1.0', private: true,
            }, null, 2));

            var res = runInstaller(targetDir, ['--install']);
            assert.equal(res.status, 0, diag(targetDir, res));

            var packages = lockOf(targetDir).packages;

            assert.ok(packages['packages/keep'], 'the member that is not excluded is described');
            assert.equal(packages['packages/skip'], undefined,
                'and the excluded one is not: ' + Object.keys(packages).filter(k => k.indexOf('packages/') === 0).join(', '));
            assert.equal(checkInstalled(targetDir, '@a/skip'), false,
                'nor installed' + listDir(path.join(targetDir, 'node_modules')));
        });

        it('lets a later pattern cancel an exclusion, which npm also does', function () {
            // npm reads the list in order and a selecting pattern that matches an
            // exclusion cancels it: `['packages/**', '!packages/b/**', 'packages/b/a']`
            // excludes nothing, because the third pattern overrides the second
            var targetDir = makeTargetDir();

            writeFile(targetDir, 'package.json', JSON.stringify({
                name: 'cancel-proj', version: '1.0.0', private: true,
                workspaces: ['packages/**', '!packages/b/**', 'packages/b/a'],
            }, null, 2));
            writeFile(targetDir, 'packages/b/a/package.json', JSON.stringify({
                name: '@c/ba', version: '0.1.0', private: true,
            }, null, 2));
            writeFile(targetDir, 'packages/b/c/package.json', JSON.stringify({
                name: '@c/bc', version: '0.1.0', private: true,
            }, null, 2));
            writeFile(targetDir, 'packages/d/package.json', JSON.stringify({
                name: '@c/d', version: '0.1.0', private: true,
            }, null, 2));

            var res = runInstaller(targetDir, ['--install']);
            assert.equal(res.status, 0, diag(targetDir, res));

            var members = Object.keys(lockOf(targetDir).packages)
                .filter(k => k.indexOf('packages/') === 0).sort();

            assert.deepEqual(members, ['packages/b/a', 'packages/b/c', 'packages/d'],
                'the exclusion was cancelled by the pattern after it: ' + members.join(', '));
        });

        it('refuses a project whose workspace member cannot be read', function () {
            // the member is silently skipped otherwise: `--install` exited 0, so
            // `fibjs --install && npm test` carried on against a tree that was not what
            // the project describes. npm stops with EJSONPARSE
            var targetDir = makeTargetDir();

            writeFile(targetDir, 'package.json', JSON.stringify({
                name: 'broken-proj', version: '1.0.0', private: true,
                workspaces: ['packages/*'],
            }, null, 2));
            writeFile(targetDir, 'packages/good/package.json', JSON.stringify({
                name: '@b/good', version: '0.1.0', private: true,
            }, null, 2));
            writeFile(targetDir, 'packages/bad/package.json', '{ "name": "@b/bad", "version": BROKEN }');

            var res = runInstaller(targetDir, ['--install']);
            var out = outputOf(res);

            assert.notEqual(res.status, 0, 'a member that cannot be read stops the install' + diag(targetDir, res));
            assert.ok(out.indexOf('cannot be used') > -1, out);
            assert.ok(out.indexOf(path.join('packages', 'bad')) > -1, 'and the file is named:\n' + out);

            // the same file was reported once per pass before, and one broken member
            // printed its warning three times in a single run. The path is counted in
            // the platform's own separators: the installer names the file it could not
            // read, and on Windows that is `packages\bad\package.json`
            var named = path.join('packages', 'bad', 'package.json');

            assert.equal(out.split(named).length - 1, 1,
                'reported once, not once per pass:\n' + out);

            assert.equal(fs.existsSync(path.join(targetDir, 'node_modules')), false,
                'nothing was installed' + diag(targetDir, res));
        });

        it('refuses a project whose own package.json does not parse', function () {
            // the whole file was read as "no package.json", which is the *other*
            // state: a project with nothing declared. A comment in the manifest then
            // installed nothing and exited 0 with a lockfile saying `0 packages` —
            // a legitimate-looking empty install, and one `--ci` accepted. npm answers
            // EJSONPARSE and exits nonzero
            var targetDir = makeTargetDir();

            writeFile(targetDir, 'package.json',
                '{\n  // project name\n  "name": "broken-root",\n' +
                '  "dependencies": { "is-number": "^7.0.0" }\n}\n');

            var res = runInstaller(targetDir, ['--install']);
            var out = outputOf(res);

            assert.notEqual(res.status, 0, 'a root manifest that cannot be read stops the install' + diag(targetDir, res));
            assert.ok(out.indexOf('is not valid JSON') > -1, out);
            assert.ok(out.indexOf('position') > -1, 'the message names where:\n' + out);

            assert.equal(fs.existsSync(path.join(targetDir, 'package-lock.json')), false,
                'no lockfile was written: ' + listDir(targetDir));
            assert.equal(fs.existsSync(path.join(targetDir, 'node_modules')), false,
                'and nothing was installed: ' + listDir(targetDir));

            // the same run with `--ci`, which used to certify the emptiness
            var ci = runInstaller(targetDir, ['--install', '--ci']);

            assert.notEqual(ci.status, 0, 'and --ci refuses it too:\n' + outputOf(ci));
        });

        it('refuses a package.json that is valid JSON but not an object', function () {
            // `null`, a string, a number and an array all parse, and none of them is a
            // manifest. Reading one as a project produced an internal TypeError from
            // deep inside the installer — the file that does not parse already got a
            // proper message, and this is the same class of input
            ['null', '"hello"', '123', 'true', '[1, 2]'].forEach(function (body) {
                var targetDir = makeTargetDir();

                writeFile(targetDir, 'package.json', body);

                var res = runInstaller(targetDir, ['--install']);
                var out = outputOf(res);

                assert.notEqual(res.status, 0, body + ' is not a manifest' + diag(targetDir, res));
                assert.ok(out.indexOf('is not a JSON object') > -1, body + ':\n' + out);
                assert.equal(out.indexOf('TypeError'), -1,
                    'and the message is the installer\'s, not a stack: ' + body + '\n' + out);
                assert.equal(out.indexOf('--install.cjs'), -1,
                    'no source location leaks: ' + body + '\n' + out);
            });
        });

        it('refuses a workspaces declaration that is not an array', function () {
            // npm answers `EWORKSPACESCONFIG`. Reading it as "no workspaces" installs
            // none of the members and says nothing — and writing a single member as a
            // string is the mistake people actually make
            var targetDir = makeTargetDir();

            writeFile(targetDir, 'package.json', JSON.stringify({
                name: 'ws-string-proj', version: '1.0.0', private: true,
                workspaces: 'packages/*',
            }, null, 2));
            writeFile(targetDir, 'packages/lib/package.json', JSON.stringify({
                name: '@c/lib', version: '1.0.0', private: true,
            }, null, 2));

            var res = runInstaller(targetDir, ['--install']);
            var out = outputOf(res);

            assert.notEqual(res.status, 0, 'a string is not a workspaces list' + diag(targetDir, res));
            assert.ok(out.indexOf('EWORKSPACESCONFIG') > -1, out);
            assert.equal(fs.existsSync(path.join(targetDir, 'package-lock.json')), false,
                'and nothing was written: ' + listDir(targetDir));
        });

        it('refuses a workspaces element that is not a pattern string', function () {
            // the container being an array is not the whole check: `String(123)` produced
            // the pattern `"123"`, which matches no directory — so `[123]` installed no
            // members and said nothing, the same failure as the string form one level in
            var targetDir = makeTargetDir();

            writeFile(targetDir, 'package.json', JSON.stringify({
                name: 'ws-number-proj', version: '1.0.0', private: true,
                workspaces: [123],
            }, null, 2));
            writeFile(targetDir, 'packages/lib/package.json', JSON.stringify({
                name: '@c/lib', version: '1.0.0', private: true,
            }, null, 2));

            var res = runInstaller(targetDir, ['--install']);
            var out = outputOf(res);

            assert.notEqual(res.status, 0, 'a number is not a pattern' + diag(targetDir, res));
            assert.ok(out.indexOf('EWORKSPACESCONFIG') > -1, out);
            assert.ok(out.indexOf('element 0') > -1, 'and the element is named:\n' + out);
            assert.equal(fs.existsSync(path.join(targetDir, 'package-lock.json')), false,
                'and nothing was written: ' + listDir(targetDir));
        });

        it('refuses a member whose package.json is not an object', function () {
            // `42` parses, and reading it as "a member with no name" invented the name
            // from the directory and linked that: `require('bad')` then failed with
            // MODULE_NOT_FOUND and the bad entry went into the lockfile. The root
            // manifest already refuses this; a member is the same question
            ['42', 'null', '"x"', '[1]'].forEach(function (body) {
                var targetDir = makeTargetDir();

                writeFile(targetDir, 'package.json', JSON.stringify({
                    name: 'bad-member-proj', version: '1.0.0', private: true,
                    workspaces: ['packages/*'],
                }, null, 2));
                writeFile(targetDir, 'packages/bad/package.json', body);

                var res = runInstaller(targetDir, ['--install']);
                var out = outputOf(res);

                assert.notEqual(res.status, 0, body + ' is not a manifest' + diag(targetDir, res));
                assert.ok(out.indexOf('cannot be used') > -1, body + ':\n' + out);
                assert.ok(out.indexOf('not a package manifest') > -1, body + ':\n' + out);
                assert.equal(fs.existsSync(path.join(targetDir, 'node_modules')), false,
                    'no link was made for it: ' + listDir(targetDir));
                assert.equal(fs.existsSync(path.join(targetDir, 'package-lock.json')), false,
                    'and no lockfile either: ' + listDir(targetDir));
            });
        });

        it('refuses dependency fields that are not maps', function () {
            // each of the four is read as name -> range. A list and a number are valid
            // JSON and describe nothing: the list was silently read as "no
            // dependencies", so the install put in nothing it was asked to; the number
            // reached `util.extend({}, 42)` and threw a TypeError with a source line
            ['[]', '42', '"is-number"', 'true'].forEach(function (body) {
                var targetDir = makeTargetDir();

                writeFile(targetDir, 'package.json',
                    '{"name": "depshape-proj", "version": "1.0.0", "dependencies": ' + body + '}');

                var res = runInstaller(targetDir, ['--install']);
                var out = outputOf(res);

                assert.notEqual(res.status, 0, 'dependencies=' + body + diag(targetDir, res));
                assert.ok(out.indexOf('malformed dependency field') > -1, body + ':\n' + out);
                assert.equal(out.indexOf('TypeError'), -1,
                    'the message is the installer\'s, not a stack: ' + body + '\n' + out);
                assert.equal(out.indexOf('--install.cjs'), -1,
                    'no source location leaks: ' + body + '\n' + out);
                assert.equal(fs.existsSync(path.join(targetDir, 'package-lock.json')), false,
                    'and nothing was written: ' + listDir(targetDir));
            });

            // `null` says nothing, and npm reads it that way too — a generated manifest
            // often writes it
            var ok = makeTargetDir();

            writeFile(ok, 'package.json',
                '{"name": "depnull-proj", "version": "1.0.0", "dependencies": null}');

            assert.equal(runInstaller(ok, ['--install']).status, 0, 'null is "none declared"');
        });

        it('refuses a dependency range that is not a string', function () {
            // the same mistake one level in: `{"is-number": 42}` reached
            // `semver.satisfies` with a number. npm answers `must provide string spec`
            var targetDir = makeTargetDir();

            writeFile(targetDir, 'package.json',
                '{"name": "deprange-proj", "version": "1.0.0", "dependencies": {"is-number": 42}}');

            var res = runInstaller(targetDir, ['--install']);
            var out = outputOf(res);

            assert.notEqual(res.status, 0, diag(targetDir, res));
            assert.ok(out.indexOf('dependencies.is-number') > -1, 'the entry is named:\n' + out);
            assert.ok(out.indexOf('not a version range') > -1, out);
        });

        it('refuses a member with a malformed dependency field', function () {
            // a member's fields are read the same way, and the report is the one the
            // workspace scan already produces — the member is named once
            var targetDir = makeTargetDir();

            writeFile(targetDir, 'package.json', JSON.stringify({
                name: 'bad-dep-member-proj', version: '1.0.0', private: true,
                workspaces: ['packages/*'],
            }, null, 2));
            writeFile(targetDir, 'packages/bad/package.json',
                '{"name": "bad", "version": "1.0.0", "dependencies": []}');

            var res = runInstaller(targetDir, ['--install']);
            var out = outputOf(res);

            assert.notEqual(res.status, 0, diag(targetDir, res));
            assert.ok(out.indexOf('cannot be used') > -1, out);
            assert.ok(out.indexOf('not an object of name') > -1, out);
            assert.equal(fs.existsSync(path.join(targetDir, 'node_modules')), false,
                'no link was made for it: ' + listDir(targetDir));
        });

        it('accepts workspaces as an object with a packages array', function () {
            // npm reads both spellings (`@npmcli/map-workspaces`); the object form was
            // being dropped, which is the same silent-no-members failure as the string
            var targetDir = makeTargetDir();

            writeFile(targetDir, 'package.json', JSON.stringify({
                name: 'ws-object-proj', version: '1.0.0', private: true,
                workspaces: { packages: ['packages/*'] },
            }, null, 2));
            writeFile(targetDir, 'packages/lib/package.json', JSON.stringify({
                name: '@c/lib', version: '1.0.0', private: true,
            }, null, 2));

            var res = runInstaller(targetDir, ['--install']);

            assert.equal(res.status, 0, diag(targetDir, res));
            assert.ok(lockOf(targetDir).packages['packages/lib'],
                'the member is described: ' + Object.keys(lockOf(targetDir).packages).join(', '));
        });

        it('refuses a workspace member whose name cannot be a directory', function () {
            // a link at `node_modules/@t10/has space` is a directory nothing can
            // require, and the install said nothing about it
            var targetDir = makeTargetDir();

            writeFile(targetDir, 'package.json', JSON.stringify({
                name: 'badname-proj', version: '1.0.0', private: true,
                workspaces: ['packages/*'],
            }, null, 2));
            writeFile(targetDir, 'packages/x/package.json', JSON.stringify({
                name: '@t10/has space', version: '0.1.0', private: true,
            }, null, 2));

            var res = runInstaller(targetDir, ['--install']);
            var out = outputOf(res);

            assert.notEqual(res.status, 0, 'the name is refused' + diag(targetDir, res));
            assert.ok(out.indexOf('@t10/has space') > -1, 'and named:\n' + out);
            assert.ok(out.indexOf('URL-friendly') > -1, out);
            assert.equal(fs.existsSync(path.join(targetDir, 'node_modules', '@t10')), false,
                'no directory was created for it');
        });

        it('accepts the names npm accepts', function () {
            // npm checks a member's name in two places, and neither of them minds
            // capital letters, a leading hyphen or the characters `~'!()*` — this has
            // to accept exactly the same set, or a project npm installs is refused here
            ['plain-name', 'MixedCase', '-dash', 'ok~name', '@s/ok'].forEach(name => {
                var targetDir = makeTargetDir();

                writeFile(targetDir, 'package.json', JSON.stringify({
                    name: 'okname-proj', version: '1.0.0', private: true,
                    workspaces: ['packages/*'],
                }, null, 2));
                writeFile(targetDir, 'packages/m/package.json', JSON.stringify({
                    name: name, version: '0.1.0', private: true,
                }, null, 2));

                var res = runInstaller(targetDir, ['--install']);

                assert.equal(res.status, 0, `'${name}' is a name npm installs` + diag(targetDir, res));
            });
        });

        it('prunes a dependency that was removed from package.json', function () {
            // the most ordinary operation there is: drop a dependency, install again.
            // The package stayed on disk *and* in the lockfile, so `--ci` went on
            // installing it — a removed dependency kept resolving, and the tree only
            // ever grew. npm prunes; this did not
            var targetDir = makeTargetDir();

            writeFile(targetDir, 'package.json', JSON.stringify({
                name: 'prune-proj', version: '1.0.0', private: true,
                dependencies: { 'is-number': '6.0.0', 'linkpkg': 'file:linkpkg' },
            }, null, 2));
            writeFile(targetDir, 'linkpkg/package.json', JSON.stringify({
                name: 'linkpkg', version: '1.0.0', dependencies: { 'is-odd': '3.0.1' },
            }, null, 2));

            var first = runInstaller(targetDir, ['--install']);
            assert.equal(first.status, 0, diag(targetDir, first));

            assert.ok(fs.existsSync(path.join(targetDir, 'node_modules', 'is-odd')),
                'the transitive dependency is there: ' + listDir(path.join(targetDir, 'node_modules')));

            // drop `linkpkg`, which is what brings `is-odd` in
            writeFile(targetDir, 'package.json', JSON.stringify({
                name: 'prune-proj', version: '1.0.0', private: true,
                dependencies: { 'is-number': '6.0.0' },
            }, null, 2));

            var second = runInstaller(targetDir, ['--install']);
            assert.equal(second.status, 0, diag(targetDir, second));
            assert.ok(outputOf(second).indexOf('prune') > -1,
                'the run says what it removed:\n' + outputOf(second));

            var packages = lockOf(targetDir).packages;
            var on_disk = fs.readdirSync(path.join(targetDir, 'node_modules')).filter(n => n.charAt(0) !== '.').sort();

            assert.deepEqual(on_disk, ['is-number'],
                'the tree holds only what the project declares: ' + on_disk.join(', '));
            assert.equal(packages['node_modules/linkpkg'], undefined, 'the link is out of the lockfile');
            assert.equal(packages['node_modules/is-odd'], undefined,
                'and so is what it brought in: ' + Object.keys(packages).filter(k => k.indexOf('node_modules/') === 0).join(', '));
        });

        it('does not install a pruned package again from a stale lockfile', function () {
            // the second half of the same problem: `rm -rf node_modules && fibjs
            // --install --ci` restored the removed package, because the entry was still
            // in the file. Both entries are local here, so the case needs no registry
            var targetDir = makeTargetDir();

            writeFile(targetDir, 'package.json', JSON.stringify({
                name: 'stale-proj', version: '1.0.0', private: true,
                dependencies: { 'linkpkg': 'file:linkpkg' },
            }, null, 2));
            writeFile(targetDir, 'linkpkg/package.json', JSON.stringify({
                name: 'linkpkg', version: '1.0.0',
            }, null, 2));
            writeFile(targetDir, 'oldpkg/package.json', JSON.stringify({
                name: 'oldpkg', version: '1.0.0',
            }, null, 2));

            writeFile(targetDir, 'package-lock.json', JSON.stringify({
                name: 'stale-proj', version: '1.0.0', lockfileVersion: 3, requires: true,
                packages: {
                    '': { name: 'stale-proj', version: '1.0.0', dependencies: { 'linkpkg': 'file:linkpkg' } },
                    'linkpkg': { version: '1.0.0' },
                    'node_modules/linkpkg': { resolved: 'linkpkg', link: true },
                    // what a dependency that is no longer declared left behind
                    'oldpkg': { version: '1.0.0' },
                    'node_modules/oldpkg': { resolved: 'oldpkg', link: true },
                },
            }, null, 2));

            var res = runInstaller(targetDir, ['--install']);
            assert.equal(res.status, 0, diag(targetDir, res));

            var on_disk = fs.readdirSync(path.join(targetDir, 'node_modules')).filter(n => n.charAt(0) !== '.').sort();

            assert.deepEqual(on_disk, ['linkpkg'],
                'the removed dependency is not installed back: ' + on_disk.join(', '));
        });

        it('takes an omitted dependency off the disk but keeps it in the lockfile', function () {
            // npm's `--omit=dev` is about the disk and never about the graph: a full
            // install followed by `--omit=dev` used to leave the dev tree in place
            var targetDir = makeTargetDir();

            writeFile(targetDir, 'package.json', JSON.stringify({
                name: 'omit-proj', version: '1.0.0', private: true,
                dependencies: { 'is-number': '6.0.0' },
                devDependencies: { 'linkpkg': 'file:linkpkg' },
            }, null, 2));
            writeFile(targetDir, 'linkpkg/package.json', JSON.stringify({
                name: 'linkpkg', version: '1.0.0',
            }, null, 2));

            runInstaller(targetDir, ['--install']);

            var res = runInstaller(targetDir, ['--install', '--omit=dev']);
            assert.equal(res.status, 0, diag(targetDir, res));

            assert.equal(checkInstalled(targetDir, 'linkpkg'), false,
                'the dev dependency left the disk: ' + listDir(path.join(targetDir, 'node_modules')));

            var packages = lockOf(targetDir).packages;

            assert.ok(packages['packages/../linkpkg'] || packages['linkpkg'],
                'and stays in the lockfile: ' + Object.keys(packages).join(', '));
        });

        it('keeps the workspace links a project owns when it prunes', function () {
            // no dependency edge arrives at a member — the manifest says `workspaces` —
            // so a prune that read the graph alone deleted the link it had just built
            var targetDir = makeTargetDir();

            writeFile(targetDir, 'package.json', JSON.stringify({
                name: 'ws-prune-proj', version: '1.0.0', private: true,
                workspaces: ['packages/*'],
            }, null, 2));
            writeFile(targetDir, 'packages/core/package.json', JSON.stringify({
                name: '@z/core', version: '0.1.0', private: true,
            }, null, 2));

            runInstaller(targetDir, ['--install']);

            var res = runInstaller(targetDir, ['--install']);
            assert.equal(res.status, 0, diag(targetDir, res));

            assert.ok(checkInstalled(targetDir, '@z/core'),
                'the member is still linked after a second install: ' +
                listDir(path.join(targetDir, 'node_modules')));
        });

        it('refuses to install where there is no package.json', function () {
            // a `cd` to the wrong place in a script looked like it worked: nothing was
            // installed and the exit code was 0. npm refuses with ENOENT
            var targetDir = makeTargetDir();

            var res = runInstaller(targetDir, ['--install']);
            var out = outputOf(res);

            assert.notEqual(res.status, 0, 'an install with nothing to install is an error' + diag(targetDir, res));
            assert.ok(out.indexOf('no package.json') > -1, out);
            assert.equal(fs.existsSync(path.join(targetDir, 'package-lock.json')), false,
                'and nothing was written');
        });

        it('still installs a named package where there is no package.json', function () {
            // the one command whose purpose is to create the project
            var targetDir = makeTargetDir();

            var res = runInstaller(targetDir, ['--install', 'is-number']);

            assert.equal(res.status, 0, 'naming a package is how a project starts' + diag(targetDir, res));
            assert.ok(fs.existsSync(path.join(targetDir, 'node_modules', 'is-number')) ||
                fs.existsSync(path.join(targetDir, 'package-lock.json')),
                'something was installed: ' + listDir(path.join(targetDir, 'node_modules')));
        });

        it('never fetches a workspace member from the registry', function () {
            // the report's fourth defect: a member the tree held as a plain directory
            // was described as a registry package, and the frozen install derived a
            // tarball url from its name — 404 for anything not published
            var targetDir = makeTargetDir();

            writeFile(targetDir, 'package.json', JSON.stringify({
                name: 'ws-no-fetch', version: '1.0.0', private: true, workspaces: ['packages/*'],
            }, null, 2));
            writeFile(targetDir, 'packages/templates/package.json', JSON.stringify({
                version: '0.1.0', private: true,
                dependencies: { linkpkg: 'file:../../linkpkg' },
            }, null, 2));
            writeFile(targetDir, 'linkpkg/package.json', JSON.stringify({ name: 'linkpkg', version: '1.0.0' }, null, 2));

            // the state the report describes: the member is already a link in the tree
            // (npm put it there) while the lockfile is gone
            fs.mkdirSync(path.join(targetDir, 'node_modules'), { recursive: true });
            fs.symlinkSync(path.join(targetDir, 'packages', 'templates'),
                path.join(targetDir, 'node_modules', 'templates'), 'dir');

            var res = runInstaller(targetDir, ['--install']);
            var out = outputOf(res);

            assert.equal(res.status, 0, diag(targetDir, res));
            assert.equal(out.indexOf('registry.npmjs.org/templates') > -1, false,
                'the member was not turned into a registry url:\n' + out);

            var packages = lockOf(targetDir).packages;

            assert.deepEqual(packages['node_modules/templates'], { resolved: 'packages/templates', link: true },
                'the tree on disk is a link and the lockfile says so: ' +
                JSON.stringify(packages['node_modules/templates']));
            assert.ok(!packages['node_modules/templates'].version,
                'and a link carries no version: ' + JSON.stringify(packages['node_modules/templates']));
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

    // ===== Phase 10: the geometry of an install (the defect audit) =====
    // plans/installer-lockfile-defect-audit-2026-09-28.md: scoped packages were
    // invisible to the snapshot, a dependency an installed package was missing stayed
    // missing, a write back dropped what it did not fetch itself, npm aliases lost the
    // name that marks them, a peer conflict nested a second copy, and the sync check
    // accepted entries the dependent cannot even see
    if (isFibjs) describe('lockfile / install geometry (Phase 10)', function () {
        function writeFile(dir, rel, content) {
            var p = path.join(dir, rel);
            fs.mkdirSync(path.dirname(p), { recursive: true });
            fs.writeFileSync(p, content);
        }

        function writeJSON(dir, rel, value) {
            writeFile(dir, rel, JSON.stringify(value, null, 2));
        }

        function lockOf(dir) {
            return JSON.parse(fs.readFileSync(path.join(dir, 'package-lock.json'), 'utf8'));
        }

        function versionAt(dir, rel) {
            var p = path.join(dir, rel, 'package.json');
            if (!fs.existsSync(p)) return null;
            return JSON.parse(fs.readFileSync(p, 'utf8')).version;
        }

        function isLink(dir, rel) {
            try {
                return fs.lstatSync(path.join(dir, rel)).isSymbolicLink();
            } catch (e) {
                return false;
            }
        }

        var SRI = 'sha512-' + 'A'.repeat(86) + '==';

        /**
         * @description a project whose tree npm already laid out: `pkg-a` on disk, a
         *              scoped dependency of it beside it, and a lockfile that describes
         *              them (the shape the audit's foreign tree had)
         */
        function makeForeignTree(opts) {
            var o = opts || {};
            var targetDir = makeTargetDir();

            writeJSON(targetDir, 'package.json', {
                name: 'foreign-proj', version: '1.0.0',
                dependencies: { 'pkg-a': '1.0.0' },
            });

            var lock = {
                name: 'foreign-proj', version: '1.0.0', lockfileVersion: 3, requires: true,
                packages: {
                    '': { name: 'foreign-proj', version: '1.0.0', dependencies: { 'pkg-a': '1.0.0' } },
                    'node_modules/pkg-a': {
                        version: '1.0.0',
                        resolved: 'https://registry.npmjs.org/pkg-a/-/pkg-a-1.0.0.tgz',
                        integrity: SRI,
                        engines: { node: '>=14' },
                        dependencies: { '@scope/dep': '^1.0.0' },
                    },
                    'node_modules/@scope/dep': {
                        version: '1.0.0',
                        resolved: 'https://registry.npmjs.org/@scope/dep/-/dep-1.0.0.tgz',
                        integrity: SRI,
                    },
                },
            };

            if (o.drop_scoped_lock_entry)
                delete lock.packages['node_modules/@scope/dep'];

            writeJSON(targetDir, 'package-lock.json', lock);

            writeJSON(targetDir, 'node_modules/pkg-a/package.json', {
                name: 'pkg-a', version: '1.0.0', dependencies: { '@scope/dep': '^1.0.0' },
            });
            writeJSON(targetDir, 'node_modules/@scope/dep/package.json', {
                name: '@scope/dep', version: '1.0.0',
            });

            // a `file:` dependency the lockfile does not describe: it makes the check
            // fail, which is what sends the install down the resolving path, and it
            // resolves from the disk (no registry needed)
            writeJSON(targetDir, 'local-pkg/package.json', { name: 'local-pkg', version: '1.0.0' });
            if (!o.no_manifest_change) {
                var pkgjson = JSON.parse(fs.readFileSync(path.join(targetDir, 'package.json'), 'utf8'));
                pkgjson.dependencies['local-pkg'] = 'file:local-pkg';
                fs.writeFileSync(path.join(targetDir, 'package.json'), JSON.stringify(pkgjson, null, 2));
            }

            return targetDir;
        }

        it('sees a scoped package that is on disk, and keeps it when it writes the lockfile', function () {
            var targetDir = makeForeignTree();
            var res = runInstaller(targetDir, ['--install', '--lockfile-only']);

            assert.equal(res.status, 0, diag(targetDir, res));

            var lock = lockOf(targetDir).packages;
            var scoped = lock['node_modules/@scope/dep'];

            assert.ok(scoped, 'the scoped dependency of an installed package is described: ' +
                Object.keys(lock).length + ' entries');
            assert.equal(scoped.version, '1.0.0');

            // a node this run did not fetch keeps what the lockfile already said about
            // it: dropping `resolved`/`integrity` (and the fields npm writes) is what
            // made a write back unusable for `--ci`
            assert.equal(scoped.resolved, 'https://registry.npmjs.org/@scope/dep/-/dep-1.0.0.tgz',
                'the source survived the write back');
            assert.equal(scoped.integrity, SRI, 'the integrity survived the write back');
            assert.equal(lock['node_modules/pkg-a'].engines.node, '>=14', 'what npm wrote is kept');

            var ci = runInstaller(targetDir, ['--install', '--ci', '--dry-run']);
            assert.equal(ci.status, 0, 'the lockfile it wrote is in sync, what it is not:\n' + outputOf(ci));
        });

        it('installs a dependency an installed package declares and the tree lacks', function () {
            var targetDir = makeForeignTree({ drop_scoped_lock_entry: true });

            // `pkg-a` is on disk and asks for something the tree does not have, and the
            // lockfile does not describe either: the walk has to look at what an
            // installed package declares, or the hole is written into the lockfile
            fs.rmSync(path.join(targetDir, 'node_modules/@scope/dep'), { recursive: true, force: true });
            writeJSON(targetDir, 'node_modules/pkg-a/package.json', {
                name: 'pkg-a', version: '1.0.0', dependencies: { 'local-dep': 'file:../../local-dep' },
            });
            writeJSON(targetDir, 'local-dep/package.json', { name: 'local-dep', version: '1.0.0' });

            var res = runInstaller(targetDir, ['--install']);
            assert.equal(res.status, 0, diag(targetDir, res));
            assert.ok(isLink(targetDir, 'node_modules/local-dep'),
                'the missing dependency is installed: ' + listDir(path.join(targetDir, 'node_modules')));

            var lock = lockOf(targetDir).packages;
            assert.ok(lock['node_modules/local-dep'], 'and described: ' + Object.keys(lock).join(', '));

            var ci = runInstaller(targetDir, ['--install', '--ci', '--dry-run']);
            assert.equal(ci.status, 0, 'the lockfile it left is in sync:\n' + outputOf(ci));
        });

        it('materializes a workspace member a package on disk depends on', function () {
            var targetDir = makeTargetDir();

            writeJSON(targetDir, 'package.json', {
                name: 'ws-proj', version: '1.0.0', private: true,
                workspaces: ['packages/*'],
                dependencies: { 'pkg-a': '1.0.0' },
            });
            writeJSON(targetDir, 'packages/sib/package.json', { name: 'sib', version: '0.1.0' });

            // `pkg-a` is installed and asks for the workspace member. The lockfile does
            // not describe `sib`, so the check fails and the install has to resolve the
            // edge — a workspace package is provided by the workspace itself, never by a
            // registry (that is how `--update` came to 404 on one)
            writeJSON(targetDir, 'node_modules/pkg-a/package.json', {
                name: 'pkg-a', version: '1.0.0', dependencies: { sib: '*' },
            });
            writeJSON(targetDir, 'package-lock.json', {
                name: 'ws-proj', version: '1.0.0', lockfileVersion: 3, requires: true,
                packages: {
                    '': {
                        name: 'ws-proj', version: '1.0.0', workspaces: ['packages/*'],
                        dependencies: { 'pkg-a': '1.0.0' },
                    },
                    'node_modules/pkg-a': {
                        version: '1.0.0',
                        resolved: 'https://registry.npmjs.org/pkg-a/-/pkg-a-1.0.0.tgz',
                        integrity: SRI,
                        dependencies: { sib: '*' },
                    },
                },
            });

            var res = runInstaller(targetDir, ['--install']);
            assert.equal(res.status, 0, diag(targetDir, res));
            assert.ok(isLink(targetDir, 'node_modules/sib'), 'the member is linked at the root: ' + listDir(path.join(targetDir, 'node_modules')));

            var lock = lockOf(targetDir).packages;
            assert.ok(lock['node_modules/sib'] && lock['node_modules/sib'].link === true,
                'and the lockfile describes the link');
        });

        it('installs the devDependencies of a workspace member', function () {
            var targetDir = makeTargetDir();

            writeJSON(targetDir, 'package.json', {
                name: 'member-dev-proj', version: '1.0.0', private: true,
                workspaces: ['packages/*'],
            });
            writeJSON(targetDir, 'packages/member/package.json', {
                name: 'member', version: '0.1.0',
                devDependencies: { 'dev-pkg': 'file:../../dev-pkg' },
            });
            writeJSON(targetDir, 'dev-pkg/package.json', { name: 'dev-pkg', version: '1.0.0' });

            var res = runInstaller(targetDir, ['--install']);
            assert.equal(res.status, 0, diag(targetDir, res));

            // a member is a project of its own: its devDependencies are installed with the
            // project (npm does the same), and a lockfile without them is refused by
            // `--ci` — and by npm
            var lock = lockOf(targetDir).packages;
            assert.ok(lock['node_modules/dev-pkg'] && lock['node_modules/dev-pkg'].link === true,
                'the member dev dependency is described: ' + Object.keys(lock).join(', '));
            assert.ok(lock['dev-pkg'], 'and so is the directory it points at');

            // `workspaces` is one of the fields npm projects into the root entry: without
            // it, npm and this rewrite the lockfile back and forth
            assert.deepEqual(lock[''].workspaces, ['packages/*'],
                'the root entry keeps the field npm writes');

            var ci = runInstaller(targetDir, ['--install', '--ci', '--dry-run']);
            assert.equal(ci.status, 0, 'the lockfile it wrote is in sync:\n' + outputOf(ci));
        });

        it('keeps a member dependency that cannot hoist inside the member', function () {
            var targetDir = makeTargetDir();

            writeJSON(targetDir, 'package.json', {
                name: 'member-nest-proj', version: '1.0.0', private: true,
                workspaces: ['packages/*'],
                dependencies: { 'dep-pkg': 'file:root-pkg' },
            });
            writeJSON(targetDir, 'packages/member/package.json', {
                name: 'member', version: '0.1.0',
                devDependencies: { 'dep-pkg': 'file:../../member-pkg' },
            });
            writeJSON(targetDir, 'root-pkg/package.json', { name: 'dep-pkg', version: '1.0.0' });
            writeJSON(targetDir, 'member-pkg/package.json', { name: 'dep-pkg', version: '2.0.0' });

            var res = runInstaller(targetDir, ['--install']);
            assert.equal(res.status, 0, diag(targetDir, res));

            var lock = lockOf(targetDir).packages;

            // the root's dep-pkg is 1.0.0 and the member's is another directory, so the
            // member's copy cannot hoist. It belongs *inside the member* — the same thing
            // on disk as `node_modules/member/node_modules/dep-pkg`, and the path npm
            // writes
            assert.ok(lock['packages/member/node_modules/dep-pkg'],
                'described inside the member: ' + Object.keys(lock).join(', '));
            assert.equal(lock['node_modules/member/node_modules/dep-pkg'], undefined,
                'never under the link that stands for the member');

            var ci = runInstaller(targetDir, ['--install', '--ci', '--dry-run']);
            assert.equal(ci.status, 0, 'the lockfile it wrote is in sync:\n' + outputOf(ci));

            // a lockfile describes where everything goes, so emptying the tree and
            // installing again has to put it all back — including what the member keeps
            fs.rmSync(path.join(targetDir, 'node_modules'), { recursive: true, force: true });
            fs.rmSync(path.join(targetDir, 'packages/member/node_modules'), { recursive: true, force: true });

            var again = runInstaller(targetDir, ['--install']);
            assert.equal(again.status, 0, diag(targetDir, again));
            assert.ok(isLink(targetDir, 'packages/member/node_modules/dep-pkg'),
                'the member dependency came back: ' + listDir(path.join(targetDir, 'packages/member')) +
                ' | ' + listDir(path.join(targetDir, 'node_modules')));
            assert.ok(isLink(targetDir, 'node_modules/dep-pkg'), 'so did the root one');
        });

        it('leaves a member dev dependency out of the tree with --omit=dev, and keeps it described', function () {
            var targetDir = makeTargetDir();

            writeJSON(targetDir, 'package.json', {
                name: 'member-omit-proj', version: '1.0.0', private: true,
                workspaces: ['packages/*'],
            });
            writeJSON(targetDir, 'packages/member/package.json', {
                name: 'member', version: '0.1.0',
                devDependencies: { 'dev-pkg': 'file:../../dev-pkg' },
            });
            writeJSON(targetDir, 'dev-pkg/package.json', { name: 'dev-pkg', version: '1.0.0' });

            var res = runInstaller(targetDir, ['--install', '--omit=dev']);
            assert.equal(res.status, 0, diag(targetDir, res));

            assert.ok(lockOf(targetDir).packages['node_modules/dev-pkg'],
                'a lockfile describes the whole graph, `omit` is about the disk');
            assert.ok(!fs.existsSync(path.join(targetDir, 'packages/member/node_modules/dev-pkg')) &&
                !fs.existsSync(path.join(targetDir, 'node_modules/dev-pkg')),
                'and it is not installed: ' + listDir(path.join(targetDir, 'node_modules')));
        });

        it('refuses a lockfile that only an unrelated nested entry satisfies', function () {
            var targetDir = makeTargetDir();

            writeJSON(targetDir, 'package.json', {
                name: 'geom-proj', version: '1.0.0', dependencies: { 'is-odd': '3.0.1', 'is-number': '6.0.0' },
            });
            writeJSON(targetDir, 'package-lock.json', {
                name: 'geom-proj', version: '1.0.0', lockfileVersion: 3, requires: true,
                packages: {
                    '': {
                        name: 'geom-proj', version: '1.0.0',
                        dependencies: { 'is-odd': '3.0.1', 'is-number': '6.0.0' },
                    },
                    'node_modules/is-number': {
                        version: '7.0.0',
                        resolved: 'https://registry.npmjs.org/is-number/-/is-number-7.0.0.tgz',
                        integrity: SRI,
                    },
                    // the 6.0.0 lives under is-odd, where nothing but is-odd can reach it
                    'node_modules/is-odd': {
                        version: '3.0.1',
                        resolved: 'https://registry.npmjs.org/is-odd/-/is-odd-3.0.1.tgz',
                        integrity: SRI,
                        dependencies: { 'is-number': '^6.0.0' },
                    },
                    'node_modules/is-odd/node_modules/is-number': {
                        version: '6.0.0',
                        resolved: 'https://registry.npmjs.org/is-number/-/is-number-6.0.0.tgz',
                        integrity: SRI,
                    },
                },
            });

            var res = runInstaller(targetDir, ['--install', '--ci', '--dry-run']);
            var out = outputOf(res);

            assert.equal(res.status, 1, 'npm refuses this lockfile, and so has to fibjs:\n' + out);
            assert.ok(out.indexOf("Invalid: lock file's is-number@7.0.0 does not satisfy is-number@6.0.0") > -1,
                'npm\'s own sentence:\n' + out);
        });

        it('summarizes a sync failure, and lists every entry with --verbose', function () {
            var targetDir = makeTargetDir();
            var deps = {};

            for (var i = 0; i < 25; i++)
                deps['missing-pkg-' + i] = '1.0.0';

            writeJSON(targetDir, 'package.json', { name: 'loud-proj', version: '1.0.0', dependencies: deps });
            writeJSON(targetDir, 'package-lock.json', {
                name: 'loud-proj', version: '1.0.0', lockfileVersion: 3, requires: true,
                packages: { '': { name: 'loud-proj', version: '1.0.0', dependencies: deps } },
            });

            var res = runInstaller(targetDir, ['--install', '--ci', '--dry-run']);
            var out = outputOf(res);

            assert.equal(res.status, 1, diag(targetDir, res));
            assert.ok(out.indexOf('25 errors (Missing 25)') > -1, 'a summary first:\n' + out);
            assert.ok(out.indexOf('and 5 more (--verbose lists them all)') > -1, 'the tail is counted:\n' + out);
            assert.equal(out.split('Missing: missing-pkg-').length - 1, 20, '20 details by default');

            var loud = outputOf(runInstaller(targetDir, ['--install', '--ci', '--dry-run', '--verbose']));
            assert.equal(loud.split('Missing: missing-pkg-').length - 1, 25, '--verbose prints them all');
        });

        it('leaves node_modules alone when the check fails', function () {
            var targetDir = makeTargetDir();

            writeJSON(targetDir, 'package.json', {
                name: 'untouched-proj', version: '1.0.0', private: true,
                workspaces: ['packages/*'], dependencies: { 'is-odd': '3.0.1' },
            });
            writeJSON(targetDir, 'packages/member/package.json', { name: 'member', version: '0.1.0' });
            writeJSON(targetDir, 'package-lock.json', {
                name: 'untouched-proj', version: '1.0.0', lockfileVersion: 3, requires: true,
                packages: {
                    '': {
                        name: 'untouched-proj', version: '1.0.0', workspaces: ['packages/*'],
                        dependencies: { 'is-odd': '3.0.1' },
                    },
                    'node_modules/is-odd': {
                        version: '9.9.9',
                        resolved: 'https://registry.npmjs.org/is-odd/-/is-odd-9.9.9.tgz',
                        integrity: SRI,
                    },
                },
            });
            writeFile(targetDir, 'node_modules/sentinel.txt', 'this file has to survive a failed check');

            var res = runInstaller(targetDir, ['--install', '--ci']);
            assert.equal(res.status, 1, diag(targetDir, res));
            assert.ok(fs.existsSync(path.join(targetDir, 'node_modules/sentinel.txt')),
                'a failed check never touches the disk: ' + listDir(path.join(targetDir, 'node_modules')));
            assert.ok(!fs.existsSync(path.join(targetDir, 'node_modules/member')),
                'and the workspace links are created only once it passed');
        });

        var NET = { skip: !process.env.FIBJS_TEST_NETWORK };

        it('installs an npm alias at its own path and records it the way npm does', NET, function () {
            var targetDir = makeTargetDir();

            writeJSON(targetDir, 'package.json', {
                name: 'alias-proj', version: '1.0.0',
                dependencies: { '@isaacs/cliui': '8.0.2' },
            });

            var res = runInstaller(targetDir, ['--install']);
            assert.equal(res.status, 0, diag(targetDir, res));

            var lock = lockOf(targetDir).packages;
            var alias = lock['node_modules/string-width-cjs'];

            assert.ok(alias, 'the alias entry is there: ' + Object.keys(lock).filter(function (k) {
                return k.indexOf('string-width') > -1;
            }).join(', '));
            assert.equal(alias.name, 'string-width', 'the name it goes by is what marks it an alias');

            // and the package really sits at the alias path, not at its own name
            assert.equal(versionAt(targetDir, 'node_modules/string-width-cjs'), '4.2.3');
            assert.equal(versionAt(targetDir, 'node_modules/string-width'), '5.1.2',
                'the package of that name is a different one and stays');

            var ci = runInstaller(targetDir, ['--install', '--ci', '--dry-run']);
            assert.equal(ci.status, 0, 'the installer accepts the lockfile it wrote:\n' + outputOf(ci));
        });

        it('keeps the version an alias edge pins', NET, function () {
            var targetDir = makeTargetDir();

            writeJSON(targetDir, 'package.json', {
                name: 'alias-pin-proj', version: '1.0.0',
                dependencies: { 'string-width-cjs': 'npm:string-width@^4.2.0' },
            });

            var npm = process.platform === 'win32' ? 'npm.cmd' : 'npm';
            var npmRes = child_process.spawnSync(npm, ['install', '--package-lock-only', '--no-audit', '--no-fund'], {
                cwd: targetDir, stdio: 'pipe', shell: process.platform === 'win32',
            });
            assert.equal(npmRes.status, 0, String(npmRes.stderr || ''));

            // pin the alias one release back, with the integrity of that release: the
            // spec (`^4.2.0`) allows 4.2.3, and only the lockfile says 4.2.2 —
            // `semver.satisfies` cannot read an alias spec, so this used to resolve
            // again and install 4.2.3
            var tarball = 'https://registry.npmjs.org/string-width/-/string-width-4.2.2.tgz';
            var http = require('http');
            var crypto = require('crypto');
            var bytes = http.getSync(tarball).bytes();
            var sri = 'sha512-' + crypto.createHash('sha512').update(bytes).digest('base64');

            var raw = lockOf(targetDir);
            raw.packages['node_modules/string-width-cjs'].version = '4.2.2';
            raw.packages['node_modules/string-width-cjs'].resolved = tarball;
            raw.packages['node_modules/string-width-cjs'].integrity = sri;
            writeJSON(targetDir, 'package-lock.json', raw);

            // a dependency the lockfile does not have: the install has to resolve, and
            // that is exactly where the pin used to be thrown away
            var pkgjson = JSON.parse(fs.readFileSync(path.join(targetDir, 'package.json'), 'utf8'));
            pkgjson.dependencies['is-odd'] = '3.0.1';
            writeJSON(targetDir, 'package.json', pkgjson);

            var res = runInstaller(targetDir, ['--install']);
            assert.equal(res.status, 0, diag(targetDir, res));

            assert.equal(versionAt(targetDir, 'node_modules/string-width-cjs'), '4.2.2',
                'the version the lockfile pins is the one installed');
            assert.equal(lockOf(targetDir).packages['node_modules/string-width-cjs'].version, '4.2.2',
                'and the lockfile still says so');
        });

        it('places a copy for every branch that needs one', NET, function () {
            var targetDir = makeTargetDir();

            writeJSON(targetDir, 'package.json', {
                name: 'n1-proj', version: '1.0.0', private: true,
                workspaces: ['packages/*'],
                dependencies: { 'is-number': '7.0.0' },
            });
            writeJSON(targetDir, 'packages/a/package.json', {
                name: '@probe/a', version: '1.0.0',
                dependencies: { 'is-odd': '3.0.1' },
                devDependencies: { 'is-buffer': '2.0.5' },
            });
            // the exact pin is what makes the two branches need a copy each: the shared
            // closure cannot hoist (the root has is-odd 3.0.1 for @probe/a), so the root
            // branch and the member have to see one of their own
            writeJSON(targetDir, 'packages/b/package.json', {
                name: '@probe/b', version: '1.0.0',
                dependencies: { '@probe/a': '*', 'is-even': '1.0.0', 'is-odd': '0.1.2' },
            });

            var res = runInstaller(targetDir, ['--install']);
            assert.equal(res.status, 0, diag(targetDir, res));

            assert.equal(versionAt(targetDir, 'node_modules/is-even/node_modules/is-odd'), '0.1.2',
                'the root branch gets the copy it needs: ' + listDir(path.join(targetDir, 'node_modules', 'is-even')));
            assert.equal(versionAt(targetDir, 'packages/b/node_modules/is-odd'), '0.1.2',
                'and so does the member: ' + listDir(path.join(targetDir, 'packages/b', 'node_modules')));

            // npm is the judge of the shape: nothing may be missing, and the only
            // complaint it is allowed to make is the peer this project cannot satisfy
            // with one copy (npm resolves the root range to the peer's version, this
            // keeps the installed one — the divergence the audit recorded)
            var npm = process.platform === 'win32' ? 'npm.cmd' : 'npm';
            var problems = String(child_process.spawnSync(npm, ['ls', '--all'], {
                cwd: targetDir, stdio: 'pipe', shell: process.platform === 'win32',
            }).stdout || '');

            assert.equal(problems.indexOf('missing') < 0, true,
                'npm ls finds nothing missing from the tree:\n' + problems);

            problems.split('\n')
                .filter(l => l.indexOf('invalid') > -1)
                .forEach(l => assert.ok(l.indexOf('data-model') > -1, 'an unexpected invalid entry:\n' + l));

            var again = runInstaller(targetDir, ['--install']);
            assert.equal(again.status, 0, diag(targetDir, again));

            var ci = runInstaller(targetDir, ['--install', '--ci', '--dry-run']);
            assert.equal(ci.status, 0, 'the lockfile it wrote is in sync:\n' + outputOf(ci));
        });

        it('installs a dependency an installed package is missing', NET, function () {
            var targetDir = makeTargetDir();

            writeJSON(targetDir, 'package.json', {
                name: 'repair-proj', version: '1.0.0', dependencies: { 'xml-crypto': '^6.1.2' },
            });

            var npm = process.platform === 'win32' ? 'npm.cmd' : 'npm';
            var npmRes = child_process.spawnSync(npm, ['install', '--no-audit', '--no-fund'], {
                cwd: targetDir, stdio: 'pipe', shell: process.platform === 'win32',
            });
            assert.equal(npmRes.status, 0, String(npmRes.stderr || ''));

            // the tree loses a dependency of an installed package. `pkg-a` stays on
            // disk, so the walk has to look at what it declares — it used to stop at
            // the packages this run created, and the hole was written into the lockfile
            fs.rmSync(path.join(targetDir, 'node_modules/xpath'), { recursive: true, force: true });

            var pkgjson = JSON.parse(fs.readFileSync(path.join(targetDir, 'package.json'), 'utf8'));
            pkgjson.dependencies['is-odd'] = '3.0.1';
            writeJSON(targetDir, 'package.json', pkgjson);

            var res = runInstaller(targetDir, ['--install']);
            assert.equal(res.status, 0, diag(targetDir, res));
            assert.ok(versionAt(targetDir, 'node_modules/xpath'), 'the missing dependency is installed');
            assert.ok(lockOf(targetDir).packages['node_modules/xpath'], 'and described');

            var ci = runInstaller(targetDir, ['--install', '--ci', '--dry-run']);
            assert.equal(ci.status, 0, 'the lockfile it left is in sync:\n' + outputOf(ci));
        });

        // a registry is needed for the rest of them: they install real packages

        /**
         * A workspace member's range moving past the copy the project already has is
         * not a reason for a second copy: the root's own range still allows the newer
         * version, so the copy the member would have nested can be the copy everybody
         * shares. npm does exactly that (`PlaceDep` picks the highest slot whose
         * neighbours all survive the swap), and the nested copy this used to leave
         * behind was two versions of one package on one machine — the thing a hoisted
         * tree exists to avoid.
         */
        it('promotes the shared copy when only a member asks for the newer version', NET, function () {
            var targetDir = makeTargetDir();

            writeJSON(targetDir, 'package.json', {
                name: 'ws-promote-proj', version: '1.0.0', private: true,
                workspaces: ['packages/*'], dependencies: { 'is-odd': '3.0.0' },
            });
            writeJSON(targetDir, 'packages/member/package.json', {
                name: 'member', version: '0.1.0', dependencies: { 'is-odd': '3.0.0' },
            });

            var first = runInstaller(targetDir, ['--install']);
            assert.equal(first.status, 0, diag(targetDir, first));

            assert.equal(versionAt(targetDir, 'node_modules/is-odd'), '3.0.0', 'one copy to start with');

            // the member moves past it, and the root's range is widened so the newer
            // version is allowed at the top as well
            writeJSON(targetDir, 'package.json', {
                name: 'ws-promote-proj', version: '1.0.0', private: true,
                workspaces: ['packages/*'], dependencies: { 'is-odd': '^3.0.0' },
            });
            writeJSON(targetDir, 'packages/member/package.json', {
                name: 'member', version: '0.1.0', dependencies: { 'is-odd': '^3.0.1' },
            });

            var res = runInstaller(targetDir, ['--install']);
            assert.equal(res.status, 0, diag(targetDir, res));

            var lock = lockOf(targetDir).packages;

            assert.equal(versionAt(targetDir, 'node_modules/is-odd'), '3.0.1',
                'the shared copy moved up instead of a second one appearing');
            assert.ok(!fs.existsSync(path.join(targetDir, 'packages/member/node_modules/is-odd')),
                'and nothing was nested under the member: ' +
                listDir(path.join(targetDir, 'packages/member', 'node_modules')));

            assert.equal(lock['node_modules/is-odd'].version, '3.0.1', 'the lockfile says the same');
            assert.equal(lock['packages/member/node_modules/is-odd'], undefined,
                'with no copy of its own: ' + Object.keys(lock).filter(function (k) {
                    return k.indexOf('is-odd') > -1;
                }).join(', '));

            var ci = runInstaller(targetDir, ['--install', '--ci', '--dry-run']);
            assert.equal(ci.status, 0, 'the lockfile it wrote is in sync:\n' + outputOf(ci));

            // and npm agrees with the shape: the tree has to be one npm would build
            var npm = process.platform === 'win32' ? 'npm.cmd' : 'npm';
            var problems = String(child_process.spawnSync(npm, ['ls', '--all'], {
                cwd: targetDir, stdio: 'pipe', shell: process.platform === 'win32',
            }).stdout || '');

            assert.equal(problems.indexOf('invalid') < 0, true, 'npm ls finds nothing invalid:\n' + problems);
            assert.equal(problems.indexOf('missing') < 0, true, 'nor anything missing:\n' + problems);
        });

        /**
         * The other side of the same rule: a copy the project pinned exactly is not
         * fair game. Every edge that resolves to it today has to survive the swap, so
         * the member gets a copy of its own instead — which is what npm does as well.
         */
        it('keeps a copy the project pinned when a member needs another version', NET, function () {
            var targetDir = makeTargetDir();

            writeJSON(targetDir, 'package.json', {
                name: 'ws-pinned-proj', version: '1.0.0', private: true,
                workspaces: ['packages/*'], dependencies: { 'is-odd': '3.0.0' },
            });
            writeJSON(targetDir, 'packages/member/package.json', {
                name: 'member', version: '0.1.0', dependencies: { 'is-odd': '^3.0.1' },
            });

            var res = runInstaller(targetDir, ['--install']);
            assert.equal(res.status, 0, diag(targetDir, res));

            assert.equal(versionAt(targetDir, 'node_modules/is-odd'), '3.0.0',
                'the pinned copy stays where the project asked for it');
            assert.equal(versionAt(targetDir, 'packages/member/node_modules/is-odd'), '3.0.1',
                'and the member gets its own: ' +
                listDir(path.join(targetDir, 'packages/member', 'node_modules')));

            var lock = lockOf(targetDir).packages;

            assert.equal(lock['node_modules/is-odd'].version, '3.0.0');
            assert.equal(lock['packages/member/node_modules/is-odd'].version, '3.0.1');

            var ci = runInstaller(targetDir, ['--install', '--ci', '--dry-run']);
            assert.equal(ci.status, 0, 'the lockfile it wrote is in sync:\n' + outputOf(ci));
        });

        it('refreshes a registry dependency without fetching a workspace sibling', NET, function () {
            var targetDir = makeTargetDir();

            writeJSON(targetDir, 'package.json', {
                name: 'update-proj', version: '1.0.0', private: true,
                workspaces: ['packages/*'], dependencies: { 'is-odd': '3.0.1' },
            });
            writeJSON(targetDir, 'packages/file-tools/package.json', {
                name: 'file-tools', version: '0.1.0',
                // the named package first, so the refresh reaches the sibling after it:
                // that is the order that used to 404
                dependencies: { 'is-number': '6.0.0', 'app-host': '*' },
            });
            writeJSON(targetDir, 'packages/app-host/package.json', { name: 'app-host', version: '0.1.0' });

            var npm = process.platform === 'win32' ? 'npm.cmd' : 'npm';
            var npmRes = child_process.spawnSync(npm, ['install', '--no-audit', '--no-fund'], {
                cwd: targetDir, stdio: 'pipe', shell: process.platform === 'win32',
            });
            assert.equal(npmRes.status, 0, String(npmRes.stderr || ''));

            var res = runInstaller(targetDir, ['--install', '--update', 'is-number']);
            var out = outputOf(res);

            assert.equal(res.status, 0, diag(targetDir, res));
            assert.equal(out.indexOf('cannot fetch metadata'), -1, 'the workspace package is not a registry one:\n' + out);
            assert.ok(isLink(targetDir, 'node_modules/app-host'), 'it stays the link the workspace made');
        });
        it('keeps one copy of a package a peer range does not accept', NET, function () {
            var targetDir = makeTargetDir();

            writeJSON(targetDir, 'package.json', {
                name: 'peer-proj', version: '1.0.0',
                dependencies: {
                    '@mlightcad/libredwg-converter': '3.14.6',
                    '@mlightcad/data-model': '^1.14.6',
                },
            });

            var res = runInstaller(targetDir, ['--install']);
            assert.equal(res.status, 0, diag(targetDir, res));

            // the converter's peer asks for data-model 1.14.6 exactly, the root for
            // ^1.14.6: nesting a second copy is what broke a module level singleton
            // downstream (npm keeps one, and so does this now)
            var copies = [];
            (function walk(dir) {
                fs.readdirSync(dir, { withFileTypes: true }).forEach(function (e) {
                    var full = path.join(dir, e.name);
                    if (e.name === 'data-model' && path.basename(dir) === '@mlightcad')
                        copies.push(path.relative(targetDir, full));
                    else if (e.isDirectory())
                        walk(full);
                });
            })(path.join(targetDir, 'node_modules'));

            assert.equal(copies.length, 1, 'one copy of data-model: ' + copies.join(', '));
        });
    });

    // ===== Phase 11: a download that stops early is a failed download =====
    // the registry here is a raw socket: it is the only way to send a body that stops
    // before the length it announced, which is what a connection dropping mid-tarball
    // looks like from the client's side
    if (isFibjs) describe('install / download integrity (Phase 11)', function () {
        var net = require('net');
        var crypto = require('crypto');

        var server = null;
        var port = 0;
        var tarball = null;
        var integrity = '';
        var tarball_requests = 0;
        var fixtureDir = '';

        before(function () {
            fixtureDir = path.join(TMP_DIR, 'truncated_' + Date.now() + '_' + Math.random().toString(36).slice(2));

            var pkgDir = path.join(fixtureDir, 'package');
            fs.mkdirSync(pkgDir, { recursive: true });
            fs.writeFileSync(path.join(pkgDir, 'package.json'), JSON.stringify({ name: 'trunc-pkg', version: '1.0.0' }));
            fs.writeFileSync(path.join(pkgDir, 'index.js'), 'module.exports = 42;\n');

            var res = child_process.spawnSync('tar', ['czf', 'trunc-pkg-1.0.0.tgz', 'package'], {
                cwd: fixtureDir, stdio: 'pipe',
            });
            assert.equal(res.status, 0, 'building the fixture needs tar: ' + String(res.stderr || ''));

            tarball = fs.readFileSync(path.join(fixtureDir, 'trunc-pkg-1.0.0.tgz'));
            integrity = 'sha512-' + crypto.createHash('sha512').update(tarball).digest('base64');

            function head_ok(socket, length) {
                socket.write('HTTP/1.1 200 OK\r\n' +
                    'Content-Type: application/octet-stream\r\n' +
                    'Content-Length: ' + length + '\r\n' +
                    'Connection: close\r\n\r\n');
            }

            function serve(socket) {
                var head = '';

                for (;;) {
                    var chunk = socket.read();
                    if (!chunk || chunk.length === 0)
                        break;

                    head += chunk.toString();
                    if (head.indexOf('\r\n\r\n') > -1)
                        break;
                }

                var address = head.split(' ')[1] || '';

                if (address.indexOf('/trunc-pkg/-/') === 0) {
                    tarball_requests++;

                    if (tarball_requests === 1) {
                        // the whole tarball is announced, half of it is sent, and the
                        // connection goes away
                        head_ok(socket, tarball.length);
                        socket.write(tarball.slice(0, Math.floor(tarball.length / 2)));
                        socket.close();
                        return;
                    }

                    head_ok(socket, tarball.length);
                    socket.write(tarball);
                    socket.close();
                    return;
                }

                var metadata = JSON.stringify({
                    name: 'trunc-pkg',
                    'dist-tags': { latest: '1.0.0' },
                    versions: {
                        '1.0.0': {
                            name: 'trunc-pkg', version: '1.0.0',
                            dist: {
                                tarball: 'http://127.0.0.1:' + port + '/trunc-pkg/-/trunc-pkg-1.0.0.tgz',
                                integrity: integrity,
                            },
                        },
                    },
                });

                head_ok(socket, Buffer.byteLength(metadata));
                socket.write(metadata);
                socket.close();
            }

            for (var attempt = 0; attempt < 20 && !server; attempt++) {
                var candidate = 41000 + Math.floor(Math.random() * 4000);
                var candidateServer = net.createServer(serve);

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

        it('fetches a body that stopped early again, instead of hashing it', function () {
            var targetDir = makeTargetDir();

            fs.writeFileSync(path.join(targetDir, 'package.json'), JSON.stringify({
                name: 'trunc-proj', version: '1.0.0', dependencies: { 'trunc-pkg': '1.0.0' },
            }, null, 2));
            fs.writeFileSync(path.join(targetDir, '.npmrc'), 'registry=http://127.0.0.1:' + port + '/\n');

            tarball_requests = 0;

            var res = runInstaller(targetDir, ['--install'], {
                env: { HTTP_PROXY: '', HTTPS_PROXY: '', http_proxy: '', https_proxy: '', NO_PROXY: '*' },
            });
            var out = outputOf(res);

            assert.equal(res.status, 0, diag(targetDir, res) + '\n' + out);
            assert.ok(tarball_requests >= 2, 'the tarball was fetched again: ' + tarball_requests);
            assert.ok(out.indexOf('retry') > -1, 'the retry is reported:\n' + out);
            assert.ok(fs.existsSync(path.join(targetDir, 'node_modules/trunc-pkg/index.js')),
                'the package is installed with its files: ' + listDir(path.join(targetDir, 'node_modules')));
            assert.equal(JSON.parse(fs.readFileSync(
                path.join(targetDir, 'node_modules/trunc-pkg/package.json'), 'utf8')).version, '1.0.0');
        });
    });

    // ===== Phase 12: the flags a lockfile does not carry =====
    // a lockfile written before npm recorded `optional` describes a platform specific
    // binary as a package every machine needs: it was downloaded and installed on a
    // machine that cannot run it, and `npm ci` refuses the lock outright. The lockfile's
    // own edges answer the question, and what they prove is written down
    if (isFibjs) describe('install / lockfile flags (Phase 12)', function () {
        var http = require('http');
        var crypto = require('crypto');

        var server = null;
        var port = 0;
        var requests = [];
        var bin_tarball = null;
        var bin_integrity = '';
        var fixtureDir = '';

        // an operating system this machine is not: the fixture has to fail the
        // platform check wherever the suite runs
        var OTHER_OS = ['linux', 'darwin', 'win32', 'freebsd', 'openbsd', 'sunos', 'netbsd', 'aix', 'android']
            .filter(function (o) { return o !== process.platform; })[0];

        function registry_url(p) {
            return 'http://127.0.0.1:' + port + '/' + p;
        }

        before(function () {
            fixtureDir = path.join(TMP_DIR, 'flags_' + Date.now() + '_' + Math.random().toString(36).slice(2));

            var pkgDir = path.join(fixtureDir, 'package');
            fs.mkdirSync(pkgDir, { recursive: true });
            fs.writeFileSync(path.join(pkgDir, 'package.json'), JSON.stringify({
                name: 'bin-pkg', version: '1.0.0', bin: { binpkg: 'cli.js' },
            }));
            fs.writeFileSync(path.join(pkgDir, 'cli.js'), '#!/usr/bin/env fibjs\nconsole.log("bin-pkg");\n');

            var res = child_process.spawnSync('tar', ['czf', 'bin-pkg-1.0.0.tgz', 'package'], {
                cwd: fixtureDir, stdio: 'pipe',
            });
            assert.equal(res.status, 0, 'building the fixture needs tar: ' + String(res.stderr || ''));

            bin_tarball = fs.readFileSync(path.join(fixtureDir, 'bin-pkg-1.0.0.tgz'));
            bin_integrity = 'sha512-' + crypto.createHash('sha512').update(bin_tarball).digest('base64');

            function handler(r) {
                requests.push(r.address);

                if (r.address === '/bin-pkg/-/bin-pkg-1.0.0.tgz') {
                    r.response.write(bin_tarball);
                    return;
                }

                // a tarball of another platform: an install that asks for it is a
                // failed install, so the test does not have to count anything
                r.response.statusCode = 404;
                r.response.write('not found');
            }

            for (var attempt = 0; attempt < 20 && !server; attempt++) {
                var candidate = 41000 + Math.floor(Math.random() * 4000);
                var candidateServer = http.createServer(handler);

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

        function offline() {
            return { env: { HTTP_PROXY: '', HTTPS_PROXY: '', http_proxy: '', https_proxy: '', NO_PROXY: '*' } };
        }

        function lockEntry(dir, p) {
            return JSON.parse(fs.readFileSync(path.join(dir, 'package-lock.json'), 'utf8')).packages[p];
        }

        it('leaves a platform package of another system out when the lockfile never said optional', function () {
            var targetDir = makeTargetDir();

            fs.writeFileSync(path.join(targetDir, 'package.json'), JSON.stringify({
                name: 'flag-proj', version: '1.0.0',
                optionalDependencies: { 'foreign-pkg': '1.0.0' },
            }, null, 2));
            fs.writeFileSync(path.join(targetDir, 'package-lock.json'), JSON.stringify({
                name: 'flag-proj', version: '1.0.0', lockfileVersion: 3, requires: true,
                packages: {
                    '': {
                        name: 'flag-proj', version: '1.0.0',
                        optionalDependencies: { 'foreign-pkg': '1.0.0' },
                    },
                    'node_modules/foreign-pkg': {
                        version: '1.0.0',
                        resolved: registry_url('foreign-pkg/-/foreign-pkg-1.0.0.tgz'),
                        integrity: 'sha512-' + 'A'.repeat(86) + '==',
                        os: [OTHER_OS], cpu: [process.arch],
                    },
                },
            }, null, 2));

            var res = runInstaller(targetDir, ['--install'], offline());
            var out = outputOf(res);

            assert.equal(res.status, 0, diag(targetDir, res) + '\n' + out);
            assert.equal(requests.length, 0, 'nothing was fetched: ' + requests.join(', '));
            assert.ok(!fs.existsSync(path.join(targetDir, 'node_modules/foreign-pkg')),
                'the package of the other system is not on disk: ' + listDir(path.join(targetDir, 'node_modules')));

            // the lockfile's own edge says the entry is optional, and now the file says so
            assert.equal(lockEntry(targetDir, 'node_modules/foreign-pkg').optional, true,
                'the entry carries the flag npm needs: ' + JSON.stringify(lockEntry(targetDir, 'node_modules/foreign-pkg')));
            assert.equal(lockEntry(targetDir, 'node_modules/foreign-pkg').os[0], OTHER_OS, 'the platform is kept');

            // and the file that already says what the graph proves is not rewritten
            var text = fs.readFileSync(path.join(targetDir, 'package-lock.json'), 'utf8');
            var again = runInstaller(targetDir, ['--install'], offline());

            assert.equal(again.status, 0, diag(targetDir, again) + '\n' + outputOf(again));
            assert.equal(fs.readFileSync(path.join(targetDir, 'package-lock.json'), 'utf8'), text,
                'a second install leaves the lockfile alone');
        });

        it('leaves a dev dependency out on --omit=dev when the lockfile never said dev', function () {
            var targetDir = makeTargetDir();

            fs.writeFileSync(path.join(targetDir, 'package.json'), JSON.stringify({
                name: 'dev-proj', version: '1.0.0',
                devDependencies: { 'bin-pkg': '1.0.0' },
            }, null, 2));
            fs.writeFileSync(path.join(targetDir, 'package-lock.json'), JSON.stringify({
                name: 'dev-proj', version: '1.0.0', lockfileVersion: 3, requires: true,
                packages: {
                    '': {
                        name: 'dev-proj', version: '1.0.0',
                        devDependencies: { 'bin-pkg': '1.0.0' },
                    },
                    'node_modules/bin-pkg': {
                        version: '1.0.0',
                        resolved: registry_url('bin-pkg/-/bin-pkg-1.0.0.tgz'),
                        integrity: bin_integrity,
                        bin: { binpkg: 'cli.js' },
                    },
                },
            }, null, 2));

            var res = runInstaller(targetDir, ['--install', '--omit=dev'], offline());
            var out = outputOf(res);

            assert.equal(res.status, 0, diag(targetDir, res) + '\n' + out);
            assert.equal(requests.length, 0, 'nothing was fetched: ' + requests.join(', '));
            assert.ok(!fs.existsSync(path.join(targetDir, 'node_modules/bin-pkg')),
                'the dev dependency is not on disk: ' + listDir(path.join(targetDir, 'node_modules')));
            assert.equal(lockEntry(targetDir, 'node_modules/bin-pkg').dev, true,
                'the entry carries the flag npm needs: ' + JSON.stringify(lockEntry(targetDir, 'node_modules/bin-pkg')));
        });

        it('keeps a bin link that already points where it should, and replaces a file in its way', function () {
            var targetDir = makeTargetDir();

            fs.writeFileSync(path.join(targetDir, 'package.json'), JSON.stringify({
                name: 'bin-proj', version: '1.0.0', dependencies: { 'bin-pkg': '1.0.0' },
            }, null, 2));
            fs.writeFileSync(path.join(targetDir, 'package-lock.json'), JSON.stringify({
                name: 'bin-proj', version: '1.0.0', lockfileVersion: 3, requires: true,
                packages: {
                    '': { name: 'bin-proj', version: '1.0.0', dependencies: { 'bin-pkg': '1.0.0' } },
                    'node_modules/bin-pkg': {
                        version: '1.0.0',
                        resolved: registry_url('bin-pkg/-/bin-pkg-1.0.0.tgz'),
                        integrity: bin_integrity,
                        bin: { binpkg: 'cli.js' },
                    },
                },
            }, null, 2));

            var res = runInstaller(targetDir, ['--install'], offline());

            assert.equal(res.status, 0, diag(targetDir, res) + '\n' + outputOf(res));

            var link = path.join(targetDir, 'node_modules/.bin/binpkg');

            // npm links bins with a symlink on POSIX and with the
            // sh/.cmd/.ps1 shims on Windows, so the bin is a plain file there
            function assert_bin(msg) {
                if (isWin32) {
                    assert.ok(fs.existsSync(link) && fs.existsSync(link + '.cmd') && fs.existsSync(link + '.ps1'), msg);
                    assert.ok(!fs.lstatSync(link).isSymbolicLink(), msg);
                } else {
                    assert.ok(fs.lstatSync(link).isSymbolicLink(), msg);
                }
            }

            assert_bin('the bin is linked: ' + listDir(path.join(targetDir, 'node_modules/.bin')));

            // the package is gone, the link is not: installing it again meets the link
            // it wrote itself, which npm's bin-links leaves alone
            rmdirSync(path.join(targetDir, 'node_modules/bin-pkg'));

            var second = runInstaller(targetDir, ['--install'], offline());
            var out = outputOf(second);

            assert.equal(second.status, 0, diag(targetDir, second) + '\n' + out);
            assert.ok(out.indexOf('EEXIST') < 0 && out.indexOf('File exists') < 0,
                'linking what is already linked is not an error:\n' + out);
            assert_bin('the link is still there');

            // a plain file in the way is replaced, the way npm replaces it
            rmdirSync(path.join(targetDir, 'node_modules/bin-pkg'));
            fs.unlinkSync(link);
            fs.writeFileSync(link, 'a shim an older install wrote');

            var third = runInstaller(targetDir, ['--install'], offline());

            assert.equal(third.status, 0, diag(targetDir, third) + '\n' + outputOf(third));
            assert_bin('the file in the way is replaced: ' +
                listDir(path.join(targetDir, 'node_modules/.bin')));
            if (isWin32) {
                // the shim points at the package's cli file
                var shim = fs.readFileSync(link, 'utf8');
                assert.ok(shim.indexOf('cli.js') >= 0, 'the shim points at the cli: ' + shim);
            } else {
                assert.equal(fs.readlinkSync(link).replace(/\\/g, '/'), '../bin-pkg/cli.js');
            }
        });
    });

    // ===== Phase 14: a cycle through a peer edge =====
    // `browserslist` depends on `update-browserslist-db`, which declares
    // `browserslist` as a peer. Resolving that peer from the lockfile without asking
    // the ancestors first gave it a copy of its own — the copy declares the
    // dependency again, and the walk never ended: a project carrying such a pair died
    // with `RangeError: Maximum call stack size exceeded` before anything was
    // installed. The names are replaced, the shape is the one npm writes
    if (isFibjs) describe('install / dependency cycle through a peer edge (Phase 14)', function () {
        var http = require('http');
        var crypto = require('crypto');

        var server = null;
        var port = 0;
        var fixtureDir = '';
        var registry = {};   // name -> version -> { tarball, integrity, shasum, manifest }

        function pack(name, version, extra) {
            var dir = path.join(fixtureDir, name + '-' + version);
            var pkgDir = path.join(dir, 'package');

            var manifest = Object.assign({ name: name, version: version }, extra || {});

            fs.mkdirSync(pkgDir, { recursive: true });
            fs.writeFileSync(path.join(pkgDir, 'package.json'), JSON.stringify(manifest));
            fs.writeFileSync(path.join(pkgDir, 'index.js'), 'module.exports = ' + JSON.stringify(version) + ';\n');

            var file = name + '-' + version + '.tgz';

            var res = child_process.spawnSync('tar', ['czf', file, 'package'], { cwd: dir, stdio: 'pipe' });

            assert.equal(res.status, 0, 'building the fixture needs tar: ' + String(res.stderr || ''));

            var buf = fs.readFileSync(path.join(dir, file));

            return {
                tarball: buf,
                manifest: manifest,
                integrity: 'sha512-' + crypto.createHash('sha512').update(buf).digest('base64'),
                shasum: crypto.createHash('sha1').update(buf).digest('hex'),
            };
        }

        function tarball_url(name, version) {
            return 'http://127.0.0.1:' + port + '/' + name + '/-/' + name + '-' + version + '.tgz';
        }

        before(function () {
            fixtureDir = path.join(TMP_DIR, 'cyc_' + Date.now() + '_' + Math.random().toString(36).slice(2));
            fs.mkdirSync(fixtureDir, { recursive: true });

            // `cyc-a` depends on `cyc-b`, and `cyc-b` declares `cyc-a` as a peer whose
            // range the copy satisfies — every copy does, which is what keeps the
            // resolution quiet and the walk going
            registry['cyc-host'] = {
                '1.0.0': pack('cyc-host', '1.0.0', { dependencies: { 'cyc-a': '^1.0.0' } }),
            };
            registry['cyc-a'] = {
                '1.0.0': pack('cyc-a', '1.0.0', { dependencies: { 'cyc-b': '^1.0.0' } }),
            };
            registry['cyc-b'] = {
                '1.0.0': pack('cyc-b', '1.0.0', { peerDependencies: { 'cyc-a': '>= 1.0.0' } }),
            };
            registry['cyc-extra'] = {
                '1.0.0': pack('cyc-extra', '1.0.0'),
            };
            // what the lockfile already carries, so that resolving has one edge it
            // keeps and one it has to look up
            registry['cyc-need'] = {
                '1.0.0': pack('cyc-need', '1.0.0'),
            };

            function handler(r) {
                var tb = /^\/([^/]+)\/-\/([^/]+)\.tgz$/.exec(r.address);

                if (tb) {
                    var name = tb[1];
                    var version = tb[2].slice(name.length + 1);

                    if (registry[name] && registry[name][version]) {
                        r.response.write(registry[name][version].tarball);
                        return;
                    }
                } else {
                    var wanted = r.address.slice(1);

                    if (registry[wanted]) {
                        var versions = {};

                        Object.keys(registry[wanted]).forEach(function (v) {
                            var entry = registry[wanted][v];

                            versions[v] = Object.assign({}, entry.manifest, {
                                dist: {
                                    tarball: tarball_url(wanted, v),
                                    integrity: entry.integrity,
                                    shasum: entry.shasum,
                                },
                            });
                        });

                        r.response.write(JSON.stringify({
                            name: wanted,
                            'dist-tags': { latest: Object.keys(registry[wanted]).sort().pop() },
                            versions: versions,
                        }));
                        return;
                    }
                }

                r.response.statusCode = 404;
                r.response.write('not found');
            }

            for (var attempt = 0; attempt < 20 && !server; attempt++) {
                var candidate = 51000 + Math.floor(Math.random() * 4000);
                var candidateServer = http.createServer(handler);

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

        function code_of(name, spec, version) {
            return {
                version: version,
                resolved: tarball_url(name, version),
                integrity: registry[name][version].integrity,
                dependencies: spec && spec.dependencies,
                peerDependencies: spec && spec.peerDependencies,
            };
        }

        /**
         * @description the fresh clone: package.json declares one dependency the
         *              lockfile does not carry, so the installer resolves — and that
         *              resolution starts from the lockfile alone, which is where the
         *              cycle was walked. `hoisted` puts the pair where npm hoists it,
         *              the other shape keeps it inside `cyc-host`, with no copy at the
         *              root for the peer to find
         */
        function makeProject(hoisted) {
            var targetDir = makeTargetDir();
            var pairs = {};

            if (hoisted) {
                pairs['node_modules/cyc-a'] = code_of('cyc-a', registry['cyc-a']['1.0.0'].manifest, '1.0.0');
                pairs['node_modules/cyc-b'] = code_of('cyc-b', registry['cyc-b']['1.0.0'].manifest, '1.0.0');
            } else {
                pairs['node_modules/cyc-host/node_modules/cyc-a'] =
                    code_of('cyc-a', registry['cyc-a']['1.0.0'].manifest, '1.0.0');
                pairs['node_modules/cyc-host/node_modules/cyc-a/node_modules/cyc-b'] =
                    code_of('cyc-b', registry['cyc-b']['1.0.0'].manifest, '1.0.0');
            }

            fs.writeFileSync(path.join(targetDir, 'package.json'), JSON.stringify({
                name: 'cyc-proj', version: '1.0.0',
                // `cyc-extra` is not in the lockfile: the tree is resolved again
                dependencies: { 'cyc-host': '1.0.0', 'cyc-need': '1.0.0', 'cyc-extra': '1.0.0' },
            }, null, 2));
            fs.writeFileSync(path.join(targetDir, '.npmrc'), 'registry=http://127.0.0.1:' + port + '/\n');

            var packages = {
                '': {
                    name: 'cyc-proj', version: '1.0.0',
                    dependencies: { 'cyc-host': '1.0.0', 'cyc-need': '1.0.0' },
                },
                'node_modules/cyc-host': code_of('cyc-host', registry['cyc-host']['1.0.0'].manifest, '1.0.0'),
                'node_modules/cyc-need': code_of('cyc-need', null, '1.0.0'),
            };

            Object.keys(pairs).forEach(function (p) { packages[p] = pairs[p]; });

            fs.writeFileSync(path.join(targetDir, 'package-lock.json'), JSON.stringify({
                name: 'cyc-proj', version: '1.0.0', lockfileVersion: 3, requires: true,
                packages: packages,
            }, null, 2));

            return targetDir;
        }

        function runCyc(targetDir, args) {
            return runInstaller(targetDir, args || ['--install'], {
                env: { HTTP_PROXY: '', HTTPS_PROXY: '', http_proxy: '', https_proxy: '', NO_PROXY: '*' },
            });
        }

        function entriesEndingWith(targetDir, suffix) {
            var lock = JSON.parse(fs.readFileSync(path.join(targetDir, 'package-lock.json'), 'utf8'));

            return Object.keys(lock.packages).filter(function (p) { return p.slice(-suffix.length) === suffix; });
        }

        it('resolves a pair the disk does not carry, instead of nesting it forever', function () {
            var targetDir = makeProject(true);
            var res = runCyc(targetDir);
            var out = outputOf(res);

            assert.equal(res.status, 0, diag(targetDir, res) + '\n' + out);
            assert.ok(out.indexOf('Maximum call stack size exceeded') === -1,
                'the walk does not recurse without end:\n' + out);

            // one copy of each: the peer is served by the copy that is already there
            assert.deepEqual(entriesEndingWith(targetDir, '/cyc-a'), ['node_modules/cyc-a'],
                'cyc-a is resolved once: ' + diag(targetDir, res));
            assert.deepEqual(entriesEndingWith(targetDir, '/cyc-b'), ['node_modules/cyc-b'],
                'and so is cyc-b, whose peer edge points back at it');
            assert.ok(fs.existsSync(path.join(targetDir, 'node_modules', 'cyc-a', 'index.js')),
                'the pair is on the disk: ' + listDir(path.join(targetDir, 'node_modules')));
        });

        it('resolves a pair that is nested all the way down, where no copy is at the root', function () {
            // nothing at the root for the peer to resolve to: the copy it is served by
            // is the one the walk created inside `cyc-host`, one level up
            var targetDir = makeProject(false);
            var res = runCyc(targetDir);
            var out = outputOf(res);

            assert.equal(res.status, 0, diag(targetDir, res) + '\n' + out);
            assert.ok(out.indexOf('Maximum call stack size exceeded') === -1,
                'the walk does not recurse without end:\n' + out);

            assert.deepEqual(entriesEndingWith(targetDir, '/cyc-a'), ['node_modules/cyc-a'],
                'cyc-a is resolved once, wherever hoisting put it: ' + diag(targetDir, res));
            assert.deepEqual(entriesEndingWith(targetDir, '/cyc-b'), ['node_modules/cyc-b'],
                'and so is cyc-b');
        });
    });

    // ===== Phase 13: what a tarball carries, and what npm refuses =====
    // two things an install has to get right at the end: a package that bundles its
    // own copy of a dependency must not leave that older copy where the install
    // resolved another one (the tree the lockfile describes is the tree on disk), and
    // a peer edge that cannot be met is npm's ERESOLVE — `npm ci` refuses the lockfile
    // of one even when it was written with `--legacy-peer-deps`
    if (isFibjs) describe('install / bundles and peers (Phase 13)', function () {
        var http = require('http');
        var crypto = require('crypto');

        var server = null;
        var port = 0;
        var requests = [];
        var tarballs = {};
        var packuments = {};
        var fixtureDir = '';

        function registry_url(p) {
            return 'http://127.0.0.1:' + port + '/' + p;
        }

        function tarball_of(name, version) {
            return tarballs[name + '-' + version];
        }

        /**
         * @description build `<name>@<version>`: the manifest, the files, and what the
         *              registry answers. `bundled` are the copies the tarball carries
         *              itself (npm's `bundleDependencies`), `blob` a file big enough
         *              that unpacking the tarball takes longer than fetching what goes
         *              inside it (the race the waves rule out)
         */
        function build_pkg(name, version, manifest, opts) {
            var o = opts || {};
            var bundled = o.bundled || {};
            // npm's registry packs most packages under `package/`, and `@types/*` under
            // the package's own name: the unpacker has to use each tarball's own root
            var root = o.root || 'package';
            var dir = path.join(fixtureDir, name + '-' + version);
            var pkg = path.join(dir, root);

            fs.mkdirSync(pkg, { recursive: true });
            fs.writeFileSync(path.join(pkg, 'package.json'), JSON.stringify(manifest, null, 2));
            fs.writeFileSync(path.join(pkg, 'index.js'), 'module.exports = "' + name + '@' + version + '";\n');

            if (o.blob)
                fs.writeFileSync(path.join(pkg, 'blob.bin'), crypto.randomBytes(o.blob));

            (o.files || []).forEach(f => {
                fs.writeFileSync(path.join(pkg, f + '.txt'), name + ' ' + f + '\n');
            });

            Object.keys(bundled).forEach(bundled_name => {
                var src = path.join(fixtureDir, bundled_name + '-' + bundled[bundled_name], 'package');
                var dst = path.join(pkg, 'node_modules', bundled_name);

                fs.mkdirSync(dst, { recursive: true });
                copyDir(src, dst);
            });

            var file = name + '-' + version + '.tgz';
            var res = child_process.spawnSync('tar', ['czf', file, root], { cwd: dir, stdio: 'pipe' });

            assert.equal(res.status, 0, 'building the fixture needs tar: ' + String(res.stderr || ''));

            var buffer = fs.readFileSync(path.join(dir, file));

            tarballs[name + '-' + version] = {
                buffer: buffer,
                integrity: 'sha512-' + crypto.createHash('sha512').update(buffer).digest('base64'),
            };

            var versions = packuments[name] || (packuments[name] = {
                name: name, 'dist-tags': { latest: version }, versions: {},
            });

            // a registry answers with the manifest, dependencies and all
            var entry = JSON.parse(JSON.stringify(manifest));

            entry.dist = {};
            // the port is not known until the server is bound: the URLs are filled
            // in below
            entry.dist.tarball = registry_url(name + '/-/' + file);
            entry.dist.integrity = tarballs[name + '-' + version].integrity;
            versions.versions[version] = entry;
            versions['dist-tags'].latest = version;
        }

        before(function () {
            fixtureDir = path.join(TMP_DIR, 'bundle_' + Date.now() + '_' + Math.random().toString(36).slice(2));

            build_pkg('helper', '1.0.0', { name: 'helper', version: '1.0.0' });
            build_pkg('helper', '1.1.0', { name: 'helper', version: '1.1.0' });
            build_pkg('helper', '2.0.0', { name: 'helper', version: '2.0.0' });

            // npm's `bundleDependencies`: the tarball carries its own copy of `helper`,
            // and it is big enough that unpacking it takes longer than fetching the
            // copy that belongs inside it
            build_pkg('bundler-pkg', '1.0.0', {
                name: 'bundler-pkg', version: '1.0.0',
                dependencies: { helper: '^1.0.0' },
                bundleDependencies: ['helper'],
            }, { bundled: { helper: '1.0.0' }, blob: 12 * 1024 * 1024 });

            build_pkg('core', '1.0.0', { name: 'core', version: '1.0.0' });
            build_pkg('core', '2.0.0', { name: 'core', version: '2.0.0' });
            build_pkg('peer-host', '1.0.0', {
                name: 'peer-host', version: '1.0.0',
                peerDependencies: { core: '^2.0.0' },
            });

            function handler(r) {
                requests.push(r.address);

                var m = /^\/([^\/]+)\/-\/([^\/]+)-(\d+\.\d+\.\d+)\.tgz$/.exec(r.address);

                if (m) {
                    var t = tarball_of(m[2], m[3]);

                    if (t) {
                        r.response.write(t.buffer);
                        return;
                    }
                }

                var packument = packuments[r.address.replace(/^\//, '')];

                if (packument) {
                    // the fixture is built before the port is bound, so the tarball
                    // URLs are answered with it here (`127.0.0.1:0` is the placeholder)
                    r.response.write(JSON.stringify(packument)
                        .replace(/127\.0\.0\.1:0\//g, '127.0.0.1:' + port + '/'));
                    return;
                }

                r.response.statusCode = 404;
                r.response.write('not found');
            }

            for (var attempt = 0; attempt < 20 && !server; attempt++) {
                var candidate = 41000 + Math.floor(Math.random() * 4000);
                var candidateServer = http.createServer(handler);

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

        function offline() {
            return { env: { HTTP_PROXY: '', HTTPS_PROXY: '', http_proxy: '', https_proxy: '', NO_PROXY: '*' } };
        }

        function versionOnDisk(dir, rel) {
            var p = path.join(dir, rel, 'package.json');

            if (!fs.existsSync(p))
                return null;

            return JSON.parse(fs.readFileSync(p, 'utf8')).version;
        }

        it('keeps the copy a tarball ships, and describes it the way npm does', function () {
            var targetDir = makeTargetDir();

            // the root pins `helper@2`, so `bundler-pkg`'s `^1.0.0` cannot be hoisted
            // away: the install resolves 1.1.0 into the directory the tarball also
            // carries a 1.0.0 in
            fs.writeFileSync(path.join(targetDir, 'package.json'), JSON.stringify({
                name: 'bundle-proj', version: '1.0.0',
                dependencies: { 'bundler-pkg': '1.0.0', helper: '2.0.0' },
            }, null, 2));
            fs.writeFileSync(path.join(targetDir, '.npmrc'), 'registry=http://127.0.0.1:' + port + '/\n');

            var res = runInstaller(targetDir, ['--install'], offline());
            var out = outputOf(res);

            assert.equal(res.status, 0, diag(targetDir, res) + '\n' + out);

            var nested = 'node_modules/bundler-pkg/node_modules/helper';

            // npm keeps the copy the tarball carries (`bundleDependencies`) and never
            // resolves the dependency: 1.1.0 is not fetched at all
            assert.equal(versionOnDisk(targetDir, nested), '1.0.0',
                'the copy the tarball ships is the one on disk: ' +
                listDir(path.join(targetDir, 'node_modules/bundler-pkg/node_modules')));
            assert.ok(!requests.some(u => u.indexOf('helper-1.1.0') > -1),
                'the dependency it bundles was not fetched: ' + requests.join(', '));

            // and the lockfile it wrote describes that tree, the way npm does
            var lock = JSON.parse(fs.readFileSync(path.join(targetDir, 'package-lock.json'), 'utf8'));

            assert.equal(lock.packages[nested].version, '1.0.0', 'the lockfile describes the copy on disk: ' +
                JSON.stringify(lock.packages[nested]));
            assert.equal(lock.packages[nested].inBundle, true, 'with `inBundle`, the way npm records it: ' +
                JSON.stringify(lock.packages[nested]));
            assert.ok(!lock.packages[nested].resolved, 'and no source of its own: ' +
                JSON.stringify(lock.packages[nested]));
            assert.deepEqual(lock.packages['node_modules/bundler-pkg'].bundleDependencies, ['helper'],
                'the package it came inside says so: ' +
                JSON.stringify(lock.packages['node_modules/bundler-pkg']));

            var disagreeing = [];

            Object.keys(lock.packages).forEach(p => {
                var entry = lock.packages[p];

                if (p === '' || entry.link || p.indexOf('node_modules/') !== 0)
                    return;

                var on_disk = versionOnDisk(targetDir, p);

                if (on_disk !== entry.version)
                    disagreeing.push(p + ': ' + entry.version + ' vs ' + on_disk);
            });

            assert.equal(disagreeing.length, 0, 'the tree and the lockfile agree: ' + disagreeing.join(', '));
        });

        /**
         * A copy a tarball ships is reached through nothing but its parent: the
         * project never names it, no other package asks for it, and the resolver
         * walked past it on purpose. So the only thing that can say it belongs on
         * the disk — and in the lockfile — is the parent's `bundleDependencies`,
         * which is what npm reads too (`Node#getBundler`).
         *
         * Getting this wrong is not a cosmetic lockfile difference: the prune
         * deletes the copies out of the tarball's own `node_modules`, and the
         * lockfile it writes then declares a parent whose dependencies the file
         * does not contain — so the project's own next `--ci` refuses a file its
         * own `--install` just wrote (`npm@2` is the real-world case: 71 names).
         */
        it('keeps a bundled dependency the project never names', function () {
            var targetDir = makeTargetDir();

            fs.writeFileSync(path.join(targetDir, 'package.json'), JSON.stringify({
                name: 'bundle-only-proj', version: '1.0.0',
                dependencies: { 'bundler-pkg': '1.0.0' },
            }, null, 2));
            fs.writeFileSync(path.join(targetDir, '.npmrc'), 'registry=http://127.0.0.1:' + port + '/\n');

            var res = runInstaller(targetDir, ['--install'], offline());
            var out = outputOf(res);

            assert.equal(res.status, 0, diag(targetDir, res) + '\n' + out);

            var nested = 'node_modules/bundler-pkg/node_modules/helper';

            assert.equal(versionOnDisk(targetDir, nested), '1.0.0',
                'the copy the tarball ships stays on the disk: ' +
                listDir(path.join(targetDir, 'node_modules/bundler-pkg/node_modules')));

            var lock = JSON.parse(fs.readFileSync(path.join(targetDir, 'package-lock.json'), 'utf8'));

            assert.ok(lock.packages[nested], 'and the lockfile describes it: ' +
                JSON.stringify(Object.keys(lock.packages)));
            assert.equal(lock.packages[nested].version, '1.0.0', JSON.stringify(lock.packages[nested]));
            assert.equal(lock.packages[nested].inBundle, true, 'as bundled, the way npm records it: ' +
                JSON.stringify(lock.packages[nested]));

            // the file this install wrote has to be one this install accepts
            var ci = runInstaller(targetDir, ['--install', '--ci', '--dry-run'], offline());

            assert.equal(ci.status, 0, 'the lockfile it wrote is in sync with itself: ' +
                diag(targetDir, ci) + '\n' + outputOf(ci));

            // and a second install has nothing left to do: the copy is not debris to
            // be pruned, so the tree it finds is the tree it describes
            var again = runInstaller(targetDir, ['--install'], offline());
            var again_out = outputOf(again);

            assert.equal(again.status, 0, diag(targetDir, again) + '\n' + again_out);
            assert.equal(versionOnDisk(targetDir, nested), '1.0.0',
                'the second install kept it: ' +
                listDir(path.join(targetDir, 'node_modules/bundler-pkg/node_modules')));
            assert.ok(again_out.indexOf('prune:') < 0, 'nothing was pruned:\n' + again_out);

            assert.deepEqual(
                JSON.parse(fs.readFileSync(path.join(targetDir, 'package-lock.json'), 'utf8')).packages,
                lock.packages, 'and the lockfile is unchanged');
        });

        it('installs a lockfile whose bundled entries carry no source', function () {
            var targetDir = makeTargetDir();

            fs.writeFileSync(path.join(targetDir, 'package.json'), JSON.stringify({
                name: 'bundle-ci-proj', version: '1.0.0',
                dependencies: { 'bundler-pkg': '1.0.0' },
            }, null, 2));
            // npm's own shape: the copy inside the tarball has no `resolved` and no
            // `integrity`, and a frozen install has to let it arrive with its parent
            // instead of asking the registry for it
            fs.writeFileSync(path.join(targetDir, 'package-lock.json'), JSON.stringify({
                name: 'bundle-ci-proj', version: '1.0.0', lockfileVersion: 3, requires: true,
                packages: {
                    '': { name: 'bundle-ci-proj', version: '1.0.0', dependencies: { 'bundler-pkg': '1.0.0' } },
                    'node_modules/bundler-pkg': {
                        version: '1.0.0',
                        resolved: registry_url('bundler-pkg/-/bundler-pkg-1.0.0.tgz'),
                        integrity: tarball_of('bundler-pkg', '1.0.0').integrity,
                        dependencies: { helper: '^1.0.0' },
                        bundleDependencies: ['helper'],
                    },
                    'node_modules/bundler-pkg/node_modules/helper': {
                        version: '1.0.0', inBundle: true,
                    },
                },
            }, null, 2));

            var before = fs.readFileSync(path.join(targetDir, 'package-lock.json'), 'utf8');
            var res = runInstaller(targetDir, ['--install', '--ci'], offline());
            var out = outputOf(res);

            assert.equal(res.status, 0, diag(targetDir, res) + '\n' + out);
            assert.ok(out.indexOf('1 shipped inside a parent') > -1, 'the copy is not fetched:\n' + out);
            assert.equal(versionOnDisk(targetDir, 'node_modules/bundler-pkg/node_modules/helper'), '1.0.0',
                'it is there, out of the tarball: ' +
                listDir(path.join(targetDir, 'node_modules/bundler-pkg/node_modules')));
            assert.equal(fs.readFileSync(path.join(targetDir, 'package-lock.json'), 'utf8'), before,
                'a frozen install does not rewrite the lockfile');
        });

        it('heals a path of the other kind an earlier install left behind', function () {
            var targetDir = makeTargetDir();

            fs.writeFileSync(path.join(targetDir, 'package.json'), JSON.stringify({
                name: 'heal-proj', version: '1.0.0',
                dependencies: { core: '1.0.0', helper: '1.0.0' },
            }, null, 2));
            fs.writeFileSync(path.join(targetDir, '.npmrc'), 'registry=http://127.0.0.1:' + port + '/\n');

            // what an install that stopped half way through leaves behind: a *file*
            // where a package's directory has to go, and a *directory* where one of its
            // files has to go. npm's tar clears both, and an install over such a tree
            // has to put it right instead of failing on it
            fs.mkdirSync(path.join(targetDir, 'node_modules/core/index.js'), { recursive: true });
            fs.writeFileSync(path.join(targetDir, 'node_modules/helper'), 'a leftover file\n');

            var res = runInstaller(targetDir, ['--install'], offline());
            var out = outputOf(res);

            assert.equal(res.status, 0, diag(targetDir, res) + '\n' + out);
            assert.equal(versionOnDisk(targetDir, 'node_modules/core'), '1.0.0', listDir(path.join(targetDir, 'node_modules/core')));
            assert.ok(!fs.lstatSync(path.join(targetDir, 'node_modules/core/index.js')).isDirectory(),
                'the directory in the way of the file member is gone: ' + listDir(path.join(targetDir, 'node_modules/core')));
            assert.equal(versionOnDisk(targetDir, 'node_modules/helper'), '1.0.0', listDir(path.join(targetDir, 'node_modules')));
        });

        it('unpacks tarballs with different member roots at the same time', function () {
            var targetDir = makeTargetDir();
            var names = [];
            var members = ['a', 'b', 'c'];

            // one wave unpacks several tarballs at once, and each tarball's root is
            // that task's own business: sharing it put one package's members inside
            // another's directory
            for (var i = 0; i < 8; i++) {
                var name = 'root-pkg-' + i;

                build_pkg(name, '1.0.0', {
                    name: name, version: '1.0.0',
                }, { root: i % 2 === 0 ? 'package' : name, files: members });

                names.push(name);
            }

            var deps = {};

            names.forEach(n => { deps[n] = '1.0.0'; });

            fs.writeFileSync(path.join(targetDir, 'package.json'), JSON.stringify({
                name: 'roots-proj', version: '1.0.0', dependencies: deps,
            }, null, 2));
            fs.writeFileSync(path.join(targetDir, '.npmrc'), 'registry=http://127.0.0.1:' + port + '/\n');

            var res = runInstaller(targetDir, ['--install'], offline());
            var out = outputOf(res);

            assert.equal(res.status, 0, diag(targetDir, res) + '\n' + out);

            names.forEach(n => {
                var dir = path.join(targetDir, 'node_modules', n);
                var wrong = fs.readdirSync(dir).filter(f => members.every(m => f !== m + '.txt') &&
                    f !== 'package.json' && f !== 'index.js');

                assert.equal(versionOnDisk(targetDir, 'node_modules/' + n), '1.0.0',
                    'the package is where it belongs: ' + listDir(dir));
                members.forEach(m => {
                    assert.ok(fs.existsSync(path.join(dir, m + '.txt')),
                        n + ' has ' + m + '.txt: ' + listDir(dir));
                });
                assert.equal(wrong.length, 0, 'nothing else landed in ' + n + ': ' + wrong.join(', '));
            });
        });

        it('refuses a lockfile whose peer cannot be met, the way npm ci does', function () {
            var targetDir = makeTargetDir();

            fs.writeFileSync(path.join(targetDir, 'package.json'), JSON.stringify({
                name: 'peer-proj', version: '1.0.0',
                dependencies: { 'peer-host': '1.0.0', core: '1.0.0' },
            }, null, 2));
            fs.writeFileSync(path.join(targetDir, 'package-lock.json'), JSON.stringify({
                name: 'peer-proj', version: '1.0.0', lockfileVersion: 3, requires: true,
                packages: {
                    '': {
                        name: 'peer-proj', version: '1.0.0',
                        dependencies: { 'peer-host': '1.0.0', core: '1.0.0' },
                    },
                    'node_modules/peer-host': {
                        version: '1.0.0',
                        resolved: registry_url('peer-host/-/peer-host-1.0.0.tgz'),
                        integrity: tarball_of('peer-host', '1.0.0').integrity,
                        peerDependencies: { core: '^2.0.0' },
                    },
                    // the root holds 1.0.0 where the peer asks for ^2.0.0: npm answers
                    // ERESOLVE and refuses to install the tree
                    'node_modules/core': {
                        version: '1.0.0',
                        resolved: registry_url('core/-/core-1.0.0.tgz'),
                        integrity: tarball_of('core', '1.0.0').integrity,
                    },
                },
            }, null, 2));

            var strict = runInstaller(targetDir, ['--install', '--ci', '--dry-run'], offline());
            var out = outputOf(strict);

            assert.equal(strict.status, 1, 'the frozen install refuses it: ' + diag(targetDir, strict) + '\n' + out);
            assert.ok(out.indexOf('cannot be met') > -1, 'and says why:\n' + out);
            assert.ok(out.indexOf('ERESOLVE') > -1, 'in npm\'s words:\n' + out);
            assert.ok(out.indexOf('--no-strict-peer') > -1, 'and how to accept it:\n' + out);

            // npm is told to install it anyway with `--legacy-peer-deps`
            var relaxed = runInstaller(targetDir, ['--install', '--ci', '--dry-run', '--no-strict-peer'], offline());

            assert.equal(relaxed.status, 0, diag(targetDir, relaxed) + '\n' + outputOf(relaxed));

            // an install that resolves says the same thing without stopping
            var install = runInstaller(targetDir, ['--install'], offline());
            var install_out = outputOf(install);

            assert.equal(install.status, 0, diag(targetDir, install) + '\n' + install_out);
            assert.ok(install_out.indexOf('peer core@^2.0.0 of node_modules/peer-host') > -1,
                'the unmet peer is named:\n' + install_out);
            assert.equal(versionOnDisk(targetDir, 'node_modules/peer-host'), '1.0.0');
        });

        it('says nothing about a peer the tree already answers', function () {
            var satisfiedDir = makeTargetDir();

            // the peer asks for `^2.0.0` and the root holds 2.0.0: npm's ordinary
            // hoisted placement, and an install has nothing to report about it
            fs.writeFileSync(path.join(satisfiedDir, 'package.json'), JSON.stringify({
                name: 'peer-quiet-proj', version: '1.0.0',
                dependencies: { 'peer-host': '1.0.0', core: '2.0.0' },
            }, null, 2));
            fs.writeFileSync(path.join(satisfiedDir, '.npmrc'), 'registry=http://127.0.0.1:' + port + '/\n');

            var quiet = runInstaller(satisfiedDir, ['--install'], offline());
            var quiet_out = outputOf(quiet);

            assert.equal(quiet.status, 0, diag(satisfiedDir, quiet) + '\n' + quiet_out);
            assert.ok(quiet_out.indexOf('is not satisfied by the installed') === -1,
                'a peer the copy above answers is not a conflict:\n' + quiet_out);
            assert.equal(versionOnDisk(satisfiedDir, 'node_modules/core'), '2.0.0');
            assert.ok(!fs.existsSync(path.join(satisfiedDir, 'node_modules/peer-host/node_modules/core')),
                'and the copy above it is the one it uses: ' +
                listDir(path.join(satisfiedDir, 'node_modules/peer-host')));

            // the same peer with a root copy that does not answer it is the conflict
            // npm warns about, and that warning stays
            var conflictDir = makeTargetDir();

            fs.writeFileSync(path.join(conflictDir, 'package.json'), JSON.stringify({
                name: 'peer-conflict-proj', version: '1.0.0',
                dependencies: { 'peer-host': '1.0.0', core: '1.0.0' },
            }, null, 2));
            fs.writeFileSync(path.join(conflictDir, '.npmrc'), 'registry=http://127.0.0.1:' + port + '/\n');

            // the same peer with a root copy that does not answer it is npm's
            // `ERESOLVE`: `peer-host` needs `core@^2.0.0` and the project pinned
            // `1.0.0`, so the tree installs and then fails at run time. npm refuses
            // it, and an installer that writes the lockfile anyway produces one
            // `npm ci` rejects — so this is a failure, not a warning
            var conflict = runInstaller(conflictDir, ['--install'], offline());
            var conflict_out = outputOf(conflict);

            assert.notEqual(conflict.status, 0,
                'a peer the copy above does not answer is npm\'s ERESOLVE:\n' + conflict_out);
            assert.ok(conflict_out.indexOf('core') > -1 &&
                conflict_out.indexOf('^2.0.0') > -1,
                'the conflict names the edge:\n' + conflict_out);

            // and the two flags npm offers are the way through it
            var forced = runInstaller(conflictDir, ['--install', '--legacy-peer-deps'], offline());

            assert.equal(forced.status, 0, diag(conflictDir, forced));
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

            var file = 'fake-pkg-1.0.0.tgz';

            // `tar` reads `D:\...` as a remote host (`host:path`), so the archive
            // name is relative and the directory is the working one: no argument
            // carries a colon on Windows
            var res = child_process.spawnSync('tar', ['czf', file, 'package'], { cwd: fixtureDir, stdio: 'pipe' });
            assert.equal(res.status, 0, 'building the fixture needs tar: ' + String(res.stderr || ''));

            tarball = fs.readFileSync(path.join(fixtureDir, file));
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
            // Node-aligned: the OpenSSL reason is the message and the
            // X509_V_ERR_* enumeration name is the code.
            assert.ok(outputOf(res).indexOf('self-signed certificate') > -1, outputOf(res));
            assert.ok(outputOf(res).indexOf('DEPTH_ZERO_SELF_SIGNED_CERT') > -1, outputOf(res));
        });
    });

    // ===== Phase 9: day two — adding a dependency, and updating one =====
    // A lockfile changes what these two everyday operations do, so both are checked
    // against a local registry that has two versions of everything: what resolving
    // picks, what the lockfile records and what lands on the disk can all be read off
    // without leaving the machine
    if (isFibjs) describe('day two: adding and updating (Phase 9)', function () {
        var crypto = require('crypto');
        var http = require('http');
        var tls = require('tls');

        var server = null;
        var port = 0;
        var fixtureDir = '';
        var registry = {};   // name -> version -> { tarball, integrity, shasum, manifest }

        /**
         * @description a package packed the way npm packs one: `package/…` at the
         *              root of the tarball
         */
        function pack(name, version, extra) {
            var dir = path.join(fixtureDir, name + '-' + version);
            var pkgDir = path.join(dir, 'package');

            var manifest = Object.assign({ name: name, version: version }, extra || {});

            fs.mkdirSync(pkgDir, { recursive: true });
            fs.writeFileSync(path.join(pkgDir, 'package.json'), JSON.stringify(manifest));
            fs.writeFileSync(path.join(pkgDir, 'index.js'), 'module.exports = ' + JSON.stringify(version) + ';\n');

            var file = name + '-' + version + '.tgz';

            // the same as the .npmrc suite: a drive letter in an argument makes
            // `tar` dial it as a remote host, so the directory is the working one
            var res = child_process.spawnSync('tar', ['czf', file, 'package'], { cwd: dir, stdio: 'pipe' });

            assert.equal(res.status, 0, 'building the fixture needs tar: ' + String(res.stderr || ''));

            var buf = fs.readFileSync(path.join(dir, file));

            return {
                tarball: buf,
                manifest: manifest,
                integrity: 'sha512-' + crypto.createHash('sha512').update(buf).digest('base64'),
                shasum: crypto.createHash('sha1').update(buf).digest('hex'),
            };
        }

        function tarball_url(name, version) {
            return 'https://localhost:' + port + '/' + name + '/-/' + name + '-' + version + '.tgz';
        }

        before(function () {
            fixtureDir = path.join(TMP_DIR, 'day2_' + Date.now() + '_' + Math.random().toString(36).slice(2));
            fs.mkdirSync(fixtureDir, { recursive: true });

            // a package with a dependency, so a refresh has a subtree to carry along
            registry['day2-pkg'] = {
                '1.0.0': pack('day2-pkg', '1.0.0', { dependencies: { 'day2-dep': '^1.0.0' } }),
                '1.0.1': pack('day2-pkg', '1.0.1', { dependencies: { 'day2-dep': '^1.0.0' } }),
            };
            registry['day2-dep'] = {
                '1.0.0': pack('day2-dep', '1.0.0'),
                '1.0.1': pack('day2-dep', '1.0.1'),
            };
            registry['day2-extra'] = {
                '1.0.0': pack('day2-extra', '1.0.0'),
                '1.0.1': pack('day2-extra', '1.0.1'),
            };

            // a package that ships a binary for every platform as an optional
            // dependency, the way esbuild and sharp do: only the entry that matches
            // this machine is installed, and a lockfile describes them all
            registry['plat-host'] = {
                '1.0.0': pack('plat-host', '1.0.0', { optionalDependencies: { 'plat-bin': '1.0.0' } }),
            };
            registry['plat-bin'] = {
                '1.0.0': pack('plat-bin', '1.0.0', { os: ['aix'], cpu: ['ppc64'] }),
            };

            var keys = crypto.generateKeyPairSync('rsa', { modulusLength: 2048 });
            var subject = { C: 'CN', O: 'fibjs', CN: 'localhost' };
            var certPem = crypto.createCertificateRequest({ key: keys.privateKey, subject: subject })
                .issue({
                    key: keys.privateKey, issuer: subject,
                    notBefore: new Date(), notAfter: new Date(Date.now() + 86400000),
                }).pem;
            var keyPem = keys.privateKey.export({ format: 'pem' });

            function handler(r) {
                var tb = /^\/([^/]+)\/-\/([^/]+)\.tgz$/.exec(r.address);

                if (tb) {
                    var name = tb[1];
                    var version = tb[2].slice(name.length + 1);

                    if (registry[name] && registry[name][version]) {
                        r.response.write(registry[name][version].tarball);
                        return;
                    }
                } else {
                    var wanted = r.address.slice(1);

                    if (registry[wanted]) {
                        var versions = {};

                        Object.keys(registry[wanted]).forEach(function (v) {
                            var entry = registry[wanted][v];

                            versions[v] = Object.assign({}, entry.manifest, {
                                dist: {
                                    tarball: tarball_url(wanted, v),
                                    integrity: entry.integrity,
                                    shasum: entry.shasum,
                                },
                            });
                        });

                        r.response.write(JSON.stringify({
                            name: wanted,
                            'dist-tags': { latest: Object.keys(registry[wanted]).sort().pop() },
                            versions: versions,
                        }));
                        return;
                    }
                }

                r.response.statusCode = 404;
                r.response.write('not found');
            }

            var context = tls.createSecureContext({ cert: certPem, key: keyPem, requestCert: false });

            for (var attempt = 0; attempt < 20 && !server; attempt++) {
                var candidate = 45000 + Math.floor(Math.random() * 4000);
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

        function writePkgjson(targetDir, deps) {
            fs.writeFileSync(path.join(targetDir, 'package.json'), JSON.stringify({
                name: 'day2-proj', version: '1.0.0', dependencies: deps,
            }, null, 2));
        }

        /**
         * @description a project that depends on the local registry only. The proxy
         *              variables are cleared: this is a loopback connection
         */
        function makeProject(deps) {
            var targetDir = makeTargetDir();

            writePkgjson(targetDir, deps);
            fs.writeFileSync(path.join(targetDir, '.npmrc'),
                'registry=https://localhost:' + port + '/\nstrict-ssl=false\n');

            return targetDir;
        }

        function runDay2(targetDir, args) {
            return runInstaller(targetDir, args || ['--install'], {
                env: { HTTP_PROXY: '', HTTPS_PROXY: '', http_proxy: '', https_proxy: '', NO_PROXY: '*' },
            });
        }

        function lockOf(targetDir) {
            return JSON.parse(fs.readFileSync(path.join(targetDir, 'package-lock.json'), 'utf8'));
        }

        function locked(targetDir, name) {
            var entry = lockOf(targetDir).packages['node_modules/' + name];

            return entry ? entry.version : null;
        }

        function installed(targetDir, name) {
            var file = path.join(targetDir, 'node_modules', name, 'package.json');

            return fs.existsSync(file) ? JSON.parse(fs.readFileSync(file, 'utf8')).version : null;
        }

        it('records the spec a package declares, not the version it resolved to', function () {
            var targetDir = makeProject({ 'day2-pkg': '^1.0.0' });
            var res = runDay2(targetDir);

            assert.equal(res.status, 0, diag(targetDir, res));

            var entry = lockOf(targetDir).packages['node_modules/day2-pkg'];

            assert.equal(entry.version, '1.0.1', 'the newest the range allows is what is installed');
            assert.equal(entry.dependencies['day2-dep'], '^1.0.0',
                'npm records the manifest spec here, and an exact version would pin every ' +
                'later reader of the lockfile: ' + JSON.stringify(entry.dependencies));
        });

        it('keeps the version the lockfile pins when package.json gains a dependency', function () {
            var targetDir = makeProject({ 'day2-pkg': '^1.0.0' });

            assert.equal(runDay2(targetDir).status, 0, diag(targetDir, runDay2(targetDir)));
            assert.equal(locked(targetDir, 'day2-dep'), '1.0.1');

            // the lockfile is put back on the older day2-dep, the way a project that
            // pinned it before would have it
            var lock = lockOf(targetDir);

            lock.packages['node_modules/day2-dep'] = {
                version: '1.0.0',
                resolved: tarball_url('day2-dep', '1.0.0'),
                integrity: registry['day2-dep']['1.0.0'].integrity,
            };
            fs.writeFileSync(path.join(targetDir, 'package-lock.json'), JSON.stringify(lock, null, 2));

            // a dependency is added, and node_modules is thrown away: this is the
            // fresh clone, where the lockfile is the only record of what was resolved
            writePkgjson(targetDir, { 'day2-pkg': '^1.0.0', 'day2-extra': '1.0.0' });
            rmdirSync(path.join(targetDir, 'node_modules'));

            var res = runDay2(targetDir);

            assert.equal(res.status, 0, diag(targetDir, res));
            assert.equal(locked(targetDir, 'day2-extra'), '1.0.0', 'the new dependency is installed');
            assert.equal(locked(targetDir, 'day2-dep'), '1.0.0',
                'the version the lockfile pins is kept: gaining one dependency must not ' +
                'upgrade the rest (npm keeps it too)');
            assert.equal(installed(targetDir, 'day2-dep'), '1.0.0', 'and it is that version on the disk');
        });

        it('--update resolves again to the newest the ranges allow', function () {
            var targetDir = makeProject({ 'day2-pkg': '1.0.0' });

            assert.equal(runDay2(targetDir).status, 0);
            assert.equal(locked(targetDir, 'day2-pkg'), '1.0.0');

            // a widened range is already satisfied: nothing to do, which is exactly
            // why refreshing needs a word of its own
            writePkgjson(targetDir, { 'day2-pkg': '^1.0.0' });
            assert.equal(runDay2(targetDir).status, 0);
            assert.equal(locked(targetDir, 'day2-pkg'), '1.0.0', 'a satisfied lockfile is left alone');

            var res = runDay2(targetDir, ['--install', '--update']);

            assert.equal(res.status, 0, diag(targetDir, res));
            assert.equal(locked(targetDir, 'day2-pkg'), '1.0.1', 'the newest the range allows');
            assert.equal(locked(targetDir, 'day2-dep'), '1.0.1', 'and what that package needs follows it');
            assert.equal(installed(targetDir, 'day2-pkg'), '1.0.1', 'the disk follows the lockfile');
            assert.equal(JSON.parse(fs.readFileSync(path.join(targetDir, 'package.json'), 'utf8'))
                .dependencies['day2-pkg'], '^1.0.0', 'package.json is not touched');
        });

        it('--update <pkg> refreshes that package only', function () {
            var targetDir = makeProject({ 'day2-pkg': '1.0.0', 'day2-extra': '1.0.0' });

            assert.equal(runDay2(targetDir).status, 0);

            writePkgjson(targetDir, { 'day2-pkg': '^1.0.0', 'day2-extra': '^1.0.0' });
            assert.equal(runDay2(targetDir).status, 0);

            var res = runDay2(targetDir, ['--install', '--update', 'day2-extra']);

            assert.equal(res.status, 0, diag(targetDir, res));
            assert.equal(locked(targetDir, 'day2-extra'), '1.0.1', 'the named package is refreshed');
            assert.equal(locked(targetDir, 'day2-pkg'), '1.0.0', 'the others are left where they are');
        });

        it('says so when --update names something that is not a dependency', function () {
            var targetDir = makeProject({ 'day2-pkg': '1.0.0' });

            assert.equal(runDay2(targetDir).status, 0);

            var res = runDay2(targetDir, ['--install', '--update', 'not-a-dep']);

            assert.equal(res.status, 0, diag(targetDir, res));
            assert.ok(outputOf(res).indexOf('is not a dependency of this project') > -1, outputOf(res));
            assert.equal(locked(targetDir, 'day2-pkg'), '1.0.0', 'and nothing moved');
        });

        it('refuses --update together with --ci', function () {
            var targetDir = makeProject({ 'day2-pkg': '1.0.0' });

            assert.equal(runDay2(targetDir).status, 0);

            var res = runDay2(targetDir, ['--install', '--update', '--ci']);

            assert.notEqual(res.status, 0, 'a frozen install installs what the lockfile says' + diag(targetDir, res));
            assert.ok(outputOf(res).indexOf('contradict') > -1, outputOf(res));
        });

        it('lists the flags it has in the usage text, and calls none of them planned', function () {
            var res = runInstaller(makeTargetDir(), ['--install', '--not-a-flag']);
            var out = outputOf(res);

            assert.notEqual(res.status, 0, 'an unknown option is still an error' + out);
            assert.ok(out.indexOf('--update [package]') > -1, out);
            assert.ok(out.indexOf('--ci, --frozen-lockfile') > -1, out);
            assert.ok(out.indexOf('planned') === -1, 'the usage text describes what is there: ' + out);
        });

        /**
         * @description a project that depends on the package with the optional
         *              platform binary. `dev` puts that package in devDependencies,
         *              which is the shape that pins both flags on the binary
         */
        function makePlatformProject(dev) {
            var targetDir = makeTargetDir();

            fs.writeFileSync(path.join(targetDir, 'package.json'), JSON.stringify({
                name: 'plat-proj', version: '1.0.0',
                dependencies: dev ? undefined : { 'plat-host': '1.0.0' },
                devDependencies: dev ? { 'plat-host': '1.0.0' } : undefined,
            }, null, 2));
            fs.writeFileSync(path.join(targetDir, '.npmrc'),
                'registry=https://localhost:' + port + '/\nstrict-ssl=false\n');

            return targetDir;
        }

        it('keeps an optional dependency of an installed package optional', function () {
            var targetDir = makePlatformProject(false);
            var res = runDay2(targetDir);

            assert.equal(res.status, 0, diag(targetDir, res));

            var lock = lockOf(targetDir).packages;
            var bin = lock['node_modules/plat-bin'];

            assert.ok(bin, 'the lockfile describes every platform: ' +
                Object.keys(lock).join(', ') + diag(targetDir, res));

            // an optional edge hangs off a package a production edge reaches: npm
            // still calls what it reaches optional, and `npm ci` refuses the lockfile
            // when it is written as a required package this platform cannot run
            assert.equal(bin.optional, true,
                'a package only an optional edge reaches is optional: ' + JSON.stringify(bin));
            assert.equal(bin.os.length, 1);
            assert.equal(bin.dev, undefined, 'nothing dev reaches it');
            assert.equal(bin.devOptional, undefined, 'the flag stands alone only when neither is set');
            assert.equal(installed(targetDir, 'plat-bin'), null,
                'nothing is installed for a platform this is not');
        });

        it('marks it dev as well when a dev dependency asks for it', function () {
            var targetDir = makePlatformProject(true);
            var res = runDay2(targetDir);

            assert.equal(res.status, 0, diag(targetDir, res));

            var bin = lockOf(targetDir).packages['node_modules/plat-bin'];

            // npm writes the overlap of the two trees as `dev` plus `optional`; a
            // `devOptional` next to them is what makes npm and this rewrite the
            // entry back and forth
            assert.equal(bin.dev, true, 'a dev edge reaches it: ' + JSON.stringify(bin));
            assert.equal(bin.optional, true);
            assert.equal(bin.devOptional, undefined, 'not three flags where npm writes two');
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

        // comparing against a live npm needs an environment where npm can install
        // (the ia32 Linux job runs inside a 2010 era container, where it cannot),
        // and the suite gates every other npm comparison behind this flag
        it('agrees with npm about what --omit=dev leaves out', { skip: !process.env.FIBJS_TEST_NETWORK }, function () {
            var ours = makeProject();
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
            assert.ok(fs.existsSync(path.join(theirs, 'node_modules')),
                'npm installed nothing, so it is no answer at all: ' +
                String(npmRes.stdout || '') + String(npmRes.stderr || ''));
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

        it('keeps a lockfile entry nothing needs, and installs it like npm does', function () {
            // npm validates the tree against the lockfile, not the other way round:
            // an entry package.json never asked for is installed, not pruned. What npm
            // does not do either is skip one it cannot fetch — `npm ci` fails on that,
            // and so does this (it used to leave the package out and report success)
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
                        // a published name at a version that was never published: the
                        // registry answers 404, as it does for `npm ci`
                        'node_modules/ghost': {
                            version: '9.9.9',
                            resolved: 'https://registry.npmjs.org/ghost/-/ghost-9.9.9.tgz',
                            integrity: 'sha512-' + 'A'.repeat(86) + '==',
                        },
                    },
                }, null, 2));
            });

            var res = runInstaller(targetDir, ['--install', '--ci']);

            assert.notEqual(res.status, 0, 'a package that cannot be fetched is an error' + diag(targetDir, res));
            assert.ok(outputOf(res).indexOf('ghost') > -1,
                'and the package is named rather than dropped in silence' + diag(targetDir, res));
        });

        it('installs a lockfile entry nothing needs, once it can be fetched', function () {
            // the same shape, with a target that exists: an entry the manifest never
            // asked for is installed, because the lockfile is the plan (`npm ci` does
            // the same)
            var targetDir = linkProject({
                name: 'extra-ok-proj', version: '1.0.0',
                dependencies: { linkpkg: 'file:linkpkg' },
            }, function (dir) {
                writeFile(dir, 'extrapkg/package.json',
                    JSON.stringify({ name: 'extrapkg', version: '3.0.0' }, null, 2));
                writeFile(dir, 'package-lock.json', JSON.stringify({
                    name: 'extra-ok-proj', version: '1.0.0', lockfileVersion: 3, requires: true,
                    packages: {
                        '': { name: 'extra-ok-proj', version: '1.0.0', dependencies: { linkpkg: 'file:linkpkg' } },
                        'linkpkg': { name: 'linkpkg', version: '1.0.0' },
                        'node_modules/linkpkg': { resolved: 'linkpkg', link: true },
                        // a second local package, which needs no registry
                        'extrapkg': { name: 'extrapkg', version: '3.0.0' },
                        'node_modules/extrapkg': { resolved: 'extrapkg', link: true },
                    },
                }, null, 2));
            });

            var res = runInstaller(targetDir, ['--install', '--ci']);

            assert.equal(res.status, 0, 'an extra lockfile entry is not an error' + diag(targetDir, res));
            assert.ok(checkInstalled(targetDir, 'linkpkg'), diag(targetDir, res));
            assert.ok(checkInstalled(targetDir, 'extrapkg'),
                'and the entry the manifest never asked for is installed: ' + diag(targetDir, res));
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

            // the flags live on the link's target (npm's `calc-dep-flags` puts them
            // there), not on the link entry
            var entry = lockfileOf(targetDir).packages['linkpkg'];
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

        it('installs a package a workspace member keeps nested', function () {
            var map = lockfile.to_path_map({
                lockfileVersion: 3,
                packages: {
                    '': { name: 'root' },
                    'node_modules/member': { resolved: 'packages/member', link: true },
                    'packages/member': { name: 'member', version: '1.0.0' },
                    'packages/member/node_modules/a': {
                        version: '1.0.0',
                        resolved: 'https://registry.npmjs.org/a/-/a-1.0.0.tgz',
                    },
                },
            });

            // the member's own path is a link target (nothing to install there), but what
            // it keeps nested is an installation target like any other: taking it for a
            // link target is how a frozen install came to skip it, leaving a lockfile
            // that describes a package the disk does not have
            assert.equal(map['packages/member'].target_only, true);
            assert.equal(map['packages/member/node_modules/a'].target_only, false);
            assert.equal(map['packages/member/node_modules/a'].name, 'a');
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

        it('resolves along the path of the dependent, the way npm does', function () {
            var packages = map_of({
                'node_modules/b': { version: '1.0.0' },
                'node_modules/a/node_modules/b': { version: '2.0.0' },
            });

            // the root reaches the root entry, and so does anything that has no copy of
            // its own: the nested 2.0.0 is not visible to it, which is why a lockfile
            // with it there is `Invalid` (the audit's D2 — this used to be accepted)
            var root = lockfile.lookup_entry(packages, 'b', '^2.0.0', semver, '');
            assert.equal(root.version, '1.0.0');

            // `a` finds its own nested copy first
            assert.equal(lockfile.lookup_entry(packages, 'b', '^2.0.0', semver, 'node_modules/a').version, '2.0.0');
            assert.equal(lockfile.lookup_entry(packages, 'b', '^1.0.0', semver, 'node_modules/a').version, '1.0.0',
                'a range the nested copy does not satisfy falls through to the root');

            // a workspace member looks under itself, then at the root
            var member = map_of({
                'node_modules/b': { version: '1.0.0' },
                'packages/member/node_modules/b': { version: '2.0.0' },
            });
            assert.equal(lockfile.lookup_entry(member, 'b', '^2.0.0', semver, 'packages/member').version, '2.0.0');
            assert.equal(lockfile.lookup_entry(member, 'b', '^2.0.0', semver, '').version, '1.0.0');
        });

        it('takes a nested entry when the resolver asks for it, which has no path yet', function () {
            var packages = map_of({
                'node_modules/b': { version: '1.0.0' },
                'node_modules/a/node_modules/b': { version: '2.0.0' },
            });

            // hoisting decides where a node ends up *after* it is resolved, so the
            // resolver keeps the plan's approximation (§4.2) — the sync check, which
            // does know the paths, must not
            assert.equal(lockfile.lookup_entry(packages, 'b', '^2.0.0', semver, '', { nested: true }).version, '2.0.0');
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

            // an entry without `name` is not an alias, whatever its path says: npm
            // writes the field only for one, and this is exactly how a lockfile the
            // installer wrote came to be refused by its own check
            var plain = map_of({ 'node_modules/alias': { version: '1.2.0' } })['node_modules/alias'];
            assert.equal(lockfile.spec_satisfied('npm:real@^1.0.0', plain, semver).ok, false);
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

        it('reads a file: spec the same way wherever the caller came from', function () {
            // a workspace member is reached through `path.relative()`, which spells
            // its path `packages\member` on Windows: the `..` segments still have to
            // cancel against the lockfile path the entry is recorded under
            var entry = map_of({
                'node_modules/linkpkg': { resolved: 'linkpkg', link: true },
                'linkpkg': { name: 'linkpkg', version: '1.0.0' },
            })['node_modules/linkpkg'];

            assert.equal(lockfile.spec_satisfied('file:../../linkpkg', entry, semver, 'packages/member').ok, true,
                'a member says file:../../x while the lockfile records x');
            assert.equal(lockfile.spec_satisfied('file:..\\..\\linkpkg', entry, semver, 'packages\\member').ok, true,
                'and Windows spells both sides with backslashes');
            assert.equal(lockfile.spec_satisfied('file:../../moved-away', entry, semver, 'packages/member').ok, false,
                'a spec that points somewhere else is still not in sync');
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

    describe('lockfile helper: writing the lockfile (Phase 2)', function () {
        /**
         * @description a lockfile as npm writes it: one registry package, with the
         *              fields this installer does not produce itself
         */
        function previousLock() {
            return {
                name: 'p', version: '1.0.0', lockfileVersion: 3, requires: true,
                packages: {
                    '': { name: 'p', version: '1.0.0', dependencies: { a: '^1.0.0' } },
                    'node_modules/a': {
                        version: '1.0.0',
                        resolved: 'https://registry.npmjs.org/a/-/a-1.0.0.tgz',
                        integrity: 'sha512-AAAA',
                        engines: { node: '>=14' },
                        dependencies: { b: '^1.0.0' },
                    },
                },
            };
        }

        it('keeps what the previous lockfile said about a node this run did not fetch', function () {
            // a node read from disk carries no `dist`, so its source is the one the
            // lockfile already recorded. Rewriting it as `{version, dependencies}` is
            // what dropped the integrity npm wrote — 835 of 1238 entries in the audit
            var built = lockfile.to_lock({
                'node_modules/a': { name: 'a', version: '1.0.0', dependencies: { b: '^1.0.0' } },
            }, { name: 'p', version: '1.0.0' }, { previous: previousLock(), root: '/proj' });

            var entry = built.lock.packages['node_modules/a'];
            assert.equal(entry.resolved, 'https://registry.npmjs.org/a/-/a-1.0.0.tgz');
            assert.equal(entry.integrity, 'sha512-AAAA');
            assert.equal(entry.engines.node, '>=14', 'the fields npm writes are kept too');
            assert.equal(built.kept.length, 1);
        });

        it('rewrites an entry whose source the tree did fetch, and one that moved', function () {
            var moved = lockfile.to_lock({
                'node_modules/a': {
                    name: 'a', version: '1.0.0',
                    resolved: 'https://mirror.example.com/a/-/a-1.0.0.tgz',
                },
            }, { name: 'p', version: '1.0.0' }, { previous: previousLock(), root: '/proj' });

            assert.equal(moved.lock.packages['node_modules/a'].resolved,
                'https://mirror.example.com/a/-/a-1.0.0.tgz', 'a source that changed is written');
            assert.equal(moved.kept.length, 0);

            var newer = lockfile.to_lock({
                'node_modules/a': {
                    name: 'a', version: '1.0.1',
                    resolved: 'https://registry.npmjs.org/a/-/a-1.0.1.tgz',
                },
            }, { name: 'p', version: '1.0.0' }, { previous: previousLock(), root: '/proj' });

            assert.equal(newer.lock.packages['node_modules/a'].version, '1.0.1');
        });

        it('writes the name a package goes by when it differs from its directory', function () {
            var built = lockfile.to_lock({
                'node_modules/alias': {
                    name: 'real', version: '1.2.0',
                    resolved: 'https://registry.npmjs.org/real/-/real-1.2.0.tgz',
                },
            }, { name: 'p', version: '1.0.0' }, { root: '/proj' });

            assert.equal(built.lock.packages['node_modules/alias'].name, 'real',
                'npm records an alias as `name`, and nothing else marks it');
        });

        it('writes the fields npm projects from package.json into the root entry', function () {
            var manifest = {
                name: 'p', version: '1.0.0',
                license: 'MIT',
                engines: { node: '>=18' },
                workspaces: ['packages/*'],
                peerDependenciesMeta: { x: { optional: true } },
                scripts: { postinstall: 'echo hi' },
                dependencies: { a: '^1.0.0' },
                devDependencies: { d: '^2.0.0' },
            };

            var root = lockfile.root_entry(manifest, null);

            // `workspaces` among them: a root entry written without it makes npm and this
            // rewrite the file back and forth
            assert.deepEqual(root.workspaces, ['packages/*']);
            assert.deepEqual(root.engines, { node: '>=18' });
            assert.equal(root.license, 'MIT');
            assert.deepEqual(root.peerDependenciesMeta, { x: { optional: true } });
            assert.equal(root.hasInstallScript, true, 'npm records the presence of an install script');
            assert.deepEqual(root.dependencies, { a: '^1.0.0' });
            assert.deepEqual(root.devDependencies, { d: '^2.0.0' });

            // what the lockfile already carried beyond those fields is kept
            var kept = lockfile.root_entry(manifest, { name: 'p', version: '1.0.0', packageManager: 'pnpm@9' });

            assert.equal(kept.packageManager, 'pnpm@9', 'a field this does not project survives');
            assert.deepEqual(kept.workspaces, ['packages/*']);

            // …but a projected field leaves with the manifest field that fed it: npm drops
            // it on its next write, and keeping it here makes the two trade the file back
            // and forth
            var dropped = lockfile.root_entry({ name: 'p', version: '1.0.0' },
                { name: 'p', version: '1.0.0', engines: { node: '>=18' }, workspaces: ['packages/*'] });

            assert.equal(dropped.engines, undefined, 'an engine dropped from package.json leaves the entry');
            assert.equal(dropped.workspaces, undefined, 'so does a workspace list');
        });

        it('orders the keys the way npm writes them', function () {
            // npm serializes through `json-stringify-nice`: plain values first (the named
            // keys among them in that order, the rest alphabetically), then the objects.
            // The named keys are consulted inside a group, not across one — which is why
            // `workspaces` (an array) comes before `dependencies` in a root entry
            var root = lockfile.entry_order({
                dependencies: { b: '^1.0.0' },
                workspaces: ['packages/*'],
                version: '1.0.0',
                name: 'p',
                engines: { node: '>=18' },
                license: 'MIT',
            });

            assert.deepEqual(Object.keys(root),
                ['name', 'version', 'license', 'workspaces', 'dependencies', 'engines'],
                'the named keys first, then license and workspaces alphabetically, then the objects');
        });

        it('writes a lockfile with npm\'s key order all the way down', function () {
            var text = lockfile.stringify_lock({
                lockfileVersion: 3,
                name: 'p',
                version: '1.0.0',
                packages: {
                    'node_modules/a': {
                        version: '1.0.0',
                        dependencies: { z: '^1.0.0', a: '^1.0.0' },
                        engines: { node: '>=18' },
                        license: 'MIT',
                    },
                    '': { version: '1.0.0', dependencies: { a: '1.0.0' }, workspaces: ['packages/*'] },
                },
            });

            var parsed = JSON.parse(text);

            assert.deepEqual(Object.keys(parsed), ['name', 'version', 'lockfileVersion', 'packages']);
            assert.deepEqual(Object.keys(parsed.packages['']), ['version', 'workspaces', 'dependencies']);
            assert.deepEqual(Object.keys(parsed.packages['node_modules/a']),
                ['version', 'license', 'dependencies', 'engines'],
                'plain values first, then the objects, `dependencies` before `engines`');
            assert.deepEqual(Object.keys(parsed.packages['node_modules/a'].dependencies), ['a', 'z'],
                'a dependency map is sorted too');
            assert.deepEqual(Object.keys(parsed.packages['node_modules/a'].engines), ['node']);
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
