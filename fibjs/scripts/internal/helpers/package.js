const url = require('url');
const path = require('path');

const CST = require('internal/constant');
const helpers_string = require('internal/helpers/string')
const semver = require('internal/helpers/semver');
const SPECIAL_PKG_PATTERN = /^\@?([a-z][a-z0-9\-]*)?(\/[a-z][a-z0-9\-]*)?(\#[\w]+)?/

exports.is_special_installname = function (pkg_name = '') {
    if (!pkg_name) return false;
    if (pkg_name === '*') return false;

    if (pkg_name.indexOf('http://') === 0) return true;
    if (pkg_name.indexOf('https://') === 0) return true;

    if (pkg_name.match(SPECIAL_PKG_PATTERN)) return true;
    
    return false;
}

const GIT_PATTERN = /^([a-zA-Z0-9_.~-]+\/([a-zA-Z0-9_.~-]+)(\.git)?)(\#(.+))?$/
const MODULE_NAME_PATTERN = /^[a-z][a-z0-9_.~-]*$/;

/**
 * @description npm's hosted git shorthands, and the host each one names. A spec
 *              written with one of these prefixes is a git spec, and reading it as a
 *              registry range is how `github:user/repo#v1` came to fail with "no
 *              published version satisfies" — the range was the whole spec string
 *
 *              `git+ssh:`, `git+https:` and bare `git://` are the transport forms; they
 *              carry their own host, so they are recognised but not rewritten
 */
const HOSTED_GIT = {
    github: 'https://github.com',
    gitlab: 'https://gitlab.com',
    bitbucket: 'https://bitbucket.org',
    gist: 'https://gist.github.com',
};

const GIT_TRANSPORT_PATTERN = /^(git\+ssh:|git\+https:|git\+http:|git:\/\/|https?:\/\/.*\.git|ssh:\/\/)/;

/**
 * @description split a hosted git shorthand into its host, repository path and the
 *              reference it asks for
 * @sample 'github:jonschlinkert/is-number#7.0.0' →
 *         { origin: 'https://github.com', path: 'jonschlinkert/is-number', reference: '7.0.0' }
 */
function split_hosted_git(spec) {
    const m = /^(github|gitlab|bitbucket|gist):(.+)$/.exec(spec);

    if (!m)
        return null;

    let rest = m[2];
    let reference = '';

    const hash = rest.lastIndexOf('#');

    if (hash > 0) {
        reference = rest.slice(hash + 1);
        rest = rest.slice(0, hash);
    }

    rest = rest.replace(/\.git$/, '');

    // `github:a/b#semver:^1` asks for the newest tag in that range, which needs the
    // registry-style range kept whole rather than parsed as a ref name
    if (rest.indexOf('/') < 0)
        return null;

    return {
        origin: HOSTED_GIT[m[1]],
        path: rest,
        reference: reference || 'master',
        basename: rest.slice(rest.lastIndexOf('/') + 1),
    };
}

/**
 * @description tell whether a spec points at a directory on disk. Both the POSIX
 *              and the Windows form are recognised on every platform, so that a
 *              spec keeps the same meaning whichever host parses it:
 *              `./x` `../x` `~/x` `/x` `\x` `\\host\share` `C:\x` `C:/x`
 */
const LOCAL_PATH_PATTERN = /^(\.{1,2}[\\/]|~[\\/]|[\\/]|[a-zA-Z]:[\\/])/;

/**
 * @description strip the `file:` protocol, and the empty authority of the
 *              `file://` / `file:///` URL forms, from a local spec
 * @sample 'file:../foo' → '../foo'; 'file:///C:/foo' → 'C:/foo'; 'file:///foo' → '/foo'
 */
function strip_file_protocol(spec) {
    let p = spec.replace(/^file:/, '');

    // file:///C:/foo, file://C:/foo
    p = p.replace(/^\/{2,3}(?=[a-zA-Z]:[\\/])/, '');

    // file:///foo
    if (p.startsWith('///'))
        p = p.slice(2);

    return p;
}

/**
 * @param pkg_name - package name, used when `input_uri` carries no spec
 * @param input_uri - dependency spec, e.g. '^1.0.0', 'user/repo#master', 'file:../pkg'
 * @param base_dir - directory a relative local path is resolved against;
 *                   defaults to the current working directory
 */
const parse_pkg_installname = exports.parse_pkg_installname = function (pkg_name, input_uri, base_dir) {
    if (!input_uri || input_uri === '*')
        input_uri = pkg_name;

    input_uri = input_uri.replace(/^npm:/, '');
    input_uri = input_uri.replace(/^https?:\/\/[^\/]+\//, '');

    // A hosted git shorthand (`github:user/repo#ref`) is a spec of its own, and it
    // has to be recognised before anything else looks at it: the `:` would otherwise
    // be read as an alias, and the whole string handed to the registry as a range
    const hosted = split_hosted_git(String(input_uri));

    if (hosted)
        return {
            type: 'git',
            package_name: null,
            registry_pkg_path: null,
            registry_semver: null,
            from_http: false,
            git_origin: helpers_string.ensure_suffx(hosted.origin),
            git_basename: hosted.basename,
            git_path: hosted.path,
            git_reference: hosted.reference,
        };

    // local path: check before @ split. Use semver to exclude version ranges (~1.0.0, ^2.0, >=1.0, etc.)
    const local_spec = strip_file_protocol(input_uri);

    // `file:` names a path whatever follows it (npm reads `file:pkg` as `./pkg`),
    // while anything else has to look like a path to be one
    const file_spec = input_uri.indexOf('file:') === 0 && local_spec !== '' &&
        !/^[a-zA-Z][a-zA-Z0-9+.-]*:\/\//.test(local_spec);

    if (!semver.validRange(input_uri) && (file_spec || LOCAL_PATH_PATTERN.test(local_spec)))
        return {
            type: 'local',
            local_path: path.resolve(base_dir || process.cwd(), local_spec),
            package_name: null,
            registry_pkg_path: null,
            registry_semver: null,
            from_http: false,
            git_origin: '',
            git_basename: null,
            git_path: null,
            git_reference: null,
        }

    const registry_part = input_uri.split('@');
    if (registry_part.length == 2 && registry_part[0] !== '')
        return {
            type: 'registry',
            package_name: registry_part[0],
            registry_pkg_path: registry_part[0],
            registry_semver: registry_part[1],
            from_http: false,
            git_origin: '',
            git_basename: null,
            git_path: null,
            git_reference: null,
        }

    if (registry_part.length == 2 && registry_part[0] === '')
        return {
            type: 'registry',
            package_name: input_uri,
            registry_pkg_path: input_uri,
            registry_semver: "*",
            from_http: false,
            git_origin: '',
            git_basename: null,
            git_path: null,
            git_reference: null,
        }

    if (registry_part.length == 3) {
        if (registry_part[0] !== '')
            throw new Error(`[parse_pkg_installname] invalid input_uri: ${input_uri}`);

        const pkg_name = '@' + registry_part[1];
        return {
            type: 'registry',
            package_name: pkg_name,
            registry_pkg_path: pkg_name,
            registry_semver: registry_part[2],
            from_http: false,
            git_origin: '',
            git_basename: null,
            git_path: null,
            git_reference: null,
        }
    }

    if (registry_part.length > 1)
        throw new Error(`[parse_pkg_installname] invalid input_uri: ${input_uri}`);

    const git_part = input_uri.match(GIT_PATTERN);
    if (git_part)
        return {
            type: 'git',
            package_name: null,
            registry_pkg_path: null,
            registry_semver: null,
            from_http: false,
            git_origin: helpers_string.ensure_suffx(CST.DEFAULT_GIT_ORIGIN),
            git_basename: git_part[2],
            git_path: git_part[1],
            git_reference: git_part[5] || 'master',
        }

    const git_part1 = pkg_name.match(GIT_PATTERN);
    if (git_part1)
        return {
            type: 'git',
            package_name: null,
            registry_pkg_path: null,
            registry_semver: null,
            from_http: false,
            git_origin: helpers_string.ensure_suffx(CST.DEFAULT_GIT_ORIGIN),
            git_basename: git_part1[2],
            git_path: git_part1[1],
            git_reference: input_uri || git_part1[5] || 'master',
        }

    if (MODULE_NAME_PATTERN.test(input_uri))
        return {
            type: 'registry',
            package_name: input_uri,
            registry_pkg_path: input_uri,
            registry_semver: "*",
            from_http: false,
            git_origin: '',
            git_basename: null,
            git_path: null,
            git_reference: null,
        };

    return {
        type: 'registry',
        package_name: pkg_name,
        registry_pkg_path: pkg_name,
        registry_semver: input_uri,
        from_http: false,
        git_origin: '',
        git_basename: null,
        git_path: null,
        git_reference: null,
    };
}

/**
 * @param git_base git host, must end with '/'
 * @param pkg_type_info from `parse_pkg_installname(...)`
 */
exports.get_git_archive_url = function (pkg_type_info, { git_base = '' } = {}) {
    if (!pkg_type_info)
        throw new Error('[get_git_archive_url] pkg_type_info required!')

    if (typeof pkg_type_info === 'string')
        pkg_type_info = parse_pkg_installname(pkg_type_info)

    git_base = helpers_string.ensure_suffx(git_base || 'https://github.com/')
    /**
     * @sample https://github.com/[PKG_NAME]/archive/[GIT_REFERENCE].zip
     */
    return `${git_base}${pkg_type_info.git_path}/archive/${pkg_type_info.git_reference}.zip`
}
/**
 * @param rawpath path of raw, must not start with '/'
 * @param git_base git host, must end with '/'
 * @param pkg_type_info from `parse_pkg_installname(...)`
 */
exports.get_git_raw_url = function (rawpath, pkg_type_info, { git_base = '' } = {}) {
    if (!pkg_type_info)
        throw new Error('[get_git_raw_url] pkg_type_info required!')

    if (typeof pkg_type_info === 'string')
        pkg_type_info = parse_pkg_installname(pkg_type_info)

    // the doc comment says the path must not start with `/`, and every caller passes
    // one anyway — so the separator is added here and a leading slash dropped, which
    // accepts both and gives one URL
    rawpath = '/' + String(rawpath).replace(/^\/+/, '');

    git_base = helpers_string.ensure_suffx(git_base || 'https://github.com/')

    // GitHub retired the `/<owner>/<repo>/raw/<ref>/<file>` path — it answers 504
    // now, not a redirect — and raw content lives on its own host. Every other
    // forge (GitLab, Gitea, a GitHub Enterprise install) keeps the path form, so
    // only github.com is rewritten
    const is_github = /^(?:https?:\/\/)?(?:www\.)?github\.com\/?$/.test(git_base);

    if (is_github)
        /**
         * @sample https://raw.githubusercontent.com/[PKG_NAME]/[GIT_REFERENCE]/[rawpath]
         */
        return `https://raw.githubusercontent.com/${pkg_type_info.git_path}/${pkg_type_info.git_reference}${rawpath}`;

    /**
     * @sample https://[GIT_HOST]/[PKG_NAME]/raw/[GIT_REFERENCE]/[rawpath]
     */
    return `${git_base}${pkg_type_info.git_path}/raw/${pkg_type_info.git_reference}${rawpath}`
}