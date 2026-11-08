const fs = require('fs');
const path = require('path');
const url = require('url');
const crypto = require('crypto');

/**
 * lockfile helper — the data layer of the lockfile support of `fibjs --install`
 * (see plans/installer-lockfile-plan.md).
 *
 * Reads the lockfile (v1 is converted to a path table in memory), verifies SRI,
 * replaces the registry of a `resolved` url, answers "do package.json and the
 * lockfile agree?" without the registry, and reads the registry/auth entries of
 * `.npmrc`. `from_tree()` (the write back) lands with P2.
 */

// ---------------------- SRI :start ------------------------- //

// strongest first: npm checks only the strongest algorithm a lock entry carries
const SRI_ALGORITHMS = ['sha512', 'sha384', 'sha256', 'sha1'];
const HEX_LENGTH = { sha512: 128, sha384: 96, sha256: 64, sha1: 40 };

/**
 * @description parse an SRI string (`sha512-<base64> sha1-<hex>`) into entries.
 *              `?options` after a digest are kept but do not take part in the
 *              comparison, and hex digests are accepted: npm writes
 *              `sha1-<hex>` when it can only fall back to `dist.shasum`
 * @returns {Array<{ raw, algorithm, digest, options, is_hex }>}
 */
function parse_sri(sri) {
    if (!sri || typeof sri !== 'string')
        return [];

    const entries = [];

    sri.trim().split(/\s+/).forEach(token => {
        const dash = token.indexOf('-');
        if (dash <= 0)
            return;

        const algorithm = token.slice(0, dash).toLowerCase();
        let digest = token.slice(dash + 1);
        let options = '';

        const question = digest.indexOf('?');
        if (question >= 0) {
            options = digest.slice(question + 1);
            digest = digest.slice(0, question);
        }

        if (!digest)
            return;

        entries.push({
            raw: token,
            algorithm: algorithm,
            digest: digest,
            options: options,
            is_hex: HEX_LENGTH[algorithm] === digest.length && /^[0-9a-f]+$/i.test(digest)
        });
    });

    return entries;
}

/**
 * @description the strongest algorithm present in the entries
 */
function best_algorithm(entries) {
    for (let i = 0; i < SRI_ALGORITHMS.length; i++) {
        const algorithm = SRI_ALGORITHMS[i];
        if (entries.some(e => e.algorithm === algorithm))
            return algorithm;
    }

    return null;
}

/**
 * @description digest a buffer
 * @param encoding 'base64' | 'hex'
 */
function digest(buf, algorithm, encoding) {
    return crypto.createHash(algorithm).update(buf).digest(encoding);
}

/**
 * @description verify a buffer against an SRI string, the way npm does it: only
 *              the strongest algorithm present is checked, and several digests of
 *              that algorithm are alternatives (any match passes)
 * @returns {{ checked, ok, algorithm, expected, actual }} — `checked` is false
 *          when there is nothing to verify (an entry without `integrity`), which
 *          is the caller's business (the frozen install refuses it)
 */
function verify_integrity(buf, sri) {
    const entries = parse_sri(sri);
    const algorithm = best_algorithm(entries);

    if (!algorithm)
        return { checked: false, ok: true, algorithm: null, expected: null, actual: null };

    const expected = entries.filter(e => e.algorithm === algorithm);
    const actual = {
        base64: digest(buf, algorithm, 'base64'),
        hex: digest(buf, algorithm, 'hex')
    };

    const ok = expected.some(e => e.digest === (e.is_hex ? actual.hex : actual.base64));

    return {
        checked: true,
        ok: ok,
        algorithm: algorithm,
        expected: expected.map(e => e.raw).join(' '),
        actual: `${algorithm}-${actual.base64}`
    };
}

/**
 * @description what a downloader has to hash while it streams a tarball, and how
 *              to compare the result: npm checks only the strongest algorithm the
 *              entry carries, so that is the one to hash with
 * @returns {null | { algorithm, entries }} — null when there is nothing to verify
 */
function integrity_plan(sri) {
    const entries = parse_sri(sri);
    const algorithm = best_algorithm(entries);

    if (!algorithm)
        return null;

    return {
        algorithm: algorithm,
        entries: entries.filter(e => e.algorithm === algorithm)
    };
}

/**
 * @description check a digest a streaming download computed (hex) against a plan
 * @returns {{ ok, expected, actual }}
 */
function integrity_ok(hex, plan) {
    const base64 = Buffer.from(hex, 'hex').toString('base64');
    const ok = plan.entries.some(e => e.digest === (e.is_hex ? hex : base64));

    return {
        ok: ok,
        expected: plan.entries.map(e => e.raw).join(' '),
        actual: `${plan.algorithm}-${base64}`
    };
}

// ---------------------- registry replacement :start ------------------------- //

/**
 * @description `resolved` of a registry entry points at the default registry as a
 *              magic value: reading a lockfile, that URL is served by the registry
 *              configured now. Mirrors npm's `replace-registry-host`
 *              (pacote/lib/remote.js): the origin comes from the configured
 *              registry, the path comes from the resolved url
 * @param policy 'npmjs' (default) | 'never' | 'always' | '<hostname>'
 */
function apply_registry_replace(resolved, registry, policy) {
    if (!resolved || typeof resolved !== 'string')
        return resolved;

    const magic = (!policy || policy === 'npmjs') ? 'registry.npmjs.org' : policy;
    if (magic === 'never' || !registry)
        return resolved;

    // git / file / link entries are not registry urls: leave them alone
    if (!/^https?:\/\//i.test(resolved))
        return resolved;

    const parsed = url.parse(resolved);
    if (!parsed.host)
        return resolved;

    if (magic !== 'always' && parsed.host !== magic)
        return resolved;

    const base = url.parse(registry);
    if (!base.host)
        return resolved;

    const pathname = parsed.pathname && parsed.pathname[0] === '/' ? parsed.pathname : '/' + parsed.pathname;

    return `${base.protocol}//${base.auth ? base.auth + '@' : ''}${base.host}${pathname}`;
}

// ---------------------- lockfile reading :start ------------------------- //

// npm-shrinkwrap.json wins over package-lock.json, npm does the same
const LOCKFILE_NAMES = ['npm-shrinkwrap.json', 'package-lock.json'];

/**
 * @description the lockfile versions the installer can read. v1 has no `packages`
 *              path table (the frozen install converts its `dependencies` tree in
 *              memory), v2/v3 carry it, and anything else is refused by the caller
 */
function supported_version(version) {
    return version === 1 || version === 2 || version === 3;
}

/**
 * @description read the project lockfile
 * @returns null when there is none, otherwise
 *          { filename, path, lockfileVersion, packages, legacy, root, raw }
 *          - packages: the v2/v3 path table (empty for v1)
 *          - legacy:   the v1/v2 `dependencies` tree (null when absent)
 *          - root:     the `""` entry, i.e. what the lock says the root declares
 */
function read_lockfile(root) {
    for (let i = 0; i < LOCKFILE_NAMES.length; i++) {
        const filename = LOCKFILE_NAMES[i];
        const file = path.join(root, filename);

        if (!fs.exists(file))
            continue;

        const text = fs.readTextFile(file).replace(/^\uFEFF/, '');
        let data;

        try {
            data = JSON.parse(text);
        } catch (e) {
            throw new Error(`[lockfile] ${filename} is not valid JSON: ${e.message}`);
        }

        // a lockfile without `lockfileVersion` predates npm 5 and only has the
        // legacy tree; npm 5/6 wrote version 1 explicitly
        const version = typeof data.lockfileVersion === 'number' ? data.lockfileVersion
            : (data.dependencies ? 1 : 0);

        return {
            filename: filename,
            path: file,
            lockfileVersion: version,
            packages: data.packages || {},
            legacy: data.dependencies || null,
            root: (data.packages && data.packages['']) || null,
            raw: data
        };
    }

    return null;
}

// ---------------------- path table :start ------------------------- //

/**
 * @description the package name a `packages` key refers to: `node_modules/a` and
 *              `node_modules/x/node_modules/@s/a` both name `a` / `@s/a`
 * @returns null for a key that is not inside a node_modules directory (npm writes
 *          such an entry for the target of a link, it installs nothing itself)
 */
function name_from_path(p) {
    const marker = 'node_modules/';
    const idx = p.lastIndexOf(marker);

    return idx < 0 ? null : p.slice(idx + marker.length);
}

/**
 * @description normalize a lock entry to the fields the installer reads. A v1 entry
 *              carries the declared ranges under `requires` — its `dependencies` is
 *              the nested tree, which becomes nested paths instead
 */
function normalize_entry(entry, p, legacy) {
    const path_name = name_from_path(p);

    const e = {
        path: p,
        // the name it is installed under — its path — and the name the package goes
        // by, which npm writes only when the two differ (that is an alias:
        // `node_modules/string-width-cjs` holding `string-width`)
        name: entry.name || path_name || p,
        real_name: entry.name && path_name && entry.name !== path_name ? entry.name : undefined,
        version: entry.version,
        resolved: entry.resolved,
        integrity: entry.integrity,
        link: entry.link === true,
        dev: entry.dev === true,
        optional: entry.optional === true,
        in_bundle: entry.inBundle === true,
        bundle_dependencies: entry.bundleDependencies !== undefined ? entry.bundleDependencies
            : entry.bundledDependencies,
        has_install_script: entry.hasInstallScript === true,
        legacy: !!legacy,
        // the entry as the file had it: a write back keeps it for anything it does
        // not produce itself (license, engines, funding…)
        raw_entry: entry,
        bin: entry.bin,
        os: entry.os,
        cpu: entry.cpu,
        libc: entry.libc,
        dependencies: legacy ? (entry.requires || null) : (entry.dependencies || null),
        optionalDependencies: legacy ? null : (entry.optionalDependencies || null),
        peerDependencies: legacy ? null : (entry.peerDependencies || null),
        peerDependenciesMeta: legacy ? null : (entry.peerDependenciesMeta || null),
        // npm writes an entry for the directory a link points at too, and that one
        // installs nothing itself — but a package a workspace member keeps nested
        // (`packages/member/node_modules/x`) is an installation target like any other,
        // and taking it for a link target is how a frozen install came to skip it
        target_only: p.indexOf('node_modules/') < 0,
    };

    // a v1 entry keeps the source in `version` when it is not a registry version:
    // `file:…` for a local package, a git url for a repository. npm converts those
    // to link / git entries when it reads a v1 lock, and so does this
    if (legacy && typeof e.version === 'string' && !/^\d/.test(e.version)) {
        if (e.version.indexOf('file:') === 0 || /^(\.{1,2}[\\/]|[\\/]|[a-zA-Z]:[\\/])/.test(e.version)) {
            e.link = true;
            e.resolved = e.version.replace(/^file:/, '');
        } else if (/^(git\+|git:\/\/|ssh:\/\/|github:|gitlab:|bitbucket:)/.test(e.version)) {
            e.resolved = e.resolved || e.version;
        }
    }

    return e;
}

/**
 * @description the lockfile as a path table: `packages` for v2/v3, the nested
 *              `dependencies` tree converted for v1. npm reads v1 the same way (it
 *              converts it in memory and only writes v1 when it was asked to)
 * @returns { [path]: entry }
 */
function to_path_map(lock) {
    const map = {};

    if (lock.lockfileVersion >= 2 && lock.packages && Object.keys(lock.packages).length > 0) {
        Object.keys(lock.packages).forEach(p => {
            if (p === '')
                return;     // the root entry mirrors package.json, it installs nothing
            map[p] = normalize_entry(lock.packages[p], p, false);
        });

        return map;
    }

    const walk = (deps, prefix) => {
        Object.keys(deps || {}).forEach(name => {
            const node = deps[name] || {};
            const p = prefix + 'node_modules/' + name;

            map[p] = normalize_entry(node, p, true);
            walk(node.dependencies, p + '/');
        });
    };

    // `read_lockfile` hands the legacy tree over as `legacy`; a lock object built by
    // hand (tests, callers) may carry it under the name the file itself uses
    walk(lock.legacy || lock.dependencies || null, '');

    return map;
}

// ---------------------- spec satisfaction :start ------------------------- //

/**
 * @description can this lock entry satisfy that spec, judged offline. Anything the
 *              installer cannot decide without the registry (a dist-tag, a bare
 *              url) is accepted when the entry exists: the lock is the authority
 * @param from the package that declares the spec, relative to the project root (a
 *             local spec is relative to it)
 */
function spec_satisfied(spec, entry, semver, from) {
    if (typeof spec !== 'string' || spec === '')
        return { ok: false, reason: 'empty spec' };

    if (spec.indexOf('workspace:') === 0)
        return entry.link ? { ok: true } : { ok: false, reason: 'not a link entry' };

    if (spec.indexOf('file:') === 0 || /^(\.{1,2}[\\/]|[\\/]|~[\\/]|[a-zA-Z]:[\\/])/.test(spec))
        return file_spec_satisfied(spec, entry, from);

    if (spec.indexOf('npm:') === 0) {
        // `npm:real-name@range`: the entry sits at the alias path and carries the
        // real name (npm writes `name` only when it differs from the path)
        const target = spec.slice(4);
        const at = target.lastIndexOf('@');
        const real = at > 0 ? target.slice(0, at) : target;
        const range = at > 0 ? target.slice(at + 1) : '';
        const have = entry.real_name || entry.name;

        if (have !== real)
            return { ok: false, reason: `alias points at ${have}` };

        return range ? semver_satisfied(range, entry, semver) : { ok: true };
    }

    if (/^(git\+|git:\/\/|ssh:\/\/|github:|gitlab:|bitbucket:)/.test(spec) || /\.git(#|$)/.test(spec))
        return git_spec_satisfied(spec, entry);

    if (/^https?:\/\//.test(spec))
        return entry.resolved === spec ? { ok: true } : { ok: false, reason: `resolved is ${entry.resolved}` };

    if (semver && semver.validRange(spec))
        return semver_satisfied(spec, entry, semver);

    // a dist-tag (`latest`, `next`) names no version: offline, only its presence is
    // knowable — the entry being there is all we can ask for
    return entry.version || entry.link ? { ok: true } : { ok: false, reason: 'no version in the lock entry' };
}

function semver_satisfied(range, entry, semver) {
    if (!semver)
        throw new Error('[lockfile] a semver implementation is required to check a version range');

    // a link has no version of its own: the workspace/local package provides it
    if (entry.link)
        return { ok: true };

    if (!entry.version)
        return { ok: false, reason: 'no version in the lock entry' };

    return semver.satisfies(entry.version, range)
        ? { ok: true }
        : { ok: false, reason: `${entry.version} does not satisfy ${range}` };
}

/**
 * @description a `file:` / local spec is satisfied by the link entry that points at
 *              the same directory. Comparing the target (not just the presence of an
 *              entry) is what catches a lock left over from a moved directory.
 *              The spec is relative to the package that declares it — a workspace
 *              member says `file:../../x` while the lockfile records `x` — so it is
 *              resolved against `from` first
 */
function file_spec_satisfied(spec, entry, from) {
    if (!entry.link)
        return { ok: false, reason: 'not a link entry' };

    const raw = spec.replace(/^file:/, '');
    // `from` is a lockfile path (always `/`) or a platform path — a workspace
    // member comes back with `\` on Windows — and the spec may use either: both are
    // read as `/`, otherwise `path.posix.join` leaves the `..` segments standing
    // and `packages/member/../../linkpkg` never equals `linkpkg`
    const base = String(from || '').replace(/\\/g, '/');
    const want = /^([\\/]|[a-zA-Z]:[\\/])/.test(raw)
        ? normalize_local_target(raw)
        : normalize_local_target(path.posix.join(base, raw.replace(/\\/g, '/')));
    const got = normalize_local_target(entry.resolved || '');

    return want === got ? { ok: true } : { ok: false, reason: `points at ${entry.resolved}` };
}

/**
 * @description compare local targets as they are written: relative to the project,
 *              with `/` separators and without a trailing slash
 */
function normalize_local_target(p) {
    return String(p)
        .replace(/\\/g, '/')
        .replace(/\/+$/, '')
        .replace(/^\.\//, '');
}

/**
 * @description a git spec is satisfied when the lock pins the same repository. The
 *              ref is deliberately not re-resolved: the recorded commit is the pin
 */
function git_spec_satisfied(spec, entry) {
    const resolved = entry.resolved || '';
    const hash = resolved.indexOf('#');

    if (resolved.indexOf('git+') !== 0 && resolved.indexOf('git://') !== 0)
        return { ok: false, reason: 'not a git entry' };

    if (hash < 0)
        return { ok: false, reason: 'the lock entry is not pinned to a commit' };

    // npm always records the commit it resolved to. A branch or a tag in there means
    // somebody wrote (or hand edited) an entry that can move under the install
    const committish = resolved.slice(hash + 1);
    if (!/^[0-9a-f]{7,40}$/i.test(committish))
        return { ok: false, reason: `the lock entry is pinned to ${committish}, not to a commit` };

    const want = git_repo_of(spec);
    const got = git_repo_of(resolved);

    return want && got && want === got
        ? { ok: true }
        : { ok: false, reason: `points at ${resolved.split('#')[0]}` };
}

/**
 * @description the repository a git url/spec names, in one comparable form
 */
function git_repo_of(u) {
    return String(u).split('#')[0]
        .replace(/^git\+/, '')
        .replace(/^git@([^:]+):/, 'https://$1/')     // the scp form: git@host:path
        .replace(/^ssh:\/\/git@/, 'https://')
        .replace(/^git:\/\//, 'https://')
        .replace(/^github:/, 'https://github.com/')
        .replace(/^gitlab:/, 'https://gitlab.com/')
        .replace(/^bitbucket:/, 'https://bitbucket.org/')
        .replace(/\.git$/, '')
        .replace(/\/+$/, '');
}

/**
 * @description the paths a dependency is looked up at, from the dependent outwards:
 *              npm resolves `<dir>/node_modules/<name>` and then walks up, one
 *              directory at a time, to the root. `from` is the lockfile path of the
 *              dependent ('' for the project root, `packages/member` for a workspace
 *              member, `node_modules/a` for a package)
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
 * @description find the entry a dependency resolves to, offline. npm walks the
 *              directories of the dependent outwards and takes the first entry that
 *              satisfies the spec; an entry that sits outside that path (nested under
 *              an unrelated package) is not visible to it, and the sync check must not
 *              count it either — that is how a manifest came to look "in sync" with a
 *              lock that installs something else
 * @param opts { nested } — also accept the shallowest nested entry that satisfies the
 *              spec. Resolving runs before hoisting, so a node has no final path yet
 *              and the plan's approximation (§4.2) is what is left; the sync check,
 *              which does know the paths, asks for the exact geometry instead
 */
function lookup_entry(packages, name, spec, semver, from, opts) {
    const o = opts || {};
    const paths = lookup_paths(name, from);

    for (let i = 0; i < paths.length; i++) {
        const entry = packages[paths[i]];
        if (entry && spec_satisfied(spec, entry, semver, from).ok)
            return entry;
    }

    if (o.nested) {
        const suffix = '/node_modules/' + name;
        const nested = Object.keys(packages)
            .filter(p => p.endsWith(suffix))
            .sort((a, b) => a.split('/').length - b.split('/').length);

        for (let i = 0; i < nested.length; i++) {
            const entry = packages[nested[i]];
            if (spec_satisfied(spec, entry, semver, from).ok)
                return entry;
        }
    }

    // nothing along the path satisfies the spec: hand back the nearest entry that does
    // exist, so the caller can report "Invalid" (npm's word) rather than "Missing"
    for (let i = 0; i < paths.length; i++) {
        if (packages[paths[i]])
            return packages[paths[i]];
    }

    return null;
}

/**
 * @description the embedded semver helper, loaded lazily so this module keeps
 *              working (and stays testable) without it; `opts.semver` overrides
 */
function default_semver() {
    try {
        return require('internal/helpers/semver');
    } catch (e) {
        return null;
    }
}

/**
 * @description the entry a dependency at `from` resolves to, or null: npm takes the
 *              first entry along the directories of the dependent, outwards
 */
function resolve_path(packages, name, from) {
    const paths = lookup_paths(name, from);

    for (let i = 0; i < paths.length; i++) {
        if (packages[paths[i]])
            return paths[i];
    }

    return null;
}

/**
 * @description which kind of edge the lockfile's own edges say reaches an entry —
 *              `dev` / `optional` / `devOptional`, read the way npm reads them
 *              (`calc-dep-flags.js`): "no path from a root arrives without an edge of
 *              that kind". A lockfile written before the flags were recorded
 *              describes a platform specific binary as a package every machine
 *              needs: its `os`/`cpu` say where it belongs, and only a flag says it
 *              may be left out — without one it is downloaded and installed on a
 *              machine that cannot run it, and `npm ci` refuses the lock outright
 *              (`EBADPLATFORM`)
 * @param packages the `to_path_map` shape
 * @param root_entry the root entry of the lockfile, which the path map leaves out:
 *              it installs nothing, and every edge starts there
 * @param opts { workspaces: [{ path, pkgjson }] } — a member's manifest, which a
 *              lockfile written before members recorded their dependencies does not
 *              carry: without it the tree a member reaches is not part of the graph
 * @returns { dev, optional, devOptional } — each a `{ [path]: true }` map
 */
function graph_flags(packages, root_entry, opts) {
    const o = opts || {};
    const manifests = {};

    (o.workspaces || []).forEach(w => {
        if (w && w.path)
            manifests[String(w.path).replace(/\/+$/, '')] = w.pkgjson || {};
    });

    const entry_at = p => {
        if (p === '')
            return root_entry || {};

        // a member's manifest is what npm resolves its edges from: the tree has a
        // link where the lockfile has an entry, and the manifest is the truth
        if (manifests[p])
            return manifests[p];

        return packages[p] || {};
    };

    // a workspace member — the target of a link, that is — is a project of its own:
    // no edge from the root arrives at it, yet what it needs is part of the graph
    const roots = [''];

    Object.keys(packages).forEach(p => {
        if (packages[p].target_only)
            roots.push(p);
    });

    const fields = ['dependencies', 'devDependencies', 'peerDependencies', 'optionalDependencies'];

    /**
     * @description walk the edges the flag does not rule out. The nearest entry wins,
     *              which is also what a require() at run time does
     */
    function reach(skip) {
        const seen = {};
        const stack = [];

        roots.forEach(r => { seen[r] = true; stack.push(r); });

        while (stack.length) {
            const from = stack.pop();
            const entry = entry_at(from);
            // a link carries no edges of its own: what it depends on is installed
            // inside the directory it points at
            const target = entry.link && entry.resolved ? entry.resolved : from;
            const node = entry_at(target);

            if (target !== from && !seen[target]) {
                seen[target] = true;
                stack.push(target);
            }

            fields.forEach(field => {
                if (skip.indexOf(field) > -1)
                    return;

                const map = node[field];

                if (!map)
                    return;

                Object.keys(map).forEach(name => {
                    const to = resolve_path(packages, name, target);

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
    };
    const flags = { dev: {}, optional: {}, devOptional: {} };

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

/**
 * @description the peer edges a lockfile does not satisfy. This is npm's own
 *              question: `ERESOLVE` is asked while resolving, and `npm ci` refuses a
 *              lockfile whose peers cannot be met — even one written with
 *              `--legacy-peer-deps`. The answer is judged along the paths npm
 *              resolves along, so a lockfile `npm ci` accepts is clean here as well
 * @param opts { semver }
 * @returns [{ path, name, spec, found, why }] — one entry per peer edge that is not
 *          met, `found` null when the lockfile has no copy of it
 */
function check_peers(lock, opts) {
    const o = opts || {};
    const semver = o.semver || default_semver();
    const packages = to_path_map(lock);
    const unmet = [];

    Object.keys(packages).forEach(p => {
        const entry = packages[p];

        // a link's own manifest is the directory it points at (npm reads it there),
        // and a bundled copy lives inside its parent's tarball
        if (entry.target_only || entry.in_bundle)
            return;

        const peers = entry.peerDependencies || {};
        const meta = entry.peerDependenciesMeta || {};

        Object.keys(peers).forEach(name => {
            // an optional peer stays uninstalled, npm does the same
            if (meta[name] && meta[name].optional)
                return;

            const spec = peers[name];
            const found = lookup_entry(packages, name, spec, semver, p);
            const verdict = found ? spec_satisfied(spec, found, semver, p) : null;

            if (!verdict || !verdict.ok)
                unmet.push({
                    path: p,
                    name: name,
                    spec: spec,
                    found: found ? found.version : null,
                    why: verdict ? verdict.reason : 'not in the lockfile',
                });
        });
    });

    return unmet;
}

/**
 * @description the offline sync check of the plan's §4.5, in two steps: the root
 *              edges (package.json and every workspace member against the lock),
 *              then the closure of the lock itself (everything an entry declares has
 *              to resolve inside the lock). npm needs the registry to answer this
 *              (it resolves an ideal tree first); this does not, so it can fail
 *              before anything is touched
 * @param opts { semver, workspaces: [{ name, path, pkgjson }] }
 * @returns { ok, errors: [{ kind, name, spec, locked, via, why }], warnings }
 */
function check_sync(root_pkgjson, lock, opts) {
    const o = opts || {};
    const semver = o.semver || default_semver();
    const packages = to_path_map(lock);
    const errors = [];
    const warnings = [];

    const roots = [{ where: '', label: 'package.json', pkgjson: root_pkgjson }];
    (o.workspaces || []).forEach(w => roots.push({
        where: w.path || '',
        label: `workspace ${w.name}`,
        pkgjson: w.pkgjson,
    }));

    // step 1: every edge the project declares has to be satisfied by the lock
    roots.forEach(root => {
        ['dependencies', 'devDependencies', 'optionalDependencies'].forEach(key => {
            const deps = root.pkgjson && root.pkgjson[key];
            if (!deps)
                return;

            Object.keys(deps).forEach(name => {
                const spec = deps[name];
                const entry = lookup_entry(packages, name, spec, semver, root.where);

                if (!entry) {
                    errors.push({ kind: 'Missing', name: name, spec: spec, via: root.label });
                    return;
                }

                const verdict = spec_satisfied(spec, entry, semver, root.where);
                if (!verdict.ok)
                    errors.push({
                        kind: 'Invalid', name: name, spec: spec,
                        locked: entry.version, via: root.label, why: verdict.reason,
                    });
            });
        });
    });

    // step 2: the lock has to be closed under its own declarations, otherwise the
    // frozen install would have to resolve something (which is not frozen at all)
    Object.keys(packages).forEach(p => {
        const entry = packages[p];

        // a link target is not installed on its own, and a bundled dependency lives
        // inside its parent's tarball: npm does not describe their deps separately
        if (entry.target_only || entry.in_bundle)
            return;

        const declared = Object.assign({}, entry.dependencies, entry.optionalDependencies);
        Object.keys(declared).forEach(name => {
            const spec = declared[name];
            const optional = !!(entry.optionalDependencies && entry.optionalDependencies[name] !== undefined);

            // `nested` on purpose: npm writes lockfiles in which a package's edge is
            // satisfied by a copy that sits beside it rather than above it (its own
            // trees do this), and `npm ci` accepts those — only an entry that is
            // missing from the whole table is a hole. A *manifest* edge is held to the
            // path it resolves along (step 1), which is where a stale lockfile lies
            const found = lookup_entry(packages, name, spec, semver, p, { nested: true });

            // a v1 `requires` mixes dependency kinds (npm 5/6 era), so a miss there is
            // reported without blocking: v2/v3 entries are checked strictly
            const soft = optional || entry.legacy;

            if (!found) {
                (soft ? warnings : errors).push({
                    kind: 'Missing', name: name, spec: spec, via: p, optional: optional,
                });
                return;
            }

            const verdict = spec_satisfied(spec, found, semver, p);
            if (!verdict.ok)
                (soft ? warnings : errors).push({
                    kind: 'Invalid', name: name, spec: spec,
                    locked: found.version, via: p, why: verdict.reason,
                });
        });
    });

    return { ok: errors.length === 0, errors: errors, warnings: warnings };
}

/**
 * @description the entries a project actually reaches, read from the lockfile's own
 *              edges: the roots (the project and every workspace member) and what
 *              their declarations lead to, walking the way `require()` would. This is
 *              the set an install of that project has to leave on the disk — an entry
 *              no path from a root arrives at is one a dependency removed from
 *              package.json left behind, which is what npm's `prune` removes
 * @param packages the `to_path_map` shape
 * @param root_entry the lockfile's `""` entry (what package.json declares)
 * @param opts { workspaces: [{ path, pkgjson }] }
 * @returns { [path]: true }
 */
function reachable_paths(packages, root_entry, opts) {
    const o = opts || {};
    const manifests = {};

    (o.workspaces || []).forEach(w => {
        if (w && w.path)
            manifests[String(w.path).replace(/\/+$/, '')] = w.pkgjson || {};
    });

    const entry_at = p => {
        if (p === '')
            return root_entry || {};

        if (manifests[p])
            return manifests[p];

        return packages[p] || {};
    };

    const reachable = {};

    // the walk starts from what the project declares in its own manifest, and only
    // from that. A link is *not* a root: the entry exists because an edge from the
    // manifest asked for it, and one whose edge is gone is an entry nothing installs —
    // treating it as a root is what kept a removed `file:` dependency's whole subtree
    // reachable
    const roots = [''];

    // a workspace member is the exception, and the manifest says so: `workspaces` names
    // it, and no dependency edge arrives at it
    (o.workspaces || []).forEach(w => {
        if (!w || !w.path)
            return;

        const target = String(w.path).replace(/\/+$/, '');

        Object.keys(packages).forEach(p => {
            if (packages[p].link && packages[p].resolved === target)
                roots.push(p);
        });
    });

    const fields = ['dependencies', 'devDependencies', 'peerDependencies', 'optionalDependencies'];
    const seen = {};
    const stack = [];

    roots.forEach(r => {
        seen[r] = true;
        stack.push(r);
    });

    while (stack.length) {
        const from = stack.pop();
        const entry = entry_at(from);

        if (from !== '' && !manifests[from])
            reachable[from] = true;

        // a link carries no edges of its own: what it depends on is installed inside
        // the directory it points at
        const target = entry.link && entry.resolved ? entry.resolved : from;
        const node = entry_at(target);

        if (target !== from && !seen[target]) {
            seen[target] = true;
            stack.push(target);
        }

        fields.forEach(field => {
            const map = node[field];

            if (!map)
                return;

            Object.keys(map).forEach(name => {
                const to = resolve_path(packages, name, target);

                if (!to || seen[to])
                    return;

                seen[to] = true;
                stack.push(to);
            });
        });
    }

    return reachable;
}

// ---------------------- .npmrc :start ------------------------- //

/**
 * @description the minimum of `.npmrc` the installer needs: the registry, the per
 *              scope registries, the auth entries, and the TLS settings (a private
 *              CA, or no verification at all). Precedence follows npm: environment >
 *              project `.npmrc` > user `~/.npmrc`; the caller keeps `package.json`'s
 *              own `registry` field as the last fallback
 * @returns { registry, scoped, auth, strict_ssl, cafile, ca, sources }
 */
function read_npmrc(root, opts) {
    const o = opts || {};
    const env = o.env || process.env;
    const result = {
        registry: null, scoped: {}, auth: {},
        strict_ssl: true, cafile: null, ca: null,
        sources: [],
    };

    // the user file first, the project file second: the later one wins
    const files = [];
    const home = env.HOME || env.USERPROFILE;

    if (home)
        files.push(path.join(home, '.npmrc'));
    if (root)
        files.push(path.join(root, '.npmrc'));

    files.forEach(file => {
        if (!fs.exists(file))
            return;

        result.sources.push(file);

        parse_npmrc(fs.readTextFile(file), env).forEach(item => {
            if (item.key === 'registry') {
                result.registry = item.value;
                return;
            }

            if (item.key === 'strict-ssl') {
                result.strict_ssl = item.value !== 'false';
                return;
            }

            if (item.key === 'cafile') {
                result.cafile = item.value;
                return;
            }

            if (item.key === 'ca') {
                result.ca = item.value;
                return;
            }

            const scoped = /^(@[^:]+):registry$/.exec(item.key);
            if (scoped) {
                result.scoped[scoped[1]] = item.value;
                return;
            }

            // `//host/path/:_authToken=…` — everything before `:_` is the prefix the
            // token applies to (it already ends with the `/` npm matches on)
            if (item.key.charAt(0) === '/' && item.key.indexOf(':_') > 0) {
                const sep = item.key.lastIndexOf(':_');
                const prefix = item.key.slice(0, sep);
                const name = item.key.slice(sep + 2);
                const bucket = result.auth[prefix] || (result.auth[prefix] = {});

                bucket[name] = item.value;
            }
        });
    });

    // npm's environment wins over both files
    if (env.npm_config_registry)
        result.registry = env.npm_config_registry;

    return result;
}

/**
 * @description one `.npmrc`: `key=value` lines, `;`/`#` comments, quoted values and
 *              `${VAR}` expansion (npm expands environment variables in there)
 */
function parse_npmrc(text, env) {
    const items = [];

    String(text).split(/\r?\n/).forEach(line => {
        const trimmed = line.trim();
        if (!trimmed || trimmed.charAt(0) === ';' || trimmed.charAt(0) === '#')
            return;

        const eq = trimmed.indexOf('=');
        if (eq <= 0)
            return;

        const key = trimmed.slice(0, eq).trim();
        let value = trimmed.slice(eq + 1).trim();

        if (value.length > 1 && ((value.charAt(0) === '"' && value.charAt(value.length - 1) === '"') ||
            (value.charAt(0) === "'" && value.charAt(value.length - 1) === "'")))
            value = value.slice(1, -1);

        value = value.replace(/\$\{([^}]+)\}/g, (m, name) => (env[name] !== undefined ? env[name] : m));

        items.push({ key: key, value: value });
    });

    return items;
}

/**
 * @description the auth entry that applies to a url: npm matches the longest
 *              `//host/path/` prefix, so a token registered for a sub path wins
 */
function auth_for_url(u, auth) {
    if (!u || !auth)
        return null;

    // npm compares the url without its protocol, e.g. `//registry.example.com/pkg`
    const bare = String(u).replace(/^[a-z][a-z0-9+.-]*:/i, '');
    let best = null;

    Object.keys(auth).forEach(prefix => {
        if (bare.indexOf(prefix) === 0 && (!best || prefix.length > best.length))
            best = prefix;
    });

    return best ? auth[best] : null;
}

/**
 * @description the header a fetch of that url needs, when `.npmrc` carries
 *              credentials for it
 * @returns { authorization } | null
 */
function auth_header_for_url(u, auth) {
    const bucket = auth_for_url(u, auth);
    if (!bucket)
        return null;

    if (bucket.authToken)
        return { authorization: `Bearer ${bucket.authToken}` };

    if (bucket.auth)
        return { authorization: `Basic ${bucket.auth}` };

    if (bucket.username && bucket.password !== undefined)
        return { authorization: `Basic ${Buffer.from(`${bucket.username}:${bucket.password}`).toString('base64')}` };

    return null;
}

// ---------------------- writing a lockfile :start ------------------------- //

// npm's field order (arborist/lib/shrinkwrap.js): a lockfile written here reads
// like one written by npm, which keeps the diff small when both tools touch it
//
// npm serializes through `json-stringify-nice` with that list of named keys: the
// named keys first, in that order, then the rest of the plain values, then the
// objects, each group in alphabetical order. A fixed list of all known keys gets a
// one-object entry right and a rich one wrong — `engines`/`funding` before `cpu`
// and `license`, say — and `npm install` rewrites the whole file just to reorder it
const NAMED_KEY_ORDER = [
    'name', 'version', 'lockfileVersion', 'resolved', 'integrity', 'requires', 'packages',
    'dependencies',
];
const TOP_KEY_ORDER = ['name', 'version', 'lockfileVersion', 'requires', 'packages', 'dependencies'];
const LEGACY_KEY_ORDER = ['version', 'resolved', 'integrity', 'dev', 'optional', 'requires', 'dependencies'];

/**
 * @description the keys of an object in a given order, unknown keys last
 */
function order_keys(obj, order) {
    const out = {};
    const seen = {};

    order.forEach(k => {
        if (obj[k] !== undefined) {
            out[k] = obj[k];
            seen[k] = true;
        }
    });

    Object.keys(obj).forEach(k => {
        if (!seen[k] && obj[k] !== undefined)
            out[k] = obj[k];
    });

    return out;
}

/**
 * @description the keys of a lockfile entry the way npm writes them. npm serializes
 *              through `json-stringify-nice`, which sorts every object in the file:
 *              the plain values first (the named keys among them in that order, the
 *              rest alphabetically), then the objects, alphabetically as well — the
 *              named keys are consulted inside a group, not across one, which is why
 *              `workspaces` comes before `dependencies` in a root entry
 */
function entry_order(obj) {
    const present = Object.keys(obj).filter(k => obj[k] !== undefined);
    const plain = [];
    const nested = [];

    // an array counts as plain here: `json-stringify-nice` only defers objects
    present.forEach(k => {
        (obj[k] && typeof obj[k] === 'object' && !Array.isArray(obj[k]) ? nested : plain).push(k);
    });

    const named_index = k => NAMED_KEY_ORDER.indexOf(k);
    const compare = (a, b) => {
        const ia = named_index(a);
        const ib = named_index(b);

        if (ia > -1 && ib > -1)
            return ia - ib;

        if (ia > -1)
            return -1;

        if (ib > -1)
            return 1;

        return a.localeCompare(b, 'en');
    };

    const out = {};

    plain.sort(compare).concat(nested.sort(compare)).forEach(k => { out[k] = obj[k]; });

    return out;
}

/**
 * @description the text of a lockfile the way npm would write it: the ordering
 *              reaches every object in the file, the `packages` paths and the
 *              dependency maps included, exactly as the serializer npm uses does
 */
function stringify_lock(lock, indent) {
    return JSON.stringify(lock, function (key, value) {
        return value && typeof value === 'object' && !Array.isArray(value)
            ? entry_order(value)
            : value;
    }, indent === undefined ? 2 : indent);
}

/**
 * @description the path npm records for a link: relative to the project, `/` only
 */
function relative_target(root, target) {
    const rel = path.relative(root || '.', target).replace(/\\/g, '/');

    return rel || '.';
}

/**
 * @description npm normalises what a package installs its commands from
 *              (arborist's `normalizeBinPaths`): a path written as `./bin/x` is
 *              recorded as `bin/x`, and a `bin` that is a single string becomes a map
 *              under the package's own (unscoped) name. A lockfile that says otherwise
 *              is rewritten
 */
function normalize_bin(bin, name) {
    if (!bin)
        return undefined;

    if (typeof bin === 'string') {
        const single = {};

        single[String(name || '').replace(/^@[^\/]+\//, '')] = bin.replace(/^\.\//, '');

        return single;
    }

    const out = {};

    Object.keys(bin).forEach(k => { out[k] = String(bin[k]).replace(/^\.\//, ''); });

    return out;
}

/**
 * @description the name a lockfile path already implies: the leaf inside a
 *              node_modules directory (`node_modules/@s/a` → `@s/a`), or the directory
 *              itself (`packages/@s/a` → `@s/a`). npm writes `name` only where the
 *              package disagrees with it — an alias, or a member whose directory is
 *              not called what its manifest says
 */
function implied_name(p) {
    const from_nm = name_from_path(p);

    return from_nm || name_from_folder(p);
}

/**
 * @description the name a package directory implies when its manifest names nothing,
 *              the way npm derives it (`@npmcli/name-from-folder`): the directory's
 *              own name, with the scope directory it sits in as a prefix — `pkg` for
 *              `packages/pkg`, `@s/pkg` for `packages/@s/pkg`
 */
function name_from_folder(dir) {
    const p = String(dir || '').replace(/\\/g, '/').replace(/\/+$/, '');
    const base = p.slice(p.lastIndexOf('/') + 1);
    const parent_path = p.slice(0, p.lastIndexOf('/'));
    const parent = parent_path.slice(parent_path.lastIndexOf('/') + 1);

    return parent.charAt(0) === '@' ? `${parent}/${base}` : base;
}

// the package.json fields npm projects into a lock entry (arborist's `pkgMetaKeys`).
// An entry for a directory — the root, or the target of a link — is that projection,
// and a projection written by hand is what left a workspace member describing its
// name and version and nothing else while its manifest declared dependencies.
//
// The two underscored entries are npm's legacy spellings (`_integrity`,
// `_hasShrinkwrap`): `metaFieldFromPkg` reads them and the key is written without
// the underscore. `hasInstallScript` is not a package.json field at all — npm derives
// it from the scripts — so it is projected below, from the package's own answer
const PKG_META_KEYS = [
    'version', 'dependencies', 'peerDependencies', 'peerDependenciesMeta',
    'optionalDependencies', 'bundleDependencies', 'acceptDependencies', 'funding',
    'engines', 'os', 'cpu', 'libc', '_integrity', 'license', '_hasShrinkwrap',
    'bin', 'deprecated', 'workspaces',
];
// the keys a projection decides, absence included: whatever the previous entry
// carried under one of them is replaced by the manifest's answer
const PROJECTED_KEYS = PKG_META_KEYS
    .map(k => k.replace(/^_/, ''))
    .concat(['name', 'devDependencies', 'hasInstallScript']);

/**
 * @description one package.json field as a lock entry carries it (npm's
 *              `metaFieldFromPkg`): a license object becomes its type, an empty
 *              object is nothing, and a falsy value is nothing
 */
function meta_field(pkgjson, key) {
    const val = pkgjson && pkgjson[key];

    if (!val)
        return null;

    if (key === 'license' && typeof val === 'object' && val.type)
        return val.type;

    if (typeof val !== 'object' || Object.keys(val).length)
        return val;

    return null;
}

/**
 * @description the lock entry a package directory describes: npm's projection of
 *              `pkgMetaKeys`, plus the name where the directory does not already say
 *              it, plus `devDependencies` (npm writes those for a directory a link
 *              points at — a workspace member is a project of its own, and its
 *              devDependencies are installed with the project)
 * @param p the directory, in lockfile terms (`packages/member`, `linkpkg`)
 */
function project_manifest(pkgjson, p) {
    if (!pkgjson)
        return {};

    const meta = {};

    PKG_META_KEYS.forEach(key => {
        const val = meta_field(pkgjson, key);

        if (val !== null)
            meta[key.replace(/^_/, '')] = val;
    });

    // npm writes `name` only where the path does not already imply it: an entry for
    // `packages/alpha` says `@repro/alpha`, one for `packages/alpha` holding `alpha`
    // says nothing at all
    const derived = name_from_folder(p);

    if (pkgjson.name && pkgjson.name !== derived)
        meta.name = pkgjson.name;

    if (pkgjson.devDependencies)
        meta.devDependencies = pkgjson.devDependencies;

    if (meta.bin)
        meta.bin = normalize_bin(meta.bin, pkgjson.name || derived);

    // npm records `bundledDependencies` under the name it reads it from as well
    if (meta.bundleDependencies === undefined && pkgjson.bundledDependencies)
        meta.bundleDependencies = pkgjson.bundledDependencies;

    // `hasInstallScript` is not a package.json field: npm derives it from the scripts
    const scripted = has_install_script(pkgjson);

    if (scripted)
        meta.hasInstallScript = scripted;

    return entry_order(meta);
}

/**
 * @description one entry of the path table as a lockfile entry. `name` is written
 *              only when it differs from the path (an alias), the way npm does it
 */
function lock_entry(entry, p) {
    // a copy a package ships inside its own tarball has no source of its own: npm
    // records the version, `inBundle` and the manifest fields, nothing else
    const bundled = entry.in_bundle === true;

    return entry_order({
        // npm's `metaFromNode`: only where the package is not what its path says
        name: entry.name !== implied_name(p) ? entry.name : undefined,
        version: entry.version,
        resolved: bundled ? undefined : entry.resolved,
        integrity: bundled ? undefined : entry.integrity,
        link: entry.link ? true : undefined,
        dev: entry.dev ? true : undefined,
        optional: entry.optional ? true : undefined,
        devOptional: entry.dev_optional ? true : undefined,
        bundleDependencies: entry.bundle_dependencies,
        inBundle: bundled ? true : undefined,
        dependencies: entry.dependencies,
        peerDependencies: entry.peerDependencies,
        peerDependenciesMeta: entry.peerDependenciesMeta,
        optionalDependencies: entry.optionalDependencies,
        funding: entry.funding,
        engines: entry.engines,
        os: entry.os,
        cpu: entry.cpu,
        libc: entry.libc,
        license: entry.license,
        hasInstallScript: entry.has_install_script ? true : undefined,
        bin: normalize_bin(entry.bin, entry.name),
    });
}

/**
 * @description the entry npm writes for the directory a link points at. It is the
 *              manifest's projection (`project_manifest`), not a stub: npm describes
 *              what that directory is — its dependencies, its bin, its engines — and
 *              a reader of the lockfile (a sync check, npm itself, a graph walk) reads
 *              the member's dependencies from exactly there.
 *
 *              The flags live here and not on the link: npm's `metaFromNode` returns
 *              `{ resolved, link }` for a link without asking about them, while its
 *              `calc-dep-flags` copies `dev`/`optional`/`devOptional` onto the target
 *              node, which is the entry that carries them
 */
function target_entry(entry, previous, p, flags) {
    const fresh = entry.manifest
        ? project_manifest(entry.manifest, p)
        : entry_order({
            name: p && entry.name !== name_from_folder(p) ? entry.name : undefined,
            version: entry.version,
            hasInstallScript: entry.has_install_script ? true : undefined,
        });

    if (flags) {
        if (flags.dev)
            fresh.dev = true;
        if (flags.optional)
            fresh.optional = true;
        if (flags.dev_optional && !flags.dev && !flags.optional)
            fresh.devOptional = true;
    }

    if (!previous || typeof previous !== 'object')
        return entry_order(fresh);

    const merged = {};

    // what npm wrote and the projection does not produce (a field of a newer npm)
    // survives; what the manifest decides is the manifest's answer, absence included
    Object.keys(previous).forEach(k => { merged[k] = previous[k]; });
    PROJECTED_KEYS.concat(['dev', 'optional', 'devOptional']).forEach(k => { delete merged[k]; });

    Object.keys(fresh).forEach(k => { merged[k] = fresh[k]; });

    // an entry whose manifest says nothing is still an entry (npm writes `{}` for a
    // member with no name and no version)
    return entry_order(merged);
}

/**
 * @description the `dependencies` section a v2 lockfile carries next to `packages`
 *              (npm 7/8 wrote both, npm 6 only understands this one)
 */
function legacy_tree(packages) {
    const tree = {};

    Object.keys(packages).forEach(p => {
        if (p.indexOf('node_modules/') !== 0)
            return;

        const entry = packages[p];
        const segments = p.split('/node_modules/');
        let node = tree;

        segments.forEach((segment, i) => {
            const name = segment.replace(/^node_modules\//, '');

            if (i === segments.length - 1) {
                const requires = Object.assign({}, entry.dependencies, entry.optionalDependencies);
                const legacy = {
                    version: entry.link ? 'file:' + entry.resolved : entry.version,
                    resolved: entry.link ? undefined : entry.resolved,
                    integrity: entry.integrity,
                    dev: entry.dev,
                    optional: entry.optional,
                    requires: Object.keys(requires).length ? requires : undefined,
                };

                node[name] = order_keys(legacy, LEGACY_KEY_ORDER);
                return;
            }

            node = node[name] || (node[name] = {});
            node.dependencies = node.dependencies || {};
            node = node.dependencies;
        });
    });

    return tree;
}

/**
 * @description the root entry of a lockfile: what package.json declares
 */
function root_entry(root_pkgjson, previous) {
    // npm projects a fixed list of package.json fields into a lock entry
    // (arborist's `pkgMetaKeys`), `workspaces` among them: a root entry written
    // without it makes npm and this rewrite the file back and forth. The root is
    // the one entry that always carries its name, and its devDependencies are
    // installed with the project
    const entry = project_manifest(root_pkgjson, '');
    const license = root_pkgjson.license && typeof root_pkgjson.license === 'object'
        ? root_pkgjson.license.type
        : root_pkgjson.license;

    entry.name = root_pkgjson.name;
    entry.version = root_pkgjson.version;
    entry.license = license;
    entry.devDependencies = root_pkgjson.devDependencies;

    if (!previous || typeof previous !== 'object')
        return entry_order(entry);

    const merged = {};

    // everything the lockfile carried (a field a newer npm writes, one this does not
    // project) is kept as it was …
    Object.keys(previous).forEach(k => { merged[k] = previous[k]; });

    // … except the fields the manifest decides, absence included: a dependency or an
    // engine dropped from package.json has to leave the entry, or npm and this write
    // the field back and forth
    PROJECTED_KEYS.forEach(k => { delete merged[k]; });

    Object.keys(entry).forEach(k => { merged[k] = entry[k]; });

    return entry_order(merged);
}

/**
 * @description does that package.json run an install script? npm records the answer
 *              as `hasInstallScript`
 */
function has_install_script(pkgjson) {
    const scripts = pkgjson && pkgjson.scripts;

    if (!scripts)
        return undefined;

    return scripts.preinstall || scripts.install || scripts.postinstall ? true : undefined;
}

/**
 * @description the entry of a package the lockfile already described and this run did
 *              not fetch, with the fields the tree produces laid over it. What npm
 *              wrote and the tree does not produce (license, engines, funding,
 *              peerDependenciesMeta…) survives, while everything a package's own
 *              manifest describes comes from the tree — a stale dependency list in the
 *              lockfile is what made a repaired tree fail its own sync check
 */
function merge_entry(previous, entry, p) {
    const fresh = lock_entry(entry, p);
    const merged = {};

    Object.keys(previous).forEach(k => { merged[k] = previous[k]; });

    // the tree decides these, `undefined` included: an installed package with no
    // dependencies declares none
    ['name', 'version', 'link', 'dev', 'optional', 'devOptional', 'bundleDependencies',
        'dependencies',
        'optionalDependencies', 'peerDependencies', 'bin', 'os', 'cpu', 'libc',
        'hasInstallScript'].forEach(k => { delete merged[k]; });

    // the source is the tree's when it fetched the package, and the lockfile's when it
    // found the package on disk (nothing fetched it, so nothing knows better). A copy
    // that came inside a parent's tarball has none, and keeps none
    if (entry.resolved || entry.in_bundle) {
        delete merged.resolved;
        delete merged.integrity;
    }

    if (entry.in_bundle)
        merged.inBundle = true;

    Object.keys(fresh).forEach(k => { merged[k] = fresh[k]; });

    return entry_order(merged);
}

/**
 * @description the lockfile a set of installed packages describes. An entry the
 *              previous lockfile already described (same version, same source) is
 *              reused as it is, so the fields npm writes and this does not produce
 *              (license, engines, funding…) survive a write back
 * @param paths { [path]: entry } — the `to_path_map` shape, plus `local_path` for a
 *              link, `has_install_script`, `dev`, `optional`, `dev_optional`
 * @param opts { previous, root, lockfileVersion, name, version }
 */
function to_lock(paths, root_pkgjson, opts) {
    const o = opts || {};
    const old = o.previous ? to_path_map(o.previous) : {};
    const packages = {};
    const kept = [];

    Object.keys(paths).forEach(p => {
        const entry = paths[p];
        const previous = old[p];

        // a link is the target and the flag, nothing else: npm's `metaFromNode` returns
        // exactly `{ resolved, link }` for one, and an entry that also carries `dev` or
        // `optional` is a file npm rewrites the moment it touches it. The flags still
        // exist — the graph answers them — they are just not written here
        if (entry.link) {
            const target = relative_target(o.root, entry.local_path);
            const previous_ok = previous && previous.link === true && previous.resolved === target;

            packages[p] = previous_ok ? previous.raw_entry : entry_order({
                resolved: target,
                link: true,
            });
            return;
        }

        // a node that this run did not fetch (it was already on disk) carries no
        // `dist`, so its source is whatever the lockfile already recorded: asking
        // for `resolved` to match would rewrite that entry into a stub and drop the
        // integrity npm wrote (835 of 1238 entries in the audit's sample). A node
        // that *was* fetched knows its source, and then it has to agree
        const same_source = !entry.resolved || (previous && previous.resolved === entry.resolved);

        // `link` is normalised to false on the previous side and left undefined by the
        // tree, so comparing them directly never matched — the entry was rebuilt from
        // scratch every time, and everything npm had written in it was dropped
        if (previous && previous.version === entry.version &&
            !!previous.link === !!entry.link && same_source) {
            packages[p] = merge_entry(previous.raw_entry, entry, p);
            kept.push(p);
            return;
        }

        packages[p] = lock_entry(entry, p);
    });

    // the directory a link points at gets an entry of its own, like npm writes it
    Object.keys(paths).forEach(p => {
        const entry = paths[p];
        if (!entry.link || !entry.local_path)
            return;

        const target = relative_target(o.root, entry.local_path);
        packages[target] = target_entry(entry, old[target], target, entry);
    });

    packages[''] = root_entry(root_pkgjson, o.previous ? o.previous.root : null);

    // `""` first, then alphabetically — npm writes the path table that way
    const ordered = {};
    ordered[''] = packages[''];
    Object.keys(packages).sort().forEach(p => {
        if (p !== '')
            ordered[p] = packages[p];
    });

    const version = o.lockfileVersion === 2 ? 2 : 3;
    const lock = {
        name: o.name || root_pkgjson.name,
        version: o.version || root_pkgjson.version,
        lockfileVersion: version,
        requires: true,
        packages: ordered,
    };

    if (version === 2)
        lock.dependencies = legacy_tree(ordered);

    return { lock: order_keys(lock, TOP_KEY_ORDER), kept: kept };
}

/**
 * @description the indentation package.json uses, so the lockfile written next to
 *              it looks the same (npm detects it the same way)
 */
function detect_indent(text) {
    const m = /\n(\s+)"/.exec(String(text));

    if (!m)
        return 2;

    return m[1].indexOf('\t') > -1 ? '\t' : m[1].length;
}

/**
 * @description write a lockfile through a temporary file: a crash must not leave a
 *              half written one behind (npm writes through write-file-atomic for
 *              the same reason)
 */
function write_lockfile(file, lock, indent) {
    const text = stringify_lock(lock, indent) + '\n';
    const tmp = file + '.tmp';
    const rename = fs.renameSync || fs.rename;

    fs.writeFile(tmp, text);
    if (fs.exists(file))
        fs.unlink(file);
    rename(tmp, file);

    return text;
}

/**
 * @description the file npm keeps inside node_modules: the record of what was
 *              installed there and where each package came from
 *              (`node_modules/.package-lock.json`). An install that describes a tree
 *              it finds on disk can only know the version from the directory, and a
 *              version alone is not something a frozen install can fetch — npm has
 *              this file precisely so that question has an answer
 */
function hidden_lock_path(root) {
    return path.join(root, 'node_modules', '.package-lock.json');
}

/**
 * @description what the hidden lockfile says about the tree on disk, as a map from
 *              lockfile path to the fields only the installer that wrote it knows:
 *              `resolved` and `integrity`. It is a cache, never an authority — a
 *              caller matches it against the version that is actually there — so a
 *              file that is stale, unreadable or from another npm simply enriches
 *              nothing
 * @returns { [path]: { resolved, integrity } }
 */
function read_hidden_lock(root) {
    const file = hidden_lock_path(root);
    const sources = {};

    if (!fs.exists(file))
        return sources;

    let data;

    try {
        data = JSON.parse(fs.readTextFile(file).replace(/^\uFEFF/, ''));
    } catch (e) {
        return sources;
    }

    if (!data || typeof data.packages !== 'object' || !data.packages)
        return sources;

    Object.keys(data.packages).forEach(p => {
        if (p === '')
            return;

        const entry = data.packages[p];

        if (!entry || entry.link || entry.inBundle)
            return;

        if (!entry.resolved && !entry.integrity)
            return;

        sources[p] = {
            version: entry.version,
            resolved: entry.resolved,
            integrity: entry.integrity,
        };
    });

    return sources;
}

/**
 * @description the hidden lockfile npm would write for that path table: the entries
 *              of the tree with the fields that describe where each one came from. It
 *              carries no root entry (`""`) and no legacy tree — npm's own does not,
 *              and a root entry there would describe package.json rather than what is
 *              installed
 *
 *              A link gets the entry of the directory it points at as well, projected
 *              from that directory's manifest the way npm describes a member. The path
 *              table of an installed tree only walks node_modules, so without this the
 *              members would be missing from the note that is supposed to describe
 *              them
 * @param packages the `to_path_map` shape, or lockfile entries
 * @param root the project, for a link whose target entry has to be read from the disk
 * @param name, version what to write at the top level
 */
function hidden_lock(name, version, packages, root) {
    const ordered = {};
    const targets = {};

    Object.keys(packages).forEach(p => {
        const entry = packages[p];

        if (!entry || !entry.link)
            return;

        // the directory it points at, as the lockfile spells it: a lock entry already
        // has the target, a tree node has the path on the disk
        const target = entry.resolved || (entry.local_path ? relative_target(root, entry.local_path) : null);

        if (target && packages[target] === undefined)
            targets[target] = entry;
    });

    Object.keys(packages).sort().forEach(p => {
        // npm leaves the root entry out (it describes package.json, not what is
        // installed) and keeps every other path, a member's own directory included
        if (p === '')
            return;

        const entry = packages[p];
        const local = entry.link === true;
        // a link's `resolved` is its target, and npm writes it: it is what an install
        // follows. `integrity` is what it does not have
        const resolved = local
            ? (entry.resolved || (entry.local_path ? relative_target(root, entry.local_path) : undefined))
            : (entry.in_bundle ? undefined : entry.resolved);

        ordered[p] = entry_order({
            name: entry.name !== implied_name(p) ? entry.name : undefined,
            version: entry.version,
            resolved: resolved,
            integrity: local || entry.in_bundle ? undefined : entry.integrity,
            link: entry.link ? true : undefined,
            dev: entry.dev ? true : undefined,
            optional: entry.optional ? true : undefined,
            devOptional: entry.dev_optional ? true : undefined,
            bundleDependencies: entry.bundle_dependencies,
            inBundle: entry.inBundle === true ? true : undefined,
            dependencies: entry.dependencies,
            peerDependencies: entry.peerDependencies,
            peerDependenciesMeta: entry.peerDependenciesMeta,
            optionalDependencies: entry.optionalDependencies,
            funding: entry.funding,
            engines: entry.engines,
            os: entry.os,
            cpu: entry.cpu,
            libc: entry.libc,
            license: entry.license,
            hasInstallScript: entry.has_install_script ? true : undefined,
            bin: entry.bin,
        });
    });

    Object.keys(targets).sort().forEach(p => {
        if (ordered[p])
            return;

        const manifest = targets[p].manifest || read_manifest(root, p);
        const entry = project_manifest(manifest, p);

        // the flags npm's `calc-dep-flags` copies onto the target of a link
        if (targets[p].dev)
            entry.dev = true;
        if (targets[p].optional)
            entry.optional = true;
        if (targets[p].dev_optional && !targets[p].dev && !targets[p].optional)
            entry.devOptional = true;

        ordered[p] = entry_order(entry);
    });

    // npm writes the path table in order: the root first (here: absent), then the rest
    const sorted = {};

    Object.keys(ordered).sort().forEach(p => { sorted[p] = ordered[p]; });

    return order_keys({
        name: name,
        version: version,
        lockfileVersion: 3,
        requires: true,
        packages: sorted,
    }, TOP_KEY_ORDER);
}

/**
 * @description the manifest of a directory in the project, or null
 */
function read_manifest(root, p) {
    if (!root || !p)
        return null;

    const file = path.join(root, p, 'package.json');

    if (!fs.exists(file))
        return null;

    try {
        return JSON.parse(fs.readTextFile(file));
    } catch (e) {
        return null;
    }
}

/**
 * @description write the hidden lockfile. Callers do this last, once the tree is what
 *              they mean to leave behind: it is a claim about the disk
 * @param packages the `to_path_map` shape of what is installed
 */
function write_hidden_lock(root, name, version, packages, indent) {
    const file = hidden_lock_path(root);
    const text = stringify_lock(hidden_lock(name, version, packages, root),
        indent === undefined ? 2 : indent) + '\n';

    fs.mkdir(path.join(root, 'node_modules'), { recursive: true });

    const tmp = file + '.tmp';

    fs.writeFile(tmp, text);
    if (fs.exists(file))
        fs.unlink(file);
    (fs.renameSync || fs.rename)(tmp, file);

    return text;
}

/**
 * @description drop the hidden lockfile. An install that changes node_modules and
 *              does not rewrite it leaves a record that describes another tree
 */
function remove_hidden_lock(root) {
    const file = hidden_lock_path(root);

    if (fs.exists(file))
        fs.unlink(file);
}

// ---------------------- not implemented yet :start ------------------------- //

module.exports = {
    LOCKFILE_NAMES,
    parse_sri,
    best_algorithm,
    verify_integrity,
    integrity_plan,
    integrity_ok,
    apply_registry_replace,
    supported_version,
    read_lockfile,
    name_from_path,
    implied_name,
    to_path_map,
    spec_satisfied,
    lookup_entry,
    graph_flags,
    reachable_paths,
    check_peers,
    check_sync,
    read_npmrc,
    parse_npmrc,
    auth_for_url,
    auth_header_for_url,
    to_lock,
    root_entry,
    project_manifest,
    name_from_folder,
    hidden_lock_path,
    read_hidden_lock,
    hidden_lock,
    write_hidden_lock,
    remove_hidden_lock,
    entry_order,
    stringify_lock,
    relative_target,
    write_lockfile,
    detect_indent,
};
