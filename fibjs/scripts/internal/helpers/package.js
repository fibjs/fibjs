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

const parse_pkg_installname = exports.parse_pkg_installname = function (input_uri = '', version = '') {
    /**
     * when providing version but not semantic version, consider it as special installation name
     */
    if (version && exports.is_special_installname(version)) {
        input_uri = version
    }

    const Is = {
        http: false,
        github: false,
        registry: false,
        file: false,
    }
    const uriObj = url.parse(input_uri)
    Is.http = uriObj.protocol === 'http:' || uriObj.protocol === 'https:'

    const origin = !Is.http ? '' : `${uriObj.protocol}//${uriObj.host}`;


    let pkg_pattern = input_uri;
    if (Is.http) pkg_pattern = `${uriObj.pathname}${uriObj.hash}`;

    pkg_pattern = helpers_string.ensure_unprefix(pkg_pattern);
    
    let [scope_str, pkg] = (pkg_pattern || '').split('/')
    
    if (!pkg) {
        pkg = scope_str
        scope_str = ''
    }

    Is.github = scope_str && /[a-zA-Z]/.test(scope_str[0])
    Is.registry = !scope_str || scope_str[0] === '@'

    let github_org_str = ''
    if (Is.github) github_org_str = scope_str

    const [name1, registry_semver = null] = (pkg || '').split('@')

    const [name0, git_reference = null] = (name1 || '').split('#')
    
    const installation_name = Is.github ? null : (scope_str ? `${scope_str}/${name0}` : name0)

    return {
        type: Is.github ? 'git' : 'registry',
        package_name: installation_name,
        registry_pkg_path: !Is.registry ? null : installation_name,
        registry_semver: registry_semver,
        from_http: Is.http,
        git_origin: Is.github ? (null || helpers_string.ensure_suffx(CST.DEFAULT_GIT_ORIGIN)) : origin,
        git_basename: !Is.github ? null : name0,
        git_path: !Is.github ? null : `${github_org_str}/${name0}`,
        git_reference: !Is.github ? null : (git_reference || 'master'),
    }
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