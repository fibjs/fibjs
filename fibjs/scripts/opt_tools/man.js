#!/usr/bin/env fibjs
/*
 * man.js
 *
 * fibjs --man support: the manual of the runtime, read from the binary.
 *
 *   fibjs --man                          list the modules
 *   fibjs --man <module>                 the module page
 *   fibjs --man <module>.<member>        one member (an alias resolves to the
 *                                        object page it points at)
 *   fibjs --man <Name>                   the object page (a Class_* interface,
 *                                        e.g. `fibjs --man HttpRequest`)
 *   fibjs --man -k <word>                search names and descriptions
 *   fibjs --man --json <name>            machine readable output
 *
 * The pages come from the very data `fibjs --check` checks against: the
 * built-in types assembled from @fibjs/types and embedded as
 * `internal/fibjs-types`, a map of virtual path to .d.ts text (see
 * tools/gen_builtin_types.js). The IDL comments in that map are the manual,
 * so the command always describes the version of the runtime it runs on and
 * nothing is read from disk or the network.
 *
 * Only the files a query needs are parsed (the checker's view of a module is
 * one .d.ts file); `--list` reads nothing but the file names and `-k` scans
 * the corpus.
 *
 * Usage/exit code follow plans/cli-help-convention.md (R2/R3): `--help` is
 * handled first and writes to stdout, argument errors write the reason plus
 * the same usage to stderr and exit 1, a name that does not resolve exits 1.
 */

'use strict';

const builtin = require('internal/fibjs-types');

const FILES = builtin.files;
const VERSION = builtin.version;
const MODULE_PREFIX = 'dts/module/';
const INTERFACE_PREFIX = 'dts/interface/';
const GLOBALS_FILE = 'dts/module/global.d.ts';

let color = !!process.stdout.isTTY && !process.env.NO_COLOR;

function sgr(code, s) {
    return color ? '\u001b[' + code + 'm' + s + '\u001b[0m' : s;
}

const b = s => sgr('1', s);
const dim = s => sgr('2', s);

const USAGE = [
    'Usage: fibjs --man [options] [name]',
    '',
    'Look up the manual of the runtime embedded in the binary: the modules,',
    'the objects they export and every member, read from the same built-in',
    'types `fibjs --check` uses.',
    '',
    'Options:',
    '  -k, --search <word>         search names and descriptions',
    '      --list                  list the modules (the default without a name)',
    '      --json                  print the entry as JSON',
    '      --no-color              disable colored output',
    '  -h, --help                  print this message',
    '',
    'Examples:',
    '  fibjs --man http            the http module',
    '  fibjs --man http.Server     the Server object (Class_HttpServer)',
    '  fibjs --man fs.readFile     all overloads and their parameters',
    '  fibjs --man -k cookie       everything that mentions cookie',
    '',
    'Names accept the node:/fibjs: prefixes: `fibjs --man node:fs`.',
    '',
    'Run `fibjs --help` for the global options.'
].join('\n');

// ---------------------------------------------------------------------------
// the corpus, parsed on demand: the index keeps `decl` + `doc` per member and
// the helpers below derive the name and the kind from the declaration line
// ---------------------------------------------------------------------------

function aliasTarget(decl) {
    const m = /^const\s+(\w+):\s*typeof\s+(Class_\w+);/.exec(decl);

    return m ? { name: m[1], target: m[2] } : null;
}

function entryName(decl) {
    const m = /^(?:(?:readonly|static|declare|abstract|const|var|let|function)\s+)*("[^"]+"|\[[^\]]*\]|\w+)/.exec(decl);

    return m ? m[1].replace(/^"/, '').replace(/"$/, '') : decl.slice(0, 16);
}

function entryKind(decl) {
    if (aliasTarget(decl))
        return 'alias';
    if (decl.startsWith('function '))
        return 'function';
    if (decl.startsWith('const '))
        return 'const';
    if (decl.startsWith('var '))
        return 'var';

    return 'member';
}

function memberKind(decl) {
    const base = decl.replace(/^static\s+/, '');
    const kind = base.startsWith('constructor(') ? 'constructor'
        : base.startsWith('[') || base.startsWith('"[') ? 'symbol'
            : /^on\(event:/.test(base) ? 'event'
                : base.includes('(') ? 'method'
                    : 'prop';

    return decl.startsWith('static ') && (kind === 'method' || kind === 'prop')
        ? 'static-' + kind
        : kind;
}

function jsdocText(lines) {
    if (!lines.length)
        return '';

    const body = [];

    for (const line of lines) {
        let t = line.trim();

        if (t === '/**' || t === '*/' || t === '*') {
            body.push('');
            continue;
        }
        if (t.startsWith('/**'))
            t = t.slice(3);
        if (t.endsWith('*/'))
            t = t.slice(0, -2);
        if (t.startsWith('*'))
            t = t.slice(1);
        if (t.startsWith(' '))
            t = t.slice(1);
        body.push(t);
    }

    if (body.length && body[0].trim() === '')
        body.shift();
    while (body.length && body[body.length - 1].trim() === '')
        body.pop();

    let text = body.join('\n');

    // the generator marks the summary with @description
    text = text.replace(/^\s*!?\s*@description\s?/, '');

    // dedent: drop the smallest common indent
    const indents = text.split('\n').filter(l => l.trim()).map(l => l.length - l.trimStart().length);

    if (indents.length) {
        const cut = Math.min.apply(null, indents);

        if (cut > 0)
            text = text.split('\n').map(l => l.slice(cut)).join('\n');
    }

    return text.trim();
}

// one .d.ts file of the map -> { kind, name, base, doc, file, exports|members }
const parsed = Object.create(null);

function parse(rel) {
    const text = FILES[rel];

    if (text === undefined)
        return null;

    const cached = parsed[rel];

    if (cached !== undefined)
        return cached;

    const lines = text.split('\n');
    const head = /^declare module '([^']+)' \{|^declare class (Class_\w+)(?:<[^>]*>)?(?: extends ([A-Za-z_$][\w.$]*)(?:<[^>]*>)?)? \{/;
    let declIdx = -1;
    let result = null;

    for (let i = 0; i < lines.length; i++) {
        const m = head.exec(lines[i]);

        if (m) {
            declIdx = i;
            result = m[1]
                ? { kind: 'module', name: m[1] }
                : { kind: 'class', name: m[2], base: m[3] || '' };
            break;
        }
    }

    if (declIdx < 0)
        return (parsed[rel] = null);

    // JSDoc above the declaration: walk up to the opening marker
    let docStart = declIdx;

    while (docStart > 0 && !lines[docStart - 1].trim().startsWith('/**'))
        docStart--;

    if (docStart > 0)
        docStart--;

    const headDoc = [];

    for (let i = docStart; i < declIdx; i++)
        if (lines[i].trim())
            headDoc.push(lines[i]);

    result.doc = headDoc.length && headDoc[0].trim().startsWith('/**')
        ? jsdocText(headDoc)
        : '';

    // members: optional JSDoc block + one-line declaration, 4-space indent
    const members = [];
    let doc = null;

    for (let i = declIdx + 1; i < lines.length; i++) {
        const line = lines[i];
        const t = line.trim();

        if (t === '}' && line[0] === '}')
            break;

        if (t.startsWith('/**')) {
            doc = [line];
            while (i + 1 < lines.length && !lines[i].trim().endsWith('*/')) {
                i++;
                doc.push(lines[i]);
            }
            continue;
        }

        if (!t)
            continue;

        if (t.startsWith('//'))
            continue;

        members.push({
            decl: t.replace(/^export\s+/, ''),
            doc: doc ? jsdocText(doc) : ''
        });
        doc = null;
    }

    result.file = rel;
    if (result.kind === 'module')
        result.exports = members;
    else
        result.members = members;

    return (parsed[rel] = result);
}

function moduleNames() {
    return Object.keys(FILES)
        .filter(rel => rel.startsWith(MODULE_PREFIX) && rel.endsWith('.d.ts'))
        .map(rel => rel.slice(MODULE_PREFIX.length, -'.d.ts'.length))
        .sort();
}

function interfaceNames() {
    return Object.keys(FILES)
        .filter(rel => rel.startsWith(INTERFACE_PREFIX) && rel.endsWith('.d.ts'))
        .map(rel => rel.slice(INTERFACE_PREFIX.length, -'.d.ts'.length))
        .sort();
}

function hasModule(name) {
    return FILES[MODULE_PREFIX + name + '.d.ts'] !== undefined;
}

function hasClass(name) {
    const short = name.replace(/^Class_/, '');

    return FILES[INTERFACE_PREFIX + short + '.d.ts'] !== undefined;
}

function loadModule(name) {
    return parse(MODULE_PREFIX + name + '.d.ts');
}

function loadClass(name) {
    return parse(INTERFACE_PREFIX + name.replace(/^Class_/, '') + '.d.ts');
}

// ---------------------------------------------------------------------------
// lookup
// ---------------------------------------------------------------------------

function findModule(name) {
    if (hasModule(name))
        return name;

    const lower = name.toLowerCase();

    return moduleNames().find(n => n.toLowerCase() === lower) || null;
}

function findClass(name) {
    const short = name.replace(/^Class_/, '');

    if (hasClass(short))
        return 'Class_' + short;

    const lower = short.toLowerCase();

    for (const n of interfaceNames())
        if (n.toLowerCase() === lower)
            return 'Class_' + n;

    return null;
}

// `const Buffer: typeof Class_Buffer;` in the global module, or the
// `typeof import('global').Buffer` alias of the generated globals file
function findGlobal(name) {
    const globals = loadModule('global');

    if (globals)
        for (const e of globals.exports) {
            const a = aliasTarget(e.decl);

            if (a && a.name === name)
                return a.target;
        }

    const generated = FILES['dts/_builtin/globals.d.ts'] || '';

    if (new RegExp('^declare const ' + name + ': typeof import\\(\'global\'\\).*;', 'm').test(generated))
        return findClass(name) || null;

    return null;
}

function resolve(query) {
    const q = String(query).replace(/^(node:|fibjs:)/, '');
    const dot = q.indexOf('.');

    if (dot < 0) {
        const mod = findModule(q);

        if (mod)
            return { kind: 'module', name: mod };

        const cls = findClass(q);

        if (cls)
            return { kind: 'class', name: cls, alias: q === cls.replace(/^Class_/, '') ? undefined : q };

        const global = findGlobal(q);

        if (global)
            return { kind: 'class', name: global, alias: q };

        return null;
    }

    const modName = findModule(q.slice(0, dot));

    if (!modName)
        return null;

    const member = q.slice(dot + 1);
    const query_name = modName + '.' + member;

    for (const e of loadModule(modName).exports) {
        if (entryName(e.decl) !== member)
            continue;

        const a = aliasTarget(e.decl);

        if (a)
            return { kind: 'class', name: a.target, alias: query_name };

        return entryKind(e.decl) === 'function'
            ? { kind: 'function', module: modName, name: member }
            : { kind: 'member', module: modName, name: member };
    }

    // `http.HttpRequest` reaches the object page as well
    const cls = findClass(member);

    return cls ? { kind: 'class', name: cls, alias: query_name } : null;
}

function suggestions(q) {
    const word = q.toLowerCase().replace(/^(node:|fibjs:)/, '');
    const hits = [];

    for (const n of moduleNames())
        if (n.toLowerCase().includes(word))
            hits.push(n);

    for (const n of interfaceNames())
        if (n.toLowerCase().includes(word))
            hits.push(n);

    return hits.slice(0, 3);
}

// ---------------------------------------------------------------------------
// rendering
// ---------------------------------------------------------------------------

function markdown(text) {
    return text
        .replace(/```[a-zA-Z]*\n/g, '')
        .replace(/`([^`]*)`/g, '$1')
        .replace(/\*\*([^*]*)\*\*/g, '$1')
        .replace(/^\s*[-*]\s+/gm, '· ');
}

// the doc lines are printed as authored: the corpus keeps one line per
// @param/@return tag and the terminal soft-wraps what does not fit, so
// reflowing here would break the tag alignment and double-wrap
function docLines(doc, indent) {
    if (!doc)
        return [];

    return markdown(doc).split('\n').map(l => l ? ' '.repeat(indent) + l : '');
}

// group the overloads of one name so all signatures are listed once, with the
// documentation of the first documented overload under them
function groupByName(list, nameOf) {
    const byName = Object.create(null);
    const order = [];

    for (const e of list) {
        const n = nameOf(e);

        if (!byName[n]) {
            byName[n] = [];
            order.push(n);
        }
        byName[n].push(e);
    }

    return order.map(n => ({ name: n, entries: byName[n] }));
}

const MODULE_GROUPS = [
    ['alias', 'Types'],
    ['function', 'Functions'],
    ['const', 'Constants'],
    ['var', 'Properties']
];

const CLASS_GROUPS = [
    ['constructor', 'Constructors'],
    ['prop', 'Properties'],
    ['method', 'Methods'],
    ['event', 'Events'],
    ['static-prop', 'Static properties'],
    ['static-method', 'Static methods'],
    ['symbol', 'Symbol members']
];

function renderModule(name) {
    const m = loadModule(name);
    const out = [b('# Module ') + b(name)];

    if (m.doc) {
        out.push('');
        out.push(markdown(m.doc));
    }

    for (const [kind, title] of MODULE_GROUPS) {
        const list = m.exports.filter(e => entryKind(e.decl) === kind);

        if (!list.length)
            continue;

        out.push('');
        out.push(b('## ' + title));

        for (const g of groupByName(list, e => aliasTarget(e.decl) ? aliasTarget(e.decl).name : entryName(e.decl))) {
            for (const e of g.entries) {
                const a = aliasTarget(e.decl);

                out.push('  ' + (a
                    ? 'type ' + a.name + ' = ' + a.target
                    : e.decl.replace(/;$/, '')));
            }

            const doc = (g.entries.find(e => e.doc) || {}).doc;

            if (doc)
                out.push(docLines(doc, 6).join('\n'));
        }
    }

    out.push('');
    out.push(dim('(' + moduleNames().length + ' modules / ' +
        interfaceNames().length + ' objects; `fibjs --man ' + name +
        '.<member>` for a member, `fibjs --man -k <word>` to search)'));

    return out.join('\n');
}

function renderClass(name, alias) {
    const c = loadClass(name);
    const out = [b('# Object ') + b(alias ? alias + ' (' + name + ')' : name) +
        (c.base ? dim('  extends ' + c.base) : '')];

    if (c.doc) {
        out.push('');
        out.push(markdown(c.doc));
    }

    for (const [kind, title] of CLASS_GROUPS) {
        const list = c.members.filter(m => memberKind(m.decl) === kind);

        if (!list.length)
            continue;

        out.push('');
        out.push(b('## ' + title));

        for (const g of groupByName(list, m => entryName(m.decl))) {
            for (const m of g.entries)
                out.push('  ' + m.decl.replace(/;$/, ''));

            const doc = (g.entries.find(m => m.doc) || {}).doc;

            if (doc)
                out.push(docLines(doc, 6).join('\n'));
        }
    }

    out.push('');
    out.push(dim('(`fibjs --man --json ' + (alias || name) + '` for structured data)'));

    return out.join('\n');
}

function renderEntries(mod, name) {
    const list = loadModule(mod).exports.filter(e => entryName(e.decl) === name);
    const out = [b('# ') + b(mod + '.' + name)];

    for (const e of list)
        out.push('  ' + e.decl.replace(/;$/, ''));

    const doc = (list.find(e => e.doc) || {}).doc;

    if (doc) {
        out.push('');
        out.push(markdown(doc));
    }

    return out.join('\n');
}

function renderList() {
    const out = [b('# Modules')];
    let line = '  ';

    for (const n of moduleNames()) {
        if ((line + n + '  ').length > 88) {
            out.push(line.trimEnd());
            line = '  ';
        }
        line += n + '  ';
    }
    out.push(line.trimEnd());
    out.push('');
    out.push(dim('(@fibjs/types ' + VERSION + '; `fibjs --man <module>` for a module page)'));

    return out.join('\n');
}

// ---------------------------------------------------------------------------
// search
// ---------------------------------------------------------------------------

function corpusFiles() {
    return Object.keys(FILES)
        .filter(rel => rel.endsWith('.d.ts') && rel !== GLOBALS_FILE && !rel.startsWith('dts/_import'));
}

function renderSearch(word) {
    const w = word.toLowerCase();
    const items = [];

    const emit = (name, doc) => {
        if (items.length < 40)
            items.push('  ' + name + (doc ? '  ' + doc.split('\n')[0].slice(0, 60) : ''));
    };

    for (const rel of corpusFiles()) {
        const parsedFile = parse(rel);

        if (!parsedFile)
            continue;

        const members = parsedFile.kind === 'module' ? parsedFile.exports : parsedFile.members;
        const prefix = parsedFile.kind === 'module'
            ? parsedFile.name + '.'
            : parsedFile.name.replace(/^Class_/, '') + '.';

        for (const e of members)
            if (e.decl.toLowerCase().includes(w) || e.doc.toLowerCase().includes(w))
                emit(prefix + entryName(e.decl), e.doc);
    }

    if (!items.length)
        return null;

    return [b('# Search ') + b(word) + dim('  ' + items.length +
        (items.length >= 40 ? '+ hits (first 40 shown)' : ' hits'))]
        .concat(items).join('\n');
}

// ---------------------------------------------------------------------------
// json
// ---------------------------------------------------------------------------

function toJson(r) {
    const entry = (e, kind) => {
        const a = aliasTarget(e.decl);
        const o = {
            name: a ? a.name : entryName(e.decl),
            kind: a ? 'type' : (kind || entryKind(e.decl)),
            decl: e.decl
        };

        if (a)
            o.target = a.target;
        if (e.doc)
            o.doc = e.doc;

        return o;
    };

    if (r.kind === 'module') {
        const m = loadModule(r.name);

        return {
            kind: 'module',
            name: r.name,
            version: VERSION,
            source: m.file,
            doc: m.doc || undefined,
            exports: m.exports.map(e => entry(e))
        };
    }

    if (r.kind === 'class') {
        const c = loadClass(r.name);

        return {
            kind: 'class',
            name: r.name,
            alias: r.alias || undefined,
            extends: c.base || undefined,
            version: VERSION,
            source: c.file,
            doc: c.doc || undefined,
            members: c.members.map(e => entry(e, memberKind(e.decl)))
        };
    }

    const file = loadModule(r.module);
    const list = file.exports.filter(e => entryName(e.decl) === r.name);

    return {
        kind: entryKind(list[0].decl),
        name: r.module + '.' + r.name,
        version: VERSION,
        source: file.file,
        signatures: list.map(e => e.decl),
        doc: (list.find(e => e.doc) || {}).doc || undefined
    };
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------

function fail(reason) {
    console.error('fibjs --man: ' + reason);
    console.error(USAGE);

    return 1;
}

function main() {
    const argv = process.argv.slice(2);
    const names = [];
    let json = false;
    let list = false;
    let word = null;

    // help wins over the other arguments (R2)
    for (const a of argv)
        if (a === '-h' || a === '--help') {
            console.log(USAGE);
            return 0;
        }

    for (let i = 0; i < argv.length; i++) {
        const a = argv[i];

        if (a === '--no-color')
            color = false;
        else if (a === '--json')
            json = true;
        else if (a === '--list')
            list = true;
        else if (a === '-k' || a === '--search') {
            if (i + 1 >= argv.length || argv[i + 1][0] === '-')
                return fail(a + ' requires a word');
            word = argv[++i];
        } else if (a[0] === '-')
            return fail('unknown option: ' + a);
        else
            names.push(a);
    }

    if (word !== null) {
        const out = renderSearch(word);

        if (out === null) {
            console.error('fibjs --man: no match for: ' + word);

            return 1;
        }

        console.log(out);

        return 0;
    }

    if (list || !names.length) {
        if (json)
            console.log(JSON.stringify({
                kind: 'list',
                version: VERSION,
                modules: moduleNames()
            }));
        else
            console.log(renderList());

        return 0;
    }

    let missing = 0;

    for (const q of names) {
        const r = resolve(q);

        if (!r) {
            missing++;
            console.error('fibjs --man: no entry: ' + q);

            const s = suggestions(q);

            if (s.length)
                console.error('  did you mean: ' + s.join(', '));
            console.error('  try `fibjs --man -k ' + q + '`');
            continue;
        }

        if (json)
            console.log(JSON.stringify(toJson(r), null, 2));
        else if (r.kind === 'module')
            console.log(renderModule(r.name));
        else if (r.kind === 'class')
            console.log(renderClass(r.name, r.alias));
        else
            console.log(renderEntries(r.module, r.name));

        if (!json)
            console.log('');
    }

    return missing ? 1 : 0;
}

process.exit(main());
