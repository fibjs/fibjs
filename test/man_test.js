// `fibjs --man` regression tests.
//
// The command renders the manual of the runtime from the embedded
// `internal/fibjs-man` index, which tools/gen_man.js generates from the
// @fibjs/types corpus (the same one `--check` embeds). Everything below must
// work without node_modules, a tsconfig or network access, and the pages must
// stay in sync with the corpus the binary carries.
//
// The suite spawns the binary through process.execPath, so it is fibjs only.

var { describe, it } = require('node:test');
var assert = require('assert');
var child_process = require('child_process');

const isFibjs = typeof process !== 'undefined' && process.versions && process.versions.fibjs;

function man(args) {
    var r = child_process.spawnSync(process.execPath, ['--man'].concat(args), {
        encoding: 'utf8',
        input: ''
    });

    return { code: r.status, stdout: r.stdout || '', stderr: r.stderr || '' };
}

describe('fibjs --man', { skip: !isFibjs }, () => {
    describe('the embedded data', () => {
        it('carries the corpus as a map of virtual .d.ts files', () => {
            var data = require('internal/fibjs-types');

            assert.ok(data.version, 'the corpus version is missing');

            var files = Object.keys(data.files);

            assert.ok(files.length >= 200, 'too few files: ' + files.length);

            ['dts/_import/_fibjs.d.ts', 'dts/module/http.d.ts', 'dts/module/fs.d.ts',
                'dts/interface/HttpServer.d.ts', 'dts/_builtin/globals.d.ts'
            ].forEach(rel => {
                assert.ok(data.files[rel], rel + ' is missing');
            });

            // the class-valued globals own their type name as an interface, so
            // one that a project (or @types/react) declares as a placeholder
            // merges instead of clashing
            assert.ok(data.files['dts/_builtin/globals.d.ts'].includes('interface Buffer extends Class_Buffer {}'),
                'the Buffer type declaration is missing');
            assert.ok(data.files['dts/_builtin/globals.d.ts'].includes('declare var Blob: typeof import(\'global\').Blob;'),
                'the Blob global is missing');
        });
    });

    describe('pages', () => {
        it('lists the modules without arguments', () => {
            var r = man([]);

            assert.equal(r.code, 0, r.stderr);
            ['http', 'fs', 'path'].forEach(name => {
                assert.ok(r.stdout.includes(name), `${name} is not listed`);
            });
        });

        it('renders a module page', () => {
            var r = man(['http']);

            assert.equal(r.code, 0, r.stderr);
            assert.ok(r.stdout.includes('Class_HttpServer'), 'the Server alias is missing');
            assert.ok(r.stdout.includes('createServer'), 'createServer is missing');
        });

        it('resolves a module alias to the object page', () => {
            var r = man(['http.Server']);

            assert.equal(r.code, 0, r.stderr);
            assert.ok(r.stdout.includes('Class_HttpServer'), 'the object is missing');
            assert.ok(r.stdout.includes('extends Class_TcpServer'), 'the base class is missing');
            assert.ok(r.stdout.includes('constructor('), 'the constructors are missing');
        });

        it('renders an object by its bare name', () => {
            var r = man(['HttpRequest']);

            assert.equal(r.code, 0, r.stderr);
            assert.ok(r.stdout.includes('Class_HttpRequest'), 'the object is missing');
        });

        it('renders the typed event listener of an event-bearing object', () => {
            var r = man(['EventSource']);

            assert.equal(r.code, 0, r.stderr);
            // the declared event params are the listener arguments
            // (plans/idl-event-types-2026-10-03.md)
            assert.ok(r.stdout.includes('on(event: "open", listener: (ev: FIBJS.GeneralObject)=>void)'),
                'the typed on() overload is missing: ' + r.stdout.slice(0, 300));
            assert.ok(r.stdout.includes('once(event: "open"'), 'the once() overload is missing');
        });

        it('renders the hand-written Buffer page without constructors', () => {
            var r = man(['Buffer']);

            assert.equal(r.code, 0, r.stderr);
            assert.ok(r.stdout.includes('extends Uint8Array'), 'the Uint8Array base is missing');
            // `new Buffer(...)` is intentionally outside the typed surface
            // (plans/buffer-types-refactor-2026-10-01.md §2.3)
            assert.ok(!r.stdout.includes('## Constructors'), 'the Buffer page must not offer a constructor');
        });

        it('renders a global object through the global module', () => {
            var r = man(['Buffer']);

            assert.equal(r.code, 0, r.stderr);
            assert.ok(r.stdout.includes('Class_Buffer'), 'the object is missing');
        });

        it('renders every overload of a function', () => {
            var r = man(['fs.readFileSync']);

            assert.equal(r.code, 0, r.stderr);
            // the merged Buffer/String and FileHandle/string overload families
            // render as one signature carrying the parameter unions; a class
            // parameter carries both flavors (the runtime accepts a fiber object
            // and its promise variant - same ClassInfo, see
            // plans/idl-event-types-2026-10-03.md §15)
            assert.ok(r.stdout.includes('function readFileSync(fname: Class_FileHandle | Class_FileHandlePromise | string'),
                'the fname union is missing');
            assert.ok(r.stdout.includes('options?: FIBJS.GeneralObject | string'),
                'the options union is missing');
        });

        it('accepts the node:/fibjs: prefixes', () => {
            var r = man(['node:fs']);

            assert.equal(r.code, 0, r.stderr);
            assert.ok(r.stdout.includes('readFileSync'), 'the fs page is missing');
        });
    });

    describe('search and json', () => {
        it('searches names and descriptions', () => {
            var r = man(['-k', 'cookie']);

            assert.equal(r.code, 0, r.stderr);
            assert.ok(r.stdout.length > 0, 'no output');
            assert.ok(r.stdout.toLowerCase().includes('cookie'), 'the hits are missing');
        });

        it('prints the entry as json', () => {
            var r = man(['--json', 'http.Server']);

            assert.equal(r.code, 0, r.stderr);
            var entry = JSON.parse(r.stdout);

            assert.equal(entry.kind, 'class');
            assert.equal(entry.name, 'Class_HttpServer');
            assert.equal(entry.source, 'dts/interface/HttpServer.d.ts');
            assert.ok(entry.members.length >= 8, 'too few members');
            assert.ok(entry.members.some(m => m.kind === 'constructor'),
                'the constructors are missing');
        });

        it('lists the modules as json', () => {
            var r = man(['--list', '--json']);

            assert.equal(r.code, 0, r.stderr);
            var list = JSON.parse(r.stdout);

            assert.equal(list.kind, 'list');
            assert.ok(list.modules.includes('http'), 'http is missing');
        });
    });

    describe('errors', () => {
        it('exits 1 without an entry on stdout', () => {
            var r = man(['no-such-thing']);

            assert.equal(r.code, 1);
            assert.equal(r.stdout, '', 'an error must not write to stdout');
            assert.ok(r.stderr.includes('no entry'), 'the reason is missing');
        });

        it('exits 1 on an unknown option, with the usage on stderr', () => {
            var r = man(['--bogus']);

            assert.equal(r.code, 1);
            assert.equal(r.stdout, '');
            assert.ok(r.stderr.includes('Usage: fibjs --man'), 'the usage is missing');
        });
    });
});
