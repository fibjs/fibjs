'use strict';

// ---------- tree helper (the three trees, fibjs-only) ----------
//
// plans/installer-three-tree-refactor.md: the installer decides what to install and
// what to remove from one place — a tree of nodes and a diff against the disk. These
// cases pin the layer down on its own, before any of the installer is moved onto it.

var { describe, it, beforeEach, afterEach } = require('node:test');
var assert = require('assert');
var fs = require('fs');
var path = require('path');
var os = require('os');

var tree = require('internal/helpers/tree');

var TMP = path.join(__dirname, 'opt_tools_test_files', 'tmp');
var made = [];

function scratch(name) {
    var dir = path.join(TMP, 'tree_' + name + '_' + Date.now() + '_' +
        Math.random().toString(36).slice(2, 7));
    fs.mkdirSync(dir, { recursive: true });
    made.push(dir);
    return dir;
}

function write(dir, rel, content) {
    var p = path.join(dir, rel);
    fs.mkdirSync(path.dirname(p), { recursive: true });
    fs.writeFileSync(p, typeof content === 'string' ? content : JSON.stringify(content, null, 2));
    return p;
}

function node(over) {
    var name = over.name || 'a';
    var p = over.path || ('node_modules/' + name);

    return tree.make_node(p, Object.assign({ name: name, version: '1.0.0' }, over));
}

/**
 * @description remove a directory tree; a link is a link, not a directory to follow
 */
function rmdirSync(dirPath) {
    if (!fs.existsSync(dirPath))
        return;

    fs.readdirSync(dirPath).forEach(function (name) {
        var full = path.join(dirPath, name);

        if (fs.lstatSync(full).isDirectory())
            rmdirSync(full);
        else
            fs.unlinkSync(full);
    });

    fs.rmdirSync(dirPath);
}

describe('tree helper', function () {
    afterEach(function () {
        made.forEach(function (d) {
            try { rmdirSync(d); } catch (e) { /* ignore */ }
        });
        made = [];
    });

    // ----- paths -----

    describe('paths', function () {
        it('names the package a lockfile path refers to', function () {
            assert.equal(tree.name_from_path('node_modules/a'), 'a');
            assert.equal(tree.name_from_path('node_modules/@s/a'), '@s/a');
            assert.equal(tree.name_from_path('node_modules/x/node_modules/@s/a'), '@s/a');
            assert.equal(tree.name_from_path('packages/member'), null);
            assert.equal(tree.name_from_path(''), null);
        });

        it('walks a dependency outwards the way require does', function () {
            assert.deepEqual(tree.lookup_paths('a', ''),
                ['node_modules/a']);
            assert.deepEqual(tree.lookup_paths('a', 'node_modules/x'),
                ['node_modules/x/node_modules/a', 'node_modules/a']);
            assert.deepEqual(tree.lookup_paths('a', 'packages/m'),
                ['packages/m/node_modules/a', 'node_modules/a']);
        });

        it('resolves a dependency to the nearest copy in the tree', function () {
            var t = new Map();

            t.set('node_modules/a', node({ name: 'a' }));
            t.set('node_modules/x/node_modules/a', node({ name: 'a', version: '2.0.0' }));

            assert.equal(tree.resolve_path(t, 'a', 'node_modules/x'), 'node_modules/x/node_modules/a');
            assert.equal(tree.resolve_path(t, 'a', 'node_modules/y'), 'node_modules/a');
            assert.equal(tree.resolve_path(t, 'zzz', 'node_modules/y'), null);
        });
    });

    // ----- flags -----

    describe('calc_flags', function () {
        function build(root_edges, nodes) {
            var t = new Map();

            t.set('', tree.make_node('', {
                name: 'p', version: '1.0.0', edges: root_edges,
            }));

            nodes.forEach(function (n) { t.set(n.path, n); });

            return t;
        }

        it('marks a package only a dev edge reaches as dev', function () {
            var t = build(
                { dependencies: { prod: '1.0.0' }, devDependencies: { dev: '1.0.0' } },
                [node({ name: 'prod', path: 'node_modules/prod' }),
                node({ name: 'dev', path: 'node_modules/dev' })]
            );

            var f = tree.calc_flags(t, ['']);

            assert.equal(f.dev['node_modules/dev'], true, 'dev is a dev node');
            assert.equal(f.dev['node_modules/prod'], undefined, 'prod is not');
        });

        it('follows a production edge through a dev package back to production', function () {
            // dev → a, a → b: b is reached without a dev edge (a → b is a production
            // edge) but the only path from the root still goes through dev
            var t = build(
                { devDependencies: { a: '1.0.0' } },
                [node({ name: 'a', path: 'node_modules/a', edges: { dependencies: { b: '1.0.0' } } }),
                node({ name: 'b', path: 'node_modules/b' })]
            );

            var f = tree.calc_flags(t, ['']);

            assert.equal(f.dev['node_modules/a'], true);
            assert.equal(f.dev['node_modules/b'], true,
                'b is only reachable through a dev edge');
        });

        it('marks the overlap of the dev and optional trees', function () {
            var t = build(
                { devDependencies: { host: '1.0.0' } },
                [node({
                    name: 'host', path: 'node_modules/host',
                    edges: { optionalDependencies: { opt: '1.0.0' } },
                }),
                node({ name: 'opt', path: 'node_modules/opt' })]
            );

            var f = tree.calc_flags(t, ['']);

            assert.equal(f.devOptional['node_modules/opt'], true,
                'opt sits in both trees');
        });

        it('treats a source the walk does not reach as neither', function () {
            var t = build({ dependencies: { a: '1.0.0' } },
                [node({ name: 'a', path: 'node_modules/a' }),
                node({ name: 'orphan', path: 'node_modules/orphan' })]);

            var f = tree.calc_flags(t, ['']);

            assert.equal(f.dev['node_modules/orphan'], undefined);
            assert.equal(f.optional['node_modules/orphan'], undefined);
        });
    });

    // ----- diff -----

    describe('diff', function () {
        function mapOf(nodes) {
            var t = new Map();

            nodes.forEach(function (n) { t.set(n.path, n); });

            return t;
        }

        it('adds what the tree has and the disk does not', function () {
            var actions = tree.diff(new Map(), mapOf([node({ name: 'a', path: 'node_modules/a' })]));

            assert.equal(actions.length, 1);
            assert.equal(actions[0].action, tree.ADD);
            assert.equal(actions[0].path, 'node_modules/a');
        });

        it('removes what the disk has and the tree does not', function () {
            // this is the whole of `prune`: an entry nothing in the tree describes
            var actions = tree.diff(mapOf([node({ name: 'old', path: 'node_modules/old' })]), new Map());

            assert.equal(actions.length, 1);
            assert.equal(actions[0].action, tree.REMOVE);
            assert.equal(actions[0].path, 'node_modules/old');
        });

        it('leaves a node both sides agree on alone', function () {
            var n = function () {
                return node({
                    name: 'a', path: 'node_modules/a',
                    resolved: 'https://r/a-1.0.0.tgz',
                    integrity: 'sha512-AAA',
                });
            };

            assert.deepEqual(tree.diff(mapOf([n()]), mapOf([n()])), []);
        });

        it('changes a node whose version differs', function () {
            var actions = tree.diff(
                mapOf([node({ name: 'a', path: 'node_modules/a', version: '1.0.0' })]),
                mapOf([node({ name: 'a', path: 'node_modules/a', version: '2.0.0' })])
            );

            assert.equal(actions.length, 1);
            assert.equal(actions[0].action, tree.CHANGE);
        });

        it('does not ask for a bundled copy to be installed on its own', function () {
            // it arrives inside its parent's tarball: there is nothing at that path
            var bundled = tree.make_node('node_modules/host/node_modules/x', {
                name: 'x', version: '1.0.0', type: tree.BUNDLE,
            });

            assert.deepEqual(tree.diff(new Map(), mapOf([bundled])), []);
        });

        it('does not install anything at a link target', function () {
            // the entry describes a project directory, and the link is the install
            var target = tree.make_target('packages/m', {
                name: '@s/m', version: '0.1.0', target_of: 'node_modules/@s/m',
            });

            assert.deepEqual(tree.diff(new Map(), mapOf([target])), []);
        });

        it('leaves an omitted package off the disk but keeps it in the tree', function () {
            // npm's omit is about the disk, never about the graph: the same tree, with
            // and without the option, has to describe the same nodes
            var dev = node({ name: 'dev', path: 'node_modules/dev', flags: { dev: true } });

            var with_it = tree.diff(new Map(), mapOf([dev]), {});
            var without = tree.diff(new Map(), mapOf([dev]), { omit: { dev: true } });

            assert.equal(with_it.length, 1, 'installed when nothing is omitted');
            assert.equal(without.length, 0, 'and left off the disk when dev is omitted');
        });

        it('puts a directory in place before what goes inside it', function () {
            var actions = tree.diff(new Map(), mapOf([
                node({ name: 'a', path: 'node_modules/a' }),
                node({ name: 'b', path: 'node_modules/a/node_modules/b' }),
            ]));

            assert.deepEqual(actions.map(function (a) { return a.path; }),
                ['node_modules/a', 'node_modules/a/node_modules/b']);
        });

        it('removes a directory before what goes inside it', function () {
            var actions = tree.diff(mapOf([
                node({ name: 'a', path: 'node_modules/a' }),
                node({ name: 'b', path: 'node_modules/a/node_modules/b' }),
            ]), new Map());

            assert.deepEqual(actions.map(function (a) { return a.path; }),
                ['node_modules/a', 'node_modules/a/node_modules/b']);
        });
    });

    // ----- actual -----

    describe('load_actual', function () {
        it('reads the packages on the disk', function () {
            var dir = scratch('actual');

            write(dir, 'node_modules/a/package.json', { name: 'a', version: '1.0.0' });
            write(dir, 'node_modules/@s/b/package.json', { name: '@s/b', version: '2.0.0' });

            var t = tree.load_actual(dir);

            assert.equal(t.get('node_modules/a').version, '1.0.0');
            assert.equal(t.get('node_modules/@s/b').name, '@s/b');
        });

        it('reads a link as a link, with its target as a node of its own', function () {
            var dir = scratch('link');

            write(dir, 'packages/m/package.json', {
                name: '@s/m', version: '0.1.0', dependencies: { x: '1.0.0' },
            });

            fs.mkdirSync(path.join(dir, 'node_modules', '@s'), { recursive: true });
            fs.symlinkSync(path.join(dir, 'packages', 'm'),
                path.join(dir, 'node_modules', '@s', 'm'), 'dir');

            var t = tree.load_actual(dir);
            var link = t.get('node_modules/@s/m');
            var target = t.get('packages/m');

            assert.equal(link.type, tree.LINK);
            // the directory it points at is an ordinary node of its own: npm's
            // `Link#target` is where the package really lives, and treating it as
            // another link is what made the resolver skip every member's dependencies
            assert.equal(target.type, tree.REGISTRY);
            assert.equal(target.target_only, true, 'the directory is not an install');
            assert.deepEqual(target.edges.dependencies, { x: '1.0.0' },
                'and its dependencies are what a reader reads');
        });

        it('takes the source from the note npm leaves', function () {
            var dir = scratch('hidden');

            write(dir, 'node_modules/a/package.json', { name: 'a', version: '1.0.0' });
            write(dir, 'node_modules/.package-lock.json', {
                name: 'p', version: '1.0.0', lockfileVersion: 3, requires: true,
                packages: {
                    'node_modules/a': {
                        version: '1.0.0',
                        resolved: 'https://r/a-1.0.0.tgz',
                        integrity: 'sha512-AAA',
                    },
                },
            });

            var t = tree.load_actual(dir, {
                hidden_sources: { 'node_modules/a': { version: '1.0.0', resolved: 'https://r/a-1.0.0.tgz', integrity: 'sha512-AAA' } },
            });

            assert.equal(t.get('node_modules/a').resolved, 'https://r/a-1.0.0.tgz');
            assert.equal(t.get('node_modules/a').integrity, 'sha512-AAA');
        });

        it('does not borrow a note about another version', function () {
            var dir = scratch('stale');

            write(dir, 'node_modules/a/package.json', { name: 'a', version: '2.0.0' });

            var t = tree.load_actual(dir, {
                hidden_sources: { 'node_modules/a': { version: '1.0.0', resolved: 'https://r/a-1.0.0.tgz' } },
            });

            assert.equal(t.get('node_modules/a').version, '2.0.0');
            assert.equal(t.get('node_modules/a').resolved, undefined,
                'the note describes a tree that is not there any more');
        });

        it('skips a dot entry, which is a note or a cache', function () {
            var dir = scratch('dot');

            write(dir, 'node_modules/.cache/x', 'x');
            write(dir, 'node_modules/a/package.json', { name: 'a', version: '1.0.0' });

            var t = tree.load_actual(dir);

            assert.equal(t.has('node_modules/.cache'), false);
            assert.ok(t.has('node_modules/a'));
        });
    });

    // ----- virtual -----

    describe('load_virtual', function () {
        var lockfile = require('internal/helpers/lockfile');

        function lockOf(packages) {
            return { lockfileVersion: 3, packages: packages, root: packages[''] || null };
        }

        it('builds the same shape the lockfile describes, without the disk', function () {
            // the point of the virtual tree: a stale node_modules cannot influence it
            var lock = lockOf({
                '': { name: 'p', version: '1.0.0', dependencies: { a: '1.0.0' } },
                'node_modules/a': {
                    version: '1.0.0',
                    resolved: 'https://r/a-1.0.0.tgz',
                    integrity: 'sha512-AAA',
                    dependencies: { b: '^2.0.0' },
                },
                'node_modules/b': { version: '2.0.0' },
            });

            var t = tree.load_virtual(lockfile.to_path_map(lock), {
                root_manifest: { name: 'p', version: '1.0.0', dependencies: { a: '1.0.0' } },
            });

            assert.equal(t.get('').name, 'p', 'the root is the manifest');
            assert.equal(t.get('node_modules/a').version, '1.0.0');
            assert.deepEqual(t.get('node_modules/a').edges.dependencies, { b: '^2.0.0' });
            assert.equal(t.get('node_modules/b').type, tree.REGISTRY);
        });

        it('marks a link and keeps what it points at out of the install set', function () {
            var t = tree.load_virtual(lockfile.to_path_map(lockOf({
                '': { name: 'p', version: '1.0.0' },
                'node_modules/@s/m': { resolved: 'packages/m', link: true },
                'packages/m': { name: '@s/m', version: '0.1.0' },
            })), {
                root_manifest: { name: 'p', version: '1.0.0' },
            });

            assert.equal(t.get('node_modules/@s/m').type, tree.LINK);
            assert.equal(t.get('node_modules/@s/m').resolved, 'packages/m');
            assert.equal(t.get('packages/m').target_only, true);
        });

        it('never reads the disk, so a stale tree cannot leak in', function () {
            var dir = scratch('virtual');

            write(dir, 'node_modules/gone/package.json', { name: 'gone', version: '9.9.9' });

            var t = tree.load_virtual(lockfile.to_path_map(lockOf({
                '': { name: 'p', version: '1.0.0', dependencies: { keep: '1.0.0' } },
                'node_modules/keep': { version: '1.0.0' },
            })), {
                root_manifest: { name: 'p', version: '1.0.0', dependencies: { keep: '1.0.0' } },
            });

            assert.equal(t.has('node_modules/gone'), false,
                'what is on the disk is not part of what the manifest describes');
        });
    });

    // ----- ideal -----
    //
    // the two cases the installer got wrong twice: a workspace member that was
    // deleted, and one that was renamed. Both are the same question — what is the
    // member set *now* — and the answer belongs in the tree, not in a comparison
    // against what is left on the disk

    describe('build_ideal', function () {
        function member(name, rel, manifest) {
            return {
                name: name,
                path: '/proj/' + rel,
                relative_path: rel,
                package_json: manifest || { name: name, version: '0.1.0' },
            };
        }

        function build(members, packages) {
            // a package table that carries a root entry describes the project itself,
            // and that entry is what the walk begins at
            var root = (packages && packages['']) ||
                { name: 'p', version: '1.0.0', workspaces: ['packages/*'] };

            return tree.build_ideal({
                root_manifest: root,
                members: members,
                packages: packages || {},
            });
        }

        it('puts a member at its name, and its directory as a target', function () {
            var out = build([member('@a/m', 'packages/m')]);
            var t = out.tree;

            assert.equal(t.get('node_modules/@a/m').type, tree.LINK);
            assert.equal(t.get('node_modules/@a/m').resolved, 'packages/m');
            assert.equal(t.get('packages/m').target_only, true);
        });

        it('does not describe a member that is gone', function () {
            // F: the directory was deleted, so it is not in the member set. The tree
            // simply has no node for it, and the diff against the disk removes the link
            var out = build([member('@a/keep', 'packages/keep')]);
            var t = out.tree;

            assert.equal(t.has('node_modules/@a/gone'), false,
                'a link nothing declared is not in the tree');
            assert.equal(t.has('packages/gone'), false);

            var disk = new Map();
            disk.set('node_modules/@a/gone', tree.make_node('node_modules/@a/gone', {
                name: '@a/gone', version: '0.1.0', type: tree.LINK, resolved: 'packages/gone',
            }));

            var actions = tree.diff(disk, t);

            assert.deepEqual(actions.filter(a => a.action === tree.REMOVE).map(a => a.path),
                ['node_modules/@a/gone'], 'the dangling link is removed');
        });

        it('follows a renamed member in one step', function () {
            // G: `@r/old` → `@r/new` at the same directory. The old link has to leave
            // and the new one has to stay — the case where the installer used to create
            // the new link and then prune it away again
            var before = build([member('@r/old', 'packages/lib')]).tree;
            var after = build([member('@r/new', 'packages/lib')]).tree;

            var disk = new Map(before);

            var actions = tree.diff(disk, after);
            var removed = actions.filter(a => a.action === tree.REMOVE).map(a => a.path);
            var added = actions.filter(a => a.action === tree.ADD).map(a => a.path);

            assert.deepEqual(added, ['node_modules/@r/new'],
                'the new name is installed');
            assert.deepEqual(removed, ['node_modules/@r/old'],
                'and the old one leaves — the new one is never a removal');
            assert.equal(after.has('node_modules/@r/new'), true);
            assert.equal(after.has('node_modules/@r/old'), false);
        });

        it('drops the lockfile entries of a member that is gone', function () {
            var out = build([member('@a/keep', 'packages/keep')], {
                'node_modules/@a/gone': { link: true, resolved: 'packages/gone', name: '@a/gone' },
                'packages/gone': { target_only: true, name: '@a/gone', version: '0.1.0' },
                'node_modules/@a/keep': { link: true, resolved: 'packages/keep', name: '@a/keep' },
                'packages/keep': { target_only: true, name: '@a/keep', version: '0.1.0' },
            });

            assert.deepEqual(out.dropped.sort(),
                ['node_modules/@a/gone', 'packages/gone'],
                'the entries of the member that left the set');
            assert.equal(out.tree.has('node_modules/@a/gone'), false);
            assert.equal(out.tree.has('packages/gone'), false);
        });

        it('carries the lockfile entries that are not members', function () {
            // an entry the project's own edges reach is carried over as the lockfile
            // pins it: same version, same source, same declared ranges
            var out = build([], {
                '': { name: 'p', version: '1.0.0', dependencies: { a: '1.0.0' } },
                'node_modules/a': {
                    version: '1.0.0', resolved: 'https://r/a-1.0.0.tgz',
                    integrity: 'sha512-AAA', dependencies: { b: '^1.0.0' },
                },
                'node_modules/b': { version: '2.0.0' },
            });

            assert.equal(out.tree.get('node_modules/a').version, '1.0.0');
            assert.deepEqual(out.tree.get('node_modules/a').edges.dependencies, { b: '^1.0.0' });
            assert.equal(out.tree.get('node_modules/a').type, tree.REGISTRY);
            assert.equal(out.tree.get('node_modules/b').version, '2.0.0',
                'and so is what that entry needs');
        });

        it('drops a lockfile entry nothing in the manifest reaches', function () {
            // the lockfile describes an *earlier* install. An entry no edge from the
            // manifest arrives at is a dependency that was removed, and keeping it in
            // the tree would keep it on the disk
            var out = build([], {
                '': { name: 'p', version: '1.0.0', dependencies: { a: '1.0.0' } },
                'node_modules/a': { version: '1.0.0' },
                'node_modules/orphan': { version: '9.9.9' },
            });

            assert.equal(out.tree.has('node_modules/orphan'), false);
            assert.ok(out.dropped.indexOf('node_modules/orphan') > -1);
        });

        it('keeps a bundled copy out of the install set', function () {
            // a copy that ships inside its parent's tarball arrives with it: there is
            // nothing to fetch at its path, whatever the tree says about it
            var out = build([], {
                'node_modules/host': { version: '1.0.0', in_bundle: false },
                'node_modules/host/node_modules/x': { version: '2.0.0', in_bundle: true },
            });

            var x = out.tree.get('node_modules/host/node_modules/x');

            // the lockfile's entry for a shipped copy may be trimmed as unreferenced
            // (the parent declares nothing about it in a lock entry), and if it is in
            // the tree it is a BUNDLE. Either way it is never something to install
            if (x)
                assert.equal(x.type, tree.BUNDLE);

            var actions = tree.diff(new Map(), out.tree);

            assert.deepEqual(actions.filter(a => a.path === 'node_modules/host/node_modules/x'), [],
                'a shipped copy is never an ADD');
        });
    });
});
