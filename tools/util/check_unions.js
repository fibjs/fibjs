/**
 * @description Static check for parameter-position unions (`Buffer|String`).
 *
 * A union is the runtime conversion's preference order: the alternatives are
 * tried in declaration order, first in the strict pass (the value must be of
 * exactly that type) and then in the lenient pass (the value matched nothing
 * and is converted). The two passes accept different values, and the order
 * that works is the one where no alternative converts a value another one
 * after it is meant to take. The acceptance below is the real converter
 * behaviour (fibjs/include/utils.h, fibjs/src/base/string.cpp and
 * fibjs/src/global/Buffer.cpp):
 *
 *   type            strict pass           lenient pass (only for values that
 *                                         matched no alternative exactly)
 *   String          real strings          a Date (ISO), a Buffer (its utf8
 *                                         bytes), an object with its own
 *                                         toString() -- but not a number
 *                                         (1 is not "1"), not {}/[] (their
 *                                         generic rendering is a tag, not a
 *                                         conversion)
 *   Integer/Long/   numbers               ToNumber(): a Date (its time), an
 *   Number                                array ([5] is 5, [] is 0), a
 *                                         boolean, null, an ArrayBuffer or
 *                                         typed array (through its bytes)
 *   Boolean         real booleans         everything
 *   Date            real Dates            the same values as the numeric types
 *   Buffer          real Buffers          ArrayBuffer/DataView/typed arrays/
 *                                         arrays/Blob; numbers and strings
 *                                         are not Buffers
 *   Object          plain JS objects      (no lenient extension)
 *   Value/Variant   everything            everything
 *
 * The alternatives that overlap in the lenient pass must therefore be ordered
 * narrowest first -- Buffer < String < Integer/Long/Number < Date < Boolean --
 * with Value/Variant last (it matches every value, in the strict pass
 * already). `String|Integer` is the order that makes a string out of a Date or
 * of an object carrying a toString(); `Integer|String` would turn those into
 * numbers instead, and a numeric string is a string (taken by the strict pass).
 *
 * Callback shapes are typing-only (the runtime never converts their inner
 * parameters), so unions inside them are not checked here.
 *
 * @param {Record<string, import('./ir').IIDLDefinition>} defs
 * @returns {Array<{def: string, member: string, static: boolean, slot: number,
 *     alternatives: string[], kind: 'order' | 'duplicate' | 'shadow', message: string}>}
 */
var { isUnion, splitUnion } = require('./type-utils');

// value kinds a runtime conversion can accept
var STR = 'string';
var NUM = 'number';
var BIGINT = 'bigint';
var BOOL = 'boolean';
var NULLISH = 'null';
var DATE = 'Date';
var BUF = 'Buffer';
var U8 = 'Uint8Array';
var TA = 'typed array';
var DV = 'DataView';
var AB = 'ArrayBuffer';
var ARRAY = 'array';
var PLAIN = 'plain object';
var NATIVE = 'native object';
var BLOB = 'Blob';
var FUNC = 'function';

var ALL_KINDS = [STR, NUM, BIGINT, BOOL, NULLISH, DATE, BUF, U8, TA, DV, AB, ARRAY, PLAIN, NATIVE, BLOB, FUNC];

// ToNumber() of the numeric family: what a value that matched no alternative
// exactly can still be converted into
var NUMERIC = [DATE, ARRAY, AB, U8, TA, DV, BUF, PLAIN, NATIVE, BOOL, NULLISH];

// the value kinds in the order the runtime converters accept them: the
// narrowest (most specific) conversion first
var ORDER_RANK = {
    'Buffer': 1,
    'String': 2,
    'Integer': 3,
    'Long': 3,
    'Number': 3,
    'Date': 4,
    'Boolean': 5,
};

var TYPE_ACCEPTS = {
    'String': { strict: [STR], lenient: [DATE, BUF, PLAIN, NATIVE] },
    'Boolean': { strict: [BOOL], lenient: ALL_KINDS },
    'Integer': { strict: [NUM], lenient: NUMERIC },
    'Long': { strict: [NUM, BIGINT], lenient: NUMERIC },
    'Number': { strict: [NUM], lenient: NUMERIC },
    'Date': { strict: [DATE], lenient: NUMERIC },
    'Buffer': { strict: [BUF], lenient: [AB, U8, TA, DV, ARRAY, BLOB, BUF] },
    'Object': { strict: [PLAIN], lenient: [] },
    'ArrayBuffer': { strict: [AB], lenient: [AB] },
    'Uint8Array': { strict: [BUF, U8], lenient: [BUF, U8] },
    'TypedArray': { strict: [BUF, U8, TA], lenient: [BUF, U8, TA] },
    'ArrayBufferView': { strict: [BUF, U8, TA, DV], lenient: [BUF, U8, TA, DV] },
    'DataView': { strict: [DV], lenient: [DV] },
    'Array': { strict: [ARRAY], lenient: [ARRAY] },
    'Function': { strict: [FUNC], lenient: [FUNC] },
    'Value': { strict: ALL_KINDS, lenient: ALL_KINDS },
    'Variant': { strict: ALL_KINDS, lenient: ALL_KINDS },
    // FileHandle::load() opens a file descriptor given as a number
    'FileHandle': { strict: ['#FileHandle'], lenient: [NUM] },
};

function modelOf(type) {
    if (TYPE_ACCEPTS[type])
        return TYPE_ACCEPTS[type];

    // a native class or an unknown name: only its own instances match, and
    // there is no lenient construction unless the class declares one above
    return { strict: ['#' + type], lenient: [] };
}

function sameSet(a, b) {
    return a.length === b.length && a.every(function (k) { return b.indexOf(k) >= 0; });
}

function isUniversal(m) {
    return m.strict.length === ALL_KINDS.length;
}

// The value kind of a declared default, or null when it cannot be told from the
// declaration text (a named constant, an expression, a generated constructor).
function classifyDefault(d) {
    if (!d)
        return null;

    var v = d.value !== undefined ? d.value : d.const;
    if (v === undefined || v === null)
        return null;

    v = String(v);
    if (/^".*"$/.test(v))
        return STR;                                  // a string literal
    if (/^-?[0-9]+$/.test(v) || /^-?[0-9]*\.[0-9]+$/.test(v))
        return NUM;                                  // a numeric literal
    if (/^v8::Object::New\(/.test(v))
        return PLAIN;                                // the generated `{}` default

    return null;
}

function takesKind(m, kind) {
    return m.strict.indexOf(kind) >= 0 || m.lenient.indexOf(kind) >= 0;
}

function defaultText(d) {
    var v = d && (d.value !== undefined ? d.value : d.const);
    return v === undefined ? '' : String(v);
}

function examplesFor(kinds) {
    var list = kinds.slice(0, 3).map(function (kind) { return 'a ' + kind; });
    if (kinds.length > 3)
        list.push('...');

    return list.join(', ');
}

module.exports = function (defs) {
    var problems = [];

    Object.keys(defs).forEach(function (defName) {
        var def = defs[defName];

        (def.members || []).forEach(function (mem) {
            // events take part: their parameter list is the listener signature
            // (see plans/idl-event-types-2026-10-03.md), so a union there is
            // subject to the same ordering / shadowing rules
            if ((mem.memType !== 'method' && mem.memType !== 'event') || !mem.params)
                return;

            mem.params.forEach(function (p, slot) {
                if (!isUnion(p.type))
                    return;

                var alternatives = splitUnion(p.type);
                var report = function (kind, message) {
                    problems.push({
                        def: defName,
                        member: mem.name,
                        static: !!mem.static,
                        slot: slot,
                        alternatives: alternatives,
                        kind: kind,
                        message: `${defName}.${mem.name}${mem.static ? ' [static]' : ''}, parameter ${slot + 1} (${p.name}): ${message}; declared as (${alternatives.join(' | ')})`,
                    });
                };

                var seen = {};
                alternatives.forEach(function (alt) {
                    if (seen[alt])
                        report('duplicate', `'${alt}' is declared twice`);
                    seen[alt] = true;
                });

                var models = alternatives.map(modelOf);

                // the default value must be something one alternative can take:
                // the generated stub passes the default through the same
                // converter, so a string default on `Object|Array` can only fail
                var defaultKind = p.isarray ? null : classifyDefault(p.default);
                if (defaultKind && !models.some(function (m) { return takesKind(m, defaultKind); }))
                    report('default', `the default value ${defaultText(p.default)} is a ${defaultKind} and no alternative can take it`);

                models.forEach(function (m, i) {
                    if (!isUniversal(m) || i === models.length - 1)
                        return;

                    report('order', `'${alternatives[i]}' matches every value, so '${alternatives.slice(i + 1).join(' | ')}' declared after it can never be reached — declare '${alternatives[i]}' last`);
                });

                // the values no alternative matches exactly: what the lenient
                // pass has left to convert
                var covered = {};
                models.forEach(function (m) {
                    m.strict.forEach(function (k) { covered[k] = true; });
                });
                var junk = ALL_KINDS.filter(function (k) { return !covered[k]; });

                for (var i = 0; i < alternatives.length; i++) {
                    for (var j = i + 1; j < alternatives.length; j++) {
                        if (alternatives[i] === alternatives[j])
                            continue;

                        if (!isUniversal(models[i]) && sameSet(models[i].strict, models[j].strict)) {
                            report('shadow', `'${alternatives[i]}' and '${alternatives[j]}' accept exactly the same values; '${alternatives[i]}' declared first always wins and '${alternatives[j]}' can never be reached`);
                            continue;
                        }

                        var overlap = models[i].lenient.filter(function (k) {
                            return models[j].lenient.indexOf(k) >= 0 && junk.indexOf(k) >= 0;
                        });

                        if (!overlap.length)
                            continue;

                        var ri = ORDER_RANK[alternatives[i]];
                        var rj = ORDER_RANK[alternatives[j]];
                        if (ri === undefined || rj === undefined || ri <= rj)
                            continue;

                        report('order', `'${alternatives[i]}' is declared before '${alternatives[j]}', and both convert the values that match neither exactly (${examplesFor(overlap)}) — '${alternatives[i]}' takes them; declare '${alternatives[j]}' first`);
                    }
                }
            });
        });
    });

    return problems;
};
