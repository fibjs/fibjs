#!/usr/bin/env fibjs
/*
 * check_union_dispatch.js
 *
 * Audits the C++ dispatch of the parameter-position unions (`A|B`) declared in
 * idl/*.idl -- the checks that cannot live in idlc because they read the
 * generated headers and the sources.
 *
 * Data sources:
 *   out/idljson/<interface|module>/<Def>.json   the IR recorded by idlc
 *   fibjs/include/ifs/*.h                       `using Union_<member>_<param> = std::variant<...>;`
 *   fibjs/src/**                                the implementations
 *   npm/types/dts/**                            the generated TypeScript definitions
 *
 * Verdicts:
 *   !!  a real risk
 *         - `std::get<X>(p)` without a guard while another alternative is left
 *           over: that value falls into the else branch and the get reads the
 *           wrong alternative (undefined behaviour, the deriveBits class of bug)
 *         - `ac->ctx(n)` beyond the smallest `m_ctx.resize()` of the function
 *           (the async phase reads a slot no branch wrote)
 *         - a v8 handle read in an async member whose entry and forwarded
 *           helper both lack the sync-phase guard
 *   ??  needs a human look (delegation, named constants, no user in src)
 *
 * Usage:
 *   fibjs tools/check_union_dispatch.js [--json] [--quiet]
 *
 * Exit code: 1 when a `!!` finding was reported.
 */
var fs = require('fs');
var path = require('path');

var root = path.resolve(__dirname, '..');
var V8_ALT = /v8::Local</;

function readText(p) {
    try {
        return fs.readFile(p).toString();
    } catch (e) {
        return null;
    }
}

function walk(dir, out, filter) {
    var names;
    try {
        names = fs.readdir(dir);
    } catch (e) {
        return out;
    }

    names.forEach(function (n) {
        var p = path.join(dir, n);
        var st = fs.stat(p);
        if (st.isDirectory())
            walk(p, out, filter);
        else if (!filter || filter(p))
            out.push(p);
    });
    return out;
}

function jsonFiles(dir, out) {
    fs.readdir(dir).forEach(function (n) {
        var sub = path.join(dir, n);
        var st = fs.stat(sub);
        if (st.isDirectory())
            jsonFiles(sub, out);
        else if (n.slice(-5) === '.json')
            out.push(sub);
    });
    return out;
}

// ---- the declarative side: the IR of every union parameter -----------------
function loadIR() {
    var out = [];
    jsonFiles(path.join(root, 'out/idljson'), []).forEach(function (f) {
        var text = readText(f);
        if (!text)
            return;

        var d;
        try {
            d = JSON.parse(text);
        } catch (e) {
            return;
        }

        var defName = (d.declare || {}).name;
        (d.members || []).forEach(function (m) {
            if (m.memType !== 'method')
                return;

            (m.params || []).forEach(function (p) {
                if (typeof p.type !== 'string' || p.type.indexOf('|') < 0)
                    return;

                var dv = p.default;
                out.push({
                    def: defName,
                    member: m.name,
                    param: p.name,
                    type: p.type,
                    array: !!p.isarray,
                    isAsync: !!m.async,
                    defaultText: dv ? String(dv.value !== undefined ? dv.value : dv.const) : null,
                    file: path.basename(f),
                });
            });
        });
    });
    return out;
}

// ---- the generated side: `using Union_x_y = std::variant<...>;` -------------
function splitAlts(body) {
    var parts = [];
    var depth = 0;
    var cur = '';

    for (var i = 0; i < body.length; i++) {
        var ch = body[i];
        if (ch === '<')
            depth++;
        else if (ch === '>')
            depth--;

        if (ch === ',' && depth === 0) {
            parts.push(cur.trim());
            cur = '';
            continue;
        }
        cur += ch;
    }
    if (cur.trim())
        parts.push(cur.trim());

    return parts.filter(function (x) { return x.length > 0; });
}

function loadAliases() {
    var out = {};
    walk(path.join(root, 'fibjs/include/ifs'), [], function (p) { return p.slice(-2) === '.h'; })
        .forEach(function (f) {
            var text = readText(f);
            if (!text)
                return;

            var re = /using\s+(Union_\w+)\s*=\s*std::variant<([\s\S]*?)>;/g;
            var m;
            while ((m = re.exec(text)) !== null) {
                // the alias name only carries the member and the parameter, so the
                // same name may be declared by several classes: keep the owner
                var owner = null;
                var head = text.slice(0, m.index);
                var cm = /class\s+(\w+)\s*(?::[^{]*)?\{[^{}]*$/.exec(head);
                if (cm)
                    owner = cm[1].replace(/_base$/, '');

                var entry = { alts: splitAlts(m[2]), header: path.basename(f), owner: owner };
                if (!out[m[1]])
                    out[m[1]] = entry;
                if (owner) {
                    if (!out[m[1] + '@' + owner])
                        out[m[1] + '@' + owner] = entry;
                }
            }
        });
    return out;
}

// ---- source helpers -------------------------------------------------------
function balanced(text, i) {
    var depth = 0;
    for (var j = i; j < text.length; j++) {
        if (text[j] === '<')
            depth++;
        else if (text[j] === '>') {
            depth--;
            if (depth === 0)
                return { inner: text.slice(i + 1, j), next: j + 1 };
        }
    }
    return null;
}

function callsWithParam(text, fname, param) {
    var found = {};
    var re = new RegExp(fname.replace('::', '::') + '\\s*<', 'g');
    var m;

    while ((m = re.exec(text)) !== null) {
        var b = balanced(text, m.index + m[0].length - 1);
        if (!b)
            continue;

        var rest = text.slice(b.next, b.next + 120);
        if (new RegExp('^\\s*\\(\\s*' + param + '\\s*[),]').test(rest))
            found[b.inner.trim().replace(/\s+/g, '')] = true;
    }
    return found;
}

function functionsMentioning(text, needle) {
    var out = [];
    var pos = text.indexOf(needle);

    while (pos >= 0) {
        var i = pos + needle.length;
        while (i < text.length && text[i] !== '{' && text[i] !== ';')
            i++;

        if (i < text.length && text[i] !== ';') {
            var depth = 0;
            var j = i;
            for (; j < text.length; j++) {
                if (text[j] === '{')
                    depth++;
                else if (text[j] === '}') {
                    depth--;
                    if (depth === 0)
                        break;
                }
            }
            out.push({ body: text.slice(i, j), line: text.slice(0, pos).split('\n').length });
        }
        pos = text.indexOf(needle, pos + needle.length);
    }
    return out;
}

function guarded(text) {
    return text.indexOf('isSync()') >= 0 || text.indexOf('ac->ctx(') >= 0;
}

function main() {
    var args = {};
    process.argv.slice(2).forEach(function (a) {
        if (a === '--json')
            args.json = true;
        else if (a === '--quiet')
            args.quiet = true;
    });

    var ir = loadIR();
    var aliases = loadAliases();
    var aliasParam = {};
    ir.forEach(function (p) {
        aliasParam['Union_' + p.member + '_' + p.param] = p.param;
        // the generator also emits a `..._load` alias (without the self class) for
        // the constructors that reference their own type; it is optional, so it is
        // not required here
    });

    var sources = walk(path.join(root, 'fibjs/src'), [], function (p) {
        return p.slice(-4) === '.cpp' || p.slice(-2) === '.h';
    });
    var texts = {};
    sources.forEach(function (p) { texts[p] = readText(p); });

    var findings = [];

    // ---- 1. every union parameter has its generated alias ----
    var missingAlias = Object.keys(aliasParam).filter(function (a) { return !aliases[a]; });
    missingAlias.forEach(function (a) {
        findings.push({ level: '!!', what: 'alias', id: a, where: '-', detail: 'the IR declares this union but no alias was generated' });
    });

    // ---- 2. dispatch safety: no unguarded std::get ----
    Object.keys(aliases).forEach(function (alias) {
        if (alias.indexOf('@') >= 0)
            return;
        var alts = aliases[alias].alts;
        var param = aliasParam[alias];
        if (!param)
            return;

        var norm = function (x) { return x.replace(/\s+/g, ''); };

        Object.keys(texts).forEach(function (file) {
            var text = texts[file];
            if (text.indexOf(alias) < 0)
                return;

            // the alias may be declared by another class with other alternatives:
            // resolve it through the class of the function being analysed
            var owner = null;
            var om = /(\w+)::\w+\s*\([^)]*$/.exec(text.slice(0, text.indexOf(alias)));
            if (om)
                owner = om[1].replace(/_base$/, '');

            var def = (owner && aliases[alias + '@' + owner]) || aliases[alias];
            var fnAlts = def.alts;
            var altsN = fnAlts.map(norm);

            functionsMentioning(text, alias).forEach(function (fn) {
                var holds = callsWithParam(fn.body, 'holds_alternative', param);
                var gets = callsWithParam(fn.body, 'std::get', param);
                var visit = /std::visit\s*\(/.test(fn.body);

                if (visit)
                    return;                        // a visitor covers every alternative

                var holdsList = Object.keys(holds);
                var unsafeGets = Object.keys(gets).filter(function (g) {
                    if (holds[g])
                        return false;

                    // an implicit else: safe only when every alternative is
                    // either explicitly held or is exactly this one
                    return altsN.some(function (a) { return holdsList.indexOf(a) < 0 && a !== g; });
                });

                if (unsafeGets.length)
                    findings.push({
                        level: '!!', what: 'dispatch', id: alias,
                        where: path.relative(root, file) + ':' + fn.line,
                        detail: 'unguarded std::get ' + JSON.stringify(unsafeGets) +
                            ' (guarded: ' + JSON.stringify(holdsList) + ', alternatives: ' + JSON.stringify(altsN) + ')',
                    });
                else if (holdsList.length + Object.keys(gets).length === 0) {
                    findings.push({
                        level: '??', what: 'dispatch', id: alias,
                        where: path.relative(root, file) + ':' + fn.line,
                        detail: 'no direct holds_alternative/std::get for ' + param + ' (delegated to a helper)',
                    });
                }
            });
        });
    });

    // ---- 3. ac->ctx(n) against the smallest m_ctx.resize() ----
    Object.keys(texts).forEach(function (file) {
        var text = texts[file];
        if (text.indexOf('ac->ctx(') < 0)
            return;

        functionsMentioning(text, 'ac->ctx(').forEach(function (fn) {
            var reads = [];
            var re = /ac->ctx\((\d+)\)/g;
            var m;
            while ((m = re.exec(fn.body)) !== null)
                reads.push(parseInt(m[1], 10));

            var resizes = [];
            re = /m_ctx\.resize\((\d+)\)/g;
            while ((m = re.exec(fn.body)) !== null)
                resizes.push(parseInt(m[1], 10));

            if (!reads.length || !resizes.length)
                return;

            var min = Math.min.apply(null, resizes);
            var bad = reads.filter(function (n) { return n >= min; });
            if (bad.length)
                findings.push({
                    level: '!!', what: 'ctx', id: path.relative(root, file) + ':' + fn.line,
                    where: path.relative(root, file) + ':' + fn.line,
                    detail: 'ac->ctx(' + JSON.stringify(bad) + ') beyond the smallest m_ctx.resize(' + min + ')',
                });
        });
    });

    // ---- 4. async members with a v8 alternative need a sync-phase guard ----
    var asyncAliases = {};
    ir.forEach(function (p) {
        if (p.isAsync)
            asyncAliases['Union_' + p.member + '_' + p.param] = p.param;
    });

    Object.keys(asyncAliases).forEach(function (alias) {
        var param = asyncAliases[alias];
        var alts = (aliases[alias] || { alts: [] }).alts;
        if (!alts.some(function (a) { return V8_ALT.test(a); }))
            return;

        var checked = false;
        Object.keys(texts).forEach(function (file) {
            var text = texts[file];
            if (checked || text.indexOf(alias) < 0)
                return;

            functionsMentioning(text, alias).forEach(function (fn) {
                if (checked || fn.body.indexOf('ac') < 0)
                    return;

                if (guarded(fn.body))
                    checked = true;
                else {
                    var m = /return\s+(\w+)\s*\(/.exec(fn.body);
                    var callee = m && m[1];
                    var cm = callee && new RegExp('result_t\\s+' + callee + '\\s*\\(').exec(text);
                    if (cm && guarded(text.slice(cm.index, cm.index + 4000)))
                        checked = true;
                    else {
                        checked = true;
                        findings.push({
                            level: '!!', what: 'phase', id: alias,
                            where: path.relative(root, file) + ':' + fn.line,
                            detail: 'a v8 alternative in an async member, and neither the entry nor the helper it forwards to guards the sync phase',
                        });
                    }
                }
            });
        });
    });

    // ---- report ----
    var serious = findings.filter(function (f) { return f.level === '!!'; });
    var uniqueAliases = Object.keys(aliases).filter(function (a) { return a.indexOf('@') < 0; }).length;
    if (args.json) {
        console.log(JSON.stringify({ unionParams: ir.length, aliases: uniqueAliases, findings: findings }, null, 2));
    } else {
        console.log('union 参数 %d 个，别名 %d 个，源码文件 %d 个',
            ir.length, uniqueAliases, Object.keys(texts).length);
        if (!findings.length)
            console.log('✅ No union dispatch problems');
        else {
            if (!args.quiet)
                findings.forEach(function (f) {
                    console.log('   %s %s %s\n      %s', f.level, f.what,
                        f.where === '-' ? f.id : f.where, f.detail);
                });
            console.log('%s %d finding(s), %d serious', serious.length ? '❌' : '⚠️', findings.length, serious.length);
        }
    }

    return serious.length ? 1 : 0;
}

process.exit(main());
