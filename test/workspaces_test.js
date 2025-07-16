var { describe, it, afterEach } = require('node:test');
var assert = require('assert');
const child_process = require('child_process');
const fs = require('fs');
const path = require('path');

// Detect if running in fibjs or nodejs
const isFibjs = typeof process !== 'undefined' && process.versions && process.versions.fibjs;

function install_npm(testDir) {
    if (isFibjs) {
        console.log('[TEST DEBUG] Running fibjs install in:', testDir);
        const npmInstall = child_process.spawnSync(process.execPath, ['--install'], {
            cwd: testDir,
            stdio: 'inherit',
            env: { ...process.env, FIBJS_SILENT_INSALL: undefined } // explicitly unset to enable logging
        });
        console.log('[TEST DEBUG] fibjs install completed with code:', npmInstall.status);
    } else {
        const npmInstall = child_process.spawnSync('npm', ['install'], {
            cwd: testDir,
            stdio: 'pipe'
        });
    }
}

const tests = [
    {
        description: "should create symlinks for basic workspace packages",
        testDir: 'basic_workspace',
        symlinks: [
            {
                from: 'packages/package-a',
                to: 'package-a'
            }
        ]
    },
    {
        description: "should handle nested workspace structure with scoped packages",
        testDir: 'nested_workspace',
        symlinks: [
            {
                from: 'apps/web-app',
                to: '@myorg/web-app'
            },
            {
                from: 'libs/ui-lib',
                to: '@myorg/ui-lib'
            },
            {
                from: 'libs/utils',
                to: '@myorg/utils'
            }
        ]
    },
    {
        description: "should handle glob pattern workspaces",
        testDir: 'glob_workspace',
        symlinks: [
            {
                from: 'packages/level1/deep-package-a',
                to: 'deep-package-a'
            }
        ]
    },
    {
        description: "should handle circular dependencies between workspace packages",
        testDir: 'circular_workspace',
        symlinks: [
            {
                from: 'services/service-a',
                to: 'service-a'
            },
            {
                from: 'services/service-b',
                to: 'service-b'
            }
        ]
    }
];

// NPM Workspaces Tests
describe("workspaces test", () => {
    var test_paths = [];

    afterEach(() => {
        test_paths.forEach((filePath) => {
            try {
                if (fs.existsSync(filePath)) {
                    const stats = fs.lstatSync(filePath);
                    if (stats.isDirectory()) {
                        fs.rmSync(filePath, { recursive: true, force: true });
                    } else {
                        fs.unlinkSync(filePath);
                    }
                }
            } catch (e) {
                console.log('Cleanup error:', e.message);
            }
        });

        test_paths = [];
    });

    tests.forEach(test => {
        it(test.description, () => {
            const testDir = path.join(__dirname, 'workspaces_files', test.testDir);

            const nodeModulesPath = path.join(testDir, 'node_modules');
            const packageLockPath = path.join(testDir, 'package-lock.json');

            test_paths.push(nodeModulesPath);
            test_paths.push(packageLockPath);

            install_npm(testDir);

            // Verify all expected symlinks
            test.symlinks.forEach(symlink => {
                const symlinkPath = path.join(testDir, 'node_modules', symlink.to);
                assert.ok(fs.existsSync(symlinkPath), `${symlink.to} symlink should exist`);

                const expectedTarget = path.join(testDir, symlink.from);
                const actualTarget = fs.realpathSync(symlinkPath);
                assert.equal(actualTarget, expectedTarget, `${symlink.to} should be symlinked to ${symlink.from}`);
            });
        });
    });
});
