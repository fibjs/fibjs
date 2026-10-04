/**
 * @description Static check for inline callback shapes (`Function(...)`).
 *
 * A bare `Function` parameter becomes `(...args: any[]) => any` in the d.ts,
 * which matches every function. TypeScript picks the first overload that
 * matches, so when a bare `Function` is declared *before* a shaped
 * `Function(...)` in the same overload group (same host, same static flag,
 * same member name) at the same parameter slot, the shaped overload never
 * takes part in the contextual typing of that callback: the user still gets
 * `any` ("filled in, but as good as not").
 *
 * A shaped overload *before* a bare one is a legitimate fallback (error-first
 * callbacks, exotic call styles) and is not reported, mirroring the direction
 * semantics of `check_overloads.js`.
 *
 * The check never rewrites the corpus: it only reports, and the migration can
 * proceed batch by batch; the warnings should go to zero when it is done.
 *
 * Event declarations are out of scope: their parameter list is the listener
 * signature, mapped by gen_dts exactly like a callback shape (a shaped
 * `Function(...)` inside it is rendered as the inner callback, not dropped).
 *
 * @param {Record<string, import('./ir').IIDLDefinition>} defs
 * @returns {Array<{kind: 'shadowing', def: string, member: string, static: boolean,
 *     slot: number, bareOverload: number, shapedOverload: number, bare: string,
 *     shaped: string}>}
 */

function isBareFunction(p) {
    return !!p && p.type === 'Function' && !p.callback;
}

function isShapedFunction(p) {
    return !!p && p.type === 'Function' && !!p.callback;
}

function paramBrief(p) {
    if (!p)
        return '?';
    if (p.type === '...' || !p.type)
        return '...';
    return (p.callback ? 'Function(...)' : p.type) + ' ' + p.name;
}

module.exports = function (defs) {
    var problems = [];

    Object.keys(defs).forEach(function (defName) {
        var def = defs[defName];
        var groups = {};
        var order = [];

        // overlay groups: same (static, member name) means one overload family.
        // Constructors are ordinary members whose name equals the class name.
        def.members.forEach(function (mem) {
            if (mem.memType !== 'method')
                return;

            var key = (mem.static ? 'static ' : '') + mem.name;
            if (!groups.hasOwnProperty(key)) {
                groups[key] = { static: !!mem.static, name: mem.name, members: [] };
                order.push(key);
            }
            groups[key].members.push(mem);
        });

        order.forEach(function (key) {
            var group = groups[key];
            var members = group.members;
            var slotCount = 0;
            members.forEach(function (mem) {
                slotCount = Math.max(slotCount, (mem.params || []).length);
            });

            for (var slot = 0; slot < slotCount; slot++) {
                var bareOverload = -1;
                var reported = false;

                for (var i = 0; i < members.length && !reported; i++) {
                    var p = (members[i].params || [])[slot];

                    if (isBareFunction(p)) {
                        if (bareOverload < 0)
                            bareOverload = i;
                    } else if (isShapedFunction(p) && bareOverload >= 0) {
                        problems.push({
                            kind: 'shadowing',
                            def: defName,
                            member: group.name,
                            static: group.static,
                            slot: slot,
                            bareOverload: bareOverload + 1,
                            shapedOverload: i + 1,
                            bare: paramBrief((members[bareOverload].params || [])[slot]),
                            shaped: paramBrief(p),
                        });
                        reported = true;
                    }
                }
            }
        });
    });

    return problems;
};
