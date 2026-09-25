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
    const e = {
        path: p,
        name: entry.name || name_from_path(p) || p,
        version: entry.version,
        resolved: entry.resolved,
        integrity: entry.integrity,
        link: entry.link === true,
        dev: entry.dev === true,
        optional: entry.optional === true,
        in_bundle: entry.inBundle === true,
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
        // npm writes an entry for the directory a link points at too; only the
        // node_modules one is an installation target
        target_only: p.indexOf('node_modules/') !== 0,
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

        if (entry.name !== real)
            return { ok: false, reason: `alias points at ${entry.name}` };

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
 * @description find the entry a dependency resolves to, offline. npm looks up the
 *              nearest ancestor from the dependent's final path; the installer does
 *              not have final paths while resolving (hoisting comes later), so the
 *              plan fixes a deterministic approximation: the root entry first, then
 *              the shallowest nested entry that satisfies the spec
 */
function lookup_entry(packages, name, spec, semver, from) {
    const direct = packages['node_modules/' + name];

    if (direct && spec_satisfied(spec, direct, semver, from).ok)
        return direct;

    const suffix = '/node_modules/' + name;
    const nested = Object.keys(packages)
        .filter(p => p.endsWith(suffix))
        .sort((a, b) => a.split('/').length - b.split('/').length);

    for (let i = 0; i < nested.length; i++) {
        const entry = packages[nested[i]];
        if (spec_satisfied(spec, entry, semver, from).ok)
            return entry;
    }

    // the root entry is returned even when it does not satisfy the spec, so the
    // caller can report "Invalid" (what npm says) instead of "Missing"
    return direct || null;
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
            const found = lookup_entry(packages, name, spec, semver, p);

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
const ENTRY_KEY_ORDER = [
    'name', 'version', 'resolved', 'integrity', 'link', 'dev', 'optional', 'devOptional',
    'dependencies', 'peerDependencies', 'peerDependenciesMeta', 'optionalDependencies',
    'funding', 'engines', 'os', 'cpu', 'libc', 'license', 'hasInstallScript', 'bin',
    'inBundle', 'hasShrinkwrap',
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
 * @description the path npm records for a link: relative to the project, `/` only
 */
function relative_target(root, target) {
    const rel = path.relative(root || '.', target).replace(/\\/g, '/');

    return rel || '.';
}

/**
 * @description one entry of the path table as a lockfile entry. `name` is written
 *              only when it differs from the path (an alias), the way npm does it
 */
function lock_entry(entry, p) {
    return order_keys({
        name: entry.name !== name_from_path(p) ? entry.name : undefined,
        version: entry.version,
        resolved: entry.resolved,
        integrity: entry.integrity,
        link: entry.link ? true : undefined,
        dev: entry.dev ? true : undefined,
        optional: entry.optional ? true : undefined,
        devOptional: entry.dev_optional ? true : undefined,
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
    }, ENTRY_KEY_ORDER);
}

/**
 * @description the entry npm writes for the directory a link points at
 */
function target_entry(entry, previous) {
    if (previous && previous.version === entry.version)
        return previous.raw_entry;

    return order_keys({
        name: entry.name,
        version: entry.version,
        hasInstallScript: entry.has_install_script ? true : undefined,
    }, ENTRY_KEY_ORDER);
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
function root_entry(root_pkgjson) {
    return order_keys({
        name: root_pkgjson.name,
        version: root_pkgjson.version,
        license: root_pkgjson.license,
        dependencies: root_pkgjson.dependencies,
        devDependencies: root_pkgjson.devDependencies,
        optionalDependencies: root_pkgjson.optionalDependencies,
        peerDependencies: root_pkgjson.peerDependencies,
    }, ENTRY_KEY_ORDER);
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

        // a link is the target plus the flag (and the dev/optional marks, which
        // apply to a link like to anything else) — npm writes it that way, and the
        // target is what an install follows
        if (entry.link) {
            const target = relative_target(o.root, entry.local_path);
            const previous_ok = previous && previous.link === true && previous.resolved === target &&
                previous.dev === entry.dev && previous.optional === entry.optional;

            packages[p] = previous_ok ? previous.raw_entry : order_keys({
                resolved: target,
                link: true,
                dev: entry.dev ? true : undefined,
                optional: entry.optional ? true : undefined,
                devOptional: entry.dev_optional ? true : undefined,
            }, ENTRY_KEY_ORDER);
            return;
        }

        if (previous && previous.version === entry.version &&
            previous.resolved === entry.resolved && previous.link === entry.link) {
            packages[p] = previous.raw_entry;
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
        packages[target] = target_entry(entry, old[target]);
    });

    packages[''] = root_entry(root_pkgjson);

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
    const text = JSON.stringify(lock, null, indent === undefined ? 2 : indent) + '\n';
    const tmp = file + '.tmp';
    const rename = fs.renameSync || fs.rename;

    fs.writeFile(tmp, text);
    if (fs.exists(file))
        fs.unlink(file);
    rename(tmp, file);

    return text;
}

// ---------------------- not implemented yet :start ------------------------- //

module.exports = {
    LOCKFILE_NAMES,
    parse_sri,
    best_algorithm,
    verify_integrity,
    apply_registry_replace,
    supported_version,
    read_lockfile,
    name_from_path,
    to_path_map,
    spec_satisfied,
    lookup_entry,
    check_sync,
    read_npmrc,
    parse_npmrc,
    auth_for_url,
    auth_header_for_url,
    to_lock,
    root_entry,
    write_lockfile,
    detect_indent,
};
