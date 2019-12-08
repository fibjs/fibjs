var UglifyJS = require("uglify-js");
var fs = require('fs');
var path = require('path');

var platforms = {
    'windows': {
        IFMACRO: '_WIN32'
    },
    'darwin': {
        IFMACRO: '__APPLE__'
    },
    'linux': {
        IFMACRO: ''
    }
}
Object.keys(platforms).forEach((platform) => {
    var js_path = path.join(__dirname, `../fibjs/src/gui/${platform}/script/console.js`);
    var jscpp_path = path.join(__dirname, `../fibjs/src/gui/${platform}/script/console_js.cpp`);

    if (!fs.exists(js_path))
        return ;

    var pconfig= platforms[platform];

    var code = fs.readTextFile(js_path);
    uglify_code: {
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
    }

    fs.writeFile(jscpp_path,
        `#ifdef ${pconfig.IFMACRO}\n\nnamespace fibjs {\nconst wchar_t* g_console_js = L"` + src.join('"\n"') + '";\n}\n\n#endif');
});