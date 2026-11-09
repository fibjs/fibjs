'use strict';

/**
 * tree — the three trees the installer decides on, the way npm does it
 * (`@npmcli/arborist`). See plans/installer-three-tree-refactor.md.
 *
 *   actual   what is on the disk (node_modules and the directories links point at)
 *   virtual  what the manifest and the lockfile describe
 *   ideal    what the tree *should* become: virtual, with the requested
 *            add/remove/update applied
 *
 * Every decision the installer makes is one of three questions, and each has exactly
 * one answer:
 *
 *   - which nodes exist and where      → the ideal tree
 *   - what to do about a path on disk  → `diff(actual, ideal)`
 *   - what to write in the lockfile    → the ideal tree, serialized
 *
 * A node is a plain object, and a tree is a `Map` from lockfile path
 * (`node_modules/a`, `packages/member/node_modules/b`) to node. Paths are the
 * lockfile's, so nothing has to convert between "how the tree is shaped" and "how the
 * file describes it".
 */

const fs = require('fs');
const path = require('path');

// ---------------------- nodes :start ------------------------- //

/**
 * @description the kind of a node's source, which is what decides how it is put on
 *              the disk:
 *              - `registry`  a tarball from a registry
 *              - `git`       a repository at a pinned commit
 *              - `link`      a directory of the project (a workspace member, or a
 *                            `file:` dependency): installed as a symlink, never
 *                            fetched
 *              - `bundle`    a copy that came inside a parent's tarball: it arrives
 *                            with the parent and is never installed on its own
 */
const REGISTRY = 'registry';
const GIT = 'git';
const LINK = 'link';
const BUNDLE = 'bundle';

/**
 * @description one node of a tree
 * @param p the lockfile path it sits at
 */
function make_node(p, fields) {
    const node = {
        path: p,
        name: fields.name,
        version: fields.version,

        type: fields.type || REGISTRY,
        // where the bytes come from: `resolved`/`integrity` for a registry or git
        // node, `local_path` for a link
        resolved: fields.resolved,
        integrity: fields.integrity,
        local_path: fields.local_path,

        // a link's target is a project directory, not an installation of its own:
        // it has an entry in the lockfile and nothing is installed at that path
        target_only: fields.target_only === true,
        // the package's own `bundleDependencies`, as a name set
        bundled: fields.bundled || null,

        // the declared ranges this node asks for
        edges: fields.edges || {},
        // dev / optional / devOptional / peer, from the graph
        flags: fields.flags || {},
        // os / cpu / libc, so the platform can be answered without the manifest
        platform: fields.platform || {},
        // the raw manifest, which is what a write back projects
        manifest: fields.manifest || null,
        // what npm records for a package whose manifest runs an install script
        has_install_script: fields.has_install_script === true,
        bin: fields.bin,
        license: fields.license,
        engines: fields.engines,
        funding: fields.funding,
    };

    return node;
}

/**
 * @description a link target: the directory a link points at. It is not installed
 *              anywhere; it exists so the lockfile can describe the package (its
 *              dependencies are what an install of that project would read).
 *
 *              It is *not* a `LINK`: npm's link target is an ordinary node of the
 *              directory — `Link#target` — and only the entry in `node_modules` is a
 *              link. Marking both as links made the resolver skip the target's edges,
 *              which is every workspace member's dependencies
 */
function make_target(p, fields) {
    const node = make_node(p, Object.assign({}, fields, { type: REGISTRY }));

    node.target_of = fields.target_of || null;
    node.target_only = true;

    return node;
}

// ---------------------- manifest projection :start ------------------------- //

/**
 * @description the names a package ships inside its own tarball (npm's
 *              `bundleDependencies`, also spelled `bundledDependencies`): `true` means
 *              every declared dependency, the array form names them
 * @returns { [name]: true } | null
 */
function bundled_names(manifest) {
    const raw = manifest.bundleDependencies !== undefined ? manifest.bundleDependencies
        : manifest.bundledDependencies;

    if (raw === true) {
        const names = {};

        Object.keys(manifest.dependencies || {}).forEach(n => { names[n] = true; });
        Object.keys(manifest.optionalDependencies || {}).forEach(n => { names[n] = true; });

        return Object.keys(names).length ? names : null;
    }

    const names = {};

    (Array.isArray(raw) ? raw : Object.keys(raw || {})).forEach(n => { names[n] = true; });

    return Object.keys(names).length ? names : null;
}

/**
 * @description the peer dependencies that have to be installed along with a package.
 *              npm (>= 7) installs them like regular dependencies, except the ones
 *              marked optional in `peerDependenciesMeta`, and the ones a workspace
 *              provides
 */
function peer_edges(manifest) {
    const peers = Object.assign({}, manifest.peerDependencies || {});
    const meta = manifest.peerDependenciesMeta || {};

    Object.keys(peers).forEach(k => {
        if (meta[k] && meta[k].optional) {
            delete peers[k];
            return;
        }

        // `workspace:` is answered by the workspace itself, and a registry lookup
        // cannot resolve it
        if (typeof peers[k] === 'string' && peers[k].indexOf('workspace:') === 0)
            delete peers[k];
    });

    return peers;
}

/**
 * @description the edges a manifest declares, in the shape a node carries them
 */
function edges_of(manifest) {
    return {
        dependencies: Object.assign({}, manifest.dependencies || {}),
        devDependencies: Object.assign({}, manifest.devDependencies || {}),
        optionalDependencies: Object.assign({}, manifest.optionalDependencies || {}),
        peerDependencies: peer_edges(manifest),
    };
}

/**
 * @description the platforms a package is for, read off its manifest
 */
function platform_of(manifest) {
    return {
        os: manifest.os,
        cpu: manifest.cpu,
        libc: manifest.libc,
    };
}

// ---------------------- paths :start ------------------------- //

/**
 * @description the package name a lockfile path refers to: `node_modules/a` and
 *              `node_modules/x/node_modules/@s/a` both name `a` / `@s/a`
 * @returns null for a path that is not inside a node_modules directory
 */
function name_from_path(p) {
    const marker = 'node_modules/';
    const idx = p.lastIndexOf(marker);

    return idx < 0 ? null : p.slice(idx + marker.length);
}

/**
 * @description the lockfile path of a directory, relative to the project and with `/`
 *              separators
 */
function relative_path(root, dir) {
    const rel = path.relative(root || '.', dir).replace(/\\/g, '/');

    return rel === '' ? '' : rel.replace(/\/+$/, '');
}

/**
 * @description the location of the node that holds `p` in its own `node_modules`:
 *              `node_modules/a/node_modules/b` → `node_modules/a`, and
 *              `node_modules/a` → the project. A directory that is not under a
 *              `node_modules` at all (`packages/member`) is a project directory of its
 *              own, and what holds it is the project as well
 * @returns null for the project itself
 */
function parent_location(p) {
    const idx = p.lastIndexOf('/node_modules/');

    if (idx >= 0)
        return p.slice(0, idx);

    return p === '' ? null : '';
}

/**
 * @description the chain a placement walks: the node itself, then whatever holds it,
 *              out to the project — npm's `Node#ancestry()`
 */
function ancestors(tree, start) {
    const out = [];
    let cur = start === null || start === undefined ? '' : start;

    for (;;) {
        out.push(cur);

        if (cur === '')
            break;

        const up = parent_location(cur);

        if (up === null || up === cur)
            break;

        cur = up;
    }

    return out;
}

/**
 * @description the paths a dependency declared at `from` is looked up at, from the
 *              dependent outwards — npm resolves `<dir>/node_modules/<name>` and then
 *              walks up one directory at a time to the root
 */
function lookup_paths(name, from) {
    return ancestors(null, from).map(dir => (dir ? dir + '/' : '') + 'node_modules/' + name);
}

/**
 * @description the path a dependency declared at `from` resolves to in that tree:
 *              the nearest entry along the dependent's own directories, outwards.
 *              This is the placement rule a `require()` follows at run time, and the
 *              one both the resolver and the diff measure with
 * @param tree Map<path, node>
 */
function resolve_path(tree, name, from) {
    const paths = lookup_paths(name, from);

    for (let i = 0; i < paths.length; i++) {
        if (tree.has(paths[i]))
            return paths[i];
    }

    return null;
}

/**
 * @description the path that serves a dependency declared at `from`, or null when the
 *              copy `require()` would find does not satisfy the spec.
 *
 *              The nearest entry wins and the walk stops there: that is what
 *              `require()` does, so a further-out copy of a satisfying version is *not*
 *              an answer — the nearer one is what the dependent gets, the tree is
 *              `invalid`, and the repair has to place a copy where the dependent can
 *              see it. Continuing outwards and finding the satisfying copy made the
 *              repair believe an invalid edge was served, which is how `is-odd@0.1.2`
 *              came to depend on a root `is-number@7.0.0`.
 * @param tree Map<path, node>
 * @param name the package name
 * @param spec the declared range
 * @param from the path the requirement is read from
 */
function serving_path(tree, name, spec, from) {
    const at = resolve_path(tree, name, from);

    if (!at)
        return null;

    return satisfies_node(tree.get(at), spec) ? at : null;
}

// ---------------------- the graph :start ------------------------- //

// the edge kinds, and what a node reached only through them is
const EDGE_KINDS = ['dependencies', 'devDependencies', 'optionalDependencies', 'peerDependencies'];

/**
 * @description which kind of edge reaches each node — npm's `calc-dep-flags`, and
 *              deliberately the same algorithm rather than an equivalent-looking one.
 *
 *              The rules read as "a node only a dev edge reaches is dev", and that is
 *              true, but the answer is not the one a reachability walk produces. npm
 *              starts every flag `true` and *unsets* as it walks edges, and the
 *              difference is visible: a node under an optional parent keeps
 *              `devOptional` even though nothing dev reaches it, because the edge that
 *              would have unset it is `optional` and npm's test is
 *              `!node.devOptional && !node.dev && !node.optional && !dev && !optional`.
 *
 *              A reachability walk written from the prose agrees on `dev` and disagrees
 *              on the overlap — which is what decides whether `--omit=optional` takes a
 *              package off the disk. So this is npm's loop, kept in npm's shape.
 * @param tree Map<path, node>
 * @param roots the paths the walk begins at: the project, and every link target that is
 *              a project of its own (a workspace member)
 * @returns { dev, optional, devOptional, peer } — each a `{ [path]: true }` map
 */
function calc_flags(tree, roots) {
    const flags = { dev: {}, optional: {}, devOptional: {}, peer: {} };

    // Every node starts in all four trees, exactly as npm's `resetDepFlags` leaves
    // them — with one exception that is easy to miss: the *root* starts clean. npm's
    // `#rootNodeFromPackage` builds it with `dev: false, optional: false, peer: false`,
    // and that is what makes the first step do any work: an edge out of the root sees
    // `!subject.dev` and unsets the flag on what it reaches. A root that started `true`
    // like everything else would mark the whole tree dev, and the walk would agree with
    // itself about nothing
    const state = new Map();

    // Every node starts in all four trees, which is npm's `resetDepFlags` — applied to
    // the root as well. It is easy to read `calc-dep-flags` and conclude otherwise,
    // because the root *is* visited and its flags are not looked at; but npm's tree is
    // reset before the walk, so the root starts `true` like everything else.
    //
    // Only `dev` and `optional` are cleared on the root here, and the reason is where a
    // flag comes from. `dev` and `optional` are cleared by an edge *out* of a node that
    // is not dev/optional, so a root holding them true would mark the whole tree dev —
    // nothing would ever unset it, and `--omit=dev` would install nothing. `peer` is
    // the other direction: npm's rule unsets it only when the subject is not peer *and*
    // the edge is not peer, so a root holding `peer: true` is exactly what marks a
    // package reached only by a peer edge (`react`, pulled in by `react-dom`'s
    // peerDependency) as `peer: true` in the lockfile. Clearing it on the root made
    // every such entry come out with no flag at all
    tree.forEach((node, p) => {
        state.set(p, p === ''
            ? { dev: false, optional: false, devOptional: false, peer: false }
            : { dev: true, optional: true, devOptional: true, peer: true });
    });

    // Every root starts clean for `dev` and `optional` for the same reason, and a
    // member is a root: nothing reaches it, so nothing would unset the initial `true`
    // and everything below it came out dev *and* optional. npm has the rule by
    // construction — its root nodes are built with those flags cleared
    (roots || []).forEach(r => {
        if (r === '' || !state.has(r))
            return;

        state.set(r, { dev: false, optional: false, devOptional: false, peer: false });
    });

    const seen = new Set();
    // what has been walked, which is not the same as what has been reached: a node is
    // queued once when it is reached, and again only when its own flags changed
    const processed = new Set();
    const queue = [];

    // Every root the caller names is seeded, and each starts clean. npm walks from the
    // project alone because its tree has an edge for `workspaces`; this model has one
    // too (`build_ideal` adds it), but a snapshot projected from `read_module` does
    // not — a member's own tree is recorded under the link, with no edge from the root
    // to reach it by. Naming the member as a root is what stands in for that edge, and
    // it is why the caller passes more than `''`.
    roots.forEach(r => { queue.push(r); });

    while (queue.length) {
        const p = queue.pop();
        const node = tree.get(p);

        if (!node || state.get(p) === undefined)
            continue;

        // a link's flags are its target's: npm copies them across, so a member reached
        // only through a dev edge marks what it needs as dev too
        if (node.type === LINK && node.resolved) {
            // the link is reached, and what the diff asks about it is whether the
            // package it installs is wanted — so it has to be in `seen`, or the final
            // pass skips it and it comes out with no flags at all while its target is
            // correctly dev
            seen.add(p);
            processed.add(p);

            const target = node.resolved;
            const mine = state.get(p);
            const theirs = state.get(target);

            if (theirs) {
                let changed = false;

                ['dev', 'optional', 'devOptional', 'peer'].forEach(kind => {
                    if (theirs[kind] && !mine[kind]) {
                        theirs[kind] = false;
                        changed = true;
                    }
                });

                // The target is a node of the walk too: it is the directory the link
                // installs, and its own edges are what a member's dependencies hang
                // from — so it is processed like any other node. It has to be queued
                // *before* it is marked seen, or the guard below cancels the visit.
                if (!processed.has(target) || changed)
                    queue.push(target);
            }

            continue;
        }

        const mine = state.get(p);

        seen.add(p);
        processed.add(p);

        for (const kind of EDGE_KINDS) {
            const map = node.edges && node.edges[kind];

            if (!map)
                continue;

            const is_peer = kind === 'peerDependencies';
            const is_optional = kind === 'optionalDependencies';
            const is_dev = kind === 'devDependencies';

            Object.keys(map).forEach(name => {
                const to = resolve_path(tree, name, p);

                // a dependency nothing serves has no flags to unset
                if (!to || state.get(to) === undefined)
                    return;

                const theirs = state.get(to);

                let changed = false;

                if (theirs.dev && !mine.dev && !is_dev) {
                    theirs.dev = false;
                    changed = true;
                }

                if (theirs.optional && !mine.optional && !is_optional) {
                    theirs.optional = false;
                    changed = true;
                }

                // npm's rule, in npm's words: only unset the overlap when neither side
                // is dev *or* optional, because unsetting any earlier would stop the
                // overlap from propagating deeper
                if (theirs.devOptional && !mine.devOptional &&
                    !mine.dev && !mine.optional && !is_dev && !is_optional) {
                    theirs.devOptional = false;
                    changed = true;
                }

                if (theirs.peer && !mine.peer && !is_peer) {
                    theirs.peer = false;
                    changed = true;
                }

                // A link reached by an edge is walked like the node it installs: the
                // link has no edges of its own, so skipping it leaves the directory it
                // points at — and everything only a member depends on — never visited.
                if (changed || !processed.has(to))
                    queue.push(to);
            });
        }
    }

    // A link and its target are one package, and npm keeps one set of flags for the
    // pair: `calc-dep-flags` ends with `if (node.isLink) Object.assign(node,
    // flags(node.target))`, so the link carries the directory's answer. Without that
    // the link node came out with no flags at all while its target was correctly dev,
    // and `--omit=dev` kept the symlink on the disk — the target was dev and the
    // thing being removed was not
    tree.forEach((node, p) => {
        if (node.type !== LINK || !node.resolved)
            return;

        const theirs = state.get(node.resolved);
        const mine = state.get(p);

        if (!theirs || !mine)
            return;

        // the link is the symlink; the directory is the package. What the diff asks
        // about the symlink is whether the package it installs is wanted, so the answer
        // is the directory's
        ['dev', 'optional', 'devOptional', 'peer'].forEach(kind => {
            mine[kind] = theirs[kind];
        });
    });

    state.forEach((v, p) => {
        if (p === '')
            return;

        // A node the walk never reached is one nothing declares — npm calls it
        // `extraneous` and the answer is "no flags", not "in every tree". Leaving the
        // initial `true` in place made an orphan come out as dev *and* optional *and*
        // peer, which is the opposite of what it is
        if (!seen.has(p))
            return;

        // npm's last step: a node that is dev or optional on its own is not the overlap
        if (v.devOptional && (v.dev || v.optional))
            v.devOptional = false;

        if (v.dev) flags.dev[p] = true;
        if (v.optional) flags.optional[p] = true;
        if (v.devOptional) flags.devOptional[p] = true;
        if (v.peer) flags.peer[p] = true;
    });

    return flags;
}

// ---------------------- placement :start ------------------------- //

// what a check answers: the dependency is already there and good, it can go here
// (nothing there), or the copy there has to go — or nothing can be put here
const OK = 'ok';
const KEEP = 'keep';
const REPLACE = 'replace';
const CONFLICT = 'conflict';

/**
 * @description the deepest node that could hold `name` for `start`, walking outwards:
 *              the first ancestor that is the project, has no holder, or does not
 *              itself ask for `name` as a peer. This is npm's
 *              `deepest-nesting-target.js` — 18 lines there, and the whole reason a
 *              package is *hoisted* rather than nested one level under its dependent
 * @param tree Map<path, node>
 * @param start the path the requirement is read from
 */
function nesting_target(tree, start, name) {
    for (const p of ancestors(tree, start)) {
        const node = tree.get(p);

        // the project is where the walk stops: it is as far out as a package can go
        if (!node || p === '')
            return p;

        const edge = node.edges && node.edges.peerDependencies
            ? node.edges.peerDependencies[name]
            : undefined;

        // a peer edge makes the walk skip *past* that node rather than place below it:
        // npm returns the first ancestor that does not itself require `name` as a peer,
        // which is one level further out
        if (edge === undefined)
            return p;
    }

    return '';
}

/**
 * @description does that string spec admit that node? Shared by the resolver and the
 *              checks that read a lockfile back
 */
function satisfies_node(node, spec) {
    if (!node || typeof spec !== 'string')
        return false;

    // a local spec is answered by the directory, not by a version
    if (spec.indexOf('file:') === 0 || spec.indexOf('link:') === 0 ||
        spec.indexOf('workspace:') === 0)
        return true;

    if (spec === '*' || spec === '' || spec === 'latest')
        return !!node.version;

    if (spec.indexOf('npm:') === 0) {
        const target = spec.slice(4);
        const at = target.lastIndexOf('@');
        const real = at > 0 ? target.slice(0, at) : target;
        const range = at > 0 ? target.slice(at + 1) : '';

        if ((node.real_name || node.name) !== real)
            return false;

        return !range || require('internal/helpers/semver').satisfies(node.version, range);
    }

    if (!node.version)
        return false;

    return require('internal/helpers/semver').satisfies(node.version, spec);
}

/**
 * @description the path a package has at a given place
 */
function place_path(parent, name) {
    return (parent ? parent + '/' : '') + 'node_modules/' + name;
}

/**
 * @description where `name` goes, for a dependency read from `from`:
 *
 *              - the shallowest slot along the chain the dependent can reach that is
 *                free — which is what hoisting *is*, and why nothing has to move
 *                anything afterwards;
 *              - failing that, the nearest slot, replacing the copy there (the
 *                dependent cannot see anything else, so the copy it needs has to be
 *                beside it).
 *
 *              The chain runs from `nesting_target` down to the dependent, which is
 *              the segment npm's `PlaceDep` walks: a peer stops it going further out,
 *              and nothing ever goes above the project.
 * @param tree Map<path, node>
 * @param from the path the dependency is read from
 * @param name the package name
 * @param opts { keep_at: a path that already holds a copy to keep }
 * @returns { path, replace: node|null }
 */
function place_dep(tree, from, name, opts) {
    const o = opts || {};
    const start = nesting_target(tree, from, name);

    // npm's `PlaceDep` walks `start.ancestry()` — from the target outwards to the
    // project — keeping the last target it could use and breaking at the first it
    // could not:
    //
    //     for (const target of start.ancestry()) {
    //       if (cpd.canPlace !== CONFLICT) this.canPlace = cpd
    //       else break
    //     }
    //
    // Walking outwards and keeping the *last* acceptable slot is what hoists: the
    // outermost free place wins, and nothing has to be moved there afterwards. A slot
    // holding a copy that does not fit is the conflict that ends the walk, so the copy
    // the dependent needs lands in the last good slot — which is nearer to it than the
    // one in the way, exactly as a nested tree requires.
    const chain = ancestors(tree, start);

    let best = null;

    for (const holder of chain) {
        const at = place_path(holder, name);
        const occupant = tree.get(at);

        // free, or already the very same node: a usable slot
        if (!occupant) {
            best = { path: at, replace: null };
            continue;
        }

        if (occupant.name === name && (o.version ? occupant.version === o.version : true)) {
            best = { path: at, replace: null };
            continue;
        }

        // something else is there. Replacing it is a placement like any other, but only
        // when nothing that currently resolves to it would be broken by the swap —
        // npm's `Node#canReplace`. A copy the project itself asks for at another range
        // stays where it is, and the new one is nested under the node that needs it
        if (o.accepts && !o.accepts(at, occupant)) {
            best = { path: place_path(chain[0], name), replace: null };
            break;
        }

        best = { path: at, replace: occupant };
        break;
    }

    if (best)
        return best;

    return { path: place_path(chain[0], name), replace: null };
}

/**
 * @description would replacing the copy at `at` break something that resolves to it
 *              today? npm's `Node#canReplace` walks the edges that land on the node and
 *              refuses the swap when any of them stops being satisfied. That rule is
 *              what keeps the tree from oscillating: without it, two dependents asking
 *              for incompatible ranges of the same package take turns replacing the
 *              root copy, and which one wins is the order the queue happened to have.
 * @param tree Map<path, node>
 * @param at the path the copy sits at
 * @param occupant what is there
 * @param spec the range that wants to take its place
 */
function replacement_is_safe(tree, at, occupant, incoming, skip) {
    // a link is a directory of the project, not a copy to be swapped for another
    // version
    if (occupant.type === LINK)
        return false;

    // every edge that resolves to this path today has to survive the swap — which
    // means the *incoming* node has to satisfy them, not the one being taken away.
    // Getting that backwards makes the root copy of a package fair game for whichever
    // dependent asked for it last. The edge doing the asking is not one of them: it is
    // the one that wants the new copy
    for (const [p, node] of tree) {
        const edges = node.edges || {};

        for (const field of ['dependencies', 'optionalDependencies', 'peerDependencies', 'devDependencies']) {
            const map = edges[field];

            if (!map)
                continue;

            for (const name of Object.keys(map)) {
                if (name !== occupant.name)
                    continue;

                if (skip && skip.from === p && skip.name === name)
                    continue;

                // only the edges that actually land here. `resolve_path` answers the
                // same question the resolver did, so the two cannot disagree
                if (resolve_path(tree, name, node_resolution_base(tree, p, node)) !== at)
                    continue;

                if (!satisfies_node(incoming, map[name]))
                    return false;
            }
        }
    }

    return true;
}

/**
 * @description where a node's dependencies are resolved from: a link's are read in the
 *              directory it points at
 */
function node_resolution_base(tree, p, node) {
    if (node.type === LINK && node.resolved)
        return node.resolved;

    return p;
}

/**
 * @description what every open edge in the tree resolves to, and — the part that is
 *              easy to get wrong — *where the node serving it has to sit*.
 *
 *              npm's `build-ideal-tree` does this as a queue of nodes whose edges are
 *              not yet met, each edge resolved to a version and then placed at the
 *              deepest target that will take it. The placement is what produces the
 *              hoisted shape; nothing hoists afterwards. That is the difference this
 *              function exists to remove: fibjs resolved first and then ran three
 *              separate passes (`move_up`, `ensure_declared_deps_visible`,
 *              `mark_reachability`) to repair the shape, and each pass disagreed with
 *              the others about where a package lived.
 *
 *              The contract is the one a caller can check: after this returns, every
 *              edge in the tree points at a node that satisfies it, and that node is
 *              at the shallowest place the placement rules allow. What that means for
 *              a path on disk is `diff`'s question, not this one's.
 *
 * @param tree Map<path, node>
 * @param opts
 *   - fetch(name, spec, from)      → a node for a dependency that is nowhere in the
 *                                    tree, or null. The caller owns the network.
 *   - deep(name, spec, from)       → a node already built for that name (the caller's
 *                                    cache), or null
 *   - update { all, name }         → refresh the copies of that name rather than
 *                                    keeping them
 *   - is_root(p)                   → is that path a node whose devDependencies are
 *                                    installed (the project and its members)
 *   - on_error(edge, from, p)      → called for an edge that could not be resolved
 * @returns { resolved, failed }
 */
function resolve(tree, opts) {
    const o = opts || {};
    const failed = [];
    const resolved = [];

    // a node is processed at most once per pass; a node whose edges changed after it
    // was processed (a placed dependency invalidated one) is put back in the queue.
    // This is npm's `#depsQueue`, and the loop shape is the same because the shape is
    // the algorithm
    const queue = [];

    function enqueue(p) {
        if (p === null || p === undefined)
            return;

        if (queue.indexOf(p) < 0)
            queue.push(p);
    }

    const wanted_dev = o.is_root || (() => false);

    tree.forEach((node, p) => {
        if (node.platform_skip)
            return;

        // a link is installed as the symlink it describes; the edges that matter are
        // the ones of the directory it points at, which is a node of its own. Note
        // that a link *target* is `target_only` — nothing is installed at its path —
        // and it is exactly the node whose edges have to be resolved
        if (node.type === LINK)
            return;

        enqueue(p);
    });

    while (queue.length) {
        const p = queue.shift();
        const node = tree.get(p);

        if (!node)
            continue;

        if (node.type === LINK || node.platform_skip)
            continue;

        const fields = [
            'dependencies',
            'optionalDependencies',
            'peerDependencies',
        ];

        if (wanted_dev(p))
            fields.push('devDependencies');

        // sorted, so that the tree two installs of the same project arrive at is the
        // same tree (npm sorts its placement batch for the same reason)
        const edges = [];

        fields.forEach(field => {
            const map = node.edges[field] || {};

            Object.keys(map).forEach(name => edges.push({ name: name, spec: map[name], field: field }));
        });

        edges.sort((a, b) => (a.name < b.name ? -1 : a.name > b.name ? 1 : 0));

        for (const e of edges) {
            // what the node ships inside its own tarball is not resolved
            if (node.bundled && node.bundled[e.name])
                continue;

            const refresh = o.update && (o.update.all || o.update.name === e.name);

            // an existing copy anywhere the resolution reaches is kept, unless the
            // caller asked for that name to be refreshed. Keeping it is the whole
            // point: a dependency gaining an edge must not upgrade the world
            let at = null;

            if (!refresh) {
                for (const candidate of lookup_paths(e.name, p)) {
                    const existing = tree.get(candidate);

                    if (existing && satisfies_node(existing, e.spec)) {
                        at = candidate;
                        break;
                    }
                }
            }

            if (!at) {
                const candidate = o.deep ? o.deep(e.name, e.spec, p) : null;

                const built = candidate || (o.fetch ? o.fetch(e.name, e.spec, node) : null);

                if (!built) {
                    failed.push({ name: e.name, spec: e.spec, field: e.field, from: p });

                    if (o.on_error)
                        o.on_error(e, node, p);

                    // an optional dependency that cannot be fetched is not a failure:
                    // that is what optional means
                    if (e.field === 'optionalDependencies')
                        continue;

                    // A caller that is only *comparing* trees cannot stop the whole
                    // run over an edge it could not place: that edge is the finding,
                    // and the rest of the tree is what it has to be compared against.
                    // `strict` is the default, so an install still refuses to continue
                    if (o.strict === false)
                        continue;

                    throw new Error(`[install] cannot resolve '${e.name}@${e.spec}' ` +
                        `(required by ${p || 'the project'})`);
                }

                // what the tree already holds for this name and version is that node:
                // two dependents of the same copy share it, which is what makes a
                // hoisted tree a tree of *nodes* rather than of paths
                const shared = o.deep ? o.deep(e.name, e.spec, p) : null;

                const node_to_place = shared || built;

                const slot = place_dep(tree, p, e.name, {
                    version: node_to_place.version,
                    // replacing the copy at that path would break an edge that resolves
                    // to it today: the same rule npm applies in `canReplace`
                    accepts: (at, occupant) => replacement_is_safe(tree, at, occupant, node_to_place,
                        { from: p, name: e.name }),
                });

                node_to_place.path = slot.path;
                node_to_place.parent_path = parent_location(slot.path);

                tree.set(slot.path, node_to_place);
                at = slot.path;

                // what it now holds is what its own edges are read from
                enqueue(slot.path);
            }

            // the edge now points at what serves it. Writing it back onto the node is
            // what makes the tree self-describing: `diff`, `calc_flags` and the
            // serializer all read these instead of recomputing an answer
            node.resolved_edges = node.resolved_edges || {};
            node.resolved_edges[e.name] = at;

            resolved.push({ name: e.name, spec: e.spec, path: at, node: tree.get(at), from: p });
        }
    }

    return { resolved: resolved, failed: failed };
}

// ---------------------- diff :start ------------------------- //

const ADD = 'add';
const CHANGE = 'change';
const REMOVE = 'remove';

/**
 * @description what has to happen to one path so that the disk matches the tree that
 *              should be there. This is the *only* place that decides add / change /
 *              remove — npm's `getAction` in `arborist/lib/diff.js`, which is what
 *              keeps its prune, its add and its reinstall from disagreeing
 * @param actual the node on disk, or null
 * @param ideal the node that should be there, or null
 */
function action_for(actual, ideal, opts) {
    const o = opts || {};

    // what the tree says should be there is not there
    if (!ideal)
        return REMOVE;

    // a copy that ships inside a parent's tarball is never installed on its own: it
    // arrives with the parent, and there is nothing to add at that path
    if (ideal.type === BUNDLE)
        return null;

    // a link target has an entry in the lockfile that describes a project directory;
    // nothing is installed at that path, so there is nothing to compare
    if (ideal.target_only)
        return null;

    // what should be there is not there
    if (!actual)
        return ADD;

    if (ideal.type !== actual.type)
        return CHANGE;

    // a link is the link: same target, nothing to do. A different target is a change,
    // and so is an absolute one where a relative one belongs (an older install wrote
    // those, and they break when the project moves)
    if (ideal.type === LINK)
        return actual.link_ok === true ? null : CHANGE;

    if (ideal.version !== actual.version)
        return CHANGE;

    // the version matches; the bytes have to match too, or the copy on disk is not
    // what the tree describes. npm treats a missing digest on either side as "no
    // complaint" — the versions agree and the source does too, which is all a
    // shrinkwrap, a bundle or a link can offer
    const no_integrity = !ideal.integrity && !actual.integrity;
    const no_resolved = !ideal.resolved && !actual.resolved;
    const resolved_match = ideal.resolved && ideal.resolved === actual.resolved;

    if (no_integrity && (resolved_match || no_resolved))
        return o.bins_ok === false ? CHANGE : null;

    if (!ideal.integrity || !actual.integrity || ideal.integrity !== actual.integrity)
        return CHANGE;

    return null;
}

/**
 * @description the difference between two trees, in the shape reify consumes:
 *              every path the disk has that the tree does not is a `REMOVE`, every
 *              path the tree has that the disk does not is an `ADD`, and a path both
 *              have but disagree on is a `CHANGE`
 * @param actual Map<path, node>
 * @param ideal Map<path, node>
 * @param opts { omit: {dev, optional}, platform: (node) => bool }
 * @returns [{ action, path, ideal, actual }]
 */
function diff(actual, ideal, opts) {
    const o = opts || {};
    const actions = [];
    const seen = {};

    // the paths an install keeps off the disk: what `omit` asks for, and what the
    // platform cannot run. They stay in the tree (the lockfile describes them) but
    // they are not part of what the disk should hold — so a copy of one that *is* on
    // the disk is a removal, not something to leave alone
    const kept_out = {};

    function left_out(node, p) {
        if (node.target_only)
            return false;

        if (o.omit) {
            const flags = node.flags || {};

            if ((flags.dev && o.omit.dev) || (flags.optional && o.omit.optional))
                return true;
        }

        return o.keep_out ? o.keep_out(node) : false;
    }

    // what the tree says should be there
    ideal.forEach((node, p) => {
        seen[p] = true;

        if (left_out(node, p)) {
            kept_out[p] = true;
            return;
        }

        const action = action_for(actual.get(p) || null, node, o);

        if (action)
            actions.push({ action: action, path: p, ideal: node, actual: actual.get(p) || null });
    });

    // what is on the disk and should not be: a path nothing describes, and a path
    // whose node this install was told to leave out
    actual.forEach((node, p) => {
        if (seen[p] && !kept_out[p])
            return;

        actions.push({ action: REMOVE, path: p, ideal: null, actual: node });
    });

    // the shallowest paths first, so a directory is put in place before what goes
    // inside it, and removed before its children (npm relies on the same ordering)
    const depth = p => p.split('/').length;

    actions.sort((a, b) => {
        if (a.action === REMOVE && b.action === REMOVE)
            return depth(a.path) - depth(b.path);

        if (a.action === REMOVE)
            return -1;

        if (b.action === REMOVE)
            return 1;

        return depth(a.path) - depth(b.path);
    });

    return actions;
}

// ---------------------- trees from the disk :start ------------------------- //

/**
 * @description the tree that is on the disk. Every directory under a `node_modules`
 *              is a node; a link is a node whose `type` is `link`, and the directory
 *              it points at is read as its target (which is where the lockfile
 *              records the member's own dependencies).
 *
 *              Entries are read from their `package.json`; the source (`resolved` /
 *              `integrity`) is the one thing a directory cannot tell, so it comes from
 *              the note npm leaves for exactly that question
 *              (`node_modules/.package-lock.json`, see `read_hidden_sources`)
 * @param root the project
 * @param opts { hidden_sources, is_member(local_path) }
 * @returns Map<path, node>
 */
function load_actual(root, opts) {
    const o = opts || {};
    const tree = new Map();
    const hidden = o.hidden_sources || {};

    function read_manifest(dir) {
        try {
            return JSON.parse(fs.readTextFile(path.join(dir, 'package.json')));
        } catch (e) {
            return null;
        }
    }

    function link_target(dir) {
        try {
            if (!fs.lstat(dir).isSymbolicLink())
                return null;

            // `realpath` answers with the path a link points at even when that path
            // is gone, so the caller still has to ask whether the target exists
            return fs.realpath(dir);
        } catch (e) {
            return null;
        }
    }

    function is_dangling_link(dir) {
        try {
            return fs.lstat(dir).isSymbolicLink();
        } catch (e) {
            return false;
        }
    }

    /**
     * @description read one level: every entry of `<dir>/node_modules`
     */
    function read_level(dir, prefix, parent) {
        const nm = path.join(dir, 'node_modules');

        if (!fs.exists(nm))
            return;

        const scope_of = name => (name.charAt(0) === '@' ? name : null);

        function visit(container, name_prefix) {
            let names;

            try {
                names = fs.readdir(container);
            } catch (e) {
                return;
            }

            names.forEach(entry => {
                // a dot entry is a note or a cache, never an installation
                if (entry.charAt(0) === '.')
                    return;

                const full = path.join(container, entry);

                // a scope holds packages: it is expanded one level
                if (name_prefix === '') {
                    const maybe = scope_of(entry);

                    if (maybe) {
                        let is_dir = false;

                        try {
                            is_dir = fs.stat(full).isDirectory();
                        } catch (e) {
                            is_dir = false;
                        }

                        if (is_dir) {
                            visit(full, maybe + '/');
                            return;
                        }
                    }
                }

                const name = name_prefix + entry;
                const p = prefix + 'node_modules/' + name;
                const target = link_target(full);
                const manifest = read_manifest(full);

                // a link whose target is gone: `realpath` still answers with the
                // path it was pointed at, so the target has to be asked whether it
                // exists. This is exactly the entry a prune exists for — a member
                // directory that was deleted, or one that was renamed
                if (!manifest) {
                    if (target === null && !is_dangling_link(full))
                        return;

                    tree.set(p, make_node(p, {
                        name: name,
                        type: LINK,
                        resolved: undefined,
                        missing_target: true,
                    }));

                    return;
                }

                const note = hidden[p];

                // a link is a link, whatever it points at
                if (target) {
                    tree.set(p, make_node(p, {
                        name: name,
                        version: manifest.version,
                        type: LINK,
                        local_path: target,
                        manifest: manifest,
                        edges: edges_of(manifest),
                        platform: platform_of(manifest),
                        bin: manifest.bin,
                        license: manifest.license && typeof manifest.license === 'object'
                            ? manifest.license.type : manifest.license,
                        engines: manifest.engines,
                        funding: manifest.funding,
                    }));

                    // and the directory it points at is a node of its own: the
                    // lockfile records it by its project-relative path
                    const target_path = relative_path(root, target);

                    if (target_path && !tree.has(target_path)) {
                        const target_manifest = read_manifest(target) || manifest;

                        tree.set(target_path, make_target(target_path, {
                            name: target_manifest.name || name,
                            version: target_manifest.version,
                            local_path: target,
                            manifest: target_manifest,
                            edges: edges_of(target_manifest),
                            platform: platform_of(target_manifest),
                            bundled: bundled_names(target_manifest),
                            bin: target_manifest.bin,
                            has_install_script: has_install_script(target_manifest),
                            license: target_manifest.license && typeof target_manifest.license === 'object'
                                ? target_manifest.license.type : target_manifest.license,
                            engines: target_manifest.engines,
                            funding: target_manifest.funding,
                            target_of: p,
                        }));
                    }

                    read_level(full, target_path ? target_path + '/' : p + '/', p);
                    return;
                }

                // a copy that came inside a parent's tarball: the parent's manifest
                // says so, and the lockfile does too
                const in_bundle = parent && tree.get(parent) &&
                    tree.get(parent).bundled && tree.get(parent).bundled[name];

                // a directory with no source of its own is a copy that ships inside
                // its parent, or a directory nobody installed (`npm prune` removes
                // those; the diff does, by never describing it)
                const source_digest = note && note.version === manifest.version ? note : null;

                tree.set(p, make_node(p, {
                    name: name,
                    version: manifest.version,
                    type: in_bundle ? BUNDLE : REGISTRY,
                    resolved: source_digest ? source_digest.resolved : undefined,
                    integrity: source_digest ? source_digest.integrity : undefined,
                    manifest: manifest,
                    edges: edges_of(manifest),
                    platform: platform_of(manifest),
                    bundled: bundled_names(manifest),
                    bin: manifest.bin,
                    has_install_script: has_install_script(manifest),
                    license: manifest.license && typeof manifest.license === 'object'
                        ? manifest.license.type : manifest.license,
                    engines: manifest.engines,
                    funding: manifest.funding,
                }));

                read_level(full, p + '/', p);
            });
        }

        visit(nm, '');
    }

    read_level(root, '', null);

    return tree;
}

/**
 * @description does that manifest run an install script? npm records the answer as
 *              `hasInstallScript`
 */
function has_install_script(manifest) {
    const scripts = manifest && manifest.scripts;

    if (!scripts)
        return false;

    return !!(scripts.preinstall || scripts.install || scripts.postinstall);
}

// ---------------------- trees from the lockfile :start ------------------------- //

/**
 * @description the tree the manifest and the lockfile describe, with no reference to
 *              what is on the disk. This is where an install starts: npm's
 *              `load-virtual`, and the reason a stale `node_modules` cannot influence
 *              what the next install decides to do
 * @param packages the lockfile's path table (see `lockfile.to_path_map`)
 * @param opts { root_entry, workspaces: [{path, pkgjson}], root_manifest }
 * @returns Map<path, node>
 */
function load_virtual(packages, opts) {
    const o = opts || {};
    const tree = new Map();

    Object.keys(packages).forEach(p => {
        const entry = packages[p];

        // the root entry describes package.json, which is the root node below
        if (p === '')
            return;

        const link = entry.link === true;
        const target = link ? String(entry.resolved || '').replace(/\/+$/, '') : null;

        // the manifest: a member's own directory has one on disk, and a link target
        // is exactly that directory
        const manifest = (o.manifests && o.manifests[p]) || null;

        const node = make_node(p, {
            name: entry.name,
            version: entry.version,
            type: link ? LINK
                : entry.in_bundle ? BUNDLE
                    : is_git_resolved(entry.resolved) ? GIT
                        : REGISTRY,
            resolved: link ? target : entry.resolved,
            integrity: entry.integrity,
            local_path: link && target && o.local_paths ? o.local_paths[target] : undefined,
            target_only: entry.target_only,
            bundled: bundled_names(entry.raw_entry || entry),
            edges: {
                dependencies: entry.dependencies || {},
                devDependencies: {},
                optionalDependencies: entry.optionalDependencies || {},
                peerDependencies: entry.peerDependencies || {},
            },
            platform: { os: entry.os, cpu: entry.cpu, libc: entry.libc },
            manifest: manifest,
            has_install_script: entry.has_install_script === true,
            bin: entry.bin,
            license: entry.license,
            engines: entry.engines,
            funding: entry.funding,
        });

        node.raw_entry = entry.raw_entry;
        node.in_bundle = entry.in_bundle === true;

        tree.set(p, node);
    });

    // the root: what package.json declares, which is where every walk starts
    const root_manifest = o.root_manifest || (o.root_entry || {});

    tree.set('', make_node('', {
        name: root_manifest.name,
        version: root_manifest.version,
        type: REGISTRY,
        edges: edges_of(root_manifest),
        manifest: root_manifest,
        target_only: false,
    }));

    return tree;
}

/**
 * @description is that `resolved` a repository rather than a tarball?
 */
function is_git_resolved(resolved) {
    return typeof resolved === 'string' &&
        (resolved.indexOf('git+') === 0 || resolved.indexOf('git://') === 0);
}

// ---------------------- the ideal tree :start ------------------------- //

/**
 * @description does any manifest of the project still declare that name as a local
 *              dependency? A `file:` / `link:` / `../x` spec is one the tree answers
 *              with a link, and the link is the directory the spec points at
 */
function declares_local_edge(manifests, name) {
    return Object.keys(manifests).some(where => {
        const manifest = manifests[where] || {};

        return ['dependencies', 'devDependencies', 'optionalDependencies', 'peerDependencies']
            .some(field => {
                const spec = manifest[field] && manifest[field][name];

                return typeof spec === 'string' && (
                    spec.indexOf('file:') === 0 ||
                    spec.indexOf('link:') === 0 ||
                    spec.indexOf('workspace:') === 0 ||
                    /^(\.{1,2}[\\/]|[\\/]|[a-zA-Z]:[\\/])/.test(spec)
                );
            });
    });
}

/**
 * @description the tree the project *should* have, built from what it declares and
 *              from the lockfile — never from the disk.
 *
 *              This is where npm's `build-ideal-tree` begins, and the reason the
 *              workspace member set is an *input* rather than something compared
 *              against later: a member that was renamed, deleted or added is simply
 *              not in `members` any more, so it never enters the tree, and the diff
 *              against the disk turns that into a `REMOVE`. That is npm's
 *              `set workspaces` (`node.js`), which detaches the edge of every name
 *              that left the set and reloads the edges of the rest.
 *
 *              What this does *not* do is resolve anything against a registry: a
 *              dependency in the tree whose version cannot be determined is reported
 *              back in `unresolved`, and the caller resolves it (that is the one part
 *              of the build that needs the network, and keeping it out here is what
 *              makes `--ci` the same code path with nothing to resolve).
 * @param opts
 *   - root_manifest     the project's package.json
 *   - members           [{ name, path (absolute), relative_path, package_json }] — the
 *                       workspace members as they are *now*
 *   - packages          the lockfile's path table (may be empty)
 *   - override          { [name]: version } — an explicit version for a named install
 * @returns { tree, unresolved: [{ name, spec, from }], dropped: [path] }
 */
function build_ideal(opts) {
    const o = opts || {};
    const root_manifest = o.root_manifest || {};
    const members = o.members || [];
    const packages = o.packages || {};
    const tree = new Map();
    const unresolved = [];
    const dropped = [];
    // the directory entries, decided once the links that describe them are known
    const targets = [];

    // ---- the root -------------------------------------------------------------
    //
    // what package.json declares. Its devDependencies are installed, because the
    // project is the one node whose dev tree is part of the tree
    const root = make_node('', {
        name: root_manifest.name,
        version: root_manifest.version,
        type: REGISTRY,
        edges: edges_of(root_manifest),
        manifest: root_manifest,
    });

    tree.set('', root);

    // ---- the members ----------------------------------------------------------
    //
    // a member is a link at `node_modules/<name>` plus the directory it points at. A
    // member that is no longer in `members` is not added at all — which is exactly
    // what makes a rename or a deletion converge in one install: the old link is not
    // in the tree, so the diff removes it
    const member_names = {};
    const manifests = { '': root_manifest };

    members.forEach(m => {
        const link_path = 'node_modules/' + m.name;
        const target_path = String(m.relative_path || '').replace(/\\/g, '/').replace(/\/+$/, '');
        const manifest = m.package_json || {};

        member_names[m.name] = true;
        manifests[target_path] = manifest;

        tree.set(link_path, make_node(link_path, {
            name: m.name,
            version: manifest.version,
            type: LINK,
            resolved: target_path,
            local_path: m.path,
            manifest: manifest,
            edges: {},
            platform: platform_of(manifest),
            bin: manifest.bin,
        }));

        tree.get(link_path).from_workspace = true;

        // A member is reached from the project through an edge, the way npm models it:
        // `#loadWorkspaces` makes one typed `workspace` with the spec `file:<path>`, and
        // that is what a flag walk follows to get from the root into a member's tree.
        // Without it the member is a node nobody reaches, so everything it needs for
        // production looked dev-only — `@types/node` and `d3-array` came out dev because
        // only the root's own `devDependencies` mentioned them.
        //
        // It is a *prod* edge for this purpose and not a separate kind: a member's
        // dependencies are installed like the project's, and `calc_flags` only knows the
        // four kinds npm unsets by
        if (!root.edges.dependencies[m.name])
            root.edges.dependencies[m.name] = 'file:' + target_path;

        // the directory it points at: it is not installed, and its own dependencies
        // are what an install of that project would read
        tree.set(target_path, make_target(target_path, {
            name: m.name,
            version: manifest.version,
            local_path: m.path,
            manifest: manifest,
            edges: edges_of(manifest),
            platform: platform_of(manifest),
            bundled: bundled_names(manifest),
            bin: manifest.bin,
            has_install_script: has_install_script(manifest),
            license: manifest.license && typeof manifest.license === 'object'
                ? manifest.license.type : manifest.license,
            engines: manifest.engines,
            funding: manifest.funding,
            target_of: link_path,
        }));
    });

    // ---- the local dependencies the manifests declare --------------------------
    //
    // a `file:` / `link:` / `../x` edge is a dependency the project installs by
    // pointing at a directory it already has, not by fetching one. npm turns that into
    // a link (`Node#loadDeps` places it, and the link's target is the directory), and
    // so does this — from the manifest, and *before* the lockfile is consulted.
    //
    // Doing it only from the lockfile's own link entries was the gap: on a first
    // install there is no lockfile, so `file:linkpkg` produced a dependency nothing
    // described, the resolver fetched a registry package called `linkpkg`, and the
    // whole of a member's dev/optional marking came out wrong because the node it
    // marked was not the one that got installed
    Object.keys(manifests).forEach(where => {
        const manifest = manifests[where] || {};

        ['dependencies', 'devDependencies', 'optionalDependencies'].forEach(field => {
            const map = manifest[field] || {};

            Object.keys(map).forEach(name => {
                const spec = map[name];

                if (typeof spec !== 'string')
                    return;

                if (spec.indexOf('file:') !== 0 && spec.indexOf('link:') !== 0 &&
                    !/^(\.{1,2}[\\/]|[\\/])/.test(spec))
                    return;

                // where the edge is declared decides the path it installs at: a
                // dependency of the project is a link in the project's own
                // node_modules, and one of a member is a link beside that member.
                // `where` is a lockfile path already, so it is used as it stands
                const link_path = (where ? where + '/' : '') + 'node_modules/' + name;

                if (tree.has(link_path))
                    return;

                // the directory the spec names, relative to the manifest that declares
                // it — which is what npm does, and what makes a member's `file:../x`
                // point at the project's sibling rather than at `node_modules/x`
                const base = o.root ? require('path').resolve(o.root, where) : process.cwd();
                const target_dir = require('path').resolve(base, spec.replace(/^(file|link):/, ''));

                const target_path = relative_path(o.root || process.cwd(), target_dir);

                const target_manifest = o.local_manifest
                    ? (o.local_manifest(target_dir) || {})
                    : {};

                tree.set(link_path, make_node(link_path, {
                    name: name,
                    version: target_manifest.version,
                    type: LINK,
                    resolved: target_path,
                    local_path: target_dir,
                    manifest: target_manifest,
                    edges: {},
                    platform: platform_of(target_manifest),
                    bin: target_manifest.bin,
                }));

                if (!tree.has(target_path)) {
                    tree.set(target_path, make_target(target_path, {
                        name: target_manifest.name || name,
                        version: target_manifest.version,
                        local_path: target_dir,
                        manifest: target_manifest,
                        edges: edges_of(target_manifest),
                        platform: platform_of(target_manifest),
                        bundled: bundled_names(target_manifest),
                        bin: target_manifest.bin,
                        has_install_script: has_install_script(target_manifest),
                        target_of: link_path,
                    }));

                    manifests[target_path] = target_manifest;
                }
            });
        });
    });

    // ---- what the lockfile already describes ----------------------------------
    //
    // entries that are not a member (and not the root) are copied over as they are,
    // with the source the lockfile pins. Entries for a member that is gone, or for a
    // name a member now owns, are *not* copied: the member above is the answer
    //
    // a `file:` edge is an edge of some manifest, and the link that satisfies it is
    // described by that manifest — so a link is kept when any manifest still declares
    // it, whether that is the project's or a member's. `manifests` was filled in above,
    // as the members were placed

    Object.keys(packages).forEach(p => {
        const entry = packages[p];

        if (p === '')
            return;

        // a link entry is a link, and two things can ask for one: the member set
        // (`workspaces`), and a `file:` edge the manifest declares. A link either of
        // them asks for is part of the tree — it is installed as the symlink it
        // describes, and its target is a directory of the project. A link neither of
        // them asks for is the entry of a dependency that was removed, or of a member
        // that was renamed, and it is not in the tree at all
        if (entry.link) {
            const name = name_from_path(p) || entry.name;

            if (tree.has(p))
                return;

            if (!member_names[name] && !declares_local_edge(manifests, name)) {
                dropped.push(p);
                return;
            }

            const target = String(entry.resolved || '').replace(/\/+$/, '');
            const target_manifest = manifests[target] || null;

            tree.set(p, make_node(p, {
                name: entry.name,
                version: entry.version,
                type: LINK,
                resolved: target,
                local_path: target ? path.resolve(o.root || process.cwd(), target) : undefined,
                manifest: null,
                edges: {},
            }));

            // the directory it points at, unless the member set already described it.
            // Its edges come from the manifest when that directory is a member (whose
            // manifest the caller handed over), and from the lockfile's own entry
            // otherwise — a `file:` dependency's directory is not something the caller
            // read, and its dependencies are nevertheless what an install of it needs
            if (target && !tree.has(target)) {
                const target_entry = packages[target];
                const target_edges = target_manifest
                    ? edges_of(target_manifest)
                    : {
                        dependencies: (target_entry && target_entry.dependencies) || {},
                        devDependencies: {},
                        optionalDependencies: (target_entry && target_entry.optionalDependencies) || {},
                        peerDependencies: (target_entry && target_entry.peerDependencies) || {},
                    };

                tree.set(target, make_target(target, {
                    name: entry.name,
                    version: entry.version,
                    local_path: path.resolve(o.root || process.cwd(), target),
                    manifest: target_manifest,
                    edges: target_edges,
                    platform: platform_of(target_manifest || {}),
                    bundled: bundled_names(target_manifest || {}),
                    has_install_script: has_install_script(target_manifest || {}),
                    target_of: p,
                }));
            }

            return;
        }

        // the entry for the directory a link points at. It is *described by* that
        // link, and the link is handled below — which may be after this entry in the
        // table, since the table is ordered by path. So this one is not decided here:
        // it is in the tree exactly when the link that points at it is
        if (entry.target_only) {
            targets.push(p);
            return;
        }

        if (tree.has(p))
            return;

        const git = is_git_resolved(entry.resolved);

        const node = make_node(p, {
            name: entry.name,
            version: entry.version,
            type: entry.in_bundle ? BUNDLE : git ? GIT : REGISTRY,
            resolved: entry.resolved,
            integrity: entry.integrity,
            bundled: bundled_names(entry.raw_entry || entry),
            edges: {
                dependencies: entry.dependencies || {},
                devDependencies: {},
                optionalDependencies: entry.optionalDependencies || {},
                peerDependencies: entry.peerDependencies || {},
            },
            platform: { os: entry.os, cpu: entry.cpu, libc: entry.libc },
            manifest: null,
            has_install_script: entry.has_install_script === true,
            bin: entry.bin,
            license: entry.license,
            engines: entry.engines,
            funding: entry.funding,
        });

        node.raw_entry = entry.raw_entry;
        node.in_bundle = entry.in_bundle === true;

        tree.set(p, node);
    });

    // an explicit request names a version the rest of the tree has to follow
    if (o.override) {
        Object.keys(o.override).forEach(name => {
            Object.keys(packages).forEach(p => {
                const node = tree.get(p);

                if (node && node.name === name && node.type === REGISTRY)
                    node.version = o.override[name];
            });
        });
    }

    // a directory entry is in the tree when the link that points at it is; anything
    // else describes a directory nothing installs
    targets.forEach(p => {
        if (!tree.has(p))
            dropped.push(p);
    });

    // the lockfile describes the tree of an *earlier* install, and an entry it still
    // carries may be one nothing declares any more — a dependency that was removed
    // from the manifest, or a member's whole subtree after the member left the set.
    // Keeping those would mean the tree still asks for them on the disk, so the tree
    // is trimmed to what the roots actually reach. This is the one place that decides
    // it, and it is the same walk `calc_flags` does (npm's ideal tree is what the
    // resolver arrived at, and its entries are reachable by construction)
    trim_to_reachable(tree, dropped);

    // which kind of edge reaches each node, which is what `omit` and the write back
    // need. It is a property of the graph, so it is computed once here rather than
    // asked of each mechanism that cares
    const flags = calc_flags(tree, ['']);
    tree.forEach((node, p) => {
        node.flags = {};

        ['dev', 'optional', 'devOptional', 'peer'].forEach(kind => {
            if (flags[kind][p])
                node.flags[kind] = true;
        });
    });

    return { tree: tree, unresolved: unresolved, dropped: dropped };
}

/**
 * @description drop the nodes nothing reaches from the roots — the project, and every
 *              link target that is a project of its own. An entry the lockfile keeps
 *              after a dependency was removed from the manifest is exactly such a
 *              node: it is described, and nothing asks for it
 */
function trim_to_reachable(tree, dropped) {
    const reached = {};

    function walk(from) {
        if (reached[from])
            return;

        reached[from] = true;

        const node = tree.get(from);

        if (!node)
            return;

        // a link carries no edges of its own: its dependencies are the target's
        const target = node.type === LINK && node.resolved ? node.resolved : from;

        if (target !== from)
            walk(target);

        EDGE_KINDS.forEach(kind => {
            const map = node.edges && node.edges[kind];

            if (!map)
                return;

            Object.keys(map).forEach(name => {
                const to = resolve_path(tree, name, target);

                if (to)
                    walk(to);
            });
        });
    }

    // the project, plus every link that is in the tree. A link is an installation of
    // a directory of the project, and its dependencies are read from that directory —
    // so what it points at is a place a walk has to begin, whether the link is a
    // workspace member (`workspaces`) or a `file:` edge (`dependencies`)
    walk('');

    Array.from(tree.keys()).forEach(p => {
        const node = tree.get(p);

        if (node.type !== LINK)
            return;

        walk(p);
    });

    Array.from(tree.keys()).forEach(p => {
        if (reached[p])
            return;

        tree.delete(p);

        if (dropped.indexOf(p) < 0)
            dropped.push(p);
    });
}

// ---------------------- serialization :start ------------------------- //


// ---------------------- from a resolved snapshot :start ------------------------- //

/**
 * @description the resolved snapshot — the nested `node_modules` maps the walk builds —
 *              as the path table this module's passes speak.
 *
 *              The two shapes hold the same information, and the installer has both at
 *              different moments: the walk builds the nested one, while `resolve`,
 *              `diff` and `calc_flags` need the flat one. Projecting here rather than
 *              inside each of them is what keeps there being one answer to "where is
 *              this node": a second projection is a second place for the two to drift,
 *              which is exactly how a prune came to disagree with a write back.
 * @param rootsnap the snapshot root: `{ name, version, dep_vs, node_modules, ... }`
 * @param opts { root }
 * @returns Map<path, node>
 */
function snapshot_to_tree(rootsnap, opts) {
    const o = opts || {};
    const root = o.root || process.cwd();
    const tree = new Map();

    const relative = dir => {
        const rel = require('path').relative(root, dir).replace(/\\/g, '/');

        return rel === '' ? '' : rel.replace(/\/+$/, '');
    };

    function node_of(info, p) {
        const local = info.local_package === true || info.workspace_package === true;
        const local_path = local ? (info.local_path || info.workspace_path) : undefined;

        return {
            path: p,
            name: info.name,
            version: info.version,

            type: local ? LINK : (info.in_bundle ? BUNDLE : REGISTRY),
            resolved: local
                ? relative(local_path)
                : (info.dist && info.dist.tarball),
            integrity: local ? undefined : (info.dist && info.dist.integrity),
            local_path: local_path,
            target_only: false,

            edges: {
                // the *declared* ranges, which is what a placement works from
                dependencies: (info.declared && info.declared.dep_vs) || info.dep_vs || {},
                devDependencies: {},
                optionalDependencies: (info.declared && info.declared.opt_dep_vs) || info.opt_dep_vs || {},
                peerDependencies: (info.declared && info.declared.peer_dep_vs) || info.peer_dep_vs || {},
            },

            flags: {
                dev: info.dev_only === true,
                optional: info.optional_only === true,
                devOptional: info.dev_optional === true,
                peer: info.peer_only === true,
            },

            from_workspace: info.workspace_package === true,
            from_local: info.local_package === true,

            // "this run put it there", which is what tells a node a previous install
            // left behind from one the resolve just arrived at
            new_module: info.new_module === true,

            // The snapshot node this was projected from, kept as it is. The tree decides
            // *where* a node sits; everything else about it — the manifest a write back
            // projects, the `dist` a download needs, the flags `omit` reads — already
            // exists on the original, and rebuilding those on the way back is a second
            // place for them to differ.
            source: info,
        };
    }

    tree.set('', {
        path: '',
        name: rootsnap.name,
        version: rootsnap.version,
        type: REGISTRY,
        target_only: false,
        edges: {
            dependencies: (rootsnap.declared && rootsnap.declared.dep_vs) || rootsnap.dep_vs || {},
            devDependencies: (rootsnap.declared && rootsnap.declared.dev_dep_vs) || rootsnap.dev_dep_vs || {},
            optionalDependencies: (rootsnap.declared && rootsnap.declared.opt_dep_vs) || rootsnap.opt_dep_vs || {},
            peerDependencies: (rootsnap.declared && rootsnap.declared.peer_dep_vs) || rootsnap.peer_dep_vs || {},
        },
        flags: {},
    });

    (function walk(info, prefix) {
        Object.keys(info.node_modules || {}).forEach(name => {
            const child = info.node_modules[name];
            const p = prefix + 'node_modules/' + name;
            const local = child.local_package === true || child.workspace_package === true;

            tree.set(p, node_of(child, p));

            // a link's own edges are installed inside the directory it points at, and
            // that directory is a node of its own
            const target = local ? relative(child.local_path || child.workspace_path) : null;

            if (target && !tree.has(target)) {
                tree.set(target, {
                    path: target,
                    name: child.name,
                    version: child.version,
                    type: REGISTRY,
                    target_only: true,
                    edges: {
                        // A member's `devDependencies` are installed: npm installs the dev
                        // tree of every member of the project. Leaving the field empty here
                        // made a member's dev dependency look production, so `--omit=dev`
                        // kept it on the disk.
                        dependencies: (child.declared && child.declared.dep_vs) || child.dep_vs || {},
                        devDependencies: (child.declared && child.declared.dev_dep_vs) || child.dev_dep_vs || {},
                        optionalDependencies: (child.declared && child.declared.opt_dep_vs) || child.opt_dep_vs || {},
                        peerDependencies: (child.declared && child.declared.peer_dep_vs) || child.peer_dep_vs || {},
                    },
                    flags: {},
                    from_workspace: child.workspace_package === true,
                    from_local: child.local_package === true,
                });
            }

            walk(child, target ? target + '/' : p + '/');
        });
    })(rootsnap, '');

    return tree;
}

// ---------------------- to a resolved snapshot :start ------------------------- //

/**
 * @description the path table as the nested maps the resolver walks — the inverse of
 *              `snapshot_to_tree`, and the direction that lets the installer stop
 *              seeding its whole pass from what happens to be on the disk.
 *
 *              The two shapes hold the same graph; only the shape differs. A node's
 *              location decides where it goes: `node_modules/a` is a child of the
 *              project, `node_modules/a/node_modules/b` a child of `a`, and a link's
 *              target directory (`packages/member`) is a root of its own whose children
 *              are `packages/member/node_modules/*`.
 *
 *              A link is recorded with `local_package` / `local_path`, because that is
 *              what the rest of the installer means by one: it is not fetched, and its
 *              own directory is where its dependencies are installed.
 * @param tree Map<path, node>
 * @param opts { root, manifest_of(p, node) }
 * @returns the snapshot root: `{ name, version, dep_vs, node_modules, ... }`
 */
function tree_to_snapshot(tree, opts) {
    const o = opts || {};
    const root = tree.get('');

    const snapshot = {
        name: root ? root.name : undefined,
        version: root ? root.version : undefined,

        dep_vs: (root && root.edges && root.edges.dependencies) || {},
        dev_dep_vs: (root && root.edges && root.edges.devDependencies) || {},
        opt_dep_vs: (root && root.edges && root.edges.optionalDependencies) || {},
        peer_dep_vs: (root && root.edges && root.edges.peerDependencies) || {},

        node_modules: {},
    };

    // where each path lives, so a child is attached to the node that holds it rather
    // than to whatever was visited last
    const holder_of = p => {
        const idx = p.lastIndexOf('/node_modules/');

        if (idx >= 0) {
            const holder = p.slice(0, idx);

            // A member's own dependencies are installed in its directory, and the
            // snapshot records them under the *link* — `read_module` reads
            // `node_modules/member/node_modules/x` into `node_modules.member`. So a
            // path under a link's target hangs off the link, not off the target: the
            // target has no entry of its own in the snapshot, it is the directory.
            const link_of_target = targets.get(holder);

            return link_of_target === undefined ? holder : link_of_target;
        }

        if (p.indexOf('node_modules/') === 0)
            return '';

        // a directory that is not under a node_modules is a project of its own: its
        // children hang off it directly
        return null;
    };

    // target path -> the link that installs it, so a member's children can be attached
    // to the link the way the snapshot records them
    const targets = new Map();

    tree.forEach((node, p) => {
        if (node.type === LINK && node.resolved)
            targets.set(node.resolved, p);
    });

    // a node's own container: the snapshot node it is recorded under
    const containers = new Map();
    containers.set('', snapshot);

    // two passes: record every node, then attach the children, so a child whose
    // parent comes later in the table still finds it
    const entries = [];

    tree.forEach((node, p) => {
        if (p === '')
            return;

        entries.push([p, node]);
    });

    // shallowest first, which is what makes a parent exist before its children
    const depth = p => p.split('/node_modules/').length;

    entries.sort((a, b) => depth(a[0]) - depth(b[0]));

    entries.forEach(([p, node]) => {
        // A link is a child of whatever holds it, like any other node; it is the
        // *directory* it points at that is a root of its own. Treating the link as the
        // root is how its entry went missing from the project's own `node_modules`.
        const holder = node.type === LINK ? parent_location(p) : holder_of(p);

        const named = p.slice(p.lastIndexOf('node_modules/') + 'node_modules/'.length);

        const info = {
            name: named,
            real_name: node.real_name && node.real_name !== named ? node.real_name : undefined,
            version: node.version,
            lock_path: p,

            dep_vs: (node.edges && node.edges.dependencies) || {},
            dev_dep_vs: {},
            opt_dep_vs: (node.edges && node.edges.optionalDependencies) || {},
            peer_dep_vs: (node.edges && node.edges.peerDependencies) || {},

            bundle_deps: node.bundled || undefined,
            manifest: o.manifest_of ? o.manifest_of(p, node) : node.manifest,

            os: node.platform && node.platform.os,
            cpu: node.platform && node.platform.cpu,
            libc: node.platform && node.platform.libc,
            bin: node.bin,
            license: node.license,
            engines: node.engines,
            funding: node.funding,

            node_modules: {},
        };

        // The node the snapshot already had, if this one came from it: the tree only
        // moves a node, and everything the installer knows about it beyond its position
        // is still on the original.
        if (node.source) {
            containers.set(p, Object.assign(node.source, { node_modules: {} }));

            const target = holder === null ? null : containers.get(holder);

            if (target)
                target.node_modules[named] = containers.get(p);

            return;
        }

        // the flags, which is what `omit` and the write back read
        const flags = node.flags || {};

        info.dev_only = flags.dev === true;
        info.optional_only = flags.optional === true;
        info.dev_optional = flags.devOptional === true;
        info.peer_only = flags.peer === true;

        // a link is not installed: the directory it points at is where the package
        // lives, and that directory has a node of its own
        if (node.type === LINK) {
            info.local_package = true;
            info.local_path = node.local_path;
        }

        if (node.type === BUNDLE)
            info.in_bundle = true;

        // a link's flags are its target's, the way npm copies them across
        const container = holder === null ? null : containers.get(holder);

        if (container) {
            container.node_modules[named] = info;
            containers.set(p, info);
        } else if (holder === null) {
            // the directory a link points at: a project of its own, whose children are
            // the ones installed inside it
            const target_root = {
                name: node.name,
                version: node.version,
                lock_path: p,
                workspace_package: node.from_workspace === true,
                local_package: node.from_local === true,
                local_path: node.local_path,
                workspace_path: node.local_path,

                dep_vs: (node.edges && node.edges.dependencies) || {},
                dev_dep_vs: {},
                opt_dep_vs: (node.edges && node.edges.optionalDependencies) || {},
                peer_dep_vs: (node.edges && node.edges.peerDependencies) || {},

                node_modules: {},
            };

            containers.set(p, target_root);
        }
    });

    return snapshot;
}

// ---------------------- repair :start ------------------------- //

/**
 * @description give a node the copy of its dependency that it can actually see.
 *
 *              Where a node sits is what its edges resolve against, and moving a node
 *              is what changes that: a package hoisted out of a member's own
 *              `node_modules` can no longer be served by a copy that stayed beside the
 *              member. npm never has this problem because it decides the position of
 *              every node once, while placing it; a tree that resolves and then moves
 *              has to look again.
 *
 *              The check is per declared edge and asks the second question, not the
 *              first: not "where would `require()` find this name" but "does the copy
 *              `require()` finds satisfy what was declared". A copy of the right name
 *              at the wrong version makes the tree `invalid`, and this is what npm's
 *              placement avoids by never putting it there.
 *
 *              A copy is only ever nested, never moved: the donor is an installed node
 *              (a link is visible from everywhere and needs no copy), and nesting is
 *              the placement a dependent that cannot see the hoisted copy requires.
 * @param tree Map<path, node>
 * @param opts { donor_of(name, spec, from) } — an installed copy to nest
 * @returns [{ from, name, spec, at, donor, base }] — the copies that have to be
 *          nested. Nothing is inserted here: the decision is this tree's, and the
 *          caller owns the shape it keeps
 */
function nest_unserved(tree, opts, done) {
    const o = opts || {};
    const nestings = [];

    // What the caller has already acted on, from an earlier round of this same pass.
    // A nested copy is a node like any other and declares dependencies of its own, so
    // the repair has to be run again after inserting one — which is what the recursive
    // version got for free, and what a single pass over a fixed tree does not.
    const known = done || [];

    // A member's directory is a node of its own, and its edges are read from there —
    // so the walk covers it like any other node, and the project's own path is the
    // one it starts at
    const order = [];

    tree.forEach((node, p) => {
        if (node.target_only || node.platform_skip)
            return;
        if (node.type === LINK)
            return;

        order.push(p);
    });

    // shallowest first: a copy nested for a parent is visible to its children, and a
    // child that needs it finds it instead of nesting another one
    order.sort((a, b) => ancestors(null, a).length - ancestors(null, b).length);

    order.forEach(p => {
        const node = tree.get(p);

        if (!node || node.type === LINK)
            return;

        const edges = node.edges || {};

        ['dependencies', 'optionalDependencies', 'peerDependencies'].forEach(field => {
            const map = edges[field];

            if (!map)
                return;

            Object.keys(map).forEach(name => {
                const spec = map[name];

                // what the node ships inside its own tarball arrives with it
                if (node.bundled && node.bundled[name])
                    return;

                // a peer the tree does not satisfy is npm's invalid peer: the copy is
                // kept where it is and the installer reports it, it is not duplicated
                // here — duplicating it is what put two instances of a module with a
                // singleton in it on disk
                if (field === 'peerDependencies' && resolve_path(tree, name, p))
                    return;

                const base = node_resolution_base(tree, p, node);

                if (serving_path(tree, name, spec, base))
                    return;

                const donor = o.donor_of ? o.donor_of(name, spec) : null;

                if (!donor)
                    return;

                const at = place_path(p, name);

                if (tree.has(at))
                    return;

                // the same copy nested twice would be the same decision reported again
                if (o.seen && o.seen(at))
                    return;

                nestings.push({ from: p, name: name, spec: spec, at: at, donor: donor, base: base });
            });
        });
    });

    return nestings;
}

/**
 * @description write the edge kind onto every node, from the tree's own graph.
 *
 *              This is `calc_flags` applied in place, so that what `omit`, the diff and
 *              the serializer read is one answer computed once. The alternative — each
 *              of them asking its own question — is what let a node be dev for the disk
 *              and production for the lockfile in the same run.
 * @param tree Map<path, node>
 * @param roots the paths the walk begins at (the project, and every member)
 * @returns the tree, so a caller can chain it
 */
function mark_flags(tree, roots) {
    const flags = calc_flags(tree, roots);

    tree.forEach((node, p) => {
        if (p === '' || node.target_only)
            return;

        node.flags = {
            dev: flags.dev[p] === true,
            optional: flags.optional[p] === true,
            devOptional: flags.devOptional[p] === true,
            peer: flags.peer[p] === true,
        };
    });

    return flags;
}

/**
 * @description the roots a flag walk starts at: the project, and every link target
 *              that is a project of its own.
 *
 *              A member is a root because npm's tree has an edge for it (the
 *              `workspaces` field) and this projection of a snapshot does not: the
 *              member's own tree hangs off the link, with nothing from the project
 *              reaching it. Naming it is what stands in for that edge, and it is why
 *              what a member needs for production is not read as something only the
 *              root's devDependencies wanted.
 * @param tree Map<path, node>
 */
function member_roots(tree) {
    const roots = [''];

    tree.forEach((node, p) => {
        if (p === '' || node.type !== LINK || !node.resolved)
            return;

        if (node.from_workspace !== true && !(node.source && node.source.workspace_package === true))
            return;

        if (tree.has(node.resolved))
            roots.push(node.resolved);
    });

    return roots;
}

// ---------------------- exports :start ------------------------- //

module.exports = {
    REGISTRY,
    GIT,
    LINK,
    BUNDLE,
    ADD,
    CHANGE,
    REMOVE,

    make_node,
    make_target,

    bundled_names,
    peer_edges,
    edges_of,
    platform_of,
    has_install_script,

    name_from_path,
    relative_path,
    parent_location,
    ancestors,
    lookup_paths,
    resolve_path,
    serving_path,

    OK,
    KEEP,
    REPLACE,
    CONFLICT,
    nesting_target,
    satisfies_node,
    place_dep,
    resolve,
    place_path,

    calc_flags,
    mark_flags,
    member_roots,
    nest_unserved,
    action_for,
    diff,

    snapshot_to_tree,
    tree_to_snapshot,

    load_actual,
    load_virtual,
    build_ideal,

};
