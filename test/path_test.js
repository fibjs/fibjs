var test = require("test");
test.setup();

var os = require('os');
var path = require('path');
var process = require('process');
var isWindows = os.type === 'Windows';

describe('path', function() {
	it('basename', function() {
		assert.equal(path.basename('./test.js'), 'test.js');
		assert.equal(path.basename('./test.js', 'js'), 'test.');
		assert.equal(path.basename('./test.js', '.js'), 'test');
		assert.equal(path.basename('./test.js', 't.js'), 'tes');
		assert.equal(path.basename('./.js', '.js'), '');
	});

	it('extname', function() {
		assert.equal(path.extname('./test.js'), '.js');

		assert.equal(path.extname(''), '');
		assert.equal(path.extname('/path/to/file'), '');
		assert.equal(path.extname('/path/to/file.ext'), '.ext');
		assert.equal(path.extname('/path.to/file.ext'), '.ext');
		assert.equal(path.extname('/path.to/file'), '');
		assert.equal(path.extname('/path.to/.file'), '');
		assert.equal(path.extname('/path.to/.file.ext'), '.ext');
		assert.equal(path.extname('/path/to/f.ext'), '.ext');
		assert.equal(path.extname('/path/to/..ext'), '.ext');
		assert.equal(path.extname('file'), '');
		assert.equal(path.extname('file.ext'), '.ext');
		assert.equal(path.extname('.file'), '');
		assert.equal(path.extname('.file.ext'), '.ext');
		assert.equal(path.extname('/file'), '');
		assert.equal(path.extname('/file.ext'), '.ext');
		assert.equal(path.extname('/.file'), '');
		assert.equal(path.extname('/.file.ext'), '.ext');
		assert.equal(path.extname('.path/file.ext'), '.ext');
		assert.equal(path.extname('file.ext.ext'), '.ext');
		assert.equal(path.extname('file.'), '.');
		assert.equal(path.extname('.'), '');
		assert.equal(path.extname('./'), '');
		assert.equal(path.extname('.file.ext'), '.ext');
		assert.equal(path.extname('.file'), '');
		assert.equal(path.extname('.file.'), '.');
		assert.equal(path.extname('.file..'), '.');
		// assert.equal(path.extname('..'), '');
		assert.equal(path.extname('../'), '');
		assert.equal(path.extname('..file.ext'), '.ext');
		assert.equal(path.extname('..file'), '.file');
		assert.equal(path.extname('..file.'), '.');
		assert.equal(path.extname('..file..'), '.');
		assert.equal(path.extname('...'), '.');
		assert.equal(path.extname('...ext'), '.ext');
		assert.equal(path.extname('....'), '.');
		assert.equal(path.extname('file.ext/'), '');

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
	});

	it('dirname', function() {
		assert.equal(path.dirname('/a/b/'), '/a');
		assert.equal(path.dirname('/a/b'), '/a');
		assert.equal(path.dirname('/a'), '/');
		assert.equal(path.dirname('/'), '/');
		assert.equal(path.dirname('main.js'), '');

		if (isWindows) {
			assert.equal(path.dirname('c:\\'), 'c:\\');
			assert.equal(path.dirname('c:\\foo'), 'c:\\');
			assert.equal(path.dirname('c:\\foo\\'), 'c:\\');
			assert.equal(path.dirname('c:\\foo\\bar'), 'c:\\foo');
			assert.equal(path.dirname('c:\\foo\\bar\\'), 'c:\\foo');
			assert.equal(path.dirname('c:\\foo\\bar\\baz'), 'c:\\foo\\bar');
			assert.equal(path.dirname('\\'), '\\');
			assert.equal(path.dirname('\\foo'), '\\');
			assert.equal(path.dirname('\\foo\\'), '\\');
			assert.equal(path.dirname('\\foo\\bar'), '\\foo');
			assert.equal(path.dirname('\\foo\\bar\\'), '\\foo');
			assert.equal(path.dirname('\\foo\\bar\\baz'), '\\foo\\bar');
			assert.equal(path.dirname('c:'), 'c:');
			assert.equal(path.dirname('c:foo'), 'c:');
			assert.equal(path.dirname('c:foo\\'), 'c:');
			assert.equal(path.dirname('c:foo\\bar'), 'c:foo');
			assert.equal(path.dirname('c:foo\\bar\\'), 'c:foo');
			assert.equal(path.dirname('c:foo\\bar\\baz'), 'c:foo\\bar');
			assert.equal(path.dirname('\\\\unc\\share'), '\\\\unc\\share');
			assert.equal(path.dirname('\\\\unc\\share\\foo'), '\\\\unc\\share\\');
			assert.equal(path.dirname('\\\\unc\\share\\foo\\'), '\\\\unc\\share\\');
			assert.equal(path.dirname('\\\\unc\\share\\foo\\bar'),
				'\\\\unc\\share\\foo');
			assert.equal(path.dirname('\\\\unc\\share\\foo\\bar\\'),
				'\\\\unc\\share\\foo');
			assert.equal(path.dirname('\\\\unc\\share\\foo\\bar\\baz'),
				'\\\\unc\\share\\foo\\bar');
		}
	});

	it('normalize', function() {
		if (isWindows) {
			assert.equal(path.normalize('./fixtures///b/../b/c.js'),
				'fixtures\\b\\c.js');
			assert.equal(path.normalize('/foo/../../../bar'), '\\bar');
			assert.equal(path.normalize('a//b//../b'), 'a\\b');
			assert.equal(path.normalize('a//b//./c'), 'a\\b\\c');
			assert.equal(path.normalize('a//b//.'), 'a\\b');
			assert.equal(path.normalize('c:/foo/../../../bar'), 'c:\\bar');
			assert.equal(path.normalize('f:/'), 'f:\\');
			assert.equal(path.normalize('f:path/to/../../../../path1'),
				'f:..\\..\\path1');
			assert.equal(path.normalize('\\\\unc\\share\/foo/..//.///../../bar\\'),
				'\\\\unc\\share\\bar');
			assert.equal(path.normalize('\\\\unc\\share\\'), '\\\\unc\\share\\');
		} else {
			assert.equal(path.normalize('./fixtures///b/../b/c.js'), 'fixtures/b/c.js');
			assert.equal(path.normalize('/foo/../../../bar'), '/bar');
			assert.equal(path.normalize('foo/../../../bar'), '../../bar');
			assert.equal(path.normalize('a//b//../b'), 'a/b');
			assert.equal(path.normalize('a//b//./c'), 'a/b/c');
			assert.equal(path.normalize('a//b//.'), 'a/b');
		}
	});

	it('join', function() {
		var failures = [];
		var joinTests = [
			[
				['.', 'x/b', '..', 'b/c.js'], 'x/b/c.js'
			],
			[
				['/.', 'x/b', '..', 'b/c.js'], '/x/b/c.js'
			],
			[
				['/.', 'x/b', '..', '/b/c.js'], '/b/c.js'
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
				['./'], ''
			],
			[
				['.', './'], ''
			],
			[
				['.', '.', '.'], ''
			],
			[
				['.', './', '.'], ''
			],
			[
				['.', '/./', '.'], '/'
			],
			[
				['.'], ''
			],
			[
				['', '.'], ''
			],
			[
				['', 'foo'], 'foo'
			],
			[
				['foo', '/bar'], '/bar'
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
				['foo/', ''], 'foo'
			],
			[
				['foo', '', '/bar'], '/bar'
			],
			[
				['./', '..', '/foo'], '/foo'
			],
			[
				['./', '..', '..', 'foo'], '../../foo'
			],
			[
				['.', '..', '..', 'foo'], '../../foo'
			],
			[
				['', '..', '..', 'foo'], '../../foo'
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
				[''], ''
			],
			[
				['', ''], ''
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
				[' ', '/'], '/'
			],
			[
				[' ', ''], ' '
			]
		];
		joinTests.forEach(function(test) {
			var actual = path.join.apply(path, test[0]);
			var expected = isWindows ? test[1].replace(/\//g, '\\') : test[1];
			var message = 'path.join(' + test[0].map(JSON.stringify).join(',') + ')' + '\n  expect=' + JSON.stringify(expected) + '\n  actual=' + JSON.stringify(actual);
			if (actual !== expected)
				failures.push('\n' + message);
		});
		assert.equal(failures.length, 0, failures.join(''));

		if (isWindows) {
			assert.equal(path.join('c:/path1', 'c:path2'), 'c:\\path1\\path2');
			assert.equal(path.join('c:/path1', 'd:path2'), 'd:path2');
		}
	});

	it("realpath", function() {
		assert.equal(path.join(process.cwd(), "main.js"), path.realpath("main.js"));
	});
});

//test.run(console.DEBUG);