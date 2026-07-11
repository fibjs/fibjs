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

        it('import.meta.url is a file URL', () => {
            var result = runEval("console.log(import.meta.url)");
            assert.ok(result.startsWith('file:///'), 'import.meta.url should start with file:///');
        });

        it('import.meta.filename is absolute path ending with [eval]', () => {
            var result = runEval("console.log(import.meta.filename)");
            var expected = path.join(process.cwd(), '[eval]');
            assert.equal(result, expected);
        });

        it('import.meta.dirname equals current working directory', () => {
            var result = runEval("console.log(import.meta.dirname)");
            assert.equal(result, process.cwd());
        });

        it('import.meta.url matches filename', () => {
            var result = runEval("const { fileURLToPath } = require('url'); console.log(fileURLToPath(import.meta.url))");
            var expected = path.join(process.cwd(), '[eval]');
            assert.equal(result, expected);
        });

        it('import.meta.url available in statically imported module', () => {
            var result = child_process.execFileSync(cmd, ['-e', "import { metaUrl } from './eval_files/esm_module.mjs'; console.log(typeof metaUrl)"], {
                encoding: 'utf8',
                cwd: path.join(__dirname)
            }).trim();
            assert.equal(result, 'string');
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

    describe('require local files', () => {
        it('require relative .js file', () => {
            var result = child_process.execFileSync(cmd, ['-e', "const m = require('./eval_files/cjs_module.js'); console.log(m.value)"], {
                encoding: 'utf8',
                cwd: path.join(__dirname)
            }).trim();
            assert.equal(result, '42');
        });

        it('require relative .js file without extension', () => {
            var result = child_process.execFileSync(cmd, ['-e', "const m = require('./eval_files/cjs_module'); console.log(m.value)"], {
                encoding: 'utf8',
                cwd: path.join(__dirname)
            }).trim();
            assert.equal(result, '42');
        });

        it('require with function call', () => {
            var result = child_process.execFileSync(cmd, ['-e', "const m = require('./eval_files/cjs_module'); console.log(m.greet('World'))"], {
                encoding: 'utf8',
                cwd: path.join(__dirname)
            }).trim();
            assert.equal(result, 'Hello, World');
        });
    });

    describe('dynamic import local files', () => {
        it('dynamic import relative .js file', () => {
            // fibjs: CJS modules are exposed directly, not wrapped in { default: ... }
            var result = child_process.execFileSync(cmd, ['-e', "import('./eval_files/cjs_module.js').then(m => console.log(m.value))"], {
                encoding: 'utf8',
                cwd: path.join(__dirname)
            }).trim();
            assert.equal(result, '42');
        });

        it('dynamic import relative .mjs file', () => {
            var result = child_process.execFileSync(cmd, ['-e', "import('./eval_files/esm_module.mjs').then(m => console.log(m.value))"], {
                encoding: 'utf8',
                cwd: path.join(__dirname)
            }).trim();
            assert.equal(result, '100');
        });

        it('dynamic import relative .mts file', () => {
            var result = child_process.execFileSync(cmd, ['-e', "import('./eval_files/ts_module.mts').then(m => console.log(m.value))"], {
                encoding: 'utf8',
                cwd: path.join(__dirname)
            }).trim();
            assert.equal(result, '300');
        });
    });

    describe('static import local files', () => {
        it('static import relative .js file', () => {
            var result = child_process.execFileSync(cmd, ['-e', "import m from './eval_files/cjs_module.js'; console.log(m.value)"], {
                encoding: 'utf8',
                cwd: path.join(__dirname)
            }).trim();
            assert.equal(result, '42');
        });

        it('static import relative .mjs file', () => {
            var result = child_process.execFileSync(cmd, ['-e', "import { value } from './eval_files/esm_module.mjs'; console.log(value)"], {
                encoding: 'utf8',
                cwd: path.join(__dirname)
            }).trim();
            assert.equal(result, '100');
        });

        it('static import relative .mts file from TypeScript', () => {
            var result = child_process.execFileSync(cmd, ['-e', "import { value } from './eval_files/ts_module.mts'; console.log(value)"], {
                encoding: 'utf8',
                cwd: path.join(__dirname)
            }).trim();
            assert.equal(result, '300');
        });

        it('static import default from .mjs file', () => {
            var result = child_process.execFileSync(cmd, ['-e', "import m from './eval_files/esm_module.mjs'; console.log(m.value)"], {
                encoding: 'utf8',
                cwd: path.join(__dirname)
            }).trim();
            assert.equal(result, '200');
        });

        it('static import namespace from .mjs file', () => {
            var result = child_process.execFileSync(cmd, ['-e', "import * as m from './eval_files/esm_module.mjs'; console.log(m.value)"], {
                encoding: 'utf8',
                cwd: path.join(__dirname)
            }).trim();
            assert.equal(result, '100');
        });
    });

    // Tests for ESM package (with "type": "module" in package.json)
    describe('require local files in ESM package', () => {
        var esmPkgDir = path.join(__dirname, 'eval_files', 'esm_pkg');

        it('require relative .cjs file in ESM package', () => {
            var result = child_process.execFileSync(cmd, ['-e', "const m = require('./cjs_module.cjs'); console.log(m.value)"], {
                encoding: 'utf8',
                cwd: esmPkgDir
            }).trim();
            assert.equal(result, '42');
        });

        it('require ESM .js file in ESM package should succeed (require(esm))', () => {
            // require(esm) support (Node.js v22.12+ behavior): ESM-syntax .js files
            // can now be required. Named exports are exposed alongside `default`.
            var result = child_process.execFileSync(cmd, ['-e', "const m = require('./esm_module.js'); console.log(m.value)"], {
                encoding: 'utf8',
                cwd: esmPkgDir
            }).trim();
            assert.equal(result, '100');
        });
    });

    describe('dynamic import local files in ESM package', () => {
        var esmPkgDir = path.join(__dirname, 'eval_files', 'esm_pkg');

        it('dynamic import relative .cjs file in ESM package', () => {
            // fibjs: CJS modules are exposed directly, not wrapped in { default: ... }
            var result = child_process.execFileSync(cmd, ['-e', "import('./cjs_module.cjs').then(m => console.log(m.value))"], {
                encoding: 'utf8',
                cwd: esmPkgDir
            }).trim();
            assert.equal(result, '42');
        });

        it('dynamic import relative .js file in ESM package', () => {
            var result = child_process.execFileSync(cmd, ['-e', "import('./esm_module.js').then(m => console.log(m.value))"], {
                encoding: 'utf8',
                cwd: esmPkgDir
            }).trim();
            assert.equal(result, '100');
        });
    });

    describe('static import local files in ESM package', () => {
        var esmPkgDir = path.join(__dirname, 'eval_files', 'esm_pkg');

        it('static import relative .cjs file in ESM package', () => {
            var result = child_process.execFileSync(cmd, ['-e', "import m from './cjs_module.cjs'; console.log(m.value)"], {
                encoding: 'utf8',
                cwd: esmPkgDir
            }).trim();
            assert.equal(result, '42');
        });

        it('static import relative .js file in ESM package', () => {
            var result = child_process.execFileSync(cmd, ['-e', "import { value } from './esm_module.js'; console.log(value)"], {
                encoding: 'utf8',
                cwd: esmPkgDir
            }).trim();
            assert.equal(result, '100');
        });

        it('static import default from .js file in ESM package', () => {
            var result = child_process.execFileSync(cmd, ['-e', "import m from './esm_module.js'; console.log(m.value)"], {
                encoding: 'utf8',
                cwd: esmPkgDir
            }).trim();
            assert.equal(result, '200');
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
