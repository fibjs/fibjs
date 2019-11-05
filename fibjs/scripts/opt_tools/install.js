const url = require('url');
const util = require('util');
const coroutine = require('coroutine');
const fs = require('fs');
const path = require('path');
const ssl = require('ssl');
const http = require('http');
const zlib = require('zlib');
const zip = require('zip');
const hash = require('hash');
const semver = require('internal/helpers/semver');
const untar = require('internal/helpers/untar');

const helpers_pkg = require('internal/helpers/package');
const helpers_fs = require('internal/helpers/fs');
const helpers_string = require('internal/helpers/string');
const CST = require('internal/constant');

const DEPENDENCIES = 'dependencies';
const DEVDEPENDENCIES = 'devDependencies';
const DEP_KEY_TUPLE = [DEPENDENCIES, DEVDEPENDENCIES];
const SEP = path.sep;

const install_log = process.env.FIBJS_SILENT_INSALL ? () => undefined : console.log.bind(console)

ssl.loadRootCerts();
const hc = new http.Client();
if (process.env.fibjs_install_http_proxy) {
    hc.proxyAgent = process.env.fibjs_install_http_proxy;
    console.log(`[install] http client using proxyAgent: ${hc.proxyAgent}`);
}

const json_format = (function () {
    let p = [],
        indentConfig = {
            tab: {
                char: '\t',
                size: 1
            },
            space: {
                char: ' ',
                size: 4
            }
        },
        configDefault = {
            type: 'tab'
        },
        push = function (m) {
            return '\\' + p.push(m) + '\\';
        },
        pop = function (m, i) {
            return p[i - 1]
        },
        tabs = function (count, indentType) {
            return new Array(count + 1).join(indentType);
        };

    function JSONFormat(json, indentType) {
        p = [];
        let out = "", indent = 0;

        // Extract backslashes and strings
        json = json
            .replace(/\\./g, push)
            .replace(/(".*?"|'.*?')/g, push)
            .replace(/\s+/, '');

        // Indent and insert newlines
        for (let i = 0; i < json.length; i++) {
            const c = json.charAt(i);

            switch (c) {
                case '{':
                case '[':
                    out += c + "\n" + tabs(++indent, indentType);
                    break;
                case '}':
                case ']':
                    out += "\n" + tabs(--indent, indentType) + c;
                    break;
                case ',':
                    out += ",\n" + tabs(indent, indentType);
                    break;
                case ':':
                    out += ": ";
                    break;
                default:
                    out += c;
                    break;
            }
        }

        // Strip whitespace from numeric arrays and put backslashes 
        // and strings back in
        out = out
            .replace(/\[[\d,\s]+?\]/g, function (m) {
                return m.replace(/\s/g, '');
            })
            .replace(/\\(\d+)\\/g, pop) // strings
            .replace(/\\(\d+)\\/g, pop); // backslashes in strings

        return out;
    };

    return function (json, config) {
        config = config || configDefault;
        const indent = indentConfig[config.type];

        if (indent == null) {
            throw new Error('Unrecognized indent type: "' + config.type + '"');
        }
        const indentType = new Array((config.size || indent.size) + 1).join(indent.char);
        return JSONFormat(JSON.stringify(json), indentType);
    }
})();

// ---------------------- UTILS :start ------------------------- //
/**
 * @description read modules from `node_modules` to generate existed modules snapshot
 * @sample 
    {
        name: "module",
        version: "1.0.0",
        dep_vs: { "module-a": "1.0.0" },
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
        const files = fs.readdir(path.join(p, 'node_modules'));

        files.forEach(n => {
            const f = path.join(p, 'node_modules', n, 'package.json');
            if (fs.exists(f)) {
                const minfo = JSON.parse(fs.readTextFile(f));

                const dep_vs = util.clone(minfo.dependencies || {});
                const dev_dep_vs = util.clone(minfo.devDependencies || {});

                modules[n] = {
                    version: minfo.version,
                    dep_vs: dep_vs,
                    dev_dep_vs: dev_dep_vs,
                    parent: parent
                };

                modules[n].node_modules = read_module(path.join(p, 'node_modules', n), modules[n]);
            }
        });
    }

    return modules;
}

function http_get(u, { quit_if_error = true } = {}) {
    let cnt = 0;

    while (cnt++ < 3)
        try {
            const res = hc.get(u);
            if (!res.body)
                throw new Error(`[http_get] get nothing from url ${u}`);

            return res
        } catch (e) {
            console.error(e);
            console.warn(`[http_get] retry ${cnt}: ${u}`);
        }

    console.error("download error.", u);
    if (quit_if_error)
        process.exit(-1);
}

function json_parse_response(http_response) {
    return JSON.parse(http_response.body.readAll().toString())
}

function find_version(m, v, parent) {
    while (parent !== undefined) {
        const info = parent.node_modules[m];
        if (info !== undefined)
            return semver.satisfies(info.version, v);

        parent = parent.parent;
    }
}

function normalize_registry_origin(registry) {
    const urlObj = url.parse(registry)

    const protocol = urlObj.protocol || 'https:'
    const hostname = urlObj.hostname || 'registry.npmjs.org'

    return `${protocol}//${hostname}/`
}
// ---------------------- UTILS :end ------------------------- //

const pkg_registrytype_module_infos = {};
const pkg_githubtype_module_infos = {};
function fetch_leveled_module_info(m, v, parent) {
    const pkg_install_typeinfo = helpers_pkg.parse_pkg_installname(m, v);

    switch (pkg_install_typeinfo.type) {
        case 'registry':
            let info = pkg_registrytype_module_infos[m];
            if (info === undefined) {
                const registry_url = `${rootsnap.registry}${encodeURIComponent(m)}`
                install_log('fetch metadata:', m, "=>", registry_url);
                pkg_registrytype_module_infos[m] = info = json_parse_response(http_get(registry_url));
            }
            /* registry: match version :start */
            const all_vers = Object.keys(info.versions);
            let filtered_vers = [];

            switch (v) {
                case 'latest':
                    filtered_vers = all_vers.sort(semver.rcompare);
                    break
                default:
                    all_vers.forEach(ver => semver.satisfies(ver, v) ? filtered_vers.push(ver) : undefined)

                    filtered_vers.sort(semver.rcompare);
                    break
            }

            matched_ver = filtered_vers[0];

            if (!matched_ver)
                throw new Error(`[package/${info.name}]no matched for pattern '${v}'`)
            /* registry: match version :end */

            const minfo = info.versions[matched_ver];

            const dep_vs = util.clone(minfo.dependencies || {});
            const dev_dep_vs = util.clone(minfo.devDependencies || {});

            return {
                name: minfo.name,
                version: minfo.version,
                dep_vs: dep_vs,
                dev_dep_vs: dev_dep_vs,
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

            return {
                name: pkgjson_info.name,
                version: pkgjson_info.version,
                dep_vs: util.extend({}, pkgjson_info.dependencies),
                dev_dep_vs: util.extend({}, pkgjson_info.devDependencies),
                node_modules: {},
                parent: parent,
                dist: null,
                pkg_install_typeinfo,
                new_module: true
            }
        default:
            throw new Error(`unsupported package type '${pkg_install_typeinfo.type}' !`)
    }
}

function get_root_snapshot() {
    const pwd = process.cwd();
    const root_is_new = true;

    let pkgjson = {};
    try {
        pkgjson = JSON.parse(
            fs.readTextFile(path.join(pwd, 'package.json'))
        );
        root_is_new = false;
    } catch (e) {
    }
    pkgjson.name = pkgjson.name || (path.basename(pwd, path.extname(pwd))).toLowerCase();
    pkgjson.version = pkgjson.version || '1.0.0';

    const dep_vs = util.extend({}, pkgjson.dependencies);
    const dev_dep_vs = util.extend({}, pkgjson.devDependencies);

    const registry = normalize_registry_origin(pkgjson.registry || 'https://registry.npmjs.org/');

    const m = {
        name: pkgjson.name,
        version: pkgjson.version,
        dep_vs: dep_vs,
        dev_dep_vs: dev_dep_vs,
        new_module: true,
        registry: registry,
        root_is_new: root_is_new,
        pkgjson: pkgjson
    };
    m.node_modules = read_module(pwd, m);

    return m;
}

const name_maps_installation2pkg = {}
/**
 * @description walk throught to generate dep_vs/dev_dep_vs information recursively
 */
function walkthrough_deps(level_info, need_dev_deps = false) {
    if (level_info.new_module) {
        ;[
            ['dep_vs', 'dependencies']
        ].concat(
            need_dev_deps ? [['dev_dep_vs', 'devDependencies']] : []
        ).forEach(([dep_type]) => {
            coroutine.parallel(
                Object.keys(level_info[dep_type]),
                dname => {
                    const _deps = level_info[dep_type];

                    let v = _deps[dname];
                    let child_level_info = level_info.node_modules[dname];

                    if (child_level_info === undefined || !semver.satisfies(child_level_info.version, v)) {
                        if (!find_version(dname, v, level_info))
                            child_level_info = level_info.node_modules[dname] = fetch_leveled_module_info(dname, v, level_info);

                        /**
                         * @todo deal with special installation name, such as 'fibjs/fib-graphql'
                         */
                        if (child_level_info && child_level_info.name !== dname) {
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
                            dname = pkg_name
                        }
                    }

                    if (child_level_info) _deps[dname] = child_level_info.version;
                },
                CST.DEFAULT_FIBERS
            );
        });

        for (let k in level_info.node_modules) {
            walkthrough_deps(
                level_info.node_modules[k],
                // only ask if need_dev_deps for installation's source root
                false
            );
        }
    }
}

function move_up(level_info, parent) {
    if (level_info.new_module) {
        for (let k in level_info.node_modules)
            move_up(level_info.node_modules[k], level_info);

        if (parent !== undefined)
            for (let k in level_info.node_modules) {
                const m = level_info.node_modules[k];
                if (m.new_module) {
                    const m1 = parent.node_modules[k];
                    if (m1 === undefined || m1.version === m.version) {
                        parent.node_modules[k] = m;
                        delete level_info.node_modules[k];
                    }
                }
            }
    }
}

const mv_paths = {};
function generate_mv_paths(level_info, parent_p) {
    if (level_info.new_module) {
        for (let k in level_info.node_modules) {
            const lmod = level_info.node_modules[k];
            const mp = path.join(parent_p, 'node_modules', lmod.name);

            if (lmod.new_module) {
                const mv = k + '@' + lmod.version;

                let ps = mv_paths[mv];
                if (ps === undefined) {
                    switch (lmod.pkg_install_typeinfo.type) {
                        case 'registry':
                            ps = {
                                pkg_install_typeinfo: lmod.pkg_install_typeinfo,
                                dist: lmod.dist,
                                path: [mp]
                            };
                            break
                        case 'git':
                            ps = {
                                pkg_install_typeinfo: lmod.pkg_install_typeinfo,
                                dist: null,
                                path: [mp]
                            };
                            break
                    }

                    mv_paths[mv] = ps;
                } else
                    ps.path.push(mp);
            }

            generate_mv_paths(lmod, mp);
        }
    }
}

function download_module() {
    coroutine.parallel(
        Object.keys(mv_paths),
        mkey => {
            const mvm = mv_paths[mkey];
            let archive_root_name;

            const registry_i_tuples = [];
            const git_i_tuples = [];
            const existed_dirs = {};
            function ensure_dir(dirname) {
                if (process.platform === 'win32')
                    if (existed_dirs[dirname]) return;

                existed_dirs[dirname] = true;
                helpers_fs.mkdirp(dirname);
            }

            switch (mvm.pkg_install_typeinfo.type) {
                case 'registry':
                    const r = http_get(mvm.dist.tarball);
                    if (r.statusCode !== 200) {
                        console.error('download error::', mvm.dist.tarball);
                        process.exit();
                    }

                    const tgz = r.data;

                    if (hash.sha1(tgz).digest().hex() !== mvm.dist.shasum) {
                        console.error('shasum:', mvm.dist.tarball);
                        process.exit();
                    }

                    let t;
                    if (tgz[0] === 0x1f && tgz[1] === 0x8b)
                        t = zlib.gunzip(tgz);
                    else
                        t = tgz;

                    const untar_files = untar(t);

                    // most package from registry is archived with root directory `package`
                    archive_root_name = `package`;
                    if (untar_files[0].filename.indexOf(archive_root_name) !== 0) {
                        archive_root_name = helpers_string.ensure_unsuffx(
                            helpers_string.find_least_common_str(untar_files[0].filename, untar_files[1].filename)
                        )
                    }

                    mvm.path.forEach(bp => {
                        coroutine.parallel(untar_files, (file) => {
                            const relpath = file.filename.slice(archive_root_name.length);

                            if (!relpath) return;

                            const tpath = path.join(bp, relpath);
                            helpers_fs.mkdirp(path.dirname(tpath));

                            fs.writeFile(tpath, file.fileData);
                        })
                    });

                    install_log('extract:', mvm.dist.tarball);
                    break
                case 'git':
                    const git_archive_url = helpers_pkg.get_git_archive_url(mvm.pkg_install_typeinfo);
                    const git_r = http_get(git_archive_url);

                    if (git_r.statusCode !== 200) {
                        console.error('download error::', mvm.dist.tarball);
                        process.exit();
                    }
                    const git_zip_file = zip.open(git_r.body.readAll());
                    const namelist = git_zip_file.namelist();

                    archive_root_name = `${mvm.pkg_install_typeinfo.git_basename}-${mvm.pkg_install_typeinfo.git_reference}`;
                    if (namelist[0].indexOf(archive_root_name) !== 0) {
                        archive_root_name = helpers_string.ensure_unsuffx(
                            helpers_string.find_least_common_str(namelist[0], namelist[1])
                        )
                    }

                    mvm.path.forEach(bp => {
                        namelist.forEach((member) => {
                            const relpath = member.slice(archive_root_name.length);
                            if (!relpath) return;

                            const tpath = path.join(bp, relpath);

                            // skip directory
                            if (tpath.endsWith(SEP)) return;
                            ensure_dir(path.dirname(tpath));

                            git_i_tuples.push([
                                member, tpath
                            ]);
                        })
                    });

                    coroutine.parallel(git_i_tuples, ([member, tpath]) => {
                        git_zip_file.extract(member, tpath);
                    });

                    install_log('extract:', git_archive_url);
                    break
            }
        },
        CST.DEFAULT_FIBERS
    );
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

    if (!pkgjson[ctx.depk][real_pkgname]) {
        if (special_source_installation_name !== real_pkgname) {
            pkgjson[ctx.depk][real_pkgname] = special_source_installation_name;
        } else {
            pkgjson[ctx.depk][real_pkgname] = `^${rootsnap.node_modules[real_pkgname].version}`;
        }
    }

    fs.writeFile('package.json', json_format(pkgjson, {
        type: 'space',
        size: 2
    }));
}

const ctx = {};

/**
 * dependencies, devDependencies
 */
ctx.depk = ctx.dep_against_k = ''

let need_add_newpkg_to_pkgjson = false
if (process.argv.indexOf('--save', 2) > -1 || process.argv.indexOf('-S', 2) > -1) {
    need_add_newpkg_to_pkgjson = true;
} else if (process.argv.indexOf('--save-dev', 2) > -1 || process.argv.indexOf('-D', 2) > -1) {
    need_add_newpkg_to_pkgjson = DEVDEPENDENCIES;
} else {
    ctx.depk = DEPENDENCIES;
}

const rootsnap = get_root_snapshot();
// when specified new_pkgname, install it only
ctx.new_pkgname = (process.argv).filter(x => !x.startsWith('-'))[1];

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
        case 'registry':
            if (new_pkginstall_typeinfo.registry_semver) {
                rootsnap[dep_type][new_pkginstall_typeinfo.package_name] = new_pkginstall_typeinfo.registry_semver

                ctx.new_pkgname = new_pkginstall_typeinfo.registry_pkg_path
            }


            if (ctx.new_pkgname && rootsnap[dep_type][ctx.new_pkgname] === undefined) rootsnap[dep_type][ctx.new_pkgname] = '*';
            break
    }
})();

walkthrough_deps(rootsnap, need_add_newpkg_to_pkgjson === DEVDEPENDENCIES);
move_up(rootsnap);
generate_mv_paths(rootsnap, process.cwd());
download_module();

dump_snap();
update_pkgjson(rootsnap);
