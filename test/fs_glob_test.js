const assert = require("assert");
const { describe, it } = require("node:test");
const path = require("path");
const fs = require("fs").promises;
const fsSync = require("fs");
const os = require("os");

// Test files directory
const testDir = path.join(__dirname, 'fs_files');

// Helper function to convert AsyncGenerator to array, or return array as-is
async function asyncGeneratorToArray(input) {
    // If it's already an array, return it directly
    if (Array.isArray(input)) {
        return input;
    }

    // If it's an AsyncGenerator, iterate through it
    const result = [];
    for await (const item of input) {
        result.push(item);
    }
    return result;
}

// Helper function to detect Windows environment
function isWindows() {
    return process.platform === 'win32';
}

// Helper function to normalize paths for Windows
function normalizeExpected(expected) {
    if (!isWindows()) {
        return expected;
    }

    if (Array.isArray(expected)) {
        return expected.map(item => item.replace(/\//g, '\\'));
    }

    return expected.replace(/\//g, '\\');
}



describe("fs.glob", () => {

    // ==================== 基础功能测试 ====================
    describe("Basic functionality", () => {

        it("should return an empty array for non-existent directory", async () => {
            const dir = path.join(os.tmpdir(), "non_existent_dir");
            const asyncGen = await fs.glob("*", { cwd: dir });
            const result = await asyncGeneratorToArray(asyncGen);
            assert.deepStrictEqual(result, []);
        });

        it("should match all files with *", async () => {
            const asyncGen = await fs.glob("*", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            // Node.js glob includes both files and directories
            let expected = ['README.md', 'app.txt', 'config.env', 'deps', 'docs', 'error.txt', 'index.js', 'package.json', 'read.txt', 'src', 'tests'];
            expected = normalizeExpected(expected);
            expected.sort();
            assert.deepStrictEqual(result, expected);
        });

        it("should match specific file extensions", async () => {
            const asyncGen = await fs.glob("*.js", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            assert.deepStrictEqual(result, ['index.js']);
        });

        it("should match files with specific patterns", async () => {
            const asyncGen = await fs.glob("*.txt", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            let expected = ['app.txt', 'error.txt', 'read.txt'];
            expected = normalizeExpected(expected);
            assert.deepStrictEqual(result, expected);
        });
    });

    // ==================== 通配符模式测试 ====================
    describe("Wildcard patterns", () => {

        it("should handle single character wildcard (?)", async () => {
            const asyncGen = await fs.glob("*.t?t", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            let expected = ['app.txt', 'error.txt', 'read.txt'];
            expected = normalizeExpected(expected);
            assert.deepStrictEqual(result, expected);
        });

        it("should handle character classes", async () => {
            const asyncGen = await fs.glob("*.[jt]s", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            assert.deepStrictEqual(result, ['index.js']);
        });

        it("should handle negated character classes", async () => {
            const asyncGen = await fs.glob("*.[!t]*", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            // Should match files that don't have 't' in their extension
            const expected = ['README.md', 'config.env', 'index.js', 'package.json'];
            expected.sort();
            assert.deepStrictEqual(result, expected);
        });
    });

    // ==================== 递归模式测试 ====================
    describe("Recursive patterns (**)", () => {

        it("should match files recursively with **", async () => {
            const asyncGen = await fs.glob("**/*.js", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            let expected = [
                'index.js',
                'deps/fake-module.js',
                'src/utils/math.js',
                'tests/math.test.js'
            ];
            expected = normalizeExpected(expected);
            expected.sort();
            assert.deepStrictEqual(result, expected);
        });

        it("should match all TypeScript files", async () => {
            const asyncGen = await fs.glob("**/*.ts", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            let expected = ['src/utils/helper.ts'];
            expected = normalizeExpected(expected);
            assert.deepStrictEqual(result, expected);
        });

        it("should match React components", async () => {
            const asyncGen = await fs.glob("**/*.jsx", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            let expected = ['src/App.jsx', 'src/components/Button.jsx'];
            expected = normalizeExpected(expected);
            expected.sort();
            assert.deepStrictEqual(result, expected);
        });

        it("should match test files", async () => {
            const asyncGen = await fs.glob("**/*.test.*", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            let expected = ['tests/components.test.tsx', 'tests/math.test.js'];
            expected = normalizeExpected(expected);
            expected.sort();
            assert.deepStrictEqual(result, expected);
        });
    });

    // ==================== 目录模式测试 ====================
    describe("Directory patterns", () => {

        it("should match files in specific directories", async () => {
            const asyncGen = await fs.glob("src/*.jsx", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            let expected = ['src/App.jsx'];
            expected = normalizeExpected(expected);
            assert.deepStrictEqual(result, expected);
        });

        it("should match files in nested directories", async () => {
            const asyncGen = await fs.glob("src/utils/*", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            let expected = ['src/utils/helper.ts', 'src/utils/math.js'];
            expected = normalizeExpected(expected);
            expected.sort();
            assert.deepStrictEqual(result, expected);
        });

        it("should match all files in any subdirectory", async () => {
            const asyncGen = await fs.glob("*/", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            // Node.js glob doesn't include trailing / in directory names
            let expected = ['deps', 'docs', 'src', 'tests'];
            expected = normalizeExpected(expected);
            expected.sort();
            assert.deepStrictEqual(result, expected);
        });
    });

    // ==================== 隐藏文件测试 ====================
    describe("Hidden files", () => {

        it("should match hidden files explicitly", async () => {
            const asyncGen = await fs.glob(".*", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            let expected = ['.gittest', '.hidden'];
            expected = normalizeExpected(expected);
            expected.sort();
            assert.deepStrictEqual(result, expected);
        });

        it("should match files in hidden directories", async () => {
            const asyncGen = await fs.glob(".hidden/*", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            let expected = ['.hidden/secret.txt'];
            expected = normalizeExpected(expected);
            assert.deepStrictEqual(result, expected);
        });

        it("should include hidden files with ** when appropriate", async () => {
            const asyncGen = await fs.glob("**/.env", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            // In Node.js, **/.env doesn't match .env in the root directory
            assert.deepStrictEqual(result, []);
        });
    });

    // ==================== 复杂模式测试 ====================
    describe("Complex patterns", () => {

        it("should handle multiple wildcards", async () => {
            const asyncGen = await fs.glob("**/src/**/*.js", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            let expected = ['src/utils/math.js'];
            expected = normalizeExpected(expected);
            assert.deepStrictEqual(result, expected);
        });

        it("should match multiple file types", async () => {
            const asyncGen = await fs.glob("**/*.{js,ts,jsx}", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            let expected = [
                'index.js',
                'deps/fake-module.js',
                'src/App.jsx',
                'src/components/Button.jsx',
                'src/utils/helper.ts',
                'src/utils/math.js',
                'tests/math.test.js'
            ];
            expected = normalizeExpected(expected);
            expected.sort();
            assert.deepStrictEqual(result, expected);
        });

        it("should exclude specific directories", async () => {
            const asyncGen = await fs.glob("**/*.js", {
                cwd: testDir,
                exclude: ['deps/**']
            });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            // Our implementation correctly excludes deps files
            let expected = ['index.js', 'src/utils/math.js', 'tests/math.test.js'];
            expected = normalizeExpected(expected);
            expected.sort();
            assert.deepStrictEqual(result, expected);
        });
    });

    // ==================== 选项测试 ====================
    describe("Options", () => {

        it("should respect the cwd option", async () => {
            const srcDir = path.join(testDir, 'src');
            const asyncGen = await fs.glob("*.jsx", { cwd: srcDir });
            const result = await asyncGeneratorToArray(asyncGen);
            assert.deepStrictEqual(result, ['App.jsx']);
        });

        it("should handle absolute patterns", async () => {
            const pattern = path.join(testDir, "*.js");
            const asyncGen = await fs.glob(pattern);
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            const expected = [path.join(testDir, 'index.js')];
            assert.deepStrictEqual(result, expected);
        });

        it("should return absolute paths when withFileTypes is false", async () => {
            const asyncGen = await fs.glob("*.js", {
                cwd: testDir,
                withFileTypes: false
            });
            const result = await asyncGeneratorToArray(asyncGen);
            // Should return relative paths by default
            assert.deepStrictEqual(result, ['index.js']);
        });

        it("should return Stat objects when withFileTypes is true", async () => {
            const asyncGen = await fs.glob("*.js", {
                cwd: testDir,
                withFileTypes: true
            });
            const result = await asyncGeneratorToArray(asyncGen);

            // Should return array of Stat objects
            assert.strictEqual(result.length, 1);
            assert.strictEqual(typeof result[0], 'object');
            assert.strictEqual(typeof result[0].isFile, 'function');
            assert.strictEqual(typeof result[0].isDirectory, 'function');
            assert.strictEqual(result[0].isFile(), true);
            assert.strictEqual(result[0].isDirectory(), false);

            // Check that path property exists and is correct
            assert.strictEqual(typeof result[0].name, 'string');
            assert.ok(result[0].name.endsWith('index.js'));
        });

        it("should return Stat objects for directories when withFileTypes is true", async () => {
            const asyncGen = await fs.glob("*/", {
                cwd: testDir,
                withFileTypes: true
            });
            const result = await asyncGeneratorToArray(asyncGen);

            // Should return array of Stat objects for directories
            assert.ok(result.length > 0);

            // Find the 'src' directory in results
            const srcStat = result.find(stat => stat.name && stat.name.includes('src'));
            assert.ok(srcStat, 'Should find src directory');
            assert.strictEqual(typeof srcStat, 'object');
            assert.strictEqual(typeof srcStat.isFile, 'function');
            assert.strictEqual(typeof srcStat.isDirectory, 'function');
            assert.strictEqual(srcStat.isFile(), false);
            assert.strictEqual(srcStat.isDirectory(), true);
        });

        it("should return Stat objects for recursive patterns when withFileTypes is true", async () => {
            const asyncGen = await fs.glob("**/*.js", {
                cwd: testDir,
                withFileTypes: true,
                exclude: ['deps/**']
            });
            const result = await asyncGeneratorToArray(asyncGen);

            // Should return array of Stat objects
            assert.ok(result.length >= 2); // At least index.js and src/utils/math.js

            for (const stat of result) {
                assert.strictEqual(typeof stat, 'object');
                assert.strictEqual(typeof stat.isFile, 'function');
                assert.strictEqual(typeof stat.isDirectory, 'function');
                assert.strictEqual(stat.isFile(), true);
                assert.strictEqual(stat.isDirectory(), false);
                assert.strictEqual(typeof stat.name, 'string');
                assert.ok(stat.name.endsWith('.js'));
            }
        });

        it("should handle mixed file types with withFileTypes", async () => {
            const asyncGen = await fs.glob("**/*.{js,md}", {
                cwd: testDir,
                withFileTypes: true,
                exclude: ['deps/**']
            });
            const result = await asyncGeneratorToArray(asyncGen);

            // Should return array of Stat objects
            assert.ok(result.length >= 3); // At least index.js, README.md, docs/api.md

            let jsFiles = 0;
            let mdFiles = 0;

            for (const stat of result) {
                assert.strictEqual(typeof stat, 'object');
                assert.strictEqual(typeof stat.isFile, 'function');
                assert.strictEqual(stat.isFile(), true);
                assert.strictEqual(typeof stat.name, 'string');

                if (stat.name.endsWith('.js')) {
                    jsFiles++;
                } else if (stat.name.endsWith('.md')) {
                    mdFiles++;
                }
            }

            assert.ok(jsFiles >= 2, 'Should have at least 2 JS files');
            assert.ok(mdFiles >= 2, 'Should have at least 2 MD files');
        });
    });

    // ==================== 边界情况测试 ====================
    describe("Edge cases", () => {

        it("should handle empty pattern", async () => {
            const asyncGen = await fs.glob("", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            assert.deepStrictEqual(result, []);
        });

        it("should handle pattern with no matches", async () => {
            const asyncGen = await fs.glob("*.xyz", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            assert.deepStrictEqual(result, []);
        });

        it("should handle special characters in filenames", async () => {
            // Test files with dots in names
            const asyncGen = await fs.glob("*.md", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            assert.deepStrictEqual(result, ['README.md']);
        });

        it("should handle case sensitivity", async () => {
            const asyncGen = await fs.glob("readme.md", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            // On case-insensitive file systems (macOS/Windows), should find README.md
            // and return the actual file system name, not the pattern case
            if (process.platform === 'linux') {
                assert.deepStrictEqual(result, []);
            } else {
                // On case-insensitive systems, should find the file but return actual name
                assert.strictEqual(result.length, 1);
                assert.ok(result[0].toLowerCase() === 'readme.md');
            }
        });
    });

    // ==================== fibjs 缺陷修复测试 ====================
    describe("Absolute path glob patterns", () => {

        it("should match workspace packages with absolute path pattern", async () => {
            // This test exposes a bug in fibjs where absolute path glob patterns 
            // with wildcards fail to match existing files
            const workspaceBasePath = path.join(__dirname, 'workspaces_files', 'basic_workspace', 'packages');
            const absolutePattern = path.join(workspaceBasePath, '*', 'package.json');

            // Test the glob pattern - this should work but currently fails in fibjs
            const asyncGen = await fs.glob(absolutePattern);
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();

            // This should find exactly 2 package.json files
            const expectedResults = [
                path.join(workspaceBasePath, 'package-a', 'package.json'),
                path.join(workspaceBasePath, 'package-b', 'package.json')
            ].sort();
            assert.deepStrictEqual(result, expectedResults);
        });

        it("should match workspace packages with relative path pattern", async () => {
            // This test shows that relative patterns should work correctly
            const relativePattern = 'workspaces_files/basic_workspace/packages/*/package.json';

            const asyncGen = await fs.glob(relativePattern, { cwd: __dirname });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();

            // This should work in both fibjs and Node.js
            let expectedResults = [
                'workspaces_files/basic_workspace/packages/package-a/package.json',
                'workspaces_files/basic_workspace/packages/package-b/package.json'
            ];
            expectedResults = normalizeExpected(expectedResults);
            expectedResults.sort();
            assert.deepStrictEqual(result, expectedResults);
        });

        it("should handle absolute paths with nested wildcards", async () => {
            // Test another absolute path pattern with nested structure
            const testFilesDir = path.join(__dirname, 'fs_files');
            const absolutePattern = path.join(testFilesDir, 'src', '**', '*.js');

            const asyncGen = await fs.glob(absolutePattern);
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();

            // Should find JS files in src subdirectories
            const expectedResults = [
                path.join(testFilesDir, 'src', 'utils', 'math.js')
            ];
            assert.deepStrictEqual(result, expectedResults);
        });

        it("should have consistent sync vs async behavior for absolute paths", async () => {
            // Test both sync and async versions for consistency
            const workspaceBasePath = path.join(__dirname, 'workspaces_files', 'basic_workspace', 'packages');
            const absolutePattern = path.join(workspaceBasePath, '*', 'package.json');

            // Test async version
            const asyncGen = await fs.glob(absolutePattern);
            const asyncResult = await asyncGeneratorToArray(asyncGen);

            // Test sync version (if available)
            let syncResult = [];
            if (fs.globSync) {
                syncResult = fs.globSync(absolutePattern);
                // Both should return the same results
                assert.deepStrictEqual(asyncResult.sort(), syncResult.sort(), 'Sync and async should return same results');
            } else {
                // If globSync is not available, just test that async returns expected results
                const expectedResults = [
                    path.join(workspaceBasePath, 'package-a', 'package.json'),
                    path.join(workspaceBasePath, 'package-b', 'package.json')
                ].sort();
                assert.deepStrictEqual(asyncResult.sort(), expectedResults);
            }
        });

        it("should handle absolute paths with multiple wildcard levels", async () => {
            // Test deeply nested absolute patterns with multiple wildcards
            const testFilesDir = path.join(__dirname, 'fs_files');
            const absolutePattern = path.join(testFilesDir, '*', '*', '*.*');

            const asyncGen = await fs.glob(absolutePattern);
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();

            // Should find files at exactly 3 levels deep (Node.js behavior)
            // tests/components.test.tsx and tests/math.test.js are only 2 levels deep, so not matched
            const expectedResults = [
                path.join(testFilesDir, 'src', 'components', 'Button.jsx'),
                path.join(testFilesDir, 'src', 'utils', 'helper.ts'),
                path.join(testFilesDir, 'src', 'utils', 'math.js')
            ].sort();
            assert.deepStrictEqual(result, expectedResults);
        });

        it("should handle absolute paths with globstar patterns", async () => {
            // Test absolute paths combined with globstar recursion
            const testFilesDir = path.join(__dirname, 'fs_files');
            const absolutePattern = path.join(testFilesDir, '**', 'components', '**', '*.jsx');

            const asyncGen = await fs.glob(absolutePattern);
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();

            // Should find all JSX files in any components directory
            const expectedResults = [
                path.join(testFilesDir, 'src', 'components', 'Button.jsx')
            ];
            assert.deepStrictEqual(result, expectedResults);
        });

        it("should handle absolute paths with character classes", async () => {
            // Test absolute paths with character class patterns
            const testFilesDir = path.join(__dirname, 'fs_files');
            const absolutePattern = path.join(testFilesDir, 'src', 'utils', '*.[jt]s');

            const asyncGen = await fs.glob(absolutePattern);
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();

            // Should find both .js and .ts files
            const expectedResults = [
                path.join(testFilesDir, 'src', 'utils', 'helper.ts'),
                path.join(testFilesDir, 'src', 'utils', 'math.js')
            ].sort();
            assert.deepStrictEqual(result, expectedResults);
        });

        it("should handle absolute paths with brace expansion", async () => {
            // Test absolute paths with brace expansion patterns
            const testFilesDir = path.join(__dirname, 'fs_files');
            const absolutePattern = path.join(testFilesDir, 'src', 'utils', '*.{js,ts}');

            const asyncGen = await fs.glob(absolutePattern);
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();

            // Should find files matching both extensions
            const expectedResults = [
                path.join(testFilesDir, 'src', 'utils', 'helper.ts'),
                path.join(testFilesDir, 'src', 'utils', 'math.js')
            ].sort();
            assert.deepStrictEqual(result, expectedResults);
        });

        it("should handle absolute paths ending with directory pattern", async () => {
            // Test absolute paths that match directories specifically
            const testFilesDir = path.join(__dirname, 'fs_files');
            const absolutePattern = path.join(testFilesDir, '*/');

            const asyncGen = await fs.glob(absolutePattern);
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();

            // Should find all subdirectories
            const expectedResults = [
                'deps', 'docs', 'src', 'tests'
            ].map(dir => path.join(testFilesDir, dir)).sort();
            assert.deepStrictEqual(result, expectedResults);
        });

        it("should handle absolute paths with question mark wildcards", async () => {
            // Test absolute paths with single character wildcards
            const testFilesDir = path.join(__dirname, 'fs_files');
            const absolutePattern = path.join(testFilesDir, 'src', '?pp.jsx');

            const asyncGen = await fs.glob(absolutePattern);
            const result = await asyncGeneratorToArray(asyncGen);

            // Should find App.jsx
            const expectedResults = [
                path.join(testFilesDir, 'src', 'App.jsx')
            ];
            assert.deepStrictEqual(result, expectedResults);
        });

        it("should handle absolute paths with negation patterns", async () => {
            // Test absolute paths with exclude/negation patterns
            const testFilesDir = path.join(__dirname, 'fs_files');
            const absolutePattern = path.join(testFilesDir, 'src', '**', '*.[!t]*');

            const asyncGen = await fs.glob(absolutePattern);
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();

            // Should find files that don't end with .ts/.tsx
            const expectedResults = [
                path.join(testFilesDir, 'src', 'App.jsx'),
                path.join(testFilesDir, 'src', 'components', 'Button.jsx'),
                path.join(testFilesDir, 'src', 'utils', 'math.js')
            ].sort();
            assert.deepStrictEqual(result, expectedResults);
        });

        it("should handle absolute paths with mixed separators on Windows", async () => {
            // Test that absolute paths work correctly with mixed path separators
            if (process.platform === 'win32') {
                const testFilesDir = path.join(__dirname, 'fs_files');
                // Mix forward and backward slashes (Windows should handle both)
                const absolutePattern = testFilesDir.replace(/\\/g, '/') + '/src\\**\\*.js';

                const asyncGen = await fs.glob(absolutePattern);
                const result = await asyncGeneratorToArray(asyncGen);
                result.sort();

                // Should find JS files despite mixed separators
                const expectedResults = [
                    path.join(testFilesDir, 'src', 'utils', 'math.js')
                ];
                assert.deepStrictEqual(result, expectedResults);
            } else {
                // On POSIX systems, test with forward slashes only
                const testFilesDir = path.join(__dirname, 'fs_files');
                const absolutePattern = testFilesDir + '/src/**/math.js';

                const asyncGen = await fs.glob(absolutePattern);
                const result = await asyncGeneratorToArray(asyncGen);

                const expectedResults = [
                    path.join(testFilesDir, 'src', 'utils', 'math.js')
                ];
                assert.deepStrictEqual(result, expectedResults);
            }
        });

        it("should handle absolute paths that don't exist", async () => {
            // Test absolute paths that point to non-existent locations
            const nonExistentPath = path.join(__dirname, 'non_existent_dir', '*', 'file.txt');

            const asyncGen = await fs.glob(nonExistentPath);
            const result = await asyncGeneratorToArray(asyncGen);

            // Should return empty array for non-existent paths
            assert.deepStrictEqual(result, []);
        });

        it("should handle very long absolute paths", async () => {
            // Test that absolute paths work with deeply nested structures
            const testFilesDir = path.join(__dirname, 'fs_files');
            const absolutePattern = path.join(testFilesDir, 'src', 'components', '**', '*.jsx');

            const asyncGen = await fs.glob(absolutePattern);
            const result = await asyncGeneratorToArray(asyncGen);

            // Should handle deep paths correctly
            const expectedResults = [
                path.join(testFilesDir, 'src', 'components', 'Button.jsx')
            ];
            assert.deepStrictEqual(result, expectedResults);
        });

        it("should handle absolute paths with root directory patterns", async () => {
            // Test absolute paths starting from filesystem root
            if (process.platform !== 'win32') {
                // On POSIX systems, test patterns starting with /
                const tempDir = os.tmpdir();
                const pattern = path.join(tempDir, '*');

                const asyncGen = await fs.glob(pattern);
                const result = await asyncGeneratorToArray(asyncGen);

                // Should return some files/directories in temp (usually non-empty)
                assert.ok(Array.isArray(result), 'Should return an array');
                // Don't assert specific contents as temp directory varies
            } else {
                // On Windows, test with drive letter patterns
                const pattern = path.join('C:', 'Windows', 'System32', 'drivers', 'etc', 'hosts');

                const asyncGen = await fs.glob(pattern);
                const result = await asyncGeneratorToArray(asyncGen);

                // Should either find the hosts file or return empty array
                assert.ok(Array.isArray(result), 'Should return an array');
                assert.ok(result.length <= 1, 'Should return at most one file');
            }
        });
    });

    // ==================== 实际应用场景测试 ====================
    describe("Real-world scenarios", () => {

        it("should find all source files", async () => {
            const asyncGen = await fs.glob("src/**/*.{js,ts,jsx,tsx}", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            let expected = [
                'src/App.jsx',
                'src/components/Button.jsx',
                'src/utils/helper.ts',
                'src/utils/math.js'
            ];
            expected = normalizeExpected(expected);
            expected.sort();
            assert.deepStrictEqual(result, expected);
        });

        it("should find all test files", async () => {
            const asyncGen = await fs.glob("**/*.{test,spec}.{js,ts,jsx,tsx}", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            let expected = ['tests/components.test.tsx', 'tests/math.test.js'];
            expected = normalizeExpected(expected);
            expected.sort();
            assert.deepStrictEqual(result, expected);
        });

        it("should find configuration files", async () => {
            const asyncGen = await fs.glob("{package.json,.gittest,config.*}", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            let expected = ['.gittest', 'config.env', 'package.json'];
            expected = normalizeExpected(expected);
            expected.sort();
            assert.deepStrictEqual(result, expected);
        });

        it("should find documentation files", async () => {
            const asyncGen = await fs.glob("**/*.md", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            let expected = ['README.md', 'docs/api.md'];
            expected = normalizeExpected(expected);
            expected.sort();
            assert.deepStrictEqual(result, expected);
        });

        it("should exclude common exclude patterns", async () => {
            const asyncGen = await fs.glob("**/*", {
                cwd: testDir,
                exclude: ['deps/**', '*.txt', '.*']
            });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            // Our implementation correctly excludes excluded files
            let expected = [
                'README.md',
                'config.env',
                'deps',
                'docs',
                'docs/api.md',
                'index.js',
                'package.json',
                'src',
                'src/App.jsx',
                'src/components',
                'src/components/Button.jsx',
                'src/utils',
                'src/utils/helper.ts',
                'src/utils/math.js',
                'tests',
                'tests/components.test.tsx',
                'tests/math.test.js'
            ];
            expected = normalizeExpected(expected);
            expected.sort();
            assert.deepStrictEqual(result, expected);
        });
    });

    // ==================== 更多边界模式测试 ====================
    describe("Advanced edge cases and boundary patterns", () => {

        it("should handle consecutive wildcards", async () => {
            // Test patterns with consecutive wildcards like **/*
            const asyncGen = await fs.glob("**/*/", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            // Should find both top-level and nested directories
            let expected = ['deps', 'docs', 'src', 'src/components', 'src/utils', 'tests'];
            expected = normalizeExpected(expected);
            expected.sort();
            assert.deepStrictEqual(result, expected);
        });

        it("should handle patterns starting with wildcards", async () => {
            // Test patterns that start with wildcards
            const asyncGen = await fs.glob("**/math.*", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            let expected = ['src/utils/math.js', 'tests/math.test.js'];
            expected = normalizeExpected(expected);
            expected.sort();
            assert.deepStrictEqual(result, expected);
        });

        it("should handle very specific nested patterns", async () => {
            // Test deeply nested specific patterns
            const asyncGen = await fs.glob("**/components/**/*.jsx", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            let expected = ['src/components/Button.jsx'];
            expected = normalizeExpected(expected);
            assert.deepStrictEqual(result, expected);
        });

        it("should handle patterns with multiple extensions", async () => {
            // Test complex extension patterns
            const asyncGen = await fs.glob("**/*.{test,spec}.{js,ts,jsx,tsx}", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            let expected = ['tests/components.test.tsx', 'tests/math.test.js'];
            expected = normalizeExpected(expected);
            expected.sort();
            assert.deepStrictEqual(result, expected);
        });

        it("should handle patterns with dots and special characters", async () => {
            // Test patterns that include dots and other special characters
            // Note: **/.*  doesn't match hidden files in Node.js by default
            const asyncGen = await fs.glob(".*", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            let expected = ['.gittest', '.hidden'];
            expected = normalizeExpected(expected);
            expected.sort();
            assert.deepStrictEqual(result, expected);
        });

        it("should handle patterns with mixed case", async () => {
            // Test case sensitivity in patterns
            const asyncGen = await fs.glob("**/README.*", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            assert.deepStrictEqual(result, ['README.md']);
        });

        it("should handle empty directory patterns", async () => {
            // Test patterns that might match empty directories
            const asyncGen = await fs.glob("empty/**", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            assert.deepStrictEqual(result, []);
        });

        it("should handle overlapping wildcard patterns", async () => {
            // Test patterns with overlapping wildcards
            const asyncGen = await fs.glob("**/*/**/*.js", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            let expected = ['deps/fake-module.js', 'src/utils/math.js', 'tests/math.test.js'];
            expected = normalizeExpected(expected);
            expected.sort();
            assert.deepStrictEqual(result, expected);
        });

        it("should handle patterns with trailing wildcards", async () => {
            // Test patterns ending with various wildcard combinations
            const asyncGen = await fs.glob("src/**", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            let expected = [
                'src',
                'src/App.jsx',
                'src/components',
                'src/components/Button.jsx',
                'src/utils',
                'src/utils/helper.ts',
                'src/utils/math.js'
            ];
            expected = normalizeExpected(expected);
            expected.sort();
            assert.deepStrictEqual(result, expected);
        });

        it("should handle patterns with leading wildcards", async () => {
            // Test patterns starting with various wildcard combinations
            const asyncGen = await fs.glob("**/src/**/*.js", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            let expected = ['src/utils/math.js'];
            expected = normalizeExpected(expected);
            assert.deepStrictEqual(result, expected);
        });

        it("should handle very deep nesting patterns", async () => {
            // Test patterns that go very deep
            const asyncGen = await fs.glob("**/**/**/*.jsx", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            let expected = ['src/App.jsx', 'src/components/Button.jsx'];
            expected = normalizeExpected(expected);
            expected.sort();
            assert.deepStrictEqual(result, expected);
        });

        it("should handle alternating wildcard patterns", async () => {
            // Test patterns with alternating wildcards and literals
            const asyncGen = await fs.glob("*/utils/*.js", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            let expected = ['src/utils/math.js'];
            expected = normalizeExpected(expected);
            assert.deepStrictEqual(result, expected);
        });

        it("should handle patterns with single and double wildcards mixed", async () => {
            // Test mixing * and ** in complex ways
            const asyncGen = await fs.glob("*/**/Button.*", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            let expected = ['src/components/Button.jsx'];
            expected = normalizeExpected(expected);
            assert.deepStrictEqual(result, expected);
        });

        it("should handle patterns with multiple question marks", async () => {
            // Test multiple single-character wildcards
            const asyncGen = await fs.glob("??*.txt", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            let expected = ['app.txt', 'error.txt', 'read.txt'];
            expected = normalizeExpected(expected);
            expected.sort();
            assert.deepStrictEqual(result, expected);
        });

        it("should handle patterns with bracket ranges", async () => {
            // Test character ranges in brackets
            const asyncGen = await fs.glob("**/[a-m]*.js", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            let expected = ['index.js', 'deps/fake-module.js', 'src/utils/math.js', 'tests/math.test.js'];
            expected = normalizeExpected(expected);
            expected.sort();
            assert.deepStrictEqual(result, expected);
        });

        it("should handle patterns with negated bracket ranges", async () => {
            // Test negated character ranges
            const asyncGen = await fs.glob("**/[!a-m]*.js", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            // Should exclude files starting with a-m
            assert.deepStrictEqual(result, []);
        });

        it("should handle patterns with escaped special characters", async () => {
            // Test escaped wildcards and special characters
            // Note: This tests literal matching of special characters
            const asyncGen = await fs.glob("package\\*.json", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            // Should not match package.json since * is escaped
            assert.deepStrictEqual(result, []);
        });

        it("should handle patterns with very long names", async () => {
            // Test patterns matching files with long names
            const asyncGen = await fs.glob("**/components.test.*", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            let expected = ['tests/components.test.tsx'];
            expected = normalizeExpected(expected);
            assert.deepStrictEqual(result, expected);
        });

        it("should handle patterns with unicode characters", async () => {
            // Test patterns that might contain unicode
            const asyncGen = await fs.glob("**/*", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            // Should work normally and include all files
            assert.ok(result.length > 0);
            assert.ok(result.includes('README.md'));
        });

        it("should handle patterns with relative path components", async () => {
            // Test patterns with . and .. components
            const asyncGen = await fs.glob("./src/**/*.js", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            let expected = ['src/utils/math.js'];
            expected = normalizeExpected(expected);
            assert.deepStrictEqual(result, expected);
        });

        it("should handle patterns ending with specific extensions", async () => {
            // Test patterns that end with very specific extensions
            const asyncGen = await fs.glob("**/*.test.js", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            let expected = ['tests/math.test.js'];
            expected = normalizeExpected(expected);
            assert.deepStrictEqual(result, expected);
        });

        it("should handle patterns with both inclusion and exclusion", async () => {
            // Test complex patterns with both positive and negative matching
            const asyncGen = await fs.glob("**/*.{js,jsx}", {
                cwd: testDir,
                exclude: ['**/test*', '**/spec*']
            });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            let expected = [
                'index.js',
                'deps/fake-module.js',
                'src/App.jsx',
                'src/components/Button.jsx',
                'src/utils/math.js'
            ];
            expected = normalizeExpected(expected);
            expected.sort();
            assert.deepStrictEqual(result, expected);
        });

        it("should handle patterns with multiple brace groups", async () => {
            // Test multiple brace expansion groups
            const asyncGen = await fs.glob("**/*.{js,jsx}.{bak,backup}", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            // Should match files with double extensions (none exist in test data)
            assert.deepStrictEqual(result, []);
        });

        it("should handle patterns with nested braces", async () => {
            // Test nested brace expansions
            const asyncGen = await fs.glob("**/{src,tests}/**/*.{js,jsx}", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            let expected = [
                'src/App.jsx',
                'src/components/Button.jsx',
                'src/utils/math.js',
                'tests/math.test.js'
            ];
            expected = normalizeExpected(expected);
            expected.sort();
            assert.deepStrictEqual(result, expected);
        });

        it("should handle patterns matching only directories", async () => {
            // Test patterns that should only match directories
            const asyncGen = await fs.glob("**/", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            // Should include all directory paths
            assert.ok(result.includes('src'));
            assert.ok(result.includes('tests'));
            assert.ok(result.includes(normalizeExpected('src/components')));
            assert.ok(result.includes(normalizeExpected('src/utils')));
            // Should not include files
            assert.ok(!result.includes('index.js'));
            assert.ok(!result.includes('README.md'));
        });

        it("should handle patterns with zero-length matches", async () => {
            // Test patterns that might create zero-length matches
            const asyncGen = await fs.glob("**/*{,.*}", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            // Should include both regular files and dotfiles
            assert.ok(result.length > 0);
            assert.ok(result.includes('README.md'));
            // Note: This pattern doesn't match dotfiles in Node.js
        });

        it("should handle patterns with redundant wildcards", async () => {
            // Test patterns with redundant wildcard sequences
            const asyncGen = await fs.glob("**/**/*.js", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            // Should work the same as **/*.js
            let expected = [
                'index.js',
                'deps/fake-module.js',
                'src/utils/math.js',
                'tests/math.test.js'
            ];
            expected = normalizeExpected(expected);
            expected.sort();
            assert.deepStrictEqual(result, expected);
        });

        it("should handle patterns with partial matches", async () => {
            // Test patterns that create partial matches
            const asyncGen = await fs.glob("**/ma*.js", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            let expected = ['src/utils/math.js', 'tests/math.test.js'];
            expected = normalizeExpected(expected);
            expected.sort();
            assert.deepStrictEqual(result, expected);
        });

        it("should handle patterns with extreme nesting", async () => {
            // Test very deeply nested patterns
            const asyncGen = await fs.glob("**/**/**/**/*.jsx", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            // Should still find the JSX files regardless of excessive nesting
            let expected = ['src/App.jsx', 'src/components/Button.jsx'];
            expected = normalizeExpected(expected);
            expected.sort();
            assert.deepStrictEqual(result, expected);
        });

        it("should handle absolute patterns with extreme complexity", async () => {
            // Test very complex absolute patterns
            const testFilesDir = path.join(__dirname, 'fs_files');
            const absolutePattern = path.join(testFilesDir, '**', '{src,tests}', '**', '*.{js,jsx,ts,tsx}');

            const asyncGen = await fs.glob(absolutePattern);
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();

            // Should find files in both src and tests directories
            const expectedResults = [
                path.join(testFilesDir, 'src', 'App.jsx'),
                path.join(testFilesDir, 'src', 'components', 'Button.jsx'),
                path.join(testFilesDir, 'src', 'utils', 'helper.ts'),
                path.join(testFilesDir, 'src', 'utils', 'math.js'),
                path.join(testFilesDir, 'tests', 'components.test.tsx'),
                path.join(testFilesDir, 'tests', 'math.test.js')
            ].sort();
            assert.deepStrictEqual(result, expectedResults);
        });

        it("should handle patterns with boundary conditions", async () => {
            // Test patterns at filesystem boundaries
            const asyncGen = await fs.glob("**", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            // Should include everything
            assert.ok(result.length > 20);
            assert.ok(result.includes('src'));
            assert.ok(result.includes(normalizeExpected('src/App.jsx')));
            // Note: ** pattern doesn't include hidden files by default in Node.js
        });
    });

    // ==================== withFileTypes 选项测试 ====================
    describe("withFileTypes option", () => {

        it("should return DirEntry objects when withFileTypes is true", async () => {
            const asyncGen = await fs.glob("*.js", { cwd: testDir, withFileTypes: true });
            const result = await asyncGeneratorToArray(asyncGen);

            assert.ok(result.length > 0);

            // Check that entries are DirEntry objects
            result.forEach(entry => {
                assert.ok(entry.name, "Entry should have name property");
                assert.ok(entry.parentPath, "Entry should have parentPath property");
                assert.equal(typeof entry.isFile, 'function', "Entry should have isFile method");
                assert.equal(typeof entry.isDirectory, 'function', "Entry should have isDirectory method");
                assert.equal(typeof entry.isSymbolicLink, 'function', "Entry should have isSymbolicLink method");
            });
        });

        it("should correctly identify files vs directories with withFileTypes", async () => {
            const asyncGen = await fs.glob("*", { cwd: testDir, withFileTypes: true });
            const result = await asyncGeneratorToArray(asyncGen);

            // Find a known file
            const indexJs = result.find(entry => entry.name === 'index.js');
            assert.ok(indexJs, "Should find index.js");
            assert.ok(indexJs.isFile(), "index.js should be a file");
            assert.ok(!indexJs.isDirectory(), "index.js should not be a directory");

            // Find a known directory
            const srcDir = result.find(entry => entry.name === 'src');
            assert.ok(srcDir, "Should find src directory");
            assert.ok(srcDir.isDirectory(), "src should be a directory");
            assert.ok(!srcDir.isFile(), "src should not be a file");
        });

        it("should return DirEntry objects with recursive glob", async () => {
            const asyncGen = await fs.glob("**/*.js", { cwd: testDir, withFileTypes: true });
            const result = await asyncGeneratorToArray(asyncGen);

            assert.ok(result.length > 0, "Should find some .js files");

            // All results should be files (not directories)
            result.forEach(entry => {
                assert.ok(entry.isFile(), `${entry.name} should be a file`);
                assert.ok(!entry.isDirectory(), `${entry.name} should not be a directory`);
                assert.ok(entry.name.endsWith('.js'), `${entry.name} should end with .js`);
            });
        });

        it("should return DirEntry objects for multiple patterns", async () => {
            const asyncGen = await fs.glob(["*.js", "*.txt"], { cwd: testDir, withFileTypes: true });
            const result = await asyncGeneratorToArray(asyncGen);

            assert.ok(result.length >= 4, "Should find at least 4 files (.js and .txt)");

            // Check all are files
            result.forEach(entry => {
                assert.ok(entry.isFile(), `${entry.name} should be a file`);
                assert.ok(
                    entry.name.endsWith('.js') || entry.name.endsWith('.txt'),
                    `${entry.name} should end with .js or .txt`
                );
            });
        });

        it("should have correct parentPath in DirEntry objects", async () => {
            const asyncGen = await fs.glob("src/*.jsx", { cwd: testDir, withFileTypes: true });
            const result = await asyncGeneratorToArray(asyncGen);

            if (result.length > 0) {
                const entry = result[0];
                const expectedParentPath = path.join(testDir, 'src');
                assert.equal(
                    path.normalize(entry.parentPath),
                    path.normalize(expectedParentPath),
                    "Parent path should be correct"
                );
            }
        });

        it("should work with exclude option and withFileTypes", async () => {
            const asyncGen = await fs.glob("**/*.js", {
                cwd: testDir,
                withFileTypes: true,
                exclude: ["**/node_modules/**"]
            });
            const result = await asyncGeneratorToArray(asyncGen);

            // Check that all results are DirEntry objects
            result.forEach(entry => {
                assert.ok(entry.isFile(), "Should be a file");
                assert.ok(!entry.name.includes('node_modules'), "Should not include node_modules");
            });
        });
    });
});


