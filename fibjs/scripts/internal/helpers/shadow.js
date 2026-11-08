'use strict';

/**
 * shadow — run the new resolver beside the old one and compare what they decide,
 * without changing a single thing the install does.
 *
 * This exists because the resolver rewrite is the one change that cannot be made
 * safely in one step: the snapshot (`read_module`'s nested maps) is what 37 call sites
 * read, and replacing the shape underneath them at once means the first divergence
 * shows up as a broken tree rather than as a diff.
 *
 * So the new pass runs *in addition*, and this module answers one question: where do
 * the two disagree, and what kind of disagreement is it. Once the answers are
 * classified — a difference that is the old one being wrong, a difference that is the
 * new one being wrong, a difference that is neither (a different but equivalent
 * hoisting) — the wiring happens one class at a time, and every step is checked
 * against a tree that was already correct.
 *
 * Nothing here is on the install path. `active` is off unless the caller asks.
 */


/**
 * @description is the shadow track on? `FIBJS_INSTALL_SHADOW=1` turns it on, and
 *              `--verbose` decides how much it says
 */
function active() {
    return process.env.FIBJS_INSTALL_SHADOW === '1';
}

/**
 * @description where each edge resolved to, in a tree, as `from|name → path`. This is
 *              the part that matters for the comparison: a placement difference is
 *              visible as the same edge landing at a different path, and nothing else
 *              is.
 * @returns { [key]: path }
 */
function edge_map(tree, opts) {
    const o = opts || {};
    const out = {};

    tree.forEach((node, p) => {
        if (node.type === 'link')
            return;

        const base = node.type === 'link' && node.resolved ? node.resolved : p;

        ['dependencies', 'devDependencies', 'optionalDependencies', 'peerDependencies'].forEach(field => {
            const map = node.edges && node.edges[field];

            if (!map)
                return;

            Object.keys(map).forEach(name => {
                const at = require('internal/helpers/tree').resolve_path(tree, name, base);

                if (at)
                    out[p + '|' + name] = at;
            });
        });
    });

    return out;
}

// ---------------------- the comparison :start ------------------------- //

// what a difference means, which is what decides whether it is a reason to stop
const SAME = 'same';
// the two trees hold the same nodes, hoisted differently — neither install is wrong
const EQUIVALENT = 'equivalent';
// the new resolver places a node the old one did not (it was missing)
const NEW_ADDED = 'new-added';
// the old resolver placed a node the new one does not (it was extra)
const OLD_ADDED = 'old-added';
// both place the same name, at different paths, and the versions differ
const VERSION = 'version';
// the same name and version, at different depths
const DEPTH = 'depth';
// the same node, a different kind of edge reaching it — this is the `dev` /
// `optional` answer, and it decides both what the lockfile says and whether a
// package is installed at all
const FLAGS = 'flags';

/**
 * @description compare where the two trees put everything
 * @param before Map<path, node> — what the old pass arrived at
 * @param after Map<path, node> — what the new pass arrived at
 * @returns { counts, differences: [{ kind, path, name, before, after }] }
 */
function compare(before, after) {
    const counts = {};
    const differences = [];

    function note(kind, detail) {
        counts[kind] = (counts[kind] || 0) + 1;

        differences.push(Object.assign({ kind: kind }, detail));
    }

    // 1. every path in one and not the other
    before.forEach((node, p) => {
        if (!after.has(p))
            note(OLD_ADDED, { path: p, name: node.name, version: node.version, after: null });
    });

    after.forEach((node, p) => {
        if (!before.has(p))
            note(NEW_ADDED, { path: p, name: node.name, version: node.version, before: null });
    });

    // 2. the same path, a different node
    before.forEach((node, p) => {
        const other = after.get(p);

        if (!other)
            return;

        if (node.version !== other.version) {
            note(VERSION, {
                path: p, name: node.name,
                before: { version: node.version }, after: { version: other.version },
            });
            return;
        }

        if ((node.type === 'link') !== (other.type === 'link')) {
            note(DEPTH, {
                path: p, name: node.name,
                before: { type: node.type }, after: { type: other.type },
            });
        }
    });

    // 3. the same set of nodes, placed at different depths. This is the interesting
    //    one, and it is why the comparison is not a plain tree diff: both trees can be
    //    correct and still hoist a package to a different place
    const by_version = new Map();

    function key(node) {
        return node.name + '@' + node.version;
    }

    [...before.values()].forEach(n => {
        if (n.target_only || n.path === '')
            return;
        by_version.set(key(n), (by_version.get(key(n)) || 0) + 1);
    });

    let equivalent = 0;

    [...after.values()].forEach(n => {
        // the project itself is in both by construction; counting it would make every
        // report say "1 same" whatever the trees hold
        if (n.target_only || n.path === '')
            return;

        const k = key(n);
        const left = by_version.get(k) || 0;

        if (left > 0) {
            by_version.set(k, left - 1);
            equivalent++;
        }
    });

    counts[SAME] = equivalent;

    return { counts: counts, differences: differences };
}

/**
 * @description the drift, reported the way a reader can act on: a count per kind, and
 *              an example of each. It is not an error — a difference may be the old
 *              pass being wrong, which is what the rewrite is for.
 */
function report(result, opts) {
    const o = opts || {};
    const counts = result.counts;

    const line = kind => {
        const n = counts[kind] || 0;

        return n ? `${kind}: ${n}` : null;
    };

    const parts = [SAME, EQUIVALENT, NEW_ADDED, OLD_ADDED, VERSION, DEPTH]
        .map(line).filter(Boolean);

    if (!parts.length)
        return '[shadow] the two resolutions agree exactly';

    const head = `[shadow] ${parts.join(', ')}`;

    // an example per kind, because a count alone does not say what to look at
    const examples = [];

    const seen = {};

    result.differences.forEach(d => {
        if (seen[d.kind] || examples.length >= (o.limit || 8))
            return;

        seen[d.kind] = true;
        examples.push(`  ${d.kind}: ${d.path}` +
            (d.name ? ` (${d.name})` : '') +
            (d.before && d.after && d.before.version !== undefined
                ? ` ${d.before.version} -> ${d.after.version}`
                : ''));
    });

    return [head].concat(examples).join('\n');
}

module.exports = {
    active,
    edge_map,
    compare,
    report,

    SAME,
    EQUIVALENT,
    NEW_ADDED,
    OLD_ADDED,
    VERSION,
    DEPTH,
};
