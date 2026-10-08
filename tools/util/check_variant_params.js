/**
 * @description Static check for Value/Variant parameter positions.
 *
 * `Value` and `Variant` render as `any` in the generated TypeScript
 * definitions, so a parameter declared with them loses the strictness a union
 * would give (`tsc --check` accepts anything). Every such position is a
 * reviewed decision: the whitelist (tools/util/variant_params_whitelist.json)
 * carries the positions that keep the generic type, each with its reason, and
 * this gate reports
 *
 *   unlisted  a parameter whose declared type is Value/Variant (or a union
 *             containing one) that no whitelist entry matches
 *   stale     a whitelist entry that matches no parameter (the corpus moved on)
 *
 * The review standard, the per-station evidence and the decision record live in
 * plans/value-variant-union-2026-10-05.md (a union brings its own runtime
 * acceptance, so a type-only "tightening" needs the call-form tests too).
 *
 * @param {Record<string, import('./ir').IIDLDefinition>} defs
 * @param {{whitelist?: string | Array}} [options] a whitelist path or a
 *     whitelist array (tests feed the entries directly)
 * @returns {Array<{kind: 'unlisted' | 'stale', def: string, member?: string,
 *     param?: string, slot?: number, type?: string, reason?: string}>}
 */
var fs = require('fs');
var path = require('path');

function loadWhitelist(source) {
    if (Array.isArray(source))
        return source;

    var file = source || path.resolve(__dirname, 'variant_params_whitelist.json');
    return JSON.parse(fs.readFileSync(file, 'utf8')).entries || [];
}

// Value/Variant as an alternative of the declared type (the plain spelling and
// the union member spelling alike)
function isGeneric(type) {
    return String(type || '').split('|').some(function (t) {
        return t === 'Value' || t === 'Variant';
    });
}

function matches(entry, def, member, param) {
    if (entry.def !== def && entry.def !== '*')
        return false;
    if (entry.member !== undefined && entry.member !== member)
        return false;
    if (entry.param !== undefined && entry.param !== param)
        return false;
    return true;
}

module.exports = function (defs, options) {
    var whitelist = loadWhitelist(options && options.whitelist);
    var used = whitelist.map(function () { return false; });
    var problems = [];

    Object.keys(defs).forEach(function (defName) {
        (defs[defName].members || []).forEach(function (mem) {
            (mem.params || []).forEach(function (p, slot) {
                if (!isGeneric(p.type))
                    return;

                var hit = -1;
                for (var i = 0; i < whitelist.length; i++) {
                    if (matches(whitelist[i], defName, mem.name, p.name)) {
                        hit = i;
                        break;
                    }
                }

                if (hit < 0) {
                    problems.push({
                        kind: 'unlisted',
                        def: defName,
                        member: mem.name,
                        param: p.name,
                        slot: slot,
                        type: p.type
                    });
                    return;
                }

                used[hit] = true;
            });
        });
    });

    whitelist.forEach(function (entry, i) {
        if (!used[i]) {
            problems.push({
                kind: 'stale',
                def: entry.def,
                member: entry.member,
                param: entry.param,
                reason: entry.reason
            });
        }
    });

    return problems;
};
