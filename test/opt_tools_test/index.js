var test = require("test");
test.setup();

var fs = require("fs");
var path = require("path");

var bin = process.execPath;

var { chdirAndDo, ensureDirectoryExisted, readJson } = require('../_helpers/process');

const processRunOptions = {
    env: {
        // all required environment would fallback to the parent's one
        FIBJS_SILENT_INSALL: 1
    }
}

// deps for this test :start
var rmdirr = require('@fibjs/rmdirr');
// deps for this test :end

function resolveNodeModules(baseroot, pkgName) {
    return path.resolve(baseroot, `./node_modules/${pkgName}`)
}

function ensurePackageJson(dirname, pkgJsonContent) {
    ensureDirectoryExisted(dirname)

    var pkgJsonPath = path.resolve(dirname, './package.json')

    fs.writeFile(pkgJsonPath, JSON.stringify(pkgJsonContent, null, '  '))

    return pkgJsonPath;
}

describe('opt_tools/install', () => {
    var installTarget = null

    before(() => {
        chdirAndDo(
            path.resolve(__dirname),
            () => {
                // ensure dependencies of this test existed
                process.run(bin, ['--install']);
            }
        )()
    })

    describe('helpers', () => {
        it('is_special_installname', () => {
            const { is_special_installname } = require('internal/helpers/package')
            assert.isTrue(is_special_installname('fibjs/fib-pool'))
            assert.isTrue(is_special_installname('fibjs/fib-pool#vnormal'))
            assert.isTrue(is_special_installname('https://github.comfibjs/fib-graphql'))
            assert.isTrue(is_special_installname('https://github.comfibjs/fib-graphql#vspecial'))

            assert.isFalse(is_special_installname('*'))
        });
    });

    ;[
        ['default registry: regitry.npmjs.org'],
        ['specify registry: https://registry.yarnpkg.com', 'https://registry.yarnpkg.com'],
        // ['specify registry: https://registry.npm.taobao.org/', '//registry.npm.taobao.org/'],
    ].forEach(
        ([desc, registry]) => {
            describe(desc, () => {
                installTarget = path.resolve(__dirname, './tmp/installFromRegistry');

                var pkgJson = null
                beforeEach(() => {
                    ensureDirectoryExisted(installTarget);
                    pkgJson = ensurePackageJson(installTarget, Object.assign({
                        name: 'install-test'
                    }, registry ? { registry } : {}));
                });

                afterEach(() => {
                    rmdirr(installTarget)
                });

                it('install public normal package', () => {
                    chdirAndDo(installTarget, () => {
                        process.run(bin, ['--install', 'ejs'], processRunOptions)
                    })();

                    assert.ok(fs.exists(
                        resolveNodeModules(installTarget, 'ejs')
                    ));

                    assert.ok(
                        readJson(pkgJson).dependencies['ejs']
                    );
                });

                it('install public @scope package', () => {
                    chdirAndDo(installTarget, () => {
                        process.run(bin, ['--install', '@fibjs/chalk'], processRunOptions)
                    })()

                    assert.ok(fs.exists(
                        resolveNodeModules(installTarget, '@fibjs/chalk')
                    ));

                    assert.ok(
                        readJson(pkgJson).dependencies['@fibjs/chalk']
                    );
                });

                it('install package --save-dev/-D', () => {
                    chdirAndDo(installTarget, () => {
                        process.run(bin, ['--install', '@fibjs/chalk', '--save-dev'], processRunOptions)
                    })();

                    assert.ok(fs.exists(
                        resolveNodeModules(installTarget, '@fibjs/chalk')
                    ));

                    assert.ok(
                        !require(pkgJson).dependencies
                    );

                    assert.ok(
                        require(pkgJson).devDependencies['@fibjs/chalk']
                    );
                });
            });
        }
    );

    describe('install from github without dependencies', () => {
        var pkgJson = null
        installTarget = path.resolve(__dirname, './tmp/installFromGithub');

        beforeEach(() => {
            ensureDirectoryExisted(installTarget);
            pkgJson = ensurePackageJson(installTarget, Object.assign({
                name: 'install-from-github',
            }));
        });

        afterEach(() => {
            rmdirr(installTarget)
        });

        ;[
            // fibjs/fib-graphql
            [
                'install (default using master branch)',
                'fibjs/fib-graphql',
                'fib-graphql'
            ],
            [
                'install (specify https host)',
                'https://github.com/fibjs/fib-graphql',
                'fib-graphql'
            ],
            // fibjs/fib-pool#v1.4.0
            [
                'install from tagged release',
                'fibjs/fib-pool#v1.4.0',
                'fib-pool'
            ],
            [
                'install from tagged release (specify https host)',
                'https://github.com/fibjs/fib-pool#v1.4.0',
                'fib-pool'
            ],
            // fibjs/fib-pool#023009973ea9754eea6183953012bf4d80f00e28
            [
                'install from commit',
                'fibjs/fib-pool#023009973ea9754eea6183953012bf4d80f00e28',
                'fib-pool'
            ],
            [
                'install from commit (specify https host)',
                'https://github.com/fibjs/fib-pool#023009973ea9754eea6183953012bf4d80f00e28',
                'fib-pool'
            ],
        ].forEach(([desc, target, pkg_name]) => {
            it(desc, () => {
                chdirAndDo(installTarget, () => {
                    process.run(bin, ['--install', target], processRunOptions)
                })();

                assert.ok(fs.exists(
                    resolveNodeModules(installTarget, pkg_name)
                ));

                assert.ok(fs.exists(
                    resolveNodeModules(installTarget, pkg_name)
                ));

                assert.ok(
                    readJson(pkgJson).dependencies[pkg_name]
                );
            });
        });
    });

    describe('install from github with dependencies', () => {
        var pkgJson = null
        var initDeps = {
            'fib-pool': 'fibjs/fib-pool#v1.4.0',
            'fibos.js': 'FIBOSIO/fibos.js',
            '@fibjs/detect-port': 'fibjs-modules/detect-port',
        };

        installTarget = path.resolve(__dirname, './tmp/installFromGithub');

        beforeEach(() => {
            ensureDirectoryExisted(installTarget);
            pkgJson = ensurePackageJson(installTarget, Object.assign({
                name: 'install-from-github',
                dependencies: initDeps
            }));
        });

        afterEach(() => {
            rmdirr(installTarget)
        });

        it('install from package.json', () => {
            chdirAndDo(installTarget, () => {
                process.run(bin, ['--install'], processRunOptions)
            })();

            Object.keys(initDeps).forEach(pkgName => {
                assert.ok(fs.exists(
                    resolveNodeModules(installTarget, pkgName)
                ));

                assert.ok(fs.exists(
                    resolveNodeModules(installTarget, pkgName)
                ));
            });
        });

        ;[
            [
                'https://github.com/fibjs/fib-graphql',
                'fib-graphql'
            ],
            [
                '@fibjs/ci@latest',
                '@fibjs/ci'
            ],
        ].forEach(([target, pkg_name]) => {

            it(`install ${pkg_name} from command line`, () => {
                chdirAndDo(installTarget, () => {
                    process.run(bin, ['--install', target], processRunOptions)
                })();

                assert.ok(fs.exists(
                    resolveNodeModules(installTarget, pkg_name)
                ));

                assert.ok(fs.exists(
                    resolveNodeModules(installTarget, pkg_name)
                ));

                assert.ok(
                    readJson(pkgJson).dependencies[pkg_name]
                );
            });
        });
    });
});

require.main === module && test.run(console.DEBUG);