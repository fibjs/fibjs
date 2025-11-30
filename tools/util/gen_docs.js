var fs = require("fs");
var path = require('path');
var ejs = require('ejs');
var beautify = require('js-beautify');
var cloneDeep = require('clone-deep');

global.cwrap = 0;

module.exports = function (defs, docsFolder) {
    // Generate function signature for better error messages
    function getMethodSignature(className, method) {
        var sig = className + '.' + method.name + '(';
        if (method.params && method.params.length > 0) {
            sig += method.params.map(p => {
                var paramStr = '';
                if (p.type) paramStr += p.type + ' ';
                paramStr += p.name;
                if (p.isarray) paramStr += '[]';
                return paramStr;
            }).join(', ');
        }
        sig += ')';
        return sig;
    }

    function check_docs() {
        for (var n in defs) {
            var def = defs[n];

            if (!def.declare.doc.descript)
                console.error(n, 'not documented.');

            def.members.forEach(m => {
                if (!m.doc.descript)
                    console.error(n + '.' + m.name, 'not documented.');

                if (m.params) {
                    var methodSig = getMethodSignature(n, m);
                    if (m.params.length !== m.doc.params.length)
                        console.error('params of', methodSig, 'not well documented.');
                    else {
                        for (var i = 0; i < m.params.length; i++) {
                            if (m.params[i].name !== m.doc.params[i].name)
                                console.error('params', m.params[i].name, 'of', methodSig, 'not well documented.');
                        }
                    }
                }

                if (m.type && m.memType == 'method') {
                    if (!m.doc.return)
                        console.error('return of', getMethodSignature(n, m), 'not well documented.');
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
                            if (m.params[i].isarray)
                                m.doc.params[i].name += '[]';
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
                return `[u_r_l::${urls.length - 1}]`;
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
                // Also process overloads if they exist
                if (m.overs) {
                    m.overs.forEach(over => {
                        link_doc(over.doc);
                    });
                }
            });
        }
    }

    function gen_summary() {
        var _summary = ejs.compile(fs.readFileSync(path.join(__dirname, './tmpl/SUMMARY.md'), "utf8"));

        fs.writeFileSync(path.join(docsFolder, "module", "SUMMARY.md"), _summary({
            title: '基础模块',
            defs: defs,
            type: 'module'
        }));

        fs.writeFileSync(path.join(docsFolder, "object", "SUMMARY.md"), _summary({
            title: '内置对象',
            defs: defs,
            type: 'interface'
        }));
    }

    function gen_readme() {
        var _readme = ejs.compile(fs.readFileSync(path.join(__dirname, './tmpl/README.md'), "utf8"));

        fs.writeFileSync(path.join(docsFolder, "module", "README.md"), _readme({
            title: '基础模块',
            defs: defs,
            type: 'module'
        }));

        fs.writeFileSync(path.join(docsFolder, "object", "README.md"), _readme({
            title: '内置对象',
            defs: defs,
            type: 'interface'
        }));
    }

    function gen_svg() {
        function get_node(def, me, simple) {
            var txts = [];

            function member_output(name, test) {
                var first = true;
                var last_member;

                def.members.forEach(m => {
                    if (test(m, def.declare.name)) {
                        if (first) {
                            first = false;
                            txts.push('|');
                        }

                        if (last_member !== m.name) {
                            last_member = m.name;
                            if (m.name == def.declare.name) {
                                if (m.memType == 'method')
                                    txts.push('new ' + m.name + '()\\l');
                                else
                                    txts.push(m.name + '\\l');
                            } else if (m.memType == 'operator')
                                txts.push('operator' + m.name + '\\l');
                            else if (m.memType == 'method')
                                txts.push(m.name + '()\\l');
                            else if (m.memType == 'event')
                                txts.push('event ' + m.name + '\\l');
                            else
                                txts.push(m.name + '\\l');
                        }
                    }
                });
            }

            txts.push("    " + def.declare.name);
            txts.push(' [tooltip="' + def.declare.name + '"' + (me ? ', fillcolor="lightgray"' : '') + ', ');
            if (!me)
                txts.push('URL="' + def.declare.name + '.md", ');
            else
                txts.push('id="me", ');
            txts.push('label="{');
            txts.push(def.declare.name);

            if (!simple) {
                member_output('构造函数', function (m, n) {
                    return m.memType == 'method' && m.name == n;
                });

                member_output('操作符', function (m) {
                    return m.memType == 'operator' || m.symbol;
                });

                member_output('对象', function (m) {
                    return m.memType == 'object';
                });

                member_output('静态函数', function (m, n) {
                    return m.memType == 'method' && m.name !== n && m.static && !m.symbol;
                });

                member_output('静态属性', function (m) {
                    return m.memType == 'prop' && m.static && !m.symbol;
                });

                member_output('常量', function (m) {
                    return m.memType == 'const';
                });

                member_output('成员属性', function (m) {
                    return m.memType == 'prop' && !m.static && !m.symbol;
                });

                member_output('成员函数', function (m, n) {
                    return m.memType == 'method' && m.name !== n && !m.static && !m.symbol;
                });

                member_output('事件', function (m) {
                    return m.memType == 'event';
                });

            }

            txts.push('}"];');

            return txts.join('');
        }

        function get_inherits(def, nodes, arrows) {
            if (def.inherits)
                def.inherits.forEach(i => {
                    nodes.push(get_node(defs[i], false, true));
                    arrows.push("    " + def.declare.name + " -> " + i + ' [dir=back];');
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
                arrows.unshift("    " + n.declare.name + " -> " + n1.declare.name + ' [dir=back];');
            }

            nodes.push(get_node(def, true));
            get_inherits(def, nodes, arrows);

            return 'digraph {\n    node [fontname="Helvetica,sans-Serif", fontsize=10, shape="record", style="filled", fillcolor="white"];\n\n' +
                nodes.join('\n') + '\n\n' + arrows.join('\n') + '\n}';
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

            if (def.declare.type == 'interface') {
                def.dot = get_dot(def);
            }
        }
    }

    function inherit_method() {
        for (var m in defs) {
            var def = defs[m];
            def.defs = defs;

            if (def.declare.type === 'interface') {
                // Create a set of member names that are already defined in the current class
                var ownMembers = new Set();
                def.members.forEach(member => {
                    if (member.name !== def.declare.name) {
                        ownMembers.add(member.name);
                    }
                });

                var ext = def.declare.extend;
                while (ext) {
                    ext = defs[ext];
                    ext.members.forEach(m => {
                        if (m.memType != 'operator' &&
                            m.name !== ext.declare.name &&
                            !m.inherit &&
                            !ownMembers.has(m.name)) { // Only inherit if not overridden
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

    function union_method_for_docs() {
        for (var m in defs) {
            var def = defs[m];

            if (def.declare.type === 'interface') {
                var method_defs = {};
                var deflist = [];
                var overriddenMethods = new Set();

                // First pass: identify which methods are overridden in this class
                def.members.forEach(fn => {
                    if (fn.memType === 'method' && fn.name !== def.declare.name) {
                        overriddenMethods.add(fn.name);
                    }
                });

                // Second pass: collect inherited method overloads for overridden methods
                if (def.declare.extend && def.declare.extend !== 'object' && defs[def.declare.extend]) {
                    var parentDef = defs[def.declare.extend];

                    if (parentDef.members) {
                        parentDef.members.forEach(fn => {
                            if (fn.memType === "method" &&
                                fn.name !== parentDef.declare.name &&
                                overriddenMethods.has(fn.name)) {

                                // Create unique key that includes static/instance distinction
                                var fname = fn.name + (fn.static ? ':static' : ':instance');

                                // Check if parent method has already been processed and has overs
                                var parentOverloads = fn.overs || [fn];

                                // Add all parent overloads
                                parentOverloads.forEach(parentOverload => {
                                    if (!method_defs.hasOwnProperty(fname)) {
                                        var fn1 = JSON.parse(JSON.stringify(parentOverload));
                                        fn1.overs = [parentOverload];
                                        method_defs[fname] = fn1;
                                    } else {
                                        method_defs[fname].overs.push(parentOverload);
                                    }
                                });
                            }
                        });
                    }
                }

                // Third pass: process current class methods (including same-class overloads)
                def.members.forEach(fn => {
                    if (fn.memType === 'method' && fn.name !== def.declare.name) {
                        // Create unique key that includes static/instance distinction
                        var fname = fn.name + (fn.static ? ':static' : ':instance');

                        if (method_defs.hasOwnProperty(fname)) {
                            // Method already exists (either inherited or from previous overload), add to overs
                            method_defs[fname].overs.push(fn);
                        } else {
                            // First time seeing this method name, create new entry
                            var fn1 = JSON.parse(JSON.stringify(fn));
                            fn1.overs = [fn];
                            method_defs[fname] = fn1;
                        }
                    } else {
                        // This is not a method or is constructor, add directly
                        deflist.push(fn);
                    }
                });

                // Add merged methods to deflist
                for (var fname in method_defs) {
                    deflist.push(method_defs[fname]);
                }

                def.members = deflist;
            }
        }
    }

    function gen_idl() {
        var _idl = ejs.compile(fs.readFileSync(path.join(__dirname, './tmpl/idl.md'), "utf8"));

        for (var m in defs) {
            var p = path.join(docsFolder, defs[m].declare.type == 'module' ? "module" : "object", "ifs", m + ".md");
            var md = _idl(defs[m]);
            md = md.replace(/\n\s+```/g, '\n```');

            md = md.replace(/\n```JavaScript\s+((.|\n)*?)\s+```\s+/gi, function (s, p1, p2, p3) {
                return '\n\n```JavaScript\n' + beautify(p1.replace(/\n( |\t)*/g, '\n')).replace(/^static\(/g, 'static (') + '\n```\n\n';
            });

            md = md.replace(/\n\n+/g, '\n\n');

            fs.writeFileSync(p, md);
        }
    }

    function clean_folder(p) {
        var dir = fs.readdirSync(p);
        console.log("clean", p);
        dir.forEach(function (name) {
            var fname = path.join(p, name);
            var f = fs.statSync(fname);
            if (f.isDirectory()) {
                clean_folder(fname);
                fs.rmdirSync(fname);
            } else
                fs.unlinkSync(fname);
        });
    }

    clean_folder(docsFolder);

    fs.mkdirSync(path.join(docsFolder, 'module'));
    fs.mkdirSync(path.join(docsFolder, 'module', 'ifs'));
    fs.mkdirSync(path.join(docsFolder, 'object'));
    fs.mkdirSync(path.join(docsFolder, 'object', 'ifs'));

    console.log('   🔍 Checking documentation completeness...');
    check_docs();

    console.log('   🏷️ Adding type information...');
    add_types();

    console.log('   📋 Generating files...');
    gen_summary();
    gen_readme();
    gen_svg();
    union_method_for_docs();
    inherit_method();
    cross_link();
    gen_idl();
}