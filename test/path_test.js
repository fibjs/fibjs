var test = require("test");
test.setup();

var path = require('path');
var isWindows = process.platform === 'win32';

describe('path', () => {
    it('win32/posix', () => {
        if (isWindows)
            assert.equal(path, path.win32);
        else
            assert.equal(path, path.posix);
    });

    it('basename', () => {
        function test(fn) {
            if (!isWindows) {
                assert.strictEqual(fn(__filename), 'path_test.js');
                assert.strictEqual(fn(__filename, '.js'), 'path_test');
            }

            assert.strictEqual(fn('.js', '.js'), '');
            assert.strictEqual(fn(''), '');
            assert.strictEqual(fn('/dir/basename.ext'), 'basename.ext');
            assert.strictEqual(fn('/basename.ext'), 'basename.ext');
            assert.strictEqual(fn('basename.ext'), 'basename.ext');
            assert.strictEqual(fn('basename.ext/'), 'basename.ext');
            assert.strictEqual(fn('basename.ext//'), 'basename.ext');
            assert.strictEqual(fn('aaa/bbb', '/bbb'), 'bbb');
            assert.strictEqual(fn('aaa/bbb', 'a/bbb'), 'bbb');
            assert.strictEqual(fn('aaa/bbb', 'bbb'), 'bbb');
            assert.strictEqual(fn('aaa/bbb//', 'bbb'), 'bbb');
            assert.strictEqual(fn('aaa/bbb', 'bb'), 'b');
            assert.strictEqual(fn('aaa/bbb', 'b'), 'bb');
            assert.strictEqual(fn('/aaa/bbb', '/bbb'), 'bbb');
            assert.strictEqual(fn('/aaa/bbb', 'a/bbb'), 'bbb');
            assert.strictEqual(fn('/aaa/bbb', 'bbb'), 'bbb');
            assert.strictEqual(fn('/aaa/bbb//', 'bbb'), 'bbb');
            assert.strictEqual(fn('/aaa/bbb', 'bb'), 'b');
            assert.strictEqual(fn('/aaa/bbb', 'b'), 'bb');
            assert.strictEqual(fn('/aaa/bbb'), 'bbb');
            assert.strictEqual(fn('/aaa/'), 'aaa');
            assert.strictEqual(fn('/aaa/b'), 'b');
            assert.strictEqual(fn('/a/b'), 'b');
            assert.strictEqual(fn('//a'), 'a');
        }
        test(path.basename.bind(path));
        test(path.win32.basename.bind(path.win32));
        test(path.posix.basename.bind(path.posix));

        function testPosix(fn) {
            assert.strictEqual(fn('\\dir\\basename.ext'),
                '\\dir\\basename.ext');
            assert.strictEqual(fn('\\basename.ext'), '\\basename.ext');
            assert.strictEqual(fn('basename.ext'), 'basename.ext');
            assert.strictEqual(fn('basename.ext\\'), 'basename.ext\\');
            assert.strictEqual(fn('basename.ext\\\\'), 'basename.ext\\\\');
            assert.strictEqual(fn('foo'), 'foo');
        }

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

        testPosix(path.posix.basename.bind(path.posix));
        testWin32(path.win32.basename.bind(path.win32));

        if (isWindows) {
            testWin32(path.basename.bind(path));
        } else {
            testPosix(path.basename.bind(path));
        }
    });

    it('extname', () => {
        const failures = [];
        const slashRE = /\//g;

        [
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
        ].forEach((test) => {
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
                const message = `path.${os}.extname(${JSON.stringify(input)})\n  expect=${
                    JSON.stringify(expected)}\n  actual=${JSON.stringify(actual)}`;
                if (actual !== expected)
                    failures.push(`\n${message}`);
            }); {
                const input = `C:${test[0].replace(slashRE, '\\')}`;
                const actual = path.win32.extname(input);
                const message = `path.win32.extname(${JSON.stringify(input)})\n  expect=${
                    JSON.stringify(expected)}\n  actual=${JSON.stringify(actual)}`;
                if (actual !== expected)
                    failures.push(`\n${message}`);
            }
        });
        assert.strictEqual(failures.length, 0, failures.join(''));

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

        // On Windows, backslash is a path separator.
        assert.strictEqual(path.win32.extname('.\\'), '');
        assert.strictEqual(path.win32.extname('..\\'), '');
        assert.strictEqual(path.win32.extname('file.ext\\'), '.ext');
        assert.strictEqual(path.win32.extname('file.ext\\\\'), '.ext');
        assert.strictEqual(path.win32.extname('file\\'), '');
        assert.strictEqual(path.win32.extname('file\\\\'), '');
        assert.strictEqual(path.win32.extname('file.\\'), '.');
        assert.strictEqual(path.win32.extname('file.\\\\'), '.');

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

    it('dirname', () => {
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

    it('normalize', () => {
        function testWin32(fn) {
            assert.equal(fn(''), '.');
            assert.equal(fn('.'), '.');
            assert.equal(fn('.\\'), '.\\');
            assert.equal(fn('..\\'), '..\\');
            assert.equal(fn('\\.\\'), '\\');
            assert.equal(fn('\\.'), '\\');
            assert.equal(fn('\\aaa\\.\\'), '\\aaa\\');
            assert.equal(fn('\\aaa\\.'), '\\aaa');
            assert.equal(fn('./fixtures///b/../b/c.js'),
                'fixtures\\b\\c.js');
            assert.equal(fn('/foo/../../../bar'), '\\bar');
            assert.equal(fn('a//b//../b'), 'a\\b');
            assert.equal(fn('a//b//./c'), 'a\\b\\c');
            assert.equal(fn('a//b//.'), 'a\\b');
            assert.equal(fn('c:/foo/../../../bar'), 'c:\\bar');
            assert.equal(fn('f:/'), 'f:\\');
            assert.equal(fn('f:path/to/../../../../path1'),
                'f:..\\..\\path1');
            assert.equal(fn('\\\\unc\\share\/foo/..//.///../../bar\\'),
                '\\\\unc\\share\\bar\\');
            assert.equal(fn('\\\\unc\\share\\'), '\\\\unc\\share\\');
            assert.equal(fn('\\\\unc\\share'), '\\\\unc\\share\\');
            assert.equal(fn('\\\\unc\\..\\folder'), '\\\\unc\\..\\folder');
            assert.equal(fn('\\\\unc\\.\\folder'), '\\\\unc\\.\\folder');
            assert.equal(fn('\\\\unc\\\\folder'), '\\\\unc\\folder\\');
            assert.equal(fn('\\\\unc\\\\'), '\\unc\\');
            assert.equal(fn('\\\\\\unc\\\\folder'), '\\unc\\folder');
            assert.equal(fn('\\\\.\\\\folder'), '\\\\.\\folder\\');
            assert.equal(fn('\\\\..\\\\folder'), '\\\\..\\folder\\');

            assert.strictEqual(fn('./fixtures///b/../b/c.js'),
                'fixtures\\b\\c.js');
            assert.strictEqual(fn('/foo/../../../bar'), '\\bar');
            assert.strictEqual(fn('a//b//../b'), 'a\\b');
            assert.strictEqual(fn('a//b//./c'), 'a\\b\\c');
            assert.strictEqual(fn('a//b//.'), 'a\\b');
            assert.strictEqual(fn('//server/share/dir/file.ext'),
                '\\\\server\\share\\dir\\file.ext');
            assert.strictEqual(fn('/a/b/c/../../../x/y/z'), '\\x\\y\\z');
            assert.strictEqual(fn('C:'), 'C:.');
            assert.strictEqual(fn('C:..\\abc'), 'C:..\\abc');
            assert.strictEqual(fn('C:..\\..\\abc\\..\\def'), 'C:..\\..\\def');
            assert.strictEqual(fn('C:\\.'), 'C:\\');
            assert.strictEqual(fn('file:stream'), 'file:stream');
            assert.strictEqual(fn('bar\\foo..\\..\\'), 'bar\\');
            assert.strictEqual(fn('bar\\foo..\\..'), 'bar');
            assert.strictEqual(fn('bar\\foo..\\..\\baz'), 'bar\\baz');
            assert.strictEqual(fn('bar\\foo..\\'), 'bar\\foo..\\');
            assert.strictEqual(fn('bar\\foo..'), 'bar\\foo..');
            assert.strictEqual(fn('..\\foo..\\..\\..\\bar'),
                '..\\..\\bar');
            assert.strictEqual(fn('..\\...\\..\\.\\...\\..\\..\\bar'), '..\\..\\bar');
        }

        testWin32(path.win32.normalize.bind(path.win32));

        function testPosix(fn) {
            assert.equal(fn(''), '.');
            assert.equal(fn('.'), '.');
            assert.equal(fn('./'), './');
            assert.equal(fn('../'), '../');
            assert.equal(fn('/./'), '/');
            assert.equal(fn('/.'), '/');
            assert.equal(fn('/aaa/./'), '/aaa/');
            assert.equal(fn('/aaa/.'), '/aaa');
            assert.equal(fn('./fixtures///b/../b/c.js'), 'fixtures/b/c.js');
            assert.equal(fn('/foo/../../../bar'), '/bar');
            assert.equal(fn('foo/../../../bar'), '../../bar');
            assert.equal(fn('a//b//../b'), 'a/b');
            assert.equal(fn('a//b//./c'), 'a/b/c');
            assert.equal(fn('a//b//.'), 'a/b');
            assert.equal(fn('/a/b/../'), '/a/');
            assert.equal(fn('/a/b/..'), '/a');

            assert.strictEqual(fn('./fixtures///b/../b/c.js'), 'fixtures/b/c.js');
            assert.strictEqual(fn('/foo/../../../bar'), '/bar');
            assert.strictEqual(fn('a//b//../b'), 'a/b');
            assert.strictEqual(fn('a//b//./c'), 'a/b/c');
            assert.strictEqual(fn('a//b//.'), 'a/b');
            assert.strictEqual(fn('/a/b/c/../../../x/y/z'), '/x/y/z');
            assert.strictEqual(fn('///..//./foo/.//bar'), '/foo/bar');
            assert.strictEqual(fn('bar/foo../../'), 'bar/');
            assert.strictEqual(fn('bar/foo../..'), 'bar');
            assert.strictEqual(fn('bar/foo../../baz'), 'bar/baz');
            assert.strictEqual(fn('bar/foo../'), 'bar/foo../');
            assert.strictEqual(fn('bar/foo..'), 'bar/foo..');
            assert.strictEqual(fn('../foo../../../bar'), '../../bar');
            assert.strictEqual(fn('../.../.././.../../../bar'), '../../bar');
        }

        testPosix(path.posix.normalize.bind(path.posix));

        if (isWindows) {
            testWin32(path.normalize.bind(path));
        } else {
            testPosix(path.normalize.bind(path));
        }
    });

    it('isAbsolute', () => {
        function testPosix(fn) {
            assert.isTrue(fn('/foo/bar'));
            assert.isTrue(fn('/baz/..'));
            assert.isFalse(fn('qux/'));
            assert.isFalse(fn('.'));
            assert.isFalse(fn('./foo'));
        }

        function testWin32(fn) {
            assert.isTrue(fn('/'));
            assert.isTrue(fn('//'));
            assert.isTrue(fn('//server'));
            assert.isTrue(fn('//server/file'));
            assert.isTrue(fn('\\\\server\\file'));
            assert.isTrue(fn('\\\\server'));
            assert.isTrue(fn('\\\\'));
            assert.isFalse(fn('c'));
            assert.isFalse(fn('c:'));
            assert.isTrue(fn('c:\\'));
            assert.isTrue(fn('c:/'));
            assert.isTrue(fn('c://'));
            assert.isTrue(fn('C:/Users/'));
            assert.isTrue(fn('C:\\Users\\'));
            assert.isFalse(fn('C:cwd/another'));
            assert.isFalse(fn('C:cwd\\another'));
            assert.isFalse(fn('directory/directory'));
            assert.isFalse(fn('directory\\directory'));
        }

        testPosix(path.posix.isAbsolute.bind(path.posix));
        testWin32(path.win32.isAbsolute.bind(path.win32));

        if (isWindows) {
            testWin32(path.isAbsolute.bind(path));
        } else {
            testPosix(path.isAbsolute.bind(path));
        }
    });

    it('join', () => {
        var failures = [];
        var joinTests = [
            [
                ['.', 'x/b', '..', '/b/c.js'], 'x/b/c.js'
            ],
            [
                [], '.'
            ],
            [
                ['/.', 'x/b', '..', '/b/c.js'], '/x/b/c.js'
            ],
            [
                ['/foo', '../../../bar'], '/bar'
            ],
            [
                ['foo', '../../../bar'], '../../bar'
            ],
            [
                ['foo/', '../../../bar'], '../../bar'
            ],
            [
                ['foo/x', '../../../bar'], '../bar'
            ],
            [
                ['foo/x', './bar'], 'foo/x/bar'
            ],
            [
                ['foo/x/', './bar'], 'foo/x/bar'
            ],
            [
                ['foo/x/', '.', 'bar'], 'foo/x/bar'
            ],
            [
                ['./'], './'
            ],
            [
                ['.', './'], './'
            ],
            [
                ['.', '.', '.'], '.'
            ],
            [
                ['.', './', '.'], '.'
            ],
            [
                ['.', '/./', '.'], '.'
            ],
            [
                ['.', '/////./', '.'], '.'
            ],
            [
                ['.'], '.'
            ],
            [
                ['', '.'], '.'
            ],
            [
                ['', 'foo'], 'foo'
            ],
            [
                ['foo', '/bar'], 'foo/bar'
            ],
            [
                ['', '/foo'], '/foo'
            ],
            [
                ['', '', '/foo'], '/foo'
            ],
            [
                ['', '', 'foo'], 'foo'
            ],
            [
                ['foo', ''], 'foo'
            ],
            [
                ['foo/', ''], 'foo/'
            ],
            [
                ['foo', '', '/bar'], 'foo/bar'
            ],
            [
                ['./', '..', '/foo'], '../foo'
            ],
            [
                ['./', '..', '..', '/foo'], '../../foo'
            ],
            [
                ['.', '..', '..', '/foo'], '../../foo'
            ],
            [
                ['', '..', '..', '/foo'], '../../foo'
            ],
            [
                ['/'], '/'
            ],
            [
                ['/', '.'], '/'
            ],
            [
                ['/', '..'], '/'
            ],
            [
                ['/', '..', '..'], '/'
            ],
            [
                [''], '.'
            ],
            [
                ['', ''], '.'
            ],
            [
                [' /foo'], ' /foo'
            ],
            [
                [' ', 'foo'], ' /foo'
            ],
            [
                [' ', '.'], ' '
            ],
            [
                [' ', '/'], ' /'
            ],
            [
                [' ', ''], ' '
            ],
            [
                ['/', 'foo'], '/foo'
            ],
            [
                ['/', '/foo'], '/foo'
            ],
            [
                ['/', '//foo'], '/foo'
            ],
            [
                ['/', '', '/foo'], '/foo'
            ],
            [
                ['', '/', 'foo'], '/foo'
            ],
            [
                ['', '/', '/foo'], '/foo'
            ]
        ];

        var joinTestsWin32 = [
            // UNC path expected
            [
                ['//foo/bar'], '\\\\foo\\bar\\'
            ],
            [
                ['\\/foo/bar'], '\\\\foo\\bar\\'
            ],
            [
                ['\\\\foo/bar'], '\\\\foo\\bar\\'
            ],
            // UNC path expected - server and share separate
            [
                ['//foo', 'bar'], '\\\\foo\\bar\\'
            ],
            [
                ['//foo/', 'bar'], '\\\\foo\\bar\\'
            ],
            [
                ['//foo', '/bar'], '\\\\foo\\bar\\'
            ],
            // UNC path expected - questionable
            [
                ['//foo', '', 'bar'], '\\\\foo\\bar\\'
            ],
            [
                ['//foo/', '', 'bar'], '\\\\foo\\bar\\'
            ],
            [
                ['//foo/', '', '/bar'], '\\\\foo\\bar\\'
            ],
            // UNC path expected - even more questionable
            [
                ['', '//foo', 'bar'], '\\\\foo\\bar\\'
            ],
            [
                ['', '//foo/', 'bar'], '\\\\foo\\bar\\'
            ],
            [
                ['', '//foo/', '/bar'], '\\\\foo\\bar\\'
            ],
            // No UNC path expected (no double slash in first component)
            [
                ['\\', 'foo/bar'], '\\foo\\bar'
            ],
            [
                ['\\', '/foo/bar'], '\\foo\\bar'
            ],
            [
                ['', '/', '/foo/bar'], '\\foo\\bar'
            ],
            // No UNC path expected (no non-slashes in first component -
            // questionable)
            [
                ['//', 'foo/bar'], '\\foo\\bar'
            ],
            [
                ['//', '/foo/bar'], '\\foo\\bar'
            ],
            [
                ['\\\\', '/', '/foo/bar'], '\\foo\\bar'
            ],
            [
                ['//'], '/'
            ],
            // No UNC path expected (share name missing - questionable).
            [
                ['//foo'], '\\foo'
            ],
            [
                ['//foo/'], '\\foo\\'
            ],
            [
                ['//foo', '/'], '\\foo\\'
            ],
            [
                ['//foo', '', '/'], '\\foo\\'
            ],
            // No UNC path expected (too many leading slashes - questionable)
            [
                ['///foo/bar'], '\\foo\\bar'
            ],
            [
                ['////foo', 'bar'], '\\foo\\bar'
            ],
            [
                ['\\\\\\/foo/bar'], '\\foo\\bar'
            ],
            // Drive-relative vs drive-absolute paths. This merely describes the
            // status quo, rather than being obviously right
            [
                ['c:'], 'c:.'
            ],
            [
                ['c:.'], 'c:.'
            ],
            [
                ['c:', ''], 'c:.'
            ],
            [
                ['', 'c:'], 'c:.'
            ],
            [
                ['c:.', '/'], 'c:.\\'
            ],
            [
                ['c:.', 'file'], 'c:file'
            ],
            [
                ['c:', '/'], 'c:\\'
            ],
            [
                ['c:', 'file'], 'c:\\file'
            ]
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

        // path.win32.join
        [...joinTests, ...joinTestsWin32].forEach((test) => {
            var actual = path.win32.join.apply(path.win32, test[0]);
            var expected = test[1].replace(/\//g, '\\');
            var message = 'path.win32.join(' + test[0].map(JSON.stringify).join(',') + ')' + '\n  expect=' + JSON.stringify(expected) + '\n  actual=' + JSON.stringify(actual);
            if (actual !== expected)
                failures.push('\n' + message);
        });
        assert.equal(failures.length, 0, failures.join(''));

        // path.posix.join
        joinTests.forEach((test) => {
            var actual = path.posix.join.apply(path.posix, test[0]);
            var expected = test[1];
            var message = 'path.posix.join(' + test[0].map(JSON.stringify).join(',') + ')' + '\n  expect=' + JSON.stringify(expected) + '\n  actual=' + JSON.stringify(actual);
            if (actual !== expected)
                failures.push('\n' + message);
        });
        assert.equal(failures.length, 0, failures.join(''));

        if (isWindows) {
            assert.equal(path.join('c:/path1', 'c:path2'), 'c:\\path1\\c:path2');
            assert.equal(path.join('c:/path1', 'd:path2'), 'c:\\path1\\d:path2');
        }

        assert.equal(path.win32.join('c:/path1', 'c:path2'), 'c:\\path1\\c:path2');
        assert.equal(path.win32.join('c:/path1', 'd:path2'), 'c:\\path1\\d:path2');
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
                    ['a/b/c/', '../../..'], process.cwd()
                ],
                [
                    ['.'], process.cwd()
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
                    `path.${os}.resolve(${test[0].map(JSON.stringify).join(',')})\n  expect=${
                    JSON.stringify(expected)}\n  actual=${JSON.stringify(actual)}`;
                if (actual !== expected && actualAlt !== expected)
                    failures.push(`\n${message}`);
            });
        });
        assert.strictEqual(failures.length, 0, failures.join(''));
    });

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
        // assert.strictEqual(path.relative('', pwd), '');
        // assert.strictEqual(path.relative(pwd, ''), '');
        // assert.strictEqual(path.relative(pwd, pwd), '');
    });

    oit("toNamespacedPath", () => {
        if (isWindows) {
            const file = __filename;
            const resolvedFile = path.resolve(file);

            assert.strictEqual(`\\\\?\\${resolvedFile}`,
                path.toNamespacedPath(file));
            // assert.strictEqual(`\\\\?\\${resolvedFile}`,
            //     path.toNamespacedPath(`\\\\?\\${file}`));
            assert.strictEqual('\\\\?\\UNC\\someserver\\someshare\\somefile',
                path.toNamespacedPath(
                    '\\\\someserver\\someshare\\somefile'));
            assert.strictEqual('\\\\?\\UNC\\someserver\\someshare\\somefile', path
                .toNamespacedPath('\\\\?\\UNC\\someserver\\someshare\\somefile'));
            assert.strictEqual('\\\\.\\pipe\\somepipe',
                path.toNamespacedPath('\\\\.\\pipe\\somepipe'));
        }

        assert.strictEqual(path.toNamespacedPath(null), null);
        assert.strictEqual(path.toNamespacedPath(100), 100);
        assert.strictEqual(path.toNamespacedPath(path), path);
        assert.strictEqual(path.toNamespacedPath(false), false);
        assert.strictEqual(path.toNamespacedPath(true), true);

        const emptyObj = {};
        assert.strictEqual(path.posix.toNamespacedPath('/foo/bar'), '/foo/bar');
        assert.strictEqual(path.posix.toNamespacedPath('foo/bar'), 'foo/bar');
        assert.strictEqual(path.posix.toNamespacedPath(null), null);
        assert.strictEqual(path.posix.toNamespacedPath(true), true);
        assert.strictEqual(path.posix.toNamespacedPath(1), 1);
        assert.strictEqual(path.posix.toNamespacedPath(), undefined);
        assert.strictEqual(path.posix.toNamespacedPath(emptyObj), emptyObj);
        if (isWindows) {
            // These tests cause resolve() to insert the cwd, so we cannot test them from
            // non-Windows platforms (easily)
            // assert.strictEqual(path.win32.toNamespacedPath('foo\\bar').toLowerCase(),
            //     `\\\\?\\${process.cwd().toLowerCase()}\\foo\\bar`);
            // assert.strictEqual(path.win32.toNamespacedPath('foo/bar').toLowerCase(),
            //     `\\\\?\\${process.cwd().toLowerCase()}\\foo\\bar`);
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
        assert.strictEqual(path.win32.toNamespacedPath('\\\\?\\foo'), '\\\\?\\foo');
        assert.strictEqual(path.win32.toNamespacedPath(null), null);
        assert.strictEqual(path.win32.toNamespacedPath(true), true);
        assert.strictEqual(path.win32.toNamespacedPath(1), 1);
        assert.strictEqual(path.win32.toNamespacedPath(), undefined);
        assert.strictEqual(path.win32.toNamespacedPath(emptyObj), emptyObj);
    });
});

repl && test.run(console.DEBUG);