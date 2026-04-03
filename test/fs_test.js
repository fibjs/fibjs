var { describe, it, before, after, beforeEach, afterEach } = require('node:test');
var assert = require('assert');
var coroutine = require('coroutine');
var path = require('path');
var fs = require('fs');
var net = require('net');
var os = require('os');
var zip = require('zip');
var io = require('io');
var {
    ensureDirectoryExisted
} = require('./_helpers/process');

var vmid = coroutine.vmid;

function rmdir(pathname) {
    try {
        fs.rmdir(pathname);
    } catch (e) { }
}

var homedir = os.homedir();

var pathname = path.join(homedir, 'test_dir' + vmid);
var pathname1 = path.join(homedir, 'test1_dir' + vmid);
var pathname2 = path.join(homedir, 'test2_dir' + vmid);

var win = process.platform === 'win32';
var linux = process.platform === 'linux';
var android = process.platform === 'android';
var isIOS = process.platform === 'ios';

function assert_stat_property(statObj) {
    assert.isNumber(statObj.dev)
    assert.isNumber(statObj.mode)
    assert.isNumber(statObj.nlink)
    assert.isNumber(statObj.uid)
    assert.isNumber(statObj.gid)
    assert.isNumber(statObj.rdev)
    assert.isNumber(statObj.blksize)
    assert.isNumber(statObj.ino)
    assert.isNumber(statObj.size)
    assert.isNumber(statObj.blocks)
    assert.isNumber(statObj.atimeMs)
    assert.isNumber(statObj.mtimeMs)
    assert.isNumber(statObj.ctimeMs)
    assert.isNumber(statObj.birthtimeMs)

    assert.ok(statObj.atime instanceof Date)
    assert.ok(statObj.mtime instanceof Date)
    assert.ok(statObj.ctime instanceof Date)
    assert.ok(statObj.birthtime instanceof Date)
}

function rmdir_recursive(pathname) {
    if (!fs.exists(pathname)) return;
    var files = fs.readdir(pathname);
    for (var i = 0; i < files.length; i++) {
        var file = path.join(pathname, files[i]);
        if (fs.stat(file).isDirectory()) {
            rmdir_recursive(file);
        } else {
            fs.unlink(file);
        }
    }
    fs.rmdir(pathname);
}

describe('fs', () => {
    before(() => {
        rmdir_recursive(pathname);
        rmdir_recursive(pathname1);
        rmdir_recursive(pathname2);
    });

    after(() => {
        try {
            fs.unlink(path.join(__dirname, 'unzip_test.zip'));
        } catch (e) { }
    });

    describe("stat", () => {
        it('file', () => {
            var st = fs.stat('.');

            assert_stat_property(st);

            assert.equal(st.isDirectory(), true);
            assert.equal(st.isFile(), false);
            if (win) {
                assert.equal(st.isExecutable(), false);
            } else {
                assert.equal(st.isExecutable(), true);
            }
            assert.equal(st.isReadable(), true);
            assert.equal(st.isWritable(), true);


            st = fs.stat(path.join(__dirname, 'abc', '../'));

            assert.equal(st.isDirectory(), true);
            assert.equal(st.isFile(), false);
            if (win) {
                assert.equal(st.isExecutable(), false);
            } else {
                assert.equal(st.isExecutable(), true);
            }
            assert.equal(st.isReadable(), true);
            assert.equal(st.isWritable(), true);
        });

        it('directory', () => {
            var test_dir = path.resolve(__dirname, `./fs_files/dir${coroutine.vmid}`);

            rmdir(test_dir);
            ensureDirectoryExisted(test_dir);
            var st = fs.stat(test_dir);
            rmdir(test_dir);

            assert_stat_property(st);

            assert.equal(st.isDirectory(), true);
            assert.equal(st.isFile(), false);
            if (win) {
                assert.equal(st.isExecutable(), false);
            } else {
                assert.equal(st.isExecutable(), true);
            }
            assert.equal(st.isReadable(), true);
            assert.equal(st.isWritable(), true);
        });

        it('error.code', () => {
            var code;

            try {
                fs.stat('aaaa');
            } catch (e) {
                code = e.code;
            }

            assert.equal(code, "ENOENT");
        });

        it('stat with options', () => {
            var st = fs.stat('.', { bigint: true });
            assert_stat_property(st);
            assert.equal(st.isDirectory(), true);

            // Test nanosecond properties
            assert.isNumber(st.mtimeNs);
            assert.isNumber(st.atimeNs);
            assert.isNumber(st.ctimeNs);
            assert.isNumber(st.birthtimeNs);
        });

        it('lstat with options', () => {
            var st = fs.lstat('.', { bigint: true });
            assert_stat_property(st);
            assert.equal(st.isDirectory(), true);

            // Test nanosecond properties
            assert.isNumber(st.mtimeNs);
            assert.isNumber(st.atimeNs);
            assert.isNumber(st.ctimeNs);
            assert.isNumber(st.birthtimeNs);
        });

        it('fstat with options', () => {
            var fd = fs.open(path.join(__dirname, 'fs_test.js'));
            var st = fs.fstat(fd, { bigint: true });
            fs.close(fd);

            assert_stat_property(st);
            assert.equal(st.isFile(), true);

            // Test nanosecond properties
            assert.isNumber(st.mtimeNs);
            assert.isNumber(st.atimeNs);
            assert.isNumber(st.ctimeNs);
            assert.isNumber(st.birthtimeNs);
        });

        it('stat nanosecond properties', () => {
            var st = fs.stat('.');

            // Nanosecond properties should be numbers (int64)
            assert.isNumber(st.mtimeNs);
            assert.isNumber(st.atimeNs);
            assert.isNumber(st.ctimeNs);
            assert.isNumber(st.birthtimeNs);

            // Nanosecond values should be valid (non-negative)
            assert.ok(st.mtimeNs >= 0, 'mtimeNs should be non-negative');
            assert.ok(st.atimeNs >= 0, 'atimeNs should be non-negative');
            assert.ok(st.ctimeNs >= 0, 'ctimeNs should be non-negative');
            assert.ok(st.birthtimeNs >= 0, 'birthtimeNs should be non-negative');
        });
    });

    it("file open & close", () => {
        var fd = fs.open(path.join(__dirname, 'fs_test.js'));

        assert.isNumber(fd.fd);
        assert.greaterThan(fd.fd, -1);
        assert.doesNotThrow(() => {
            fs.close(fd);
        });

        if (!win) {
            assert.doesNotThrow(() => {
                fs.close(fd);
            });
        }

        var fd1 = fs.open(path.join(__dirname, 'fs_test.js'));
        var fd2 = fs.open(path.join(__dirname, 'fs_test.js'));

        assert.greaterThan(fd2.fd, fd1.fd);
        fs.close(fd1);
        fs.close(fd2);
    });

    it("file openSync & closeSync", () => {
        var fd = fs.openSync(path.join(__dirname, 'fs_test.js'));
        assert.greaterThan(fd.fd, -1);
        assert.doesNotThrow(() => {
            fs.closeSync(fd);
        });
        if (!win) {
            assert.doesNotThrow(() => {
                fs.closeSync(fd);
            });
        }

        var fd1 = fs.openSync(path.join(__dirname, 'fs_test.js'));
        var fd2 = fs.openSync(path.join(__dirname, 'fs_test.js'));

        assert.greaterThan(fd2.fd, fd1.fd);
        fs.closeSync(fd1);
        fs.closeSync(fd2);
    });

    it("file openFile & close", () => {
        var f = fs.openFile(path.join(__dirname, 'fs_test.js'));
        assert.doesNotThrow(() => {
            f.read(100);
        });
        f.close();
        assert.throws(() => {
            f.read(100);
        });

        assert.ok(fs.exists(path.join(__dirname, 'fs_test.js')));
    });

    it("file.stat", () => {
        var f = fs.openFile(path.join(__dirname, 'fs_test.js'));

        assert.deepEqual(f.stat().toJSON(), fs.stat(path.join(__dirname, 'fs_test.js')).toJSON());
    });

    describe("createReadStream", () => {
        var testFile = path.join(__dirname, 'fs_test.js');
        var testContent = "Hello, createReadStream!";
        var tmpFile;

        before(() => {
            tmpFile = path.join(homedir, 'test_createReadStream_' + vmid + '.txt');
            fs.writeFile(tmpFile, testContent);
        });

        after(() => {
            if (fs.exists(tmpFile))
                fs.unlink(tmpFile);
        });

        it("basic read", () => {
            var stm = fs.createReadStream(tmpFile);
            assert.equal(stm.readAll().toString(), testContent);
            stm.close();
        });

        it("with start and end (end is inclusive)", () => {
            var stm = fs.createReadStream(tmpFile, { start: 7, end: 22 });
            assert.equal(stm.readAll().toString(), "createReadStream");
            stm.close();
        });

        it("with start only", () => {
            var stm = fs.createReadStream(tmpFile, { start: 7 });
            assert.equal(stm.readAll().toString(), "createReadStream!");
            stm.close();
        });

        it("with end only", () => {
            var stm = fs.createReadStream(tmpFile, { end: 4 });
            assert.equal(stm.readAll().toString(), "Hello");
            stm.close();
        });

        it("start=0, end=0 reads one byte", () => {
            var stm = fs.createReadStream(tmpFile, { start: 0, end: 0 });
            assert.equal(stm.readAll().toString(), "H");
            stm.close();
        });

        it("returned stream supports seek/tell/size", () => {
            var stm = fs.createReadStream(tmpFile, { start: 7, end: 22 });
            assert.equal(stm.size(), 16);
            assert.equal(stm.tell(), 0);

            stm.seek(6, fs.SEEK_SET);
            assert.equal(stm.tell(), 6);
            assert.equal(stm.readAll().toString(), "ReadStream");
            stm.close();
        });

        it("returned stream without range is a plain file stream", () => {
            var stm = fs.createReadStream(tmpFile);
            var sz = stm.size();
            assert.equal(sz, testContent.length);
            stm.seek(0, fs.SEEK_SET);
            assert.equal(stm.tell(), 0);
            stm.close();
        });

        it("async callback", done => {
            fs.createReadStream(tmpFile, { start: 0, end: 4 }, (err, stm) => {
                done(() => {
                    assert.isNull(err);
                    assert.equal(stm.readAll().toString(), "Hello");
                    stm.close();
                });
            });
        });

        it("read existing file without options", () => {
            var stm = fs.createReadStream(testFile);
            var buf = stm.readAll();
            assert.ok(buf.length > 0);
            assert.equal(buf.length, fs.stat(testFile).size);
            stm.close();
        });

        it("error on non-existent file", () => {
            assert.throws(() => {
                fs.createReadStream('/tmp/non_existent_file_12345.txt');
            });
        });
    });

    it("mkdir", () => {
        fs.mkdir(pathname, 0o755);
        assert.equal(fs.exists(pathname), true);

        if (!win) {
            var st = fs.stat(pathname);
            assert.equal(st.mode & 0o777, 0o755);
        }
    });

    it("mkdir with fstat mode", () => {
        const mode = 0o511;
        fs.mkdir(pathname2, mode);
        assert.equal(fs.exists(pathname2), true);

        if (!win) {
            var st = fs.stat(pathname2);
            assert.equal(st.mode & 0o777, mode);
        }

        fs.rmdir(pathname2);
        assert.equal(fs.exists(pathname2), false);
    });

    it("rename", () => {
        fs.rename(pathname, pathname1);
        assert.equal(fs.exists(pathname), false);
        assert.equal(fs.exists(pathname1), true);
    });

    it("rmdir", () => {
        fs.rmdir(pathname1);
        assert.equal(fs.exists(pathname1), false);
    });

    it("rmdir recursive", () => {
        // Create nested directory structure for testing
        var recursive_test_path = path.join(homedir, 'rmdir_recursive_test' + vmid);
        var nested_path = path.join(recursive_test_path, 'level1', 'level2', 'level3');
        var file_in_nested = path.join(nested_path, 'test_file.txt');
        var file_in_root = path.join(recursive_test_path, 'root_file.txt');

        // Create the nested structure
        fs.mkdir(nested_path, { recursive: true });
        fs.writeFile(file_in_nested, 'nested file content');
        fs.writeFile(file_in_root, 'root file content');

        // Verify structure exists
        assert.equal(fs.exists(recursive_test_path), true);
        assert.equal(fs.exists(nested_path), true);
        assert.equal(fs.exists(file_in_nested), true);
        assert.equal(fs.exists(file_in_root), true);

        // Remove recursively
        fs.rmdir(recursive_test_path, { recursive: true });

        // Verify everything is removed
        assert.equal(fs.exists(recursive_test_path), false);
        assert.equal(fs.exists(nested_path), false);
        assert.equal(fs.exists(file_in_nested), false);
        assert.equal(fs.exists(file_in_root), false);
    });

    it("rmdir recursive should throw error when directory does not exist", () => {
        var non_existent_path = path.join(homedir, 'non_existent_dir' + vmid + Date.now());

        // Should throw error for non-existent directory (following Node.js behavior)
        assert.throws(() => {
            fs.rmdir(non_existent_path, { recursive: true });
        });
    });

    it("rmdir recursive should throw error on single file", () => {
        var test_file_path = path.join(homedir, 'test_file_for_rmdir' + vmid + '.txt');

        // Create a test file
        fs.writeFile(test_file_path, 'test content');
        assert.equal(fs.exists(test_file_path), true);

        // rmdir recursive should not delete files, use rm instead
        assert.throws(() => {
            fs.rmdir(test_file_path, { recursive: true });
        });

        // File should still exist
        assert.equal(fs.exists(test_file_path), true);

        // Clean up using rm
        fs.rm(test_file_path);
        assert.equal(fs.exists(test_file_path), false);
    });

    it("rmdir recursive with complex nested structure", () => {
        var base_path = path.join(homedir, 'rmdir_complex_test' + vmid);

        // Create a complex nested structure
        var paths = {
            level1_dir1: path.join(base_path, 'level1_dir1'),
            level1_dir2: path.join(base_path, 'level1_dir2'),
            level1_file: path.join(base_path, 'level1_file.txt'),
            level2_dir1: path.join(base_path, 'level1_dir1', 'level2_dir1'),
            level2_dir2: path.join(base_path, 'level1_dir1', 'level2_dir2'),
            level2_file1: path.join(base_path, 'level1_dir1', 'level2_file1.txt'),
            level2_file2: path.join(base_path, 'level1_dir2', 'level2_file2.txt'),
            level3_dir: path.join(base_path, 'level1_dir1', 'level2_dir1', 'level3_dir'),
            level3_file1: path.join(base_path, 'level1_dir1', 'level2_dir1', 'level3_file1.txt'),
            level3_file2: path.join(base_path, 'level1_dir1', 'level2_dir2', 'level3_file2.txt'),
            level4_file: path.join(base_path, 'level1_dir1', 'level2_dir1', 'level3_dir', 'level4_file.txt')
        };

        // Create directories
        fs.mkdir(paths.level1_dir1, { recursive: true });
        fs.mkdir(paths.level1_dir2, { recursive: true });
        fs.mkdir(paths.level2_dir1, { recursive: true });
        fs.mkdir(paths.level2_dir2, { recursive: true });
        fs.mkdir(paths.level3_dir, { recursive: true });

        // Create files at different levels
        fs.writeFile(paths.level1_file, 'level 1 file content');
        fs.writeFile(paths.level2_file1, 'level 2 file 1 content');
        fs.writeFile(paths.level2_file2, 'level 2 file 2 content');
        fs.writeFile(paths.level3_file1, 'level 3 file 1 content');
        fs.writeFile(paths.level3_file2, 'level 3 file 2 content');
        fs.writeFile(paths.level4_file, 'level 4 file content');

        // Verify structure exists
        assert.equal(fs.exists(base_path), true);
        for (var key in paths) {
            assert.equal(fs.exists(paths[key]), true, 'Path should exist: ' + paths[key]);
        }

        // Remove entire structure recursively
        fs.rmdir(base_path, { recursive: true });

        // Verify everything is removed
        assert.equal(fs.exists(base_path), false);
        for (var key in paths) {
            assert.equal(fs.exists(paths[key]), false, 'Path should be removed: ' + paths[key]);
        }
    });

    it("rmdir recursive with empty directories", () => {
        var base_path = path.join(homedir, 'rmdir_empty_dirs_test' + vmid);

        // Create nested empty directories
        var empty_dirs = [
            path.join(base_path, 'empty1'),
            path.join(base_path, 'empty2'),
            path.join(base_path, 'empty1', 'empty1_1'),
            path.join(base_path, 'empty1', 'empty1_2'),
            path.join(base_path, 'empty1', 'empty1_1', 'empty1_1_1'),
            path.join(base_path, 'empty2', 'empty2_1')
        ];

        // Create all directories
        empty_dirs.forEach(dir => {
            fs.mkdir(dir, { recursive: true });
            assert.equal(fs.exists(dir), true);
        });

        // Remove recursively
        fs.rmdir(base_path, { recursive: true });

        // Verify all removed
        assert.equal(fs.exists(base_path), false);
        empty_dirs.forEach(dir => {
            assert.equal(fs.exists(dir), false);
        });
    });

    it("rmdir recursive with mixed file types", () => {
        var base_path = path.join(homedir, 'rmdir_mixed_types_test' + vmid);

        // Create mixed structure
        fs.mkdir(path.join(base_path, 'subdir'), { recursive: true });

        // Create different types of files
        fs.writeFile(path.join(base_path, 'text.txt'), 'text file');
        fs.writeFile(path.join(base_path, 'data.dat'), 'binary data');
        fs.writeFile(path.join(base_path, 'config.json'), '{"test": true}');
        fs.writeFile(path.join(base_path, 'script.js'), 'console.log("test");');
        fs.writeFile(path.join(base_path, 'subdir', 'nested.txt'), 'nested file');

        // Create some files with special names
        fs.writeFile(path.join(base_path, '.hidden'), 'hidden file');
        fs.writeFile(path.join(base_path, 'file with spaces.txt'), 'spaces in name');
        fs.writeFile(path.join(base_path, '中文文件.txt'), 'chinese filename');

        // Verify all exist
        var files = [
            'text.txt', 'data.dat', 'config.json', 'script.js',
            '.hidden', 'file with spaces.txt', '中文文件.txt',
            path.join('subdir', 'nested.txt')
        ];

        files.forEach(file => {
            assert.equal(fs.exists(path.join(base_path, file)), true, 'File should exist: ' + file);
        });

        // Remove all recursively
        fs.rmdir(base_path, { recursive: true });

        // Verify all removed
        assert.equal(fs.exists(base_path), false);
    });

    it("rmdir recursive with deep nesting", () => {
        var base_path = path.join(homedir, 'rmdir_deep_test' + vmid);

        // Create very deep nested structure (20 levels)
        var current_path = base_path;
        var all_paths = [base_path];

        for (var i = 1; i <= 20; i++) {
            current_path = path.join(current_path, 'level' + i);
            all_paths.push(current_path);
        }

        // Create the deep structure
        fs.mkdir(current_path, { recursive: true });

        // Add files at various depths
        fs.writeFile(path.join(base_path, 'root.txt'), 'root file');
        fs.writeFile(path.join(base_path, 'level1', 'level1.txt'), 'level 1 file');
        fs.writeFile(path.join(base_path, 'level1', 'level2', 'level3', 'level4', 'level5', 'mid.txt'), 'middle file');
        fs.writeFile(current_path + path.sep + 'deep.txt', 'deepest file');

        // Verify structure exists
        all_paths.forEach(p => {
            assert.equal(fs.exists(p), true, 'Deep path should exist: ' + p);
        });

        // Remove entire deep structure
        fs.rmdir(base_path, { recursive: true });

        // Verify all removed
        assert.equal(fs.exists(base_path), false);
        all_paths.forEach(p => {
            assert.equal(fs.exists(p), false, 'Deep path should be removed: ' + p);
        });
    });

    it("rmdir recursive with large number of files", () => {
        var base_path = path.join(homedir, 'rmdir_many_files_test' + vmid);

        // Create structure with many files
        fs.mkdir(base_path, { recursive: true });
        fs.mkdir(path.join(base_path, 'subdir1'), { recursive: true });
        fs.mkdir(path.join(base_path, 'subdir2'), { recursive: true });

        // Create 100 files in root
        for (var i = 0; i < 100; i++) {
            fs.writeFile(path.join(base_path, 'file' + i + '.txt'), 'content ' + i);
        }

        // Create 50 files in each subdirectory
        for (var i = 0; i < 50; i++) {
            fs.writeFile(path.join(base_path, 'subdir1', 'sub1_file' + i + '.txt'), 'sub1 content ' + i);
            fs.writeFile(path.join(base_path, 'subdir2', 'sub2_file' + i + '.txt'), 'sub2 content ' + i);
        }

        // Verify some files exist
        assert.equal(fs.exists(path.join(base_path, 'file0.txt')), true);
        assert.equal(fs.exists(path.join(base_path, 'file99.txt')), true);
        assert.equal(fs.exists(path.join(base_path, 'subdir1', 'sub1_file0.txt')), true);
        assert.equal(fs.exists(path.join(base_path, 'subdir2', 'sub2_file49.txt')), true);

        // Count total items (should be 200 files + 2 subdirs + 1 root = 203 items to remove)
        var root_items = fs.readdir(base_path);
        var sub1_items = fs.readdir(path.join(base_path, 'subdir1'));
        var sub2_items = fs.readdir(path.join(base_path, 'subdir2'));

        assert.equal(root_items.length, 102); // 100 files + 2 subdirs
        assert.equal(sub1_items.length, 50);
        assert.equal(sub2_items.length, 50);

        // Remove all recursively
        fs.rmdir(base_path, { recursive: true });

        // Verify completely removed
        assert.equal(fs.exists(base_path), false);
    });

    it("rm removes single file", () => {
        var test_file = path.join(homedir, 'rm_single_file_test' + vmid + '.txt');

        // Create a test file
        fs.writeFile(test_file, 'test content');
        assert.equal(fs.exists(test_file), true);

        // Remove file using rm
        fs.rm(test_file);

        // Verify file is removed
        assert.equal(fs.exists(test_file), false);
    });

    it("rm removes empty directory", () => {
        var test_dir = path.join(homedir, 'rm_empty_dir_test' + vmid);

        // Create an empty directory
        fs.mkdir(test_dir);
        assert.equal(fs.exists(test_dir), true);

        // Remove directory using rm
        fs.rm(test_dir);

        // Verify directory is removed
        assert.equal(fs.exists(test_dir), false);
    });

    it("rm throws error for non-empty directory without recursive", () => {
        var test_dir = path.join(homedir, 'rm_non_empty_test' + vmid);
        var test_file = path.join(test_dir, 'file.txt');

        // Create directory with file
        fs.mkdir(test_dir);
        fs.writeFile(test_file, 'content');
        assert.equal(fs.exists(test_dir), true);
        assert.equal(fs.exists(test_file), true);

        // Should throw error when trying to remove non-empty directory without recursive
        assert.throws(() => {
            fs.rm(test_dir);
        });

        // Clean up
        fs.rm(test_dir, { recursive: true });
    });

    it("rm recursive removes directory with contents", () => {
        var base_path = path.join(homedir, 'rm_recursive_test' + vmid);
        var nested_path = path.join(base_path, 'level1', 'level2');
        var file_in_nested = path.join(nested_path, 'test.txt');
        var file_in_root = path.join(base_path, 'root.txt');

        // Create nested structure
        fs.mkdir(nested_path, { recursive: true });
        fs.writeFile(file_in_nested, 'nested content');
        fs.writeFile(file_in_root, 'root content');

        // Verify structure exists
        assert.equal(fs.exists(base_path), true);
        assert.equal(fs.exists(nested_path), true);
        assert.equal(fs.exists(file_in_nested), true);
        assert.equal(fs.exists(file_in_root), true);

        // Remove recursively
        fs.rm(base_path, { recursive: true });

        // Verify everything is removed
        assert.equal(fs.exists(base_path), false);
    });

    it("rm throws error for non-existent path", () => {
        var non_existent = path.join(homedir, 'non_existent_rm_test' + vmid + Date.now());

        assert.throws(() => {
            fs.rm(non_existent);
        });
    });

    it("rm recursive throws error for non-existent path", () => {
        var non_existent = path.join(homedir, 'non_existent_rm_recursive_test' + vmid + Date.now());

        assert.throws(() => {
            fs.rm(non_existent, { recursive: true });
        });
    });

    it("rm recursive with complex structure", () => {
        var base_path = path.join(homedir, 'rm_complex_test' + vmid);

        // Create complex structure
        var dirs = [
            path.join(base_path, 'dir1'),
            path.join(base_path, 'dir2'),
            path.join(base_path, 'dir1', 'subdir1'),
            path.join(base_path, 'dir1', 'subdir2'),
            path.join(base_path, 'dir2', 'subdir3')
        ];

        dirs.forEach(dir => fs.mkdir(dir, { recursive: true }));

        // Create files
        fs.writeFile(path.join(base_path, 'root.txt'), 'root');
        fs.writeFile(path.join(base_path, 'dir1', 'file1.txt'), 'file1');
        fs.writeFile(path.join(base_path, 'dir1', 'subdir1', 'deep.txt'), 'deep');
        fs.writeFile(path.join(base_path, 'dir2', 'file2.txt'), 'file2');

        // Verify structure
        assert.equal(fs.exists(base_path), true);
        dirs.forEach(dir => assert.equal(fs.exists(dir), true));

        // Remove all
        fs.rm(base_path, { recursive: true });

        // Verify removed
        assert.equal(fs.exists(base_path), false);
    });

    it("rmdir without recursive should not delete files", () => {
        var test_file = path.join(homedir, 'rmdir_file_test' + vmid + '.txt');

        // Create a test file
        fs.writeFile(test_file, 'test content');
        assert.equal(fs.exists(test_file), true);

        // rmdir should fail on a file (without recursive)
        assert.throws(() => {
            fs.rmdir(test_file);
        });

        // File should still exist
        assert.equal(fs.exists(test_file), true);

        // Clean up
        fs.rm(test_file);
    });

    it("mkdir recursive", () => {
        var recursive_path = path.join(pathname, 'test_dir');
        fs.mkdir(recursive_path, {
            recursive: true
        });
        assert.equal(fs.exists(recursive_path), true);
    });

    it("mkdir recursive do not throw error when directory exists", () => {
        fs.mkdir(path.join(__dirname, "fs_files"), {
            recursive: true
        });
    });

    it("mkdir recursive throw error when path is not directory", () => {
        assert.throws(() => {
            fs.mkdir(path.join(__filename, "fs_files"), {
                recursive: true
            });
        });
    });

    it("mkdtemp creates unique temp directory", () => {
        var dir = fs.mkdtempSync(path.join(homedir, 'fibjs-mkdtemp-test-'));
        try {
            assert.ok(fs.exists(dir), "temp dir should exist");
            assert.ok(dir.startsWith(path.join(homedir, 'fibjs-mkdtemp-test-')), "dir should start with prefix");
            var stat = fs.stat(dir);
            assert.ok(stat.isDirectory(), "should be a directory");
        } finally {
            fs.rmdir(dir);
        }
    });

    it("mkdtemp creates unique directories each call", () => {
        var dir1 = fs.mkdtempSync(path.join(homedir, 'fibjs-mkdtemp-'));
        var dir2 = fs.mkdtempSync(path.join(homedir, 'fibjs-mkdtemp-'));
        try {
            assert.notEqual(dir1, dir2, "each call should return a unique path");
        } finally {
            fs.rmdir(dir1);
            fs.rmdir(dir2);
        }
    });

    it("file.size", () => {
        var f = fs.openFile(path.join(__dirname, 'fs_test.js'));
        var st = fs.stat(path.join(__dirname, 'fs_test.js'));
        assert.equal(st.size, f.size());
        f.close();
    });

    if (!win) {
        it("fs.chmod", () => {
            var st = fs.stat(path.join(__dirname, 'fs_test.js'));
            var oldm = st.mode;

            fs.chmod(path.join(__dirname, 'fs_test.js'), 511);
            var st = fs.stat(path.join(__dirname, 'fs_test.js'));
            assert.equal(st.mode & 511, 511);

            fs.chmod(path.join(__dirname, 'fs_test.js'), oldm);
            var st = fs.stat(path.join(__dirname, 'fs_test.js'));
            assert.equal(st.mode & 511, oldm & 511);
        });

        it("file.chmod", () => {
            var f = fs.openFile(path.join(__dirname, 'fs_test.js'));
            var st = fs.stat(path.join(__dirname, 'fs_test.js'));
            var oldm = st.mode;

            f.chmod(511);
            var st = fs.stat(path.join(__dirname, 'fs_test.js'));
            assert.equal(st.mode & 511, 511);

            f.chmod(oldm);
            var st = fs.stat(path.join(__dirname, 'fs_test.js'));
            assert.equal(st.mode & 511, oldm & 511);

            f.close();
        });

        it("fs.lchmod", () => {
            if (linux || android)
                return;
            var fn = path.join(__dirname, 'fs_test.js.symlink');
            try {
                fs.unlink(fn);
            } catch (e) { }

            fs.symlink(path.join(__dirname, 'fs_test.js'), fn);
            fs.lchmod(fn, 511);

            var st = fs.stat(path.join(__dirname, 'fs_test.js'));
            var oldm = st.mode;
            var st = fs.lstat(fn);

            assert.notEqual(st.mode & 511, oldm & 511);
            assert.equal(st.mode & 511, 511);

            fs.unlink(fn);
        });

        (isIOS ? it.skip : it)("fs.chown", () => {
            var fn = path.join(__dirname, 'fs_test.js' + vmid);
            fs.writeFile(fn, 'chown test');
            if (require('os').userInfo().username != 'root')
                assert.throws(() => {
                    fs.chown(fn, 23, 45)
                });
            else {
                assert.doesNotThrow(() => fs.chown(fn, 23, 45));
                var st = fs.stat(fn);
                assert.equal(st.uid, 23);
                assert.equal(st.gid, 45);
            }
            fs.unlink(fn);
        });

        (isIOS ? it.skip : it)("fs.lchown", () => {
            var fn = path.join(__dirname, 'fs_test.js.symlink');

            if (fs.exists(fn))
                fs.unlink(fn);
            fs.symlink(path.join(__dirname, 'fs_test.js'), fn);
            if (require('os').userInfo().username != 'root')
                assert.throws(() => {
                    fs.lchown(fn, 23, 45)
                });
            else {
                assert.doesNotThrow(() => fs.lchown(fn, 23, 45));
                var st = fs.stat(path.join(__dirname, 'fs_test.js'));
                var lst = fs.lstat(fn);
                assert.notEqual(st.uid, 23);
                assert.notEqual(st.gid, 45);
                assert.equal(lst.uid, 23);
                assert.equal(lst.gid, 45);
            }
            fs.unlink(fn);
        });

        it("fs.utimes", () => {
            var fn = path.join(__dirname, 'fs_test.js.utimes' + vmid);
            fs.writeFile(fn, 'utimes test');
            try {
                var atime = 1000000;
                var mtime = 2000000;
                fs.utimes(fn, atime, mtime);
                var st = fs.stat(fn);
                assert.equal(Math.floor(st.atime / 1000), atime);
                assert.equal(Math.floor(st.mtime / 1000), mtime);
            } finally {
                fs.unlink(fn);
            }
        });

        it("fs.lutimes", () => {
            var fn = path.join(__dirname, 'fs_test.js.lutimes' + vmid);
            var target = path.join(__dirname, 'fs_test.js');
            fs.symlink(target, fn);
            try {
                var atime = 1111111;
                var mtime = 2222222;
                fs.lutimes(fn, atime, mtime);
                // symlink times changed, target NOT changed
                var lst = fs.lstat(fn);
                var st = fs.stat(target);
                assert.equal(Math.floor(lst.atime / 1000), atime);
                assert.equal(Math.floor(lst.mtime / 1000), mtime);
                assert.notEqual(Math.floor(st.mtime / 1000), mtime);
            } finally {
                fs.unlink(fn);
            }
        });

        it("fs.futimes", () => {
            var fn = path.join(__dirname, 'fs_test.js.futimes' + vmid);
            fs.writeFile(fn, 'futimes test');
            var fd = fs.open(fn, 'r+');
            try {
                var atime = 3000000;
                var mtime = 4000000;
                fs.futimes(fd, atime, mtime);
                var st = fs.stat(fn);
                assert.equal(Math.floor(st.atime / 1000), atime);
                assert.equal(Math.floor(st.mtime / 1000), mtime);
            } finally {
                fs.close(fd);
                fs.unlink(fn);
            }
        });
    }

    it("file read & write", () => {
        var f = fs.openFile(path.join(__dirname, 'fs_test.js'));

        var f1 = fs.openFile(path.join(__dirname, 'fs_test.js.bak' + vmid), 'w+');
        f1.write(f.read(f.size()));

        f1.rewind();
        var b = f1.read(f1.size() + 100);
        assert.equal(true, f1.eof());
        assert.equal(f1.size(), b.length);

        f.close();
        f1.close();
        fs.unlink(path.join(__dirname, 'fs_test.js.bak' + vmid));
    });

    it("readFile", () => {
        var f = fs.openFile(path.join(__dirname, 'fs_test.js'));
        var d = f.readAll();

        var s = fs.readFile(path.join(__dirname, "fs_test.js"));
        assert.deepEqual(s, d);

        s = fs.readFile(path.join(__dirname, "fs_test.js"), "utf8");
        assert.deepEqual(s, d.toString());

        s = fs.readFile(path.join(__dirname, "fs_test.js"), {
            encoding: "utf8"
        });
        assert.deepEqual(s, d.toString());

        f.close();
    });

    it("readTextFile", () => {
        var f = fs.openFile(path.join(__dirname, 'fs_test.js'));

        var s = fs.readTextFile(path.join(__dirname, "fs_test.js"));
        assert.equal(s, f.read(f.size()).toString());

        f.close();
    });

    it("openTextStream", () => {
        var f = fs.openTextStream(path.join(__dirname, 'fs_test.js'));
        f.EOL = '\n';

        var a = fs.readTextFile(path.join(__dirname, "fs_test.js")).replace(/\r/g, "").split("\n");
        var b = f.readLines();
        assert.deepEqual(a, b);

        f.close();
    });

    it("tell", () => {
        var f = fs.openFile(path.join(__dirname, 'fs_test.js'));
        var st = fs.stat(path.join(__dirname, 'fs_test.js'));

        f.read(f.size());

        assert.equal(st.size, f.tell());
        f.rewind();
        assert.equal(0, f.tell());

        f.close();
    });

    it("seek", () => {
        var f = fs.openFile(path.join(__dirname, 'fs_test.js'));
        f.seek(f.size() + 10, fs.SEEK_SET);
        assert.equal(f.tell(), f.size() + 10);
        f.seek(10, fs.SEEK_SET);
        var b = f.read(f.size());
        assert.equal(f.size() - 10, b.length);
        f.close();
    });

    it("seek 64 bits", () => {
        var f = fs.openFile(path.join(__dirname, 'fs_test.js'));
        f.seek(f.size() + 8589934592, fs.SEEK_SET);
        assert.equal(f.tell(), f.size() + 8589934592);
        f.close();
    });

    it("copyTo", () => {
        var f = fs.openFile(path.join(__dirname, 'fs_test.js'));
        var f1 = fs.openFile(path.join(__dirname, 'fs_test.js.bak' + vmid), 'w');

        var s = f.copyTo(f1, 100);
        assert.equal(s, 100);
        assert.equal(f1.size(), 100);
        f.copyTo(f1);
        assert.equal(f1.size(), f.size());

        f.close();
        f1.close();

        fs.unlink(path.join(__dirname, 'fs_test.js.bak' + vmid));
    });

    it("zip folder", () => {
        function save_zip(n) {
            var zipfile = zip.open(path.join(__dirname, 'unzip_test.zip'), "w");
            zipfile.write(new Buffer('test ' + n), 'test.txt');
            zipfile.write(new Buffer(`module.exports = "fibjs-test-require-${n}"`), 'test.js');
            zipfile.close();
        }

        function test_zip(n, first_test_n = n) {
            assert.equal(fs.readTextFile(path.join(__dirname, "unzip_test.zip$", "test.txt")),
                "test " + n);
            assert.equal(require('./unzip_test.zip$/test.js'), `fibjs-test-require-${first_test_n}`)
        }

        var first_test_n = 1;

        save_zip(first_test_n);
        coroutine.sleep(1000);
        test_zip(first_test_n);

        save_zip(2);
        test_zip(1, first_test_n);

        coroutine.sleep(4000);
        test_zip(2, first_test_n);
    });

    it("zip data", () => {
        function save_zip(n) {
            var stream = new io.MemoryStream();
            var zipfile = zip.open(stream, "w");
            zipfile.write(new Buffer('test ' + n), 'test.txt');
            zipfile.close();

            stream.rewind();
            fs.setZipFS("/unzip_test.zip", stream.readAll());
        }

        function test_zip(n) {
            assert.equal(fs.readTextFile(path.join("/unzip_test.zip$", "test.txt")),
                "test " + n);
        }

        save_zip(1);
        test_zip(1);

        save_zip(2);
        test_zip(2);

        /**
         * content of zip-file(e.g. named with 'safename.zip') would be cached when
         * read by 'fs' module, and expire after 3000ms, so here we sleep 4000ms to
         * wait cached 'unzip_test.zip' expired
         */
        coroutine.sleep(4000);
        test_zip(2);

        fs.clearZipFS("/unzip_test.zip");

        assert.throws(() => {
            test_zip(2);
        });
    });

    describe('read', () => {
        var fd;
        before(() => fd = fs.open(path.join(__dirname, 'fs_files', 'read.txt')));
        after(() => fs.close(fd));

        describe('fiber sync', () => {
            it('zero read', () => {
                const buf = Buffer.alloc(1);
                let bytes = fs.read(fd, buf);
                assert.equal(bytes, 0);
                assert.deepEqual(buf, Buffer.alloc(1));

                bytes = fs.read(fd, buf, 0);
                assert.equal(bytes, 0);
                assert.deepEqual(buf, Buffer.alloc(1));

                bytes = fs.read(fd, buf, 0, 0);
                assert.equal(bytes, 0);
                assert.deepEqual(buf, Buffer.alloc(1));

                bytes = fs.read(fd, buf, 0, 0, 0);
                assert.equal(bytes, 0);
                assert.deepEqual(buf, Buffer.alloc(1));
            });

            it('full read', () => {
                const buf = Buffer.alloc(15);
                const bytes = fs.read(fd, buf, 0, 15, 0);
                assert.equal(bytes, 15);
                assert.deepEqual(buf, new Buffer('abcdefg\nhijklmn'));
            });

            it('repeat read', () => {
                const buf1 = Buffer.alloc(15);
                const buf2 = Buffer.alloc(15);
                const buf3 = Buffer.alloc(1);
                const bytes1 = fs.read(fd, buf1, 0, 15, 0);
                const bytes2 = fs.read(fd, buf2, 0, 15, 0);
                const bytes3 = fs.read(fd, buf3, 0, 1);
                assert.equal(bytes1, 15);
                assert.equal(bytes2, 15);
                assert.equal(bytes3, 0);
                assert.deepEqual(buf1, new Buffer('abcdefg\nhijklmn'));
                assert.deepEqual(buf2, new Buffer('abcdefg\nhijklmn'));
                assert.deepEqual(buf3, Buffer.alloc(1));
            });

            it('offset error read', () => {
                const buf = Buffer.alloc(1);
                assert.throws(() => fs.read(fd, buf, 1, 1, 0));
                assert.throws(() => fs.read(fd, buf, 2, 1, 0));
                assert.throws(() => fs.read(fd, buf, -1, 1, 0));
                assert.doesNotThrow(() => fs.read(fd, buf, 0, 1, 0));
            });

            it('beyond buffer error read', () => {
                const buf = Buffer.alloc(4);
                assert.throws(() => fs.read(fd, buf, 2, 3, 0));
                assert.throws(() => fs.read(fd, buf, 3, 2, 0));
                assert.throws(() => fs.read(fd, buf, 0, -1, 0));
                assert.doesNotThrow(() => fs.read(fd, buf, 2, 2, 0));
            });

            it('spec offset read', () => {
                const buf = Buffer.alloc(16);
                buf.write('x');
                let bytes = fs.read(fd, buf, 1, 15, 0);
                assert.equal(bytes, 15);
                assert.deepEqual(buf, new Buffer('xabcdefg\nhijklmn'));
            });

            it('spec position read', () => {
                const buf = Buffer.alloc(14);
                let bytes = fs.read(fd, buf, 0, 14, 1);
                assert.equal(bytes, 14);
                assert.deepEqual(buf, new Buffer('bcdefg\nhijklmn'));
            });
        });

        describe('block sync', () => {
            it('zero read', () => {
                const buf = Buffer.alloc(1);
                let bytes = fs.read(fd, buf);
                assert.equal(bytes, 0);
                assert.deepEqual(buf, Buffer.alloc(1));

                bytes = fs.read(fd, buf, 0);
                assert.equal(bytes, 0);
                assert.deepEqual(buf, Buffer.alloc(1));

                bytes = fs.read(fd, buf, 0, 0);
                assert.equal(bytes, 0);
                assert.deepEqual(buf, Buffer.alloc(1));

                bytes = fs.read(fd, buf, 0, 0, 0);
                assert.equal(bytes, 0);
                assert.deepEqual(buf, Buffer.alloc(1));
            });

            it('full read', () => {
                const buf = Buffer.alloc(15);
                const bytes = fs.readSync(fd, buf, 0, 15, 0);
                assert.equal(bytes, 15);
                assert.deepEqual(buf, new Buffer('abcdefg\nhijklmn'));
            });

            it('repeat read', () => {
                const buf1 = Buffer.alloc(15);
                const buf2 = Buffer.alloc(15);
                const buf3 = Buffer.alloc(1);
                const bytes1 = fs.readSync(fd, buf1, 0, 15, 0);
                const bytes2 = fs.readSync(fd, buf2, 0, 15, 0);
                const bytes3 = fs.readSync(fd, buf3, 0, 1);
                assert.equal(bytes1, 15);
                assert.equal(bytes2, 15);
                assert.equal(bytes3, 0);
                assert.deepEqual(buf1, new Buffer('abcdefg\nhijklmn'));
                assert.deepEqual(buf2, new Buffer('abcdefg\nhijklmn'));
                assert.deepEqual(buf3, Buffer.alloc(1));
            });

            it('offset error read', () => {
                const buf = Buffer.alloc(1);
                assert.throws(() => fs.readSync(fd, buf, 1, 15, 0));
                assert.throws(() => fs.readSync(fd, buf, 2, 15, 0));
                assert.throws(() => fs.readSync(fd, buf, -1, 1, 0));
                assert.doesNotThrow(() => fs.readSync(fd, buf, 0, 1, 0));
            });

            it('beyond buffer error read', () => {
                const buf = Buffer.alloc(4);
                assert.throws(() => fs.readSync(fd, buf, 2, 3, 0));
                assert.throws(() => fs.readSync(fd, buf, 3, 2, 0));
                assert.doesNotThrow(() => fs.readSync(fd, buf, 2, 2, 0));
            });

            it('spec offset read', () => {
                const buf = Buffer.alloc(16);
                buf.write('x');
                let bytes = fs.readSync(fd, buf, 1, 15, 0);
                assert.equal(bytes, 15);
                assert.deepEqual(buf, new Buffer('xabcdefg\nhijklmn'));
            });

            it('spec position read', () => {
                const buf = Buffer.alloc(14);
                let bytes = fs.readSync(fd, buf, 0, 14, 1);
                assert.equal(bytes, 14);
                assert.deepEqual(buf, new Buffer('bcdefg\nhijklmn'));
            });
        });

        describe('event callback', () => {
            it('zero read', done => {
                const buf = Buffer.alloc(1);
                fs.read(fd, buf, (err, bytes) => {
                    if (err) done(err)
                    else {
                        assert.equal(bytes, 0);
                        assert.deepEqual(buf, Buffer.alloc(1));
                        fs.read(fd, buf, 0, (err, bytes) => {
                            if (err) done(err)
                            else {
                                assert.equal(bytes, 0);
                                assert.deepEqual(buf, Buffer.alloc(1));
                                fs.read(fd, buf, 0, 0, (err, bytes) => {
                                    if (err) done(err)
                                    else {
                                        assert.equal(bytes, 0);
                                        assert.deepEqual(buf, Buffer.alloc(1));
                                        fs.read(fd, buf, 0, 0, 0, (err, bytes) => {
                                            if (err) done(err)
                                            else {
                                                assert.equal(bytes, 0);
                                                assert.deepEqual(buf, Buffer.alloc(1));
                                                done();
                                            }
                                        });
                                    }
                                });
                            }
                        });
                    }
                });
            });

            it('full read', done => {
                const buf = Buffer.alloc(15);
                fs.read(fd, buf, 0, 15, 0, (err, bytes) => {
                    if (err) done(err)
                    else {
                        assert.equal(bytes, 15);
                        assert.deepEqual(buf, new Buffer('abcdefg\nhijklmn'));
                        done();
                    }
                });
            });

            it('repeat read', done => {
                const buf1 = Buffer.alloc(15);
                const buf2 = Buffer.alloc(15);
                const buf3 = Buffer.alloc(1);
                fs.read(fd, buf1, 0, 15, 0, (err, bytes1) => {
                    if (err) done(err)
                    else {
                        fs.read(fd, buf2, 0, 15, 0, (err, bytes2) => {
                            if (err) done(err)
                            else {
                                fs.read(fd, buf2, 3, 1, (err, bytes3) => {
                                    if (err) done(err)
                                    else {
                                        assert.equal(bytes1, 15);
                                        assert.equal(bytes2, 15);
                                        assert.equal(bytes3, 0);
                                        assert.deepEqual(buf1, new Buffer('abcdefg\nhijklmn'));
                                        assert.deepEqual(buf2, new Buffer('abcdefg\nhijklmn'));
                                        assert.deepEqual(buf3, Buffer.alloc(1));
                                        done();
                                    }
                                });
                            }
                        });
                    }
                });
            });

            it('offset error read', done => {
                const buf = Buffer.alloc(1);
                fs.read(fd, buf, 1, 1, 0, (err, bytes) => {
                    if (err) {
                        fs.read(fd, buf, 2, 1, 0, (err, bytes) => {
                            if (err) {
                                fs.read(fd, buf, -1, 1, 0, (err, byts) => {
                                    if (err) {
                                        fs.read(fd, buf, 0, 1, 0, (err, byts) => {
                                            if (err) done(err)
                                            else
                                                done();
                                        });
                                    } else done(new Error('should throws'));
                                });
                            } else done(new Error('should throws'));
                        });
                    } else done(new Error('should throws'));
                });
            });

            it('beyond buffer error read', done => {
                const buf = Buffer.alloc(4);
                fs.read(fd, buf, 2, 3, 0, (err, bytes) => {
                    if (err) {
                        fs.read(fd, buf, 3, 2, 0, (err, bytes) => {
                            if (err) {
                                fs.read(fd, buf, 2, 2, 0, (err, byts) => {
                                    if (err) done(err)
                                    else
                                        done();
                                });
                            } else done(new Error('should throws'));
                        });
                    } else done(new Error('should throws'));
                });
            });

            it('spec offset read', done => {
                const buf = Buffer.alloc(16);
                buf.write('x');
                fs.read(fd, buf, 1, 15, 0, (err, bytes) => {
                    if (err) done(err)
                    else {
                        assert.equal(bytes, 15);
                        assert.deepEqual(buf, new Buffer('xabcdefg\nhijklmn'));
                        done();
                    }
                });
            });

            it('spec position read', done => {
                const buf = Buffer.alloc(14);
                fs.read(fd, buf, 0, 14, 1, (err, bytes) => {
                    if (err) done(err)
                    else {
                        assert.equal(bytes, 14);
                        assert.deepEqual(buf, new Buffer('bcdefg\nhijklmn'));
                        done();
                    }
                });
            });
        });
    });

    describe('write', () => {
        var fd;

        beforeEach(() => fd = fs.open(path.join(homedir, 'test.txt'), 'w+'));
        afterEach(() => fs.close(fd));
        after(() => {
            try {
                fs.unlink(path.join(homedir, 'test.txt'));
            } catch (e) { }
        });

        it("normal write", () => {
            var buf = Buffer.alloc(3);
            fs.write(fd, 'abc');
            fs.read(fd, buf, 0, 3, 0);
            assert.deepEqual(buf, new Buffer('abc'));
        });

        it("multi write", () => {
            var buf = Buffer.alloc(6);
            fs.write(fd, 'abc');
            fs.write(fd, 'abc');
            fs.read(fd, buf, 0, 6, 0);
            assert.deepEqual(buf, new Buffer('abcabc'));
        });

        it("spec offset", () => {
            var buf = Buffer.from("abc");
            fs.write(fd, buf, 1);
            fs.write(fd, 'd');
            fs.read(fd, buf, 0, 3, 0);
            assert.deepEqual(buf, new Buffer('bcd'));

            assert.throws(() => {
                fs.write(fd, buf, -1);
            });

            assert.throws(() => {
                fs.write(fd, buf, 4);
            });
        });

        it("spec length", () => {
            var buf = Buffer.from("abc");
            fs.write(fd, buf, 0, 2);
            fs.write(fd, 'd');
            fs.read(fd, buf, 0, 3, 0);
            assert.deepEqual(buf, new Buffer('abd'));

            var buf = Buffer.from("abc");
            fs.write(fd, buf, 1, 2, 0);
            fs.write(fd, 'd');
            fs.read(fd, buf, 0, 3, 0);
            assert.deepEqual(buf, new Buffer('bcd'));

            assert.throws(() => {
                fs.write(fd, buf, 0, 4);
            });
        });

        it("spec position", () => {
            var buf = Buffer.from("abc");
            fs.write(fd, buf, 0, 3, 10);
            fs.read(fd, buf, 0, 3, 10);
            assert.deepEqual(buf, new Buffer('abc'));
        });
    });

    it("readdir", () => {
        var fl = fs.readdir(path.join(__dirname, 'dir_test'));
        assert.deepEqual(fl.sort(), ["dir1", "file1", "file2"]);

        var fl = fs.readdir(path.join(__dirname, 'dir_test'), { recursive: true });

        if (win)
            assert.deepEqual(fl.sort(), ["dir1", "dir1\\file3", "file1", "file2"]);
        else
            assert.deepEqual(fl.sort(), ["dir1", "dir1/file3", "file1", "file2"]);
    });

    it("readdir with withFileTypes", () => {
        var entries = fs.readdir(path.join(__dirname, 'dir_test'), { withFileTypes: true });
        assert.equal(entries.length, 3);

        // Check entries are DirEntry objects
        entries.forEach(entry => {
            assert.ok(entry.name);
            assert.ok(entry.parentPath);
            assert.equal(typeof entry.isDirectory, 'function');
            assert.equal(typeof entry.isFile, 'function');
        });

        // Sort entries by name for consistent testing
        entries.sort((a, b) => a.name.localeCompare(b.name));

        // Check first entry (dir1)
        assert.equal(entries[0].name, "dir1");
        assert.equal(entries[0].parentPath, path.join(__dirname, 'dir_test'));
        assert.ok(entries[0].isDirectory());
        assert.ok(!entries[0].isFile());

        // Check second entry (file1)
        assert.equal(entries[1].name, "file1");
        assert.equal(entries[1].parentPath, path.join(__dirname, 'dir_test'));
        assert.ok(!entries[1].isDirectory());
        assert.ok(entries[1].isFile());

        // Check third entry (file2)
        assert.equal(entries[2].name, "file2");
        assert.equal(entries[2].parentPath, path.join(__dirname, 'dir_test'));
        assert.ok(!entries[2].isDirectory());
        assert.ok(entries[2].isFile());

        // Test with recursive option
        var recursiveEntries = fs.readdir(path.join(__dirname, 'dir_test'), {
            recursive: true,
            withFileTypes: true
        });
        assert.equal(recursiveEntries.length, 4);

        // All should be DirEntry objects
        recursiveEntries.forEach(entry => {
            assert.ok(entry.name);
            assert.ok(entry.parentPath);
        });
    });

    it("readdir with withFileTypes using relative path", () => {
        // Save current directory and switch to test directory
        var cwd = process.cwd();
        process.chdir(__dirname);

        try {
            // Test relative path with withFileTypes option
            var entries = fs.readdir('./dir_test', { withFileTypes: true });
            assert.equal(entries.length, 3);

            // Check entries are DirEntry objects
            entries.forEach(entry => {
                assert.ok(entry.name);
                assert.ok(entry.parentPath);
                assert.equal(typeof entry.isDirectory, 'function');
                assert.equal(typeof entry.isFile, 'function');
            });

            // Sort entries by name for consistent testing
            entries.sort((a, b) => a.name.localeCompare(b.name));

            // Check first entry (dir1)
            assert.equal(entries[0].name, "dir1");
            assert.ok(entries[0].isDirectory());
            assert.ok(!entries[0].isFile());

            // Check second entry (file1)
            assert.equal(entries[1].name, "file1");
            assert.ok(!entries[1].isDirectory());
            assert.ok(entries[1].isFile());

            // Check third entry (file2)
            assert.equal(entries[2].name, "file2");
            assert.ok(!entries[2].isDirectory());
            assert.ok(entries[2].isFile());
        } finally {
            // Restore original directory
            process.chdir(cwd);
        }
    });

    it("writeFile & appendFile", () => {
        var fn = path.join(__dirname, 'fs_test.js' + vmid);

        fs.writeFile(fn, 'data to be written');
        assert.equal(fs.readFile(fn), 'data to be written');

        fs.writeFile(fn, 'data to be written', "utf8");
        assert.equal(fs.readFile(fn), 'data to be written');

        fs.writeFile(fn, 'data to be written', {
            encoding: "utf8"
        });
        assert.equal(fs.readFile(fn), 'data to be written');

        fs.appendFile(fn, 'data to be appended');
        assert.equal(fs.readFile(fn), 'data to be writtendata to be appended');

        fs.unlink(fn);
    });

    it("link", () => {
        var fn = path.join(__dirname, 'fs_test.js' + vmid);
        var fn1 = path.join(__dirname, 'fs_test.js.link' + vmid);
        fs.writeFile(fn, 'link test');
        fs.link(fn, fn1);
        assert.deepEqual(fs.stat(fn).ctime, fs.stat(fn1).ctime);
        assert.deepEqual(fs.stat(fn).atime, fs.stat(fn1).atime);
        assert.deepEqual(fs.stat(fn).mtime, fs.stat(fn1).mtime);
        assert.equal(fs.stat(fn).mode, fs.stat(fn1).mode);
        assert.equal(fs.stat(fn).size, fs.stat(fn1).size);
        fs.unlink(fn);
        assert.equal(fs.readFile(fn1), 'link test');
        fs.unlink(fn1);
    });

    it("truncate", () => {
        var fn = path.join(__dirname, 'fs_test.js' + vmid);
        fs.writeFile(fn, 'truncate test');
        assert.equal(fs.stat(fn).size, 13);
        fs.truncate(fn, 100);
        assert.equal(fs.stat(fn).size, 100);
        fs.truncate(fn, 10);
        assert.equal(fs.stat(fn).size, 10);
        fs.unlink(fn);
    });

    describe("symlink & lstat & readlink & realpath", () => {
        function proc() {
            var fn = path.join(__dirname, 'fs_test.js');
            var fn1 = path.join(__dirname, 'fs_test.js.symlink' + vmid);
            fs.symlink(fn, fn1);
            var _lstat = fs.lstat(fn1);

            assert_stat_property(_lstat);

            assert.ok(_lstat.isSymbolicLink());
            assert.equal(fs.readlink(fn1), fn);
            assert.equal(fs.realpath(fn1), fn);
            assert.equal(fs.readFile(fn).toString(), fs.readFile(fn1).toString());
            if (win) {
                var dir = path.join(__dirname, 'dirtest');
                var dir1 = path.join(__dirname, 'dirtestsymlink');
                var file = path.join(dir, 'file');
                var file1 = path.join(dir1, 'file');
                fs.mkdir(dir, 511);
                fs.writeFile(file, 'symlink test');
                fs.symlink(dir, dir1, 'junction');
                assert.equal(fs.readFile(file1).toString(), 'symlink test');
                fs.unlink(file1);
                fs.rmdir(dir1);
                fs.rmdir(dir);
            }
            fs.unlink(fn1);
        }
        it("number version", () => {
            proc(false);
        });

        it("realpath should resolve symlink in middle of path", () => {
            // Create test directory structure
            var realDir = path.join(__dirname, 'realpath_test_real' + vmid);
            var linkDir = path.join(__dirname, 'realpath_test_link' + vmid);
            var realFile = path.join(realDir, 'test.txt');
            var linkFile = path.join(linkDir, 'test.txt');

            // cleanup first in case of previous failed test
            try { fs.unlink(linkDir); } catch (e) { }
            try { fs.unlink(realFile); } catch (e) { }
            try { fs.rmdir(realDir); } catch (e) { }

            fs.mkdir(realDir);
            fs.writeFile(realFile, 'realpath test');
            fs.symlink(realDir, linkDir);

            // realpath should resolve symlink in the middle of path
            assert.equal(fs.realpath(linkFile), realFile);

            // cleanup
            fs.unlink(linkDir);
            fs.unlink(realFile);
            fs.rmdir(realDir);
        });
    })

    it("access", () => {
        var fn = path.join(__dirname, 'fs_test.js');
        var fn1 = path.join(__dirname, 'fs_test.js.access' + vmid);
        assert.doesNotThrow(() => {
            fs.access(fn, fs.constants.F_OK | fs.constants.W_OK | fs.constants.R_OK);
        });
        assert.throws(() => {
            fs.access(fn1, fs.constants.F_OK);
        })
    })

    it("promises", async () => {
        var f = await fs.promises.openFile(path.join(__dirname, 'fs_test.js'));
        assert.isTrue(f.stat() instanceof Promise);
        assert.property(await f.stat(), 'size');
        await f.close();

        var f = await fs.promises.open(path.join(__dirname, 'fs_test.js'));
        assert.isTrue(f.stat() instanceof Promise);
        assert.property(await f.stat(), 'size');
        await f.close();
    });

    it("write methods return value validation", () => {
        var fn = path.join(__dirname, 'fs_write_test' + vmid);

        try {
            // Test fs.writeFile return value
            var testContent = 'File write test content';
            var result = fs.writeFile(fn, testContent);
            assert.equal(result, 23);

            // Verify file was written
            var readContent = fs.readFile(fn).toString();
            assert.equal(readContent, testContent);

            // Test fs.appendFile return value
            result = fs.appendFile(fn, ' appended');
            assert.equal(result, 9);

            // Test file handle write return value (write now returns Boolean)
            var f = fs.openFile(fn, 'w+');
            var writeData = 'Handle write test';
            var bytesWritten = f.write(writeData);
            assert.equal(bytesWritten, true);

            // Test write with Buffer
            f.rewind();
            f.truncate(0);
            var bufferData = new Buffer('Buffer write test');
            bytesWritten = f.write(bufferData);
            assert.equal(bytesWritten, true);

            // Test write with empty string
            f.rewind();
            f.truncate(0);
            bytesWritten = f.write('');
            assert.equal(bytesWritten, true);

            f.close();
        } finally {
            try {
                fs.unlink(fn);
            } catch (e) { }
        }
    });

    describe("stream close event", () => {
        it("EOF autoDestroy triggers close", () => {
            var fn = path.join(__dirname, '_test_close_fs_' + coroutine.vmid);
            fs.writeFile(fn, 'hello world');
            try {
                var f = fs.openFile(fn);
                var events = [];
                var done = new coroutine.Event();
                f.on('data', () => { events.push('data'); });
                f.on('end', () => { events.push('end'); });
                f.on('close', () => { events.push('close'); done.set(); });
                done.wait();
                assert.ok(events.indexOf('end') >= 0);
                assert.ok(events.indexOf('close') >= 0);
                assert.ok(events.indexOf('end') < events.indexOf('close'));
            } finally {
                fs.unlink(fn);
            }
        });

        it("destroy() triggers close", () => {
            var fn = path.join(__dirname, '_test_close_fs2_' + coroutine.vmid);
            fs.writeFile(fn, 'hello world');
            try {
                var f = fs.openFile(fn);
                var done = new coroutine.Event();
                var closed = false;
                f.on('close', () => { closed = true; done.set(); });
                f.destroy();
                done.wait();
                assert.strictEqual(closed, true);
            } finally {
                fs.unlink(fn);
            }
        });

        it("destroy(err) emits error then close", () => {
            var fn = path.join(__dirname, '_test_close_fs3_' + coroutine.vmid);
            fs.writeFile(fn, 'hello world');
            try {
                var f = fs.openFile(fn);
                var events = [];
                var done = new coroutine.Event();
                f.on('error', () => { events.push('error'); });
                f.on('close', () => { events.push('close'); done.set(); });
                f.destroy(new Error('test'));
                done.wait();
                assert.deepStrictEqual(events, ['error', 'close']);
            } finally {
                fs.unlink(fn);
            }
        });
    });

});