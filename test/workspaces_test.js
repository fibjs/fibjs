var { describe, it, afterEach } = require('node:test');
var assert = require('assert');
const child_process = require('child_process');
const fs = require('fs');
const path = require('path');

// Detect if running in fibjs or nodejs
const isFibjs = typeof process !== 'undefined' && process.versions && process.versions.fibjs;

function install_npm(testDir) {
    if (isFibjs) {
        const npmInstall = child_process.spawnSync(process.execPath, ['--install'], {
            cwd: testDir,
            stdio: 'inherit',
            env: { ...process.env, FIBJS_SILENT_INSALL: undefined } // explicitly unset to enable logging
        });
    } else {
        // Use npm.cmd on Windows for better compatibility
        const npmCommand = process.platform === 'win32' ? 'npm.cmd' : 'npm';
        const npmInstall = child_process.spawnSync(npmCommand, ['install'], {
            cwd: testDir,
            stdio: 'inherit',
            shell: process.platform === 'win32' // Use shell on Windows
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
    },
    {
        description: "should install dependencies for workspace packages",
        testDir: 'deps_workspace',
        symlinks: [
            {
                from: 'packages/simple-pkg',
                to: 'simple-pkg'
            }
        ],
        // Verify that workspace package dependencies are installed
        dependencies: [
            {
                packageName: 'simple-pkg',
                dependency: 'ms'
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
                        // Use recursive directory removal that works in both Node.js and FibJS
                        function removeDir(dirPath) {
                            const files = fs.readdirSync(dirPath);
                            files.forEach(file => {
                                const fullPath = path.join(dirPath, file);
                                const stat = fs.lstatSync(fullPath);
                                if (stat.isDirectory()) {
                                    removeDir(fullPath);
                                } else {
                                    fs.unlinkSync(fullPath);
                                }
                            });
                            fs.rmdirSync(dirPath);
                        }
                        removeDir(filePath);
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

            // Verify workspace package dependencies are installed
            if (test.dependencies) {
                test.dependencies.forEach(dep => {
                    const rootNodeModules = path.join(testDir, 'node_modules');
                    const packageNodeModules = path.join(testDir, 'packages', dep.packageName, 'node_modules');

                    // Check if dependency exists in root node_modules (hoisted)
                    const depInRoot = path.join(rootNodeModules, dep.dependency);
                    const depInPackage = path.join(packageNodeModules, dep.dependency);

                    const rootExists = fs.existsSync(depInRoot);
                    const packageExists = fs.existsSync(depInPackage);

                    // Dependency should exist in either root or package node_modules
                    const dependencyExists = rootExists || packageExists;
                    assert.ok(dependencyExists,
                        `Dependency '${dep.dependency}' should be installed for package '${dep.packageName}'`);
                });
            }
        });
    });
});
