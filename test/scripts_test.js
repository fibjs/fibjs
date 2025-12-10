var { describe, it, before, after } = require('node:test');
var assert = require('assert');
const child_process = require('child_process');
const path = require('path');

// Detect if running in fibjs or nodejs
const isFibjs = typeof process !== 'undefined' && process.versions && process.versions.fibjs;
const isWindows = process.platform === 'win32';

function runScript(scriptName, testDir, args = []) {
    if (isFibjs) {
        const cmdArgs = [scriptName, ...args];
        return child_process.spawnSync(process.execPath, cmdArgs, {
            cwd: testDir,
            encoding: 'utf8'
        });
    } else {
        // Use npm run for nodejs
        const npmCommand = process.platform === 'win32' ? 'npm.cmd' : 'npm';
        const cmdArgs = ['run', scriptName, '--', ...args];
        return child_process.spawnSync(npmCommand, cmdArgs, {
            cwd: testDir,
            encoding: 'utf8',
            shell: process.platform === 'win32'
        });
    }
}

describe("scripts execution test", () => {
    const basicTestDir = path.join(__dirname, 'scripts_files/basic');

    describe("basic scripts", () => {
        it("should run simple echo command", () => {
            const result = runScript('echo', basicTestDir);
            assert.equal(result.status, 0, `Command failed with stderr: ${result.stderr}`);
            assert.ok(result.stdout.includes('hello'), `Expected 'hello' in stdout, got: ${result.stdout}`);
        });

        it("should support pipe in scripts", () => {
            const result = runScript('pipe', basicTestDir);
            assert.equal(result.status, 0, `Command failed with stderr: ${result.stderr}`);
            assert.ok(result.stdout.includes('hello'), `Expected 'hello' in stdout, got: ${result.stdout}`);
        });

        it("should support && in scripts", () => {
            const result = runScript('multi', basicTestDir);
            assert.equal(result.status, 0, `Command failed with stderr: ${result.stderr}`);
            assert.ok(result.stdout.includes('hello'), `Expected 'hello' in stdout, got: ${result.stdout}`);
            // Verify && actually works - both commands should execute
            assert.ok(result.stdout.includes('world'), `Expected 'world' in stdout (second command should run). Got: ${result.stdout}`);
        });

        it("should pass arguments to script", () => {
            const result = runScript('echo-args', basicTestDir, ['foo', 'bar']);
            assert.equal(result.status, 0, `Command failed with stderr: ${result.stderr}`);
            assert.ok(result.stdout.includes('foo'), `Expected 'foo' in stdout, got: ${result.stdout}`);
            assert.ok(result.stdout.includes('bar'), `Expected 'bar' in stdout, got: ${result.stdout}`);
        });

        // Skip on Windows - Unix shell syntax (FOO=bar && echo $FOO)
        (isWindows ? it.skip : it)("should support inline environment variables", () => {
            const result = runScript('env-inline', basicTestDir);
            assert.equal(result.status, 0, `Command failed with stderr: ${result.stderr}`);
            assert.ok(result.stdout.includes('bar'), `Expected 'bar' in stdout, got: ${result.stdout}`);
        });

        // Skip on Windows - Unix shell syntax (FOO=bar node ...)
        (isWindows ? it.skip : it)("should replace 'node' with inline env vars", () => {
            const result = runScript('node-check-fibjs-env', basicTestDir);
            assert.equal(result.status, 0, `Command failed with stderr: ${result.stderr}`);
            // Check the runtime matches expectation
            const expectedOutput = `fibjs:${isFibjs ? 'true' : 'false'}`;
            assert.ok(result.stdout.includes(expectedOutput), 
                `Expected '${expectedOutput}' in stdout (running in ${isFibjs ? 'fibjs' : 'node'}), got: ${result.stdout}`);
        });

    });

    describe("node command replacement", () => {
        const basicTestDir = path.join(__dirname, 'scripts_files/basic');

        // Expected runtime: fibjs in fibjs, node in node
        const expectedRuntime = isFibjs ? 'fibjs' : 'node';
        // Script output: 'fibjs:true' or 'fibjs:false'
        const expectedFibjsFlag = isFibjs ? 'true' : 'false';

        it("should replace 'node' with current executable (simple)", () => {
            const result = runScript('node-check-fibjs', basicTestDir);
            assert.equal(result.status, 0, `Command failed with stderr: ${result.stderr}`);
            // Check the runtime matches expectation
            const expectedOutput = `fibjs:${expectedFibjsFlag}`;
            assert.ok(result.stdout.includes(expectedOutput), 
                `Expected '${expectedOutput}' in stdout (running in ${expectedRuntime}), got: ${result.stdout}`);
        });

        it("should replace 'node' with pipe", () => {
            const result = runScript('node-check-fibjs-pipe', basicTestDir);
            assert.equal(result.status, 0, `Command failed with stderr: ${result.stderr}`);
            // Check the runtime matches expectation
            const expectedOutput = `fibjs:${expectedFibjsFlag}`;
            assert.ok(result.stdout.includes(expectedOutput), 
                `Expected '${expectedOutput}' in stdout (running in ${expectedRuntime}), got: ${result.stdout}`);
        });

        it("should replace ALL 'node' commands with &&", () => {
            const result = runScript('node-check-fibjs-multi', basicTestDir);
            assert.equal(result.status, 0, `Command failed with stderr: ${result.stderr}`);
            // Check BOTH commands use the expected runtime
            const expected1 = `fibjs1:${expectedFibjsFlag}`;
            const expected2 = `fibjs2:${expectedFibjsFlag}`;
            assert.ok(result.stdout.includes(expected1), 
                `Expected '${expected1}' in stdout (first command in ${expectedRuntime}), got: ${result.stdout}`);
            assert.ok(result.stdout.includes(expected2), 
                `Expected '${expected2}' in stdout (second command in ${expectedRuntime}), got: ${result.stdout}`);
        });

        it("should replace 'node' with || operator (only first runs on success)", () => {
            const result = runScript('node-check-or', basicTestDir);
            assert.equal(result.status, 0, `Command failed with stderr: ${result.stderr}`);
            // Only first command should run (it succeeds, so || short-circuits)
            const expected1 = `fibjs1:${expectedFibjsFlag}`;
            assert.ok(result.stdout.includes(expected1), 
                `Expected '${expected1}' in stdout, got: ${result.stdout}`);
            // Second command should NOT run - check for actual output, not script echo
            // Count occurrences of the actual output pattern (not in the npm script echo line)
            const outputLines = result.stdout.split('\n').filter(line => 
                !line.includes('> node') && !line.includes('> scripts-test'));
            const fibjs2InOutput = outputLines.some(line => line.includes('fibjs2:'));
            assert.ok(!fibjs2InOutput, 
                `Expected second command NOT to run (|| short-circuit), got: ${result.stdout}`);
        });

        // Skip on Windows - ; is not a command separator in cmd.exe
        (isWindows ? it.skip : it)("should replace ALL 'node' commands with ; separator", () => {
            const result = runScript('node-check-semicolon', basicTestDir);
            assert.equal(result.status, 0, `Command failed with stderr: ${result.stderr}`);
            // Check BOTH commands use the expected runtime
            const expected1 = `fibjs1:${expectedFibjsFlag}`;
            const expected2 = `fibjs2:${expectedFibjsFlag}`;
            assert.ok(result.stdout.includes(expected1), 
                `Expected '${expected1}' in stdout, got: ${result.stdout}`);
            assert.ok(result.stdout.includes(expected2), 
                `Expected '${expected2}' in stdout, got: ${result.stdout}`);
        });

        it("should replace 'node' inside parentheses", () => {
            const result = runScript('node-check-parens', basicTestDir);
            assert.equal(result.status, 0, `Command failed with stderr: ${result.stderr}`);
            const expectedOutput = `fibjs:${expectedFibjsFlag}`;
            assert.ok(result.stdout.includes(expectedOutput), 
                `Expected '${expectedOutput}' in stdout, got: ${result.stdout}`);
        });

        it("should NOT replace 'node' inside quoted strings", () => {
            const result = runScript('node-quoted-no-replace', basicTestDir);
            assert.equal(result.status, 0, `Command failed with stderr: ${result.stderr}`);
            // The string "node command test" should appear in output unchanged
            assert.ok(result.stdout.includes('node command test'), 
                `Expected 'node command test' in stdout (quoted string preserved), got: ${result.stdout}`);
            // And it should run with the expected runtime
            const expectedOutput = `${expectedFibjsFlag}`;
            assert.ok(result.stdout.includes(expectedOutput), 
                `Expected '${expectedOutput}' in stdout, got: ${result.stdout}`);
        });

        // Skip on Windows - Unix shell syntax (FOO=bar BAZ=qux node ...)
        (isWindows ? it.skip : it)("should replace 'node' with multiple env var assignments", () => {
            const result = runScript('node-multi-env', basicTestDir);
            assert.equal(result.status, 0, `Command failed with stderr: ${result.stderr}`);
            const expectedOutput = `fibjs:${expectedFibjsFlag}`;
            assert.ok(result.stdout.includes(expectedOutput), 
                `Expected '${expectedOutput}' in stdout, got: ${result.stdout}`);
            assert.ok(result.stdout.includes('FOO:bar'), 
                `Expected 'FOO:bar' in stdout (env var should be set), got: ${result.stdout}`);
        });

        // Skip on Windows - Unix shell syntax
        (isWindows ? it.skip : it)("should replace 'node' with env var containing equals sign", () => {
            const result = runScript('node-env-with-equals', basicTestDir);
            assert.equal(result.status, 0, `Command failed with stderr: ${result.stderr}`);
            const expectedOutput = `fibjs:${expectedFibjsFlag}`;
            assert.ok(result.stdout.includes(expectedOutput), 
                `Expected '${expectedOutput}' in stdout, got: ${result.stdout}`);
        });

        it("should replace 'node' in complex pipe chain", () => {
            const result = runScript('node-complex-pipe', basicTestDir);
            assert.equal(result.status, 0, `Command failed with stderr: ${result.stderr}`);
            const expectedOutput = `fibjs:${expectedFibjsFlag}`;
            assert.ok(result.stdout.includes(expectedOutput), 
                `Expected '${expectedOutput}' in stdout, got: ${result.stdout}`);
            assert.ok(result.stdout.includes('start'), 
                `Expected 'start' in stdout, got: ${result.stdout}`);
        });

        it("should replace 'node' with output redirection", () => {
            const result = runScript('node-redirect', basicTestDir);
            assert.equal(result.status, 0, `Command failed with stderr: ${result.stderr}`);
            const expectedOutput = `fibjs:${expectedFibjsFlag}`;
            assert.ok(result.stdout.includes(expectedOutput), 
                `Expected '${expectedOutput}' in stdout, got: ${result.stdout}`);
        });
    });

    describe("PATH extension", () => {
        const binPathTestDir = path.join(__dirname, 'scripts_files/bin_path');

        it("should add node_modules/.bin to PATH", () => {
            const result = runScript('local-cmd', binPathTestDir);
            assert.equal(result.status, 0, `Command failed with stderr: ${result.stderr}`);
            assert.ok(result.stdout.includes('local-cmd-output'), `Expected 'local-cmd-output' in stdout, got: ${result.stdout}`);
        });
    });

    // These tests only run in fibjs - direct .bin script execution with shebang
    if (isFibjs) {
        describe(".bin script shebang handling", () => {
            const shebangTestDir = path.join(__dirname, 'scripts_files/shebang');

            // Helper to run .bin script directly (fibjs only)
            function runBinScript(scriptName, testDir) {
                return child_process.spawnSync(process.execPath, [scriptName], {
                    cwd: testDir,
                    encoding: 'utf8'
                });
            }

            it("should run node shebang script directly with fibjs", () => {
                const result = runBinScript('test-node-shebang', shebangTestDir);
                assert.equal(result.status, 0, `Command failed with stderr: ${result.stderr}`);
                // Should run with fibjs directly (not spawn node)
                assert.ok(result.stdout.includes('shebang-node:fibjs=true'), 
                    `Expected 'shebang-node:fibjs=true' (run directly by fibjs), got: ${result.stdout}`);
            });

            // Skip on Windows - no /bin/bash
            (isWindows ? it.skip : it)("should run bash shebang script via shell", () => {
                const result = runBinScript('test-bash-shebang', shebangTestDir);
                assert.equal(result.status, 0, `Command failed with stderr: ${result.stderr}`);
                // Should run via shell
                assert.ok(result.stdout.includes('shebang-bash:ok'), 
                    `Expected 'shebang-bash:ok', got: ${result.stdout}`);
            });

            // Skip on Windows - Unix shell syntax (FOO=bar cmd)
            (isWindows ? it.skip : it)("should run shebang script with inline env vars", () => {
                const result = runScript('shebang-env', shebangTestDir);
                assert.equal(result.status, 0, `Command failed with stderr: ${result.stderr}`);
                assert.ok(result.stdout.includes('shebang-env:FOO=bar,fibjs=true'), 
                    `Expected 'shebang-env:FOO=bar,fibjs=true', got: ${result.stdout}`);
            });

            it("should run shebang script with pipe", () => {
                const result = runScript('shebang-pipe', shebangTestDir);
                assert.equal(result.status, 0, `Command failed with stderr: ${result.stderr}`);
                assert.ok(result.stdout.includes('shebang-node:fibjs=true'), 
                    `Expected 'shebang-node:fibjs=true', got: ${result.stdout}`);
            });

            it("should run multiple shebang scripts with &&", () => {
                const result = runScript('shebang-multi', shebangTestDir);
                assert.equal(result.status, 0, `Command failed with stderr: ${result.stderr}`);
                assert.ok(result.stdout.includes('shebang-node:fibjs=true'), 
                    `Expected 'shebang-node:fibjs=true' (first command), got: ${result.stdout}`);
                assert.ok(result.stdout.includes('shebang-second:fibjs=true'), 
                    `Expected 'shebang-second:fibjs=true' (second command), got: ${result.stdout}`);
            });
        });
    }
});
