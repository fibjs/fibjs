'use strict';

var { describe, it } = require('node:test');
var assert = require('assert');

var tree = require('internal/helpers/tree');

function node(over) {
    var name = over.name || 'a';
    var p = over.path === undefined ? '' : over.path;
    var fields = Object.assign({ name: name, version: '1.0.0' }, over);

    delete fields.path;

    return tree.make_node(p, fields);
}

// ---------- the resolver: placement, not repair ----------
//
// npm's `build-ideal-tree` resolves each unmet edge to a version and *then places the
// node at the deepest target that will take it*. The placement produces the hoisted
// shape; nothing hoists afterwards. These cases pin that down: every edge points at the
// shallowest node that satisfies it, and no pass runs after the resolve to fix where
// things ended up.

describe('tree resolver', function () {
    function target(map) {
        var t = new Map();

        Object.keys(map).forEach(function (p) {
            t.set(p, map[p].node || map[p]);
        });

        return t;
    }

    function dep(version, deps) {
        return tree.make_node('', {
            name: 'x', version: version, edges: tree.edges_of({ dependencies: deps || {} }),
        });
    }

    it('hoists a shared dependency to the project', function () {
        // a -> c@1, b -> c@1: the one copy the tree needs sits at the root, which is
        // the shallowest target either of them can reach
        var t = target({
            '': node({ name: 'root', path: '', version: '1.0.0', edges: tree.edges_of({ dependencies: { a: '^1', b: '^1' } }) }),
            'node_modules/a': node({ name: 'a', edges: tree.edges_of({ dependencies: { c: '^1' } }) }),
            'node_modules/b': node({ name: 'b', edges: tree.edges_of({ dependencies: { c: '^1' } }) }),
        });

        var c = dep('1.0.0');

        var built = [];

        tree.resolve(t, {
            fetch: function (name, spec, from) {
                if (name !== 'c')
                    return null;
                built.push(from.path || '');
                return dep('1.0.0');
            },
            is_root: function () { return false; },
        });

        assert.ok(t.has('node_modules/c'), 'c is placed once, at the root');
        assert.equal(t.has('node_modules/a/node_modules/c'), false, 'not nested under a');
        assert.equal(t.has('node_modules/b/node_modules/c'), false, 'not nested under b');
        assert.equal(built.length, 1, 'the second dependent reuses the copy: one fetch');
    });

    it('nests a copy when the root holds an incompatible version', function () {
        // a -> c@1, b -> c@2, the root already has c@1: b's requirement cannot be met
        // from above, so a copy goes under b -- and it goes *under b*, not somewhere the
        // resolver happened to be visiting
        var t = target({
            '': node({ name: 'root', path: '', version: '1.0.0', edges: tree.edges_of({ dependencies: { a: '^1', b: '^1', c: '^1' } }) }),
            'node_modules/a': node({ name: 'a', edges: tree.edges_of({ dependencies: { c: '^1' } }) }),
            'node_modules/b': node({ name: 'b', edges: tree.edges_of({ dependencies: { c: '^2' } }) }),
            'node_modules/c': node({ name: 'c', version: '1.0.0' }),
        });

        tree.resolve(t, {
            fetch: function (name, spec) {
                if (name === 'c' && spec === '^2')
                    return dep('2.0.0');
                return null;
            },
            is_root: function () { return false; },
        });

        assert.equal(t.get('node_modules/c').version, '1.0.0', 'the root copy stays');
        assert.ok(t.has('node_modules/b/node_modules/c'), 'b gets its own copy');
        assert.equal(t.get('node_modules/b/node_modules/c').version, '2.0.0');
    });

    it('walks past a peer to place the copy beside the project', function () {
        // npm's `deepest-nesting-target`: the walk returns the first ancestor that does
        // not itself ask for the name as a peer -- so a peer edge makes it skip *past*
        // that node, outwards. That is why peers hoist to the project
        assert.equal(
            tree.nesting_target(target({
                '': node({ name: 'root', version: '1.0.0' }),
                'node_modules/a': node({ name: 'a', edges: tree.edges_of({ peerDependencies: { p: '^1' } }) }),
            }), 'node_modules/a', 'p'),
            '');

        // a name that is not a peer stops at the first ancestor, which is the node
        // itself -- a plain dependency is nested under its dependent
        assert.equal(
            tree.nesting_target(target({
                '': node({ name: 'root', version: '1.0.0' }),
                'node_modules/a': node({ name: 'a', edges: tree.edges_of({ dependencies: { d: '^1' } }) }),
            }), 'node_modules/a', 'd'),
            'node_modules/a');
    });

    it('installs the devDependencies of a project node, and not of a package', function () {
        // npm installs the dev tree of the project and of every member, and never the
        // dev tree of a dependency
        var t = target({
            '': node({ name: 'root', path: '', version: '1.0.0', edges: tree.edges_of({ devDependencies: { d: '^1' } }) }),
            'node_modules/p': node({ name: 'p', edges: tree.edges_of({ devDependencies: { q: '^1' } }) }),
        });

        tree.resolve(t, {
            fetch: function (name) {
                return node({ name: name });
            },
            is_root: function (p) { return p === ''; },
        });

        assert.ok(t.has('node_modules/d'), 'the project dev dependency is installed');
        assert.equal(t.has('node_modules/p/node_modules/q'), false, 'a package dev dependency is not');
    });

    it('leaves a shipped copy to the tarball that carries it', function () {
        // a package's `bundleDependencies` arrive inside its own tarball: resolving one
        // would fetch a second copy and overwrite the shipped one
        var t = target({
            '': node({ name: 'root', path: '', version: '1.0.0', edges: tree.edges_of({ dependencies: { h: '^1' } }) }),
            'node_modules/h': node({
                name: 'h',
                bundled: { x: true },
                edges: tree.edges_of({ dependencies: { x: '^2' } }),
            }),
        });

        tree.resolve(t, {
            fetch: function () { throw new Error('must not fetch a bundled copy'); },
            is_root: function () { return false; },
        });

        assert.equal(t.has('node_modules/h/node_modules/x'), false);
    });

    it('keeps what a link points at out of the resolution', function () {
        // a link is installed as the symlink it describes; the edges that matter are
        // the ones of the directory it points at, which is a node of its own
        var t = target({
            '': node({ name: 'root', path: '', version: '1.0.0' }),
            'node_modules/m': tree.make_node('node_modules/m', {
                name: 'm', version: '1.0.0', type: tree.LINK, resolved: 'packages/m',
                edges: tree.edges_of({ dependencies: { z: '^1' } }),
            }),
            'packages/m': tree.make_target('packages/m', {
                name: 'm', version: '1.0.0', edges: tree.edges_of({ dependencies: { z: '^1' } }),
            }),
        });

        var asked = [];

        tree.resolve(t, {
            fetch: function (name, spec, from) {
                asked.push(name);
                return node({ name: name });
            },
            is_root: function () { return false; },
        });

        // the target's dependencies are resolved from the target, not from the link:
        // `packages/m` is where `z` has to be visible, and the link is only the symlink
        // that makes the directory reachable
        assert.deepEqual(asked, ['z'], 'z is asked for once, from the target');
        assert.ok(t.has('node_modules/z') || t.has('packages/m/node_modules/z'),
            'z is placed where the target can see it');
    });

    it('refreshes only what the caller named', function () {
        // `--update <pkg>` is the one thing that takes a kept copy away: everything else
        // holds, or the install would upgrade the tree on every run
        var t = target({
            '': node({ name: 'root', path: '', version: '1.0.0', edges: tree.edges_of({ dependencies: { k: '*', j: '*' } }) }),
            'node_modules/k': node({ name: 'k', version: '1.0.0' }),
            'node_modules/j': node({ name: 'j', version: '1.0.0' }),
        });

        tree.resolve(t, {
            fetch: function (name) {
                return node({ name: name, version: '2.0.0' });
            },
            update: { all: false, name: 'k' },
            is_root: function () { return false; },
        });

        assert.equal(t.get('node_modules/k').version, '2.0.0', 'the named package is refreshed');
        assert.equal(t.get('node_modules/j').version, '1.0.0', 'the rest is kept');
    });

    it('says which edge it could not resolve, and stops', function () {
        var t = target({
            '': node({ name: 'root', path: '', version: '1.0.0', edges: tree.edges_of({ dependencies: { missing: '^1' } }) }),
        });

        var failed = [];

        assert.throws(function () {
            tree.resolve(t, {
                fetch: function () { return null; },
                on_error: function (edge, from, p) { failed.push([edge.name, p]); },
            });
        }, /cannot resolve 'missing@\^1'/);

        assert.deepEqual(failed, [['missing', '']]);
    });

    it('does not fail over an optional dependency that is not there', function () {
        var t = target({
            '': node({ name: 'root', path: '', version: '1.0.0', edges: tree.edges_of({ optionalDependencies: { maybe: '^1' } }) }),
        });

        var out = tree.resolve(t, { fetch: function () { return null; } });

        assert.equal(out.failed.length, 1, 'it is reported');
        assert.equal(t.has('node_modules/maybe'), false, 'and it is not in the tree');
    });
});

// ---------- the two projections ----------
//
// The installer holds the same graph in two shapes: the nested maps the resolver walks,
// and the path table the diff and the flags speak. Each has a projection to the other,
// and they have to be inverses — otherwise the answer to "where is this node" depends
// on which shape was asked, which is what made a prune disagree with a write back.

describe('tree projections', function () {
    function tree_of(entries) {
        var t = new Map();

        entries.forEach(function (e) {
            t.set(e.path, tree.make_node(e.path, Object.assign({
                name: e.name || 'a', version: '1.0.0',
            }, e)));
        });

        return t;
    }

    it('round-trips a plain nesting', function () {
        var t = tree_of([
            { path: '', name: 'root', edges: tree.edges_of({ dependencies: { a: '^1' } }) },
            { path: 'node_modules/a', name: 'a', edges: tree.edges_of({ dependencies: { b: '^2' } }) },
            { path: 'node_modules/a/node_modules/b', name: 'b' },
        ]);

        var snap = tree.tree_to_snapshot(t, { root: process.cwd() });
        var back = tree.snapshot_to_tree(snap, { root: process.cwd() });

        assert.equal(back.get('node_modules/a').version, '1.0.0');
        assert.equal(back.get('node_modules/a/node_modules/b').version, '1.0.0');
        assert.deepEqual(back.get('node_modules/a').edges.dependencies, { b: '^2' });
    });

    it('puts a link at its name and its directory at the target path', function () {
        // the paths are real, because a link's target is recorded relative to the
        // project and the projection has to resolve it
        var root = require('fs').mkdtempSync(require('path').join(require('os').tmpdir(), 'proj-'));
        var target = require('path').join(root, 'packages/m');

        var t = tree_of([
            { path: '', name: 'root' },
            {
                path: 'node_modules/m', name: 'm', type: tree.LINK,
                resolved: 'packages/m', local_path: target,
            },
            {
                path: 'packages/m', name: 'm', target_only: true,
                local_path: target,
                edges: tree.edges_of({ dependencies: { x: '^1' } }),
            },
            { path: 'packages/m/node_modules/x', name: 'x' },
        ]);

        var snap = tree.tree_to_snapshot(t, { root: root });

        assert.ok(snap.node_modules.m, 'the link is a child of the project');
        assert.equal(snap.node_modules.m.local_package, true, 'and it is a link');
        assert.ok(snap.node_modules.m.node_modules.x,
            "the member's own dependency hangs off the member");

        var back = tree.snapshot_to_tree(snap, { root: root });

        assert.equal(back.get('node_modules/m').type, tree.LINK);
        assert.ok(back.has('packages/m/node_modules/x'),
            'and the projection finds it again');
    });

    it('round-trips without losing what the tree did not decide', function () {
        // The tree decides where a node sits. Everything else about it — the manifest a
        // write back projects, the `dist` a download needs — is on the node it came
        // from, and rebuilding those on the way back is a second place for them to
        // differ. This is what lets the resolver replace the old passes instead of
        // running beside them: `reify` needs those fields, and the tree does not model
        // them.
        var snap = {
            name: 'root', version: '1.0.0',
            dep_vs: { a: '^1' }, dev_dep_vs: {}, opt_dep_vs: {}, peer_dep_vs: {},
            node_modules: {
                a: {
                    name: 'a', version: '1.0.0', lock_path: 'node_modules/a',
                    dep_vs: { b: '^2' }, dev_dep_vs: {}, opt_dep_vs: {}, peer_dep_vs: {},
                    manifest: { name: 'a', version: '1.0.0', dependencies: { b: '^2' } },
                    dist: { tarball: 'https://r/a.tgz', integrity: 'sha512-a' },
                    pkg_install_typeinfo: { type: 'registry' },
                    binary_pending: false,
                    new_module: true,
                    node_modules: {},
                },
            },
        };

        var t = tree.snapshot_to_tree(snap, { root: process.cwd() });

        // move it, the way a placement would: same node, another location
        var moved = t.get('node_modules/a');
        t.delete('node_modules/a');
        moved.path = 'node_modules/b/node_modules/a';
        moved.source = moved.source;

        t.set('node_modules/b', tree.make_node('node_modules/b', {
            name: 'b', version: '1.0.0',
        }));
        t.set(moved.path, moved);

        var back = tree.tree_to_snapshot(t, { root: process.cwd() });
        var a = back.node_modules.b.node_modules.a;

        assert.equal(a.name, 'a');
        assert.deepEqual(a.manifest, snap.node_modules.a.manifest,
            'the manifest the write back projects is still there');
        assert.equal(a.dist.tarball, 'https://r/a.tgz', 'and the source a download needs');
        assert.equal(a.pkg_install_typeinfo.type, 'registry');
        assert.equal(a.lock_path, 'node_modules/a', 'the record of where it came from');
    });

    it('carries the flags, which is what `omit` reads', function () {
        var t = tree_of([
            { path: '', name: 'root' },
            { path: 'node_modules/d', name: 'd', flags: { dev: true } },
            { path: 'node_modules/o', name: 'o', flags: { optional: true } },
        ]);

        var snap = tree.tree_to_snapshot(t, { root: process.cwd() });

        assert.equal(snap.node_modules.d.dev_only, true);
        assert.equal(snap.node_modules.o.optional_only, true);
    });
});
