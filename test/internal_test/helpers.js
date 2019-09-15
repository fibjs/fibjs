var test = require("test");
test.setup();

describe('internal/helpers', () => {
    describe('@package', () => {
        var helpers_package = require("internal/helpers/package");

        describe('parse_pkg_installname', () => {
            ;[
                [
                    '@scope/pkg', {
                        "type": "registry",
                        "package_name": "@scope/pkg",
                        "registry_pkg_path": "@scope/pkg",
                        "registry_semver": null,
                        "from_http": false,
                        "git_origin": "",
                        "git_basename": null,
                        "git_path": null,
                        "git_reference": null,
                    }
                ],
                [
                    '@scope/pkg@^1.2.3', {
                        "type": "registry",
                        "package_name": "@scope/pkg",
                        "registry_pkg_path": "@scope/pkg",
                        "registry_semver": "^1.2.3",
                        "from_http": false,
                        "git_origin": "",
                        "git_basename": null,
                        "git_path": null,
                        "git_reference": null
                    }
                ],
                [
                    '@scope/pkg123@>1.2.4', {
                        "type": "registry",
                        "package_name": "@scope/pkg123",
                        "registry_pkg_path": "@scope/pkg123",
                        "registry_semver": ">1.2.4",
                        "from_http": false,
                        "git_origin": "",
                        "git_basename": null,
                        "git_path": null,
                        "git_reference": null
                    }
                ],
                [
                    '@scope123/pkg123', {
                        "type": "registry",
                        "package_name": "@scope123/pkg123",
                        "registry_pkg_path": "@scope123/pkg123",
                        "registry_semver": null,
                        "from_http": false,
                        "git_origin": "",
                        "git_basename": null,
                        "git_path": null,
                        "git_reference": null
                    }
                ],
                [
                    'pkg123', {
                        "type": "registry",
                        "package_name": "pkg123",
                        "registry_pkg_path": "pkg123",
                        "registry_semver": null,
                        "from_http": false,
                        "git_origin": "",
                        "git_basename": null,
                        "git_path": null,
                        "git_reference": null,
                    }
                ],
                [
                    'pkg123@3.8.1', {
                        "type": "registry",
                        "package_name": "pkg123",
                        "registry_pkg_path": "pkg123",
                        "registry_semver": "3.8.1",
                        "from_http": false,
                        "git_origin": "",
                        "git_basename": null,
                        "git_path": null,
                        "git_reference": null,
                    }
                ],
                [
                    'pkg123@~3.8.1', {
                        "type": "registry",
                        "package_name": "pkg123",
                        "registry_pkg_path": "pkg123",
                        "registry_semver": "~3.8.1",
                        "from_http": false,
                        "git_origin": "",
                        "git_basename": null,
                        "git_path": null,
                        "git_reference": null,
                    }
                ],
                [
                    'org/github123', {
                        "type": "git",
                        "registry_pkg_path": null,
                        "registry_semver": null,
                        "from_http": false,
                        "git_origin": "https://github.com/",
                        "git_basename": "github123",
                        "git_path": "org/github123",
                        "git_reference": "master",
                        "package_name": null
                    }
                ],
                [
                    'org/github123#branch', {
                        "type": "git",
                        "registry_pkg_path": null,
                        "registry_semver": null,
                        "from_http": false,
                        "git_origin": "https://github.com/",
                        "git_basename": "github123",
                        "git_path": "org/github123",
                        "git_reference": "branch",
                        "package_name": null
                    }
                ],
                [
                    'org/github123#v1.7.0', {
                        "type": "git",
                        "registry_pkg_path": null,
                        "registry_semver": null,
                        "from_http": false,
                        "git_origin": "https://github.com/",
                        "git_basename": "github123",
                        "git_path": "org/github123",
                        "git_reference": "v1.7.0",
                        "package_name": null
                    }
                ],
                [
                    'org/github123#43c5d0be', {
                        "type": "git",
                        "registry_pkg_path": null,
                        "registry_semver": null,
                        "from_http": false,
                        "git_origin": "https://github.com/",
                        "git_basename": "github123",
                        "git_path": "org/github123",
                        "git_reference": "43c5d0be",
                        "package_name": null
                    }
                ],
            ].forEach(([pkg_input, result]) => {
                it(`parse ${pkg_input}`, () => {
                    assert.deepEqual(
                        helpers_package.parse_pkg_installname(pkg_input),
                        result
                    )
                })
            })
        });

        ;[
            [
                ['fibjs/fib-graphql', { git_base: '' }],
                'https://github.com/fibjs/fib-graphql/archive/master.zip'
            ],
            [
                ['fibjs/fib-graphql', { git_base: 'https://git.fibjs.org' }],
                'https://git.fibjs.org/fibjs/fib-graphql/archive/master.zip'
            ],
            [
                ['fibjs/fib-graphql#v3.0.0', { git_base: '' }],
                'https://github.com/fibjs/fib-graphql/archive/v3.0.0.zip'
            ],
        ].forEach(([args, result]) => {
            it(`[get_git_archive_url] arguments: ${JSON.stringify(args)}, target: ${result}`, () => {
                assert.equal(
                    helpers_package.get_git_archive_url(...args),
                    result
                )
            });
        });

        ;[
            [
                ['/package.json', 'fibjs/fib-graphql'],
                'https://github.com/fibjs/fib-graphql/raw/master/package.json'
            ],
            [
                ['/package.json', 'fibjs/fib-graphql#fb41568'],
                'https://github.com/fibjs/fib-graphql/raw/fb41568/package.json'
            ],
            [
                ['/package.json', 'fibjs/fib-graphql#v3.0.0'],
                'https://github.com/fibjs/fib-graphql/raw/v3.0.0/package.json'
            ],
        ].forEach(([args, result]) => {
            it(`[get_git_raw_url] arguments: ${JSON.stringify(args)}, target: ${result}`, () => {
                assert.equal(
                    helpers_package.get_git_raw_url(...args),
                    result
                )
            });
        });
    });
});

require.main === module && test.run(console.DEBUG);