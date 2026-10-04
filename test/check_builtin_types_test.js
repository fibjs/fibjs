// `fibjs --check` built-in types regression tests.
//
// The checker carries the built-in types of the runtime (the map built by
// tools/gen_builtin_types.js, embedded as `internal/fibjs-types`), so a
// project without node_modules must know http/fs/... and the fibjs globals,
// user errors must still be reported, and --no-builtin-types must fall back
// to the behaviour without the embedded types.
//
// Mixed @types/node projects are not a target: the fibjs globals would
// overlap the node ones, --no-builtin-types is the way out there.
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
});
