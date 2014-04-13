var fs = require("fs");
var txt = JSON.stringify(fs.open("js.cab").readAll());
fs.writeFile("js.h", "unsigned char js_data[] = {" + txt.substr(1, txt.length - 2).replace(/,/g, ",\n") + "};\n");