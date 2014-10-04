#!/bin/js

var fs = require('fs');
var process = require('process');

var v8Folder = process.argv[2];

var paltFolder = 'src/base/platform';
var plats = [
	'_date_cache.cc',
	'platform-fiber.cc',
	'condition-variable.cc',
	'condition-variable.h',
	'mutex.cc',
	'mutex.h',
	'semaphore.cc',
	'semaphore.h'
];
var platTxts = [];

function save_plat() {
	platTxts = [];
	plats.forEach(function(f) {
		console.log("save", paltFolder + '/' + f);
		platTxts.push(fs.readFile(paltFolder + '/' + f));
	});
}

function update_plat() {
	var i;
	var a = paltFolder.split('/');

	for (i = 1; i < a.length; i++) {
		try {
			fs.mkdir(a.slice(0, i + 1).join('/'));
		} catch (e) {}
	}

	for (i = 0; i < plats.length; i++) {
		console.log("update", paltFolder + '/' + plats[i]);
		fs.writeFile(paltFolder + '/' + plats[i], platTxts[i]);
	}
}

function clean_folder(path) {
	var dir = fs.readdir(path);
	console.log("clean", path);
	dir.forEach(function(f) {
		var name = f.name;
		if (name !== '.' && name !== '..') {
			var fname = path + '/' + name;
			if (f.isDirectory()) {
				clean_folder(fname);
				fs.rmdir(fname);
			} else
				fs.unlink(fname);
		}
	});
}

var files = {
	'src/natives-external.cc': 1,
	'src/snapshot-external.cc': 1,
	'src/mksnapshot.cc': 1,
	'src/v8dll-main.cc': 1,
	'src/base/platform/platform-qnx.cc': 1,
	'src/base/platform/platform-cygwin.cc': 1
};

var re = [
	/^src\/d8.*$/,
	/^src\/i18n\..*$/,
	/^.*unittest.*\.cc$/,
	/^src\/test\/.*\.cc$/
];

function chk_file(fname) {
	var len = fname.length;
	if (fname.substr(len - 2, 2) != '.h' && fname.substr(len - 3, 3) != '.cc')
		return false;

	if (files[fname])
		return false;

	for (var i = 0; i < re.length; i++)
		if (re[i].test(fname))
			return false;

	return true;
}

function cp_folder(path) {
	var dir = fs.readdir(v8Folder + '/' + path);
	dir.forEach(function(f) {
		var name = f.name;
		if (name !== '.' && name !== '..') {
			var fname = path + '/' + name;
			if (f.isDirectory()) {
				fs.mkdir(fname);
				cp_folder(fname);
			} else {
				if (chk_file(fname)) {
					console.log("copy", fname);
					fs.writeFile(fname, fs.readFile(v8Folder + '/' + fname));
				}
			}
		}
	});
}


var gens = [
	'libraries.cc',
	'experimental-libraries.cc'
];

function cp_gen() {
	gens.forEach(function(f) {
		fs.writeFile('src/' + f, fs.readFile(v8Folder + '/out/ia32.release/obj/gen/' + f));
	});
}

function fix_src(path, val) {
	var dir = fs.readdir(path);
	dir.forEach(function(f) {
		var name = f.name;
		if (name.substr(name.length - 3, 3) == '.cc') {
			var fname = path + '/' + name;
			var txt = fs.readFile(fname);

			if (txt.indexOf(val) >= 0)
				return;

			console.log("fix", fname);
			fs.writeFile(fname, '#include "src/v8.h"\n\n#if ' + val + '\n\n' + txt + '\n\n#endif  // ' + val);
		}
	});
}

var archs = {
	arm: 'V8_TARGET_ARCH_ARM',
	arm64: 'V8_TARGET_ARCH_ARM64',
	mips: 'V8_TARGET_ARCH_MIPS',
	mips64: 'V8_TARGET_ARCH_MIPS64',
	ia32: 'V8_TARGET_ARCH_IA32',
	x64: 'V8_TARGET_ARCH_X64',
	x87: 'V8_TARGET_ARCH_X87'
};

function patch_src(path) {
	var dir = fs.readdir(path);
	dir.forEach(function(f) {
		var name = f.name;
		if (name !== '.' && name !== '..') {
			var fname = path + '/' + name;
			if (f.isDirectory()) {
				if (archs[name])
					fix_src(fname, archs[name]);
				else
					patch_src(fname);
			}
		}
	});
}

var plats1 = {
	'freebsd': "#ifdef FreeBSD",
	'linux': "#ifdef Linux",
	'macos': "#ifdef MacOS",
	'openbsd': "#ifdef OpenBSD",
	'solaris': "#ifdef Solaris",
	'win32': "#ifdef Windows",
	'posix': "#ifndef Windows"
};

function patch_plat() {
	for (var f in plats1) {
		var fname = paltFolder + '/platform-' + f + '.cc';
		var txt = fs.readFile(fname);
		var txt1;
		var val = plats1[f];

		if (txt.indexOf(val) >= 0)
			return;

		console.log("patch", fname);
		txt = '#include <exlib/include/osconfig.h>\n\n' + val + '\n\n' + txt + '\n\n#endif';
		txt1 = txt.replace('void OS::Sleep', 'void OS_Sleep');
		txt1 = txt1.replace('class Thread::PlatformData {', '#if 0\nclass Thread::PlatformData {');

		if (txt != txt1) {
			var idx = txt1.indexOf('} }', txt1.lastIndexOf('Thread::'));

			txt1 = txt1.substr(0, idx) + '#endif\n\n' + txt1.substr(idx);
		}

		fs.writeFile(fname, txt1);
	}
}

function patch_samp() {
	var fname = "src/sampler.cc";
	var txt = fs.readFile(fname);

	var idx = txt.lastIndexOf("#if defined(USE_SIGNALS)");
	if (idx < 0)
		return;

	var txt1 = txt.substr(0, idx);
	txt1 += "#if 0\n\n";
	txt1 += txt.substr(idx);

	txt = txt1;
	idx = txt.lastIndexOf("#endif  // USE_SIGNALS");

	txt1 = txt.substr(0, idx);
	txt1 += "#endif\n\n";
	txt1 += txt.substr(idx);

	fs.writeFile(fname, txt1);
}

save_plat();

clean_folder('include');
clean_folder('src');
clean_folder('third_party/fdlibm');

cp_folder('include')
cp_folder('src')
cp_folder('third_party/fdlibm');

cp_gen();

clean_folder('src/third_party/vtune');
fs.rmdir('src/third_party/vtune');

update_plat();
patch_samp();

patch_src('src');
patch_plat();


run('./vsmake.js');