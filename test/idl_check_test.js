// Unit coverage for the IDL validators run by tools/idlc.js:
//
//   tools/util/check_overloads.js        overload shadowing (String variants)
//   tools/util/check_callback_shapes.js  bare Function before Function(...)
//   tools/util/check_unions.js           parameter-position unions (A|B)
//
// The three are pure functions of the parsed IDL (defs -> problems), so the
// cases below feed them synthetic definitions. The last suite runs them over
// the real corpus (idl/): idlc already fails on a problem, this keeps the
// checks themselves honest — a validator that silently stops reporting is a
// regression the compiler cannot show on its own.

var { describe, it } = require('node:test');
var assert = require('assert');
var fs = require('fs');
var os = require('os');
var path = require('path');

var check_unions = require('../tools/util/check_unions');
var check_overloads = require('../tools/util/check_overloads');
var check_callback_shapes = require('../tools/util/check_callback_shapes');
var check_idl_docs = require('../tools/util/check_idl_docs');

function makeDef(members) {
    return {
        Foo: {
            declare: { name: 'Foo', extend: 'object' },
            members: members
        }
    };
}

function method(name, params, options) {
    options = options || {};

    return {
        memType: options.memType || 'method',
        name: name,
        static: !!options.static,
        params: params.map(function (p, i) {
            return {
                type: p.type,
                name: p.name || ('p' + i),
                isarray: !!p.isarray,
                default: p.default !== undefined ? p.default : null,
                callback: p.callback
            };
        })
    };
}

describe('check_unions', () => {
    it('accepts a canonical Buffer|String parameter', () => {
        var problems = check_unions(makeDef([
            method('foo', [{ type: 'Buffer|String' }])
        ]));

        assert.deepStrictEqual(problems, []);
    });

    it('rejects a duplicated alternative', () => {
        var problems = check_unions(makeDef([
            method('foo', [{ type: 'Buffer|Buffer' }])
        ]));

        assert.equal(problems.length, 1);
        assert.equal(problems[0].kind, 'duplicate');
    });

    it('rejects a universal alternative that is not last', () => {
        var problems = check_unions(makeDef([
            method('foo', [{ type: 'Value|Integer' }])
        ]));

        assert.equal(problems.length, 1);
        assert.equal(problems[0].kind, 'order');
    });

    it('enforces the conversion order: String before Integer', () => {
        var bad = check_unions(makeDef([
            method('foo', [{ type: 'Integer|String' }])
        ]));
        assert.equal(bad.length, 1);
        assert.equal(bad[0].kind, 'order');

        // the canonical order of the runtime preference
        var good = check_unions(makeDef([
            method('foo', [{ type: 'String|Integer' }])
        ]));
        assert.deepStrictEqual(good, []);
    });

    it('rejects a default no alternative can take', () => {
        var problems = check_unions(makeDef([
            method('foo', [{ type: 'Buffer|Integer', default: { value: '"oops"' } }])
        ]));

        assert.equal(problems.length, 1);
        assert.equal(problems[0].kind, 'default');

        var fine = check_unions(makeDef([
            method('foo', [{ type: 'Buffer|Integer', default: { value: '0' } }])
        ]));
        assert.deepStrictEqual(fine, []);
    });

    it('covers event parameters (the listener signature)', () => {
        var problems = check_unions(makeDef([
            method('change', [{ type: 'Integer|String' }], { memType: 'event' })
        ]));

        assert.equal(problems.length, 1);
        assert.equal(problems[0].member, 'change');
    });
});

describe('check_overloads', () => {
    it('reports a String overload declared before a Blob load', () => {
        // a Buffer argument reaches the String lenient pass (utf8 rendering)
        // and the Blob overload never sees it
        var problems = check_overloads(makeDef([
            method('foo', [{ type: 'String' }]),
            method('foo', [{ type: 'Blob' }])
        ]));

        assert.equal(problems.length, 1);
        assert.equal(problems[0].member, 'foo');
        assert.equal(problems[0].slot, 1);
        assert.equal(problems[0].stringType, 'String');
        assert.equal(problems[0].shadowedType, 'Blob');
    });

    it('reports a String overload declared before an own-toString object load', () => {
        var problems = check_overloads(makeDef([
            method('foo', [{ type: 'String' }]),
            method('foo', [{ type: 'Headers' }])
        ]));

        assert.equal(problems.length, 1);
        assert.equal(problems[0].shadowedType, 'Headers');
    });

    it('does not report the canonical numeric order', () => {
        // String|Integer is the runtime preference order, and a Buffer
        // argument reaches the Buffer overload in the strict pass
        assert.deepStrictEqual(check_overloads(makeDef([
            method('foo', [{ type: 'String' }]),
            method('foo', [{ type: 'Buffer' }])
        ])), []);

        assert.deepStrictEqual(check_overloads(makeDef([
            method('foo', [{ type: 'String' }]),
            method('foo', [{ type: 'Integer' }])
        ])), []);
    });

    it('does not report a host/port style pair', () => {
        // net.connect(Integer port, String host) then (String url, Integer
        // timeout): the two forms do not share a call
        assert.deepStrictEqual(check_overloads(makeDef([
            method('connect', [{ type: 'Integer' }, { type: 'String' }], { static: true }),
            method('connect', [{ type: 'String' }, { type: 'Integer' }], { static: true })
        ])), []);
    });

    it('does not report a structured overload declared before String', () => {
        assert.deepStrictEqual(check_overloads(makeDef([
            method('foo', [{ type: 'Blob' }]),
            method('foo', [{ type: 'String' }])
        ])), []);
    });
});

describe('check_callback_shapes', () => {
    var shaped = { type: 'Function', callback: { params: [{ type: 'Error', name: 'e' }], ret: null } };

    it('reports a bare Function declared before a shaped Function at one slot', () => {
        var problems = check_callback_shapes(makeDef([
            method('foo', [{ type: 'String' }, { type: 'Function' }]),
            method('foo', [{ type: 'String' }, shaped])
        ]));

        assert.equal(problems.length, 1);
        assert.equal(problems[0].kind, 'shadowing');
        assert.equal(problems[0].slot, 1);
        assert.equal(problems[0].bareOverload, 1);
        assert.equal(problems[0].shapedOverload, 2);
    });

    it('accepts a shaped Function with a bare fallback after it', () => {
        assert.deepStrictEqual(check_callback_shapes(makeDef([
            method('foo', [{ type: 'String' }, shaped]),
            method('foo', [{ type: 'String' }, { type: 'Function' }])
        ])), []);
    });

    it('ignores shaped callbacks in different slots', () => {
        assert.deepStrictEqual(check_callback_shapes(makeDef([
            method('foo', [{ type: 'Function' }, { type: 'String' }]),
            method('foo', [{ type: 'String' }, shaped])
        ])), []);
    });
});

// ---- check_idl_docs: synthetic definitions --------------------------------
// The documentation checker (X1-X10, plans/idl-doc-completion-plan-2026-10-05.md
// section 7.1) is a pure function of the parsed IDL too; `baseline: null` runs
// it in absolute mode, where every rule is zero tolerance.

var ABSOLUTE = { baseline: null, report: false };

function docOf(descript, detail, params, ret) {
    var doc = { descript: descript, detail: detail || [], params: params || [] };
    if (ret)
        doc['return'] = { descript: ret };
    return doc;
}

function jsBlock(code) {
    return '```JavaScript\n' + code + '\n```';
}

function declDoc(blocks) {
    return docOf('Sample definition', [
        'A long enough declaration-level explanation: it says what the definition is',
        'for, when to use it and how to obtain an instance, so the declaration passes',
        'the X5 detail and length requirements without depending on the blocks below.',
        '',
        blocks.join('\n\n')
    ]);
}

function docDef(name, kind, declareDoc, members) {
    return {
        declare: { name: name, type: kind, extend: 'object', doc: declareDoc },
        members: members || []
    };
}

function docMember(name, doc, options) {
    options = options || {};
    return {
        memType: options.memType || 'method',
        name: name,
        static: !!options.static,
        type: options.type !== undefined ? options.type : 'Integer',
        params: (options.params || []).map(function (n) {
            return { type: 'String', name: n, default: null };
        }),
        doc: doc
    };
}

// a definition that satisfies every rule in absolute mode
function compliantDef(name) {
    var memberDoc = docOf('Echoes the value', [
        'Returns the value unchanged; the longer wording keeps the member doc above',
        'the thin threshold and gives X4 a detail section to find.'
    ], [{ name: 'value', descript: 'the value to echo back' }], 'the value');

    return docDef(name, 'module', declDoc([jsBlock('console.log("first");'), jsBlock('console.log("second");')]), [
        docMember('doIt', memberDoc, { params: ['value'] })
    ]);
}

function runDocs(def, options) {
    options = options || ABSOLUTE;
    var defs = {};
    defs[def.declare.name] = def;
    return check_idl_docs(defs, options);
}

function problemsOf(def, rule, options) {
    return runDocs(def, options).filter(function (p) { return p.rule === rule; });
}

describe('check_idl_docs', () => {
    it('accepts a fully documented definition in absolute mode', () => {
        assert.deepStrictEqual(runDocs(compliantDef('Good')), []);
    });

    it('reports X7 when a definition carries fewer than two examples', () => {
        var def = compliantDef('Few');
        def.declare.doc = declDoc([jsBlock('console.log("only one");')]);
        var x7 = problemsOf(def, 'X7');
        assert.equal(x7.length, 1);
        assert.ok(/needs at least 2/.test(x7[0].message));
    });

    it('requires three examples for a big definition', () => {
        var members = [];
        for (var i = 0; i < 20; i++)
            members.push(docMember('m' + i, compliantDef('X').members[0].doc, { params: ['value'] }));
        var def = docDef('Big', 'interface', declDoc([jsBlock('console.log("a");'), jsBlock('console.log("b");')]), members);
        var x7 = problemsOf(def, 'X7');
        assert.equal(x7.length, 1);
        assert.ok(/needs at least 3/.test(x7[0].message));
    });

    it('reports malformed and unknown example markers (X6)', () => {
        var def = compliantDef('Marked');
        def.declare.doc = declDoc([
            jsBlock('// fragment:\nvar a = 1;'),
            jsBlock('// requires: postgres\nvar b = 2;')
        ]);
        var x6 = problemsOf(def, 'X6');
        assert.equal(x6.length, 2);
        assert.ok(x6.some(function (p) { return /non-empty reason/.test(p.message); }));
        assert.ok(x6.some(function (p) { return /not one of/.test(p.message); }));
    });

    it('accepts fragment/requires markers and a top-level await block (X6)', () => {
        var def = compliantDef('Marked');
        def.declare.doc = declDoc([
            jsBlock('// fragment: constructor shape only\nnew Something(1, 2);'),
            jsBlock('// requires: redis\nvar c = redis.createClient();'),
            jsBlock('await Promise.resolve();')
        ]);
        assert.deepStrictEqual(runDocs(def), []);
    });

    it('reports an unmarked block that is not valid JavaScript (X6)', () => {
        var def = compliantDef('Broken');
        def.declare.doc = declDoc([
            jsBlock('const broken = ;'),
            jsBlock('console.log("fine");')
        ]);
        var x6 = problemsOf(def, 'X6');
        assert.equal(x6.length, 1);
        assert.ok(/not valid JavaScript/.test(x6[0].message));
    });

    it('reports X9 when a member example constructs an abstract base class', () => {
        var def = compliantDef('Base');
        def.members[0].doc.detail.push(jsBlock('var s = new Stream();'));
        var x9 = problemsOf(def, 'X9');
        assert.equal(x9.length, 1);
        assert.equal(x9[0].member, 'doIt');
        assert.ok(/`Stream`/.test(x9[0].message));
    });

    it('does not mistake a longer class name for a base class (X9)', () => {
        var def = compliantDef('Base');
        def.members[0].doc.detail.push(jsBlock('var s = new StreamReader();'));
        assert.equal(problemsOf(def, 'X9').length, 0);
    });

    it('reports X10 when a base class member example skips the concrete subclasses', () => {
        var def = compliantDef('Stream');
        def.members[0].doc.detail.push(jsBlock('var s = getStream();'));
        var x10 = problemsOf(def, 'X10');
        assert.equal(x10.length, 1);
        assert.ok(/io\.MemoryStream/.test(x10[0].message));
    });

    it('accepts a base class member example built on a concrete subclass (X10)', () => {
        var def = compliantDef('Stream');
        def.members[0].doc.detail.push(jsBlock('var s = new io.MemoryStream();'));
        assert.equal(problemsOf(def, 'X10').length, 0);
    });

    it('reports X2 when @param names do not match the declaration', () => {
        var def = compliantDef('Sig');
        def.members[0].doc.params = [{ name: 'valeu', descript: 'the value' }];
        var x2 = problemsOf(def, 'X2');
        assert.equal(x2.length, 1);
        assert.ok(/`value`/.test(x2[0].message));
    });

    it('reports X2 when the @param count does not match the declaration', () => {
        var def = compliantDef('Sig');
        def.members[0].doc.params = [
            { name: 'value', descript: 'the value' },
            { name: 'extra', descript: 'not declared' }
        ];
        var x2 = problemsOf(def, 'X2');
        assert.equal(x2.length, 1);
        assert.ok(/@param count 2/.test(x2[0].message));
    });

    it('reports X2 for @return on a declaration without a return type', () => {
        var def = compliantDef('Void');
        def.members[0].type = null;
        var x2 = problemsOf(def, 'X2');
        assert.equal(x2.length, 1);
        assert.ok(/no return type/.test(x2[0].message));
    });

    it('exempts const members from X3/X4', () => {
        var def = compliantDef('Consts');
        def.members.push(docMember('MAX', docOf('The maximum', [], []), { memType: 'const' }));
        assert.deepStrictEqual(runDocs(def), []);
    });

    it('tolerates baseline counts and still rejects a definition outside the baseline', () => {
        var oldDef = compliantDef('Old');
        oldDef.declare.doc = declDoc([jsBlock('console.log("one");')]);
        assert.ok(runDocs(oldDef).some(function (p) { return p.rule === 'X7'; }));

        var dir = fs.mkdtempSync(path.join(os.tmpdir(), 'idl-docs-'));
        var baselinePath = path.join(dir, 'baseline.json');
        fs.writeFileSync(baselinePath, JSON.stringify({
            global: { X7: 1 },
            definitions: { Old: { X7: 1 } }
        }));
        try {
            assert.deepStrictEqual(runDocs(oldDef, { baseline: baselinePath, report: false }), []);

            var newDef = compliantDef('New');
            newDef.declare.doc = declDoc([jsBlock('console.log("one");')]);
            var problems = runDocs(newDef, { baseline: baselinePath, report: false });
            assert.ok(problems.some(function (p) { return p.rule === 'X7'; }));
        } finally {
            fs.unlinkSync(baselinePath);
            try { fs.rmdirSync(dir); } catch (e) { /* the tmp dir is disposable */ }
        }
    });
});

// The corpus itself: idlc runs the same three validators before generating, so
// a failure here means the checks stopped reporting rather than the corpus
// turning bad (idlc would have failed first).
var parser = null;
try {
    parser = require('../tools/util/parser');
} catch (e) {
    // the tool dependencies (pegjs) are installed by the build environment;
    // without them only the synthetic suites above can run
}

describe('IDL corpus', { skip: !parser }, () => {
    it('passes the three idlc validators', () => {
        var defs = parser(path.resolve(__dirname, '../idl'));

        assert.ok(Object.keys(defs).length > 100, 'the corpus must parse');
        assert.deepStrictEqual(check_unions(defs), []);
        assert.deepStrictEqual(check_overloads(defs), []);
        assert.deepStrictEqual(check_callback_shapes(defs), []);
    });

    it('passes the documentation ratchet against the checked-in baseline', () => {
        var defs = parser(path.resolve(__dirname, '../idl'));

        assert.deepStrictEqual(check_idl_docs(defs, {
            baseline: path.resolve(__dirname, '../tools/util/idl_docs_baseline.json'),
            report: false
        }), []);
    });
});
