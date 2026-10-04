/**
 * @description Shared helpers for IDL type strings.
 *
 * A union (`Buffer|String`, `Buffer|KeyObject|Object|String`) is a
 * parameter-position type whose alternatives are joined with `|` in
 * declaration order — that order is the runtime conversion's preference order
 * (see plans/idl-union-types-2026-10-02.md); the C++ side receives one
 * `std::variant`. The IR keeps the plain string so every generator that only
 * compares type names keeps working.
 */

/**
 * @description Is the type a union string (`A|B`)?
 */
function isUnion(type) {
    return typeof type === 'string' && type.indexOf('|') > 0;
}

/**
 * @description The alternatives of a union, or `[type]` for a plain type.
 */
function splitUnion(type) {
    return isUnion(type) ? type.split('|') : [type];
}

/**
 * @description Join alternatives into the canonical union string.
 */
function joinUnion(types) {
    return types.join('|');
}

module.exports = {
    isUnion,
    splitUnion,
    joinUnion,
};
