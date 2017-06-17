var fs = require("fs");
var util = require("util");
var path = require('path');
var ejs = require('ejs');

module.exports = function (defs, docsFolder) {
    function gen_summary() {
        var _summary = ejs.compile(fs.readTextFile(path.join(__dirname, './tmpl/SUMMARY.md')));

        fs.writeFile(path.join(docsFolder, "module", "SUMMARY.md"), _summary({
            title: '基础模块',
            defs: defs,
            type: 'module'
        }));

        fs.writeFile(path.join(docsFolder, "object", "SUMMARY.md"), _summary({
            title: '内置对象',
            defs: defs,
            type: 'interface'
        }));
    }

    function gen_readme() {
        var _readme = ejs.compile(fs.readTextFile(path.join(__dirname, './tmpl/README.md')));

        fs.writeFile(path.join(docsFolder, "module", "README.md"), _readme({
            title: '基础模块',
            defs: defs,
            type: 'module'
        }));

        fs.writeFile(path.join(docsFolder, "object", "README.md"), _readme({
            title: '内置对象',
            defs: defs,
            type: 'interface'
        }));
    }

    function inherit_method() {
        for (var m in defs) {
            var def = defs[m];
            if (def.declare.type === 'interface') {
                var ext = def.declare.extend;
                while (ext) {
                    ext = defs[ext];
                    ext.members.forEach(m => {
                        if (m.memType != 'operator' &&
                            m.name !== ext.declare.name &&
                            !m.inherit) {
                            var m1 = util.clone(m);
                            m1.inherit = true;
                            def.members.push(m1);
                        }
                    });
                    if (ext.declare.extend === ext.declare.name)
                        break;
                    ext = ext.declare.extend;
                }
            }
        }
    }

    function gen_idl() {
        var _idl = ejs.compile(fs.readTextFile(path.join(__dirname, './tmpl/idl.md')));

        for (var m in defs) {
            var p = path.join(docsFolder, defs[m].declare.type == 'module' ? "module" : "object", "ifs", m + ".md");
            var md = _idl(defs[m]);
            md = md.replace(/\n\n+/g, '\n\n');
            fs.writeFile(p, md);
        }
    }

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
    fs.mkdir(path.join(docsFolder, 'module', 'ifs'));
    fs.mkdir(path.join(docsFolder, 'object'));
    fs.mkdir(path.join(docsFolder, 'object', 'ifs'));

    // console.log(defs['Buffer']);

    inherit_method();

    gen_summary();
    gen_readme();
    gen_idl();
}