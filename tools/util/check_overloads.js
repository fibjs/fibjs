/**
 * @description Static check for overload shadowing.
 *
 * The second (lenient) argument pass of a `String` parameter renders a value
 * through a `toString()` of its own: a Date (ISO), a Buffer (its utf8 bytes),
 * a plain object carrying an own `toString()` and a native object whose class
 * implements one. So when a variant declares `String` at some position and a
 * *later* variant of the same member declares a structured type at the same
 * position, the values that variant could only take in the lenient pass can be
 * swallowed by the earlier `String` parameter and never reach it. Binary
 * values are the exception: a Buffer / TypedArray / ArrayBuffer / DataView
 * keeps its bytes, so those arguments always reach the overload that takes
 * them.
 *
 * Whether the shadowing really happens is decided per position, from the
 * acceptance set of each type — see `accepts()` / `acceptsStrict()` below,
 * which follow the actual `GetArgumentValue` implementations in
 * `fibjs/include/utils.h`, `fibjs/src/base/string.cpp`, `fibjs/src/global/Buffer.cpp`
 * and the per-class `load` functions in `fibjs/include/ifs/*.h`.
 *
 * Variant A declared *before* variant B is reported when there is an argument
 * count k accepted by both variants ([min,max] overlap) such that at some
 * position q < k A takes a `String` parameter while B does not, and B only
 * takes a binary value (a Buffer reaching a `load`) or an own-toString object
 * at that position in its lenient pass, which A's `String` renders and
 * consumes first. Plain strings, numeric strings and Dates are not reported:
 * the canonical order of the conversion preference puts `String` before the
 * numeric forms, so those values reaching A first is the intended dispatch
 * (see plans/idl-union-types-2026-10-02.md, check_unions.js).
 *
 * Declaring the `String` variants last keeps them reachable for real strings
 * while the structured overloads keep winning for everything else.
 *
 * The check never rewrites the corpus: it only reports, so that the author of
 * the IDL decides how to order the overloads.
 *
 * @param {Record<string, import('./ir').IIDLDefinition>} defs
 * @returns {Array<{def: string, member: string, static: boolean, argc: number,
 *     slot: number, stringType: string, shadowedType: string,
 *     stringVariant: string, shadowed: string}>}
 */

// ---------------------------------------------------------------------------
// value classes and per-type acceptance sets
// ---------------------------------------------------------------------------

// Values a numeric-ish converter takes: `ToNumber` / `ToBigInt` succeeds for
// numbers, numeric strings, booleans, null (0) and Date objects. Arrays and
// plain objects only convert when they carry a custom valueOf, which is not
// modeled here (and would not reach an overload on purpose anyway).
var { isUnion, splitUnion } = require('./type-utils');

var NUMERIC = ['num', 'str', 'bool', 'null', 'date'];

// accepts(type) -> array of value classes; '*' means "any value"
var TYPE_ACCEPTS = {
    // the first pass takes real strings; the second pass renders a value
    // through a toString() of its own: a Date (ISO), a Buffer (its utf8
    // bytes), a plain object carrying its own toString() and a native object
    // whose class implements one (a URL renders its href). Everything else is
    // not a string: primitives are never rendered (1 is not "1", null is not
    // "null"), objects that only inherit Object.prototype/Array.prototype
    // rendering, typed arrays / ArrayBuffers (their toString is inherited)
    // and native objects without a toString of their own (Blob reports an
    // error) stay out (fibjs/src/base/string.cpp: has_own_toString).
    // `objstr` is "a plain object that owns a toString"; the finer distinction
    // (own vs inherited) cannot be told from the IDL type, so `plainobj` and
    // `objstr` are treated as overlapping in holdsClass().
    'String': ['str', 'strobj', 'date', 'buffer', 'objstr'],
    'Value': ['*'],     // taken as is
    'Variant': ['*'],   // d = v
    'Boolean': ['*'],   // isolate->toBoolean(v)

    'Integer': NUMERIC,
    'Long': NUMERIC,
    'Number': NUMERIC,
    'Date': NUMERIC,

    'Object': ['plainobj'],     // IsJSObject(): plain literal objects only
    'Array': ['array'],         // v->IsArray()
    'Function': ['func'],       // v->IsFunction()
    'RegExp': ['regexp'],
    'Promise': ['promise'],
    'TypedArray': ['typedarr'],
    'Uint8Array': ['typedarr'],
    'ArrayBuffer': ['arraybuffer', 'typedarr'],

    // Buffer_base::load conversion: Buffer / TypedArray / ArrayBuffer /
    // SharedArrayBuffer / DataView / Array / Blob. Strings are *not* accepted
    // here: the `func(Buffer, encoding)` string shortcut was removed, the
    // `func(string)` overloads handle them.
    'Buffer': ['buffer', 'typedarr', 'arraybuffer', 'array', 'blob'],

    // obj_ptr<object_base> (the lowercase `object` type): native instances
    // (any InternalFieldCount == 1 object) and Buffer
    'object': ['nativeAny', 'buffer'],
};

// Extra classes accepted by the hand-written `load` of a native type. The
// remaining native types fall back to obj_ptr<T>'s generated
// `load() { return CALL_E_TYPEMISMATCH; }` and only take their own instances.
var NATIVE_EXTRA = {
    'Handler': ['array', 'plainobj', 'func'],                    // Function / Object / [Handler]
    'X509Certificate': ['buffer', 'typedarr', 'arraybuffer', 'array', 'blob', 'str'],
    'FileHandle': NUMERIC,                                       // fd
    'Blob': ['array', 'buffer', 'typedarr', 'arraybuffer', 'blob'],// no string: load skips the String form (see gen_code LOAD_SKIP_STRING)
    'Socket': NUMERIC,                                           // default family argument
    'HttpRequest': ['str'],
    'HttpCookie': ['plainobj'],
    'Routing': ['plainobj'],
    'RTCIceCandidate': ['plainobj'],
    'RTCSessionDescription': ['plainobj'],
    'EventEmitter': ['plainobj'],
    'UrlObject': ['plainobj', 'str'],
    'URLSearchParams': ['str', 'plainobj', 'array'],
    'Headers': ['plainobj', 'array'],
    'FormData': ['str', 'blob', 'plainobj'],
    'Lock': [],
};

// `ARG` (and `LOAD_ENTER`) run the whole variant chain twice: the first pass
// uses the strict converters (`bStrict = true`), and only when every variant
// fails that pass the chain is retried with the lenient ones. A type only
// needs the second pass when its lenient acceptance set is wider than the
// strict one — types absent from this table behave the same in both passes
// (e.g. `Object`, `Array`, `Function`, `Value`, `Variant`, `...`).
//
// The strict acceptance sets follow the `bStrict` branches of the converters:
// String takes real strings and String objects, numbers take numbers (Long
// also BigInt), Boolean takes booleans, Date takes dates, Buffer takes Buffer
// instances only and obj_ptr<T> takes its own instances.
var STRICT_ACCEPTS = {
    'String': ['str', 'strobj'],
    'Boolean': ['bool'],
    'Integer': ['num'],
    'Long': ['num', 'bigint'],
    'Number': ['num'],
    'Date': ['date'],
    'Buffer': ['buffer'],
};


function buildExtendOf(defs) {
    var extendOf = {};
    Object.keys(defs).forEach(function (n) {
        extendOf[n] = defs[n].declare && defs[n].declare.extend;
    });
    return extendOf;
}

/**
 * @description The value classes accepted by a parameter of the given IDL
 * type: the argument converter runs through `GetArgumentValue`, whose lenient
 * path widens the accepted set far beyond the nominal type.
 *
 * `Type[]` maps to std::vector<T>: it takes arrays whose elements the T
 * converter accepts, so the array class carries the element acceptance set
 * (`array<...>`).
 * @param {string} type
 * @returns {string[]} value classes, `'*'` means any value
 */
function accepts(type) {
    if (type === '...')
        return ['*'];

    // A union accepts the values of every alternative: the runtime tries them
    // in declaration order within one pass.
    if (isUnion(type)) {
        var merged = [];
        splitUnion(type).forEach(function (t) {
            accepts(t).forEach(function (c) {
                if (merged.indexOf(c) < 0)
                    merged.push(c);
            });
        });
        return merged;
    }

    var element = elementSet(type);
    if (element)
        return ['array<' + element + '>'];

    if (TYPE_ACCEPTS.hasOwnProperty(type))
        return TYPE_ACCEPTS[type].map(function (c) { return c; });

    var list = ['native:' + type];
    if (NATIVE_EXTRA.hasOwnProperty(type))
        list = list.concat(NATIVE_EXTRA[type]);
    return list;
}

// canonical key of the acceptance set of the array element type, or null when
// the type is not an array type
function elementSet(type) {
    if (type.slice(-2) !== '[]')
        return null;

    var inner = type.slice(0, -2);
    var set = accepts(inner);
    if (set.indexOf('*') >= 0)
        return '*';

    return set.sort().join('|');
}

/**
 * @description The value classes accepted by a parameter of the given IDL
 * type in the strict pass (`bStrict = true`).
 * @param {string} type
 * @returns {string[]} value classes, `'*'` means any value
 */
function acceptsStrict(type) {
    if (type === '...')
        return ['*'];

    // the same union merge as accepts(), over the strict sets
    if (isUnion(type)) {
        var merged = [];
        splitUnion(type).forEach(function (t) {
            acceptsStrict(t).forEach(function (c) {
                if (merged.indexOf(c) < 0)
                    merged.push(c);
            });
        });
        return merged;
    }

    // std::vector<T> converts its elements leniently even in the strict pass
    var element = elementSet(type);
    if (element)
        return ['array<' + element + '>'];

    if (STRICT_ACCEPTS.hasOwnProperty(type))
        return STRICT_ACCEPTS[type].map(function (c) { return c; });

    if (TYPE_ACCEPTS.hasOwnProperty(type))
        return TYPE_ACCEPTS[type].map(function (c) { return c; });

    // obj_ptr<T> without a load(): own instances only
    return ['native:' + type];
}

/**
 * @description Does a parameter of this type need the lenient second pass —
 * is the lenient acceptance set wider than the strict one? Only then can a
 * call that "belongs" to this variant fall through into the second pass,
 * where an earlier String variant gets a chance to swallow it.
 */
function needsLenientPass(type, extendOf) {
    return lenientOnly(type, extendOf).length > 0;
}

/**
 * @description The value classes a parameter of this type only accepts in the
 * lenient pass — the values that are left for it after the strict pass failed
 * everywhere. An earlier `String` parameter is tried first in that pass too,
 * so only these values can be swallowed by it.
 * @param {string} type
 * @param {Record<string, string>} extendOf interface inheritance map
 * @returns {string[]} value classes
 */
function lenientOnly(type, extendOf) {
    var strict = acceptsStrict(type);

    return accepts(type).filter(function (c) {
        return !holdsClass(strict, c, extendOf);
    });
}

function holdsClass(set, c, extendOf) {
    if (set.indexOf('*') >= 0 || set.indexOf(c) >= 0)
        return true;

    // `objstr` ("a plain object that owns a toString") is a subset of
    // `plainobj` (any IsJSObject() literal): a set accepting any plain object
    // also takes the one that carries its own toString
    if (c === 'plainobj' && set.indexOf('objstr') >= 0)
        return true;
    if (c === 'objstr' && set.indexOf('plainobj') >= 0)
        return true;

    // `array<...>`: every array a `c` array type accepts must be accepted by
    // one of the array classes in the set
    if (c.lastIndexOf('array<', 0) === 0) {
        var want = c.slice(6, -1);

        for (var i = 0; i < set.length; i++) {
            var a = set[i];
            if (a.lastIndexOf('array<', 0) !== 0)
                continue;

            var have = a.slice(6, -1);
            if (have === '*')
                return true;
            if (want === '*')
                continue;

            var missing = want.split('|').filter(function (x) {
                return have.split('|').indexOf(x) < 0;
            });
            if (missing.length === 0)
                return true;
        }

        return false;
    }

    if (c.lastIndexOf('native:', 0) === 0) {
        if (set.indexOf('nativeAny') >= 0)
            return true;

        var cls = c.substr(7);
        for (var i = 0; i < set.length; i++) {
            var s = set[i];
            if (s.lastIndexOf('native:', 0) !== 0)
                continue;

            var parent = s.substr(7);
            var cur = cls;
            var guard = 0;
            while (cur && guard++ < 32) {
                if (cur === parent)
                    return true;
                cur = extendOf[cur];
            }
        }
    }

    return false;
}

/**
 * @description Can a value accepted by the `from` parameter type be handed to
 * a parameter of the `to` type — is every value `from` accepts also accepted
 * by `to`?
 * @param {string} from
 * @param {string} to
 * @param {Record<string, string>} extendOf interface inheritance map
 */
function canConvert(from, to, extendOf) {
    var a = accepts(from);
    var b = accepts(to);

    for (var i = 0; i < a.length; i++)
        if (!holdsClass(b, a[i], extendOf))
            return false;
    return true;
}

function paramInfo(p) {
    var variadic = p.name === '...' || p.type === '...';
    var type = Array.isArray(p.type)
        ? p.type.map(function (x) { return x.type; }).join('|')
        : p.type;

    if (p.isarray)
        type += '[]';

    return {
        type: variadic ? '...' : type,
        hasDefault: p.default !== undefined && p.default !== null,
        variadic: variadic,
    };
}

function signature(ps, argc, variadic) {
    return ps.slice(0, argc).map(function (p) { return p.type; }).join(',') +
        (variadic ? ',...' : '');
}

module.exports = function (defs) {
    var extendOf = buildExtendOf(defs);
    var problems = [];
    Object.keys(defs).forEach(function (name) {
        var groups = {};

        (defs[name].members || []).forEach(function (m) {
            if (m.memType !== 'method')
                return;

            var key = (m.static ? 'static:' : 'instance:') + m.name;
            (groups[key] = groups[key] || []).push(m);
        });

        Object.keys(groups).forEach(function (key) {
            var ovs = groups[key];
            if (ovs.length < 2)
                return;

            var infos = ovs.map(function (m) {
                var ps = (m.params || []).map(paramInfo);
                var variadic = ps.some(function (p) { return p.variadic; });

                // leading arguments are required, `...` and defaulted
                // arguments can be omitted
                var min = ps.length;
                while (min > 0 && ps[min - 1] &&
                    (ps[min - 1].hasDefault || ps[min - 1].variadic))
                    min--;

                return {
                    m: m,
                    ps: ps,
                    min: min,
                    max: variadic ? Infinity : ps.length,
                    variadic: variadic,
                };
            });

            for (var i = 0; i < infos.length; i++)
                for (var j = i + 1; j < infos.length; j++) {
                    var A = infos[i];
                    var B = infos[j];

                    // argument counts both variants accept
                    var lo = Math.max(A.min, B.min);
                    var hi = Math.min(A.max, B.max);
                    if (lo > hi)
                        continue;

                    // beyond the declared parameters every extra argument is
                    // taken by '...' on both sides, so comparing it is useless
                    var cap = Math.min(hi, Math.max(A.ps.length, B.ps.length));
                    var hit = null;

                    for (var k = lo; k <= cap && !hit; k++) {
                        var shadowed = -1;

                        for (var q = 0; q < k; q++) {
                            var at = A.ps[q] ? A.ps[q].type : '...';
                            var bt = B.ps[q] ? B.ps[q].type : '...';

                            if (at !== 'String' || bt === 'String')
                                continue;

                            // A's String parameter comes first in both passes:
                            // a binary value or an own-toString object that
                            // only B's lenient pass accepts (B's `load`) is
                            // rendered by A and never reaches B. Plain strings,
                            // numeric strings and Dates are not reported: the
                            // canonical order puts String before the numeric
                            // forms, so those reaching A first is the intended
                            // dispatch.
                            var swallowed = lenientOnly(bt, extendOf).some(function (c) {
                                if (c !== 'buffer' && c !== 'plainobj')
                                    return false;

                                return holdsClass(accepts(at), c, extendOf);
                            });

                            if (swallowed)
                                shadowed = q;
                        }

                        if (shadowed >= 0)
                            hit = { k: k, slot: shadowed };
                    }

                    if (hit) {
                        problems.push({
                            def: name,
                            member: A.m.name,
                            static: !!A.m.static,
                            argc: hit.k,
                            slot: hit.slot + 1,
                            stringType: A.ps[hit.slot].type,
                            shadowedType: B.ps[hit.slot].type,
                            stringVariant: signature(A.ps, hit.k, A.variadic),
                            shadowed: signature(B.ps, hit.k, B.variadic),
                        });
                    }
                }
        });
    });

    return problems;
};

module.exports.accepts = accepts;
module.exports.acceptsStrict = acceptsStrict;
module.exports.canConvert = canConvert;
module.exports.needsLenientPass = needsLenientPass;
