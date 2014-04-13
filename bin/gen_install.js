var fs = require("fs");
var process = require("process");

var build = false;

if (!fs.exists("js.cab"))
	build = true;
else {
	var st1 = fs.stat("fibjs.exe");
	var st2 = fs.stat("js.cab");
	build = st2.mtime < st1.mtime;
}

if (build)
	process.system("makecab fibjs.exe js.cab");

var txt = JSON.stringify(fs.open("js.cab").readAll());
var src = "unsigned char js_data[] = {" + txt.substr(1, txt.length - 2).replace(/,/g, ",\n") + "};\n";
if (!fs.exists("js.h") || fs.readFile("js.h") != src)
	fs.writeFile("js.h", src);