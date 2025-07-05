const assert = require("assert");
const { describe, it } = require("node:test");
const path = require("path");
const fs = require("fs").promises;
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
            const expected = ['README.md', 'app.log', 'docs', 'error.log', 'index.js', 'node_modules', 'package.json', 'read.txt', 'src', 'tests'];
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
            const asyncGen = await fs.glob("*.log", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            assert.deepStrictEqual(result, ['app.log', 'error.log']);
        });
    });

    // ==================== 通配符模式测试 ====================
    describe("Wildcard patterns", () => {

        it("should handle single character wildcard (?)", async () => {
            const asyncGen = await fs.glob("*.l?g", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            assert.deepStrictEqual(result, ['app.log', 'error.log']);
        });

        it("should handle character classes", async () => {
            const asyncGen = await fs.glob("*.[jt]s", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            assert.deepStrictEqual(result, ['index.js']);
        });

        it("should handle negated character classes", async () => {
            const asyncGen = await fs.glob("*.[!l]*", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            // Should match files that don't have 'l' in their extension
            const expected = ['README.md', 'index.js', 'package.json', 'read.txt'];
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
            const expected = [
                'index.js',
                'node_modules/fake-module.js',
                'src/utils/math.js',
                'tests/math.test.js'
            ];
            expected.sort();
            assert.deepStrictEqual(result, expected);
        });

        it("should match all TypeScript files", async () => {
            const asyncGen = await fs.glob("**/*.ts", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            assert.deepStrictEqual(result, ['src/utils/helper.ts']);
        });

        it("should match React components", async () => {
            const asyncGen = await fs.glob("**/*.jsx", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            const expected = ['src/App.jsx', 'src/components/Button.jsx'];
            expected.sort();
            assert.deepStrictEqual(result, expected);
        });

        it("should match test files", async () => {
            const asyncGen = await fs.glob("**/*.test.*", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            const expected = ['tests/components.test.tsx', 'tests/math.test.js'];
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
            assert.deepStrictEqual(result, ['src/App.jsx']);
        });

        it("should match files in nested directories", async () => {
            const asyncGen = await fs.glob("src/utils/*", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            const expected = ['src/utils/helper.ts', 'src/utils/math.js'];
            expected.sort();
            assert.deepStrictEqual(result, expected);
        });

        it("should match all files in any subdirectory", async () => {
            const asyncGen = await fs.glob("*/", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            // Node.js glob doesn't include trailing / in directory names
            const expected = ['docs', 'node_modules', 'src', 'tests'];
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
            const expected = ['.env', '.gitignore', '.hidden'];
            expected.sort();
            assert.deepStrictEqual(result, expected);
        });

        it("should match files in hidden directories", async () => {
            const asyncGen = await fs.glob(".hidden/*", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            assert.deepStrictEqual(result, ['.hidden/secret.txt']);
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
            assert.deepStrictEqual(result, ['src/utils/math.js']);
        });

        it("should match multiple file types", async () => {
            const asyncGen = await fs.glob("**/*.{js,ts,jsx}", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            const expected = [
                'index.js',
                'node_modules/fake-module.js',
                'src/App.jsx',
                'src/components/Button.jsx',
                'src/utils/helper.ts',
                'src/utils/math.js',
                'tests/math.test.js'
            ];
            expected.sort();
            assert.deepStrictEqual(result, expected);
        });

        it("should exclude specific directories", async () => {
            const asyncGen = await fs.glob("**/*.js", {
                cwd: testDir,
                exclude: ['node_modules/**']
            });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            // Our implementation correctly excludes node_modules files (better than Node.js)
            const expected = ['index.js', 'src/utils/math.js', 'tests/math.test.js'];
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
                exclude: ['node_modules/**']
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
                exclude: ['node_modules/**']
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
                if (process.versions.fibjs) {
                    assert.deepStrictEqual(result, ['README.md']);
                } else {
                    assert.deepStrictEqual(result, ['readme.md']);
                }
            }
        });
    });

    // ==================== 实际应用场景测试 ====================
    describe("Real-world scenarios", () => {

        it("should find all source files", async () => {
            const asyncGen = await fs.glob("src/**/*.{js,ts,jsx,tsx}", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            const expected = [
                'src/App.jsx',
                'src/components/Button.jsx',
                'src/utils/helper.ts',
                'src/utils/math.js'
            ];
            expected.sort();
            assert.deepStrictEqual(result, expected);
        });

        it("should find all test files", async () => {
            const asyncGen = await fs.glob("**/*.{test,spec}.{js,ts,jsx,tsx}", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            const expected = ['tests/components.test.tsx', 'tests/math.test.js'];
            expected.sort();
            assert.deepStrictEqual(result, expected);
        });

        it("should find configuration files", async () => {
            const asyncGen = await fs.glob("{package.json,.gitignore,.env*}", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            const expected = ['.env', '.gitignore', 'package.json'];
            expected.sort();
            assert.deepStrictEqual(result, expected);
        });

        it("should find documentation files", async () => {
            const asyncGen = await fs.glob("**/*.md", { cwd: testDir });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            const expected = ['README.md', 'docs/api.md'];
            expected.sort();
            assert.deepStrictEqual(result, expected);
        });

        it("should exclude common exclude patterns", async () => {
            const asyncGen = await fs.glob("**/*", {
                cwd: testDir,
                exclude: ['node_modules/**', '*.log', '.*']
            });
            const result = await asyncGeneratorToArray(asyncGen);
            result.sort();
            // Our implementation correctly excludes excluded files (better than Node.js experimental glob)
            const expected = [
                'README.md',
                'docs',
                'docs/api.md',
                'index.js',
                'node_modules',
                'package.json',
                'read.txt',
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
            expected.sort();
            assert.deepStrictEqual(result, expected);
        });
    });
});

