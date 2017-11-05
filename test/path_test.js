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
            assert.equal(fn('./test.js'), 'test.js');
            assert.equal(fn('./test.js', 'js'), 'test.');
            assert.equal(fn('./test.js', '.js'), 'test');
            assert.equal(fn('./test.js', 't.js'), 'tes');
            assert.equal(fn('./.js', '.js'), '');
            assert.equal(fn('./test/'), 'test');
            assert.equal(fn('./test.js/', '.js'), 'test');
        }
        test(path.basename.bind(path));
        test(path.win32.basename.bind(path.win32));
        test(path.posix.basename.bind(path.posix));
    });

    it('extname', () => {
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
            // assert.equal(fn('..'), '');
            assert.equal(fn('../'), '');
            assert.equal(fn('..file.ext'), '.ext');
            assert.equal(fn('..file'), '.file');
            assert.equal(fn('..file.'), '.');
            assert.equal(fn('..file..'), '.');
            assert.equal(fn('...'), '.');
            assert.equal(fn('...ext'), '.ext');
            assert.equal(fn('....'), '.');
            assert.equal(fn('file.ext/'), '');
        }

        test(path.extname.bind(path));
        test(path.win32.extname.bind(path.win32));
        test(path.posix.extname.bind(path.posix));

        if (isWindows) {
            // On windows, backspace is a path separator.
            assert.equal(path.extname('.\\'), '');
            assert.equal(path.extname('..\\'), '');
            assert.equal(path.extname('file.ext\\'), '');
        } else {
            // On unix, backspace is a valid name component like any other character.
            assert.equal(path.extname('.\\'), '');
            assert.equal(path.extname('..\\'), '.\\');
            assert.equal(path.extname('file.ext\\'), '.ext\\');
        }

        // On windows, backspace is a path separator.
        assert.equal(path.win32.extname('.\\'), '');
        assert.equal(path.win32.extname('..\\'), '');
        assert.equal(path.win32.extname('file.ext\\'), '');
        // On unix, backspace is a valid name component like any other character.
        assert.equal(path.posix.extname('.\\'), '');
        assert.equal(path.posix.extname('..\\'), '.\\');
        assert.equal(path.posix.extname('file.ext\\'), '.ext\\');
    });

    it('dirname', () => {
        function test(fn) {
            assert.equal(fn('/a/b/'), '/a');
            assert.equal(fn('/a/b'), '/a');
            assert.equal(fn('/a'), '/');
            assert.equal(fn('/'), '/');
            assert.equal(fn('main.js'), '');

        }

        test(path.dirname.bind(path));
        test(path.win32.dirname.bind(path.win32));
        test(path.posix.dirname.bind(path.posix));

        function testWin32(fn) {
            assert.equal(fn('c:\\'), 'c:\\');
            assert.equal(fn('c:\\foo'), 'c:\\');
            assert.equal(fn('c:\\foo\\'), 'c:\\');
            assert.equal(fn('c:\\foo\\bar'), 'c:\\foo');
            assert.equal(fn('c:\\foo\\bar\\'), 'c:\\foo');
            assert.equal(fn('c:\\foo\\bar\\baz'), 'c:\\foo\\bar');
            assert.equal(fn('\\'), '\\');
            assert.equal(fn('\\foo'), '\\');
            assert.equal(fn('\\foo\\'), '\\');
            assert.equal(fn('\\foo\\bar'), '\\foo');
            assert.equal(fn('\\foo\\bar\\'), '\\foo');
            assert.equal(fn('\\foo\\bar\\baz'), '\\foo\\bar');
            assert.equal(fn('c:'), 'c:');
            assert.equal(fn('c:foo'), 'c:');
            assert.equal(fn('c:foo\\'), 'c:');
            assert.equal(fn('c:foo\\bar'), 'c:foo');
            assert.equal(fn('c:foo\\bar\\'), 'c:foo');
            assert.equal(fn('c:foo\\bar\\baz'), 'c:foo\\bar');
            assert.equal(fn('\\\\unc\\share'), '\\\\unc\\share');
            assert.equal(fn('\\\\unc\\share\\foo'), '\\\\unc\\share\\');
            assert.equal(fn('\\\\unc\\share\\foo\\'), '\\\\unc\\share\\');
            assert.equal(fn('\\\\unc\\share\\foo\\bar'),
                '\\\\unc\\share\\foo');
            assert.equal(fn('\\\\unc\\share\\foo\\bar\\'),
                '\\\\unc\\share\\foo');
            assert.equal(fn('\\\\unc\\share\\foo\\bar\\baz'),
                '\\\\unc\\share\\foo\\bar');
        }

        if (isWindows) {
            testWin32(path.dirname.bind(path));
        }

        testWin32(path.win32.dirname.bind(path.win32));
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
        assert.isTrue(path.posix.isAbsolute('/foo/bar'));
        assert.isTrue(path.posix.isAbsolute('/baz/..'));
        assert.isFalse(path.posix.isAbsolute('qux/'));
        assert.isFalse(path.posix.isAbsolute('.'));

        assert.isTrue(path.win32.isAbsolute('//server'));
        assert.isTrue(path.win32.isAbsolute('\\\\server'));
        assert.isTrue(path.win32.isAbsolute('C:/foo/..'));
        assert.isTrue(path.win32.isAbsolute('C:\\foo\\..'));
        assert.isFalse(path.win32.isAbsolute('bar\\baz'));
        assert.isFalse(path.win32.isAbsolute('bar/baz'));
        assert.isFalse(path.win32.isAbsolute('.'));
    });

    it('join', () => {
        var failures = [];
        var joinTests = [
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

        var joinTestsWin32 = [
            // UNC path expected
            [['//foo/bar'], '\\\\foo\\bar\\'],
            [['\\/foo/bar'], '\\\\foo\\bar\\'],
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
            // No UNC path expected (no double slash in first component)
            [['\\', 'foo/bar'], '\\foo\\bar'],
            [['\\', '/foo/bar'], '\\foo\\bar'],
            [['', '/', '/foo/bar'], '\\foo\\bar'],
            // No UNC path expected (no non-slashes in first component -
            // questionable)
            [['//', 'foo/bar'], '\\foo\\bar'],
            [['//', '/foo/bar'], '\\foo\\bar'],
            [['\\\\', '/', '/foo/bar'], '\\foo\\bar'],
            [['//'], '/'],
            // No UNC path expected (share name missing - questionable).
            [['//foo'], '\\foo'],
            [['//foo/'], '\\foo\\'],
            [['//foo', '/'], '\\foo\\'],
            [['//foo', '', '/'], '\\foo\\'],
            // No UNC path expected (too many leading slashes - questionable)
            [['///foo/bar'], '\\foo\\bar'],
            [['////foo', 'bar'], '\\foo\\bar'],
            [['\\\\\\/foo/bar'], '\\foo\\bar'],
            // Drive-relative vs drive-absolute paths. This merely describes the
            // status quo, rather than being obviously right
            [['c:'], 'c:.'],
            [['c:.'], 'c:.'],
            [['c:', ''], 'c:.'],
            [['', 'c:'], 'c:.'],
            [['c:.', '/'], 'c:.\\'],
            [['c:.', 'file'], 'c:file'],
            [['c:', '/'], 'c:\\'],
            [['c:', 'file'], 'c:\\file']
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

    it("resolve", function () {

        var resolveTestsWin32 =
            // arguments                                    result
            [
                [
                    ['c:/blah\\blah', 'd:/games', 'c:../a'], 'c:\\blah\\a'
                ],
                [
                    ['c:blah\\blah', '/path'], 'c:\\path'
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
                    ['.'], process.cwd().replace(/\//g, '\\')
                ],
                [
                    ['//server/share', '..', 'relative\\'], '\\\\server\\share\\relative'
                ],
                // [[‘c:/‘, '//'], 'c:\\'],
                // [[‘c:/‘, '//dir'], 'c:\\dir'],
                [
                    ['c:/', '//server/share'], '\\\\server\\share\\'
                ],
                [
                    ['c:/', '//server/share/file'], '\\\\server\\share\\file'
                ],
                [
                    ['c:/', '//server//share'], '\\\\server\\share\\'
                ],
                [
                    ['c:/', '/some//dir'], 'c:\\some\\dir'
                ]
            ];
        // Posix
        var resolveTestsPosix =
            // arguments                                    result
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
                ]
            ];

        var failures = [];

        var resolveTests = isWindows ? resolveTestsWin32 : resolveTestsPosix;

        // path.resolve
        resolveTests.forEach(function (test) {
            var actual = path.resolve.apply(path, test[0]);
            var expected = test[1];
            var message = 'path.resolve(' + test[0].map(JSON.stringify).join(',') + ')' +
                '\n  expect=' + JSON.stringify(expected) +
                '\n  actual=' + JSON.stringify(actual);

            if (isWindows) {
                actual = actual.toLowerCase();
                expected = expected.toLowerCase();
            }

            if (actual !== expected) failures.push('\n' + message);
            assert.equal(actual, expected, message);
        });
        assert.equal(failures.length, 0, failures.join(''));

        // path.posix.resolve
        resolveTestsPosix.forEach(function (test) {
            var actual = path.posix.resolve.apply(path.posix, test[0]);
            var expected = test[1];
            var message = 'path.posix.resolve(' + test[0].map(JSON.stringify).join(',') + ')' +
                '\n  expect=' + JSON.stringify(expected) +
                '\n  actual=' + JSON.stringify(actual);

            if (actual !== expected) failures.push('\n' + message);
            assert.equal(actual, expected, message);
        });
        assert.equal(failures.length, 0, failures.join(''));

        // path.win32.resolve
        resolveTestsWin32.forEach(function (test) {
            var actual = path.win32.resolve.apply(path.win32, test[0]);
            var expected = test[1];
            var message = 'path.win32.resolve(' + test[0].map(JSON.stringify).join(',') + ')' +
                '\n  expect=' + JSON.stringify(expected) +
                '\n  actual=' + JSON.stringify(actual);

            actual = actual.toLowerCase();
            expected = expected.toLowerCase();

            if (actual !== expected) failures.push('\n' + message);
            assert.equal(actual, expected, message);
        });
        assert.equal(failures.length, 0, failures.join(''));
    });

    it("fullpath", () => {
        assert.equal(path.join(process.cwd(), "not_exists"), path.fullpath("not_exists"));
        assert.equal(path.join(process.cwd(), "not_exists"), path.posix.fullpath("not_exists"));
        if (isWindows) {
            assert.equal(path.join(process.cwd(), "not_exists"), path.win32.fullpath("not_exists"));
        }
    });
});

repl && test.run(console.DEBUG);