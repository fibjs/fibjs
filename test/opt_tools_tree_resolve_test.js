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

// ---------- the lockfile is the ideal tree, written out ----------
//
// npm writes `package-lock.json` from its ideal tree and nothing else, which is why the
// file and `npm ci` cannot disagree. These cases pin the projection down: the same tree
// gives the same bytes, and what comes back out is the tree that went in.

describe('tree serialization', function () {
    it('writes the entries of the tree it is given, and the root', function () {
        var t = new Map();

        t.set('', tree.make_node('', { name: 'root', version: '1.0.0', manifest: {} }));
        t.set('node_modules/a', tree.make_node('node_modules/a', {
            name: 'a', version: '1.0.0', resolved: 'https://r/a-1.0.0.tgz', integrity: 'sha512-x',
            edges: tree.edges_of({ dependencies: { b: '^2' } }),
        }));

        var packages = tree.serialize(t, {
            root_manifest: { name: 'root', version: '1.0.0', dependencies: { a: '^1' } },
        });

        assert.deepEqual(packages[''], { name: 'root', version: '1.0.0', dependencies: { a: '^1' } });
        assert.equal(packages['node_modules/a'].version, '1.0.0');
        assert.equal(packages['node_modules/a'].resolved, 'https://r/a-1.0.0.tgz');
        assert.deepEqual(packages['node_modules/a'].dependencies, { b: '^2' },
            'the declared range, not the chosen version');
    });

    it('round-trips: what it writes is what it reads back', function () {
        // the property that makes `npm ci` accept what `npm install` wrote
        var t = new Map();

        t.set('', tree.make_node('', { name: 'root', version: '1.0.0', manifest: {} }));
        t.set('node_modules/a', tree.make_node('node_modules/a', {
            name: 'a', version: '1.0.0', resolved: 'https://r/a.tgz', integrity: 'sha512-x',
            edges: tree.edges_of({ dependencies: { b: '^2' }, optionalDependencies: { c: '^3' } }),
            flags: { dev: false, optional: true },
        }));
        t.set('node_modules/b', tree.make_node('node_modules/b', {
            name: 'b', version: '2.0.0', resolved: 'https://r/b.tgz', integrity: 'sha512-y',
        }));

        var packages = tree.serialize(t, { root_manifest: { name: 'root', version: '1.0.0' } });
        var back = tree.load_virtual(packages, { root_manifest: { name: 'root', version: '1.0.0' } });

        assert.equal(back.get('node_modules/a').version, '1.0.0');
        assert.equal(back.get('node_modules/a').integrity, 'sha512-x');
        assert.deepEqual(back.get('node_modules/a').edges.dependencies, { b: '^2' });
        assert.equal(back.get('node_modules/a').edges.optionalDependencies.c, '^3');
        assert.equal(back.get('node_modules/b').version, '2.0.0');
    });

    it('writes the source of a node that came off the disk from the note', function () {
        // a directory carries a version and nothing else, and an entry without a source
        // is one a frozen install refuses
        var t = new Map();

        t.set('', tree.make_node('', { name: 'root', version: '1.0.0', manifest: {} }));
        t.set('node_modules/a', tree.make_node('node_modules/a', { name: 'a', version: '1.0.0' }));

        var packages = tree.serialize(t, {
            root_manifest: { name: 'root', version: '1.0.0' },
            hidden_source: function (p, version) {
                return p === 'node_modules/a' && version === '1.0.0'
                    ? { resolved: 'https://r/a.tgz', integrity: 'sha512-z' }
                    : null;
            },
        });

        assert.equal(packages['node_modules/a'].resolved, 'https://r/a.tgz');
        assert.equal(packages['node_modules/a'].integrity, 'sha512-z');
    });

    it('marks a link as a link and a shipped copy as shipped', function () {
        var t = new Map();

        t.set('', tree.make_node('', { name: 'root', version: '1.0.0', manifest: {} }));
        t.set('node_modules/m', tree.make_node('node_modules/m', {
            name: 'm', version: '1.0.0', type: tree.LINK, resolved: 'packages/m', local_path: '/p/packages/m',
        }));
        t.set('node_modules/h/node_modules/x', tree.make_node('node_modules/h/node_modules/x', {
            name: 'x', version: '1.0.0', type: tree.BUNDLE,
        }));

        var packages = tree.serialize(t, { root_manifest: { name: 'root', version: '1.0.0' } });

        assert.equal(packages['node_modules/m'].link, true);
        assert.equal(packages['node_modules/m'].resolved, undefined, 'a link has no tarball');
        assert.equal(packages['node_modules/h/node_modules/x'].in_bundle, true);
    });

    it('leaves the omitted nodes in the tree and out of the include set', function () {
        // `omit` is about the disk: the file describes the whole graph, so the caller
        // decides which nodes are written -- and the file stays the same either way
        var t = new Map();

        t.set('', tree.make_node('', { name: 'root', version: '1.0.0', manifest: {} }));
        t.set('node_modules/d', tree.make_node('node_modules/d', { name: 'd', version: '1.0.0', flags: { dev: true } }));
        t.set('node_modules/p', tree.make_node('node_modules/p', { name: 'p', version: '1.0.0' }));

        var with_dev = tree.serialize(t, { root_manifest: { name: 'root', version: '1.0.0' } });
        var without = tree.serialize(t, {
            root_manifest: { name: 'root', version: '1.0.0' },
            include: function (p, n) { return !(n.flags && n.flags.dev); },
        });

        assert.ok(with_dev['node_modules/d'], 'the file describes it');
        assert.equal(without['node_modules/d'], undefined, 'the invisible-to-this-platform set leaves it out');
        assert.equal(t.has('node_modules/d'), true, 'the tree still holds it');
    });
});
