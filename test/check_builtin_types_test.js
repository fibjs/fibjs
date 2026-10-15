// `fibjs --check` built-in types regression tests.
//
// The checker carries the built-in types of the runtime (the map built by
// tools/gen_builtin_types.js, embedded as `internal/fibjs-types`), so a
// project without node_modules must know http/fs/... and the fibjs globals,
// user errors must still be reported, and --no-builtin-types must fall back
// to the behaviour without the embedded types.
//
// @types/node never joins a checked program (a dependency's directive, a
// `types` entry, or the default type-roots scan alike); --no-builtin-types is
// the way out for a node-targeted project.
//
// Every scenario runs in a temp directory: tsc searches tsconfig.json upwards
// and the repository root has one. The suite spawns the binary through
// process.execPath, so it is fibjs only.

var { describe, it, before, after } = require('node:test');
var assert = require('assert');
var fs = require('fs');
var os = require('os');
var path = require('path');
var child_process = require('child_process');

const isFibjs = typeof process !== 'undefined' && process.versions && process.versions.fibjs;

function runCheck(cwd, args) {
    var r = child_process.spawnSync(process.execPath, ['--check'].concat(args || []), {
        encoding: 'utf8',
        cwd: cwd,
        input: ''
    });

    return { code: r.status, stdout: r.stdout || '', stderr: r.stderr || '' };
}

function errors(text) {
    return (text.match(/error TS/g) || []).length;
}

describe('fibjs --check built-in types', { skip: !isFibjs }, () => {
    var scratch;

    before(() => {
        scratch = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-check-types-'));
    });

    after(() => {
        fs.rmSync(scratch, { recursive: true, force: true });
    });

    it('knows the built-in modules and globals without node_modules', () => {
        var dir = path.join(scratch, 'bare');

        fs.mkdirSync(dir, { recursive: true });
        fs.writeFileSync(path.join(dir, 'a.ts'), [
            "import http from 'http';",
            "import fs from 'fs';",
            "const req = new http.Request('http://x/');",
            'const h: Class_Headers = req.headers;',
            "const buf: Class_Buffer = fs.readFileSync('.');",
            "const b = Buffer.from('abc');",
            'const home: string | undefined = process.env.HOME;',
            'const r = require("fs");',
            'console.log(req, h, buf, b, home, r);'
        ].join('\n'));

        var r = runCheck(dir, ['a.ts']);

        assert.equal(errors(r.stdout + r.stderr), 0, r.stdout + r.stderr);
    });

    it('still reports the errors of the checked files', () => {
        var dir = path.join(scratch, 'user-error');

        fs.mkdirSync(dir, { recursive: true });
        fs.writeFileSync(path.join(dir, 'b.ts'), [
            "import http from 'http';",
            "const req = new http.Request('http://x/');",
            'req.nope();'
        ].join('\n'));

        var r = runCheck(dir, ['b.ts']);

        assert.equal(errors(r.stdout + r.stderr), 1, r.stdout + r.stderr);
        assert.ok((r.stdout + r.stderr).includes('nope'), 'the user error is missing');
    });

    it('types the callable modules and the callable instances', () => {
        var dir = path.join(scratch, 'callable');

        fs.mkdirSync(dir, { recursive: true });
        fs.writeFileSync(path.join(dir, 'callable.ts'), [
            "import assert from 'assert';",
            "import nassert from 'node:assert';",
            "import { describe, it, xit, todo } from 'test';",
            "import test from 'test';",
            "import suite from 'test_suite';",
            "import ntest from 'node:test';",
            "import util from 'util';",
            'assert.ok(1);',
            'assert(1);',
            'nassert.ok(1);',
            "describe('suite', () => { it('case', () => {}); });",
            "suite('nested', () => {});",
            "test('item', () => {});",
            "ntest('node item', () => {});",
            "xit('paused', () => {});",
            "todo('planned', () => {});",
            "it.skip('skip', () => {});",
            "it.only('only', () => {});",
            "it.todo('later');",
            'test.assert(1);',
            "const log = util.debuglog('section');",
            "log('direct');",
            'console.log(log);'
        ].join('\n'));

        // The modules are callable objects: `operator(...)` in the IDL becomes
        // a function merged with the module namespace and published with
        // `export =` (the shape @types/node gives `node:test`), so the aliases
        // (`describe`, `it`, `assert.ok`, ...) are callable too.
        var r = runCheck(dir, ['callable.ts']);

        assert.equal(errors(r.stdout + r.stderr), 0, r.stdout + r.stderr);

        // ... and the call signatures are typed, not `any`
        fs.writeFileSync(path.join(dir, 'callable-bad.ts'), [
            "import assert from 'assert';",
            "import test from 'test';",
            'assert(1, 2, 3);',
            "test.nonexistent('x', () => {});"
        ].join('\n'));

        r = runCheck(dir, ['callable-bad.ts']);

        assert.equal(errors(r.stdout + r.stderr), 2, r.stdout + r.stderr);
    });

    it('types the assert/strict module under its runtime name', () => {
        var dir = path.join(scratch, 'assert-strict');

        fs.mkdirSync(dir, { recursive: true });
        fs.writeFileSync(path.join(dir, 'strict.ts'), [
            "import strict from 'assert/strict';",
            "import nstrict from 'node:assert/strict';",
            "import { deepStrictEqual } from 'node:assert/strict';",
            "import assert from 'assert';",
            'strict.deepStrictEqual(1, 1);',
            'nstrict.ok(1);',
            'deepStrictEqual(1, 1);',
            'assert.strict(true);',
            '// @ts-expect-error the parameters are typed, not any',
            'strict.deepStrictEqual(1);'
        ].join('\n'));

        // the runtime registers the strict assert API as `assert/strict`
        // (SandBox::installGlobal) with the fibjs:/node: prefixes; the
        // declarations use that name, so `node:assert/strict` types and the
        // `assert.strict` alias agrees
        var r = runCheck(dir, ['strict.ts']);

        assert.equal(errors(r.stdout + r.stderr), 0, r.stdout + r.stderr);

        // `assert_strict` is not a runtime module name; the types refuse it too
        fs.writeFileSync(path.join(dir, 'bad.ts'), "import bad from 'assert_strict';");

        r = runCheck(dir, ['bad.ts']);

        assert.equal(errors(r.stdout + r.stderr), 1, r.stdout + r.stderr);
    });

    it('keeps the callable and export= declarations in their documented shapes', () => {
        // The callable modules and the class call signatures are corpus facts:
        // a class declaration cannot hold a call signature (TS1068) and
        // `export *` cannot re-export an `export =` module (TS2498), so the
        // generated shapes are asserted here directly.
        var files = require('internal/fibjs-types').files;

        [['test', 'test'], ['test_suite', 'test_suite'],
            ['assert', 'assert'], ['assert_strict', 'assert/strict']
        ].forEach(([unit, declared]) => {
            var rel = 'dts/module/' + unit + '.d.ts';
            var text = files[rel];

            assert.ok(text, rel + ' is missing');
            assert.ok(text.includes("declare module '" + declared + "' {"),
                rel + " must declare the runtime name '" + declared + "'");
            assert.ok(new RegExp('^    function ' + unit + '\\(', 'm').test(text),
                rel + ' must declare the call overloads as functions named after the module');
            assert.ok(text.includes('namespace ' + unit + ' {'), rel + ' must merge a namespace');
            assert.ok(text.includes('export = ' + unit + ';'), rel + ' must publish with export =');
        });

        // the alias chain must point at the runtime module names
        assert.ok(files['dts/module/assert.d.ts'].includes("const strict: typeof import ('assert/strict');"),
            'assert.strict must reference assert/strict');
        assert.ok(files['dts/module/test.d.ts'].includes("const describe: typeof import ('test_suite');"),
            'test.describe must reference test_suite');

        // a class declaration cannot hold a call signature (TS1068): the
        // ConsoleObject signatures merge in through a sibling interface
        var consoleObject = files['dts/interface/ConsoleObject.d.ts'];
        var mergedAt = consoleObject.indexOf('declare interface Class_ConsoleObject');
        // the anonymous form: the named members (`log(...args: any[]): void;`)
        // must not count
        var callSignature = /^    \(\.\.\.args: any\[\]\): void;$/m;

        assert.ok(mergedAt > 0, 'the merged interface is missing');
        assert.ok(consoleObject.slice(0, mergedAt).includes('declare class Class_ConsoleObject'),
            'the class declaration is missing');
        assert.ok(!callSignature.test(consoleObject.slice(0, mergedAt)),
            'the class body must not hold the call signature');
        assert.ok(callSignature.test(consoleObject.slice(mergedAt)),
            'the merged interface must carry the call signature');

        // `export *` cannot re-export an `export =` module (TS2498): the
        // fibjs:/node: aliases bind and re-export the entity instead
        var prefixed = files['dts/_builtin/prefixed-modules.d.ts'];

        [['test', 'test'], ['assert', 'assert'], ['assert/strict', 'assert_strict']].forEach(([name, bind]) => {
            assert.ok(prefixed.includes('declare module "node:' + name + '" { import ' + bind + ' = require("' + name + '"); export = ' + bind + '; }'),
                'node:' + name + ' must bind and re-export the export = entity');
            assert.ok(!prefixed.includes('export * from "' + name + '"'),
                'export * from "' + name + '" would be TS2498');
        });
    });

    it('--no-builtin-types falls back to the types-less behaviour', () => {
        var dir = path.join(scratch, 'bare');
        var r = runCheck(dir, ['--no-builtin-types', 'a.ts']);

        assert.ok(errors(r.stdout + r.stderr) > 0,
            'the fallback must not know the built-in types: ' + r.stdout + r.stderr);
    });

    it('a project that lists DOM keeps the browser types and skips the built-in ones', () => {
        var dir = path.join(scratch, 'dom-project');

        fs.mkdirSync(dir, { recursive: true });
        fs.writeFileSync(path.join(dir, 'tsconfig.json'), JSON.stringify({
            compilerOptions: { lib: ['ES2020', 'DOM'], types: [] }
        }));
        fs.writeFileSync(path.join(dir, 'a.ts'), [
            'const b: Blob = new Blob([]);',
            'const buf: Class_Buffer = Buffer.from("a");',
            'console.log(b, buf);'
        ].join('\n'));

        var r = runCheck(dir, []);
        var text = r.stdout + r.stderr;

        assert.ok(text.includes('note: the project lists DOM in `lib`'),
            'the skip note is missing: ' + text);
        assert.ok(text.includes("Cannot find name 'Class_Buffer'"),
            'the built-in types must not be attached: ' + text);
        assert.ok(!text.includes("'Blob'"),
            'the browser Blob must be the one in play (no built-in one): ' + text);
    });

    it('keeps @types/node out when a dependency references it and the types exclude node', () => {
        // undici-types (reached through @anthropic-ai/sdk) carries
        // `/// <reference types="node" />`. The directive resolves against the
        // type roots and ignores the project's `types` list, so without the
        // refusal the whole @types/node global set would join the built-in
        // ones and the two Buffer/WebSocket/... sets would fight.
        var dir = path.join(scratch, 'node-reference');

        fs.mkdirSync(path.join(dir, 'node_modules', '@types', 'node'), { recursive: true });
        fs.mkdirSync(path.join(dir, 'node_modules', 'dep'), { recursive: true });
        fs.writeFileSync(path.join(dir, 'node_modules', '@types', 'node', 'package.json'),
            JSON.stringify({ name: '@types/node', types: 'index.d.ts' }));
        fs.writeFileSync(path.join(dir, 'node_modules', '@types', 'node', 'index.d.ts'),
            'declare var Buffer: { from(value: string): { node: true } };\n');
        fs.writeFileSync(path.join(dir, 'node_modules', 'dep', 'package.json'),
            JSON.stringify({ name: 'dep', types: 'index.d.ts' }));
        fs.writeFileSync(path.join(dir, 'node_modules', 'dep', 'index.d.ts'),
            '/// <reference types="node" />\nexport declare function f(): void;\n');
        fs.writeFileSync(path.join(dir, 'tsconfig.json'), JSON.stringify({
            compilerOptions: { types: [], strict: true, skipLibCheck: true },
            files: ['a.ts']
        }));
        fs.writeFileSync(path.join(dir, 'a.ts'), [
            "import { f } from 'dep';",
            "const b: Class_Buffer = Buffer.from('abc');",
            'f();',
            'console.log(b);'
        ].join('\n'));

        var r = runCheck(dir, []);
        var text = r.stdout + r.stderr;

        assert.equal(errors(text), 0, text);
        assert.ok(text.includes('the reference is not resolved'),
            'the refusal note is missing: ' + text);

        // the flip side: a `types` list naming node changes nothing - the
        // program never carries @types/node, and the built-in globals stay
        // (they are not dropped for a node-named project)
        fs.writeFileSync(path.join(dir, 'tsconfig.json'), JSON.stringify({
            compilerOptions: { types: ['node'], strict: true, skipLibCheck: true },
            files: ['b.ts']
        }));
        fs.writeFileSync(path.join(dir, 'b.ts'), [
            "import { f } from 'dep';",
            "const b: Class_Buffer = Buffer.from('abc');",
            'f();',
            'console.log(b);'
        ].join('\n'));

        r = runCheck(dir, []);
        text = r.stdout + r.stderr;

        assert.equal(errors(text), 0, text);
        assert.ok(text.includes('not loaded'),
            'the refusal note for the `types` entry is missing: ' + text);

        r = runCheck(dir, ['--listFilesOnly']);

        assert.ok(!r.stdout.includes('@types/node'),
            'a `types` entry naming node must not load it: ' + r.stdout);
    });

    it('keeps @types/node out of a DOM project whose types exclude node as well', () => {
        // the refusal is not tied to the fibjs built-in types: a browser
        // project that pinned its `types` gets the same policy, so node-only
        // globals (process, Buffer, ...) do not type-check in browser code
        // just because a dependency references them.
        var dir = path.join(scratch, 'node-reference-browser');

        fs.mkdirSync(path.join(dir, 'node_modules', '@types', 'node'), { recursive: true });
        fs.mkdirSync(path.join(dir, 'node_modules', 'dep'), { recursive: true });
        fs.writeFileSync(path.join(dir, 'node_modules', '@types', 'node', 'package.json'),
            JSON.stringify({ name: '@types/node', types: 'index.d.ts' }));
        fs.writeFileSync(path.join(dir, 'node_modules', '@types', 'node', 'index.d.ts'),
            'declare var Buffer: { from(value: string): { node: true } };\n');
        fs.writeFileSync(path.join(dir, 'node_modules', 'dep', 'package.json'),
            JSON.stringify({ name: 'dep', types: 'index.d.ts' }));
        fs.writeFileSync(path.join(dir, 'node_modules', 'dep', 'index.d.ts'),
            '/// <reference types="node" />\nexport declare function f(): void;\n');
        fs.writeFileSync(path.join(dir, 'tsconfig.json'), JSON.stringify({
            compilerOptions: { lib: ['ES2020', 'DOM'], types: [], strict: true, skipLibCheck: true },
            files: ['a.ts']
        }));
        fs.writeFileSync(path.join(dir, 'a.ts'), [
            "import { f } from 'dep';",
            'const d: Document = document;',
            'f();',
            'console.log(d);'
        ].join('\n'));

        var r = runCheck(dir, []);
        var text = r.stdout + r.stderr;

        assert.equal(errors(text), 0, text);
        assert.ok(text.includes('the reference is not resolved'),
            'the refusal note is missing: ' + text);

        // the program itself must not carry the node globals
        r = runCheck(dir, ['--listFilesOnly']);
        assert.ok(!r.stdout.includes('@types/node'),
            'the refused program must not contain @types/node: ' + r.stdout);

        // the flip side: a `types` list naming node changes nothing here either
        fs.writeFileSync(path.join(dir, 'tsconfig.json'), JSON.stringify({
            compilerOptions: { lib: ['ES2020', 'DOM'], types: ['node'], strict: true, skipLibCheck: true },
            files: ['a.ts']
        }));

        r = runCheck(dir, ['--listFilesOnly']);

        assert.ok(!r.stdout.includes('@types/node'),
            'a checked program never carries @types/node: ' + r.stdout);
    });

    it('never auto-includes @types/node (a `types: ["*"]` scan is refused too)', () => {
        // the package sits in node_modules and the project scans the type
        // roots with `types: ["*"]`: stock tsc would load it. A checked
        // program does not - the built-in globals stay the environment, the
        // misleading TS2688 ("the package is installed") is not reported, and
        // the refusal is said once.
        var dir = path.join(scratch, 'node-auto-include');

        fs.mkdirSync(path.join(dir, 'node_modules', '@types', 'node'), { recursive: true });
        fs.writeFileSync(path.join(dir, 'node_modules', '@types', 'node', 'package.json'),
            JSON.stringify({ name: '@types/node', types: 'index.d.ts' }));
        fs.writeFileSync(path.join(dir, 'node_modules', '@types', 'node', 'index.d.ts'),
            'declare var Buffer: { from(value: string): { node: true } };\n');
        fs.writeFileSync(path.join(dir, 'tsconfig.json'), JSON.stringify({
            compilerOptions: { types: ['*'], skipLibCheck: true }
        }));
        fs.writeFileSync(path.join(dir, 'a.ts'), [
            "const b: Class_Buffer = Buffer.from('abc');",
            'console.log(b);'
        ].join('\n'));

        var r = runCheck(dir, []);
        var text = r.stdout + r.stderr;

        assert.equal(errors(text), 0, text);
        assert.ok(text.includes('not loaded'),
            'the auto-include refusal note is missing: ' + text);

        r = runCheck(dir, ['--listFilesOnly']);

        assert.ok(!r.stdout.includes('@types/node'),
            'the default scan must not pull @types/node in: ' + r.stdout);
    });

    it('the default lib of a fibjs project is es-only', () => {
        var dir = path.join(scratch, 'bare');

        fs.writeFileSync(path.join(dir, 'dom.ts'), 'const d = document; console.log(d);');

        var r = runCheck(dir, ['dom.ts']);

        assert.ok((r.stdout + r.stderr).includes("Cannot find name 'document'"),
            'lib.dom must not come in through the compiler defaults: ' + r.stdout + r.stderr);
    });

    it('types the Buffer surface and keeps the removed constructor untyped', () => {
        var dir = path.join(scratch, 'buffer');

        fs.mkdirSync(dir, { recursive: true });
        fs.writeFileSync(path.join(dir, 'ok.ts'), [
            "const a: Class_Buffer = Buffer.from('abc');",
            'const b: Class_Buffer = Buffer.alloc(3);',
            "const c: Class_Buffer = Buffer.concat([Buffer.from('a')]);",
            "const n: number = Buffer.byteLength('x');",
            'const u8: Uint8Array = a;',
            'console.log(a, b, c, n, u8);'
        ].join('\n'));

        var r = runCheck(dir, ['ok.ts']);

        assert.equal(errors(r.stdout + r.stderr), 0, r.stdout + r.stderr);

        // `new Buffer(...)` was removed from the typed surface on purpose
        // (plans/buffer-types-refactor-2026-10-01.md §2.3): the runtime keeps
        // accepting it through the JS layer, the d.ts must not offer it.
        fs.writeFileSync(path.join(dir, 'new.ts'), 'const x = new Buffer(3);');
        r = runCheck(dir, ['new.ts']);

        assert.ok(errors(r.stdout + r.stderr) > 0,
            'new Buffer must remain a type error: ' + r.stdout + r.stderr);
    });

    it('closes the audited type gaps (process.exit/globalThis.fetch/URL/placeholder merge)', () => {
        var dir = path.join(scratch, 'audit-gaps');

        fs.mkdirSync(dir, { recursive: true });
        fs.writeFileSync(path.join(dir, 'gaps.ts'), [
            // process.exit terminates the process: the narrowing must work
            'function die(): never { process.exit(1); }',
            'const f: typeof fetch = globalThis.fetch;',
            "const b: Blob = new Blob(['x']);",
            "const e: Event = new Event('t');",
            'const et: EventTarget = new EventTarget();',
            'const fd: FormData = new FormData();',
            // the URL base accepts a string and a UrlObject alike
            "const rel = new URL('./a', new URL('https://x/'));",
            "const rel2 = new URL('./a', 'https://x/');",
            'console.log(die, f, b, e, et, fd, rel, rel2);'
        ].join('\n'));

        var r = runCheck(dir, ['gaps.ts']);

        assert.equal(errors(r.stdout + r.stderr), 0, r.stdout + r.stderr);

        // @types/react's `declare global { interface Blob {} }` placeholder
        // must merge with the built-in `interface Blob extends Class_Blob {}`
        // instead of shadowing the value (the TS2693 of the audit)
        fs.writeFileSync(path.join(dir, 'placeholder.ts'), [
            'declare global { interface Blob {} }',
            'export {};'
        ].join('\n'));
        fs.writeFileSync(path.join(dir, 'use.ts'),
            'export const use = (x: unknown) => x instanceof Blob;');

        r = runCheck(dir, ['placeholder.ts', 'use.ts']);

        assert.equal(errors(r.stdout + r.stderr), 0, r.stdout + r.stderr);
    });

    it('accepts the Handler conversion face the IDL declares', () => {
        var dir = path.join(scratch, 'handler-union');

        fs.mkdirSync(dir, { recursive: true });
        fs.writeFileSync(path.join(dir, 'handler.ts'), [
            "import http from 'http';",
            "import http2 from 'http2';",
            "import net from 'net';",
            "import tls from 'tls';",
            "import mq from 'mq';",
            // the IDL declares the parameter as
            // `Handler|Handler[]|Function(...) => Value|Object|String`: the
            // callable alternative carries the shapes the runtime converts
            // into a Handler, so --strict gives the arrow parameters their
            // contextual type (no TS7006) and the declared parameter types
            "http.createServer((req, res) => { const h: Class_Headers = req.headers; res.write('x'); res.end(); });",
            "http.createServer({ '/': (req: any, res: any) => { } });",
            "http.createServer('./www');",
            "http.createServer('http://backend');",
            "http.createServer([new mq.Handler((req: any, res: any) => { })]);",
            'new http.Server((req, res) => { req.method; res.end(); });',
            "new http.Handler('./www');",
            'net.createServer((sock) => { sock.remotePort; sock.end(); });',
            'new net.TcpServer(0, (sock) => { sock.remotePort; sock.end(); });',
            'tls.createServer({}, (sock) => { sock.alpnProtocol; sock.end(); });',
            'http2.createServer({}, (req, res) => { req.method; res.end(); });',
            'const r = new mq.Routing({});',
            "r.get('/x/*', (req, p1, res) => { req.value; console.log(p1, res); });",
            "r.append('/x', './www');",
            'mq.invoke((req: any) => req, new mq.Routing({}));',
            'console.log(http, http2, net, tls, mq, r);'
        ].join('\n'));

        var r = runCheck(dir, ['--strict', 'handler.ts']);

        assert.equal(errors(r.stdout + r.stderr), 0, r.stdout + r.stderr);

        // the callable shape types the callback parameters per server: a
        // member of another server's argument is a type error
        fs.writeFileSync(path.join(dir, 'shapes.ts'), [
            "import net from 'net';",
            "import tls from 'tls';",
            '// @ts-expect-error a tcp socket has no tls member',
            'net.createServer((sock) => { sock.alpnProtocol; });',
            '// @ts-expect-error a tls socket is not an http request',
            'tls.createServer({}, (sock) => { sock.headers; });'
        ].join('\n'));

        r = runCheck(dir, ['shapes.ts']);

        assert.equal(errors(r.stdout + r.stderr), 0, r.stdout + r.stderr);

        // the union must not have widened the parameters to `any`: the values
        // no alternative accepts stay type errors
        fs.writeFileSync(path.join(dir, 'neg.ts'), [
            "import http from 'http';",
            "import net from 'net';",
            '// @ts-expect-error',
            'http.createServer(123);',
            '// @ts-expect-error',
            'net.createServer(123);'
        ].join('\n'));

        r = runCheck(dir, ['neg.ts']);

        assert.equal(errors(r.stdout + r.stderr), 0, r.stdout + r.stderr);
    });

    it('accepts the descriptor unions the IDL declares', () => {
        var dir = path.join(scratch, 'descriptor-union');

        fs.mkdirSync(dir, { recursive: true });
        fs.writeFileSync(path.join(dir, 'fd.ts'), [
            "import fs from 'fs';",
            // the descriptor parameters declare `Integer|FileHandle`: a raw
            // descriptor or a FileHandle object
            'const h: Class_FileHandle = fs.open("x");',
            'const st = fs.fstat(1);',
            'fs.fstat(h);',
            'const buf = Buffer.alloc(8);',
            'fs.read(0, buf);',
            'fs.write(1, buf);',
            'fs.close(1);',
            'fs.fsync(1);',
            'fs.ftruncate(1, 0);',
            'fs.readFile(0);',
            'fs.readFile(h);',
            'fs.writeFile(1, "x");',
            'fs.appendFile(1, "x");',
            'console.log(st, buf);'
        ].join('\n'));

        var r = runCheck(dir, ['--strict', 'fd.ts']);

        assert.equal(errors(r.stdout + r.stderr), 0, r.stdout + r.stderr);

        fs.writeFileSync(path.join(dir, 'fd-neg.ts'), [
            "import fs from 'fs';",
            '// @ts-expect-error a string is not a descriptor',
            'fs.fstat("1");'
        ].join('\n'));

        r = runCheck(dir, ['fd-neg.ts']);

        assert.equal(errors(r.stdout + r.stderr), 0, r.stdout + r.stderr);
    });

    it('types the pipe destination as the untyped compatibility face', () => {
        var dir = path.join(scratch, 'pipe-union');

        fs.mkdirSync(dir, { recursive: true });
        fs.writeFileSync(path.join(dir, 'pipe.ts'), [
            "import io from 'io';",
            "import mq from 'mq';",
            // the destination stays `any` on purpose: the node:stream layer
            // produces pure-JS objects, so a class-typed alternative would
            // reject them at the binding (the MCP stdio regression)
            'const src = new io.MemoryStream();',
            'const dst = new io.MemoryStream();',
            'const back = src.pipe(dst);',
            'const msg = new mq.Message();',
            "msg.pipe('a node:stream object is accepted at run time');",
            'console.log(back);'
        ].join('\n'));

        var r = runCheck(dir, ['pipe.ts']);

        assert.equal(errors(r.stdout + r.stderr), 0, r.stdout + r.stderr);
    });

    it('accepts the AsyncResource triggerAsyncId the IDL declares', () => {
        var dir = path.join(scratch, 'async-resource-union');

        fs.mkdirSync(dir, { recursive: true });
        fs.writeFileSync(path.join(dir, 'async-resource.ts'), [
            "import { AsyncResource } from 'async_hooks';",
            // the constructor declares `Number|Object triggerAsyncId`
            "const ar = new AsyncResource('T', 7);",
            "const ar2 = new AsyncResource('T', { triggerAsyncId: 7 });",
            'console.log(ar, ar2);'
        ].join('\n'));

        var r = runCheck(dir, ['async-resource.ts']);

        assert.equal(errors(r.stdout + r.stderr), 0, r.stdout + r.stderr);

        fs.writeFileSync(path.join(dir, 'async-resource-neg.ts'), [
            "import { AsyncResource } from 'async_hooks';",
            '// @ts-expect-error a string is not a number or an options object',
            "new AsyncResource('T', 'nope');"
        ].join('\n'));

        r = runCheck(dir, ['async-resource-neg.ts']);

        assert.equal(errors(r.stdout + r.stderr), 0, r.stdout + r.stderr);
    });

    it('accepts the markResourceTiming object parameters the IDL declares', () => {
        var dir = path.join(scratch, 'perf-union');

        fs.mkdirSync(dir, { recursive: true });
        fs.writeFileSync(path.join(dir, 'perf.ts'), [
            "import { performance } from 'perf_hooks';",
            // markResourceTiming declares plain objects for timingInfo and
            // bodyInfo; the global argument still takes any value
            "performance.markResourceTiming({}, 'https://example.com/', 'fetch', globalThis, 'local', {}, 200);"
        ].join('\n'));

        var r = runCheck(dir, ['perf.ts']);

        assert.equal(errors(r.stdout + r.stderr), 0, r.stdout + r.stderr);

        fs.writeFileSync(path.join(dir, 'perf-neg.ts'), [
            "import { performance } from 'perf_hooks';",
            '// @ts-expect-error a string is not a plain object',
            "performance.markResourceTiming('x', 'https://example.com/', 'fetch', globalThis, 'local', {}, 200);",
            '// @ts-expect-error a string is not a plain body object',
            "performance.markResourceTiming({}, 'https://example.com/', 'fetch', globalThis, 'local', 'x', 200);"
        ].join('\n'));

        r = runCheck(dir, ['perf-neg.ts']);

        assert.equal(errors(r.stdout + r.stderr), 0, r.stdout + r.stderr);
    });

    it('accepts the URL unions the IDL declares', () => {
        var dir = path.join(scratch, 'url-union');

        fs.mkdirSync(dir, { recursive: true });
        fs.writeFileSync(path.join(dir, 'url.ts'), [
            "import url from 'url';",
            // the URL parameters declare `UrlObject|String|Object`: the
            // constructor base, url.format and url.fileURLToPath accept a
            // UrlObject, a URL string and a URL components object alike
            "const a = new URL('/a', 'https://x/');",
            "const b = new URL('/a', new URL('https://x/'));",
            "const c = new URL('/a', { protocol: 'https:', hostname: 'x' });",
            "const f: string = url.format('https://x/a?b=1#c', { fragment: false });",
            'const f2: string = url.format({ protocol: "https:", hostname: "x" });',
            "const p: string = url.fileURLToPath({ protocol: 'file:', pathname: '/tmp/x' });",
            'console.log(a, b, c, f, f2, p);'
        ].join('\n'));

        var r = runCheck(dir, ['--strict', 'url.ts']);

        assert.equal(errors(r.stdout + r.stderr), 0, r.stdout + r.stderr);
    });
    it('accepts the init-object unions the IDL declares', () => {
        var dir = path.join(scratch, 'init-union');

        fs.mkdirSync(dir, { recursive: true });
        fs.writeFileSync(path.join(dir, 'init.ts'), [
            "import http from 'http';",
            "import rtc from 'rtc';",
            "import crypto from 'crypto';",
            // HttpCookie|Object: the instance or its options object
            'const res = new http.Response();',
            "res.addCookie({ name: 'a', value: 'b', path: '/' });",
            "res.addCookie(new http.Cookie({ name: 'c', value: 'd' }));",
            // RTCSessionDescription|Object / RTCIceCandidate|Object
            'const pc = new rtc.RTCPeerConnection();',
            "pc.setLocalDescription({ type: 'offer', sdp: 'v=0' });",
            "pc.setRemoteDescription(new rtc.RTCSessionDescription({ type: 'offer', sdp: 'v=0' }));",
            "pc.addIceCandidate({ candidate: 'a', sdpMid: '0' });",
            // X509Certificate|Buffer|String
            'const leaf = new crypto.X509Certificate(Buffer.from("x"));',
            'leaf.checkIssued(new crypto.X509Certificate(Buffer.from("y")));',
            "leaf.checkIssued('-----BEGIN CERTIFICATE-----');",
            "leaf.checkIssued(Buffer.from('-----BEGIN CERTIFICATE-----'));",
            'console.log(res, pc, leaf);'
        ].join('\n'));

        var r = runCheck(dir, ['--strict', 'init.ts']);

        assert.equal(errors(r.stdout + r.stderr), 0, r.stdout + r.stderr);
    });

    it('types the node compatibility modules (events/buffer/stream/... and the subpaths)', () => {
        // The modules below have no IDL definition: the runtime builds them in
        // the pure-JS compatibility layer (fibjs/scripts/*.js) and their
        // declarations are hand-written under tools/handwritten/module
        // (plans/fibjs-upstream-gaps.md B1). Every one of them used to answer
        // TS2591 "Cannot find name 'node:events'", which reads like a missing
        // global while the module is really there.
        //
        // The subpath modules carry a second fix: the corpus used to declare
        // them under their IDL unit name (`path_posix`, `types`), which is not
        // a name the runtime answers to (B2). The declarations now use the
        // runtime spelling, so the prefixed aliases (`node:path/posix`,
        // `node:util/types`) resolve too.
        var dir = path.join(scratch, 'node-compat');

        fs.mkdirSync(dir, { recursive: true });
        fs.writeFileSync(path.join(dir, 'compat.ts'), [
            "import { EventEmitter } from 'node:events';",
            "import { Buffer as NBuffer, SlowBuffer, transcode } from 'node:buffer';",
            "import * as stream from 'node:stream';",
            "import { finished, pipeline } from 'node:stream/promises';",
            "import { ReadableStream, TransformStream } from 'node:stream/web';",
            "import * as https from 'node:https';",
            "import * as readline from 'node:readline';",
            "import * as readlinePromises from 'node:readline/promises';",
            "import { setTimeout as delay, setImmediate as defer } from 'node:timers/promises';",
            "import * as pPosix from 'node:path/posix';",
            "import * as pWin32 from 'node:path/win32';",
            "import * as dc from 'node:diagnostics_channel';",
            "import * as util from 'node:util';",
            // B5: the AddressInfo export of node:net
            "import * as net from 'node:net';",
            "import type { AddressInfo } from 'node:net';",
            '',
            // the class value is usable in the type position (the Node spelling)
            'const emitter: EventEmitter = new EventEmitter();',
            'const isEmitter: boolean = emitter instanceof EventEmitter;',
            'const nb: Class_Buffer = NBuffer.from("x");',
            'const sb: Class_Buffer = new SlowBuffer(4);',
            'const tr: Class_Buffer = transcode(nb, "utf8", "latin1");',
            'const readable: stream.Readable = new stream.Readable();',
            'const writable: stream.Writable = new stream.Writable();',
            'const base: stream.Stream = readable;',
            'stream.addAbortSignal(new AbortController().signal, readable);',
            'const piped: stream.Writable = readable.pipe(writable);',
            'const pf: Promise<void> = finished(readable);',
            'const pp: Promise<void> = pipeline(readable, writable);',
            'const rs: ReadableStream<string> = new ReadableStream<string>();',
            'const ts: TransformStream<string, number> = new TransformStream<string, number>();',
            // https is the http module object at runtime
            'const hs: https.Server = https.createServer(() => { });',
            'const rl: readline.Interface = readline.createInterface({ input: readable });',
            'const q: Promise<string> = readlinePromises.createInterface({ input: readable }).question("?");',
            'const t1: Promise<number> = delay(1, 42);',
            'const t2: Promise<string> = defer("x");',
            'const joined: string = pPosix.join("a", "b");',
            'const normalized: string = pWin32.normalize("C:\\\\a\\\\b");',
            'const ch: dc.Channel = dc.channel("x");',
            'const hasCh: boolean = dc.hasSubscribers("x") || ch.hasSubscribers;',
            // util.types is the runtime name; `require("types")` fails
            'const sameTypes: boolean = util.types === require("util/types");',
            'const server = net.createServer(() => { });',
            'const info: AddressInfo = server.address();',
            'console.log(emitter, isEmitter, nb, sb, tr, readable, writable, base, piped,',
            '    pf, pp, rs, ts, hs, rl, q, t1, t2, joined, normalized, hasCh, sameTypes, info);'
        ].join('\n'));

        var r = runCheck(dir, ['--strict', 'compat.ts']);

        assert.equal(errors(r.stdout + r.stderr), 0, r.stdout + r.stderr);
    });

    it('gives the module class aliases a type twin (http.Server as a type)', () => {
        // `const Server: typeof Class_HttpServer` is a value only, so the Node
        // spelling `let s: http.Server` answered TS2749 ("refers to a value,
        // but is being used as a type here"). The module aliases now carry a
        // same-named `type X = Class_Y` next to the const (B3).
        var dir = path.join(scratch, 'type-twin');

        fs.mkdirSync(dir, { recursive: true });
        fs.writeFileSync(path.join(dir, 'twin.ts'), [
            "import * as http from 'node:http';",
            "import * as net from 'node:net';",
            "import * as fs from 'node:fs';",
            'const server: http.Server = new http.Server(() => { });',
            'const response: http.ServerResponse = new http.Response();',
            'const request: http.Request = new http.Request();',
            'const socket: net.Socket = new net.Socket();',
            'const stats: fs.Stats = fs.statSync(".");',
            'console.log(server, response, request, socket, stats);'
        ].join('\n'));

        var r = runCheck(dir, ['--strict', 'twin.ts']);

        assert.equal(errors(r.stdout + r.stderr), 0, r.stdout + r.stderr);
    });

    it('types the stream family depth (hooks, iteration, operators, generic chunks)', async () => {
        // P2 of plans/fibjs-upstream-gaps-remediation-2026-10-14.md: the
        // declarations must describe the subclassing contract (`_read`,
        // `_write`, `_transform`, `_flush`, `_final`, `_destroy`), the option
        // objects, the async iteration protocol with its operator helpers, and
        // the chunk-type parameter. The runtime side is asserted in the same
        // test - a declaration that promises what the runtime does not have is
        // exactly the defect this suite exists for.
        var dir = path.join(scratch, 'stream-depth');

        fs.mkdirSync(dir, { recursive: true });
        fs.writeFileSync(path.join(dir, 'depth.ts'), [
            "import { Readable, Writable, Duplex, Transform, PassThrough } from 'node:stream';",
            "import type { ReadableOptions, ReadableAsyncIterator, ReadableEventMap } from 'node:stream';",
            "import { ReadableStream, WritableStream } from 'node:stream/web';",
            "import * as readline from 'node:readline';",
            '',
            'class Source extends Readable<number> {',
            '    private i = 0;',
            '    _read(size: number): void { this.push(this.i++ < 3 ? this.i : null); }',
            '}',
            '',
            'class Sink extends Writable<string> {',
            '    _write(chunk: string, encoding: string, callback: (error?: Error | null) => void): void {',
            '        callback();',
            '    }',
            '    _final(callback: (error?: Error | null) => void): void { callback(); }',
            '}',
            '',
            'class Upper extends Transform<string> {',
            '    _transform(chunk: string, encoding: string, callback: (error?: Error | null, data?: any) => void): void {',
            '        callback(null, chunk.toUpperCase());',
            '    }',
            '    _flush(callback: (error?: Error | null, data?: any) => void): void { callback(); }',
            '}',
            '',
            'const options: ReadableOptions = { objectMode: true, highWaterMark: 4, read(size: number) { this.push(size); } };',
            'const source = new Source();',
            'const sink = new Sink();',
            'const duplex = new Duplex<number>();',
            'const upper = new Upper({ transform(chunk, encoding, callback) { callback(null, chunk); } });',
            'const pass = new PassThrough<Buffer>();',
            '',
            '// the typed listeners carry the chunk type',
            "source.on('data', (chunk: number) => { void chunk.toFixed(2); });",
            "source.on('end', () => { });",
            "sink.on('finish', () => { });",
            "duplex.on('drain', () => { });",
            'const map: ReadableEventMap<number> = { "data": [1], "end": [], "error": [new Error("x")], "readable": [], "close": [], "pause": [], "resume": [] };',
            'void map;',
            '',
            'async function run(): Promise<void> {',
            '    // the chunk type flows through the iteration protocol',
            '    for await (const chunk of source) { const n: number = chunk; void n; }',
            '    const iterator: ReadableAsyncIterator<number> = source.iterator({ destroyOnReturn: false });',
            '    const first = await iterator.next();',
            '    if (!first.done) { const v: number = first.value; void v; }',
            '    const back: Readable<number> = iterator.stream;',
            '    void back;',
            '',
            '    // the operators keep the type',
            '    const doubled: Promise<number[]> = Readable.from([1, 2, 3]).map(n => n * 2).toArray();',
            '    const even: Readable<number> = Readable.from([1, 2]).filter(n => n % 2 === 0);',
            '    const letters: Readable<string> = Readable.from(["ab"]).flatMap((s: string) => s.split(""));',
            '    const taken: Readable<number> = Readable.from([1, 2]).drop(0).take(1);',
            '    const some: Promise<boolean> = Readable.from([1]).some(n => n > 0);',
            '    const found: Promise<number | undefined> = Readable.from([1]).find(n => n === 1);',
            '    const folded: Promise<number> = Readable.from([1, 2]).reduce((a: number, b: number) => a + b, 0);',
            '    await Promise.all([doubled, some, found, folded]);',
            '    void even; void letters; void taken;',
            '',
            '    // the web streams narrow their read result and keep the pipe types',
            '    const web = new ReadableStream<string>({ start(controller) { controller.enqueue("a"); controller.close(); } });',
            '    const read = await web.getReader().read();',
            '    if (!read.done) { const value: string = read.value; void value; }',
            '    const byteReader = new ReadableStream<Uint8Array>({ type: "bytes" }).getReader({ mode: "byob" });',
            '    const bytes = await byteReader.read(new Uint8Array(4));',
            '    if (!bytes.done) { const view: ArrayBufferView = bytes.value; void view.byteLength; }',
            '    const piped: Promise<void> = web.pipeTo(new WritableStream<string>());',
            '    void piped;',
            '',
            '    // readline answers the signal form of question()',
            '    const rl: readline.Interface = readline.createInterface({ input: source, terminal: false });',
            '    rl.on("line", (line: string) => { void line.length; });',
            '    rl.question("q? ", { signal: new AbortController().signal }, (answer: string) => { void answer; });',
            '}',
            'void run; void options; void sink; void duplex; void upper; void pass;',
        ].join('\n'));

        var r = runCheck(dir, ['--strict', 'depth.ts']);
        var text = r.stdout + r.stderr;

        assert.equal(errors(text), 0, text);

        // The runtime behind the declarations: the iteration protocol and the
        // operator helpers are the compat layer's own (P2), so exercise them.
        var stream = require('stream');
        var iterable = new stream.Readable({ objectMode: true, read() { } });

        assert.equal(typeof iterable[Symbol.asyncIterator], 'function',
            'Readable must expose the async iterator');
        ['iterator', 'map', 'filter', 'flatMap', 'drop', 'take', 'toArray', 'forEach', 'some', 'every', 'find', 'reduce']
            .forEach(name => {
                assert.equal(typeof iterable[name], 'function', 'Readable#' + name + ' is missing');
            });

        class Numbers extends stream.Readable {
            constructor() { super({ objectMode: true }); this.i = 0; }
            _read() { this.push(this.i++ < 3 ? this.i : null); }
        }

        var seen = [];
        for await (var chunk of new Numbers())
            seen.push(chunk);

        assert.deepEqual(seen, [1, 2, 3], 'the async iterator must yield the pushed chunks');

        var pipelineOut = await stream.Readable.from([1, 2, 3, 4])
            .filter(n => n % 2 === 0)
            .map(n => n * 10)
            .toArray();

        assert.deepEqual(pipelineOut, [20, 40], 'the operator helpers must compose');

        // `destroy(err)` must reject the pending iteration, not lose the error
        var cancelled = new stream.Readable({ read() { } });
        var cancelError = null;
        var pending = (async () => {
            try {
                for await (var value of cancelled)
                    void value;
            } catch (e) {
                cancelError = e;
            }
        })();

        cancelled.destroy(new Error('boom'));
        await pending;

        assert.ok(cancelError && cancelError.message === 'boom',
            'destroy(err) must reject the iteration: ' + (cancelError && cancelError.message));
    });
});
