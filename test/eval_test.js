var { describe, it, before, after } = require('node:test');
var assert = require('assert');

var child_process = require('child_process');
var path = require('path');

describe('eval (-e)', () => {
    var cmd = process.execPath;

    function runEval(code, options) {
        return child_process.execFileSync(cmd, ['-e', code], {
            encoding: 'utf8',
            ...options
        }).trim();
    }

    describe('basic', () => {
        it('simple expression', () => {
            var result = runEval('console.log("hello world")');
            assert.equal(result, 'hello world');
        });

        it('arithmetic', () => {
            var result = runEval('console.log(1 + 2)');
            assert.equal(result, '3');
        });

        it('multiline code', () => {
            var result = runEval('var a = 1; var b = 2; console.log(a + b)');
            assert.equal(result, '3');
        });
    });

    describe('require', () => {
        it('require builtin module', () => {
            var result = runEval("const fs = require('fs'); console.log(typeof fs.readFileSync)");
            assert.equal(result, 'function');
        });

        it('require path module', () => {
            var result = runEval("const path = require('path'); console.log(path.join('a', 'b'))");
            assert.equal(result, path.join('a', 'b'));
        });
    });

    describe('dynamic import', () => {
        it('import builtin module', () => {
            var result = runEval("import('fs').then(fs => console.log(typeof fs.readFileSync))");
            assert.equal(result, 'function');
        });

        it('import path module', () => {
            var result = runEval("import('path').then(path => console.log(path.join('a', 'b')))");
            assert.equal(result, path.join('a', 'b'));
        });
    });

    describe('static import', () => {
        it('import default', () => {
            var result = runEval("import fs from 'fs'; console.log(typeof fs.readFileSync)");
            assert.equal(result, 'function');
        });

        it('import named', () => {
            var result = runEval("import { join } from 'path'; console.log(join('a', 'b'))");
            assert.equal(result, path.join('a', 'b'));
        });

        it('import namespace', () => {
            var result = runEval("import * as fs from 'fs'; console.log(typeof fs.readFileSync)");
            assert.equal(result, 'function');
        });

        it('import multiple modules', () => {
            var result = runEval("import fs from 'fs'; import path from 'path'; console.log(typeof fs.readFileSync, typeof path.join)");
            assert.equal(result, 'function function');
        });
    });

    describe('mixed usage', () => {
        // fibjs supports require() in ESM mode, but Node.js doesn't
        var isFibjs = process.versions.fibjs !== undefined;

        (isFibjs ? it : it.skip)('require and static import together', () => {
            var result = runEval("import fs from 'fs'; const path = require('path'); console.log(typeof fs.readFileSync, typeof path.join)");
            assert.equal(result, 'function function');
        });

        it('static import and dynamic import together', () => {
            var result = runEval("import fs from 'fs'; import('path').then(path => console.log(typeof fs.readFileSync, typeof path.join))");
            assert.equal(result, 'function function');
        });
    });

    describe('error handling', () => {
        it('syntax error', () => {
            assert.throws(() => {
                runEval('const a =');
            });
        });

        it('runtime error', () => {
            assert.throws(() => {
                runEval('throw new Error("test error")');
            });
        });

        it('module not found', () => {
            assert.throws(() => {
                runEval("import foo from 'nonexistent_module_12345'");
            });
        });
    });

    describe('import.meta', () => {
        it('import.meta is object', () => {
            var result = runEval("console.log(typeof import.meta)");
            assert.equal(result, 'object');
        });
    });

    describe('global variables', () => {
        it('can access global console', () => {
            var result = runEval("console.log(typeof console.log)");
            assert.equal(result, 'function');
        });

        it('can access global process', () => {
            var result = runEval("console.log(typeof process.version)");
            assert.equal(result, 'string');
        });

        it('can override global with const', () => {
            var result = runEval("const console = {log: () => {}}; require('node:console').log(typeof console.log)");
            assert.equal(result, 'function');
        });

        it('can override global with let', () => {
            var result = runEval("let console = {log: () => {}}; require('node:console').log(typeof console.log)");
            assert.equal(result, 'function');
        });
    });

    describe('top level await', () => {
        it('simple await', () => {
            var result = runEval("const result = await Promise.resolve(42); console.log(result)");
            assert.equal(result, '42');
        });

        it('await with import', () => {
            var result = runEval("const fs = await import('fs'); console.log(typeof fs.readFileSync)");
            assert.equal(result, 'function');
        });
    });

    describe('arguments', () => {
        it('access process.argv', () => {
            // In fibjs, process.argv[0] is the executable path
            // process.argv[1...] are the arguments after -e code
            var result = child_process.execFileSync(cmd, ['-e', 'console.log(process.argv.slice(1).join(","))', 'arg1', 'arg2'], {
                encoding: 'utf8'
            }).trim();
            assert.equal(result, 'arg1,arg2');
        });
    });

    describe('CommonJS-style variables', () => {
        it('__dirname is defined', () => {
            var result = runEval('console.log(typeof __dirname)');
            assert.equal(result, 'string');
        });

        it('__filename is defined', () => {
            var result = runEval('console.log(typeof __filename)');
            assert.equal(result, 'string');
        });

        it('__dirname equals current working directory', () => {
            var result = runEval('console.log(__dirname)');
            var cwd = process.cwd();
            assert.equal(result, cwd);
        });

        it('__filename equals [eval]', () => {
            var result = runEval('console.log(__filename)');
            var expected = path.join(process.cwd(), '[eval]');
            assert.equal(result, expected);
        });

        it('__filename is absolute path', () => {
            var result = runEval("const path = require('path'); console.log(path.isAbsolute(__filename))");
            assert.equal(result, 'true');
        });

        it('can use __dirname in path operations', () => {
            var result = runEval("const path = require('path'); console.log(path.join(__dirname, 'test.js').endsWith('test.js'))");
            assert.equal(result, 'true');
        });

        it('can override __dirname with let', () => {
            var result = runEval("let __dirname = '/custom'; console.log(__dirname)");
            assert.equal(result, '/custom');
        });

        it('can override __filename with const', () => {
            var result = runEval("const __filename = 'custom.js'; console.log(__filename)");
            assert.equal(result, 'custom.js');
        });
    });
});
