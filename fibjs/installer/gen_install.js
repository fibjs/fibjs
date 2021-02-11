var fs = require("fs");
var process = require("process");

var build = false;
var st1 = fs.stat("fibjs.exe");
if (!fs.exists("fibjs.cab"))
	build = true;
else {
	var st2 = fs.stat("fibjs.cab");
	build = st2.mtime < st1.mtime;
}

if (build) {
	process.run("makecab", ["fibjs.exe", "fibjs.cab"]);
	var st2 = fs.stat("fibjs.cab");
}

build = false;
if (!fs.exists("js.h"))
	build = true;
else {
	var st3 = fs.stat("js.h");
	build = st3.mtime < st2.mtime;
}

if (build) {
	var txt = JSON.stringify(fs.openFile("fibjs.cab").readAll().toArray());
	var src = "unsigned char js_data[] = {" + txt.substr(1, txt.length - 2) + "};\n";
	fs.writeFile("js.h", src);
}