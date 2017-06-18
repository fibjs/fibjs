var fs = require("fs");
var util = require("util");
var path = require('path');
var ejs = require('ejs');

module.exports = function (defs, docsFolder) {
    function check_docs() {
        for (var n in defs) {
            var def = defs[n];

            if (!def.declare.doc.descript)
                console.error(n, 'not documented.');

            def.members.forEach(m => {
                if (!m.doc.descript)
                    console.error(n + '.' + m.name, 'not documented.');

                if (m.params) {
                    if (m.params.length !== m.doc.params.length)
                        console.error('params of', n + '.' + m.name, 'not well documented.');
                    else {
                        for (var i = 0; i < m.params.length; i++) {
                            if (m.params[i].name !== m.doc.params[i].name)
                                console.error('params', m.params[i].name, 'of', n + '.' + m.name, 'not well documented.');
                        }
                    }
                }

                if (m.type && m.memType == 'method') {
                    if (!m.doc.return)
                        console.error('return of', n + '.' + m.name, 'not well documented.');
                }
            });
        }
    }

    function cross_link() {
        var def;

        function link_line(t) {
            t = t.replace(/\w+/g, function (k) {
                var def1 = defs[k];
                if (def1 && def1.declare && def1 != def) {
                    var k1 = '[' + k + '](';
                    if (def1.declare.type != def.declare.type)
                        k1 += (def1.declare.type === 'module' ? '../../module/ifs/' : '../../object/ifs/');

                    k1 += k + '.md)';
                    return k1;
                }

                return k;
            });

            return t;
        }

        function link_doc(doc) {
            doc.descript = link_line(doc.descript);

            var in_code = false;
            doc.detail = doc.detail.map(d => {
                if (d.indexOf('```') !== -1)
                    in_code = !in_code;
                return in_code ? d : link_line(d);
            });
        }

        for (var n in defs) {
            def = defs[n];
            link_doc(def.declare.doc);
            def.members.forEach(m => {
                link_doc(m.doc);
            });

        }
    }

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
            def.defs = defs;

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

    console.log(defs['ZmqSocket']);

    clean_folder(docsFolder);

    fs.mkdir(path.join(docsFolder, 'module'));
    fs.mkdir(path.join(docsFolder, 'module', 'ifs'));
    fs.mkdir(path.join(docsFolder, 'object'));
    fs.mkdir(path.join(docsFolder, 'object', 'ifs'));

    check_docs();

    gen_summary();
    gen_readme();

    cross_link();
    inherit_method();

    gen_idl();
}