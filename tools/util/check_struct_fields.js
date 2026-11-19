/**
 * @description Static check for fields declared `required` in an options
 * record used as a defaulted parameter.
 *
 * A parameter such as `foo((required String name) opts = {})` is generated as
 * `OPT_ARG_NAMED(obj_ptr<T>, new T(), "opts")`: when the caller omits the
 * argument, the default-constructed record is handed to the native code
 * WITHOUT running `T::load()`, so a required field would be skipped silently.
 *
 * A defaulted record is therefore only valid when every TOP-LEVEL field is
 * optional. A required field nested under an optional field is not a
 * violation: while that parent is absent there is nothing to validate, and
 * when it is present the nested `load()` enforces its own fields. A defaulted
 * array (`T[] = []`) is always fine: it has no elements, and each supplied
 * element is loaded and checked on its own.
 *
 * The check runs on the raw definitions, before `expandStructParams()`, so a
 * named struct can be resolved by name and the diagnostic can name both the
 * parameter and the required field.
 *
 * @param {Record<string, import('./ir').IIDLDefinition>} defs
 * @returns {Array<{def: string, member: string, static: boolean, param: string,
 *     field: string, message: string}>}
 */
var { collectStructs, isStructDef } = require('./struct-utils');

function fieldsOf(type, structs) {
    if (Array.isArray(type))
        return type;

    if (typeof type === 'string' && structs[type])
        return structs[type].fields || [];

    return null;
}

module.exports = function (defs) {
    var structs = collectStructs(defs);
    var problems = [];

    Object.keys(defs || {}).forEach(function (defName) {
        var def = defs[defName];
        if (isStructDef(def))
            return;

        (def.members || []).forEach(function (mem) {
            var overs = (mem.overs && mem.overs.length) ? mem.overs : [mem];

            overs.forEach(function (ov) {
                (ov.params || []).forEach(function (p) {
                    if (!p.default || p.isarray)
                        return;

                    var fields = fieldsOf(p.type, structs);
                    if (!fields)
                        return;

                    var required = fields.filter(function (f) { return f.required; })[0];
                    if (!required)
                        return;

                    var record = Array.isArray(p.type) ? 'the inline record' : `'${p.type}'`;
                    problems.push({
                        def: defName,
                        member: mem.name,
                        static: !!mem.static,
                        param: p.name,
                        field: required.name,
                        message: `parameter '${p.name}' is defaulted, but ${record} has required field '${required.name}'; a defaulted record may only declare optional top-level fields`
                    });
                });
            });
        });
    });

    return problems;
};
