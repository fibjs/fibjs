'use strict';

// ---------- the shadow track ----------
//
// The resolver rewrite cannot be done in one step: the snapshot's nested maps are what
// 37 call sites read, and replacing the shape under them at once turns the first
// divergence into a broken tree instead of a diff. So the new pass runs beside the old
// one and the two are compared. These cases pin the comparison down, because a
// comparison that cannot tell the kinds of difference apart is worse than none.

var { describe, it } = require('node:test');
var assert = require('assert');

var shadow = require('internal/helpers/shadow');
var tree = require('internal/helpers/tree');

function node(over) {
    return Object.assign({
        path: '', name: 'a', version: '1.0.0', type: 'registry', target_only: false,
        edges: {}, flags: {},
    }, over);
}

// the comparison skips the project itself, and a path is how it recognises it
function at(p, over) {
    return node(Object.assign({ path: p }, over));
}

describe('shadow: comparison', function () {
    it('says so when the two resolutions agree', function () {
        var a = new Map();
        a.set('', at('', { name: 'root' }));
        a.set('node_modules/x', at('node_modules/x', { name: 'x' }));

        var b = new Map();
        b.set('', at('', { name: 'root' }));
        b.set('node_modules/x', at('node_modules/x', { name: 'x' }));

        var r = shadow.compare(a, b);

        assert.equal(r.counts[shadow.NEW_ADDED], undefined);
        assert.equal(r.counts[shadow.OLD_ADDED], undefined);
        assert.equal(r.counts[shadow.SAME], 1, 'the one node both hold');
    });

    it('names a node only the old pass placed, and one only the new did', function () {
        var a = new Map();
        a.set('node_modules/x', at('node_modules/x', { name: 'x' }));

        var b = new Map();
        b.set('node_modules/y', at('node_modules/y', { name: 'y' }));

        var r = shadow.compare(a, b);

        assert.equal(r.counts[shadow.OLD_ADDED], 1);
        assert.equal(r.counts[shadow.NEW_ADDED], 1);

        var kinds = r.differences.map(function (d) { return d.kind; });
        assert.ok(kinds.indexOf(shadow.OLD_ADDED) > -1);
        assert.ok(kinds.indexOf(shadow.NEW_ADDED) > -1);
    });

    it('names a version difference at the same path', function () {
        var a = new Map();
        a.set('node_modules/x', at('node_modules/x', { name: 'x', version: '1.0.0' }));

        var b = new Map();
        b.set('node_modules/x', at('node_modules/x', { name: 'x', version: '2.0.0' }));

        var r = shadow.compare(a, b);

        assert.equal(r.counts[shadow.VERSION], 1);
        assert.deepEqual(r.differences[0].before, { version: '1.0.0' });
        assert.deepEqual(r.differences[0].after, { version: '2.0.0' });
    });

    it('counts a different hoisting as equivalent, not as a difference', function () {
        // both trees hold x@1.0.0 once, and one nests it: neither install is wrong,
        // and calling that an error would bury the differences that matter
        var a = new Map();
        a.set('node_modules/x', at('node_modules/x', { name: 'x' }));

        var b = new Map();
        b.set('node_modules/p/node_modules/x', at('node_modules/p/node_modules/x', { name: 'x' }));

        var r = shadow.compare(a, b);

        assert.equal(r.counts[shadow.SAME], 1, 'the same node is in both');
    });

    it('reports what it found as one line per kind, with an example', function () {
        var a = new Map();
        a.set('node_modules/x', at('node_modules/x', { name: 'x', version: '1.0.0' }));

        var b = new Map();
        b.set('node_modules/x', at('node_modules/x', { name: 'x', version: '2.0.0' }));
        b.set('node_modules/y', at('node_modules/y', { name: 'y' }));

        var text = shadow.report(shadow.compare(a, b));

        assert.ok(text.indexOf('version: 1') > -1, text);
        assert.ok(text.indexOf('new-added: 1') > -1, text);
        assert.ok(text.indexOf('node_modules/x') > -1, 'an example names the path');
    });

    it('is off unless it is asked for', function () {
        // nothing about the install may depend on the environment of whoever ran it
        assert.equal(shadow.active(), process.env.FIBJS_INSTALL_SHADOW === '1');
    });
});

describe('shadow: the snapshot as a path table', function () {
    function snap() {
        return {
            name: 'root', version: '1.0.0',
            dep_vs: { a: '^1' },
            dev_dep_vs: {}, opt_dep_vs: {}, peer_dep_vs: {},
            node_modules: {
                a: {
                    name: 'a', version: '1.0.0',
                    dep_vs: { b: '^2' }, dev_dep_vs: {}, opt_dep_vs: {}, peer_dep_vs: {},
                    node_modules: {
                        b: { name: 'b', version: '2.0.0', dep_vs: {}, node_modules: {} },
                    },
                },
            },
        };
    }

    it('flattens the nesting to lockfile paths', function () {
        var t = require('internal/helpers/tree').snapshot_to_tree(snap(), { root: process.cwd() });

        assert.ok(t.has(''), 'the project');
        assert.ok(t.has('node_modules/a'));
        assert.ok(t.has('node_modules/a/node_modules/b'));
        assert.equal(t.get('node_modules/a/node_modules/b').version, '2.0.0');
    });

    it('carries the declared ranges, which is what the resolver places', function () {
        var t = require('internal/helpers/tree').snapshot_to_tree(snap(), { root: process.cwd() });

        assert.deepEqual(t.get('node_modules/a').edges.dependencies, { b: '^2' });
        assert.deepEqual(t.get('').edges.dependencies, { a: '^1' });
    });

    it('marks a workspace member as a link, with its directory as a node of its own', function () {
        var s = snap();

        s.node_modules.m = {
            name: 'm', version: '0.1.0',
            workspace_package: true,
            workspace_path: require('path').join(process.cwd(), 'packages/m'),
            dep_vs: {}, dev_dep_vs: {}, opt_dep_vs: {}, peer_dep_vs: {},
            node_modules: {},
        };

        var t = require('internal/helpers/tree').snapshot_to_tree(s, { root: process.cwd() });

        assert.equal(t.get('node_modules/m').type, 'link');
        assert.equal(t.get('packages/m').target_only, true,
            'the directory is not an installation');
    });

    it('resolves an edge the way require() does', function () {
        var t = tree.snapshot_to_tree(snap(), { root: process.cwd() });

        assert.equal(tree.resolve_path(t, 'b', 'node_modules/a'), 'node_modules/a/node_modules/b');
        assert.equal(tree.resolve_path(t, 'a', ''), 'node_modules/a');
        assert.equal(tree.resolve_path(t, 'b', ''), null, 'not visible from the root');
    });
});
