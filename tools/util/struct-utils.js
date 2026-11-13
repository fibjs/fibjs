/**
 * @description Shared options records declared with a top-level
 * `struct Name { ... };`.
 *
 * The C++ side keeps the name (`obj_ptr<ConnectOptions>`), so gen_code must
 * see the original definitions. The typing and documentation generators do
 * not know named records, so they work on a resolved copy where every named
 * struct parameter is replaced by its field list. That keeps the generated
 * d.ts an inline object literal and avoids a separate global type.
 */

var { isUnion, splitUnion } = require('./type-utils');

function isStructDef(def) {
    return !!def && !!def.declare && def.declare.type === 'struct';
}

function collectStructs(defs) {
    var out = {};
    Object.keys(defs || {}).forEach(function (name) {
        if (isStructDef(defs[name]))
            out[name] = defs[name];
    });
    return out;
}

function clone(value) {
    return JSON.parse(JSON.stringify(value));
}

function expandFields(fields, structs, stack) {
    (fields || []).forEach(function (field) {
        if (Array.isArray(field.type))
            field.type = expandFields(field.type, structs, stack);
        else
            field.type = expandType(field.type, structs, stack);
    });
    return fields;
}

function expandType(type, structs, stack) {
    if (typeof type !== 'string')
        return type;

    // A named struct is a parameter-only record; keeping it inside a union or
    // a return position would make the C++ type and the d.ts disagree.
    if (isUnion(type)) {
        splitUnion(type).forEach(function (alt) {
            var name = alt.slice(-2) === '[]' ? alt.slice(0, -2) : alt;
            if (structs[name])
                throw new Error(`[struct] '${type}': a named struct cannot be a union alternative`);
        });
        return type;
    }

    var name = type.slice(-2) === '[]' ? type.slice(0, -2) : type;
    if (!structs[name])
        return type;

    if (stack.indexOf(name) >= 0)
        throw new Error(`[struct] '${name}' is recursive; value records cannot contain themselves`);

    return expandFields(clone(structs[name].fields || []), structs, stack.concat(name));
}

/**
 * @description A deep copy of `defs` with every named struct parameter and
 * field replaced by its field list. Struct declarations themselves are
 * dropped: they are not JS-visible definitions and have no members.
 */
function expandStructParams(defs) {
    var structs = collectStructs(defs);
    var out = {};

    Object.keys(defs || {}).forEach(function (name) {
        var def = defs[name];
        if (isStructDef(def))
            return;

        var copy = clone(def);
        (copy.members || []).forEach(function (mem) {
            (mem.params || []).forEach(function (p) {
                if (Array.isArray(p.type))
                    p.type = expandFields(p.type, structs, []);
                else
                    p.type = expandType(p.type, structs, []);
            });

            // returns, properties and events are not parameter positions
            if (typeof mem.type === 'string' && structs[mem.type])
                throw new Error(`[struct] '${name}.${mem.name}': a named struct is parameter-only`);
        });

        out[name] = copy;
    });

    return out;
}

module.exports = {
    isStructDef,
    collectStructs,
    expandStructParams,
};
