var UglifyJS = require("uglify-js");
var fs = require('fs');
var path = require('path');

var code = fs.readTextFile(path.join(__dirname, "../fibjs/src/gui/windows/script/console.js"));
code = code.replace(/\n[ \t]*/g, '\n');
code = UglifyJS.minify(code).code;
var src = [];

var pos = 0;
var sz = 80;

function c_encode(s) {
    return s.replace(/[\\\n'"]/g, (c) => {
        if (c == '\n')
            return "\\n"
        return "\\" + c;
    });
}

while (pos < code.length - sz) {
    src.push(c_encode(code.substr(pos, sz)));
    pos += sz;
}

src.push(c_encode(code.substr(pos)));

fs.writeFile(path.join(__dirname, "../fibjs/src/gui/windows/script/console_js.cpp"),
    '#ifdef _WIN32\n\nnamespace fibjs {\nconst wchar_t* g_console_js = L"' + src.join('"\n"') + '";\n}\n\n#endif');