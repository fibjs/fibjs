var UglifyJS = require("uglify-js");
var fs = require('fs');
var path = require('path');

var platforms = {
    'windows': {
        IFMACRO: '_WIN32',
    },
    'darwin': {
        IFMACRO: '__APPLE__',
        scripts: {
            'scriptStart_regExternal': 'scriptStart_regExternal.js',
            'scriptEnd_inwardPostMessage': 'scriptEnd_inwardPostMessage.js',
            'scriptEnd_afterInternal': 'scriptEnd_afterInternal.js'
        }
    },
    'linux': {
        IFMACRO: ''
    }
}

function uglify(code) {
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

    return src.filter(x => x);
}

function wrapCChar(platform, code) {
    return `L"${code}"`;
}

const LF = "\n";
const LF_QUOTE = '"\n"';

Object.keys(platforms).forEach((platform) => {
    var basedir = path.join(__dirname, `../fibjs/src/gui`);
    var js_path = path.join(basedir, `${platform}/script/console.js`);
    var jscpp_path = path.join(basedir, `${platform}/script/console_js.cpp`);

    if (!fs.exists(js_path))
        return;

    var pconfig = platforms[platform];

    var code = fs.readTextFile(js_path);

    var otherFragements = [];
    var C_CHAR_T = platform === 'windows' ? 'wchar_t*' : 'wchar_t*'

    var scripts = platforms[platform].scripts;
    scripts && Object.keys(scripts).forEach(varname => {
        var filename = path.resolve(basedir, `./${platform}/script/${scripts[varname]}`);
        if (!fs.exists(filename)) return;

        var code = fs.readTextFile(filename) || ''
        var minified = uglify(code).join(LF_QUOTE);
        otherFragements.push(
            `const ${C_CHAR_T} ${varname} = ${wrapCChar(platform, minified)}`
        );
    });

    var console_js_char = wrapCChar(platform, uglify(code).join(LF_QUOTE))

    if (otherFragements.length) otherFragements.push("");

    switch (platform) {
        case 'windows':
            fs.writeFile(jscpp_path,
                `#ifdef ${pconfig.IFMACRO}\n\nnamespace fibjs {\nconst ${C_CHAR_T} g_console_js = ${console_js_char};\n${otherFragements.join(";" + LF)}}\n\n#endif`);
            break
        case 'darwin':
            fs.writeFile(jscpp_path,
                `#ifdef ${pconfig.IFMACRO}\n\nnamespace fibjs {\nconst ${C_CHAR_T} g_console_js = ${console_js_char};\n${otherFragements.join(";" + LF)}}\n\n#endif`);
            break
    }
});