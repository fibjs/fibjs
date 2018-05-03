var fs = require("fs");
var ssl = require("ssl");
var http = require("http");
var path = require('path');
var coroutine = require('coroutine');

ssl.loadRootCerts();
var docsFolder = path.join(__dirname, "../docs/docs/awesome/");

function clean_folder(p) {
    var dir = fs.readdir(p);
    console.log("clean", p);
    dir.forEach(function (name) {
        var fname = path.join(p, name);
        var f = fs.stat(fname);
        if (f.isDirectory()) {
            clean_folder(fname);
            fs.rmdir(fname);
        } else
            fs.unlink(fname);
    });
}

clean_folder(docsFolder);
fs.mkdir(path.join(docsFolder, 'module'));

var headers = {
    "user": process.env.GITHUB_AUTH_USER || "fibjs-io"
}

var list = new Buffer(http.get("https://api.github.com/repos/fibjs/awesome/readme", { headers }).json().content, 'base64').toString();

var ls = [];
list = list.replace(/- \[(.+?)\]\((.+?)\)/g, (s, s1, s2) => {
    var u = s2.replace(/^https:\/\/github.com\//, 'https://api.github.com/repos/') + '/readme';
    console.log('getting', u);
    coroutine.sleep(100);
    var doc = new Buffer(http.get(u, { headers }).json().content, 'base64').toString();
    fs.writeTextFile(path.join(docsFolder, 'module', s1 + '.md'), doc);

    var s = `[${s1}](./module/${s1}.md)`

    ls.push('* ' + s);
    return '- ' + s;
});

fs.writeTextFile(path.join(docsFolder, 'README.md'), list);
fs.writeTextFile(path.join(docsFolder, 'SUMMARY.md'), ls.join('\n'));