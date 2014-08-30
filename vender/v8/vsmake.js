#!/bin/js

var fs = require("fs");
var hash = require("hash");

var Includes = {};
var Compiles = {};
var filters = [];

function do_folder(path, base) {
	filters.push(base);

	var dir = fs.readdir(path.replace(/\\/g, '/'));

	dir.forEach(function(f) {
		var name = f.name;
		if (name.substr(0, 1) !== '.') {
			if (f.isDirectory())
				do_folder(path + '\\' + name, base + '\\' + name);
			else {
				var len = name.length;
				var bInc = name.substr(len - 2, 2) === '.h';
				var bCc = name.substr(len - 3, 3) === '.cc';

				if (name.substr(len - 2, 2) === '.h')
					Includes[path + '\\' + name] = base;
				else if (name.substr(len - 3, 3) === '.cc')
					Compiles[path + '\\' + name] = base;
			}
		}
	});
}

do_folder("include", "Header Files");
do_folder("src", "Source Files");
do_folder("third_party", "third_party");

var proj = fs.readFile('tools/proj.txt');
var filter = fs.readFile('tools/filter.txt');

var txts, f, s, h;

txts = [];
for (f in Includes) {
	txts.push('    <ClInclude Include="' + f + '" />');
}
txts.sort();
proj = proj.replace('<ClIncludes />', txts.join('\r\n'));


txts = [];
for (f in Compiles) {
	txts.push('    <ClCompile Include="' + f + '" />');
}
txts.sort();
proj = proj.replace('<ClCompiles />', txts.join('\r\n'));


fs.writeFile("v8.vcxproj", proj);


filters.sort();

txts = [];
filters.forEach(function(f) {
	h = hash.md5(f).digest().hex();

	s = '    <Filter Include="' + f + '">\r\n      <UniqueIdentifier>{';
	s += h.substr(0, 8) + '-' + h.substr(8, 4) + '-' + h.substr(12, 4) + '-' + h.substr(16, 4) + '-' + h.substr(20);
	s += '}</UniqueIdentifier>\r\n    </Filter>';

	txts.push(s);
});
filter = filter.replace('<Filters />', txts.join('\r\n'));

txts = [];
for (f in Includes) {
	txts.push('    <ClInclude Include="' + f + '">\r\n      <Filter>' + Includes[f] + '</Filter>\r\n    </ClInclude>');
}
txts.sort();
filter = filter.replace('<ClIncludes />', txts.join('\r\n'));

txts = [];
for (f in Compiles) {
	txts.push('    <ClCompile Include="' + f + '">\r\n      <Filter>' + Compiles[f] + '</Filter>\r\n    </ClCompile>');
}
txts.sort();
filter = filter.replace('<ClCompiles />', txts.join('\r\n'));


fs.writeFile("v8.vcxproj.filters", filter);