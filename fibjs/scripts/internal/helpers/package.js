const url = require('url');

const CST = require('internal/constant');
const helpers_string = require('internal/helpers/string')
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

const parse_pkg_installname = exports.parse_pkg_installname = function (pkg_name, input_uri) {
    if (!input_uri || input_uri === '*')
        input_uri = pkg_name;

    input_uri = input_uri.replace(/^npm:/, '');
    input_uri = input_uri.replace(/^https?:\/\/[^\/]+\//, '');

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

    git_base = helpers_string.ensure_suffx(git_base || 'https://github.com/')
    /**
     * @sample https://github.com/[PKG_NAME]/raw/[GIT_REFERENCE]/[rawpath]
     */
    return `${git_base}${pkg_type_info.git_path}/raw/${pkg_type_info.git_reference}${rawpath}`
}