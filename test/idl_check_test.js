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
var path = require('path');

var check_unions = require('../tools/util/check_unions');
var check_overloads = require('../tools/util/check_overloads');
var check_callback_shapes = require('../tools/util/check_callback_shapes');

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
});
