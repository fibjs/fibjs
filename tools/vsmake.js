#!/bin/js

var fs = require("fs");
var hash = require("hash");
var path = require("path");

var baseDir = path.join(__dirname, '../fibjs');

var Includes = {};
var Compiles = {};
var filters = [];

var dis_archs = {
    arm: 1,
    arm64: 1,
    mips: 1,
    mips64: 1
};

function do_folder(p, base) {
    filters.push(base);

    var dir = fs.readdir(path.join(baseDir, p.replace(/\\/g, '/')));

    dir.forEach(function (name) {
        var f = fs.stat(path.join(baseDir, p.replace(/\\/g, '/') + '/' + name));
        if (f.isDirectory()) {
            if (!dis_archs[name])
                do_folder(p + '\\' + name, base + '\\' + name);
        } else {
            var len = name.length;
            var bInc = name.substr(len - 2, 2) === '.h';
            var bCc = name.substr(len - 3, 3) === '.cc' || name.substr(len - 4, 4) === '.cpp' || name.substr(len - 2, 2) === '.c';

            if (bInc)
                Includes[p + '\\' + name] = base;
            else if (bCc)
                Compiles[p + '\\' + name] = base;
        }
    });
}

do_folder("include", "Header Files");
do_folder("src", "Source Files");

var proj = fs.readTextFile(path.join(__dirname, './tpls/proj.txt'));
var filter = fs.readTextFile(path.join(__dirname, './tpls/filter.txt'));

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


fs.writeFile(path.join(baseDir, "fibjs.vcxproj"), proj);

filters.sort();

txts = [];
filters.forEach(function (f) {
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

fs.writeFile(path.join(baseDir, "fibjs.vcxproj.filters"), filter);