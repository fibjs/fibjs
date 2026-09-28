const url = require('url');
const util = require('util');
const coroutine = require('coroutine');
const fs = require('fs');
const path = require('path');
const os = require('os');
const ssl = require('ssl');
const http = require('http');
const zlib = require('zlib');
const zip = require('zip');
const crypto = require('crypto');
const child_process = require('child_process');
const semver = require('internal/helpers/semver');
const untar = require('internal/helpers/untar');

const helpers_pkg = require('internal/helpers/package');
const helpers_string = require('internal/helpers/string');
const lockfile = require('internal/helpers/lockfile');
const versioning = require('internal/helpers/versioning');
const win32_cli = require('internal/helpers/win32_cli');
const CST = require('internal/constant');

const DEPENDENCIES = 'dependencies';
const DEVDEPENDENCIES = 'devDependencies';
const OPTDEPENDENCIES = 'optionalDependencies';
const DEP_KEY_TUPLE = [DEPENDENCIES, DEVDEPENDENCIES];
const SEP = path.sep;

const install_log = process.env.FIBJS_SILENT_INSALL ? () => undefined : console.log.bind(console)

http.setGlobalProxyFromEnv();

// ---------------------- UTILS :start ------------------------- //
/**
 * @description read modules from `node_modules` to generate existed modules snapshot
 * @sample 
    {
        name: "module",
        version: "1.0.0",
        dep_vs: { "module-a": "1.0.0" },
        peer_dep_vs: { "peer-module-a": "1.0.0" },
        devDeps: { "dev-module-a": "1.0.0" },
        node_modules: {},
        parent: null,
        // dist info from registry
        dist: {},
        new_module: true,
    }
 */
function read_module(p, parent) {
    const modules = {};

    if (fs.exists(path.join(p, 'node_modules'))) {
        const nm = path.join(p, 'node_modules');

        // a directory under `node_modules` is either a package or a scope that
        // holds packages (`node_modules/@scope/pkg`). The scope directory has no
        // package.json of its own, so it is expanded one level and the package is
        // registered under its full name — the key the resolver, the hoisting and
        // the installer all use. Skipping it made every scoped package (and the
        // whole subtree below it) invisible, which is how a write back lost them:
        // the walk never repaired what it could not see
        function read_entries(dir, prefix) {
            fs.readdir(dir).forEach(n => {
                if (prefix === '' && n.charAt(0) === '@') {
                    const scope = path.join(dir, n);

                    try {
                        if (!fs.stat(scope).isDirectory())
                            return;
                    } catch (e) {
                        return;
                    }

                    read_entries(scope, n + '/');
                    return;
                }

                const name = prefix + n;
                const f = path.join(dir, n, 'package.json');
                if (!fs.exists(f))
                    return;

                const minfo = JSON.parse(fs.readTextFile(f));

                const dep_vs = util.clone(minfo.dependencies || {});
                const dev_dep_vs = util.clone(minfo.devDependencies || {});
                const opt_dep_vs = util.clone(minfo.optionalDependencies || {});

                modules[name] = {
                    // the name it is installed under — its directory, which is what the
                    // tree, the hoisting and the lockfile key on — and the name the
                    // package calls itself, which differs for an npm alias
                    name: name,
                    real_name: minfo.name && minfo.name !== name ? minfo.name : undefined,
                    version: minfo.version,
                    // where it sits, in lockfile terms: the resolver looks a dependency
                    // up along that path, the way npm does
                    lock_path: lock_path_of(path.join(dir, n)),
                    dep_vs: dep_vs,
                    dev_dep_vs: dev_dep_vs,
                    opt_dep_vs: opt_dep_vs,
                    peer_dep_vs: read_peer_dep_vs(minfo),
                    bundle_deps: bundled_names(minfo),
                    bundle_list: bundled_list(minfo),
                    // what the entry a write back produces needs and the directory is
                    // the only place it can come from: the platform it is for, and the
                    // commands it installs. A node this run fetched has them from the
                    // registry, and dropping them here made a kept entry lose `os`/`cpu`
                    // (a lockfile that then installs another platform's package) and its
                    // `bin` links
                    os: minfo.os,
                    cpu: minfo.cpu,
                    libc: minfo.libc,
                    bin: minfo.bin,
                    license: minfo.license && typeof minfo.license === 'object'
                        ? minfo.license.type : minfo.license,
                    engines: minfo.engines,
                    funding: minfo.funding,
                    parent: parent
                };

                // a copy the parent ships lives where the parent's tarball put it: it
                // is read here, and never resolved or fetched — and a copy inside a copy
                // came the same way (npm marks the whole subtree of a tarball)
                if (parent && (parent.in_bundle ||
                    (parent.parent && parent.bundle_deps && parent.bundle_deps[name])))
                    modules[name].in_bundle = true;

                modules[name].node_modules = read_module(path.join(dir, n), modules[name]);
            });
        }

        read_entries(nm, '');
    }

    return modules;
}

/**
 * @description a package directory as a lockfile path (`node_modules/a`,
 *              `packages/member/node_modules/b`). The resolver walks a dependency up
 *              that path, exactly like npm walks the directories of the package that
 *              declares it
 */
function lock_path_of(dir) {
    return path.relative(process.cwd(), dir).replace(/\\/g, '/');
}

/**
 * @description the names a package ships inside its own tarball, which is npm's
 *              `bundleDependencies` (also spelled `bundledDependencies`): `true`
 *              means every declared dependency, the array form names them. npm
 *              installs the copies the tarball carries instead of resolving those
 *              names, and a lockfile records them with `inBundle`
 */
function bundled_names(manifest) {
    const raw = manifest.bundleDependencies !== undefined ? manifest.bundleDependencies
        : manifest.bundledDependencies;

    if (raw === true)
        return util.extend({}, manifest.dependencies, manifest.optionalDependencies);

    const names = {};

    (Array.isArray(raw) ? raw : Object.keys(raw || {})).forEach(n => { names[n] = true; });

    return Object.keys(names).length ? names : undefined;
}

/**
 * @description the same list as it goes into a lockfile: npm writes what the
 *              manifest declared (`true`, or the names), and a file that says
 *              something else makes npm rewrite the entry
 */
function bundled_list(manifest) {
    const raw = manifest.bundleDependencies !== undefined ? manifest.bundleDependencies
        : manifest.bundledDependencies;

    if (raw === true)
        return true;

    const names = Array.isArray(raw) ? raw : Object.keys(raw || {});

    return names.length ? names : undefined;
}

/**
 * @description the peer dependencies that have to be installed along with a package.
 *              npm (>= 7) installs them like regular dependencies, except the ones
 *              marked optional in `peerDependenciesMeta`
 */
function read_peer_dep_vs(pkgjson) {
    const peer_dep_vs = util.clone(pkgjson.peerDependencies || {});
    const peer_meta = pkgjson.peerDependenciesMeta || {};

    for (let k in peer_dep_vs) {
        // an optional peer stays uninstalled, npm does the same
        if (peer_meta[k] && peer_meta[k].optional) {
            delete peer_dep_vs[k];
            continue;
        }

        // a workspace spec is provided by the workspace itself (the members are
        // materialized at the root), and a registry lookup cannot resolve it:
        // `parse_pkg_installname` only knows registry/git/local specs
        if (typeof peer_dep_vs[k] === 'string' && peer_dep_vs[k].indexOf('workspace:') === 0)
            delete peer_dep_vs[k];
    }

    return peer_dep_vs;
}

function http_get(u, { quit_if_error = true, headers = null } = {}) {
    let cnt = 0;

    while (cnt++ < 10)
        try {
            const opts = headers ? { headers: headers } : undefined;

            // `.npmrc` may ask for a private CA or for no verification at all, and
            // those are client options in fibjs: `http.getSync(url, opts)` takes
            // request options, so a client has to be built for them
            const res = ctx.client ? ctx.client.getSync(u, opts) : http.getSync(u, opts);
            if (!res.body)
                throw new Error(`[http_get] get nothing from url ${u}`);

            return res
        } catch (e) {
            console.log(e);
            console.warn(`[http_get] retry ${cnt}: ${u}`);
            coroutine.sleep(100 * cnt);
        }

    console.error("download error.", u);
    if (quit_if_error)
        process.exit(-1);
}

function json_parse_response(http_response) {
    return JSON.parse(http_response.text())
}

/**
 * @description does a node satisfy a spec? A plain range compares versions; an npm
 *              alias (`npm:string-width@^4.2.0`) compares the package the node really
 *              is (`real_name`) and then the range. `semver.satisfies` on the alias
 *              spec itself is always false, which made every alias edge resolve again
 *              and throw the version the lockfile pinned away
 */
function node_satisfies(node, spec) {
    if (!node || !node.version || typeof spec !== 'string')
        return false;

    if (spec.indexOf('npm:') === 0) {
        const target = spec.slice(4);
        const at = target.lastIndexOf('@');
        const real = at > 0 ? target.slice(0, at) : target;
        const range = at > 0 ? target.slice(at + 1) : '';

        if ((node.real_name || node.name) !== real)
            return false;

        return !range || semver.satisfies(node.version, range);
    }

    return semver.satisfies(node.version, spec);
}

function find_version(m, v, parent) {
    while (parent !== undefined) {
        const info = parent.node_modules[m];
        if (info !== undefined)
            return node_satisfies(info, v);

        parent = parent.parent;
    }
}

/**
 * @description the node this level or one of its ancestors already holds for a name.
 *              A workspace member or a `file:` package is materialized at the root,
 *              so a sibling that declares it finds it up there — not in its own
 *              `node_modules`, which is where the resolver used to look
 */
function find_provided_node(m, level_info) {
    while (level_info !== undefined) {
        const info = level_info.node_modules[m];
        if (info !== undefined)
            return info;

        level_info = level_info.parent;
    }

    return undefined;
}

function normalize_registry_origin(registry) {
    const urlObj = url.parse(registry)

    const protocol = urlObj.protocol || 'https:'
    // `host` keeps the port: a private registry usually lives on one
    const host = urlObj.host || 'registry.npmjs.org'

    return `${protocol}//${host}/`
}

/**
 * @description render the requiring chain (root > ... > parent) of a module for diagnostics
 */
function describe_require_chain(level_info) {
    const chain = [];
    let cur = level_info;

    while (cur) {
        chain.unshift(`${cur.name || '(unnamed)'}@${cur.version || '?'}`);
        cur = cur.parent;
    }

    return chain.join(' > ') || '(root)';
}

/**
 * @description list versions in a compact, readable form
 */
function summarize_versions(vers) {
    const sorted = vers.slice().sort(semver.rcompare);
    const shown = sorted.slice(0, 10);

    return sorted.length > shown.length
        ? `${shown.join(', ')} ... (${sorted.length} versions in total, latest ${sorted[0]})`
        : shown.join(', ');
}

/**
 * @description shorten a raw http body for error messages
 */
function response_snippet(raw) {
    const s = String(raw).replace(/\s+/g, ' ').trim();

    return s.length > 200 ? s.slice(0, 200) + ' ...' : s;
}

/**
 * @description look for local directories that seem to be meant to provide `pkg_name`,
 *              e.g. a workspace member without package.json, or one with a different name
 */
function find_local_pkg_candidates(pkg_name) {
    const found = [];
    const unscoped = pkg_name.startsWith('@') ? pkg_name.slice(pkg_name.indexOf('/') + 1) : pkg_name;
    const workspaces = rootsnap && rootsnap.workspaces;

    if (!Array.isArray(workspaces)) return found;

    workspaces.forEach(pattern => {
        const dir = path.join(process.cwd(), pattern.replace(/\*.*$/, ''));
        if (!fs.exists(dir)) return;

        fs.readdir(dir).forEach(entry => {
            const sub = path.join(dir, entry);
            const pkgjson_path = path.join(sub, 'package.json');
            const rel = path.relative(process.cwd(), sub);

            if (!fs.exists(pkgjson_path)) {
                if (entry === unscoped)
                    found.push(`${rel} (directory exists, but has no package.json)`);
                return;
            }

            try {
                const info = JSON.parse(fs.readTextFile(pkgjson_path));

                if (info.name === pkg_name || entry === unscoped)
                    found.push(`${rel} (package name: ${info.name || '(unnamed)'})`);
            } catch (e) {
                if (entry === unscoped)
                    found.push(`${rel} (invalid package.json: ${e.message})`);
            }
        });
    });

    return found;
}

/**
 * @description build an actionable error for a failed registry metadata lookup
 */
function registry_lookup_error(m, v, parent, registry_url, reason) {
    const lines = [];

    lines.push(`[install] cannot fetch metadata for '${m}@${v}'`);
    lines.push(`  required by : ${describe_require_chain(parent)} > ${m}@${v}`);
    lines.push(`  registry    : ${registry_url}`);
    lines.push(`  reason      : ${reason}`);

    const candidates = find_local_pkg_candidates(m);
    const named = candidates.filter(c => c.indexOf(`(package name: ${m})`) > 0);

    if (named.length > 0)
        // the name is right and the package is here: the lookup should not have gone to
        // a registry at all (a workspace member is installed as a link)
        lines.push(`  hint        : ${named.join(', ')} provides '${m}' — a local workspace package is linked, not fetched`);
    else if (candidates.length > 0)
        lines.push(`  hint        : ${candidates.join(', ')} — a local workspace package is only recognised when its package.json "name" is exactly '${m}'`);
    else if (m.startsWith('@'))
        lines.push(`  hint        : scoped package, make sure it is published (and not unpublished), or point at your private registry via .npmrc / package.json "registry"`);
    else
        lines.push(`  hint        : check the package name, or configure a mirror registry via .npmrc / package.json "registry"`);

    return new Error(lines.join('\n'));
}

/**
 * @description aggregate the resolution failures of a dependency level into a single error
 */
function format_dep_failures(level_info, failures) {
    const lines = [];

    lines.push(`[install] failed to resolve ${failures.length} ${failures.length === 1 ? 'dependency' : 'dependencies'} of ${level_info.name}@${level_info.version}:`);

    failures.forEach(({ name, spec, dep_field, error }) => {
        lines.push('');
        lines.push(`  - ${name}@${spec || '*'} (${dep_field})`);
        String(error && error.message || error).split('\n').forEach(line => lines.push('    ' + line));
    });

    return lines.join('\n');
}
// ---------------------- UTILS :end ------------------------- //

// ---------------------- WORKSPACES UTILS :start ------------------------- //
/**
 * @description find all workspace packages based on workspaces config
 */
function find_workspace_packages(root_path, workspaces_config) {
    if (!workspaces_config || !Array.isArray(workspaces_config)) {
        return [];
    }

    const workspace_packages = [];

    workspaces_config.forEach(pattern => {
        const glob_pattern = path.join(root_path, pattern, 'package.json');

        try {
            const matched_files = fs.glob(glob_pattern);

            matched_files.forEach(pkg_json_path => {
                try {
                    const pkg_dir = path.dirname(pkg_json_path);
                    const pkg_info = JSON.parse(fs.readTextFile(pkg_json_path));

                    if (pkg_info.name) {
                        workspace_packages.push({
                            name: pkg_info.name,
                            version: pkg_info.version || '1.0.0',
                            path: pkg_dir,
                            relative_path: path.relative(root_path, pkg_dir),
                            package_json: pkg_info
                        });
                    }
                } catch (e) {
                    console.warn(`[workspaces] Failed to read package.json at ${pkg_json_path}:`, e.message);
                }
            });
        } catch (e) {
            console.warn(`[workspaces] Failed to glob pattern ${pattern}:`, e.message);
        }
    });

    return workspace_packages;
}

/**
 * @description copy a directory tree; links inside the package are not followed
 */
function copy_dir(src_path, dest_path) {
    fs.mkdir(dest_path, { recursive: true });

    fs.readdir(src_path).forEach(name => {
        const src = path.join(src_path, name);
        const dest = path.join(dest_path, name);
        const stats = fs.lstat(src);

        // the source package does not own the target of a link, it is not followed
        if (stats.isSymbolicLink())
            return;

        if (stats.isDirectory())
            copy_dir(src, dest);
        else
            fs.copyFile(src, dest);
    });
}

/**
 * @description install a local package directory into node_modules, as a link to
 *              the source directory when the filesystem can host one. Some
 *              filesystems cannot: a mapped network drive, a VM shared folder or
 *              a FAT/exFAT volume on Windows refuses reparse points. There the
 *              package is copied instead, which installs a snapshot of the source
 *              directory rather than a live link to it.
 */
function link_or_copy_dir(src_path, dest_path) {
    var link_error = null;

    try {
        // create symlink or junction based on platform
        if (process.platform === 'win32') {
            fs.symlink(src_path, dest_path, 'junction');
        } else {
            fs.symlink(src_path, dest_path);
        }

        install_log('link:', src_path, '→', dest_path);
        return;
    } catch (e) {
        link_error = e;
    }

    console.warn(`[install] cannot link ${dest_path} to ${src_path}: ${link_error.message}\n` +
        '  hint: this filesystem cannot host a directory link - a mapped network drive, a VM\n' +
        '  shared folder or a FAT/exFAT volume on Windows refuses reparse points\n' +
        '  installing a copy instead: it is a snapshot of the source directory, not a live link');

    copy_dir(src_path, dest_path);
    install_log('copy:', src_path, '→', dest_path);
}

// packages already installed into node_modules by this run, see create_local_symlinks
const installed_paths = {};

/**
 * @description create symlinks for local/workspace packages
 * @param {string} root_path - root project path
 * @param {Array<{name: string, srcPath: string}>} packages - packages to symlink
 */
function create_local_symlinks(root_path, packages) {
    const node_modules_path = path.join(root_path, 'node_modules');

    // ensure node_modules directory exists
    if (!fs.exists(node_modules_path)) {
        fs.mkdir(node_modules_path, { recursive: true });
    }

    packages.forEach(pkg => {
        const link_path = path.join(node_modules_path, pkg.name);
        const target_path = pkg.srcPath;

        try {
            // a package is reachable from more than one place in the snapshot (the
            // requested one and the dependency it resolves to), it is installed once
            if (installed_paths[link_path]) {
                return;
            }

            // For scoped packages, ensure parent directory exists
            const link_dir = path.dirname(link_path);
            if (!fs.exists(link_dir)) {
                fs.mkdir(link_dir, { recursive: true });
            }

            // remove existing symlink or directory
            if (fs.exists(link_path)) {
                const stats = fs.lstat(link_path);
                if (stats.isSymbolicLink()) {
                    fs.unlink(link_path);
                } else if (stats.isDirectory()) {
                    // don't remove if it's a real directory with installed packages
                    console.warn(`[install] Skipping ${pkg.name}: directory already exists` +
                        ` (remove it to install ${target_path} again)`);
                    return;
                }
            }

            link_or_copy_dir(target_path, link_path);
            installed_paths[link_path] = true;

            // a local package's binaries are linked like any other package's: the
            // lockfile does not describe them for a link, its package.json does
            try {
                const local_pj = path.join(target_path, 'package.json');

                if (fs.exists(local_pj))
                    link_bins(pkg.name, JSON.parse(fs.readTextFile(local_pj)).bin, [node_modules_path]);
            } catch (e) {
                console.log(e);
            }
        } catch (e) {
            console.warn(`[install] Failed to install ${pkg.name}:`, e.stack || e.message);
        }
    });
}

/**
 * @description create symlinks for workspace packages (delegates to create_local_symlinks)
 */
function create_workspace_symlinks(root_path, workspace_packages) {
    create_local_symlinks(root_path, workspace_packages.map(pkg => ({
        name: pkg.name,
        srcPath: path.resolve(root_path, pkg.relative_path)
    })));
}

/**
 * @description materialize the workspace members at the root. It happens once the
 *              lockfile has been accepted, because it writes to node_modules: a check
 *              that fails has to leave the disk alone (`npm ci` validates before it
 *              removes anything, and the same promise is in the plan §4.5)
 */
function link_workspace_packages() {
    if (!rootsnap.workspaces)
        return;

    const workspace_packages = find_workspace_packages(process.cwd(), rootsnap.workspaces);

    if (workspace_packages.length > 0)
        create_workspace_symlinks(process.cwd(), workspace_packages);
}

/**
 * @description add workspace packages to module snapshot
 */
function add_workspace_packages_to_snapshot(rootsnap, workspace_packages) {
    workspace_packages.forEach(pkg => {
        // add workspace package to node_modules snapshot
        rootsnap.node_modules[pkg.name] = {
            name: pkg.name,
            version: pkg.version,
            dep_vs: util.extend({}, pkg.package_json.dependencies),
            dev_dep_vs: util.extend({}, pkg.package_json.devDependencies),
            opt_dep_vs: util.extend({}, pkg.package_json.optionalDependencies),
            peer_dep_vs: read_peer_dep_vs(pkg.package_json),
            parent: rootsnap,
            workspace_package: true,
            workspace_path: pkg.path,
            new_module: true  // Mark as new module so dependencies will be processed
        };

        // read nested node_modules if any
        rootsnap.node_modules[pkg.name].node_modules = read_module(pkg.path, rootsnap.node_modules[pkg.name]);
    });
}
// ---------------------- WORKSPACES UTILS :end ------------------------- //

// ---------------------- LIFECYCLE SCRIPTS :start ------------------------- //

/**
 * @description flatten an object to npm_package_* env vars (e.g., {scripts:{install:"x"}} → npm_package_scripts_install)
 */
function flatten_package_env(prefix, obj) {
    var env = {};
    for (var k in obj) {
        var val = obj[k];
        var key = prefix + '_' + k.replace(/[^a-zA-Z0-9_]/g, '_');
        if (typeof val === 'object' && val !== null && !Array.isArray(val))
            env = util.extend(env, flatten_package_env(key, val));
        else
            env[key] = String(val);
    }
    return env;
}

/**
 * @description set up npm-compatible environment variables for a lifecycle script
 */
function setup_script_env(pkgjson, pkg_path, event) {
    var env = util.extend({}, process.env);

    // npm_package_* variables
    var pkg_env = flatten_package_env('npm_package', pkgjson);
    util.extend(env, pkg_env);

    // npm_lifecycle_* variables
    env.npm_lifecycle_event = event;
    env.npm_lifecycle_script = pkgjson.scripts && pkgjson.scripts[event] || '';

    // npm_config_registry
    if (!env.npm_config_registry)
        env.npm_config_registry = 'https://registry.npmjs.org/';

    // npm puts `node_modules/.bin` and the directory of the runtime that runs the
    // scripts in front of PATH, so that a script can call the very runtime which
    // installs it (`fibjs` in a script works without a global installation).
    // `env` is a plain copy of process.env here, and Windows spells the variable
    // `Path`: update the spelling which is actually present, a second `PATH` next
    // to it would leave the child with an unpredictable search path.
    var path_key = Object.keys(env).find(k => k.toUpperCase() === 'PATH') || 'PATH';
    var path_sep = process.platform === 'win32' ? ';' : ':';
    var bin_path = path.join(pkg_path, 'node_modules', '.bin');
    var exec_path = path.dirname(process.execPath);

    env[path_key] = bin_path + path_sep + exec_path + path_sep + (env[path_key] || '');

    return env;
}

/**
 * @description run lifecycle scripts for a single module
 * @param skip events not to run (the root's `preinstall` has already run before the
 *             install started, see `run_root_preinstall()`)
 */
function run_module_scripts(pkg_path, pkg_info, is_root, skip) {
    var pkgjson;
    try {
        pkgjson = JSON.parse(fs.readTextFile(path.join(pkg_path, 'package.json')));
    } catch (e) {
        return;
    }

    var scripts = pkgjson.scripts || {};

    // build lifecycle event list
    var events;
    if (is_root)
        events = ['preinstall', 'install', 'postinstall', 'prepublish', 'preprepare', 'prepare', 'postprepare'];
    else
        events = ['install', 'postinstall'];

    if (skip)
        events = events.filter(e => skip.indexOf(e) < 0);

    for (var i = 0; i < events.length; i++) {
        var event = events[i];
        var script = scripts[event];
        if (!script) continue;

        install_log('  ' + event + ' ' + (pkgjson.name || '') + '@' + (pkgjson.version || ''));

        var env = setup_script_env(pkgjson, pkg_path, event);

        try {
            var result = child_process.spawnSync(process.execPath, [event], {
                cwd: pkg_path,
                env: env,
                stdio: 'pipe'
            });

            // what a script printed is the operator's business to see, whether it
            // succeeded or not: a script that fails without a word (or one that
            // quietly installs nothing) is otherwise invisible
            if (result.stdout) install_log('  stdout:', result.stdout.toString().trim());
            if (result.stderr) install_log('  stderr:', result.stderr.toString().trim());

            if (result.status !== 0) {
                var msg = '[lifecycle] ' + pkgjson.name + ': ' + event + ' exited with code ' + result.status;
                if (process.env.FIBJS_STRICT_SCRIPTS)
                    throw new Error(msg);
                else
                    console.warn(msg);
            }
        } catch (e) {
            if (process.env.FIBJS_STRICT_SCRIPTS)
                throw e;
            else
                console.warn('[lifecycle] ' + pkgjson.name + ': ' + event + ' error:', e.message);
        }
    }

    // binding.gyp default check
    if (!scripts.install && !scripts.preinstall) {
        if (fs.exists(path.join(pkg_path, 'binding.gyp'))) {
            install_log('  [warn] ' + (pkgjson.name || '') + ': binding.gyp found but no install script; node-gyp rebuild not yet supported');
        }
    }
}

/**
 * @description the root's `preinstall` runs *before* anything is installed, and a
 *              failure stops the install right there — npm does the same for both
 *              `install` and `ci` (a script that prepares the tree must not run
 *              after the tree has been built)
 */
function run_root_preinstall() {
    const pj = path.join(process.cwd(), 'package.json');

    if (!fs.exists(pj))
        return;

    let pkgjson;
    try {
        pkgjson = JSON.parse(fs.readTextFile(pj));
    } catch (e) {
        return;
    }

    if (!pkgjson.scripts || !pkgjson.scripts.preinstall)
        return;

    install_log('  preinstall ' + (pkgjson.name || '') + '@' + (pkgjson.version || ''));

    const res = child_process.spawnSync(process.execPath, ['preinstall'], {
        cwd: process.cwd(),
        env: setup_script_env(pkgjson, process.cwd(), 'preinstall'),
        stdio: 'pipe',
    });

    if (res.status !== 0) {
        console.error('[lifecycle] ' + (pkgjson.name || '') + ': preinstall exited with code ' + res.status);
        if (res.stdout) install_log('  stdout:', String(res.stdout).trim());
        if (res.stderr) install_log('  stderr:', String(res.stderr).trim());
        console.error('[install] nothing was installed');
        process.exit(1);
    }
}

/**
 * @description recursively run lifecycle scripts in topological order (children first)
 */
function run_lifecycle_scripts(level_info, base_path, is_root) {
    // process children first (deep-first)
    for (var k in level_info.node_modules) {
        var child = level_info.node_modules[k];
        var child_path = path.join(base_path, 'node_modules', k);

        // a dependency `omit` leaves out is not on disk: there is nothing to run
        // (the lockfile describes it, the install does not have it)
        if (omitted_by(child))
            continue;

        run_lifecycle_scripts(child, child_path, false);
    }

    // process current module
    if (level_info.new_module)
        run_module_scripts(base_path, level_info, is_root, is_root ? ['preinstall'] : null);
}

// ---------------------- LIFECYCLE SCRIPTS :end ------------------------- //

const pkg_registrytype_module_infos = {};
const pkg_githubtype_module_infos = {};
/**
 * @description fetch the snapshot of a dependency, from the registry/git or from disk
 * @param base_dir - directory a relative local path is resolved against, see walkthrough_deps
 */
function fetch_leveled_module_info(m, v, parent, base_dir) {
    const pkg_install_typeinfo = helpers_pkg.parse_pkg_installname(m, v, base_dir);

    switch (pkg_install_typeinfo.type) {
        case 'registry':
            let info = pkg_registrytype_module_infos[m];
            if (info === undefined) {
                const registry_url = `${registry_for_package(m)}${encodeURIComponent(pkg_install_typeinfo.registry_pkg_path)}`
                install_log('fetch metadata:', m, "=>", registry_url);

                const res = http_get(registry_url, {
                    quit_if_error: false,
                    headers: auth_headers_for(registry_url),
                });

                if (!res)
                    throw registry_lookup_error(m, v, parent, registry_url, 'no response, network unreachable or proxy rejected the request');

                const raw = res.text();

                if (res.statusCode !== 200)
                    throw registry_lookup_error(m, v, parent, registry_url, `HTTP ${res.statusCode} ${response_snippet(raw)}`);

                try {
                    info = JSON.parse(raw);
                } catch (e) {
                    throw registry_lookup_error(m, v, parent, registry_url, `response is not valid JSON (${response_snippet(raw)})`);
                }

                if (info.error)
                    throw registry_lookup_error(m, v, parent, registry_url, info.error);

                if (!info.versions)
                    throw registry_lookup_error(m, v, parent, registry_url, `response has no "versions" field (${response_snippet(raw)})`);

                pkg_registrytype_module_infos[m] = info;
            }

            /* registry: match version :start */
            const all_vers = Object.keys(info.versions);
            let filtered_vers = [];

            switch (pkg_install_typeinfo.registry_semver) {
                case 'latest':
                case '*':
                    filtered_vers = all_vers.sort(semver.rcompare);
                    break
                default:
                    all_vers.forEach(ver => semver.satisfies(ver, pkg_install_typeinfo.registry_semver) ? filtered_vers.push(ver) : undefined)

                    filtered_vers.sort(semver.rcompare);
                    break
            }

            const matched_ver = filtered_vers[0];

            if (!matched_ver)
                throw new Error(
                    `[install] no published version of '${info.name || m}' satisfies '${pkg_install_typeinfo.registry_semver}'\n` +
                    `  required by : ${describe_require_chain(parent)} > ${m}@${pkg_install_typeinfo.registry_semver}\n` +
                    `  available   : ${summarize_versions(all_vers)}`
                )
            /* registry: match version :end */

            const minfo = info.versions[matched_ver];

            const dep_vs = util.clone(minfo.dependencies || {});
            const dev_dep_vs = util.clone(minfo.devDependencies || {});
            const opt_dep_vs = util.clone(minfo.optionalDependencies || {});
            const peer_dep_vs = read_peer_dep_vs(minfo);
            const bundle_deps = bundled_names(minfo);
            const bundle_list = bundled_list(minfo);

            var binary;
            if (minfo.binary) {
                var opt;
                try {
                    opt = versioning.evaluate(minfo, {
                        target_platform: process.versions.musl ? "alpine" : process.platform,
                        target_libc: process.platform === 'linux' ? process.versions.musl ? "musl" : "glibc" : undefined,
                        module_root: '/'
                    }, 3);

                    binary = {
                        module: opt.module,
                        module_path: minfo.binary.module_path,
                        hosted_tarball: opt.hosted_tarball
                    }
                } catch (e) {
                    console.log("node-pre-gyp", e);
                    // process.exit();
                }
            }

            return {
                // the name it is installed under (the key the dependency was declared
                // with) and, for an `npm:` alias, the package that name points at
                name: m,
                real_name: minfo.name && minfo.name !== m ? minfo.name : undefined,
                version: minfo.version,
                bin: minfo.bin,
                binary: binary,
                dep_vs: dep_vs,
                dev_dep_vs: dev_dep_vs,
                opt_dep_vs: opt_dep_vs,
                peer_dep_vs: peer_dep_vs,
                bundle_deps: bundle_deps,
                bundle_list: bundle_list,
                // what npm projects from the manifest of the package itself
                license: minfo.license && typeof minfo.license === 'object'
                    ? minfo.license.type : minfo.license,
                engines: minfo.engines,
                funding: minfo.funding,
                os: minfo.os,
                cpu: minfo.cpu,
                libc: minfo.libc,
                node_modules: {},
                parent: parent,
                dist: minfo.dist,
                pkg_install_typeinfo: pkg_install_typeinfo,
                new_module: true
            }
        case 'git':
            const git_origin = pkg_install_typeinfo.git_origin = pkg_install_typeinfo.git_origin || rootsnap.git_origin || CST.DEFAULT_GIT_ORIGIN;
            let pkgjson_info = pkg_githubtype_module_infos[m];

            if (pkgjson_info === undefined) {
                const git_pkgjson_uri = helpers_pkg.get_git_raw_url('/package.json', pkg_install_typeinfo, { git_origin });
                const git_r = http_get(git_pkgjson_uri, { quit_if_error: false })

                if (git_r.statusCode !== 200)
                    throw new Error(`Could not install from "${ctx.new_pkgname || git_pkgjson_uri}" as it does not contain a package.json file.`)

                try {
                    install_log('fetch package.json:', m, "=>", git_pkgjson_uri);
                    pkg_githubtype_module_infos[m] = pkgjson_info = json_parse_response(git_r);
                } catch (error) {
                    console.warn(`error occured when try to fetch package.json from '${git_pkgjson_uri}' for '${m}'`)
                    throw new Error(`[package/${m}] error detail: \n${error.message}\n${error.stack}`)
                }

            }

            var binary;
            if (pkgjson_info.binary)
                // `module_root: '.'` keeps `module_path` relative: the installer joins
                // it onto the package directory, and an absolute one (which is what
                // node-pre-gyp would resolve against the current directory) would end
                // up nested inside the package instead
                binary = versioning.evaluate(pkgjson_info, { root: '/', module_root: '.' }, 3);

            return {
                name: pkgjson_info.name,
                version: pkgjson_info.version,
                bin: pkgjson_info.bin,
                binary: binary,
                dep_vs: util.extend({}, pkgjson_info.dependencies),
                dev_dep_vs: util.extend({}, pkgjson_info.devDependencies),
                opt_dep_vs: util.extend({}, pkgjson_info.optionalDependencies),
                peer_dep_vs: read_peer_dep_vs(pkgjson_info),
                node_modules: {},
                parent: parent,
                dist: null,
                pkg_install_typeinfo,
                new_module: true
            }
        case 'local':
            const local_pkg_path = path.join(pkg_install_typeinfo.local_path, 'package.json');
            if (!fs.exists(local_pkg_path))
                throw new Error(`[local] package.json not found at ${pkg_install_typeinfo.local_path}`);

            const local_pkg_info = JSON.parse(fs.readTextFile(local_pkg_path));

            return {
                name: local_pkg_info.name || m,
                version: local_pkg_info.version || '0.0.0',
                bin: local_pkg_info.bin,
                binary: undefined,
                dep_vs: util.extend({}, local_pkg_info.dependencies),
                dev_dep_vs: util.extend({}, local_pkg_info.devDependencies),
                opt_dep_vs: util.extend({}, local_pkg_info.optionalDependencies),
                peer_dep_vs: read_peer_dep_vs(local_pkg_info),
                node_modules: {},
                parent: parent,
                dist: null,
                local_package: true,
                local_path: pkg_install_typeinfo.local_path,
                pkg_install_typeinfo,
                new_module: true
            };
        default:
            throw new Error(`unsupported package type '${pkg_install_typeinfo.type}' !`)
    }
}

function get_root_snapshot() {
    const pwd = process.cwd();
    let root_is_new = true;

    let pkgjson = {};
    try {
        const pkgjson_path = path.join(pwd, 'package.json');
        pkgjson = JSON.parse(
            fs.readTextFile(pkgjson_path)
        );
        root_is_new = false;
    } catch (e) {
        // package.json doesn't exist, create default
    }
    pkgjson.name = pkgjson.name || (path.basename(pwd, path.extname(pwd))).toLowerCase();
    pkgjson.version = pkgjson.version || '1.0.0';

    const dep_vs = util.extend({}, pkgjson.dependencies);
    const dev_dep_vs = util.extend({}, pkgjson.devDependencies);
    const opt_dep_vs = util.extend({}, pkgjson.optionalDependencies);
    const peer_dep_vs = read_peer_dep_vs(pkgjson);

    const registry = normalize_registry_origin(pkgjson.registry || 'https://registry.npmjs.org/');

    const m = {
        name: pkgjson.name,
        version: pkgjson.version,
        dep_vs: dep_vs,
        dev_dep_vs: dev_dep_vs,
        opt_dep_vs: opt_dep_vs,
        peer_dep_vs: peer_dep_vs,
        new_module: true,
        registry: registry,
        root_is_new: root_is_new,
        pkgjson: pkgjson,
        workspaces: pkgjson.workspaces
    };

    m.node_modules = read_module(pwd, m);

    // handle workspaces
    if (m.workspaces) {
        const workspace_packages = find_workspace_packages(pwd, m.workspaces);

        if (workspace_packages.length > 0) {
            // the members are part of the snapshot. The links that make them
            // resolvable are created by link_workspace_packages(), once the lockfile
            // has been accepted: creating them here wrote to node_modules before the
            // frozen check had a chance to refuse it
            add_workspace_packages_to_snapshot(m, workspace_packages);
        }
    }

    return m;
}

/**
 * @description check if a package's os/cpu constraints match the current platform
 */
function check_platform_match(pkg_info) {
    var i;
    var matched;

    // check os
    if (pkg_info.os && pkg_info.os.length > 0) {
        matched = false;
        for (i = 0; i < pkg_info.os.length; i++) {
            var o = pkg_info.os[i];
            if (o[0] === '!') {
                if (o.slice(1) === process.platform) return false;
                continue;
            }
            if (o === process.platform) matched = true;
        }
        if (!matched) return false;
    }

    // check cpu
    if (pkg_info.cpu && pkg_info.cpu.length > 0) {
        matched = false;
        for (i = 0; i < pkg_info.cpu.length; i++) {
            if (pkg_info.cpu[i] === process.arch) { matched = true; break; }
        }
        if (!matched) return false;
    }

    // check libc (musl)
    if (pkg_info.libc && pkg_info.libc.length > 0) {
        var is_musl = !!process.versions.musl;
        matched = false;
        for (i = 0; i < pkg_info.libc.length; i++) {
            var l = pkg_info.libc[i];
            if (l[0] === '!') {
                if (l.slice(1) === 'musl' && is_musl) return false;
                continue;
            }
            if ((l === 'musl' && is_musl) || (l === 'glibc' && !is_musl))
                matched = true;
            else if (l === process.versions.modules ? 'glibc' : 'unknown')
                matched = true;
        }
        if (!matched) return false;
    }

    return true;
}

const name_maps_installation2pkg = {}

/**
 * @description the node the lockfile already describes for a dependency, so that
 *              resolving keeps the version it pins instead of taking the newest the
 *              range allows. npm starts from the tree its lockfile records and only
 *              resolves what the change touches; without this, a project whose
 *              node_modules is not there (a fresh clone, a CI checkout) upgrades the
 *              whole tree the moment package.json gains one dependency. `--update`
 *              is how the newest the ranges allow is asked for instead
 */
function locked_module_info(name, spec, level_info) {
    if (!ctx.lock_packages)
        return undefined;

    // the node the lockfile records for this dependency. `nested` keeps the plan's
    // deterministic approximation (§4.2): resolving happens before hoisting, so a
    // node has no final path yet; the sync check — which does know the paths — asks
    // for the exact geometry instead
    const entry = lockfile.lookup_entry(ctx.lock_packages, name, spec, semver,
        level_info && level_info.lock_path, { nested: true });

    // only a registry entry can be used as it stands: a link is read from the
    // directory it points at, and a git entry is fetched by the resolve path (with
    // the sha the lockfile pins)
    if (!entry || entry.link || entry.target_only || is_git_entry(entry))
        return undefined;

    if (!entry.version || !entry.resolved)
        return undefined;

    if (!lockfile.spec_satisfied(spec, entry, semver).ok)
        return undefined;

    install_log('lockfile:', name, '@', entry.version);

    const tarball = lockfile.apply_registry_replace(entry.resolved, ctx.registry);

    return {
        // the name it is installed under — for an npm alias that is the alias — and
        // the package it really is
        name: name,
        real_name: entry.name && entry.name !== name ? entry.name : undefined,
        lock_path: entry.path,
        version: entry.version,
        // a package with an install script may ship a prebuilt addon. The lockfile has
        // no field for it (npm does not model it), so it is evaluated from the package
        // once it is unpacked instead of asking the registry again just for that
        binary_pending: entry.has_install_script === true,
        bin: entry.bin,
        dep_vs: util.clone(entry.dependencies || {}),
        // a lockfile records no devDependencies below the root: they are never
        // installed for a transitive package, and the walk only asks the root
        dev_dep_vs: {},
        opt_dep_vs: util.clone(entry.optionalDependencies || {}),
        peer_dep_vs: util.clone(entry.peerDependencies || {}),
        os: entry.os,
        cpu: entry.cpu,
        libc: entry.libc,
        bundle_deps: bundled_names(entry.raw_entry || entry),
        bundle_list: bundled_list(entry.raw_entry || entry),
        node_modules: {},
        parent: level_info,
        dist: {
            tarball: tarball,
            integrity: entry.integrity,
            shasum: null,
            headers: auth_headers_for(tarball),
        },
        pkg_install_typeinfo: { type: 'registry' },
        from_lock: true,
        new_module: true,
    };
}

/**
 * @description walk throught to generate dep_vs/dev_dep_vs information recursively
 * @param base_dir - directory of `level_info`, relative local dependencies are
 *                   resolved against it (`file:../x` refers to the package itself,
 *                   not to the directory the installation was started from)
 */
function walkthrough_deps(level_info, need_dev_deps = false, base_dir = process.cwd()) {
    // every level is walked, not only the nodes this run created: a package that is
    // already on disk still declares dependencies, and one of them may be missing (an
    // interrupted install, a hand pruned tree, a lockfile written from an incomplete
    // one). Walking the new nodes alone left such a hole alone and then wrote a
    // lockfile that contradicted itself, while `npm install` repairs the tree

    // the specs this level declares, before they are replaced below by the versions
    // that were chosen for them. The tree keeps the latter — it is what `--save`
    // writes into package.json — while a lockfile records the former
    level_info.declared = {
        dep_vs: util.clone(level_info.dep_vs || {}),
        opt_dep_vs: util.clone(level_info.opt_dep_vs || {}),
        peer_dep_vs: util.clone(level_info.peer_dep_vs || {}),
    };

    // `coroutine.parallel` swallows the exceptions raised inside its fibers and only
    // reports '[20020] Internal error', so failures are collected here and rethrown
    // once the whole dependency level has been walked through.
    const dep_failures = [];

    ;[
        ['dep_vs', 'dependencies'],
        ['opt_dep_vs', 'optionalDependencies'],
        // npm (>= 7) installs the peer dependencies of every package it installs,
        // the ones marked optional via `peerDependenciesMeta` excepted
        ['peer_dep_vs', 'peerDependencies']
    ].concat(
        need_dev_deps ? [['dev_dep_vs', 'devDependencies']] : []
    ).forEach(([dep_type, dep_field]) => {
        const deps_of_type = level_info[dep_type] || {};

        coroutine.parallel(
            Object.keys(deps_of_type),
            dname => {
                try {
                    resolve_dep(dname);
                } catch (e) {
                    dep_failures.push({
                        name: dname,
                        spec: deps_of_type[dname],
                        dep_field: dep_field,
                        error: e
                    });
                }
            },
            CST.DEFAULT_FIBERS
        );

        function resolve_dep(dname) {
            const _deps = level_info[dep_type];

            // what the package ships inside its own tarball is not resolved: npm keeps
            // the copy the tarball carries, and a lockfile describes it with
            // `inBundle`. The root project is the exception — its own bundled list is
            // about what it publishes, and npm still installs those dependencies
            if (level_info.parent && level_info.bundle_deps && level_info.bundle_deps[dname])
                return;

            let v = _deps[dname];
            let child_level_info = level_info.node_modules[dname];

            // For workspace packages, skip external fetch but still process their dependencies
            if (child_level_info && child_level_info.workspace_package) {
                // Ensure version matches for workspace packages
                if (child_level_info) _deps[dname] = child_level_info.version;
                return;
            }

            if (child_level_info === undefined) {
                // a workspace member or a local package is not a registry package, so
                // neither a refresh nor a fetch may be aimed at it: `--update` used to
                // refresh the whole level, which sent a sibling workspace package to the
                // registry and failed with a 404. A `workspace:` spec only a member can
                // answer, and a `file:` one is checked against the directory it names —
                // another copy of the same name elsewhere is a different package
                const provided = find_provided_node(dname, level_info);

                if (provided && (provided.workspace_package || provided.local_package) &&
                    (String(v).indexOf('workspace:') === 0 || node_satisfies(provided, v))) {
                    _deps[dname] = provided.version;
                    return;
                }
            }

            // `--update`: ask the registry again instead of keeping what is
            // already there, and pass that down to the subtree (npm's
            // `update <pkg>` refreshes what that package needs too)
            const refresh = should_update(dname, level_info);

            if (refresh) {
                level_info.updated = true;
                child_level_info = undefined;
            } else if (child_level_info === undefined) {
                // nothing on disk to start from: the lockfile is the tree npm
                // would have started from, and the version it pins is kept
                // while it satisfies the spec
                const locked = locked_module_info(dname, v, level_info);

                if (locked)
                    child_level_info = level_info.node_modules[dname] = locked;
            }

            if (child_level_info === undefined || !node_satisfies(child_level_info, v)) {
                // a peer dependency is a placement requirement, not a second install:
                // when the tree already holds the package at another version npm keeps
                // that copy and warns, and nesting another one here is what put two
                // instances of a module with a singleton in it on disk (a real project's
                // drawing converter went from 7/7 to 1/7 on exactly that)
                if (dep_type === 'peer_dep_vs') {
                    const provided = find_provided_node(dname, level_info);

                    if (provided) {
                        console.warn(`[install] peer ${dname}@${v} is not satisfied by the installed ` +
                            `${provided.version} (${describe_require_chain(level_info)}) - keeping it`);
                        _deps[dname] = provided.version;
                        return;
                    }
                }

                // `find_version` answers "an ancestor already provides this",
                // which is not the answer a refresh is looking for
                if (refresh || !find_version(dname, v, level_info))
                    child_level_info = level_info.node_modules[dname] = fetch_leveled_module_info(dname, v, level_info, base_dir);

                // check platform compatibility; skip optional deps that don't match
                if (child_level_info && !check_platform_match(child_level_info)) {
                    if (dep_type === 'opt_dep_vs') {
                        install_log('skip incompatible:', dname, '@', child_level_info.version, '(os/cpu mismatch)');

                        // npm keeps such a package in the tree — and in the
                        // lockfile — and only skips it while installing: a
                        // lockfile without it cannot be used on the platform
                        // it was written for
                        child_level_info.platform_mismatch = true;
                        child_level_info.node_modules = {};
                        return;
                    }
                    console.warn('platform mismatch for', dname, '- may not work');
                }

                /**
                 * @todo deal with special installation name, such as 'fibjs/fib-graphql'
                 */
                // a package whose own name differs from the one it was asked for: a
                // registry package asked for under another name is an npm alias, and it
                // keeps that name — it is its directory, and what a lockfile records —
                // while a git/local spec that resolves to another name keeps the
                // historical rename, because that name is what `--save` writes into
                // package.json
                if (child_level_info &&
                    (child_level_info.pkg_install_typeinfo || {}).type !== 'registry' &&
                    child_level_info.name !== dname) {
                    const pkg_name = child_level_info.name;
                    const installnation_name = dname;
                    child_level_info = level_info.node_modules[pkg_name] = level_info.node_modules[dname]

                    if (child_level_info.parent) {
                        if (child_level_info.parent.dep_vs[installnation_name]) {
                            child_level_info.parent.dep_vs[pkg_name] = installnation_name
                            delete child_level_info.parent.dep_vs[installnation_name]
                            name_maps_installation2pkg[installnation_name] = pkg_name
                        }
                        if (child_level_info.parent.dev_dep_vs[installnation_name]) {
                            child_level_info.parent.dev_dep_vs[installnation_name] = installnation_name
                            delete child_level_info.parent.dev_dep_vs[installnation_name]
                            name_maps_installation2pkg[installnation_name] = pkg_name
                        }
                    }

                    delete level_info.node_modules[installnation_name]

                    // the node now sits under the name it resolved to, and the
                    // lockfile has to agree with it: the declared spec follows
                    // the rename, falling back to what the tree records
                    if (level_info.declared && level_info.declared[dep_type])
                        delete level_info.declared[dep_type][installnation_name]

                    dname = pkg_name
                }
            }

            if (child_level_info) _deps[dname] = child_level_info.version;
        }
    });

    if (dep_failures.length > 0)
        throw new Error(format_dep_failures(level_info, dep_failures));

    for (let k in level_info.node_modules) {
        const child = level_info.node_modules[k];

        walkthrough_deps(
            child,
            // a workspace member is installed like a root of its own: npm installs the
            // devDependencies of every member of the project, while a dependency's own
            // devDependencies are never installed
            child.workspace_package === true,
            // a local package lives outside node_modules, so its own relative
            // dependencies are resolved against its own directory
            child.local_path || path.join(base_dir, 'node_modules', k)
        );
    }
}

function move_up(level_info, parent) {
    // only what this run produced is hoisted; a package that was already on disk stays
    // where npm put it, and the tree is not reshaped around it. A node a repair or a
    // resolve just created, on the other hand, has to bubble up as far as its version
    // allows — that is what keeps one instance of a package instead of a copy per
    // dependent
    if (parent !== undefined)
        for (let k in level_info.node_modules) {
            const m = level_info.node_modules[k];
            if (m.new_module && !m.workspace_package) { // don't move workspace packages
                const m1 = parent.node_modules[k];
                if (m1 === undefined || m1.version === m.version) {
                    parent.node_modules[k] = m;
                    delete level_info.node_modules[k];

                    // where a node sits is what its dependencies are resolved against:
                    // a parent pointer left pointing at the level it came from makes the
                    // tree claim a copy is visible when it is not
                    m.parent = parent;

                    parent.module_list.push(k);
                }
            }
        }

    level_info.module_list = Object.keys(level_info.node_modules);
    for (let i = 0; i < level_info.module_list.length; i++)
        move_up(level_info.node_modules[level_info.module_list[i]], level_info);
}

/**
 * @description is that dependency served from where the node sits now? Its own
 *              `node_modules` first, then every ancestor's
 */
function dep_provided(level_info, name, spec) {
    let cur = level_info;

    while (cur !== undefined) {
        const info = cur.node_modules[name];

        // a link is materialized at the root and is visible from anywhere
        if (info !== undefined)
            return info.local_package === true || info.workspace_package === true ||
                node_satisfies(info, spec);

        cur = cur.parent;
    }

    return false;
}

/**
 * @description an installed copy somewhere in the tree that could serve that spec. The
 *              copy to nest under a node is a copy of it, so it has to be something the
 *              installer put there (registry or git) and something this platform can
 *              run — a link is visible from everywhere and needs no copy
 */
function find_installable_node(name, spec) {
    let found;

    (function scan(node) {
        const keys = Object.keys(node.node_modules || {});

        for (let i = 0; i < keys.length && !found; i++) {
            const child = node.node_modules[keys[i]];
            const type = (child.pkg_install_typeinfo || {}).type;

            if (keys[i] === name && (type === 'registry' || type === 'git') &&
                !child.platform_mismatch && node_satisfies(child, spec))
                found = child;
            else
                scan(child);
        }
    })(rootsnap);

    return found;
}

/**
 * @description a copy of an installed package under another node: the same version
 *              from the same source, with its dependencies to be checked from its new
 *              place in turn
 */
function copy_node_under(donor, parent) {
    const declared = donor.declared;

    return {
        name: donor.name,
        real_name: donor.real_name,
        version: donor.version,
        dep_vs: util.clone(donor.dep_vs || {}),
        // devDependencies are never installed for a dependency
        dev_dep_vs: {},
        opt_dep_vs: util.clone(donor.opt_dep_vs || {}),
        peer_dep_vs: util.clone(donor.peer_dep_vs || {}),
        declared: declared ? {
            dep_vs: util.clone(declared.dep_vs || {}),
            opt_dep_vs: util.clone(declared.opt_dep_vs || {}),
            peer_dep_vs: util.clone(declared.peer_dep_vs || {}),
        } : undefined,
        bin: donor.bin,
        binary: donor.binary,
        binary_pending: donor.binary_pending,
        os: donor.os,
        cpu: donor.cpu,
        libc: donor.libc,
        dist: donor.dist,
        pkg_install_typeinfo: donor.pkg_install_typeinfo,
        node_modules: {},
        parent: parent,
        new_module: true,
    };
}

/**
 * @description hoisting moves a package away from the copies it was resolved against.
 *              A node inside a workspace member can be resolved against a copy that
 *              sits beside it (the member's own `node_modules`), and hoisting then
 *              takes it up to the root, where that copy is invisible: `npm ls` calls
 *              the result `invalid` while the lockfile — written from the tree — looks
 *              fine, and a second install does not repair it. Every declared edge is
 *              checked from where its node ended up, and a copy of the provider is
 *              nested under it when nothing above can serve it, which is the shape
 *              npm's ideal tree arrives at as well
 */
function ensure_declared_deps_visible(level_info) {
    const declared = level_info.declared;

    if (declared) {
        [
            ['dep_vs', declared.dep_vs],
            ['opt_dep_vs', declared.opt_dep_vs],
            ['peer_dep_vs', declared.peer_dep_vs],
        ].forEach(([field, deps]) => {
            Object.keys(deps || {}).forEach(name => {
                // a peer the tree does not satisfy is npm's invalid peer: the resolver
                // keeps the copy it warned about, it does not duplicate it here
                if (field === 'peer_dep_vs' && find_provided_node(name, level_info))
                    return;

                // what the package ships inside its own tarball needs no nesting
                if (level_info.bundle_deps && level_info.bundle_deps[name])
                    return;

                const spec = deps[name];

                if (dep_provided(level_info, name, spec))
                    return;

                const donor = find_installable_node(name, spec);

                if (!donor)
                    return;

                level_info.node_modules[name] = copy_node_under(donor, level_info);
                install_log('nest:', name, '@', donor.version, '(the copy it was resolved against is out of reach)');
            });
        });
    }

    Object.keys(level_info.node_modules).forEach(k => ensure_declared_deps_visible(level_info.node_modules[k]));
}

/**
 * @description mark the nodes a production edge cannot reach. npm answers the same
 *              question in `calc-dep-flags.js`, starting every node in the dev and
 *              optional trees and walking the edges it can reach; the answer decides
 *              two things: what the lockfile says (`dev` / `optional` / `devOptional`
 *              / `peer`) and whether a package is installed at all — a lockfile
 *              always describes the whole graph (N14), an install may not
 *
 *              Read as paths, a flag means "no path from the root reaches this node
 *              without an edge of that kind": a package only an optional edge reaches
 *              is optional — even when that edge hangs off a package a production
 *              edge reaches, which is every platform specific binary in a lockfile
 */
function mark_reachability(rootsnap) {
    let next_id = 0;

    function id_of(node) {
        if (node._reach_id === undefined)
            node._reach_id = ++next_id;

        return node._reach_id;
    }

    // the nearest ancestor that has the name, then upwards: the tree is hoisted, so
    // most of them live at the root
    function lookup(from, name) {
        let cur = from;

        while (cur) {
            if (cur.node_modules && cur.node_modules[name])
                return cur.node_modules[name];
            cur = cur.parent;
        }

        return rootsnap.node_modules[name] || null;
    }

    const EDGES = [
        ['prod', node => node.dep_vs],
        ['dev', node => node.dev_dep_vs],
        ['optional', node => node.opt_dep_vs],
        ['peer', node => node.peer_dep_vs],
    ];
    const FLAGS = [
        ['dev', ['dev']],
        ['optional', ['optional']],
        ['devOptional', ['dev', 'optional']],
        ['peer', ['peer']],
    ];

    // a workspace member is a project of its own: what it needs is part of the graph
    // even when the root never asks for it
    const roots = [rootsnap];

    Object.keys(rootsnap.node_modules || {}).forEach(name => {
        const member = rootsnap.node_modules[name];

        if (member.workspace_package)
            roots.push(member);
    });

    /**
     * @description the nodes a path from a root reaches without using an edge of the
     *              kinds in `skip`
     */
    function reachable(skip) {
        const seen = {};
        const stack = roots.slice();

        roots.forEach(r => { seen[id_of(r)] = true; });

        while (stack.length) {
            const node = stack.pop();

            EDGES.forEach(edge => {
                if (skip.indexOf(edge[0]) > -1)
                    return;

                Object.keys(edge[1](node) || {}).forEach(name => {
                    const child = lookup(node, name);
                    if (!child)
                        return;

                    const id = id_of(child);

                    if (seen[id])
                        return;

                    seen[id] = true;
                    stack.push(child);
                });
            });
        }

        return seen;
    }

    const reach = { any: reachable([]) };

    FLAGS.forEach(flag => { reach[flag[0]] = reachable(flag[1]); });

    (function mark(node) {
        Object.keys(node.node_modules || {}).forEach(name => {
            const child = node.node_modules[name];
            const id = id_of(child);
            const known = reach.any[id] === true;

            // npm's `dev` / `optional` / `devOptional` say which kind of edge reaches
            // an entry when the other kind does not: a node every path needs a dev
            // edge for is a dev node, one every path needs an optional edge for is an
            // optional node, and one that needs either is in the overlap
            child.dev_only = known && reach.dev[id] !== true;
            child.optional_only = known && reach.optional[id] !== true;
            child.dev_optional = known && reach.devOptional[id] !== true;
            // what `--omit=peer` leaves out
            child.peer_only = known && reach.peer[id] !== true;

            mark(child);
        });
    })(rootsnap);
}

/**
 * @description does `--update` want this dependency resolved again? Without a
 *              package argument every dependency is refreshed, with one the named
 *              package is — and so is everything it needs, because the level that
 *              was refreshed says so
 */
function should_update(name, level_info) {
    if (!ctx.update)
        return false;

    if (level_info.updated)
        return true;

    if (ctx.update.all)
        return true;

    if (name === ctx.update.name)
        ctx.update.matched = true;

    return name === ctx.update.name;
}

/**
 * @description does `omit` leave this node out of the install? The lockfile still
 *              describes it: npm's omit is about the disk, not about the graph (N14).
 *              The answers are npm's (`audit-report.js`): a node in the overlap of the
 *              dev and the optional tree — a platform binary of a dev dependency, say
 *              — is left out only when both are
 */
function omitted_by(node) {
    if (node.dev_only && ctx.omit.dev)
        return true;
    if (node.optional_only && ctx.omit.optional)
        return true;
    if (node.dev_optional && ctx.omit.dev && ctx.omit.optional)
        return true;
    if (node.peer_only && ctx.omit.peer)
        return true;

    return false;
}

const mv_paths = {};
function generate_mv_paths(level_info, parent_p) {
    if (level_info.new_module) {
        for (let k in level_info.node_modules) {
            const lmod = level_info.node_modules[k];
            const bp = path.join(parent_p, 'node_modules');

            if (lmod.new_module && !lmod.workspace_package && !lmod.local_package &&
                !lmod.platform_mismatch && !omitted_by(lmod)) { // don't generate download paths for workspace/local packages, nor for a package this platform cannot run, nor for one `omit` leaves out
                const mv = k + '@' + lmod.version;

                let ps = mv_paths[mv];
                if (ps === undefined) {
                    switch (lmod.pkg_install_typeinfo.type) {
                        case 'registry':
                            ps = {
                                name: lmod.name,
                                pkg_install_typeinfo: lmod.pkg_install_typeinfo,
                                bin: lmod.bin,
                                binary: lmod.binary,
                                // the entry came from the lockfile and its package may
                                // ship a prebuilt addon: read from the unpacked
                                // manifest instead of asking the registry again
                                binary_pending: lmod.binary_pending === true,
                                dist: lmod.dist,
                                base_path: [bp]
                            };
                            break
                        case 'git':
                            ps = {
                                name: lmod.name,
                                pkg_install_typeinfo: lmod.pkg_install_typeinfo,
                                bin: lmod.bin,
                                binary: lmod.binary,
                                dist: null,
                                base_path: [bp]
                            };
                            break
                        case 'local':
                            // local packages don't need download; skip
                            break
                    }

                    if (ps)
                        mv_paths[mv] = ps;
                } else
                    ps.base_path.push(bp);
            }

            generate_mv_paths(lmod, path.join(bp, lmod.name));
        }
    }
}

function find_tar_home(untar_files) {
    var archive_root_name;
    var first_file_name;
    var found = false;

    untar_files.forEach(file => {
        if (file.typeflag == "0" || file.typeflag == "1") {
            if (!archive_root_name)
                first_file_name = archive_root_name = file.filename;
            else {
                found = true;
                archive_root_name = helpers_string.find_least_common_str(archive_root_name, file.filename);
            }
        }
    });

    if (!found)
        archive_root_name = path.dirname(first_file_name);

    return helpers_string.ensure_unsuffx(archive_root_name);
}

/**
 * @description fetch and unpack the prebuilt addon a package asks for (its own
 *              `binary` field). npm has no such field, so a lockfile never describes
 *              it: this is fetched without an integrity to check against, which is
 *              the limitation recorded in the plan (§7 R10)
 */
function download_binary_task(mvm) {
    install_log("[install addon]", mvm.binary.hosted_tarball);

    var binary_tgz;
    for (let _dl = 0; _dl < 3; _dl++) {
        try {
            var binary_r = http_get(mvm.binary.hosted_tarball, {
                headers: auth_headers_for(mvm.binary.hosted_tarball)
            });
            if (binary_r.statusCode !== 200) {
                console.error('[download] error:', mvm.name, mvm.binary.hosted_tarball, `-> HTTP ${binary_r.statusCode}`);
                process.exit();
            }
            binary_tgz = binary_r.bytes();
            binary_r = null;
            break;
        } catch (e) {
            console.log(e);
            console.warn(`[download] retry ${_dl + 1}: ${mvm.binary.hosted_tarball}`);
        }
    }
    if (!binary_tgz) {
        console.error('[download] failed:', mvm.name, 'prebuilt binary', mvm.binary.hosted_tarball);
        process.exit(-1);
    }

    var tgz = binary_tgz;

    let t;
    if (tgz[0] === 0x1f && tgz[1] === 0x8b)
        t = zlib.gunzip(tgz);
    else
        t = tgz;
    tgz = null;

    const untar_files = untar(t.buffer);
    t = null;

    const archive_root_name = find_tar_home(untar_files);

    mvm.base_path.forEach(bp => {
        untar_files.forEach(file => {
            if (file.typeflag == "1") {
                const read_files = untar_files.filter(f => f.filename == file.linkname);
                file.typeflag = "0";
                file.linkname = "";
                file.fileData = read_files[0].fileData;
                file.size = read_files[0].size;
            }

            if (file.typeflag == "0") {
                var bpath = path.join(bp, task_dir(mvm), mvm.binary.module_path, file.filename.slice(archive_root_name.length));
                fs.mkdir(path.dirname(bpath), { recursive: true });
                fs.writeFile(bpath, file.fileData);
                fs.chmod(bpath, parseInt(file.mode, 8));
            }
        });
    });

    install_log("extract addon:", mvm.binary.hosted_tarball);
}

/**
 * @description the prebuilt addon of a package the lockfile describes: a lockfile has
 *              no field for it (npm does not model it), so the package's own
 *              package.json is asked once it is unpacked — the frozen install does the
 *              same for what it puts in place
 * @returns true when an addon was fetched
 */
function download_manifest_binary(mvm) {
    if (!mvm.base_path.length)
        return false;

    let pkgjson;
    try {
        pkgjson = JSON.parse(fs.readTextFile(path.join(mvm.base_path[0], task_dir(mvm), 'package.json')));
    } catch (e) {
        return false;
    }

    if (!pkgjson.binary)
        return false;

    let binary;
    try {
        // relative `module_path`, see the note in the local package branch
        binary = versioning.evaluate(pkgjson, { root: '/', module_root: '.' }, 3);
    } catch (e) {
        // a `binary` block that does not describe a node-pre-gyp package is the
        // package's problem: the install goes on without the prebuilt addon
        console.warn(`[install] ${mvm.name}: ignoring an unusable binary field` +
            ` (${String(e.message).split('\n')[0]})`);
        return false;
    }

    if (!binary || !binary.hosted_tarball)
        return false;

    mvm.binary = binary;
    download_binary_task(mvm);

    return true;
}

function download_module() {
    // a package's tarball carries copies of the dependencies it bundles
    // (`bundleDependencies`), so unpacking it after those were put in place left
    // the bundled version on disk where the plan had another one: one pass per
    // depth, a package before anything that goes inside it (npm reifies in
    // dependency order for the same reason)
    const waves = [];
    let deepest = -1;

    Object.keys(mv_paths).forEach(mkey => {
        const d = mv_depth(mv_paths[mkey]);

        (waves[d] = waves[d] || []).push(mkey);

        if (d > deepest)
            deepest = d;
    });

    const download_wave = keys => coroutine.parallel(
        keys,
        mkey => {
            const mvm = mv_paths[mkey];

            const registry_i_tuples = [];
            const git_i_tuples = [];
            const existed_dirs = {};
            function ensure_dir(dirname) {
                if (process.platform === 'win32')
                    if (existed_dirs[dirname]) return;

                existed_dirs[dirname] = true;
                fs.mkdir(dirname, { recursive: true });
            }

            switch (mvm.pkg_install_typeinfo.type) {
                case 'registry':
                    let tgz_path = null;
                    let tgz_download = null;
                    for (let _dl = 0; _dl < 3; _dl++) {
                        try {
                            var r = http_get(mvm.dist.tarball, {
                                headers: mvm.dist.headers || auth_headers_for(mvm.dist.tarball)
                            });
                            if (r.statusCode !== 200) {
                                console.error('[download] error:', mvm.name, mvm.dist.tarball, `-> HTTP ${r.statusCode}`);
                                process.exit();
                            }
                            // straight to the disk, hashed on the way: a big tarball
                            // is never read into memory (see download_to_file)
                            tgz_path = temp_path(mvm);
                            tgz_download = download_to_file(r, tgz_path, mvm.dist);
                            r = null;
                            break;
                        } catch (e) {
                            remove_temp(tgz_path);
                            tgz_path = null;
                            tgz_download = null;
                            console.log(e);
                            console.warn(`[download] retry ${_dl + 1}: ${mvm.dist.tarball}`);
                        }
                    }
                    if (!tgz_path) {
                        console.error('[download] failed:', mvm.name, 'tarball', mvm.dist.tarball);
                        process.exit(-1);
                    }

                    // the lockfile is what a frozen install trusts: an entry it cannot
                    // verify is refused (the resolve path only warns, npm does not
                    // verify at all there)
                    if (!verify_digest(tgz_download.digest, mvm, mvm.frozen === true)) {
                        remove_temp(tgz_path);
                        console.error('[download] failed:', mvm.name, 'tarball', mvm.dist.tarball);
                        process.exit(-1);
                    }

                    let t;
                    if (tgz_download.gzip) {
                        // only the unpacked tar has to be in memory: the compressed
                        // payload goes through a scratch file as well
                        const tar_path = tgz_path + '.tar';
                        const gz_in = fs.createReadStream(tgz_path);
                        const tar_out = fs.createWriteStream(tar_path);

                        try {
                            zlib.gunzipTo(gz_in, tar_out);
                        } finally {
                            gz_in.close();
                            tar_out.close();
                        }

                        remove_temp(tgz_path);
                        t = fs.readFile(tar_path);
                        remove_temp(tar_path);
                    } else {
                        t = fs.readFile(tgz_path);
                        remove_temp(tgz_path);
                    }

                    const untar_files = untar(t.buffer);


                    // most package from registry is archived with root directory `package`
                    archive_root_name = find_tar_home(untar_files);

                    untar_files.forEach(file => {
                        mvm.base_path.forEach(bp => {
                            if (file.typeflag == "1") {
                                const read_files = untar_files.filter(f => f.filename == file.linkname);
                                file.typeflag = "0";
                                file.linkname = "";
                                file.fileData = read_files[0].fileData;
                                file.size = read_files[0].size;
                            }

                            if (file.typeflag == "0") {
                                const tpath = path.join(bp, task_dir(mvm), file.filename.slice(archive_root_name.length));
                                fs.mkdir(path.dirname(tpath), { recursive: true });
                                fs.writeFile(tpath, file.fileData);
                                fs.chmod(tpath, parseInt(file.mode, 8));
                            }
                        });
                    });

                    install_log('extract:', mvm.dist.tarball);
                    break
                case 'git':
                    const git_archive_url = helpers_pkg.get_git_archive_url(mvm.pkg_install_typeinfo);
                    var git_r;
                    var git_zip_file;
                    for (let _dl = 0; _dl < 3; _dl++) {
                        try {
                            git_r = http_get(git_archive_url);
                            if (git_r.statusCode !== 200) {
                                console.error('[download] error:', mvm.name, git_archive_url, `-> HTTP ${git_r.statusCode}`);
                                process.exit();
                            }
                            git_zip_file = zip.open(git_r.bytes());
                            git_r = null;
                            break;
                        } catch (e) {
                            console.log(e);
                            console.warn(`[download] retry ${_dl + 1}: ${git_archive_url}`);
                            git_r = null;
                            git_zip_file = null;
                        }
                    }
                    if (!git_zip_file) {
                        console.error('[download] failed:', mvm.name, 'git archive', git_archive_url);
                        process.exit(-1);
                    }
                    const namelist = git_zip_file.namelist();

                    archive_root_name = `${mvm.pkg_install_typeinfo.git_basename}-${mvm.pkg_install_typeinfo.git_reference}`;
                    if (namelist[0].indexOf(archive_root_name) !== 0) {
                        archive_root_name = helpers_string.ensure_unsuffx(
                            helpers_string.find_least_common_str(namelist[0], namelist[1])
                        )
                    }

                    mvm.base_path.forEach(bp => {
                        namelist.forEach((member) => {
                            const relpath = member.slice(archive_root_name.length);
                            if (!relpath) return;

                            const tpath = path.join(bp, task_dir(mvm), relpath);

                            // skip directory
                            if (tpath.endsWith(SEP)) return;
                            ensure_dir(path.dirname(tpath));

                            git_i_tuples.push([
                                member, tpath
                            ]);
                        })
                    });

                    git_i_tuples.forEach(([member, tpath]) => {
                        git_zip_file.extract(member, tpath);
                    });

                    install_log('extract:', git_archive_url);
                    break
                case 'local':
                    const localSrcPath = mvm.pkg_install_typeinfo.local_path;
                    mvm.base_path.forEach(bp => {
                        const destPath = path.join(bp, task_dir(mvm));

                        // skip if already exists (symlink or directory)
                        if (fs.exists(destPath)) {
                            try {
                                const st = fs.lstat(destPath);
                                if (st.isSymbolicLink() || st.isDirectory())
                                    return;
                            } catch (e) { }
                        }

                        fs.mkdir(path.dirname(destPath), { recursive: true });
                        link_or_copy_dir(localSrcPath, destPath);
                    });
                    break
            }

            if (mvm.binary)
                download_binary_task(mvm);
            else if (mvm.binary_pending)
                download_manifest_binary(mvm);

            if (mvm.bin)
                link_bins(task_dir(mvm), mvm.bin, mvm.base_path);
        },
        CST.DEFAULT_FIBERS
    );

    for (let d = 0; d <= deepest; d++) {
        if (waves[d])
            download_wave(waves[d]);
    }
}

/**
 * @description how deep the copies of a task go: the number of path segments below
 *              the project. A parent's tarball may carry a copy of the dependency
 *              that goes inside it, so the parent is unpacked first
 */
function mv_depth(mvm) {
    const root = process.cwd();
    let depth = 0;

    (mvm.base_path || []).forEach(p => {
        let n = 0;

        path.relative(root, p).split(path.sep).forEach(seg => {
            if (seg.length)
                n++;
        });

        if (n > depth)
            depth = n;
    });

    return depth;
}

function download_one(mkey) {
    const mvm = mv_paths[mkey];

    const registry_i_tuples = [];
    const git_i_tuples = [];
    const existed_dirs = {};
    function ensure_dir(dirname) {
        if (process.platform === 'win32')
            if (existed_dirs[dirname]) return;

        existed_dirs[dirname] = true;
        fs.mkdir(dirname, { recursive: true });
    }

    switch (mvm.pkg_install_typeinfo.type) {
        case 'registry':
            let tgz_path = null;
            let tgz_download = null;
            for (let _dl = 0; _dl < 3; _dl++) {
                try {
                    var r = http_get(mvm.dist.tarball, {
                        headers: mvm.dist.headers || auth_headers_for(mvm.dist.tarball)
                    });
                    if (r.statusCode !== 200) {
                        console.error('[download] error:', mvm.name, mvm.dist.tarball, `-> HTTP ${r.statusCode}`);
                        process.exit();
                    }
                    // straight to the disk, hashed on the way: a big tarball
                    // is never read into memory (see download_to_file)
                    tgz_path = temp_path(mvm);
                    tgz_download = download_to_file(r, tgz_path, mvm.dist);
                    r = null;
                    break;
                } catch (e) {
                    remove_temp(tgz_path);
                    tgz_path = null;
                    tgz_download = null;
                    console.log(e);
                    console.warn(`[download] retry ${_dl + 1}: ${mvm.dist.tarball}`);
                }
            }
            if (!tgz_path) {
                console.error('[download] failed:', mvm.name, 'tarball', mvm.dist.tarball);
                process.exit(-1);
            }

            // the lockfile is what a frozen install trusts: an entry it cannot
            // verify is refused (the resolve path only warns, npm does not
            // verify at all there)
            if (!verify_digest(tgz_download.digest, mvm, mvm.frozen === true)) {
                remove_temp(tgz_path);
                console.error('[download] failed:', mvm.name, 'tarball', mvm.dist.tarball);
                process.exit(-1);
            }

            let t;
            if (tgz_download.gzip) {
                // only the unpacked tar has to be in memory: the compressed
                // payload goes through a scratch file as well
                const tar_path = tgz_path + '.tar';
                const gz_in = fs.createReadStream(tgz_path);
                const tar_out = fs.createWriteStream(tar_path);

                try {
                    zlib.gunzipTo(gz_in, tar_out);
                } finally {
                    gz_in.close();
                    tar_out.close();
                }

                remove_temp(tgz_path);
                t = fs.readFile(tar_path);
                remove_temp(tar_path);
            } else {
                t = fs.readFile(tgz_path);
                remove_temp(tgz_path);
            }

            const untar_files = untar(t.buffer);


            // most package from registry is archived with root directory `package`
            archive_root_name = find_tar_home(untar_files);

            untar_files.forEach(file => {
                mvm.base_path.forEach(bp => {
                    if (file.typeflag == "1") {
                        const read_files = untar_files.filter(f => f.filename == file.linkname);
                        file.typeflag = "0";
                        file.linkname = "";
                        file.fileData = read_files[0].fileData;
                        file.size = read_files[0].size;
                    }

                    if (file.typeflag == "0") {
                        const tpath = path.join(bp, task_dir(mvm), file.filename.slice(archive_root_name.length));
                        fs.mkdir(path.dirname(tpath), { recursive: true });
                        fs.writeFile(tpath, file.fileData);
                        fs.chmod(tpath, parseInt(file.mode, 8));
                    }
                });
            });

            install_log('extract:', mvm.dist.tarball);
            break
        case 'git':
            const git_archive_url = helpers_pkg.get_git_archive_url(mvm.pkg_install_typeinfo);
            var git_r;
            var git_zip_file;
            for (let _dl = 0; _dl < 3; _dl++) {
                try {
                    git_r = http_get(git_archive_url);
                    if (git_r.statusCode !== 200) {
                        console.error('[download] error:', mvm.name, git_archive_url, `-> HTTP ${git_r.statusCode}`);
                        process.exit();
                    }
                    git_zip_file = zip.open(git_r.bytes());
                    git_r = null;
                    break;
                } catch (e) {
                    console.log(e);
                    console.warn(`[download] retry ${_dl + 1}: ${git_archive_url}`);
                    git_r = null;
                    git_zip_file = null;
                }
            }
            if (!git_zip_file) {
                console.error('[download] failed:', mvm.name, 'git archive', git_archive_url);
                process.exit(-1);
            }
            const namelist = git_zip_file.namelist();

            archive_root_name = `${mvm.pkg_install_typeinfo.git_basename}-${mvm.pkg_install_typeinfo.git_reference}`;
            if (namelist[0].indexOf(archive_root_name) !== 0) {
                archive_root_name = helpers_string.ensure_unsuffx(
                    helpers_string.find_least_common_str(namelist[0], namelist[1])
                )
            }

            mvm.base_path.forEach(bp => {
                namelist.forEach((member) => {
                    const relpath = member.slice(archive_root_name.length);
                    if (!relpath) return;

                    const tpath = path.join(bp, task_dir(mvm), relpath);

                    // skip directory
                    if (tpath.endsWith(SEP)) return;
                    ensure_dir(path.dirname(tpath));

                    git_i_tuples.push([
                        member, tpath
                    ]);
                })
            });

            git_i_tuples.forEach(([member, tpath]) => {
                git_zip_file.extract(member, tpath);
            });

            install_log('extract:', git_archive_url);
            break
        case 'local':
            const localSrcPath = mvm.pkg_install_typeinfo.local_path;
            mvm.base_path.forEach(bp => {
                const destPath = path.join(bp, task_dir(mvm));

                // skip if already exists (symlink or directory)
                if (fs.exists(destPath)) {
                    try {
                        const st = fs.lstat(destPath);
                        if (st.isSymbolicLink() || st.isDirectory())
                            return;
                    } catch (e) { }
                }

                fs.mkdir(path.dirname(destPath), { recursive: true });
                link_or_copy_dir(localSrcPath, destPath);
            });
            break
    }

    if (mvm.binary)
        download_binary_task(mvm);
    else if (mvm.binary_pending)
        download_manifest_binary(mvm);

    if (mvm.bin)
        link_bins(task_dir(mvm), mvm.bin, mvm.base_path);
}

/**
 * @description link the binaries a package declares into the `.bin` directory of
 *              every place it was installed. npm does this for every kind of
 *              package, a local one included
 * @param bin the package's `bin` field (a string or a name → file map)
 */
function link_bins(name, bin, base_paths) {
    if (!bin)
        return;

    var bins = bin;

    if (util.isString(bins)) {
        var single = {};
        single[path.basename(bins)] = bins;
        bins = single;
    }

    for (var b in bins) {
        base_paths.forEach(p => {
            var bin_path = path.join(p, '.bin');
            var cli_link = path.join(bin_path, b);
            var cli_file = path.join(p, name, bins[b]);
            var cli_file_r = path.relative(bin_path, cli_file);

            fs.mkdir(bin_path, { recursive: true });

            try {
                if (process.platform === 'win32') {
                    const regex = /^#!\/usr\/bin\/(?:env\s+)?([^\s]+)$/gm;
                    const script = fs.readTextFile(cli_file);
                    const match = regex.exec(script);
                    const sh = match ? match[1] : 'fibjs';

                    const scripts = win32_cli(sh, cli_file_r);

                    fs.writeFile(cli_link, scripts.sh);
                    fs.writeFile(cli_link + ".cmd", scripts.cmd);
                    fs.writeFile(cli_link + ".ps1", scripts.ps1);
                } else {
                    // npm's `bin-links` makes way for a link it has to write and
                    // leaves one that already points where it should: a tree that
                    // has them must not make a reinstall throw EEXIST, and a plain
                    // file in the way (a shim an older install wrote) is replaced
                    // the way npm replaces it
                    let current = null;

                    try {
                        current = fs.readlink(cli_link);
                    } catch (e) { }

                    if (current !== cli_file_r) {
                        if (current !== null || fs.exists(cli_link))
                            fs.unlink(cli_link);

                        fs.symlink(cli_file_r, cli_link);
                    }

                    fs.chmod(cli_file, 0o755);
                }
            } catch (e) {
                console.log(e);
            }

            install_log("install cli:", cli_link);
        });
    }
}

function dump_snap() {
    function dump_mod(base, k, m, end) {
        const has_ms = !util.isEmpty(m.node_modules);
        install_log(base + (end ? '└' : '├') + '─' + (has_ms ? '┬' : '─'), k + '@' + m.version);
        if (has_ms)
            dump_mods(base + (end ? ' ' : '│') + ' ', m.node_modules);
    }

    function dump_mods(base, ms) {
        const ks = Object.keys(ms).sort();
        ks.forEach((k, i) => {
            dump_mod(base, k, ms[k], i == ks.length - 1);
        });
    }

    dump_mods('', rootsnap.node_modules);
}

/**
 * @description the spec to record in package.json for a package that was installed
 *              from a local path: npm writes `file:<path>`, and that is what its
 *              lockfiles carry too
 */
function file_spec(spec) {
    if (typeof spec !== 'string' || spec === '' || spec.indexOf('file:') === 0)
        return spec;

    if (/^(\.{1,2}[\\/]|[\\/]|~[\\/]|[a-zA-Z]:[\\/])/.test(spec))
        return 'file:' + spec;

    return spec;
}

function update_pkgjson(rootsnap) {
    if (!need_add_newpkg_to_pkgjson) return;

    const pkgjson = rootsnap.pkgjson;

    const [real_pkgname, special_source_installation_name] = [
        name_maps_installation2pkg[ctx.new_pkgname] ? name_maps_installation2pkg[ctx.new_pkgname] : ctx.new_pkgname,
        ctx.new_pkgname
    ];

    if (!real_pkgname) return;

    if (need_add_newpkg_to_pkgjson === DEVDEPENDENCIES) {
        if (!rootsnap.dev_dep_vs || !rootsnap.dev_dep_vs[real_pkgname])
            throw new Error(`[update_pkgjson] no version specified for devDependency ${real_pkgname}`)

        ctx.dep_against_k = DEP_KEY_TUPLE[0];
        ctx.depk = DEP_KEY_TUPLE[1];
    } else {
        if (!rootsnap.dep_vs || !rootsnap.dep_vs[real_pkgname])
            throw new Error(`[update_pkgjson] no version specified for dependency ${real_pkgname}`)

        ctx.depk = DEP_KEY_TUPLE[0];
        ctx.dep_against_k = DEP_KEY_TUPLE[1];
    }

    try {
        delete pkgjson[ctx.dep_against_k][real_pkgname];
    } catch (error) { };

    if (!pkgjson[ctx.depk]) pkgjson[ctx.depk] = {};

    if (!pkgjson[ctx.depk][real_pkgname] || pkgjson[ctx.depk][real_pkgname] !== `^${rootsnap.node_modules[real_pkgname].version}`) {
        if (special_source_installation_name !== real_pkgname) {
            pkgjson[ctx.depk][real_pkgname] = file_spec(special_source_installation_name);
        } else {
            pkgjson[ctx.depk][real_pkgname] = `^${rootsnap.node_modules[real_pkgname].version}`;
        }
    }

    fs.writeFile('package.json', JSON.stringify(pkgjson, null, "  "));
}

const ctx = {};

// ---------------------- FROZEN INSTALL :start ------------------------- //

/**
 * @description remove a path, whatever it is
 */
function remove_path(p) {
    try {
        const st = fs.lstat(p);

        if (st.isDirectory() && !st.isSymbolicLink())
            rmdir_recursive(p);
        else
            fs.unlink(p);
    } catch (e) {
        console.warn(`[install] could not remove ${p}: ${e.message}`);
    }
}

/**
 * @description remove a directory tree. `quiet` is for the way out: another fiber may
 *              still be writing a scratch file, and a directory that outlives the
 *              process sits in the OS temp directory where nobody minds it
 */
function rmdir_recursive(dir, quiet) {
    if (!fs.exists(dir))
        return;

    fs.readdir(dir).forEach(name => {
        const p = path.join(dir, name);

        try {
            const st = fs.lstat(p);
            if (st.isDirectory() && !st.isSymbolicLink())
                rmdir_recursive(p, quiet);
            else
                fs.unlink(p);
        } catch (e) {
            if (!quiet)
                console.warn(`[install] could not remove ${p}: ${e.message}`);
        }
    });

    try {
        fs.rmdir(dir);
    } catch (e) {
        if (!quiet)
            console.warn(`[install] could not remove ${dir}: ${e.message}`);
    }
}

/**
 * @description where a temporary git checkout goes: the OS temp directory, not
 *              node_modules (that is being rebuilt while this runs)
 */
function tmp_dir() {
    return process.env.TMPDIR || process.env.TEMP || process.env.TMP || '/tmp';
}

/**
 * @description run git, loudly
 */
function git_run(args) {
    const res = child_process.spawnSync('git', args, { stdio: 'pipe' });

    if (res.error)
        throw new Error(`git ${args[0]} could not run: ${res.error.message}`);
    if (res.status !== 0)
        throw new Error(`git ${args.join(' ')} failed (${res.status}): ${String(res.stderr || '').trim()}`);

    return res;
}

/**
 * @description a lock entry that comes from a git repository
 */
function is_git_entry(entry) {
    const resolved = entry.resolved || '';

    return resolved.indexOf('git+') === 0 || resolved.indexOf('git://') === 0;
}

/**
 * @description the tarball url of a registry entry that carries no `resolved` (a
 *              lock written with `omit-lockfile-registry-resolved` looks like that):
 *              npm derives it from the registry and the package name
 */
function default_tarball_url(registry, entry) {
    const base = String(registry || 'https://registry.npmjs.org/').replace(/\/+$/, '');
    const slash = entry.name.indexOf('/');
    const short = entry.name.charAt(0) === '@' && slash > 0 ? entry.name.slice(slash + 1) : entry.name;

    return `${base}/${entry.name}/-/${short}-${entry.version}.tgz`;
}

/**
 * @description a scratch directory one install streams through. Tarballs are kept
 *              on the disk while they are fetched so that a big package never has
 *              to fit in memory (see `download_to_file`); the directory goes away
 *              with the process, however the process ends
 */
let temp_dir_path = null;
let temp_seq = 0;

function temp_dir() {
    if (!temp_dir_path) {
        temp_dir_path = path.join(os.tmpdir(), 'fibjs-install-' + process.pid);
        fs.mkdir(temp_dir_path, { recursive: true });

        process.on('exit', function () {
            // quiet: a fiber that is still streaming has a scratch file open, and the
            // directory it leaves behind is in the OS temp directory
            rmdir_recursive(temp_dir_path, true);
        });
    }

    return temp_dir_path;
}

function temp_path(task) {
    return path.join(temp_dir(), (++temp_seq) + '-' +
        String(task.name).replace(/[^\w.-]+/g, '_') + '.tgz');
}

function remove_temp(file_path) {
    if (!file_path)
        return;

    try {
        fs.unlink(file_path);
    } catch (e) {
        // a scratch file that is already gone is not an error
    }
}

// the size of one streaming step: big enough to keep the syscall count low, small
// enough that the copy never holds a large package
const DOWNLOAD_CHUNK = 1 << 20;

/**
 * @description stream a response body into `file_path`, hashing it on the way.
 *              Nothing holds the compressed payload whole: only the digest, the
 *              file on the disk and one chunk grow with the download
 * @returns {{ digest: null|string, gzip: boolean }} the hex digest of the algorithm
 *          the entry has to be checked with (the strongest one it carries, null
 *          when there is none) and whether the payload is a gzip stream
 */
function download_to_file(r, file_path, dist) {
    dist = dist || {};

    const plan = lockfile.integrity_plan(dist.integrity);
    const algorithm = plan ? plan.algorithm : (dist.shasum ? 'sha1' : null);

    // `copyTo` runs the response to its end. A `read()` loop stops at the first moment
    // the body has nothing buffered — which is how a partial download came to be hashed
    // and reported as an integrity mismatch, with a retry passing because the bytes the
    // registry serves were never the problem
    const out = fs.createWriteStream(file_path);
    let copied;

    try {
        copied = r.body.copyTo(out);
    } finally {
        try {
            out.close();
        } catch (e) {
            // a stream that is already closed is fine here
        }
    }

    // a body that stopped early is a failed download, not a short one: retry it
    const announced = parseInt(r.headers && r.headers['content-length'], 10);

    if (!isNaN(announced) && copied !== announced)
        throw new Error(`the body stopped early: ${copied} of ${announced} bytes`);

    // the digest is computed from what landed on the disk, which is also what gets
    // unpacked below
    const read = digest_file(file_path, algorithm);

    return {
        digest: read.digest,
        gzip: !!read.head && read.head.length === 2 && read.head[0] === 0x1f && read.head[1] === 0x8b
    };
}

/**
 * @description the digest of a scratch file, and its first two bytes, read in chunks:
 *              the compressed payload is never held whole, and the check covers exactly
 *              what the unpacker is about to read
 */
function digest_file(file_path, algorithm) {
    const hash = algorithm ? crypto.createHash(algorithm) : null;
    const fd = fs.open(file_path, 'r');
    const buf = Buffer.alloc(DOWNLOAD_CHUNK);
    let head = null;

    try {
        for (;;) {
            const n = fs.read(fd, buf, 0, buf.length);

            if (!n)
                break;

            const chunk = n === buf.length ? buf : buf.slice(0, n);

            // the two bytes are *copied*: `slice` hands back a view into the buffer,
            // which the next read overwrites — reading them later said "not a gzip"
            // about a perfectly good tarball, and the unpacker was fed the compressed
            // bytes
            if (!head && n >= 2) {
                head = Buffer.alloc(2);
                chunk.copy(head, 0, 0, 2);
            }

            if (hash)
                hash.update(chunk);
        }
    } finally {
        fs.close(fd);
    }

    return { digest: hash ? hash.digest('hex') : null, head: head };
}

/**
 * @description the check a tarball has to pass against what the lockfile (or the
 *              registry metadata) says about it: the SRI string when there is one,
 *              the sha1 `dist.shasum` otherwise. It works on the digest the
 *              streaming download computed instead of on the bytes. `strict`
 *              refuses an entry there is nothing to verify with, which is what a
 *              frozen install wants
 */
function verify_digest(digest, task, strict) {
    const dist = task.dist || {};
    const plan = lockfile.integrity_plan(dist.integrity);

    if (plan) {
        const res = lockfile.integrity_ok(digest, plan);

        if (!res.ok) {
            console.error(`[install] integrity mismatch: ${task.name}`);
            console.error(`  source  : ${dist.tarball}`);
            console.error(`  expected: ${res.expected}`);
            console.error(`  actual  : ${res.actual}`);
            return false;
        }

        return true;
    }

    // an SRI the helper cannot read (none of the algorithms npm writes) counts as
    // nothing to compare against, which is what `verify_integrity` decided
    if (dist.integrity)
        return true;

    if (dist.shasum) {
        if (digest !== dist.shasum) {
            console.error(`[install] shasum mismatch: ${task.name} ${dist.tarball}`);
            return false;
        }

        return true;
    }

    if (strict) {
        console.error(`[install] ${task.name}: the lockfile carries no integrity for`);
        console.error(`  ${dist.tarball}`);
        console.error('  pass --no-strict-integrity to install it unverified');
        return false;
    }

    console.warn(`[install] ${task.name}: nothing to verify the tarball with, installing unverified`);
    return true;
}

/**
 * @description the directory a package installs *into*: a package lands at
 *              `<base>/<name>`, and a scoped name carries its own `/`, so the base
 *              is the destination minus the name's own path segments
 *              (`node_modules/@scope/pkg` -> `node_modules`, not `node_modules/@scope`)
 */
function package_base_dir(dest, name) {
    let base = dest;
    const parts = String(name || '').split('/');

    for (let i = Math.max(parts.length, 1); i > 0; i--)
        base = path.dirname(base);

    return base;
}

/**
 * @description the directory a task installs its package *into*. A lockfile entry
 *              lives at its path, and npm's alias entries (`node_modules/x` holding
 *              the package `y`) show that the path — not the name the package calls
 *              itself — is where it goes
 */
function task_dir(mvm) {
    return mvm.dir_name || mvm.name;
}

/**
 * @description the header a fetch of that url needs, from the `.npmrc` entries the
 *              caller read (null when the url needs no credentials)
 */
function auth_headers_for(u) {
    if (!ctx.npmrc || !ctx.npmrc.auth)
        return null;

    return lockfile.auth_header_for_url(u, ctx.npmrc.auth);
}

/**
 * @description the TLS settings `.npmrc` asks for, as the options an http client
 *              takes. fibjs verifies through `rejectUnverified` (its
 *              `rejectUnauthorized` is the server side flag), and a private CA is
 *              handed over in `ca`
 */
function tls_options(npmrc) {
    if (!npmrc)
        return null;

    const tls = {};

    if (npmrc.strict_ssl === false)
        tls.rejectUnverified = false;

    let ca = npmrc.ca;

    if (!ca && npmrc.cafile) {
        const file = path.resolve(npmrc.cafile);

        if (fs.exists(file))
            ca = fs.readTextFile(file);
        else
            console.warn(`[install] cafile not found: ${file}`);
    }

    if (ca)
        tls.ca = ca;

    return Object.keys(tls).length ? tls : null;
}

/**
 * @description the registry a package is fetched from: a scope registry when
 *              `.npmrc` has one, the configured registry otherwise
 */
function registry_for_package(name) {    if (ctx.npmrc && name && name.charAt(0) === '@') {
        const slash = name.indexOf('/');
        const scope = slash > 0 ? name.slice(0, slash) : name;

        if (ctx.npmrc.scoped[scope])
            return normalize_registry_origin(ctx.npmrc.scoped[scope]);
    }

    return ctx.registry || rootsnap.registry;
}

/**
 * @description empty `node_modules` the way `npm ci` does: every entry goes. It
 *              happens only after the lockfile has been accepted, so a failed check
 *              never touches what is on disk
 */
function clear_node_modules(root) {
    const nm = path.join(root, 'node_modules');

    if (!fs.exists(nm))
        return;

    install_log('clear node_modules');
    fs.readdir(nm).forEach(name => remove_path(path.join(nm, name)));
}

/**
 * @description the offline sync check with the project's workspaces filled in
 */
function check_project_sync(rootsnap, lock) {
    return lockfile.check_sync(rootsnap.pkgjson, lock, {
        semver: semver,
        workspaces: find_workspace_packages(process.cwd(), rootsnap.pkgjson.workspaces).map(p => ({
            name: p.name,
            path: p.relative_path,
            pkgjson: p.package_json,
        })),
    });
}

/**
 * @description the sync check errors in npm's words. npm resolves the spec online
 *              to print a version in `Missing:`; this prints the spec instead (see
 *              the plan §4.5 for why it deliberately does not go online)
 */
function describe_sync_error(e) {
    if (e.kind === 'Missing')
        return `Missing: ${e.name}@${e.spec} from lock file` +
            (e.via && e.via !== 'package.json' ? ` (required by ${e.via})` : '');

    // npm's sentence is exactly this; the reason is only interesting when it is not the
    // version that is off (a link that points elsewhere, an alias that is not one)
    const why = e.why && e.why.indexOf('does not satisfy') < 0 ? ` (${e.why})` : '';

    return `Invalid: lock file's ${e.name}@${e.locked} does not satisfy ${e.name}@${e.spec}${why}`;
}

/**
 * @description report the errors of a sync check. A lockfile that fell behind produces
 *              hundreds of them (the audit saw 217), so the shape of the problem comes
 *              first; the details are there for the first few entries, and `--verbose`
 *              prints them all
 */
function print_sync_errors(lock, sync) {
    const counts = {};

    sync.errors.forEach(e => {
        const kind = e.optional ? `${e.kind} (optional)` : e.kind;
        counts[kind] = (counts[kind] || 0) + 1;
    });

    const summary = Object.keys(counts).map(k => `${k} ${counts[k]}`).join(', ');
    const shown = ctx.verbose ? sync.errors : sync.errors.slice(0, 20);

    console.error(`[install] ${lock.filename} is not in sync with package.json:`);
    console.error(`  ${sync.errors.length} error${sync.errors.length === 1 ? '' : 's'} (${summary})`);
    shown.forEach(e => console.error(`  ${describe_sync_error(e)}`));

    if (shown.length < sync.errors.length)
        console.error(`  … and ${sync.errors.length - shown.length} more (--verbose lists them all)`);

    console.error('  run `fibjs --install` to update the lockfile, then try again');
}

/**
 * @description is that path already what the lockfile asks for? A lock-first
 *              install keeps what is on disk and fills the gaps the way npm's reify
 *              does — emptying node_modules is what `npm ci` is for
 */
function lock_entry_satisfied(root, p, entry) {
    const dest = path.join(root, p);

    if (!fs.exists(dest))
        return false;

    if (entry.link) {
        try {
            if (!fs.lstat(dest).isSymbolicLink())
                return false;

            return path.resolve(fs.realpath(dest)) === path.resolve(root, entry.resolved || '');
        } catch (e) {
            return false;
        }
    }

    const pj = path.join(dest, 'package.json');
    if (!fs.exists(pj))
        return false;

    try {
        return JSON.parse(fs.readTextFile(pj)).version === entry.version;
    } catch (e) {
        return false;
    }
}

/**
 * @description node_modules/.package-lock.json is npm's note of what it installed.
 *              The installer changes node_modules, so whatever is in there is stale
 *              now: npm has checks for that, but removing it cannot hurt
 */
function remove_hidden_lock(root) {
    const p = path.join(root, 'node_modules', '.package-lock.json');

    if (fs.exists(p)) {
        remove_path(p);
        install_log('remove stale node_modules/.package-lock.json');
    }
}

/**
 * @description install exactly what the lockfile describes: the `packages` path
 *              table *is* the final node_modules layout, so nothing is resolved,
 *              hoisted or moved and the tree has npm's shape by construction
 * @param opts { registry, strict_integrity, incremental }
 */
function install_from_lock(lock, opts) {
    const o = opts || {};
    const packages = lockfile.to_path_map(lock);
    const root = process.cwd();
    const git_entries = [];
    const installed = [];
    const plan = { registry: 0, link: 0, git: 0, skipped: 0, kept: 0, bundled: 0 };

    // what the lockfile's own edges say about the kind of edge that reaches an entry.
    // A lockfile written before the flags were recorded leaves them out, and then a
    // platform specific binary looks like a package every machine needs: it is
    // downloaded and installed on a machine that cannot run it. A member's manifest
    // is part of the graph too — a lockfile of that vintage does not describe a
    // member's dependencies at all
    const graph = lockfile.graph_flags(packages, lock.raw && lock.raw.packages && lock.raw.packages[''], {
        workspaces: find_workspace_packages(process.cwd(), rootsnap.pkgjson.workspaces)
            .map(w => ({ path: w.relative_path, pkgjson: w.package_json })),
    });

    Object.keys(packages).forEach(p => {
        const entry = packages[p];

        // a link target is not installed on its own
        if (entry.target_only)
            return;

        // a copy the parent's tarball carries arrives with it: npm does not install
        // one of those on its own, and a lockfile records them with no source
        if (entry.in_bundle) {
            plan.bundled++;
            return;
        }

        // the entry's own flags are what npm wrote; a lockfile that does not carry
        // them says nothing, and the graph is the answer then
        const optional = entry.optional === true || graph.optional[p] === true;
        const dev = entry.dev === true || graph.dev[p] === true;

        // `omit` is about the disk, not about the lockfile: the entry stays described
        if ((dev && o.omit && o.omit.dev) || (optional && o.omit && o.omit.optional)) {
            plan.skipped++;
            install_log('skip (omit):', p);
            return;
        }

        // npm records every platform in the lockfile, only the matching entries land
        if (optional && !check_platform_match(entry)) {
            plan.skipped++;
            install_log('skip (platform):', p);
            return;
        }

        // lock-first install: keep what is already there and correct
        if (o.incremental && lock_entry_satisfied(root, p, entry)) {
            plan.kept++;
            return;
        }

        const dest = path.join(root, p);

        // npm writes an alias as an entry whose `name` is the package and whose path
        // is the name it is installed under (`node_modules/string-width-cjs` holding
        // `string-width`): the path decides where it goes, the name only says what
        // it is
        const leaf = lockfile.name_from_path(p) || entry.name;

        if (entry.link) {
            const target = path.resolve(root, entry.resolved || '');
            let bin = entry.bin;

            if (!bin) {
                // a lockfile link entry carries no `bin`; the package it points at does
                try {
                    bin = JSON.parse(fs.readTextFile(path.join(target, 'package.json'))).bin;
                } catch (e) {
                    bin = undefined;
                }
            }

            mv_paths['link:' + p] = {
                name: entry.name,
                dir_name: leaf,
                pkg_install_typeinfo: { type: 'local', local_path: target },
                bin: bin,
                dist: null,
                base_path: [package_base_dir(dest, leaf)],
            };
            plan.link++;
            installed.push(p);
            return;
        }

        if (is_git_entry(entry)) {
            git_entries.push({ entry: entry, dest: dest });
            plan.git++;
            installed.push(p);
            return;
        }

        const tarball = entry.resolved
            ? lockfile.apply_registry_replace(entry.resolved, o.registry, o.registry_policy)
            : default_tarball_url(o.registry, entry);

        // one task per path, not per name@version: two entries can share a name and
        // a version and still land in different directories
        const mv = 'lock:' + p;
        const task = mv_paths[mv] || (mv_paths[mv] = {
            name: entry.name,
            dir_name: leaf,
            pkg_install_typeinfo: { type: 'registry' },
            bin: entry.bin,
            binary: undefined,
            // an entry the lockfile cannot vouch for is refused unless the caller
            // asked for `--no-strict-integrity`
            frozen: o.strict_integrity !== false,
            dist: {
                tarball: tarball,
                integrity: entry.integrity,
                shasum: null,
                headers: auth_headers_for(tarball),
            },
            base_path: [],
        });

        task.base_path.push(package_base_dir(dest, leaf));
        plan.registry++;
        installed.push(p);
    });

    install_log(`[install] lockfile: ${plan.registry} registry, ${plan.link} link, ${plan.git} git` +
        (plan.skipped ? `, ${plan.skipped} skipped for this platform` : '') +
        (plan.bundled ? `, ${plan.bundled} shipped inside a parent` : '') +
        (plan.kept ? `, ${plan.kept} already in place` : ''));

    download_module();
    install_git_entries(git_entries);
    install_lock_binaries(lock, root, installed);

    return { plan: plan, installed: installed, packages: packages, graph: graph };
}

/**
 * @description the manifest a lockfile remembers for a node that ships copies of its
 *              own (`bundleDependencies`) or came inside another's tarball. A packer
 *              rewrites the manifest of what it bundles — npm@2's copy lost two
 *              dependencies and its bundled copies lost their `optionalDependencies`,
 *              which is also why npm marks those optional and this did not — while the
 *              lockfile has the manifest that was published, the one npm reads a
 *              candidate from. The version has to agree: a node on disk at another
 *              version is the tree's own answer, and an ordinary node's manifest is the
 *              tree's answer as well (a stale dependency list in the lockfile is what
 *              made a repaired tree fail its own sync check)
 */
function declared_from_lock(node) {
    const entry = ctx.lock_packages && ctx.lock_packages[node.lock_path];

    if (!entry || entry.version !== node.version)
        return;

    // a lockfile that says the copy came inside a tarball is believed: the version
    // alone cannot tell a shipped copy from an installed one
    if (entry.in_bundle)
        node.in_bundle = true;

    node.dep_vs = util.clone(entry.dependencies || {});
    node.opt_dep_vs = util.clone(entry.optionalDependencies || {});
    node.peer_dep_vs = util.clone(entry.peerDependencies || {});
    node.bundle_deps = bundled_names(entry.raw_entry || entry);
    node.bundle_list = bundled_list(entry.raw_entry || entry) || node.bundle_list;

    // the ranges a write back records are the declared ones, which the walk took from
    // the directory: they have to follow the manifest this node now describes
    if (node.declared) {
        node.declared.dep_vs = util.clone(node.dep_vs);
        node.declared.opt_dep_vs = util.clone(node.opt_dep_vs);
        node.declared.peer_dep_vs = util.clone(node.peer_dep_vs);
    }
}

/**
 * @description what a package's tarball ships inside itself (npm's
 *              `bundleDependencies`): the copies are on disk as soon as the parent is
 *              unpacked, and the walk skipped them — nothing resolved them, nothing
 *              fetched them. They are read here, once the downloads are done, so the
 *              lockfile describes them the way npm does (`inBundle`, no source)
 */
function read_bundled_copies(level_info, dir) {
    Object.keys(level_info.node_modules || {}).forEach(name => {
        const node = level_info.node_modules[name];
        // a link points at a directory of its own; everything else sits under this one
        const node_dir = node.local_path || node.workspace_path || path.join(dir, 'node_modules', name);

        // a package that ships copies of its own, or a copy that came inside another's
        // tarball: its manifest is the packer's, and the lockfile remembers the one
        // that was published
        if (node.in_bundle || node.bundle_deps)
            declared_from_lock(node);

        if (node.bundle_deps) {
            const read = read_module(node_dir, node);

            Object.keys(read).forEach(k => {
                // only the shipped copies: the resolved ones are already in the tree
                if (node.bundle_deps[k] && !node.node_modules[k])
                    node.node_modules[k] = read[k];
            });
        }

        read_bundled_copies(node, node_dir);
    });
}

/**
 * @description the prebuilt addons of the packages a frozen install just put in
 *              place: the lockfile cannot describe them (npm has no such field), so
 *              each installed package.json is asked. A linked package is included,
 *              the way the resolve path does it for local packages
 * @param only the paths that were installed just now (everything when omitted)
 */
function install_lock_binaries(lock, root, only) {
    const packages = lockfile.to_path_map(lock);
    const filter = only ? {} : null;
    let count = 0;

    if (only)
        only.forEach(p => filter[p] = true);

    Object.keys(packages).forEach(p => {
        const entry = packages[p];

        if (entry.target_only)
            return;

        if (filter && !filter[p])
            return;

        if (!fs.exists(path.join(root, p, 'package.json')))
            return;

        // a registry/git package is keyed by its path, a link by the same key
        // (`link:` is kept for the entries an older write back may have left)
        const task = mv_paths['lock:' + p] || mv_paths['link:' + p];
        if (!task)
            return;

        if (download_manifest_binary(task))
            count++;
    });

    if (count)
        install_log(`[install] prebuilt addons: ${count}`);
}

/**
 * @description fetch git entries at the commit the lockfile pins. npm clones with
 *              git too (its `revs.js` is a `git ls-remote`), and the recorded commit
 *              is the authority: a branch or a tag in there is refused by the sync
 *              check before this runs
 */
function install_git_entries(entries) {
    if (!entries.length)
        return;

    const tmp_root = path.join(tmp_dir(), 'fibjs-install-git');
    fs.mkdir(tmp_root, { recursive: true });

    entries.forEach(item => {
        const entry = item.entry;
        // `git+https://…` is npm's notation: git itself only knows the part after
        // the plus (pacote strips it the same way before shelling out)
        const repo = entry.resolved.split('#')[0].replace(/^git\+/, '');
        const sha = entry.resolved.split('#')[1];
        const dir = path.join(tmp_root, entry.name.replace(/[/@]/g, '_') + '-' + sha.slice(0, 8));

        try {
            if (fs.exists(dir))
                rmdir_recursive(dir);

            git_run(['init', '--quiet', dir]);
            git_run(['-C', dir, 'remote', 'add', 'origin', repo]);
            git_run(['-C', dir, 'fetch', '--quiet', '--depth', '1', 'origin', sha]);
            git_run(['-C', dir, 'checkout', '--quiet', 'FETCH_HEAD']);

            // the work tree is what gets installed: the history is of no use here,
            // and a copy (not a link) has to survive the temp directory
            rmdir_recursive(path.join(dir, '.git'));
            copy_dir(dir, item.dest);
            install_log('git:', entry.name, sha.slice(0, 8));
        } catch (e) {
            console.error(`[install] failed to fetch ${entry.name} from ${repo}#${sha.slice(0, 8)}`);
            console.error(`  ${e.message}`);
            process.exit(1);
        }
    });
}

/**
 * @description the frozen install has no level tree to walk, so the scripts run per
 *              installed package: nested dependencies first, the root last (the
 *              order npm uses), and `--ignore-scripts` still wins
 * @param only the paths that were installed just now (everything when omitted)
 */
function run_lock_lifecycle_scripts(lock, root, only) {
    const packages = lockfile.to_path_map(lock);
    const filter = only ? {} : null;

    if (only)
        only.forEach(p => filter[p] = true);

    const paths = Object.keys(packages)
        .filter(p => !packages[p].target_only)
        .filter(p => !filter || filter[p])
        .sort((a, b) => b.split('/').length - a.split('/').length);

    paths.forEach(p => {
        const dir = path.join(root, p);

        if (fs.exists(path.join(dir, 'package.json')))
            run_module_scripts(dir, packages[p], false);
    });

    run_module_scripts(root, {}, true, ['preinstall']);
}

// ---------------------- FROZEN INSTALL :end ------------------------- //

// ---------------------- WRITING THE LOCKFILE :start ------------------------- //

/**
 * @description one installed node as a lockfile entry
 */
function tree_entry(node, p) {
    const has = obj => obj && Object.keys(obj).length > 0;
    const local = node.local_package || node.workspace_package;

    // the spec a package declares, not the version that was chosen for it: npm's
    // `dependencies` field is the manifest, and a lockfile that recorded exact
    // versions there would pin everything a reader of it does afterwards
    const declared = (field, fallback) => {
        const src = (node.declared && node.declared[field]) || fallback;

        return has(src) ? src : undefined;
    };

    return {
        // npm writes `name` only where the directory and the package disagree, which is
        // how an alias is recorded (`node_modules/string-width-cjs` holding
        // `string-width`); `lock_entry()` decides whether to write the field
        name: node.real_name || node.name || lockfile.name_from_path(p),
        version: node.version,
        // a copy that came inside a parent's tarball has no source of its own
        in_bundle: node.in_bundle === true,
        bundle_dependencies: node.bundle_list,
        resolved: local ? undefined : (node.dist && node.dist.tarball),
        integrity: local ? undefined : (node.dist && node.dist.integrity),
        link: local ? true : undefined,
        local_path: local ? (node.local_path || node.workspace_path) : undefined,
        dev: node.dev_only === true,
        optional: node.optional_only === true,
        // npm writes the overlap of the two trees as `dev` plus `optional`, and the
        // flag on its own only where neither of those is set
        dev_optional: node.dev_optional === true && !node.dev_only && !node.optional_only,
        dependencies: declared('dep_vs', node.dep_vs),
        optionalDependencies: declared('opt_dep_vs', node.opt_dep_vs),
        peerDependencies: declared('peer_dep_vs', node.peer_dep_vs),
        bin: node.bin,
        os: node.os,
        cpu: node.cpu,
        libc: node.libc,
        // what npm projects from the manifest of the package itself: a fresh entry
        // without them makes npm rewrite the file
        license: node.license,
        engines: node.engines,
        funding: node.funding,
        has_install_script: node.has_install_script,
    };
}

/**
 * @description the installed tree as a path table. `dev` / `optional` come from
 *              `mark_reachability()`, the same answer npm's ideal tree gives and the
 *              one `npm ci --omit=dev` depends on
 */
function tree_to_path_map(rootsnap, root) {
    const paths = {};

    (function flatten(node, prefix) {
        Object.keys(node.node_modules || {}).forEach(name => {
            const child = node.node_modules[name];
            const p = prefix + 'node_modules/' + name;

            paths[p] = tree_entry(child, p);

            // what a link depends on is installed inside the directory it points at
            // — the same directory on disk, and the path npm records
            // (`packages/member/node_modules/x`, never
            // `node_modules/member/node_modules/x`, which a reader of the lockfile
            // would look for in vain)
            const target = child.local_package || child.workspace_package
                ? (child.local_path || child.workspace_path)
                : null;

            flatten(child, target ? lockfile.relative_target(root, target) + '/' : p + '/');
        });
    })(rootsnap, '');

    return paths;
}

/**
 * @description the root entry of a lockfile mirrors package.json. A lock-first
 *              install keeps every other entry exactly as npm wrote it, but this one
 *              has to follow package.json: a spec that moved while the tree did not
 *              (what `--save` does, for instance) is what a reader of the file looks
 *              for. Only the one entry is rewritten, and only when it differs
 */
function sync_lock_root(lock) {
    if (!lock || !lock.raw || !lock.raw.packages)
        return false;

    const want = lockfile.root_entry(rootsnap.pkgjson, lock.root);

    // written form, not field order: the manifest lists a dependency map in its own
    // order, the file in npm's, and writing a file whose text comes out identical is
    // a change a reader cannot see — the entry only moves when something differs
    if (lockfile.stringify_lock(lock.raw.packages[''], 2) === lockfile.stringify_lock(want, 2))
        return false;

    const file = path.join(process.cwd(), lock.filename);
    const indent = lockfile.detect_indent(fs.readTextFile(path.join(process.cwd(), 'package.json')));

    lock.raw.packages[''] = want;
    lockfile.write_lockfile(file, lock.raw, indent);
    install_log(`update: ${lock.filename} (the root entry follows package.json)`);

    return true;
}

/**
 * @description the flags a lockfile written before npm recorded them leaves out: the
 *              graph says which kind of edge reaches an entry, and without the flag
 *              `npm ci` reads a platform specific binary as a package every machine
 *              needs and refuses the whole lock (`EBADPLATFORM`). What the lockfile's
 *              own edges prove is what npm would write, so write it — a flag is only
 *              ever added, never taken away: the file belongs to npm, and a lockfile
 *              that already carries what the graph says is left alone
 * @returns the number of entries that gained a flag
 */
function sync_lock_flags(lock, packages, graph) {
    if (!lock || !lock.raw || !lock.raw.packages || !packages || !graph)
        return 0;

    let fixed = 0;

    Object.keys(packages).forEach(p => {
        const raw = lock.raw.packages[p];

        if (!raw)
            return;

        // npm's answer for the overlap of the two trees: `devOptional` only where
        // neither `dev` nor `optional` is set
        const want = [];

        if (graph.optional[p] === true)
            want.push('optional');
        if (graph.dev[p] === true)
            want.push('dev');
        if (graph.devOptional[p] === true && graph.dev[p] !== true && graph.optional[p] !== true)
            want.push('devOptional');

        let touched = false;

        want.forEach(k => {
            if (raw[k] !== true) {
                raw[k] = true;
                touched = true;
            }
        });

        if (touched)
            fixed++;
    });

    if (!fixed)
        return 0;

    const file = path.join(process.cwd(), lock.filename);
    const indent = lockfile.detect_indent(fs.readTextFile(path.join(process.cwd(), 'package.json')));

    lockfile.write_lockfile(file, lock.raw, indent);
    install_log(`update: ${lock.filename} (${fixed} ${fixed === 1 ? 'entry carries' : 'entries carry'} ` +
        'the edge kind the lockfile\'s own graph shows)');

    return fixed;
}

/**
 * @description the lockfile the installed tree describes, with the smallest change
 *              against the one that was read
 * @returns { file, lock, kept, indent } | null when there is no package.json
 */
function build_lockfile(lock, rootsnap) {
    const root = process.cwd();
    const pkgjson_file = path.join(root, 'package.json');

    if (!fs.exists(pkgjson_file))
        return null;

    const raw = fs.readTextFile(pkgjson_file);
    const paths = tree_to_path_map(rootsnap, root);
    const built = lockfile.to_lock(paths, rootsnap.pkgjson, {
        previous: lock,
        root: root,
        lockfileVersion: lock ? lock.lockfileVersion : 3,
        name: rootsnap.pkgjson.name,
        version: rootsnap.pkgjson.version,
    });

    return {
        // npm keeps updating the shrinkwrap it finds, and so does this
        file: path.join(root, lock ? lock.filename : 'package-lock.json'),
        lock: built.lock,
        kept: built.kept,
        indent: lockfile.detect_indent(raw),
    };
}

/**
 * @description write the lockfile the installed tree describes. npm's `--no-save`
 *              never writes the lockfile, and neither does an install of a single
 *              package that is not being recorded in package.json
 */
function write_back_lockfile(lock, rootsnap) {
    const built = build_lockfile(lock, rootsnap);

    if (!built)
        return null;

    lockfile.write_lockfile(built.file, built.lock, built.indent);

    // a registry entry without a source cannot be verified later: `--ci` refuses
    // it, so say so now instead of letting the next frozen install fail. It happens
    // when the tree holds a package no lockfile ever described (installed by hand,
    // or left over from one written before the lockfile existed)
    const unsourced = Object.keys(built.lock.packages).filter(p => {
        const e = built.lock.packages[p];

        // only a package that installs into node_modules needs a source: the entry of
        // the directory a link points at carries none by nature, and neither does a
        // copy a package ships inside its own tarball (npm records those with
        // `inBundle`)
        return p.indexOf('node_modules/') === 0 && !e.link && !e.inBundle && !e.resolved && !e.integrity &&
            e.version && /^\d/.test(String(e.version));
    });

    if (unsourced.length)
        console.warn(`[install] ${unsourced.length} lockfile entr${unsourced.length === 1 ? 'y has' : 'ies have'} no ` +
            `source (${unsourced.slice(0, 3).join(', ')}${unsourced.length > 3 ? ', …' : ''}): ` +
            '`--ci` will refuse them; `fibjs --install --update` fetches them again');

    install_log(`write: ${path.basename(built.file)}` +
        ` (${Object.keys(built.lock.packages).length - 1} packages)` +
        (built.kept.length ? `, ${built.kept.length} kept as npm wrote them` : ''));

    return built;
}

/**
 * @description the peer edges the lockfile cannot meet, in npm's words. npm refuses
 *              such a tree (`ERESOLVE`), and `npm ci` refuses the lockfile of one even
 *              when it was written with `--legacy-peer-deps`: a project npm would not
 *              install is named here instead of installed in silence. The check reads
 *              the lockfile, so it answers the same question on every path — the
 *              frozen install (fatal), and an install that resolves (a warning)
 * @returns the number of unmet edges
 */
function report_unmet_peers(lock, fatal) {
    const unmet = lockfile.check_peers(lock, { semver: semver });

    if (!unmet.length)
        return 0;

    const line = u => `peer ${u.name}@${u.spec} of ${u.path} is ` +
        (u.found === null ? 'not in the lockfile' : `the installed ${u.found}`);

    if (fatal) {
        console.error(`[install] ${unmet.length} peer dependenc${unmet.length === 1 ? 'y' : 'ies'} cannot be met:`);
        (ctx.verbose ? unmet : unmet.slice(0, 20)).forEach(u => console.error(`  ${line(u)}`));

        if (!ctx.verbose && unmet.length > 20)
            console.error(`  … and ${unmet.length - 20} more (--verbose lists them all)`);

        console.error('  npm refuses to install this tree (ERESOLVE); ' +
            'pass --no-strict-peer to install it anyway');
        process.exit(1);
    }

    unmet.slice(0, 5).forEach(u => console.warn(`[install] ${line(u)} — npm refuses this tree (ERESOLVE)`));

    if (unmet.length > 5)
        console.warn(`[install] … and ${unmet.length - 5} more unmet peer dependencies`);

    return unmet.length;
}

/**
 * @description the tree an install leaves has to be the tree the lockfile describes.
 *              A package's tarball carries copies of the dependencies it bundles
 *              (`bundleDependencies`), and one unpacked after them left the older copy
 *              on disk while the lockfile kept the version it resolved: the next
 *              install replaced it, and the frozen install does the same, but the run
 *              that wrote the lockfile left the two disagreeing. Every path is looked
 *              at, and the ones that disagree are put in place
 * @returns { checked, placed } — paths looked at, paths put in place
 */
function converge_with_lockfile(root) {
    const fresh = lockfile.read_lockfile(root);

    if (!fresh)
        return { checked: 0, placed: 0 };

    const result = install_from_lock(fresh, {
        registry: ctx.registry,
        strict_integrity: false,
        incremental: true,
        omit: ctx.omit,
    });

    const placed = Object.keys(result.installed).length;

    if (placed)
        install_log(`[install] ${placed} paths were not what ${fresh.filename} describes ` +
            '(a tarball carries the copies of the dependencies it bundles): put in place');

    return { checked: Object.keys(result.packages).length, placed: placed };
}

// ---------------------- WRITING THE LOCKFILE :end ------------------------- //

// ---------------------- CLI ARGUMENTS :start ------------------------- //

/**
 * @description the installer's own flags. `fibjs --install` passes everything that
 *              follows it straight to this script, so an unknown flag used to be
 *              dropped silently: `fibjs --install --frozen-lockfile` installed
 *              exactly as if the flag were not there. Flags are declared here
 *              instead — an unknown `-` argument fails the install, and a flag the
 *              plan has but the code has not (yet) says so out loud.
 *
 * `stage` marks a flag of plans/installer-lockfile-plan.md that is not implemented
 * yet: it is rejected together with its stage, never ignored.
 */
const ARG_SPECS = [
    { names: ['--help', '-h'], flag: 'help' },
    { names: ['--save', '-S'], flag: 'save' },
    { names: ['--save-dev', '-D'], flag: 'save_dev' },
    { names: ['--target'], flag: 'target', value: true },
    { names: ['--ignore-scripts'], flag: 'ignore_scripts' },

    // refresh what the ranges allow, like `npm update`: the lockfile follows, the
    // ranges in package.json do not
    { names: ['--update'], flag: 'update' },

    // npm compatible flags: a script written for npm should not start failing just
    // because fibjs reads the same arguments now. Mapped where fibjs has the same
    // concept, ignored with a notice where it has none.
    { names: ['--production'], flag: 'omit_dev', alias: '--omit=dev' },
    { names: ['--no-audit', '--no-fund', '--force', '--legacy-peer-deps', '--silent'], flag: 'npm_ignored' },

    // list every sync error instead of the first screenful
    { names: ['--verbose'], flag: 'verbose' },

    // planned, not implemented yet (rejected instead of silently ignored)
    { names: ['--lockfile-only'], flag: 'lockfile_only' },
    { names: ['--omit'], flag: 'omit', value: true, multiple: true },
    { names: ['--include'], flag: 'include', value: true, multiple: true },

    // the frozen install (P1)
    { names: ['--frozen-lockfile', '--ci'], flag: 'frozen' },
    { names: ['--no-package-lock'], flag: 'no_package_lock' },
    { names: ['--dry-run'], flag: 'dry_run' },
    { names: ['--no-strict-integrity'], flag: 'no_strict_integrity' },
    // a peer edge the lockfile cannot meet is npm's ERESOLVE: `npm ci` refuses such
    // a lockfile even when it was written with `--legacy-peer-deps`, and `--ci`
    // does the same unless this is passed
    { names: ['--no-strict-peer'], flag: 'no_strict_peer' },
];

// The single source of truth for the installer's command line: every entry of
// ARG_SPECS appears here, and nothing else does. `fibjs --install --help` prints
// this, and so does every argument error (plans/cli-help-convention.md).
function usage_text() {
    return [
        'Usage: fibjs --install [options] [package]',
        '',
        'Install the dependencies in the local node_modules folder. Without a',
        'package argument the project in the current directory is installed; with',
        'one, that package is installed, and nothing is recorded in package.json',
        'unless --save or --save-dev is given.',
        '',
        'Options:',
        '  -h, --help                  print this message',
        '  -S, --save                  save the installed package into dependencies',
        '  -D, --save-dev              save the installed package into devDependencies',
        '  --target <dir>              install into <dir> (its package.json is used)',
        '  --ignore-scripts            do not run lifecycle scripts',
        '  --update [package]          resolve again to the newest the ranges allow,',
        '                              and write the lockfile. package.json is not',
        '                              touched',
        '',
        'Lockfile:',
        '  --ci, --frozen-lockfile     install exactly what the lockfile says, and',
        '                              fail when it does not match package.json',
        '  --lockfile-only             write the lockfile without installing',
        '  --no-package-lock           ignore the lockfile',
        '  --no-strict-integrity       install lockfile entries that carry no',
        '                              integrity, instead of refusing them',
        '  --no-strict-peer            install a lockfile whose peer dependencies',
        '                              cannot be met, instead of refusing it',
        '  --dry-run                   report what would be written, write nothing',
        '  --verbose                   list every lockfile sync error, not just the',
        '                              first 20',
        '',
        'What lands in node_modules:',
        '  --omit=dev,optional,peer    leave those types out of node_modules (the',
        '                              lockfile still describes them)',
        '  --include=dev,optional,peer',
        '                              take a type back (overrides --omit)',
        '',
        'npm compatible (accepted so npm style scripts keep working):',
        '  --production                same as --omit=dev',
        '  --no-audit, --no-fund, --force, --legacy-peer-deps, --silent',
        '                              no equivalent in fibjs, ignored with a notice',
        '',
        'Notes:',
        '  devDependencies are installed by default, like npm, and NODE_ENV=production',
        '  means --omit=dev. package-lock.json is read and written the way npm does',
        '  it (npm-shrinkwrap.json wins over it). Resolving keeps the versions the',
        '  lockfile pins; --update is how the newest the ranges allow is asked for.',
        '  An unknown option is rejected, it used to be ignored silently.',
        '',
        'Run `fibjs --help` for the global options.',
    ].join('\n');
}

/**
 * @description parse the script arguments into flags and positionals. A flag that
 *              takes a value accepts both `--flag value` and `--flag=value`; `--`
 *              ends the flags, everything after it is a positional argument
 * @returns { flags, positionals, unknown, missing_value, pending, ignored }
 */
function parse_argv(argv) {
    const result = { flags: {}, positionals: [], unknown: null, missing_value: null, pending: null, ignored: [] };
    let positional_only = false;

    for (let i = 0; i < argv.length; i++) {
        const token = argv[i];

        if (positional_only) {
            result.positionals.push(token);
            continue;
        }

        if (token === '--') {
            positional_only = true;
            continue;
        }

        if (token === '' || token[0] !== '-') {
            result.positionals.push(token);
            continue;
        }

        const equal = token.indexOf('=');
        const name = equal > 0 ? token.slice(0, equal) : token;
        let value = equal > 0 ? token.slice(equal + 1) : undefined;

        const spec = ARG_SPECS.find(s => s.names.indexOf(name) > -1);
        if (!spec) {
            result.unknown = name;
            return result;
        }

        if (spec.value && value === undefined) {
            if (i + 1 >= argv.length) {
                result.missing_value = name;
                return result;
            }
            value = argv[++i];
        }

        if (spec.stage) {
            result.pending = { name: name, stage: spec.stage };
            return result;
        }

        const value_or_true = value === undefined ? true : value;

        if (spec.multiple) {
            // npm takes `--omit=dev --omit=optional`, and so does this
            if (result.flags[spec.flag] === undefined)
                result.flags[spec.flag] = [value_or_true];
            else
                result.flags[spec.flag].push(value_or_true);
        } else {
            result.flags[spec.flag] = value_or_true;
        }

        if (spec.flag === 'npm_ignored')
            result.ignored.push(name);
    }

    return result;
}

/**
 * @description what not to install, npm's `omit`: `dev` by default when NODE_ENV
 *              says production, `--omit=<kind>` adds one, `--include=<kind>` takes
 *              it back (the last word on a kind wins)
 * @returns { dev, optional, peer }
 */
function resolve_omit(flags, env) {
    const omit = { dev: env.NODE_ENV === 'production', optional: false, peer: false };
    const kinds = Object.keys(omit);

    function apply(list, values) {
        (values || []).forEach(value => {
            String(value).split(',').forEach(raw => {
                const kind = raw.trim();

                if (kind === '')
                    return;

                if (kinds.indexOf(kind) < 0) {
                    console.warn(`[install] unknown dependency type to ${list}: ${kind}` +
                        ` (${kinds.join(', ')})`);
                    return;
                }

                omit[kind] = list === 'omit';
            });
        });
    }

    apply('omit', flags.omit);
    apply('include', flags.include);

    return omit;
}

/**
 * @description report a bad command line the way a command line tool should: the
 *              reason, the usage, and a non zero exit code (the caller is a CI job
 *              as often as it is a human)
 */
function arg_error(message) {
    console.error(message);
    console.error(usage_text());
    process.exit(1);
}

/**
 * dependencies, devDependencies
 */
ctx.depk = ctx.dep_against_k = ''

const args = parse_argv(process.argv.slice(2));

// help wins over everything else, and does nothing but print
if (args.flags.help) {
    console.log(usage_text());
    process.exit(0);
}

if (args.unknown)
    arg_error(`[install] unknown option: ${args.unknown}`);

if (args.missing_value)
    arg_error(`[install] option ${args.missing_value} needs a value`);

if (args.pending)
    arg_error(`[install] option ${args.pending.name} is not implemented yet (planned for ${args.pending.stage})`);

args.ignored.forEach(n => console.warn(`[install] ${n} has no equivalent in fibjs, ignored`));

let need_add_newpkg_to_pkgjson = false
let pkgjson_path_specified = false;
let ignore_scripts = args.flags.ignore_scripts === true;

// how much of a sync failure to print (see print_sync_errors)
ctx.verbose = args.flags.verbose === true;

// the precedence has always been --save > --save-dev > --target
if (args.flags.save) {
    need_add_newpkg_to_pkgjson = true;
} else if (args.flags.save_dev) {
    need_add_newpkg_to_pkgjson = DEVDEPENDENCIES;
} else if (args.flags.target !== undefined) {
    const installTarget = args.flags.target;
    if (!installTarget) throw new Error('[install] no path specified');
    if (!path.isAbsolute(installTarget)) {
        process.chdir(path.join(process.cwd(), installTarget));
    } else {
        process.chdir(installTarget);
    }
    pkgjson_path_specified = true;
} else {
    ctx.depk = DEPENDENCIES;
}

const rootsnap = get_root_snapshot();

if (!pkgjson_path_specified) {
    // when specified new_pkgname, install it only
    ctx.new_pkgname = args.positionals[0];
}

// the registry and the credentials to fetch from: `.npmrc` first (npm's own way of
// configuring both), then the `registry` field of package.json, then the default
ctx.npmrc = lockfile.read_npmrc(process.cwd());
ctx.registry = ctx.npmrc.registry ? normalize_registry_origin(ctx.npmrc.registry) : rootsnap.registry;
ctx.tls = tls_options(ctx.npmrc);
ctx.client = ctx.tls ? new http.Client(ctx.tls) : null;

// what to install and what to leave out (npm's `omit` / `include`, NODE_ENV). The
// frozen and lock-first paths filter by it too, so it is decided before them
ctx.omit = resolve_omit(args.flags, process.env);
ctx.install_dev = !ctx.omit.dev;

// `--update` refreshes what the ranges allow (npm's `update`): the whole project, or
// the package that was named (and what that one needs). The named package is not an
// install of its own: the tree stays whole and the lockfile is written
ctx.update = args.flags.update
    ? { all: !ctx.new_pkgname, name: ctx.new_pkgname, matched: false }
    : null;

if (ctx.update)
    ctx.new_pkgname = null;

if (args.flags.frozen && ctx.update)
    arg_error('[install] --update and --ci contradict each other: --ci installs what the lockfile says');

// the lockfile, unless the caller asked to ignore it
const lock = args.flags.no_package_lock ? null : lockfile.read_lockfile(process.cwd());

if (lock && !lockfile.supported_version(lock.lockfileVersion)) {
    console.error(`[install] ${lock.filename}: lockfileVersion ${lock.lockfileVersion} is not supported`);
    process.exit(1);
}

// frozen install: the lockfile decides everything, nothing is resolved
if (args.flags.frozen) {
    if (ctx.new_pkgname)
        arg_error('[install] --ci installs what the lockfile says: it takes no package argument');

    // npm refuses to install where there is no package.json, and emptying
    // node_modules of a directory that is not a project would be rude
    if (!fs.exists(path.join(process.cwd(), 'package.json'))) {
        console.error(`[install] --ci needs a package.json in ${process.cwd()}`);
        process.exit(1);
    }

    // npm forces `packageLock: true` for its `ci`; saying so is more useful than
    // ignoring the flag
    if (args.flags.no_package_lock)
        arg_error('[install] --no-package-lock cannot be combined with --ci');

    if (!lock) {
        console.error('[install] --ci needs a package-lock.json or npm-shrinkwrap.json');
        console.error('  run `fibjs --install` once to create one');
        process.exit(1);
    }

    const sync = check_project_sync(rootsnap, lock);

    sync.warnings.forEach(w => console.warn(`[install] ${describe_sync_error(w)}`));

    if (ctx.omit.peer)
        console.warn('[install] --omit=peer cannot be honoured from a lockfile: ' +
            'neither npm nor this writes a peer mark into it');

    if (!sync.ok) {
        print_sync_errors(lock, sync);
        process.exit(1);
    }

    install_log(`[install] ${lock.filename} is in sync (lockfileVersion ${lock.lockfileVersion})`);

    // npm ci refuses a lockfile whose peers cannot be met — before it touches the
    // disk, and `--legacy-peer-deps` is how npm is told to install it anyway
    report_unmet_peers(lock, !args.flags.no_strict_peer);

    if (args.flags.dry_run) {
        install_log('[install] --dry-run: nothing was written');
        process.exit(0);
    }

    // the root's preinstall runs before the tree is touched, and a failure stops here
    if (!ignore_scripts && !process.env.FIBJS_IGNORE_SCRIPTS)
        run_root_preinstall();

    // the check passed, so this is where node_modules may be emptied (npm ci does
    // the same, and a failed check never touches the disk)
    clear_node_modules(process.cwd());

    // the members are links at the root, recreated after the directory was emptied
    link_workspace_packages();

    install_from_lock(lock, {
        registry: ctx.registry,
        strict_integrity: !args.flags.no_strict_integrity,
        omit: ctx.omit,
    });

    if (!ignore_scripts && !process.env.FIBJS_IGNORE_SCRIPTS) {
        install_log('\nrun lifecycle scripts...');
        run_lock_lifecycle_scripts(lock, process.cwd());
    }

    install_log('\n[install] frozen install complete');
    process.exit(0);
}

// lock-first: a lockfile that already covers package.json is installed as it is.
// The shape stays npm's, nothing is resolved and nothing is rewritten — unless
// `--update` was asked for, which is the opposite of that
if (lock && !ctx.new_pkgname && !args.flags.update) {
    const sync = check_project_sync(rootsnap, lock);

    sync.warnings.forEach(w => console.warn(`[install] ${describe_sync_error(w)}`));

    if (ctx.omit.peer)
        console.warn('[install] --omit=peer cannot be honoured from a lockfile: ' +
            'neither npm nor this writes a peer mark into it');

    if (sync.ok) {
        install_log(`[install] ${lock.filename} covers package.json (lockfileVersion ${lock.lockfileVersion})`);

        // the tree this installs is the one the lockfile describes, so the answer is
        // the same as the frozen install's: say it rather than install it in silence
        report_unmet_peers(lock, false);

        if (args.flags.lockfile_only || args.flags.dry_run) {
            install_log('[install] the lockfile is already what package.json asks for, nothing to write');
            process.exit(0);
        }

        // preinstall before anything is installed, like npm
        if (!ignore_scripts && !process.env.FIBJS_IGNORE_SCRIPTS)
            run_root_preinstall();

        // the members are links at the root; the lockfile describes them too, this
        // only saves the install from creating what is already there
        link_workspace_packages();

        const result = install_from_lock(lock, {
            registry: ctx.registry,
            strict_integrity: false,
            incremental: true,
            omit: ctx.omit,
        });

        if (!ignore_scripts && !process.env.FIBJS_IGNORE_SCRIPTS) {
            install_log('\nrun lifecycle scripts...');
            run_lock_lifecycle_scripts(lock, process.cwd(), result.installed);
        }

        sync_lock_root(lock);
        sync_lock_flags(lock, result.packages, result.graph);
        remove_hidden_lock(process.cwd());
        process.exit(0);
    }

    install_log(`[install] ${lock.filename} does not cover package.json: resolving, then updating it`);
}

// process_new_pkgname
(() => {
    const new_pkginstall_typeinfo = helpers_pkg.parse_pkg_installname(ctx.new_pkgname || '');
    const dep_type = need_add_newpkg_to_pkgjson === DEVDEPENDENCIES ? 'dev_dep_vs' : 'dep_vs';

    switch (new_pkginstall_typeinfo.type) {
        case 'git':
            if (new_pkginstall_typeinfo.from_http)
                ctx.new_pkgname = `${new_pkginstall_typeinfo.git_path}#${new_pkginstall_typeinfo.git_reference}`;

            if (ctx.new_pkgname && rootsnap[dep_type][ctx.new_pkgname] === undefined)
                rootsnap[dep_type][ctx.new_pkgname] = '*';
            break
        case 'local':
            const localPkgPath = path.join(new_pkginstall_typeinfo.local_path, 'package.json');
            if (!fs.exists(localPkgPath))
                throw new Error(`[local] package.json not found at ${new_pkginstall_typeinfo.local_path}`);

            const localPkg = JSON.parse(fs.readTextFile(localPkgPath));
            const pkgName = localPkg.name || path.basename(new_pkginstall_typeinfo.local_path);

            rootsnap.node_modules[pkgName] = {
                version: localPkg.version || '0.0.0',
                dep_vs: util.extend({}, localPkg.dependencies),
                dev_dep_vs: util.extend({}, localPkg.devDependencies),
                opt_dep_vs: util.extend({}, localPkg.optionalDependencies),
                peer_dep_vs: read_peer_dep_vs(localPkg),
                bin: localPkg.bin,
                parent: rootsnap,
                local_package: true,
                local_path: new_pkginstall_typeinfo.local_path,
                pkg_install_typeinfo: new_pkginstall_typeinfo,
                new_module: true,
                node_modules: read_module(new_pkginstall_typeinfo.local_path, rootsnap.node_modules[pkgName])
            };

            // create symlink for local package
            create_local_symlinks(process.cwd(), [{
                name: pkgName,
                srcPath: new_pkginstall_typeinfo.local_path
            }]);

            // mark as symlinked to prevent duplicate creation
            rootsnap.node_modules[pkgName]._symlinked = true;

            // use the package name as the dep key (not the CLI path), and remember
            // the pair: `update_pkgjson` needs the name to find the version, and the
            // path is what gets recorded as the spec
            if (ctx.new_pkgname) {
                name_maps_installation2pkg[ctx.new_pkgname] = pkgName;
                rootsnap[dep_type][pkgName] = '*';
            }
            break
        case 'registry':
            if (new_pkginstall_typeinfo.registry_semver) {
                rootsnap[dep_type][new_pkginstall_typeinfo.package_name] = new_pkginstall_typeinfo.registry_semver

                ctx.new_pkgname = new_pkginstall_typeinfo.registry_pkg_path
            }

            if (ctx.new_pkgname && rootsnap[dep_type][ctx.new_pkgname] === undefined) rootsnap[dep_type][ctx.new_pkgname] = '*';
            break
    }

    if (ctx.new_pkgname) {
        // a local package is registered under its own name, not under the path the
        // caller typed: the name map is what tells the two apart
        const narrowed = name_maps_installation2pkg[ctx.new_pkgname] || ctx.new_pkgname;

        // `--update <pkg>` refreshes that package inside the whole project (npm's
        // `update` reifies everything), so the tree is not narrowed for it
        if (!ctx.update)
            rootsnap[dep_type] = { [narrowed]: rootsnap[dep_type][narrowed] };
    }
})();

// the tree the lockfile describes is where resolving starts from: a version it pins
// is kept while it satisfies the spec, so that gaining a dependency does not upgrade
// everything else (npm does the same; only `--update` asks for the newest the ranges
// allow). With no node_modules to read, the lockfile is the only record of what the
// project resolved to
ctx.lock_packages = lock ? lockfile.to_path_map(lock) : null;

// the dev subtree is always resolved — a lockfile describes the whole graph (N14)
// and one without the dev entries cannot be used by `npm ci` — while whether those
// packages land on disk is `ctx.omit`'s business
walkthrough_deps(rootsnap, true);
move_up(rootsnap);

// hoisting can take a package away from the copy it was resolved against: the edge is
// checked from where it ended up, and a copy is nested when nothing above can serve it
ensure_declared_deps_visible(rootsnap);

mark_reachability(rootsnap);

// a name that was never asked for is a silent no-op otherwise
if (ctx.update && !ctx.update.all && !ctx.update.matched)
    console.warn(`[install] --update: '${ctx.update.name}' is not a dependency of this project, nothing was refreshed`);

if (args.flags.lockfile_only) {
    write_back_lockfile(lock, rootsnap);
    install_log('[install] --lockfile-only: the lockfile is written, nothing is installed');

    const written = lockfile.read_lockfile(process.cwd());

    if (written)
        report_unmet_peers(written, false);

    process.exit(0);
}

if (args.flags.dry_run) {
    const built = build_lockfile(lock, rootsnap);

    install_log(built
        ? `[install] --dry-run: would write ${path.basename(built.file)}` +
        ` (${Object.keys(built.lock.packages).length - 1} packages)`
        : '[install] --dry-run: nothing to write');
    process.exit(0);
}

// preinstall before anything is installed, like npm
if (!ignore_scripts && !process.env.FIBJS_IGNORE_SCRIPTS)
    run_root_preinstall();

// the workspace members are links at the root, and they have to exist before the
// tree is written (the snapshot carries them; this is what makes them resolvable)
link_workspace_packages();

generate_mv_paths(rootsnap, process.cwd());
download_module();

// the copies the packages ship inside themselves are on disk now: the tree learns
// about them before it is written out (npm describes them with `inBundle`), and the
// marking is done again so that an entry the parent reaches through an optional edge
// is not written as one every machine needs
read_bundled_copies(rootsnap, process.cwd());
mark_reachability(rootsnap);

// create symlinks for local packages found during dependency walking
(function create_local_package_symlinks(level_info, base_path) {
    for (let k in level_info.node_modules) {
        const mod = level_info.node_modules[k];

        // a dependency `omit` leaves out is not installed, and a link is an install
        // like any other
        if (omitted_by(mod))
            continue;

        if (mod.new_module && mod.local_package && !mod._symlinked) {
            create_local_symlinks(base_path, [{
                name: k,
                srcPath: mod.local_path || mod.pkg_install_typeinfo.local_path
            }]);
            mod._symlinked = true;
        }
        create_local_package_symlinks(mod, path.join(base_path, 'node_modules', k));
    }
})(rootsnap, process.cwd());

// the lockfile this install describes. npm writes it while it reifies (before the
// scripts), and never with `--no-save`. An install of a single package is the other
// exception: it resolves — and therefore installs — only that package, so the tree
// it builds is not the whole project and a lockfile written from it would be missing
// everything else (npm re-resolves the whole project here instead)
if (!args.flags.no_package_lock && !ctx.new_pkgname) {
    write_back_lockfile(lock, rootsnap);

    // what one run leaves has to be what the lockfile it just wrote describes: a
    // path whose copy on disk is not that version is put in place now, instead of
    // being left to the next install (or to `--ci`) to repair
    converge_with_lockfile(process.cwd());

    const written = lockfile.read_lockfile(process.cwd());

    if (written)
        report_unmet_peers(written, false);
} else if (ctx.new_pkgname && !args.flags.no_package_lock) {
    console.warn('[install] the lockfile was left alone: run `fibjs --install` to bring it back in sync');
}

remove_hidden_lock(process.cwd());

// run lifecycle scripts (unless --ignore-scripts)
if (!ignore_scripts && !process.env.FIBJS_IGNORE_SCRIPTS) {
    install_log('\nrun lifecycle scripts...');
    run_lifecycle_scripts(rootsnap, process.cwd(), true);
}

dump_snap();
update_pkgjson(rootsnap);
