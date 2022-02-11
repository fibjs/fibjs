var test = require("test");
test.setup();

var fs = require("fs");
var path = require("path");
var child_process = require("child_process");

var bin = process.execPath;

var { chdirAndDo, ensureDirectoryExisted, readJson } = require('../../_helpers/process');

const processRunOptions = {
    env: {
        FIBJS_SILENT_INSALL: process.env.hasOwnProperty('FIBJS_SILENT_INSALL') ? process.env.FIBJS_SILENT_INSALL : 1,
        http_proxy: process.env.http_proxy,
        https_proxy: process.env.https_proxy,
    }
}

function resolveNodeModules(baseroot, pkgName) {
    return path.resolve(baseroot, `./node_modules/${pkgName}`)
}

describe('opt_tools/install from raw', () => {
    var installTarget = null
    // deps for this test :start
    var rmdirr;
    // deps for this test :end

    function clean_pkgjson() {
        const _p = path.resolve(__dirname, './package.json')
        if (fs.exists(_p))
            fs.unlink(_p)
    }

    before(() => {
        clean_pkgjson();

        chdirAndDo(
            path.resolve(__dirname),
            () => {
                // ensure dependencies of this test existed
                child_process.run(bin, ['--install', '@fibjs/rmdirr']);
                rmdirr = require('@fibjs/rmdirr');
            }
        )()
    })

    after(() => {
        clean_pkgjson();
    });

    ;[
        ['default registry: regitry.npmjs.org'],
    ].forEach(
        ([desc, registry]) => {
            describe(desc, () => {
                installTarget = path.resolve(__dirname, './tmp/installFromRegistry');

                var pkgJson = null
                beforeEach(() => {
                    ensureDirectoryExisted(installTarget);
                    pkgJson = path.resolve(installTarget, './package.json');
                });

                afterEach(() => {
                    rmdirr(installTarget)
                });

                it('install public normal package', () => {
                    chdirAndDo(installTarget, () => {
                        child_process.run(bin, ['--install', 'ejs'], processRunOptions)
                    })();

                    assert.ok(fs.exists(
                        resolveNodeModules(installTarget, 'ejs')
                    ));

                    assert.notOk(
                        fs.exists(pkgJson)
                    );
                });

                it('install --save public normal package', () => {
                    chdirAndDo(installTarget, () => {
                        child_process.run(bin, ['--install', '--save', 'ejs'], processRunOptions)
                    })();

                    assert.ok(fs.exists(
                        resolveNodeModules(installTarget, 'ejs')
                    ));

                    assert.ok(
                        readJson(pkgJson).dependencies['ejs']
                    );
                });

                it('install -S public normal package', () => {
                    chdirAndDo(installTarget, () => {
                        child_process.run(bin, ['--install', '-S', 'ejs'], processRunOptions)
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
                        child_process.run(bin, ['--install', '@fibjs/chalk'], processRunOptions)
                    })()

                    assert.ok(fs.exists(
                        resolveNodeModules(installTarget, '@fibjs/chalk')
                    ));

                    assert.notOk(
                        fs.exists(pkgJson)
                    );
                });

                it('install --save public @scope package', () => {
                    chdirAndDo(installTarget, () => {
                        child_process.run(bin, ['--install', '--save', '@fibjs/chalk'], processRunOptions)
                    })()

                    assert.ok(fs.exists(
                        resolveNodeModules(installTarget, '@fibjs/chalk')
                    ));

                    assert.ok(
                        readJson(pkgJson).dependencies['@fibjs/chalk']
                    );
                });

                it('install -S public @scope package', () => {
                    chdirAndDo(installTarget, () => {
                        child_process.run(bin, ['--install', '-S', '@fibjs/chalk'], processRunOptions)
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
                        child_process.run(bin, ['--install', '--save-dev', 'ejs'], processRunOptions)
                    })();

                    assert.ok(fs.exists(
                        resolveNodeModules(installTarget, 'ejs')
                    ));

                    assert.ok(
                        fs.exists(pkgJson)
                    );

                    assert.ok(
                        readJson(pkgJson).devDependencies['ejs']
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
            pkgJson = path.resolve(installTarget, 'package.json');
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
                    child_process.run(bin, ['--install', target], processRunOptions)
                })();

                assert.ok(fs.exists(
                    resolveNodeModules(installTarget, pkg_name)
                ));

                assert.notOk(
                    fs.exists(pkgJson)
                );
            });

            it(`[--save] ${desc}`, () => {
                chdirAndDo(installTarget, () => {
                    child_process.run(bin, ['--install', '--save', target], processRunOptions)
                })();

                assert.ok(fs.exists(
                    resolveNodeModules(installTarget, pkg_name)
                ));

                assert.equal(readJson(pkgJson).version, '1.0.0');

                assert.ok(
                    readJson(pkgJson).dependencies[pkg_name]
                );
            });

            it(`[-S] ${desc}`, () => {
                chdirAndDo(installTarget, () => {
                    child_process.run(bin, ['--install', '-S', target], processRunOptions)
                })();

                assert.ok(fs.exists(
                    resolveNodeModules(installTarget, pkg_name)
                ));

                assert.equal(readJson(pkgJson).version, '1.0.0');

                assert.ok(
                    readJson(pkgJson).dependencies[pkg_name]
                );
            });

            it(`[-D] ${desc}`, () => {
                chdirAndDo(installTarget, () => {
                    child_process.run(bin, ['--install', '-D', target], processRunOptions)
                })();

                assert.ok(fs.exists(
                    resolveNodeModules(installTarget, pkg_name)
                ));

                assert.equal(readJson(pkgJson).version, '1.0.0');

                assert.ok(
                    readJson(pkgJson).devDependencies[pkg_name]
                );
            });
        });
    });
});

require.main === module && test.run(console.DEBUG);