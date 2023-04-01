const fs = require('fs');
const path = require('path');
const { HtmlSpliter, TestSpliter } = require('fib-spliter');

function split_html(filename) {
    var html = fs.readTextFile(filename);

    const splitter = new HtmlSpliter({
        ignore_tags: [
            'svg'
        ],
        before_split: function (html) {
            html = html.replace(/<p>[0-9\r\n]*<\/p>/g, '');
            html = html.replace(/<h3 /g, '</div><div><h3 ');
            html = html.replace(/<code>/g, '<code>```\n');
            html = html.replace(/<\/code>/g, '\n```</code>');
            return html;
        }
    });
    return splitter.split(html);
}

function split_test(filename) {
    var code = fs.readTextFile(filename);

    const spliter = new TestSpliter();
    return spliter.split(code);
}

function split_folder(folders) {
    var all_nodes = [];

    folders.forEach(function (folder) {
        var dir = fs.readdir(folder);
        dir.forEach(function (entry) {
            var filename = path.join(folder, entry);
            console.log(`splitting ${filename} ...`);
            if (!fs.stat(filename).isDirectory()) {
                var nodes = path.extname(filename) == '.js' ? split_test(filename) :
                    path.extname(filename) == '.html' ? split_html(filename) : [];
                all_nodes = all_nodes.concat(nodes);
            }
        });
    });

    return all_nodes;
}

module.exports = split_folder;