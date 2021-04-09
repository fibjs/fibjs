var fs = require("fs");
var util = require("util");
var path = require('path');
var ejs = require('ejs');
var beautify = require('js-beautify');
var cloneDeep = require('clone-deep');

global.cwrap = 0;

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

    function add_types() {
        for (var n in defs) {
            var def = defs[n];

            def.members.forEach(m => {
                if (m.params) {
                    for (var i = 0; i < m.params.length && i < m.doc.params.length; i++) {
                        if (m.params[i].type && m.params[i].name === m.doc.params[i].name) {
                            m.doc.params[i].descript = m.params[i].type + ", " + m.doc.params[i].descript;
                        }
                    }
                }

                if (Array.isArray(m.type)) {
                    var ts = '';

                    m.type.forEach(function (p) {
                        if (ts)
                            ts += ', ';

                        if (p.type)
                            ts += p.type + ' ';
                        ts += p.name;
                    });
                    m.type = '(' + ts + ')';
                }

                if (m.type) {
                    if (m.memType == 'method' && m.doc.return && m.doc.return.descript) {
                        m.doc.return.descript = m.type + ", " + m.doc.return.descript;
                    }

                    if (m.memType == 'prop' && m.doc.descript) {
                        m.doc.descript = m.type + ", " + m.doc.descript;
                    }
                }
            });
        }
    }

    function cross_link() {
        var def;
        var keyworks = {};

        function link_line(t) {
            var urls = [];
            var re = /https?:\/\/[-a-zA-Z0-9@:%._\+\/~#=]+/g;

            t = t.replace(re, function (u) {
                urls.push(u);
                return `[u_r_l::${urls.length-1}]`;
            });

            t = t.replace(/(\w+)(\.(\w+))?/g, function (k, k1, k2, k3) {
                var tidx = t.indexOf(k)
                // in code quotation
                if (tidx > 0 && t[tidx - 1] === '`') {
                    // like `fs.watch(...`
                    switch (t[tidx + k.length]) {
                        case '(':
                        case '`':
                            return k;
                        default:
                            break;
                    }
                }

                var def1 = defs[k1];
                if (keyworks[k] && def1 && def1.declare && (def1 != def || k3)) {
                    var nk = '[' + k + '](';
                    if (def1.declare.type != def.declare.type)
                        nk += (def1.declare.type === 'module' ? '../../module/ifs/' : '../../object/ifs/');

                    nk += k1 + '.md';
                    if (k3)
                        nk += '#' + k3.replace(/\s+/g, '-');

                    nk += ')';
                    return nk;
                }

                return k;
            });

            t = t.replace(/\[u_r_l::([0-9]+)\]/g, function (u, n) {
                urls.push(u);
                u = urls[new Number(n)];
                return u;
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

            if (doc.return)
                doc.return.descript = link_line(doc.return.descript);

            if (doc.params)
                doc.params.forEach(p => {
                    p.descript = link_line(p.descript);
                });
        }

        for (var n in defs) {
            keyworks[n] = true;
            defs[n].members.forEach(m => {
                keyworks[n + '.' + m.name] = true;
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

    function gen_svg() {
        function get_node(def, me, simple) {
            var txts = [];
            var lines = [];

            function member_output(name, test) {
                var first = true;
                var last_member;

                def.members.forEach(m => {
                    if (test(m, def.declare.name)) {
                        if (first) {
                            first = false;
                            txts.push(lines.join(';'));
                            lines = [];
                            txts.push('|');
                        }

                        if (last_member !== m.name) {
                            last_member = m.name;
                            if (m.name == def.declare.name) {
                                if (m.memType == 'method')
                                    lines.push('new ' + m.name + '()');
                                else
                                    lines.push(m.name);
                            } else if (m.memType == 'operator')
                                lines.push('operator' + m.name.replace(/\[|\]/g, s => `\\${s}`));
                            else
                                lines.push(m.name + (m.memType == 'method' ? '()' : ''));
                        }
                    }
                });
            }

            txts.push("[");
            txts.push(me ? '<this>' : '<class>');
            txts.push(def.declare.name);

            if (!simple) {
                member_output('构造函数', function (m, n) {
                    return m.memType == 'method' && m.name == n;
                });

                member_output('下标操作', function (m) {
                    return m.memType == 'operator';
                });

                member_output('对象', function (m) {
                    return m.memType == 'object';
                });

                member_output('静态函数', function (m, n) {
                    return m.memType == 'method' && m.name !== n && m.static;
                });

                member_output('静态属性', function (m) {
                    return m.memType == 'prop' && m.static;
                });

                member_output('常量', function (m) {
                    return m.memType == 'const';
                });

                member_output('成员属性', function (m) {
                    return m.memType == 'prop' && !m.static;
                });

                member_output('成员函数', function (m, n) {
                    return m.memType == 'method' && m.name !== n && !m.static;
                });

                txts.push(lines.join(';'));
            }

            txts.push(']');

            return txts.join('');
        }

        function get_inherits(def, nodes, arrows) {
            if (def.inherits)
                def.inherits.forEach(i => {
                    nodes.push(get_node(defs[i], false, true));
                    arrows.push(`[${def.declare.name}] <:- [${i}]`);
                    get_inherits(defs[i], nodes, arrows);
                });
        }

        function get_dot(def) {
            var n = def;
            var n1;
            var nodes = [];
            var arrows = [];

            while (n.declare.extend) {
                n1 = n;
                n = defs[n1.declare.extend];
                nodes.unshift(get_node(n));
                arrows.unshift(`[${n.declare.name}] <:- [${n1.declare.name}]`);
            }

            nodes.push(get_node(def, true));
            get_inherits(def, nodes, arrows);

            return `#lineWidth: 1.5
#font: Helvetica,sans-Serif
#fontSize: 10
#leading: 1.6
#.this: fill=lightgray
#.class: fill=white

` + nodes.join('\n') + '\n\n' + arrows.join('\n');
        }

        for (var m in defs) {
            var def = defs[m];

            if (def.declare.type == 'interface') {
                var e = def.declare.extend;
                if (e) {
                    var ext = defs[e];
                    if (!ext.inherits)
                        ext.inherits = [m];
                    else
                        ext.inherits.push(m);
                }
            }
        }

        for (var m in defs) {
            var def = defs[m];

            if (def.declare.type == 'interface')
                def.dot = get_dot(def);
        }
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
                            var m1 = cloneDeep(m);
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
            md = md.replace(/\n\s+```/g, '\n```');

            md = md.replace(/\n```JavaScript\s+((.|\n)*?)\s+```\s+/gi, function (s, p1, p2, p3) {
                return '\n\n```JavaScript\n' + beautify(p1.replace(/\n( |\t)*/g, '\n')).replace(/^static\(/g, 'static (') + '\n```\n\n';
            });

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

    check_docs();

    add_types();

    gen_summary();
    gen_readme();

    gen_svg();

    inherit_method();

    cross_link();

    gen_idl();
}