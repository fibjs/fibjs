/**
 * @description Static checks for `Map<T>` and named `map Name: ...;` types.
 *
 * The map syntax is intentionally small: an inline value may only be a named
 * type, an array of a named type or a nested map; unions and callback shapes
 * belong to a named map, where the full `fieldType` grammar is available.
 * Named maps are plain-object dictionaries; the parser rejects a declaration
 * that reuses an interface/module/struct name before this check runs.
 *
 * @param {Record<string, import('./ir').IIDLDefinition>} defs
 * @returns {Array<{def: string, member: string, slot: number,
 *     message: string}>}
 */
var { isUnion, splitUnion, isInlineMapType, mapValueType, isMapType } = require('./type-utils');

function isMapDef(def) {
    return !!def && !!def.declare && def.declare.type === 'map';
}

function mapNames(defs) {
    var names = {};
    Object.keys(defs || {}).forEach(function (n) {
        if (isMapDef(defs[n]))
            names[n] = true;
    });
    return names;
}

function validateInlineMap(type) {
    if (!isInlineMapType(type))
        return null;

    var value = mapValueType(type);
    if (!value)
        return "an inline map has an empty value type";

    if (isUnion(value))
        return "an inline map value cannot be a union; use a named map";

    if (Array.isArray(value))
        return "an inline map value cannot be an inline struct; use a named map";

    if (isInlineMapType(value))
        return validateInlineMap(value);

    var array = /^([A-Za-z_$][\w$]*)\[\]$/.exec(value);
    if (array)
        return validateInlineMap(array[1]);

    if (/^\(/.test(value))
        return "an inline map value cannot be an inline struct; use a named map";

    return null;
}

module.exports = function (defs) {
    var problems = [];
    var maps = mapNames(defs);

    function report(def, member, slot, message) {
        problems.push({ def: def, member: member, slot: slot, message: message });
    }

    // A named map is a declaration, not a class/interface. Reuse the existing
    // names as a collision check: the d.ts namespace cannot hold both.
    Object.keys(defs || {}).forEach(function (name) {
        var def = defs[name];
        if (!isMapDef(def))
            return;

        if (!def.valueType)
            report(name, '', 0, "a named map must declare a value type: `map " + name + ": T;`");

        var value = def.valueType;
        if (Array.isArray(value))
            report(name, '', 0, "a named map value cannot be an inline struct; use a union of named types");

        if (typeof value === 'string') {
            var inlineProblem = validateInlineMap(value);
            if (inlineProblem)
                report(name, '', 0, inlineProblem);
        }

        if (value && isUnion(value)) {
            splitUnion(value).forEach(function (alt) {
                var altProblem = validateInlineMap(alt);
                if (altProblem)
                    report(name, '', 0, altProblem);
            });
        }
    });

    function walkType(defName, member, slot, type) {
        if (Array.isArray(type)) {
            type.forEach(function (field) {
                walkType(defName, member, slot, field.type);
            });
            return;
        }

        if (typeof type !== 'string')
            return;

        if (isInlineMapType(type)) {
            var inlineProblem = validateInlineMap(type);
            if (inlineProblem)
                report(defName, member, slot, inlineProblem);
            return;
        }

        if (isUnion(type)) {
            var mapAlts = [];
            var hasObject = false;
            splitUnion(type).forEach(function (alt) {
                if (alt === 'Object')
                    hasObject = true;
                if (isMapType(alt, maps))
                    mapAlts.push(alt);
                walkType(defName, member, slot, alt);
            });

            if (mapAlts.length > 1)
                report(defName, member, slot, "a union has more than one map alternative (" + mapAlts.join(', ') + "); all expand to the same v8::Local<v8::Object>");

            if (hasObject && mapAlts.length)
                report(defName, member, slot, "a union mixes Object with a map alternative (" + mapAlts.join(', ') + "); both expand to the same v8::Local<v8::Object>");
            return;
        }

        if (type.slice(-2) === '[]')
            walkType(defName, member, slot, type.slice(0, -2));
        else {
            var inlineProblem = validateInlineMap(type);
            if (inlineProblem)
                report(defName, member, slot, inlineProblem);
        }
    }

    Object.keys(defs || {}).forEach(function (name) {
        var def = defs[name];
        if (isMapDef(def))
            return;

        (def.members || []).forEach(function (mem) {
            (mem.params || []).forEach(function (p, slot) {
                walkType(name, mem.name, slot, p.type);
            });

            walkType(name, mem.name, 0, mem.type);
        });

        (def.fields || []).forEach(function (field) {
            walkType(name, field.name, 0, field.type);
        });
    });

    return problems;
};
