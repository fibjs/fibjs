var fs = require("fs");
var path = require("path");

var js_protocol = require('devtools-protocol/json/js_protocol.json');
var browser_protocol = require('devtools-protocol/json/browser_protocol.json');

var methods = {};

function build(protocol) {
    protocol.domains.forEach(domain => {
        if (!domain.experimental) {
            var ms = [];
            domain.commands.forEach(command => !command.experimental && ms.push(command.name));
            methods[domain.domain] = ms;
        }
    });
}

build(js_protocol);
build(browser_protocol);

var fname = path.join(__dirname, "../fibjs/src/gui/cef/CefWebView_dev.h")

var txts = [];
var domtxts = [];

Object.keys(methods).forEach(dom => {
    var txt = `const char* s_${dom}_apis[] = {\n\t"` + methods[dom].join(`",\n\t"`) + `",\n\tNULL\n};\n`;
    txts.push(txt);
    domtxts.push(`\t{ "${dom}", s_${dom}_apis },`);
})

var src = txts.join('\n') + "\nconst void* s_domains[][2] = {\n" + domtxts.join('\n') + "\n\t{ NULL, NULL }\n};"

fs.writeFile(fname, src);

console.log("write", fname)