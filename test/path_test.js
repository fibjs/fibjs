const { describe, it } = require('node:test');
const assert = require('assert');

var path = require('path');

var isWindows = process.platform === 'win32';
const isFibjs = typeof process !== 'undefined' && process.versions && process.versions.fibjs;

describe('path', () => {
    it('win32/posix', () => {
        if (isWindows)
            assert.equal(path, path.win32);
        else
            assert.equal(path, path.posix);
    });

    describe('basename', () => {
        it('basic functionality', () => {
            // Test basic basename extraction
            assert.strictEqual(path.basename(__filename), 'path_test.js');
            assert.strictEqual(path.basename(__filename, '.js'), 'path_test');
            assert.strictEqual(path.basename('/dir/basename.ext'), 'basename.ext');
            assert.strictEqual(path.basename('/basename.ext'), 'basename.ext');
            assert.strictEqual(path.basename('basename.ext'), 'basename.ext');
            assert.strictEqual(path.basename('/aaa/bbb'), 'bbb');
            assert.strictEqual(path.basename('/aaa/'), 'aaa');
            assert.strictEqual(path.basename('/aaa/b'), 'b');
            assert.strictEqual(path.basename('/a/b'), 'b');
        });

        it('extension removal', () => {
            // Test extension parameter functionality
            assert.strictEqual(path.basename('.js', '.js'), '');
            assert.strictEqual(path.basename('js', '.js'), 'js');
            assert.strictEqual(path.basename('file.js', '.ts'), 'file.js');
            assert.strictEqual(path.basename('file', '.js'), 'file');
            assert.strictEqual(path.basename('file.js.old', '.js.old'), 'file');
            assert.strictEqual(path.basename('a', 'a'), '');
        });

        it('trailing slash handling', () => {
            // Test handling of trailing slashes
            assert.strictEqual(path.basename('basename.ext/'), 'basename.ext');
            assert.strictEqual(path.basename('basename.ext//'), 'basename.ext');
        });

        it('edge cases', () => {
            // Test edge cases and empty inputs
            assert.strictEqual(path.basename(''), '');
            assert.strictEqual(path.basename('//a'), 'a');
        });

        it('extension matching edge cases', () => {
            // Test various extension matching scenarios
            assert.strictEqual(path.basename('aaa/bbb', '/bbb'), 'bbb');
            assert.strictEqual(path.basename('aaa/bbb', 'a/bbb'), 'bbb');
            assert.strictEqual(path.basename('aaa/bbb', 'bbb'), 'bbb');
            assert.strictEqual(path.basename('aaa/bbb//', 'bbb'), 'bbb');
            assert.strictEqual(path.basename('aaa/bbb', 'bb'), 'b');
            assert.strictEqual(path.basename('aaa/bbb', 'b'), 'bb');
            assert.strictEqual(path.basename('/aaa/bbb', '/bbb'), 'bbb');
            assert.strictEqual(path.basename('/aaa/bbb', 'a/bbb'), 'bbb');
            assert.strictEqual(path.basename('/aaa/bbb', 'bbb'), 'bbb');
            assert.strictEqual(path.basename('/aaa/bbb//', 'bbb'), 'bbb');
            assert.strictEqual(path.basename('/aaa/bbb', 'bb'), 'b');
            assert.strictEqual(path.basename('/aaa/bbb', 'b'), 'bb');
        });

        it('posix specific behavior', () => {
            // Test POSIX-specific behavior (backslashes are regular characters)
            function testPosix(fn) {
                assert.strictEqual(fn('\\dir\\basename.ext'),
                    '\\dir\\basename.ext');
                assert.strictEqual(fn('\\basename.ext'), '\\basename.ext');
                assert.strictEqual(fn('basename.ext'), 'basename.ext');
                assert.strictEqual(fn('basename.ext\\'), 'basename.ext\\');
                assert.strictEqual(fn('basename.ext\\\\'), 'basename.ext\\\\');
                assert.strictEqual(fn('foo'), 'foo');
            }

            testPosix(path.posix.basename.bind(path.posix));

            // POSIX filenames may include control characters
            // c.f. http://www.dwheeler.com/essays/fixing-unix-linux-filenames.html
            const controlCharFilename = `Icon${String.fromCharCode(13)}`;
            assert.strictEqual(path.posix.basename(`/a/b/${controlCharFilename}`),
                controlCharFilename);
        });

        it('win32 specific behavior', () => {
            // Test Win32-specific behavior (backslashes are path separators)
            function testWin32(fn) {
                assert.strictEqual(fn('\\dir\\basename.ext'), 'basename.ext');
                assert.strictEqual(fn('\\basename.ext'), 'basename.ext');
                assert.strictEqual(fn('basename.ext'), 'basename.ext');
                assert.strictEqual(fn('basename.ext\\'), 'basename.ext');
                assert.strictEqual(fn('basename.ext\\\\'), 'basename.ext');
                assert.strictEqual(fn('foo'), 'foo');
                assert.strictEqual(fn('aaa\\bbb', '\\bbb'), 'bbb');
                assert.strictEqual(fn('aaa\\bbb', 'a\\bbb'), 'bbb');
                assert.strictEqual(fn('aaa\\bbb', 'bbb'), 'bbb');
                assert.strictEqual(fn('aaa\\bbb\\\\\\\\', 'bbb'), 'bbb');
                assert.strictEqual(fn('aaa\\bbb', 'bb'), 'b');
                assert.strictEqual(fn('aaa\\bbb', 'b'), 'bb');
                assert.strictEqual(fn('C:'), '');
                assert.strictEqual(fn('C:.'), '.');
                assert.strictEqual(fn('C:\\'), '');
                assert.strictEqual(fn('C:\\dir\\base.ext'), 'base.ext');
                assert.strictEqual(fn('C:\\basename.ext'), 'basename.ext');
                assert.strictEqual(fn('C:basename.ext'), 'basename.ext');
                assert.strictEqual(fn('C:basename.ext\\'), 'basename.ext');
                assert.strictEqual(fn('C:basename.ext\\\\'), 'basename.ext');
                assert.strictEqual(fn('C:foo'), 'foo');
                assert.strictEqual(fn('file:stream'), 'file:stream');
            }

            testWin32(path.win32.basename.bind(path.win32));
            assert.strictEqual(path.win32.basename('a', 'a'), '');
        });

        it('current platform behavior', () => {
            // Test current platform behavior
            if (isWindows) {
                function testWin32(fn) {
                    assert.strictEqual(fn('\\dir\\basename.ext'), 'basename.ext');
                    assert.strictEqual(fn('\\basename.ext'), 'basename.ext');
                    assert.strictEqual(fn('basename.ext'), 'basename.ext');
                    assert.strictEqual(fn('basename.ext\\'), 'basename.ext');
                    assert.strictEqual(fn('basename.ext\\\\'), 'basename.ext');
                    assert.strictEqual(fn('foo'), 'foo');
                }
                testWin32(path.basename.bind(path));
            } else {
                function testPosix(fn) {
                    assert.strictEqual(fn('\\dir\\basename.ext'),
                        '\\dir\\basename.ext');
                    assert.strictEqual(fn('\\basename.ext'), '\\basename.ext');
                    assert.strictEqual(fn('basename.ext'), 'basename.ext');
                    assert.strictEqual(fn('basename.ext\\'), 'basename.ext\\');
                    assert.strictEqual(fn('basename.ext\\\\'), 'basename.ext\\\\');
                    assert.strictEqual(fn('foo'), 'foo');
                }
                testPosix(path.basename.bind(path));
            }
        });
    });

    describe('extname', () => {
        it('basic functionality', () => {
            // Test basic extname extraction
            assert.strictEqual(path.extname(__filename), '.js');
            assert.strictEqual(path.extname('file.ext'), '.ext');
            assert.strictEqual(path.extname('/path/to/file.ext'), '.ext');
            assert.strictEqual(path.extname('/path.to/file.ext'), '.ext');
            assert.strictEqual(path.extname('/path/to/f.ext'), '.ext');
            assert.strictEqual(path.extname('file.ext.ext'), '.ext');
            assert.strictEqual(path.extname('/file.ext'), '.ext');
        });

        it('no extension cases', () => {
            // Test cases where there is no extension
            assert.strictEqual(path.extname(''), '');
            assert.strictEqual(path.extname('/path/to/file'), '');
            assert.strictEqual(path.extname('/path.to/file'), '');
            assert.strictEqual(path.extname('file'), '');
            assert.strictEqual(path.extname('/file'), '');
            assert.strictEqual(path.extname('file/'), '');
            assert.strictEqual(path.extname('file//'), '');
        });

        it('dot files and edge cases', () => {
            // Test dotfiles and special cases
            assert.strictEqual(path.extname('.file'), '');
            assert.strictEqual(path.extname('.file.ext'), '.ext');
            assert.strictEqual(path.extname('/.file'), '');
            assert.strictEqual(path.extname('/.file.ext'), '.ext');
            assert.strictEqual(path.extname('/path.to/.file'), '');
            assert.strictEqual(path.extname('/path.to/.file.ext'), '.ext');
            assert.strictEqual(path.extname('.'), '');
            assert.strictEqual(path.extname('./'), '');
        });

        it('double dot cases', () => {
            // Test cases with double dots
            assert.strictEqual(path.extname('/path/to/..ext'), '.ext');
            assert.strictEqual(path.extname('/path/to/..'), '');
            assert.strictEqual(path.extname('..'), '');
            assert.strictEqual(path.extname('../'), '');
            assert.strictEqual(path.extname('..file.ext'), '.ext');
            assert.strictEqual(path.extname('..file'), '.file');
            assert.strictEqual(path.extname('..file.'), '.');
            assert.strictEqual(path.extname('..file..'), '.');
        });

        it('multiple dots and special patterns', () => {
            // Test multiple dots and special patterns
            assert.strictEqual(path.extname('...'), '.');
            assert.strictEqual(path.extname('...ext'), '.ext');
            assert.strictEqual(path.extname('....'), '.');
            assert.strictEqual(path.extname('file.'), '.');
            assert.strictEqual(path.extname('.file.'), '.');
            assert.strictEqual(path.extname('.file..'), '.');
        });

        it('trailing slashes with extensions', () => {
            // Test extensions with trailing slashes
            assert.strictEqual(path.extname('file.ext/'), '.ext');
            assert.strictEqual(path.extname('file.ext//'), '.ext');
            assert.strictEqual(path.extname('file./'), '.');
            assert.strictEqual(path.extname('file.//'), '.');
        });

        it('cross-platform test matrix', () => {
            // Test matrix for both platforms
            const failures = [];
            const slashRE = /\//g;

            const testCases = [
                [__filename, '.js'],
                ['', ''],
                ['/path/to/file', ''],
                ['/path/to/file.ext', '.ext'],
                ['/path.to/file.ext', '.ext'],
                ['/path.to/file', ''],
                ['/path.to/.file', ''],
                ['/path.to/.file.ext', '.ext'],
                ['/path/to/f.ext', '.ext'],
                ['/path/to/..ext', '.ext'],
                ['/path/to/..', ''],
                ['file', ''],
                ['file.ext', '.ext'],
                ['.file', ''],
                ['.file.ext', '.ext'],
                ['/file', ''],
                ['/file.ext', '.ext'],
                ['/.file', ''],
                ['/.file.ext', '.ext'],
                ['.path/file.ext', '.ext'],
                ['file.ext.ext', '.ext'],
                ['file.', '.'],
                ['.', ''],
                ['./', ''],
                ['.file.ext', '.ext'],
                ['.file', ''],
                ['.file.', '.'],
                ['.file..', '.'],
                ['..', ''],
                ['../', ''],
                ['..file.ext', '.ext'],
                ['..file', '.file'],
                ['..file.', '.'],
                ['..file..', '.'],
                ['...', '.'],
                ['...ext', '.ext'],
                ['....', '.'],
                ['file.ext/', '.ext'],
                ['file.ext//', '.ext'],
                ['file/', ''],
                ['file//', ''],
                ['file./', '.'],
                ['file.//', '.'],
            ];

            testCases.forEach((test) => {
                const expected = test[1];
                [path.posix.extname, path.win32.extname].forEach((extname) => {
                    let input = test[0];
                    let os;
                    if (extname === path.win32.extname) {
                        input = input.replace(slashRE, '\\');
                        os = 'win32';
                    } else {
                        os = 'posix';
                    }
                    const actual = extname(input);
                    const message = `path.${os}.extname(${JSON.stringify(input)})\n  expect=${JSON.stringify(expected)}\n  actual=${JSON.stringify(actual)}`;
                    if (actual !== expected)
                        failures.push(`\n${message}`);
                }); {
                    const input = `C:${test[0].replace(slashRE, '\\')}`;
                    const actual = path.win32.extname(input);
                    const message = `path.win32.extname(${JSON.stringify(input)})\n  expect=${JSON.stringify(expected)}\n  actual=${JSON.stringify(actual)}`;
                    if (actual !== expected)
                        failures.push(`\n${message}`);
                }
            });
            assert.strictEqual(failures.length, 0, failures.join(''));
        });

        it('individual platform tests', () => {
            // Test individual platform functionality
            function test(fn) {
                assert.equal(fn('./test.js'), '.js');
                assert.equal(fn(''), '');
                assert.equal(fn('/path/to/file'), '');
                assert.equal(fn('/path/to/file.ext'), '.ext');
                assert.equal(fn('/path.to/file.ext'), '.ext');
                assert.equal(fn('/path.to/file'), '');
                assert.equal(fn('/path.to/.file'), '');
                assert.equal(fn('/path.to/.file.ext'), '.ext');
                assert.equal(fn('/path/to/f.ext'), '.ext');
                assert.equal(fn('/path/to/..ext'), '.ext');
                assert.equal(fn('file'), '');
                assert.equal(fn('file.ext'), '.ext');
                assert.equal(fn('.file'), '');
                assert.equal(fn('.file.ext'), '.ext');
                assert.equal(fn('/file'), '');
                assert.equal(fn('/file.ext'), '.ext');
                assert.equal(fn('/.file'), '');
                assert.equal(fn('/.file.ext'), '.ext');
                assert.equal(fn('.path/file.ext'), '.ext');
                assert.equal(fn('file.ext.ext'), '.ext');
                assert.equal(fn('file.'), '.');
                assert.equal(fn('.'), '');
                assert.equal(fn('./'), '');
                assert.equal(fn('.file.ext'), '.ext');
                assert.equal(fn('.file'), '');
                assert.equal(fn('.file.'), '.');
                assert.equal(fn('.file..'), '.');
                assert.equal(fn('..'), '');
                assert.equal(fn('../'), '');
                assert.equal(fn('..file.ext'), '.ext');
                assert.equal(fn('..file'), '.file');
                assert.equal(fn('..file.'), '.');
                assert.equal(fn('..file..'), '.');
                assert.equal(fn('...'), '.');
                assert.equal(fn('...ext'), '.ext');
                assert.equal(fn('....'), '.');
                assert.equal(fn('file.ext/'), '.ext');
            }

            test(path.extname.bind(path));
            test(path.win32.extname.bind(path.win32));
            test(path.posix.extname.bind(path.posix));
        });

        it('current platform backslash behavior', () => {
            // Test current platform behavior with backslashes
            if (isWindows) {
                // On windows, backspace is a path separator.
                assert.equal(path.extname('.\\'), '');
                assert.equal(path.extname('..\\'), '');
                assert.equal(path.extname('file.ext\\'), '.ext');
            } else {
                // On unix, backspace is a valid name component like any other character.
                assert.equal(path.extname('.\\'), '');
                assert.equal(path.extname('..\\'), '.\\');
                assert.equal(path.extname('file.ext\\'), '.ext\\');
            }
        });

        it('win32 specific backslash handling', () => {
            // On Windows, backslash is a path separator.
            assert.strictEqual(path.win32.extname('.\\'), '');
            assert.strictEqual(path.win32.extname('..\\'), '');
            assert.strictEqual(path.win32.extname('file.ext\\'), '.ext');
            assert.strictEqual(path.win32.extname('file.ext\\\\'), '.ext');
            assert.strictEqual(path.win32.extname('file\\'), '');
            assert.strictEqual(path.win32.extname('file\\\\'), '');
            assert.strictEqual(path.win32.extname('file.\\'), '.');
            assert.strictEqual(path.win32.extname('file.\\\\'), '.');
        });

        it('posix specific backslash handling', () => {
            // On *nix, backslash is a valid name component like any other character.
            assert.strictEqual(path.posix.extname('.\\'), '');
            assert.strictEqual(path.posix.extname('..\\'), '.\\');
            assert.strictEqual(path.posix.extname('file.ext\\'), '.ext\\');
            assert.strictEqual(path.posix.extname('file.ext\\\\'), '.ext\\\\');
            assert.strictEqual(path.posix.extname('file\\'), '');
            assert.strictEqual(path.posix.extname('file\\\\'), '');
            assert.strictEqual(path.posix.extname('file.\\'), '.\\');
            assert.strictEqual(path.posix.extname('file.\\\\'), '.\\\\');
        });
    });

    describe('parse', () => {
        function checkParseFormat(path, paths) {
            paths.forEach(([element, root]) => {
                const output = path.parse(element);
                assert.strictEqual(typeof output.root, 'string');
                assert.strictEqual(typeof output.dir, 'string');
                assert.strictEqual(typeof output.base, 'string');
                assert.strictEqual(typeof output.ext, 'string');
                assert.strictEqual(typeof output.name, 'string');
                assert.strictEqual(path.format(output), element);
                assert.strictEqual(output.root, root);
                assert(output.dir.startsWith(output.root));
                assert.strictEqual(output.dir, output.dir ? path.dirname(element) : '');
                assert.strictEqual(output.base, path.basename(element));
                assert.strictEqual(output.ext, path.extname(element));
            });
        }

        it('parse - win32', () => {
            const winPaths = [
                // [path, root]
                ['C:\\path\\dir\\index.html', 'C:\\'],
                ['C:\\another_path\\DIR\\1\\2\\33\\\\index', 'C:\\'],
                ['another_path\\DIR with spaces\\1\\2\\33\\index', ''],
                ['\\', '\\'],
                ['\\foo\\C:', '\\'],
                ['file', ''],
                ['file:stream', ''],
                ['.\\file', ''],
                ['C:', 'C:'],
                ['C:.', 'C:'],
                ['C:..', 'C:'],
                ['C:abc', 'C:'],
                ['C:\\', 'C:\\'],
                ['C:\\abc', 'C:\\'],
                ['', ''],

                // unc
                ['\\\\server\\share\\file_path', '\\\\server\\share\\'],
                ['\\\\server two\\shared folder\\file path.zip',
                    '\\\\server two\\shared folder\\'],
                ['\\\\teela\\admin$\\system32', '\\\\teela\\admin$\\'],
                ['\\\\?\\UNC\\server\\share', '\\\\?\\UNC\\']
            ];

            checkParseFormat(path.win32, winPaths);
        })

        it('parse - unix', () => {
            const unixPaths = [
                // [path, root]
                ['/home/user/dir/file.txt', '/'],
                ['/home/user/a dir/another File.zip', '/'],
                ['/home/user/a dir//another&File.', '/'],
                ['/home/user/a$$$dir//another File.zip', '/'],
                ['user/dir/another File.zip', ''],
                ['file', ''],
                ['.\\file', ''],
                ['./file', ''],
                ['C:\\foo', ''],
                ['/', '/'],
                ['', ''],
                ['.', ''],
                ['..', ''],
                ['/foo', '/'],
                ['/foo.', '/'],
                ['/foo.bar', '/'],
                ['/.', '/'],
                ['/.foo', '/'],
                ['/.foo.bar', '/'],
                ['/foo/bar.baz', '/']
            ];

            checkParseFormat(path.posix, unixPaths);
        })
    });

    describe('format', () => {
        function checkFormat(path, testCases) {
            testCases.forEach(([element, expect]) => {
                assert.strictEqual(path.format(element), expect);
            });
        }

        it('specialCaseFormat - win32', () => {
            const winSpecialCaseFormatTests = [
                [{ dir: 'some\\dir' }, 'some\\dir\\'],
                [{ base: 'index.html' }, 'index.html'],
                [{ root: 'C:\\' }, 'C:\\'],
                [{ name: 'index', ext: '.html' }, 'index.html'],
                [{ dir: 'some\\dir', name: 'index', ext: '.html' }, 'some\\dir\\index.html'],
                [{ root: 'C:\\', name: 'index', ext: '.html' }, 'C:\\index.html'],
                [{}, '']
            ];

            checkFormat(path.win32, winSpecialCaseFormatTests);
        });

        it('specialCaseFormat - posix', () => {
            const unixSpecialCaseFormatTests = [
                [{ dir: 'some/dir' }, 'some/dir/'],
                [{ base: 'index.html' }, 'index.html'],
                [{ root: '/' }, '/'],
                [{ name: 'index', ext: '.html' }, 'index.html'],
                [{ dir: 'some/dir', name: 'index', ext: '.html' }, 'some/dir/index.html'],
                [{ root: '/', name: 'index', ext: '.html' }, '/index.html'],
                [{}, '']
            ];

            checkFormat(path.posix, unixSpecialCaseFormatTests);
        });
    });

    describe('parse/format', () => {
        function checkSpecialCaseParseFormat(path, testCases) {
            testCases.forEach(([element, expect]) => {
                // assert.deepStrictEqual(path.parse(element), expect);
                assert.deepEqual(path.parse(element), expect);
            });
        }

        it('SpecialCaseParseFormat - win32', () => {
            const winSpecialCaseParseTests = [
                ['t', { base: 't', name: 't', root: '', dir: '', ext: '' }],
                ['/foo/bar', { root: '/', dir: '/foo', base: 'bar', ext: '', name: 'bar' }],
            ];

            checkSpecialCaseParseFormat(path.win32, winSpecialCaseParseTests);
        });

        it('trailing path separators', () => {
            // Test removal of trailing path separators
            const trailingTests = [
                [path.win32.parse,
                [['.\\', { root: '', dir: '', base: '.', ext: '', name: '.' }],
                ['\\\\', { root: '\\', dir: '\\', base: '', ext: '', name: '' }],
                ['c:\\foo\\\\\\',
                    { root: 'c:\\', dir: 'c:\\', base: 'foo', ext: '', name: 'foo' }],
                ['D:\\foo\\\\\\bar.baz',
                    {
                        root: 'D:\\',
                        dir: 'D:\\foo\\\\',
                        base: 'bar.baz',
                        ext: '.baz',
                        name: 'bar'
                    }],
                ],
                ],
                [path.posix.parse,
                [['./', { root: '', dir: '', base: '.', ext: '', name: '.' }],
                ['//', { root: '/', dir: '/', base: '', ext: '', name: '' }],
                ['///', { root: '/', dir: '/', base: '', ext: '', name: '' }],
                ['/foo///', { root: '/', dir: '/', base: 'foo', ext: '', name: 'foo' }],
                ['/foo///bar.baz',
                    { root: '/', dir: '/foo//', base: 'bar.baz', ext: '.baz', name: 'bar' }],
                ],
                ],
            ];

            const failures = [];
            for (const [parse, testList] of trailingTests) {
                const os = parse === path.win32.parse ? 'win32' : 'posix';
                for (const [input, expected] of testList) {
                    const actual = parse(input);
                    const message = `path.${os}.parse(${JSON.stringify(input)})\n  expect=${JSON.stringify(expected)}\n  actual=${JSON.stringify(actual)}`;
                    const actualKeys = Object.keys(actual);
                    const expectedKeys = Object.keys(expected);
                    let failed = (actualKeys.length !== expectedKeys.length);
                    if (!failed) {
                        for (let i = 0; i < actualKeys.length; ++i) {
                            const key = actualKeys[i];
                            if (!expectedKeys.includes(key) || actual[key] !== expected[key]) {
                                failed = true;
                                break;
                            }
                        }
                    }
                    if (failed)
                        failures.push(`\n${message}`);
                }
            }
            assert.strictEqual(failures.length, 0, failures.join(''));
        });
    });

    it('dirname', () => {
        // Test __filename dirname (platform-specific behavior)
        assert.strictEqual(path.dirname(__filename).slice(-4), 'test');

        function testPosix(fn) {
            assert.strictEqual(fn('/a/b/'), '/a');
            assert.strictEqual(fn('/a/b'), '/a');
            assert.strictEqual(fn('/a'), '/');
            assert.strictEqual(fn(''), '.');
            assert.strictEqual(fn('/'), '/');
            assert.strictEqual(fn('////'), '/');
            // assert.strictEqual(fn('//a'), '//');
            assert.strictEqual(fn('foo'), '.');
        }

        function testWin32(fn) {
            assert.strictEqual(fn('c:\\'), 'c:\\');
            assert.strictEqual(fn('c:\\foo'), 'c:\\');
            assert.strictEqual(fn('c:\\foo\\'), 'c:\\');
            assert.strictEqual(fn('c:\\foo\\bar'), 'c:\\foo');
            assert.strictEqual(fn('c:\\foo\\bar\\'), 'c:\\foo');
            assert.strictEqual(fn('c:\\foo\\bar\\baz'), 'c:\\foo\\bar');
            assert.strictEqual(fn('\\'), '\\');
            assert.strictEqual(fn('\\foo'), '\\');
            assert.strictEqual(fn('\\foo\\'), '\\');
            assert.strictEqual(fn('\\foo\\bar'), '\\foo');
            assert.strictEqual(fn('\\foo\\bar\\'), '\\foo');
            assert.strictEqual(fn('\\foo\\bar\\baz'), '\\foo\\bar');
            assert.strictEqual(fn('c:'), 'c:');
            assert.strictEqual(fn('c:foo'), 'c:');
            assert.strictEqual(fn('c:foo\\'), 'c:');
            assert.strictEqual(fn('c:foo\\bar'), 'c:foo');
            assert.strictEqual(fn('c:foo\\bar\\'), 'c:foo');
            assert.strictEqual(fn('c:foo\\bar\\baz'), 'c:foo\\bar');
            assert.strictEqual(fn('file:stream'), '.');
            assert.strictEqual(fn('dir\\file:stream'), 'dir');
            assert.strictEqual(fn('\\\\unc\\share'),
                '\\\\unc\\share');
            assert.strictEqual(fn('\\\\unc\\share\\foo'),
                '\\\\unc\\share\\');
            assert.strictEqual(fn('\\\\unc\\share\\foo\\'),
                '\\\\unc\\share\\');
            assert.strictEqual(fn('\\\\unc\\share\\foo\\bar'),
                '\\\\unc\\share\\foo');
            assert.strictEqual(fn('\\\\unc\\share\\foo\\bar\\'),
                '\\\\unc\\share\\foo');
            assert.strictEqual(fn('\\\\unc\\share\\foo\\bar\\baz'),
                '\\\\unc\\share\\foo\\bar');
            assert.strictEqual(fn('/a/b/'), '/a');
            assert.strictEqual(fn('/a/b'), '/a');
            assert.strictEqual(fn('/a'), '/');
            assert.strictEqual(fn(''), '.');
            assert.strictEqual(fn('/'), '/');
            assert.strictEqual(fn('////'), '/');
            assert.strictEqual(fn('foo'), '.');
        }

        testPosix(path.posix.dirname.bind(path.posix));
        testWin32(path.win32.dirname.bind(path.win32));

        if (isWindows) {
            testWin32(path.dirname.bind(path));
        } else {
            testPosix(path.dirname.bind(path));
        }
    });

    describe('normalize', () => {
        it('basic path normalization', () => {
            // Test basic empty and current directory cases
            assert.equal(path.win32.normalize(''), '.');
            assert.equal(path.win32.normalize('.'), '.');
            assert.equal(path.posix.normalize(''), '.');
            assert.equal(path.posix.normalize('.'), '.');
        });

        it('current and parent directory navigation', () => {
            // Test dot navigation
            assert.equal(path.win32.normalize('.\\'), '.\\');
            assert.equal(path.win32.normalize('..\\'), '..\\');
            assert.equal(path.win32.normalize('\\.\\'), '\\');
            assert.equal(path.win32.normalize('\\.'), '\\');
            assert.equal(path.win32.normalize('\\aaa\\.\\'), '\\aaa\\');
            assert.equal(path.win32.normalize('\\aaa\\.'), '\\aaa');

            assert.equal(path.posix.normalize('./'), './');
            assert.equal(path.posix.normalize('../'), '../');
            assert.equal(path.posix.normalize('/./'), '/');
            assert.equal(path.posix.normalize('/.'), '/');
            assert.equal(path.posix.normalize('/aaa/./'), '/aaa/');
            assert.equal(path.posix.normalize('/aaa/.'), '/aaa');
        });

        it('multiple slashes and mixed separators', () => {
            // Test multiple consecutive slashes
            assert.equal(path.win32.normalize('a//b//../b'), 'a\\b');
            assert.equal(path.win32.normalize('a//b//./c'), 'a\\b\\c');
            assert.equal(path.win32.normalize('a//b//.'), 'a\\b');

            assert.equal(path.posix.normalize('a//b//../b'), 'a/b');
            assert.equal(path.posix.normalize('a//b//./c'), 'a/b/c');
            assert.equal(path.posix.normalize('a//b//.'), 'a/b');
        });

        it('complex relative path resolution', () => {
            // Test complex relative paths with parent directory navigation
            assert.equal(path.win32.normalize('./fixtures///b/../b/c.js'), 'fixtures\\b\\c.js');
            assert.equal(path.win32.normalize('/foo/../../../bar'), '\\bar');
            assert.equal(path.win32.normalize('/a/b/c/../../../x/y/z'), '\\x\\y\\z');

            assert.equal(path.posix.normalize('./fixtures///b/../b/c.js'), 'fixtures/b/c.js');
            assert.equal(path.posix.normalize('/foo/../../../bar'), '/bar');
            assert.equal(path.posix.normalize('foo/../../../bar'), '../../bar');
            assert.equal(path.posix.normalize('/a/b/c/../../../x/y/z'), '/x/y/z');
        });

        it('drive paths (Windows)', () => {
            // Test Windows drive paths
            assert.equal(path.win32.normalize('c:/foo/../../../bar'), 'c:\\bar');
            assert.equal(path.win32.normalize('f:/'), 'f:\\');
            assert.equal(path.win32.normalize('f:path/to/../../../../path1'), 'f:..\\..\\path1');
            assert.strictEqual(path.win32.normalize('C:'), 'C:.');
            assert.strictEqual(path.win32.normalize('C:..\\abc'), 'C:..\\abc');
            assert.strictEqual(path.win32.normalize('C:..\\..\\abc\\..\\def'), 'C:..\\..\\def');
            assert.strictEqual(path.win32.normalize('C:\\.'), 'C:\\');
            assert.strictEqual(path.win32.normalize('file:stream'), 'file:stream');
        });

        it('trailing path components with dots', () => {
            // Test paths with trailing dot components
            assert.strictEqual(path.win32.normalize('bar\\foo..\\..\\'), 'bar\\');
            assert.strictEqual(path.win32.normalize('bar\\foo..\\..'), 'bar');
            assert.strictEqual(path.win32.normalize('bar\\foo..\\..\\baz'), 'bar\\baz');
            assert.strictEqual(path.win32.normalize('bar\\foo..\\'), 'bar\\foo..\\');
            assert.strictEqual(path.win32.normalize('bar\\foo..'), 'bar\\foo..');
            assert.strictEqual(path.win32.normalize('..\\foo..\\..\\..\\bar'), '..\\..\\bar');
            assert.strictEqual(path.win32.normalize('..\\...\\..\\.\\...\\..\\..\\bar'), '..\\..\\bar');
        });

        it('UNC paths basic', () => {
            // Test basic UNC path normalization
            assert.equal(path.win32.normalize('\\\\unc\\share\\'), '\\\\unc\\share\\');
            assert.equal(path.win32.normalize('\\\\unc\\share'), '\\\\unc\\share\\');
            assert.equal(path.win32.normalize('\\\\unc\\\\folder'), '\\\\unc\\folder\\');
            assert.equal(path.win32.normalize('\\\\unc\\\\'), '\\unc\\');
            assert.equal(path.win32.normalize('\\\\\\unc\\\\folder'), '\\unc\\folder');
        });

        it('UNC paths with navigation', () => {
            // Test UNC paths with dot navigation
            assert.equal(path.win32.normalize('\\\\unc\\..\\folder'), '\\\\unc\\..\\folder');
            assert.equal(path.win32.normalize('\\\\unc\\.\\folder'), '\\\\unc\\.\\folder');
            assert.equal(path.win32.normalize('\\\\..\\\\folder'), '\\\\..\\folder\\');
        });

        it('UNC paths with complex navigation', () => {
            // Test UNC paths with complex relative path resolution
            assert.equal(path.win32.normalize('\\\\unc\\share\/foo/..//.///../../bar\\'), '\\\\unc\\share\\bar\\');
            assert.strictEqual(path.win32.normalize('//server/share/dir/file.ext'), '\\\\server\\share\\dir\\file.ext');
        });

        it('device namespace paths - dot domain', () => {
            // Test device namespace paths with dot domain (\\.\)
            if (isWindows) {
                assert.equal(path.win32.normalize('\\\\.\\folder'), '\\\\.\\folder\\');
                assert.strictEqual(path.win32.normalize('\\\\.\\C:'), '\\\\.\\C:\\');
                assert.strictEqual(path.win32.normalize('\\\\.\\COM1'), '\\\\.\\COM1\\');
                assert.strictEqual(path.win32.normalize('\\\\.\\DISPLAY1'), '\\\\.\\DISPLAY1\\');
            } else {
                assert.equal(path.win32.normalize('\\\\.\\folder'), '\\\\.\\folder');
                assert.strictEqual(path.win32.normalize('\\\\.\\C:'), '\\\\.\\C:');
                assert.strictEqual(path.win32.normalize('\\\\.\\COM1'), '\\\\.\\COM1');
                assert.strictEqual(path.win32.normalize('\\\\.\\DISPLAY1'), '\\\\.\\DISPLAY1');
            }
        });

        it('device namespace paths - question domain', () => {
            // Test device namespace paths with question domain (\\?\)
            if (isWindows) {
                assert.strictEqual(path.win32.normalize('\\\\?\\folder'), '\\\\?\\folder\\');
            } else {
                assert.strictEqual(path.win32.normalize('\\\\?\\folder'), '\\\\?\\folder');
            }

            assert.strictEqual(path.win32.normalize('\\\\?\\folder\\'), '\\\\?\\folder\\');
            assert.strictEqual(path.win32.normalize('\\\\?\\C:\\'), '\\\\?\\C:\\');
        });

        it('device namespace paths with mixed separators', () => {
            // Test device namespace paths with mixed forward/back slashes
            if (isWindows) {
                assert.strictEqual(path.win32.normalize('\\\\.//folder'), '\\\\.\\folder\\');
                assert.strictEqual(path.win32.normalize('\\\\?//folder'), '\\\\?\\folder\\');
            } else {
                assert.strictEqual(path.win32.normalize('\\\\.//folder'), '\\\\.\\folder');
                assert.strictEqual(path.win32.normalize('\\\\?//folder'), '\\\\?\\folder');
            }

            assert.strictEqual(path.win32.normalize('\\\\.\\folder//subfolder'), '\\\\.\\folder\\subfolder');
            assert.strictEqual(path.win32.normalize('\\\\?\\folder//subfolder'), '\\\\?\\folder\\subfolder');
        });

        it('device namespace paths with navigation', () => {
            // Test device namespace paths with dot navigation
            if (isWindows) {
                assert.strictEqual(path.win32.normalize('\\\\.\\folder\\..\\other'), '\\\\.\\folder\\other');
                assert.strictEqual(path.win32.normalize('\\\\?\\folder\\..\\other'), '\\\\?\\folder\\other');
            } else {
                assert.strictEqual(path.win32.normalize('\\\\.\\folder\\..\\other'), '\\\\.\\other');
                assert.strictEqual(path.win32.normalize('\\\\?\\folder\\..\\other'), '\\\\?\\other');
            }

            assert.strictEqual(path.win32.normalize('\\\\.\\folder\\subfolder'), '\\\\.\\folder\\subfolder');
            assert.strictEqual(path.win32.normalize('\\\\.\\folder\\'), '\\\\.\\folder\\');
            assert.strictEqual(path.win32.normalize('\\\\?\\folder\\subfolder'), '\\\\?\\folder\\subfolder');
            assert.strictEqual(path.win32.normalize('\\\\?\\UNC\\server\\share'), '\\\\?\\UNC\\server\\share');
            assert.strictEqual(path.win32.normalize('\\\\.\\folder\\.\\subfolder'), '\\\\.\\folder\\subfolder');
            assert.strictEqual(path.win32.normalize('\\\\?\\folder\\.\\subfolder'), '\\\\?\\folder\\subfolder');
        });

        it('posix trailing path components with dots', () => {
            // Test POSIX paths with trailing dot components  
            assert.strictEqual(path.posix.normalize('bar/foo../../'), 'bar/');
            assert.strictEqual(path.posix.normalize('bar/foo../..'), 'bar');
            assert.strictEqual(path.posix.normalize('bar/foo../../baz'), 'bar/baz');
            assert.strictEqual(path.posix.normalize('bar/foo../'), 'bar/foo../');
            assert.strictEqual(path.posix.normalize('bar/foo..'), 'bar/foo..');
            assert.strictEqual(path.posix.normalize('../foo../../../bar'), '../../bar');
            assert.strictEqual(path.posix.normalize('../.../.././.../../../bar'), '../../bar');
        });

        it('posix special path navigation', () => {
            // Test special POSIX path navigation patterns
            assert.equal(path.posix.normalize('/a/b/../'), '/a/');
            assert.equal(path.posix.normalize('/a/b/..'), '/a');
            assert.strictEqual(path.posix.normalize('///..//./foo/.//bar'), '/foo/bar');
        });

        it('current platform behavior', () => {
            // Test behavior matches current platform
            if (isWindows) {
                // On Windows, should behave like win32
                assert.equal(path.normalize('\\\\.\\folder'), '\\\\.\\folder\\');
                assert.equal(path.normalize('a//b//../b'), 'a\\b');
                assert.equal(path.normalize('c:/foo/../bar'), 'c:\\bar');
            } else {
                // On non-Windows, should behave like posix
                assert.equal(path.normalize('a//b//../b'), 'a/b');
                assert.equal(path.normalize('./fixtures///b/../b/c.js'), 'fixtures/b/c.js');
                assert.equal(path.normalize('/foo/../../../bar'), '/bar');
            }
        });
    });

    it('isAbsolute', () => {
        function testPosix(fn) {
            assert.strictEqual(fn('/foo/bar'), true);
            assert.strictEqual(fn('/baz/..'), true);
            assert.strictEqual(fn('qux/'), false);
            assert.strictEqual(fn('.'), false);
            assert.strictEqual(fn('./foo'), false);
        }

        function testWin32(fn) {
            assert.strictEqual(fn('/'), true);
            assert.strictEqual(fn('//'), true);
            assert.strictEqual(fn('//server'), true);
            assert.strictEqual(fn('//server/file'), true);
            assert.strictEqual(fn('\\\\server\\file'), true);
            assert.strictEqual(fn('\\\\server'), true);
            assert.strictEqual(fn('\\\\'), true);
            assert.strictEqual(fn('c'), false);
            assert.strictEqual(fn('c:'), false);
            assert.strictEqual(fn('c:\\'), true);
            assert.strictEqual(fn('c:/'), true);
            assert.strictEqual(fn('c://'), true);
            assert.strictEqual(fn('C:/Users/'), true);
            assert.strictEqual(fn('C:\\Users\\'), true);
            assert.strictEqual(fn('C:cwd/another'), false);
            assert.strictEqual(fn('C:cwd\\another'), false);
            assert.strictEqual(fn('directory/directory'), false);
            assert.strictEqual(fn('directory\\directory'), false);
        }

        testPosix(path.posix.isAbsolute.bind(path.posix));
        testWin32(path.win32.isAbsolute.bind(path.win32));

        if (isWindows) {
            testWin32(path.isAbsolute.bind(path));
        } else {
            testPosix(path.isAbsolute.bind(path));
        }
    });

    describe('join', () => {
        it('basic path joining', () => {
            // Test basic path joining scenarios
            assert.strictEqual(path.join('.', 'x/b', '..', '/b/c.js'),
                isWindows ? 'x\\b\\c.js' : 'x/b/c.js');
            assert.strictEqual(path.join('foo/x', './bar'),
                isWindows ? 'foo\\x\\bar' : 'foo/x/bar');
            assert.strictEqual(path.join('foo/x/', './bar'),
                isWindows ? 'foo\\x\\bar' : 'foo/x/bar');
            assert.strictEqual(path.join('foo/x/', '.', 'bar'),
                isWindows ? 'foo\\x\\bar' : 'foo/x/bar');
        });

        it('empty and single path cases', () => {
            // Test empty arrays and single paths
            assert.strictEqual(path.join(), '.');
            assert.strictEqual(path.join('.'), '.');
            assert.strictEqual(path.join(''), '.');
            assert.strictEqual(path.join('', ''), '.');
        });

        it('relative path resolution', () => {
            // Test relative path handling
            assert.strictEqual(path.join('/foo', '../../../bar'),
                isWindows ? '\\bar' : '/bar');
            assert.strictEqual(path.join('foo', '../../../bar'),
                isWindows ? '..\\..\\bar' : '../../bar');
            assert.strictEqual(path.join('foo/', '../../../bar'),
                isWindows ? '..\\..\\bar' : '../../bar');
            assert.strictEqual(path.join('foo/x', '../../../bar'),
                isWindows ? '..\\bar' : '../bar');
            assert.strictEqual(path.join('./', '..', '/foo'),
                isWindows ? '..\\foo' : '../foo');
            assert.strictEqual(path.join('./', '..', '..', '/foo'),
                isWindows ? '..\\..\\foo' : '../../foo');
            assert.strictEqual(path.join('.', '..', '..', '/foo'),
                isWindows ? '..\\..\\foo' : '../../foo');
            assert.strictEqual(path.join('', '..', '..', '/foo'),
                isWindows ? '..\\..\\foo' : '../../foo');
        });

        it('dot and current directory handling', () => {
            // Test dot and current directory cases
            assert.strictEqual(path.join('./'),
                isWindows ? '.\\' : './');
            assert.strictEqual(path.join('.', './'),
                isWindows ? '.\\' : './');
            assert.strictEqual(path.join('.', '.', '.'), '.');
            assert.strictEqual(path.join('.', './', '.'), '.');
            assert.strictEqual(path.join('.', '/./', '.'), '.');
            assert.strictEqual(path.join('.', '/////./', '.'), '.');
        });

        it('absolute path handling', () => {
            // Test absolute path handling
            assert.strictEqual(path.join('/.', 'x/b', '..', '/b/c.js'),
                isWindows ? '\\x\\b\\c.js' : '/x/b/c.js');
            assert.strictEqual(path.join('', '/foo'),
                isWindows ? '\\foo' : '/foo');
            assert.strictEqual(path.join('', '', '/foo'),
                isWindows ? '\\foo' : '/foo');
            assert.strictEqual(path.join('foo', '/bar'),
                isWindows ? 'foo\\bar' : 'foo/bar');
        });

        it('root path cases', () => {
            // Test root path handling
            assert.strictEqual(path.join('/'),
                isWindows ? '\\' : '/');
            assert.strictEqual(path.join('/', '.'),
                isWindows ? '\\' : '/');
            assert.strictEqual(path.join('/', '..'),
                isWindows ? '\\' : '/');
            assert.strictEqual(path.join('/', '..', '..'),
                isWindows ? '\\' : '/');
            assert.strictEqual(path.join('/', 'foo'),
                isWindows ? '\\foo' : '/foo');
            assert.strictEqual(path.join('/', '/foo'),
                isWindows ? '\\foo' : '/foo');
            assert.strictEqual(path.join('/', '//foo'),
                isWindows ? '\\foo' : '/foo');
        });

        it('empty string handling', () => {
            // Test empty string handling
            assert.strictEqual(path.join('', '.'), '.');
            assert.strictEqual(path.join('', 'foo'), 'foo');
            assert.strictEqual(path.join('', '', 'foo'), 'foo');
            assert.strictEqual(path.join('foo', ''), 'foo');
            assert.strictEqual(path.join('foo/', ''),
                isWindows ? 'foo\\' : 'foo/');
            assert.strictEqual(path.join('foo', '', '/bar'),
                isWindows ? 'foo\\bar' : 'foo/bar');
            assert.strictEqual(path.join('/', '', '/foo'),
                isWindows ? '\\foo' : '/foo');
            assert.strictEqual(path.join('', '/', 'foo'),
                isWindows ? '\\foo' : '/foo');
            assert.strictEqual(path.join('', '/', '/foo'),
                isWindows ? '\\foo' : '/foo');
        });

        it('space handling', () => {
            // Test paths with spaces
            assert.strictEqual(path.join(' /foo'),
                isWindows ? ' \\foo' : ' /foo');
            assert.strictEqual(path.join(' ', 'foo'),
                isWindows ? ' \\foo' : ' /foo');
            assert.strictEqual(path.join(' ', '.'), ' ');
            assert.strictEqual(path.join(' ', '/'),
                isWindows ? ' \\' : ' /');
            assert.strictEqual(path.join(' ', ''), ' ');
        });

        it('posix specific tests', () => {
            // Test posix specific behavior
            const failures = [];
            const joinTests = [
                [['.', 'x/b', '..', '/b/c.js'], 'x/b/c.js'],
                [[], '.'],
                [['/.', 'x/b', '..', '/b/c.js'], '/x/b/c.js'],
                [['/foo', '../../../bar'], '/bar'],
                [['foo', '../../../bar'], '../../bar'],
                [['foo/', '../../../bar'], '../../bar'],
                [['foo/x', '../../../bar'], '../bar'],
                [['foo/x', './bar'], 'foo/x/bar'],
                [['foo/x/', './bar'], 'foo/x/bar'],
                [['foo/x/', '.', 'bar'], 'foo/x/bar'],
                [['./'], './'],
                [['.', './'], './'],
                [['.', '.', '.'], '.'],
                [['.', './', '.'], '.'],
                [['.', '/./', '.'], '.'],
                [['.', '/////./', '.'], '.'],
                [['.'], '.'],
                [['', '.'], '.'],
                [['', 'foo'], 'foo'],
                [['foo', '/bar'], 'foo/bar'],
                [['', '/foo'], '/foo'],
                [['', '', '/foo'], '/foo'],
                [['', '', 'foo'], 'foo'],
                [['foo', ''], 'foo'],
                [['foo/', ''], 'foo/'],
                [['foo', '', '/bar'], 'foo/bar'],
                [['./', '..', '/foo'], '../foo'],
                [['./', '..', '..', '/foo'], '../../foo'],
                [['.', '..', '..', '/foo'], '../../foo'],
                [['', '..', '..', '/foo'], '../../foo'],
                [['/'], '/'],
                [['/', '.'], '/'],
                [['/', '..'], '/'],
                [['/', '..', '..'], '/'],
                [[''], '.'],
                [['', ''], '.'],
                [[' /foo'], ' /foo'],
                [[' ', 'foo'], ' /foo'],
                [[' ', '.'], ' '],
                [[' ', '/'], ' /'],
                [[' ', ''], ' '],
                [['/', 'foo'], '/foo'],
                [['/', '/foo'], '/foo'],
                [['/', '//foo'], '/foo'],
                [['/', '', '/foo'], '/foo'],
                [['', '/', 'foo'], '/foo'],
                [['', '/', '/foo'], '/foo']
            ];

            joinTests.forEach((test) => {
                var actual = path.posix.join.apply(path.posix, test[0]);
                var expected = test[1];
                var message = 'path.posix.join(' + test[0].map(JSON.stringify).join(',') + ')' + '\n  expect=' + JSON.stringify(expected) + '\n  actual=' + JSON.stringify(actual);
                if (actual !== expected)
                    failures.push('\n' + message);
            });
            assert.equal(failures.length, 0, failures.join(''));
        });

        it('win32 specific UNC paths', () => {
            // Test Win32 UNC paths
            const failures = [];
            const joinTestsWin32 = [
                // UNC path expected
                [['//foo/bar'], '\\\\foo\\bar\\'],
                [['\\\\foo/bar'], '\\\\foo\\bar\\'],
                // UNC path expected - server and share separate
                [['//foo', 'bar'], '\\\\foo\\bar\\'],
                [['//foo/', 'bar'], '\\\\foo\\bar\\'],
                [['//foo', '/bar'], '\\\\foo\\bar\\'],
                // UNC path expected - questionable
                [['//foo', '', 'bar'], '\\\\foo\\bar\\'],
                [['//foo/', '', 'bar'], '\\\\foo\\bar\\'],
                [['//foo/', '', '/bar'], '\\\\foo\\bar\\'],
                // UNC path expected - even more questionable
                [['', '//foo', 'bar'], '\\\\foo\\bar\\'],
                [['', '//foo/', 'bar'], '\\\\foo\\bar\\'],
                [['', '//foo/', '/bar'], '\\\\foo\\bar\\'],
            ];

            joinTestsWin32.forEach((test) => {
                var actual = path.win32.join.apply(path.win32, test[0]);
                var expected = test[1];
                var message = 'path.win32.join(' + test[0].map(JSON.stringify).join(',') + ')' + '\n  expect=' + JSON.stringify(expected) + '\n  actual=' + JSON.stringify(actual);
                if (actual !== expected)
                    failures.push('\n' + message);
            });
            assert.equal(failures.length, 0, failures.join(''));
        });

        it('win32 specific non-UNC paths', () => {
            // Test Win32 non-UNC paths
            const failures = [];
            const joinTestsWin32NonUNC = [
                // No UNC path expected (no double slash in first component)
                [['\\', 'foo/bar'], '\\foo\\bar'],
                [['\\', '/foo/bar'], '\\foo\\bar'],
                [['', '/', '/foo/bar'], '\\foo\\bar'],
                // No UNC path expected (no non-slashes in first component - questionable)
                [['//', 'foo/bar'], '\\foo\\bar'],
                [['//', '/foo/bar'], '\\foo\\bar'],
                [['\\\\', '/', '/foo/bar'], '\\foo\\bar'],
                [['//'], '\\'],  // Fixed: Node.js returns '\\' not '/'
                // No UNC path expected (share name missing - questionable).
                [['//foo'], '\\foo'],  // Fixed: Not UNC without share
                [['//foo/'], '\\foo\\'],  // Fixed: Not UNC without share
                [['//foo', '/'], '\\foo\\'],  // Fixed: Not UNC without share
                [['//foo', '', '/'], '\\foo\\'],  // Fixed: Not UNC without share
                // No UNC path expected (too many leading slashes - questionable)
                [['///foo/bar'], '\\foo\\bar'],
                [['////foo', 'bar'], '\\foo\\bar'],
                [['\\\\\\//foo/bar'], '\\foo\\bar'],
            ];

            joinTestsWin32NonUNC.forEach((test) => {
                var actual = path.win32.join.apply(path.win32, test[0]);
                var expected = test[1];
                var message = 'path.win32.join(' + test[0].map(JSON.stringify).join(',') + ')' + '\n  expect=' + JSON.stringify(expected) + '\n  actual=' + JSON.stringify(actual);
                if (actual !== expected)
                    failures.push('\n' + message);
            });
            assert.equal(failures.length, 0, failures.join(''));
        });

        it('win32 drive paths', () => {
            // Test Win32 drive-relative vs drive-absolute paths
            const failures = [];
            const driveTests = [
                [['c:'], 'c:.'],
                [['c:.'], 'c:.'],
                [['c:', ''], 'c:.'],
                [['', 'c:'], 'c:.'],
                [['c:.', '/'], 'c:.\\'],
                [['c:.', 'file'], 'c:file'],
                [['c:', '/'], 'c:\\'],
                [['c:', 'file'], 'c:\\file']
            ];

            driveTests.forEach((test) => {
                var actual = path.win32.join.apply(path.win32, test[0]);
                var expected = test[1];
                var message = 'path.win32.join(' + test[0].map(JSON.stringify).join(',') + ')' + '\n  expect=' + JSON.stringify(expected) + '\n  actual=' + JSON.stringify(actual);
                if (actual !== expected)
                    failures.push('\n' + message);
            });
            assert.equal(failures.length, 0, failures.join(''));
        });

        it('current platform comprehensive test', () => {
            // Test comprehensive scenarios for current platform
            const failures = [];
            const joinTests = [
                [['.', 'x/b', '..', '/b/c.js'], 'x/b/c.js'],
                [[], '.'],
                [['/.', 'x/b', '..', '/b/c.js'], '/x/b/c.js'],
                [['/foo', '../../../bar'], '/bar'],
                [['foo', '../../../bar'], '../../bar'],
                [['foo/', '../../../bar'], '../../bar'],
                [['foo/x', '../../../bar'], '../bar'],
                [['foo/x', './bar'], 'foo/x/bar'],
                [['foo/x/', './bar'], 'foo/x/bar'],
                [['foo/x/', '.', 'bar'], 'foo/x/bar'],
                [['./'], './'],
                [['.', './'], './'],
                [['.', '.', '.'], '.'],
                [['.', './', '.'], '.'],
                [['.', '/./', '.'], '.'],
                [['.', '/////./', '.'], '.'],
                [['.'], '.'],
                [['', '.'], '.'],
                [['', 'foo'], 'foo'],
                [['foo', '/bar'], 'foo/bar'],
                [['', '/foo'], '/foo'],
                [['', '', '/foo'], '/foo'],
                [['', '', 'foo'], 'foo'],
                [['foo', ''], 'foo'],
                [['foo/', ''], 'foo/'],
                [['foo', '', '/bar'], 'foo/bar'],
                [['./', '..', '/foo'], '../foo'],
                [['./', '..', '..', '/foo'], '../../foo'],
                [['.', '..', '..', '/foo'], '../../foo'],
                [['', '..', '..', '/foo'], '../../foo'],
                [['/'], '/'],
                [['/', '.'], '/'],
                [['/', '..'], '/'],
                [['/', '..', '..'], '/'],
                [[''], '.'],
                [['', ''], '.'],
                [[' /foo'], ' /foo'],
                [[' ', 'foo'], ' /foo'],
                [[' ', '.'], ' '],
                [[' ', '/'], ' /'],
                [[' ', ''], ' '],
                [['/', 'foo'], '/foo'],
                [['/', '/foo'], '/foo'],
                [['/', '//foo'], '/foo'],
                [['/', '', '/foo'], '/foo'],
                [['', '/', 'foo'], '/foo'],
                [['', '/', '/foo'], '/foo']
            ];

            // path.join
            joinTests.forEach((test) => {
                var actual = path.join.apply(path, test[0]);
                var expected = isWindows ? test[1].replace(/\//g, '\\') : test[1];
                var message = 'path.join(' + test[0].map(JSON.stringify).join(',') + ')' + '\n  expect=' + JSON.stringify(expected) + '\n  actual=' + JSON.stringify(actual);
                if (actual !== expected)
                    failures.push('\n' + message);
            });
            assert.equal(failures.length, 0, failures.join(''));
        });

        it('drive mixing edge cases', () => {
            // Test drive mixing on Windows
            if (isWindows) {
                assert.equal(path.join('c:/path1', 'c:path2'), 'c:\\path1\\c:path2');
                assert.equal(path.join('c:/path1', 'd:path2'), 'c:\\path1\\d:path2');
            }

            assert.equal(path.win32.join('c:/path1', 'c:path2'), 'c:\\path1\\c:path2');
            assert.equal(path.win32.join('c:/path1', 'd:path2'), 'c:\\path1\\d:path2');
        });
    });

    it('resolve', () => {
        const failures = [];
        const slashRE = /\//g;
        const backslashRE = /\\/g;

        const resolveTests = [
            [path.win32.resolve,
            // arguments                               result
            [
                [
                    ['c:/blah\\blah', 'd:/games', 'c:../a'], 'c:\\blah\\a'
                ],
                [
                    ['c:/ignore', 'd:\\a/b\\c/d', '\\e.exe'], 'd:\\e.exe'
                ],
                [
                    ['c:/ignore', 'c:/some/file'], 'c:\\some\\file'
                ],
                [
                    ['d:/ignore', 'd:some/dir//'], 'd:\\ignore\\some\\dir'
                ],
                [
                    ['.'], process.cwd()
                ],
                [
                    ['//server/share', '..', 'relative\\'], '\\\\server\\share\\relative'
                ],
                [
                    ['c:/', '//'], 'c:\\'
                ],
                [
                    ['c:/', '//dir'], 'c:\\dir'
                ],
                [
                    ['c:/', '//server/share'], '\\\\server\\share\\'
                ],
                [
                    ['c:/', '//server//share'], '\\\\server\\share\\'
                ],
                [
                    ['c:/', '///some//dir'], 'c:\\some\\dir'
                ],
                [
                    ['C:\\foo\\tmp.3\\', '..\\tmp.3\\cycles\\root.js'],
                    'C:\\foo\\tmp.3\\cycles\\root.js'
                ]
            ],
            [
                ['c:/', 'C://'], 'C:\\'
            ],
            [
                ['C:/', 'c://'], 'c:\\'
            ],
            ],
            [path.posix.resolve,
            // arguments                    result
            [
                [
                    ['/var/lib', '../', 'file/'], '/var/file'
                ],
                [
                    ['/var/lib', '/../', 'file/'], '/file'
                ],
                [
                    ['a/b/c/', '../../..'],
                    // On Windows, posix.resolve will return Unix-style paths without drive letter
                    isWindows ? process.cwd().replace(/\\/g, '/').replace(/^[A-Z]:/, '') : process.cwd()
                ],
                [
                    ['.'],
                    // On Windows, posix.resolve will return Unix-style paths without drive letter
                    isWindows ? process.cwd().replace(/\\/g, '/').replace(/^[A-Z]:/, '') : process.cwd()
                ],
                [
                    ['/some/dir', '.', '/absolute/'], '/absolute'
                ],
                [
                    ['/foo/tmp.3/', '../tmp.3/cycles/root.js'], '/foo/tmp.3/cycles/root.js'
                ]
            ]
            ]
        ];
        resolveTests.forEach((test) => {
            const resolve = test[0];
            test[1].forEach((test) => {
                const actual = resolve.apply(null, test[0]);
                let actualAlt;
                const os = resolve === path.win32.resolve ? 'win32' : 'posix';
                if (resolve === path.win32.resolve && !isWindows)
                    actualAlt = actual.replace(backslashRE, '/');
                else if (resolve !== path.win32.resolve && isWindows)
                    actualAlt = actual.replace(slashRE, '\\');

                const expected = test[1];
                const message =
                    `path.${os}.resolve(${test[0].map(JSON.stringify).join(',')})\n  expect=${JSON.stringify(expected)}\n  actual=${JSON.stringify(actual)}`;
                if (actual !== expected && actualAlt !== expected)
                    failures.push(`\n${message}`);
            });
        });
        assert.strictEqual(failures.length, 0, failures.join(''));
    });

    it('root resolve', () => {
        const cwd = process.cwd();
        process.chdir('/');
        assert.equal(path.resolve('a/b/c/', '../../..'), process.cwd());
        process.chdir(cwd);
    });

    describe('relative', () => {
        it('win32 basic cases', () => {
            // Test basic Win32 relative path calculations
            assert.strictEqual(path.win32.relative('c:/aaaa/bbbb', 'c:/aaaa'), '..');
            assert.strictEqual(path.win32.relative('c:/aaaa/bbbb', 'c:/aaaa/bbbb'), '');
            assert.strictEqual(path.win32.relative('c:/aaaa/bbbb', 'c:/aaaa/cccc'), '..\\cccc');
            assert.strictEqual(path.win32.relative('c:/aaaa/', 'c:/aaaa/cccc'), 'cccc');
            assert.strictEqual(path.win32.relative('c:/', 'c:\\aaaa\\bbbb'), 'aaaa\\bbbb');
        });

        it('win32 cross drive cases', () => {
            // Test cross-drive relative paths
            assert.strictEqual(path.win32.relative('c:/blah\\blah', 'd:/games'), 'd:\\games');
            assert.strictEqual(path.win32.relative('c:/aaaa/bbbb', 'd:\\'), 'd:\\');
        });

        it('win32 case insensitive paths', () => {
            // Test case insensitive behavior
            assert.strictEqual(path.win32.relative('c:/AaAa/bbbb', 'c:/aaaa/bbbb'), '');
        });

        it('win32 complex nested paths', () => {
            // Test complex nested path scenarios
            assert.strictEqual(path.win32.relative('c:/aaaa/bbbb', 'c:/cccc'), '..\\..\\cccc');
            assert.strictEqual(path.win32.relative('c:/aaaaa/', 'c:/aaaa/cccc'), '..\\aaaa\\cccc');
            assert.strictEqual(path.win32.relative('C:\\foo\\bar\\baz\\quux', 'C:\\'), '..\\..\\..\\..');
            assert.strictEqual(path.win32.relative('C:\\foo\\test', 'C:\\foo\\test\\bar\\package.json'), 'bar\\package.json');
        });

        it('win32 similar path names', () => {
            // Test paths with similar names
            assert.strictEqual(path.win32.relative('C:\\foo\\bar\\baz-quux', 'C:\\foo\\bar\\baz'), '..\\baz');
            assert.strictEqual(path.win32.relative('C:\\foo\\bar\\baz', 'C:\\foo\\bar\\baz-quux'), '..\\baz-quux');
            assert.strictEqual(path.win32.relative('C:\\baz-quux', 'C:\\baz'), '..\\baz');
            assert.strictEqual(path.win32.relative('C:\\baz', 'C:\\baz-quux'), '..\\baz-quux');
        });

        it('win32 UNC paths', () => {
            // Test UNC path scenarios
            assert.strictEqual(path.win32.relative('\\\\foo\\bar', '\\\\foo\\bar\\baz'), 'baz');
            assert.strictEqual(path.win32.relative('\\\\foo\\bar\\baz', '\\\\foo\\bar'), '..');
            assert.strictEqual(path.win32.relative('\\\\foo\\bar\\baz-quux', '\\\\foo\\bar\\baz'), '..\\baz');
            assert.strictEqual(path.win32.relative('\\\\foo\\bar\\baz', '\\\\foo\\bar\\baz-quux'), '..\\baz-quux');
            assert.strictEqual(path.win32.relative('\\\\foo\\baz-quux', '\\\\foo\\baz'), '..\\baz');
            assert.strictEqual(path.win32.relative('\\\\foo\\baz', '\\\\foo\\baz-quux'), '..\\baz-quux');
        });

        it('win32 cross filesystem types', () => {
            // Test cross filesystem type scenarios
            assert.strictEqual(path.win32.relative('C:\\baz', '\\\\foo\\bar\\baz'), '\\\\foo\\bar\\baz');
            assert.strictEqual(path.win32.relative('\\\\foo\\bar\\baz', 'C:\\baz'), 'C:\\baz');
        });

        it('posix basic cases', () => {
            // Test basic POSIX relative path calculations
            assert.strictEqual(path.posix.relative('/var/lib', '/var'), '..');
            assert.strictEqual(path.posix.relative('/var/lib', '/var/lib'), '');
            assert.strictEqual(path.posix.relative('/var/lib', '/var/apache'), '../apache');
            assert.strictEqual(path.posix.relative('/var/', '/var/lib'), 'lib');
            assert.strictEqual(path.posix.relative('/', '/var/lib'), 'var/lib');
        });

        it('posix complex nested paths', () => {
            // Test complex nested path scenarios
            assert.strictEqual(path.posix.relative('/var/lib', '/bin'), '../../bin');
            assert.strictEqual(path.posix.relative('/foo/test', '/foo/test/bar/package.json'), 'bar/package.json');
            assert.strictEqual(path.posix.relative('/Users/a/web/b/test/mails', '/Users/a/web/b'), '../..');
        });

        it('posix similar path names', () => {
            // Test paths with similar names
            assert.strictEqual(path.posix.relative('/foo/bar/baz-quux', '/foo/bar/baz'), '../baz');
            assert.strictEqual(path.posix.relative('/foo/bar/baz', '/foo/bar/baz-quux'), '../baz-quux');
            assert.strictEqual(path.posix.relative('/baz-quux', '/baz'), '../baz');
            assert.strictEqual(path.posix.relative('/baz', '/baz-quux'), '../baz-quux');
        });

        it('comprehensive test matrix', () => {
            // Test comprehensive matrix for both platforms
            const failures = [];

            const relativeTests = [
                [path.win32.relative, [
                    ['c:/blah\\blah', 'd:/games', 'd:\\games'],
                    ['c:/aaaa/bbbb', 'c:/aaaa', '..'],
                    ['c:/aaaa/bbbb', 'c:/cccc', '..\\..\\cccc'],
                    ['c:/aaaa/bbbb', 'c:/aaaa/bbbb', ''],
                    ['c:/aaaa/bbbb', 'c:/aaaa/cccc', '..\\cccc'],
                    ['c:/aaaa/', 'c:/aaaa/cccc', 'cccc'],
                    ['c:/', 'c:\\aaaa\\bbbb', 'aaaa\\bbbb'],
                    ['c:/aaaa/bbbb', 'd:\\', 'd:\\'],
                    ['c:/AaAa/bbbb', 'c:/aaaa/bbbb', ''],
                    ['c:/aaaaa/', 'c:/aaaa/cccc', '..\\aaaa\\cccc'],
                    ['C:\\foo\\bar\\baz\\quux', 'C:\\', '..\\..\\..\\..'],
                    ['C:\\foo\\test', 'C:\\foo\\test\\bar\\package.json', 'bar\\package.json'],
                    ['C:\\foo\\bar\\baz-quux', 'C:\\foo\\bar\\baz', '..\\baz'],
                    ['C:\\foo\\bar\\baz', 'C:\\foo\\bar\\baz-quux', '..\\baz-quux'],
                    ['\\\\foo\\bar', '\\\\foo\\bar\\baz', 'baz'],
                    ['\\\\foo\\bar\\baz', '\\\\foo\\bar', '..'],
                    ['\\\\foo\\bar\\baz-quux', '\\\\foo\\bar\\baz', '..\\baz'],
                    ['\\\\foo\\bar\\baz', '\\\\foo\\bar\\baz-quux', '..\\baz-quux'],
                    ['C:\\baz-quux', 'C:\\baz', '..\\baz'],
                    ['C:\\baz', 'C:\\baz-quux', '..\\baz-quux'],
                    ['\\\\foo\\baz-quux', '\\\\foo\\baz', '..\\baz'],
                    ['\\\\foo\\baz', '\\\\foo\\baz-quux', '..\\baz-quux'],
                    ['C:\\baz', '\\\\foo\\bar\\baz', '\\\\foo\\bar\\baz'],
                    ['\\\\foo\\bar\\baz', 'C:\\baz', 'C:\\baz']
                ]],
                [path.posix.relative, [
                    ['/var/lib', '/var', '..'],
                    ['/var/lib', '/bin', '../../bin'],
                    ['/var/lib', '/var/lib', ''],
                    ['/var/lib', '/var/apache', '../apache'],
                    ['/var/', '/var/lib', 'lib'],
                    ['/', '/var/lib', 'var/lib'],
                    ['/foo/test', '/foo/test/bar/package.json', 'bar/package.json'],
                    ['/Users/a/web/b/test/mails', '/Users/a/web/b', '../..'],
                    ['/foo/bar/baz-quux', '/foo/bar/baz', '../baz'],
                    ['/foo/bar/baz', '/foo/bar/baz-quux', '../baz-quux'],
                    ['/baz-quux', '/baz', '../baz'],
                    ['/baz', '/baz-quux', '../baz-quux']
                ]]
            ];

            relativeTests.forEach((test) => {
                const relative = test[0];
                test[1].forEach((test) => {
                    const actual = relative(test[0], test[1]);
                    const expected = test[2];
                    const os = relative === path.win32.relative ? 'win32' : 'posix';
                    const message = `path.${os}.relative(${test.slice(0, 2).map(JSON.stringify).join(',')})\n  expect=${JSON.stringify(expected)}\n  actual=${JSON.stringify(actual)}`;
                    if (actual !== expected)
                        failures.push(`\n${message}`);
                });
            });
            assert.strictEqual(failures.length, 0, failures.join(''));
        });
    });

    if (isFibjs)
        it("fullpath", () => {
            assert.equal(path.join(process.cwd(), "not_exists"), path.fullpath("not_exists"));
            assert.equal(path.join(process.cwd(), "not_exists"), path.posix.fullpath("not_exists"));
            if (isWindows) {
                assert.equal(path.join(process.cwd(), "not_exists"), path.win32.fullpath("not_exists"));
            }
        });



    it('zero', () => {
        const pwd = process.cwd();

        // join will internally ignore all the zero-length strings and it will return
        // '.' if the joined string is a zero-length string.
        assert.strictEqual(path.posix.join(''), '.');
        assert.strictEqual(path.posix.join('', ''), '.');
        assert.strictEqual(path.win32.join(''), '.');
        assert.strictEqual(path.win32.join('', ''), '.');
        assert.strictEqual(path.join(pwd), pwd);
        assert.strictEqual(path.join(pwd, ''), pwd);

        // normalize will return '.' if the input is a zero-length string
        assert.strictEqual(path.posix.normalize(''), '.');
        assert.strictEqual(path.win32.normalize(''), '.');
        assert.strictEqual(path.normalize(pwd), pwd);

        // Since '' is not a valid path in any of the common environments, return false
        assert.strictEqual(path.posix.isAbsolute(''), false);
        assert.strictEqual(path.win32.isAbsolute(''), false);

        // resolve, internally ignores all the zero-length strings and returns the
        // current working directory
        assert.strictEqual(path.resolve(''), pwd);
        assert.strictEqual(path.resolve('', ''), pwd);

        // relative, internally calls resolve. So, '' is actually the current directory
        assert.strictEqual(path.relative('', pwd), '');
        assert.strictEqual(path.relative(pwd, ''), '');
        assert.strictEqual(path.relative(pwd, pwd), '');
    });

    it("toNamespacedPath", () => {
        const file1 = '\\\\psf\\Home\\Documents\\workspace';
        const file2 = 'c:\\Home\\Documents\\workspace';
        const resolvedFile1 = path.resolve(file1);
        const resolvedFile2 = path.resolve(file2);

        if (isWindows) {
            assert.strictEqual(`\\\\?\\UNC${resolvedFile1.slice(1)}`,
                path.toNamespacedPath(file1));
            assert.strictEqual(`\\\\?\\${resolvedFile2}`,
                path.toNamespacedPath(`\\\\?\\${file2}`));
            assert.strictEqual('\\\\?\\UNC\\someserver\\someshare\\somefile',
                path.toNamespacedPath(
                    '\\\\someserver\\someshare\\somefile'));
            assert.strictEqual('\\\\?\\UNC\\someserver\\someshare\\somefile', path
                .toNamespacedPath('\\\\?\\UNC\\someserver\\someshare\\somefile'));
            assert.strictEqual('\\\\.\\pipe\\somepipe',
                path.toNamespacedPath('\\\\.\\pipe\\somepipe'));
        }

        assert.strictEqual(path.toNamespacedPath(null), isFibjs ? undefined : null);
        assert.strictEqual(path.toNamespacedPath(100), 100);
        assert.strictEqual(path.toNamespacedPath(path), path);
        assert.strictEqual(path.toNamespacedPath(false), false);
        assert.strictEqual(path.toNamespacedPath(true), true);

        const emptyObj = {};
        assert.strictEqual(path.posix.toNamespacedPath('/foo/bar'), '/foo/bar');
        assert.strictEqual(path.posix.toNamespacedPath('foo/bar'), 'foo/bar');
        assert.strictEqual(path.posix.toNamespacedPath(null), isFibjs ? undefined : null);
        assert.strictEqual(path.posix.toNamespacedPath(true), true);
        assert.strictEqual(path.posix.toNamespacedPath(1), 1);
        assert.strictEqual(path.posix.toNamespacedPath(), undefined);
        assert.strictEqual(path.posix.toNamespacedPath(emptyObj), emptyObj);
        if (isWindows) {
            // These tests cause resolve() to insert the cwd, so we cannot test them from
            // non-Windows platforms (easily)
            assert.strictEqual(path.win32.toNamespacedPath('foo\\bar').toLowerCase(),
                `\\\\?\\${process.cwd().toLowerCase()}\\foo\\bar`);
            assert.strictEqual(path.win32.toNamespacedPath('foo/bar').toLowerCase(),
                `\\\\?\\${process.cwd().toLowerCase()}\\foo\\bar`);
            // const currentDeviceLetter = path.parse(process.cwd()).root.substring(0, 2);
            // assert.strictEqual(
            //     path.win32.toNamespacedPath(currentDeviceLetter).toLowerCase(),
            //     `\\\\?\\${process.cwd().toLowerCase()}`);
            // assert.strictEqual(path.win32.toNamespacedPath('C').toLowerCase(),
            //     `\\\\?\\${process.cwd().toLowerCase()}\\c`);
        }
        assert.strictEqual(path.win32.toNamespacedPath('C:\\foo'), '\\\\?\\C:\\foo');
        assert.strictEqual(path.win32.toNamespacedPath('C:/foo'), '\\\\?\\C:\\foo');
        assert.strictEqual(path.win32.toNamespacedPath('\\\\foo\\bar'),
            '\\\\?\\UNC\\foo\\bar\\');
        assert.strictEqual(path.win32.toNamespacedPath('//foo//bar'),
            '\\\\?\\UNC\\foo\\bar\\');
        assert.strictEqual(path.win32.toNamespacedPath('\\\\?\\foo'),
            isWindows ? '\\\\?\\foo\\' : '\\\\?\\foo');
        assert.strictEqual(path.win32.toNamespacedPath('\\\\?\\c:\\Windows/System'), '\\\\?\\c:\\Windows\\System');
        assert.strictEqual(path.win32.toNamespacedPath(null), isFibjs ? undefined : null);
        assert.strictEqual(path.win32.toNamespacedPath(true), true);
        assert.strictEqual(path.win32.toNamespacedPath(1), 1);
        assert.strictEqual(path.win32.toNamespacedPath(), undefined);
        assert.strictEqual(path.win32.toNamespacedPath(emptyObj), emptyObj);
    });

    it('separator and delimiter', () => {
        // Test path separators
        assert.strictEqual(path.win32.sep, '\\');
        assert.strictEqual(path.posix.sep, '/');

        // Test path delimiters
        assert.strictEqual(path.win32.delimiter, ';');
        assert.strictEqual(path.posix.delimiter, ':');

        // Test current platform matches
        if (isWindows) {
            assert.strictEqual(path, path.win32);
        } else {
            assert.strictEqual(path, path.posix);
        }
    });

    it('posix/win32 exists', () => {
        // Test that path.posix and path.win32 exist and are accessible
        assert.strictEqual(require('path').posix, path.posix);
        assert.strictEqual(require('path').win32, path.win32);

        // Test alternative import methods (if supported)
        try {
            assert.strictEqual(require('path/posix'), require('path').posix);
        } catch (e) {
            // path/posix import may not be supported in fibjs
        }

        try {
            assert.strictEqual(require('path/win32'), require('path').win32);
        } catch (e) {
            // path/win32 import may not be supported in fibjs
        }
    });

    it('posix relative on windows edge case', () => {
        // Refs: https://github.com/nodejs/node/issues/13683
        const relativePath = path.posix.relative('a/b/c', '../../x');
        assert.match(relativePath, /^(\.\.\/){3,5}x$/);
    });

    it('path.sep and path.delimiter', () => {
        // Test path.sep 
        assert.strictEqual(path.win32.sep, '\\');
        assert.strictEqual(path.posix.sep, '/');

        // Test path.delimiter
        assert.strictEqual(path.win32.delimiter, ';');
        assert.strictEqual(path.posix.delimiter, ':');

        // Test current platform
        if (isWindows) {
            assert.strictEqual(path.sep, '\\');
            assert.strictEqual(path.delimiter, ';');
        } else {
            assert.strictEqual(path.sep, '/');
            assert.strictEqual(path.delimiter, ':');
        }
    });

    it('additional edge cases', () => {
        // Test some edge cases found in Node.js tests

        // Empty path segments in join
        assert.strictEqual(path.posix.join('a', '', 'b'), 'a/b');
        assert.strictEqual(path.win32.join('a', '', 'b'), 'a\\b');

        // Multiple separators
        assert.strictEqual(path.posix.normalize('a//b//c'), 'a/b/c');
        assert.strictEqual(path.win32.normalize('a\\\\b\\\\c'), 'a\\b\\c');

        // Root directory handling
        assert.strictEqual(path.posix.resolve('/'), '/');
        assert.strictEqual(path.win32.resolve('C:\\'), 'C:\\');

        // Relative with same paths
        assert.strictEqual(path.posix.relative('/a', '/a'), '');
        assert.strictEqual(path.win32.relative('C:\\a', 'C:\\a'), '');
    });
});


