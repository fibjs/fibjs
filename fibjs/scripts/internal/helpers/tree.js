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
 *              dependencies are what an install of that project would read)
 */
function make_target(p, fields) {
    const node = make_node(p, Object.assign({}, fields, { type: LINK, target_only: true }));

    node.target_of = fields.target_of || null;

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
 * @description the paths a dependency declared at `from` is looked up at, from the
 *              dependent outwards — npm resolves `<dir>/node_modules/<name>` and then
 *              walks up one directory at a time to the root
 */
function lookup_paths(name, from) {
    const paths = [];
    let dir = String(from || '').replace(/\\/g, '/').replace(/\/+$/, '');

    for (;;) {
        paths.push((dir ? dir + '/' : '') + 'node_modules/' + name);

        if (!dir)
            break;

        const idx = dir.lastIndexOf('/node_modules/');
        dir = idx < 0 ? '' : dir.slice(0, idx);
    }

    return paths;
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

// ---------------------- the graph :start ------------------------- //

// the edge kinds, and what a node reached only through them is
const EDGE_KINDS = ['dependencies', 'devDependencies', 'optionalDependencies', 'peerDependencies'];

/**
 * @description which kind of edge reaches each node, the way npm's `calc-dep-flags`
 *              answers it: a node every path from a root needs a `dev` edge for is
 *              `dev`, the same for `optional`, one that needs either is the overlap,
 *              and a production edge clears both.
 *
 *              The answer is a property of the edges, which is why it is computed from
 *              the tree rather than carried around by the resolver: whether a package
 *              is dev-only is not something an install decides per node, it is what
 *              the graph says about it.
 * @param tree Map<path, node>
 * @param roots the paths a walk starts at: the project (and every link target that is
 *              a project of its own — a workspace member)
 * @returns { dev, optional, devOptional, peer } — each a `{ [path]: true }` map
 */
function calc_flags(tree, roots) {
    function reach(skip) {
        const seen = {};
        const stack = [];

        roots.forEach(r => {
            if (seen[r])
                return;

            seen[r] = true;
            stack.push(r);
        });

        while (stack.length) {
            const from = stack.pop();
            const node = tree.get(from);

            if (!node)
                continue;

            // a link carries no edges of its own: what it depends on is installed
            // inside the directory it points at
            const target = node.type === LINK && node.resolved ? node.resolved : from;

            if (target !== from && !seen[target]) {
                seen[target] = true;
                stack.push(target);
            }

            EDGE_KINDS.forEach(kind => {
                if (skip.indexOf(kind) > -1)
                    return;

                const map = node.edges && node.edges[kind];

                if (!map)
                    return;

                Object.keys(map).forEach(name => {
                    const to = resolve_path(tree, name, target);

                    if (!to || seen[to])
                        return;

                    seen[to] = true;
                    stack.push(to);
                });
            });
        }

        return seen;
    }

    const any = reach([]);
    const without = {
        dev: reach(['devDependencies']),
        optional: reach(['optionalDependencies']),
        devOptional: reach(['devDependencies', 'optionalDependencies']),
        peer: reach(['peerDependencies']),
    };

    const flags = { dev: {}, optional: {}, devOptional: {}, peer: {} };

    Object.keys(any).forEach(p => {
        if (!p)
            return;

        Object.keys(without).forEach(kind => {
            if (!without[kind][p])
                flags[kind][p] = true;
        });
    });

    return flags;
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
    tree.set('', make_node('', {
        name: root_manifest.name,
        version: root_manifest.version,
        type: REGISTRY,
        edges: edges_of(root_manifest),
        manifest: root_manifest,
    }));

    // ---- the members ----------------------------------------------------------
    //
    // a member is a link at `node_modules/<name>` plus the directory it points at. A
    // member that is no longer in `members` is not added at all — which is exactly
    // what makes a rename or a deletion converge in one install: the old link is not
    // in the tree, so the diff removes it
    const member_names = {};

    members.forEach(m => {
        const link_path = 'node_modules/' + m.name;
        const target_path = String(m.relative_path || '').replace(/\\/g, '/').replace(/\/+$/, '');
        const manifest = m.package_json || {};

        member_names[m.name] = true;

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

    // ---- what the lockfile already describes ----------------------------------
    //
    // entries that are not a member (and not the root) are copied over as they are,
    // with the source the lockfile pins. Entries for a member that is gone, or for a
    // name a member now owns, are *not* copied: the member above is the answer
    //
    // a `file:` edge is an edge of some manifest, and the link that satisfies it is
    // described by that manifest — so a link is kept when any manifest still declares
    // it, whether that is the project's or a member's
    const manifests = { '': root_manifest };

    members.forEach(m => {
        const rel = String(m.relative_path || '').replace(/\\/g, '/').replace(/\/+$/, '');

        manifests[rel] = m.package_json || {};
    });

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
    lookup_paths,
    resolve_path,

    calc_flags,
    action_for,
    diff,

    load_actual,
    load_virtual,
    build_ideal,
};
