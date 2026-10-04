#!/usr/bin/env fibjs
/*
 * check_idl_overloads.js
 *
 * Audits the string-acceptance surface of `Buffer` parameters in idl/*.idl.
 *
 * Rule (plans/buffer-types-refactor-2026-10-01.md §2.2 / §2.2b):
 *   A member that declares a `Buffer` parameter must also declare an overload
 *   with a `String` parameter at the same position, because the runtime no
 *   longer converts strings implicitly - `GetArgumentValue(Buffer)` lost its
 *   `encoding` parameter and the handler-level special case
 *   (`GetArgumentValue(..., false, encoding.c_str())`) is gone. The declared
 *   overload is the successor: its implementation decodes the string (utf8 by
 *   default, or the encoding the member itself declares for that parameter).
 *
 * Verdicts, per (member, Buffer position):
 *   ok       a declared `String` overload covers the call
 *   gap      no overload declares `String` at that position
 *   partial  a `String` overload exists there but its other parameters / arity
 *            differ, so it does not cover the call
 *   review   array form (`Buffer x[]`): decide `String[]` vs single `String`
 *   n/a      events carry the runtime payload type and take no dispatch
 *
 * Usage:
 *   fibjs tools/check_idl_overloads.js [options]
 *     --json              print the full result as JSON
 *     --markdown [FILE]   also write the audit table (default: the plan's
 *                         plans/buffer-param-audit-2026-10-01.md)
 *     --file SUBSTR       only members declared in matching idl files
 *     --quiet             summary only
 *     --fail-on-gaps      exit 1 when gap/partial rows exist (CI gate; off by
 *                         default while the migration is in flight)
 *     --write             FIX: insert the missing `String` overloads into the
 *                         IDL and regenerate idl/*.idl in place (the corpus is
 *                         canonical, so untouched files are not rewritten)
 *     --fix-comments      FIX: copy the function / parameter comments of the
 *                         Buffer declaration onto the String overload
 *
 * The native side is *not* generated: every `String` overload is implemented by
 * hand, per member, with the shape that member needs (see
 * plans/buffer-types-refactor-2026-10-01.md 2.2e).
 */

var fs = require('fs');
var path = require('path');

var parseIDL = require('./util/parser');

var IDL_FOLDER = path.resolve(__dirname, '../idl');
var IFS_FOLDER = path.resolve(__dirname, '../fibjs/include/ifs');
var AUDIT_DOC = path.resolve(__dirname, '../plans/buffer-param-audit-2026-10-01.md');

function parseArgs(argv) {
    var opts = { json: false, markdown: null, file: null, quiet: false, failOnGaps: false, write: false, fixComments: false };

    for (var i = 0; i < argv.length; i++) {
        var a = argv[i];
        if (a === '--json')
            opts.json = true;
        else if (a === '--markdown')
            opts.markdown = (argv[i + 1] && argv[i + 1][0] !== '-') ? argv[++i] : AUDIT_DOC;
        else if (a === '--file')
            opts.file = argv[++i];
        else if (a === '--quiet')
            opts.quiet = true;
        else if (a === '--fail-on-gaps')
            opts.failOnGaps = true;
        else if (a === '--write')
            opts.write = true;
        else if (a === '--reorder')
            opts.reorder = true;
        else if (a === '--fix-comments')
            opts.fixComments = true;
        else if (a === '--help' || a === '-h')
            opts.help = true;
        else
            throw new Error('unknown option: ' + a);
    }

    return opts;
}

/* interface/module name -> idl file name (declares are not always 1:1 named) */
function fileIndex() {
    var map = {};

    fs.readdirSync(IDL_FOLDER).sort().forEach(function (f) {
        if (path.extname(f) !== '.idl')
            return;

        var text = fs.readFileSync(path.join(IDL_FOLDER, f), 'utf8');
        var m = /^[ \t]*(?:interface|module)[ \t]+([A-Za-z_][A-Za-z0-9_]*)/m.exec(text);
        if (m)
            map[m[1]] = f;
    });

    return map;
}

function typeText(p) {
    return p.type + (p.isarray ? '[]' : '');
}

/* the IR keeps the C++ expression for `{}` / `[]` defaults */
function defaultText(p) {
    if (!p.default)
        return '';

    var v = p.default.value;

    if (v === 'v8::Object::New(isolate->m_isolate)')
        return '{}';

    if (v === 'v8::Array::New(isolate->m_isolate)')
        return '[]';

    return v;
}

function paramText(p) {
    var d = defaultText(p);

    return typeText(p) + ' ' + p.name + (d ? ' = ' + d : '');
}

/* `[static ]<return> name(<params>)[ async];` - constructors have no return */
function signature(m) {
    var ret = m.type ? typeText(m) + ' ' : '';
    var stat = m.static ? 'static ' : '';
    var asy = m.async ? ' async' : '';

    return stat + ret + m.name + '(' + (m.params || []).map(paramText).join(', ') + ')' + asy + ';';
}

function requiredCount(m) {
    var n = 0;

    (m.params || []).forEach(function (p) {
        if (!p.default)
            n++;
    });

    return n;
}

/*
 * Does `o` cover every call `ov` accepts, with argument `i` given as a string?
 * Returns { ok: true } or { ok: false, why: ... }.
 */
function cover(o, ov, i) {
    var op = o.params || [];
    var ovp = ov.params || [];

    if (!op[i] || op[i].type !== 'String')
        return null;

    for (var j = 0; j < ovp.length; j++) {
        if (j === i)
            continue;

        var other = op[j];
        if (!other || other.type !== ovp[j].type)
            return {
                ok: false,
                why: 'parameter ' + (j + 1) + ' is ' +
                    (other ? typeText(other) : 'missing') + ', expected ' + typeText(ovp[j])
            };
    }

    for (var k = ovp.length; k < op.length; k++) {
        if (!op[k].default)
            return { ok: false, why: 'extra required parameter `' + paramText(op[k]) + '`' };
    }

    if (requiredCount(o) > requiredCount(ov))
        return {
            ok: false,
            why: 'requires ' + requiredCount(o) + ' arguments, the Buffer overload accepts ' +
                requiredCount(ov)
        };

    return { ok: true };
}

/*
 * Params that are deliberately out of the String-overload worklist.
 *
 * sink   - a destination: the bytes never reach the caller, so the old
 *          acceptance was unobservable (Node requires a Uint8Array too).
 * shadow - the member is shadowed by the node-compat JS Buffer class
 *          (fibjs/scripts/internal/buffer.js, a Uint8Array subclass): JS
 *          callers never reach the IDL declaration.
 */
var NA_PARAMS = {
    'FileHandle.idl|FileHandle.read|buffer': 'sink',
    'fs.idl|fs.read|buffer': 'sink',
    'TextEncoder.idl|TextEncoder.encodeInto|destination': 'sink',
    'Buffer.idl|Buffer.copy|targetBuffer': 'shadow',
    'Buffer.idl|Buffer.compare|buf': 'shadow',
    'Buffer.idl|Buffer.compare|buf1': 'shadow',
    'Buffer.idl|Buffer.compare|buf2': 'shadow',
    'Buffer.idl|Buffer.set|src': 'shadow'
};

var NA_REASON = {
    sink: '目标缓冲区（输出型）：写入临时缓冲后即丢弃，传入字符串无可观察语义',
    shadow: '被 node 兼容 JS Buffer 类覆盖（Uint8Array 子类），JS 调用不经 IDL 声明'
};

function audit(defs, fileMap) {
    var rows = [];
    var events = [];

    Object.keys(defs).forEach(function (name) {
        var def = defs[name];
        var file = fileMap[name] || (name + '.idl');
        var groups = {};

        def.members.forEach(function (m) {
            m.params = m.params || [];

            if (m.memType === 'event') {
                m.params.forEach(function (p) {
                    if (p.type === 'Buffer')
                        events.push({
                            file: file,
                            def: name,
                            member: m.name,
                            param: p.name,
                            signature: 'event ' + m.name + '(' + m.params.map(paramText).join(', ') + ');'
                        });
                });
                return;
            }

            if (m.memType !== 'method')
                return;

            var key = (m.static ? 'static:' : 'instance:') + m.name;
            (groups[key] = groups[key] || []).push(m);
        });

        Object.keys(groups).forEach(function (key) {
            var ovs = groups[key];

            ovs.forEach(function (ov) {
                ov.params.forEach(function (p, i) {
                    // `Buffer|String` union slots carry every accepted type
                    // already, so only bare `Buffer` parameters need the
                    // partner-overload audit (plans/idl-union-types-2026-10-02.md).
                    if (p.type !== 'Buffer')
                        return;

                    var verdict = 'gap';
                    var detail = '';
                    var partial = null;
                    var coveredBy = null;

                    ovs.forEach(function (other) {
                        if (other === ov || verdict === 'ok')
                            return;

                        var r = cover(other, ov, i);
                        if (!r)
                            return;

                        if (r.ok) {
                            verdict = 'ok';
                            detail = signature(other);
                            coveredBy = other;
                        } else if (!partial) {
                            partial = r;
                        }
                    });

                    if (verdict === 'gap' && partial) {
                        verdict = 'partial';
                        detail = partial.why;
                    }

                    if (verdict !== 'ok' && p.isarray)
                        verdict = 'review';

                    var na = NA_PARAMS[file + '|' + name + '.' + ov.name + '|' + p.name];
                    if (verdict !== 'ok' && na) {
                        verdict = 'na';
                        detail = NA_REASON[na];
                    }

                    rows.push({
                        file: file,
                        def: name,
                        member: ov.name,
                        position: i + 1,
                        param: p.name,
                        signature: signature(ov),
                        verdict: verdict,
                        detail: detail,
                        ref: ov,
                        coveredBy: coveredBy
                    });
                });
            });
        });
    });

    rows.sort(function (a, b) {
        return a.file.localeCompare(b.file) || a.member.localeCompare(b.member) ||
            a.position - b.position || a.signature.localeCompare(b.signature);
    });

    return { rows: rows, events: events };
}

var VERDICT_LABEL = {
    ok: '已覆盖',
    gap: '补',
    partial: '复核',
    review: '待裁决',
    na: 'n/a'
};

/* display width (CJK counts as two columns) - console.log has no %-9s in fibjs */
function width(text) {
    var w = 0;

    for (var i = 0; i < text.length; i++)
        w += text.charCodeAt(i) > 0x2e80 ? 2 : 1;

    return w;
}

function pad(text, columns, right) {
    text = String(text);

    var blanks = '';
    var n = columns - width(text);
    while (blanks.length < n)
        blanks += ' ';

    return right ? blanks + text : text + blanks;
}

function summarize(rows) {
    var s = { ok: 0, gap: 0, partial: 0, review: 0, na: 0 };

    rows.forEach(function (r) {
        s[r.verdict]++;
    });

    return s;
}

function printReport(result, opts) {
    var rows = result.rows;
    var s = summarize(rows);
    var members = {};

    rows.forEach(function (r) {
        members[r.file + '|' + r.member] = true;
    });

    console.log('# Buffer 参数 —— IDL 重载检查');
    console.log('  Buffer 参数位置 ' + rows.length + ' 处，涉及 ' + Object.keys(members).length +
        ' 个成员；事件参数 ' + result.events.length + ' 处（n/a）');
    console.log('  已覆盖 ' + s.ok + ' · 缺重载 ' + s.gap + ' · 需复核 ' + s.partial +
        ' · 待裁决(数组) ' + s.review + ' · n/a ' + s.na);

    if (opts.quiet)
        return;

    var currentFile = null;

    rows.forEach(function (r) {
        if (r.verdict === 'ok')
            return;

        if (r.file !== currentFile) {
            currentFile = r.file;
            console.log('');
            console.log(currentFile);
        }

        console.log('  ' + pad('[' + VERDICT_LABEL[r.verdict] + ']', 10) + pad(r.member, 30) +
            pad(r.position, 4, true) + ' ' + pad(r.param, 18) + ' ' + r.signature);

        if (r.detail)
            console.log('            ' + r.detail);
    });
}

function auditMarkdown(result, generated) {
    var rows = result.rows;
    var s = summarize(rows);
    var out = [];

    out.push('# Buffer 参数全量审计（' + generated + '）');
    out.push('');
    out.push('> 由 `fibjs tools/check_idl_overloads.js --markdown` 从 `idl/*.idl` 生成，可随时重跑；');
    out.push('> 判据见 `plans/buffer-types-refactor-2026-10-01.md` §2.2/§2.2b：Buffer 参数要么有同位置的');
    out.push('> 显式 `String` 重载（实现按 utf8 解码），要么运行期报 TYPEMISMATCH —— 隐式转换与');
    out.push('> handler 内 `GetArgumentValue(..., encoding)` 的特例都已废除。');
    out.push('>');
    out.push('> **老二进制（隐式转换）只作接受性对照**（确认旧实现不把该调用当类型错误），');
    out.push('> **不把旧 utf8 解码语义当契约**。');
    out.push('');
    out.push('| 状态 | 数量 | 含义 |');
    out.push('| --- | --- | --- |');
    out.push('| ✅ | ' + s.ok + ' | 已有同位置 `String` 重载 |');
    out.push('| 补 | ' + s.gap + ' | 缺 `String` 重载 |');
    out.push('| 复核 | ' + s.partial + ' | 有 `String` 重载但参数/arity 不匹配 |');
    out.push('| 待裁决 | ' + s.review + ' | 数组形参（`Buffer x[]`），需定 `String[]` / `String` |');
    out.push('| n/a | ' + s.na + ' | 输出型目标缓冲（sink）或被 node 兼容 JS Buffer 覆盖（shadow），不做重载 |');
    out.push('');
    out.push('| # | 文件 | 成员 | 位置 | 参数 | 签名 | 状态 |');
    out.push('| --- | --- | --- | --- | --- | --- | --- |');

    rows.forEach(function (r, i) {
        out.push('| ' + (i + 1) + ' | ' + r.file + ' | ' + r.def + '.' + r.member + ' | ' +
            r.position + ' | ' + r.param + ' | `' + r.signature + '` | ' + VERDICT_LABEL[r.verdict] + ' |');
    });

    out.push('');
    out.push('### n/a 参数（不做重载）');
    out.push('');
    out.push('| # | 文件 | 成员 | 位置 | 参数 | 原因 |');
    out.push('| --- | --- | --- | --- | --- | --- |');

    var naRows = rows.filter(function (r) {
        return r.verdict === 'na';
    });

    naRows.forEach(function (r, i) {
        out.push('| ' + (i + 1) + ' | ' + r.file + ' | ' + r.def + '.' + r.member + ' | ' +
            r.position + ' | ' + r.param + ' | ' + r.detail + ' |');
    });

    out.push('');
    out.push('### 事件参数（运行时载荷类型，不做重载）');
    out.push('');
    out.push('| # | 文件 | 成员 | 参数 | 声明 |');
    out.push('| --- | --- | --- | --- | --- |');

    result.events.forEach(function (e, i) {
        out.push('| ' + (i + 1) + ' | ' + e.file + ' | ' + e.def + '.' + e.member + ' | ' +
            e.param + ' | `' + e.signature + '` |');
    });

    out.push('');
    return out.join('\n');
}

/* ------------------------------------------------------------------ fixer */

function cloneMember(m) {
    return JSON.parse(JSON.stringify(m));
}

function stringNote(names) {
    return names.length === 1
        ? '; a string ' + names[0] + ' is encoded as utf8'
        : '; strings are encoded as utf8';
}

/* append the note to the brief, leaving the rest of the comment block intact */
function withNote(comments, note) {
    if (!comments)
        return note;

    var i = comments.indexOf('\n');

    return i < 0 ? comments + note : comments.slice(0, i) + note + comments.slice(i);
}

/* gap rows -> one entry per Buffer overload, with the missing positions */
function groupGaps(rows) {
    var groups = [];
    var index = {};

    rows.forEach(function (r) {
        if (r.verdict !== 'gap')
            return;

        var key = r.def + '|' + r.signature;
        if (!index[key]) {
            index[key] = { def: r.def, file: r.file, signature: r.signature, ref: r.ref, positions: [] };
            groups.push(index[key]);
        }

        index[key].positions.push(r.position - 1);
    });

    return groups;
}

/*
 * The overloads to add for one Buffer declaration: one per missing position
 * (String there, Buffer elsewhere) plus - when several positions are missing -
 * one with all of them as String, because "all strings" is what callers
 * actually write (`pbkdf2('pwd', 'salt', ...)`). Calls mixing strings and
 * buffers in a longer list stay uncovered on purpose.
 */
function variantPlan(positions) {
    var variants = positions.map(function (pos) {
        return [pos];
    });

    if (positions.length > 1)
        variants.push(positions.slice());

    return variants;
}

/*
 * Insert the missing overloads into the IR and write idl/*.idl back through the
 * generator (which only rewrites files whose normalized text changed).
 */
function applyAdds(defs, rows) {
    var added = [];

    groupGaps(rows).forEach(function (g) {
        var def = defs[g.def];
        if (!def)
            return;

        var members = def.members;
        var at = members.indexOf(g.ref);
        if (at < 0)
            return;

        var known = {};
        members.forEach(function (m) {
            if (m.memType === 'method' && m.name === g.ref.name && !!m.static === !!g.ref.static)
                known[signature(m)] = true;
        });

        var slots = 0;
        variantPlan(g.positions).forEach(function (positions) {
            var clone = cloneMember(g.ref);
            var names = positions.map(function (pos) {
                return clone.params[pos].name;
            });

            positions.forEach(function (pos) {
                clone.params[pos].type = 'String';
            });

            var note = stringNote(names);
            clone.comments = withNote(clone.comments, note);
            if (clone.doc)
                clone.doc.descript = (clone.doc.descript || '') + note;

            var sig = signature(clone);
            if (known[sig])
                return;

            known[sig] = true;
            members.splice(at + 1 + slots, 0, clone);
            slots++;

            added.push({ file: g.file, def: g.def, member: clone.name, signature: sig });
        });
    });

    return added;
}

/*
 * A String overload carries the documentation of the Buffer declaration it
 * covers: the brief (plus the utf8 note) and the parameter / return comments.
 * Copy the lines the block lost - parameters are matched by name, so a block
 * that already explains the string form keeps its own wording - and drop
 * duplicated @param / @return entries.
 */
function paramName(line) {
    var m = /@param\s+(\S+)/.exec(line);

    return m ? m[1] : null;
}

function isReturn(line) {
    return /^\s*@return\b/.test(line);
}

function alignComments(dst, src) {
    if (!dst || !src)
        return null;

    var seenParam = {};
    var seenReturn = false;
    var kept = [];
    var changed = false;

    dst.split('\n').forEach(function (l) {
        var name = paramName(l);

        if (name) {
            if (seenParam[name]) {
                changed = true;
                return;
            }
            seenParam[name] = true;
        } else if (isReturn(l)) {
            if (seenReturn) {
                changed = true;
                return;
            }
            seenReturn = true;
        }

        kept.push(l);
    });

    var have = {};
    kept.forEach(function (l) {
        have[l.trim()] = true;
    });

    var missing = src.split('\n').slice(1).filter(function (l) {
        if (!l.trim())
            return false;

        var name = paramName(l);
        if (name)
            return !seenParam[name];

        if (isReturn(l))
            return !seenReturn;

        return !have[l.trim()];
    });

    if (!missing.length)
        return changed ? kept.join('\n') : null;

    var head = kept.slice();
    while (head.length && !head[head.length - 1].trim())
        head.pop();

    var indent = /^\s*/.exec(missing[0])[0];

    return (head.length ? head.join('\n') + '\n' : '') + missing.join('\n') + '\n' + indent;
}

function repairDoc(dst, src) {
    if (!dst.doc || !src.doc)
        return false;

    var changed = false;

    (src.doc.params || []).forEach(function (p) {
        var found = (dst.doc.params || []).some(function (q) {
            return q.name === p.name;
        });

        if (!found) {
            dst.doc.params = dst.doc.params || [];
            dst.doc.params.push(JSON.parse(JSON.stringify(p)));
            changed = true;
        }
    });

    if (src.doc.return && !dst.doc.return) {
        dst.doc.return = JSON.parse(JSON.stringify(src.doc.return));
        changed = true;
    }

    if ((!dst.doc.detail || !dst.doc.detail.length) && src.doc.detail && src.doc.detail.length) {
        dst.doc.detail = JSON.parse(JSON.stringify(src.doc.detail));
        changed = true;
    }

    return changed;
}

function fixComments(rows) {
    var fixed = [];
    var seen = [];

    rows.forEach(function (r) {
        if (r.verdict !== 'ok' || !r.coveredBy || !r.ref)
            return;

        if (seen.indexOf(r.coveredBy) >= 0)
            return;

        var text = alignComments(r.coveredBy.comments, r.ref.comments);
        var doc = repairDoc(r.coveredBy, r.ref);

        if (!text && !doc)
            return;

        seen.push(r.coveredBy);
        if (text)
            r.coveredBy.comments = text;

        fixed.push({
            file: r.file,
            def: r.def,
            member: r.coveredBy.name,
            signature: signature(r.coveredBy)
        });
    });

    return fixed;
}

/* ---------------------------------------- native stubs for the new overloads */

/* members ready to be inserted into the module sources */
function nativeStubs(defs, rows) {
    var stubs = [];
    var manual = [];
    var implemented = 0;
    var groups = [];
    var index = {};

    rows.forEach(function (r) {
        if (r.verdict === 'review')
            return;

        var key = r.def + '|' + r.signature;
        if (!index[key]) {
            index[key] = { def: r.def, file: r.file, signature: r.signature, ref: r.ref, positions: [] };
            groups.push(index[key]);
        }

        index[key].positions.push(r.position - 1);
    });

    groups.forEach(function (g) {
        var def = defs[g.def];
        var irParams = g.ref.params || [];
        var isInterface = def && def.declare && def.declare.type === 'interface';
        var cls = isInterface ? g.def : g.def + '_base';
        var isCtor = g.ref.name === g.def;

        /* the Buffer overload itself - the anchor a forwarder is inserted before */
        var source = isCtor ? null : variantDecl(g.def, g.ref.name, irParams, []);

        variantPlan(g.positions).forEach(function (positions) {
            var decl = isCtor ? null : variantDecl(g.def, g.ref.name, irParams, positions);

            if (!decl) {
                manual.push({
                    group: { file: g.file, def: g.def, member: g.ref.name },
                    why: isCtor ? '构造函数（native 名是 _new），需手工处理'
                                : 'IDL/生成头里没有这个 String 变体（可能由既有变体承接，或需先 --write + idlc）'
                });
                return;
            }

            if (findDefinition(cls, g.ref.name, decl.params).length > 0) {
                implemented++;
                return;
            }

            if (!source || findDefinition(cls, g.ref.name, source.params).length !== 1) {
                manual.push({
                    group: { file: g.file, def: g.def, member: g.ref.name },
                    why: '定位不到唯一的 Buffer 变体定义（插入锚点）'
                });
                return;
            }

            var head = decl.head;
            var tail = decl.params.slice(irParams.length);
            var base = localName(head);
            var buffers = positions.map(function (pos, n) {
                return positions.length === 1 ? base : base + (n + 1);
            });

            var lines = [];

            lines.push('result_t ' + cls + '::' + g.ref.name + '(' + decl.params.join(', ') + ')');
            lines.push('{');

            positions.forEach(function (pos, n) {
                if (n > 0)
                    lines.push('');
                lines.push('    obj_ptr<Buffer_base> ' + buffers[n] + ';');
                lines.push('');
                lines.push('    result_t hr' + (n > 0 ? String(n + 1) : '') + ' = Buffer_base::from(' +
                    lastName(head[pos]) + ', "utf8", ' + buffers[n] + ');');
                lines.push('    if (hr' + (n > 0 ? String(n + 1) : '') + ' < 0)');
                lines.push('        return hr' + (n > 0 ? String(n + 1) : '') + ';');
            });

            lines.push('');
            lines.push('    return ' + g.ref.name + '(' + head.map(function (p, i) {
                var n = positions.indexOf(i);

                return n < 0 ? lastName(p) : buffers[n];
            }).concat(tail.map(lastName)).join(', ') + ');');
            lines.push('}');

            stubs.push({
                file: g.file,
                def: g.def,
                member: g.ref.name,
                cls: cls,
                positions: positions,
                headerParams: decl.params,
                sourceParams: source.params,
                text: lines.join('\n')
            });
        });
    });

    return { stubs: stubs, manual: manual, implemented: implemented };
}

/* does the generated header already declare this exact signature? */
function headerDeclares(defName, params) {
    var file = path.join(IFS_FOLDER, defName + '.h');
    if (!fs.existsSync(file))
        return false;

    var needle = ('(' + params.join(', ') + ')').replace(/\s+/g, ' ');

    return fs.readFileSync(file, 'utf8').replace(/\s+/g, ' ').indexOf(needle) >= 0;
}

/* ------------------------------------------------ definition lookup (cached) */

var SOURCES = null;

function sources() {
    if (SOURCES)
        return SOURCES;

    SOURCES = [];

    (function walk(dir) {
        fs.readdirSync(dir).sort().forEach(function (f) {
            var p = path.join(dir, f);

            if (fs.statSync(p).isDirectory()) {
                walk(p);
                return;
            }

            if (path.extname(p) !== '.cpp')
                return;

            SOURCES.push({ file: p, lines: fs.readFileSync(p, 'utf8').split('\n') });
        });
    })(path.resolve(__dirname, '../fibjs/src'));

    return SOURCES;
}

/* keep the cache honest while writeCpp edits files */
function updateSource(file, lines) {
    var list = sources();

    for (var i = 0; i < list.length; i++) {
        if (list[i].file === file) {
            list[i].lines = lines;
            return;
        }
    }

    list.push({ file: file, lines: lines });
}

/* the type part of a parameter - definitions may name it differently than the
   generated declaration does */
function paramType(text) {
    return text.trim().replace(/\s*[A-Za-z_][A-Za-z0-9_]*$/, '').trim();
}

function typeList(params) {
    return params.map(paramType).join(', ');
}

function findDefinition(cls, member, params) {
    var want = typeList(params);
    var prefix = 'result_t ' + cls + '::' + member + '(';
    var hits = [];

    sources().forEach(function (src) {
        src.lines.forEach(function (line, i) {
            if (line.indexOf(prefix) < 0)
                return;

            /* the definition may wrap: collect up to the closing paren */
            var text = line;

            for (var j = i; j < Math.min(i + 10, src.lines.length) && text.indexOf(')') < 0; j++)
                text += src.lines[j + 1] || '';

            var collapsed = text.slice(text.indexOf('result_t ')).replace(/\s+/g, ' ');
            var open = collapsed.indexOf('(');
            var close = collapsed.indexOf(')');

            if (open < 0 || close < 0)
                return;

            if (typeList(collapsed.slice(open + 1, close).split(',')) === want)
                hits.push({ file: src.file, line: i, text: line.trim(), raw: line });
        });
    });

    return hits;
}

/*
 * Add the forwarders at the end of the file that defines the Buffer variant,
 * just before the closing brace of `namespace fibjs`. Appending avoids having
 * to anchor on a - possibly wrapped - definition, and the marker keeps the
 * block identifiable.
 */
var CPP_MARK = '// ---- idl String overloads: decode utf8 and forward ----';

function namespaceEnd(lines) {
    for (var i = lines.length - 1; i >= 0; i--) {
        if (lines[i] !== '}')
            continue;

        var trailing = lines.slice(i + 1).every(function (l) {
            return !l.trim() || /^\s*(\/\/|\/\*|\*)/.test(l);
        });

        if (trailing)
            return i;
    }

    return -1;
}

function writeCpp(defs, rows) {
    var plan = nativeStubs(defs, rows);
    var written = [];
    var manual = plan.manual.slice();
    var byFile = {};

    plan.stubs.forEach(function (s) {
        var hits = findDefinition(s.cls, s.member, s.sourceParams || s.headerParams);

        if (hits.length !== 1) {
            manual.push({
                group: { file: s.file, def: s.def, member: s.member },
                why: hits.length === 0 ? '找不到 Buffer 变体的定义文件'
                                       : '匹配到 ' + hits.length + ' 处定义，需手工插入'
            });
            return;
        }

        (byFile[hits[0].file] = byFile[hits[0].file] || []).push(s);
    });

    Object.keys(byFile).forEach(function (file) {
        var lines = fs.readFileSync(file, 'utf8').replace(/\s*$/, '\n').split('\n');
        var at = namespaceEnd(lines);

        if (at < 0) {
            byFile[file].forEach(function (s) {
                manual.push({
                    group: { file: s.file, def: s.def, member: s.member },
                    why: '定位不到 namespace 收尾花括号，需手工插入'
                });
            });
            return;
        }

        var blocks = byFile[file].map(function (s) {
            return s.text;
        });

        lines.splice(at, 0, CPP_MARK, '', blocks.join('\n\n'), '');
        fs.writeFileSync(file, lines.join('\n'));

        blocks.forEach(function (t) {
            written.push({
                file: path.relative(path.resolve(__dirname, '..'), file),
                signature: t.split('\n')[0]
            });
        });
    });

    return { written: written, manual: manual };
}

/* ------------------------------------------ native stubs for the new overloads */

function splitParams(text) {
    var out = [];
    var depth = 0;
    var start = 0;

    for (var i = 0; i < text.length; i++) {
        var c = text[i];

        if (c === '<' || c === '(')
            depth++;
        else if (c === '>' || c === ')')
            depth--;
        else if (c === ',' && depth === 0) {
            out.push(text.slice(start, i).trim());
            start = i + 1;
        }
    }

    var tail = text.slice(start).trim();
    if (tail)
        out.push(tail);

    return out;
}

function lastName(param) {
    var m = /([A-Za-z_][A-Za-z0-9_]*)\s*$/.exec(param);

    return m ? m[1] : param;
}

/* name for the decoded buffer local - `buf` may already be a parameter */
function localName(params) {
    var used = params.map(lastName);
    var candidates = ['buf', 'buffer', 'data_buf', 'bytes'];

    for (var i = 0; i < candidates.length; i++) {
        if (used.indexOf(candidates[i]) < 0)
            return candidates[i];
    }

    return 'buf';
}

/* IR type -> the C++ type idlc generates (best effort: only used to tell the
   Buffer / KeyObject / Object variants of one member apart) */
var TYPE_MAP = {
    Buffer: 'Buffer_base',
    String: 'exlib::string',
    Integer: 'int32_t',
    Long: 'int64_t',
    Boolean: 'bool',
    Number: 'double',
    Object: 'v8::Local<v8::Object>',
    NObject: 'v8::Local<v8::Object>',
    Value: 'v8::Local<v8::Value>',
    Array: 'v8::Local<v8::Array>',
    Date: 'date_t',
    Function: 'v8::Local<v8::Function>',
    Blob: 'Blob_base',
    KeyObject: 'KeyObject_base',
    CryptoKey: 'CryptoKey_base'
};

function typeMatches(param, irParam) {
    var want = TYPE_MAP[irParam.type];

    return !want || param.indexOf(want) >= 0;
}

/*
 * The declaration idlc generated for one variant of a member. Members with
 * several overloads (Buffer / KeyObject / Object keys) are told apart by the
 * parameter names - the header takes them from the IDL - plus the IR types of
 * the parameters that are not part of this variant.
 */
function variantDecl(defName, member, irParams, positions) {
    var file = path.join(IFS_FOLDER, defName + '.h');
    if (!fs.existsSync(file))
        return null;

    var lines = fs.readFileSync(file, 'utf8').split('\n');

    for (var i = 0; i < lines.length; i++) {
        var m = /^\s*(?:static |virtual )*result_t ([A-Za-z_][A-Za-z0-9_]*)\((.*?)\)\s*(?:=\s*0)?\s*;\s*$/.exec(lines[i]);
        if (!m || m[1] !== member)
            continue;

        var params = splitParams(m[2]);
        if (params.length < irParams.length)
            continue;

        var ok = true;

        for (var j = 0; j < irParams.length && ok; j++) {
            var flipped = positions.indexOf(j) >= 0;

            ok = lastName(params[j]) === irParams[j].name &&
                (flipped ? params[j].indexOf('exlib::string') === 0 : typeMatches(params[j], irParams[j]));
        }

        if (!ok)
            continue;

        return { params: params, head: params.slice(0, irParams.length) };
    }

    return null;
}

/*
 * Print the native forwarder each new overload needs. The signature is taken
 * from what idlc generated, so it always matches the binding; the body is the
 * mechanical decode-and-forward.
 */
function printManual(manual) {
    if (!manual.length)
        return;

    console.log('# 需要手工处理：');
    manual.forEach(function (m) {
        console.log('  ' + m.group.file + ' ' + m.group.def + '.' + m.group.member + ' — ' + m.why);
    });
}

/*
 * Print the native forwarder each new overload needs. The signature comes from
 * the declaration idlc generated, so it always matches the binding.
 */
function printStubs(defs, rows, opts) {
    var plan = nativeStubs(defs, rows);
    var text = plan.stubs.map(function (s) {
        return '// ' + s.file + ' — ' + s.def + '.' + s.member +
            ' (position ' + s.positions.map(function (p) { return p + 1; }).join(', ') + ')\n' +
            s.text + '\n';
    }).join('\n');

    if (typeof opts.stubs === 'string') {
        fs.writeFileSync(path.resolve(opts.stubs), text);
        console.log('native 转发骨架已写入 ' + path.resolve(opts.stubs));
    } else {
        console.log(text);
    }

    console.log('native：已实现 ' + plan.implemented + ' 个，待补 ' + plan.stubs.length + ' 个');
    printManual(plan.manual);
}

/* Reordering the declarations is a developer decision: idlc reports the
   shadowing overloads (tools/util/check_overloads.js) and the vendored IDL is
   edited by hand. */

function main(argv) {
    var opts = parseArgs(argv);

    if (opts.help) {
        console.log('usage: fibjs tools/check_idl_overloads.js [--json] [--markdown [FILE]] ' +
            '[--file SUBSTR] [--quiet] [--fail-on-gaps]\n' +
            '       [--write] [--fix-comments]   (the native side is hand-written)');
        return 0;
    }

    var defs = parseIDL(IDL_FOLDER);
    var fileMap = fileIndex();
    var result = audit(defs, fileMap);

    if (opts.file) {
        result.rows = result.rows.filter(function (r) {
            return r.file.indexOf(opts.file) >= 0;
        });
        result.events = result.events.filter(function (e) {
            return e.file.indexOf(opts.file) >= 0;
        });
    }

    var s = summarize(result.rows);

    if (opts.markdown) {
        var target = opts.markdown === true ? AUDIT_DOC : path.resolve(opts.markdown);
        fs.mkdirSync(path.dirname(target), { recursive: true });
        fs.writeFileSync(target, auditMarkdown(result, new Date().toISOString().slice(0, 10)));
        console.log('审计表已写入 ' + target);
    }

    if (opts.json) {
        var clean = function (r) {
            return {
                file: r.file,
                def: r.def,
                member: r.member,
                position: r.position,
                param: r.param,
                signature: r.signature,
                verdict: r.verdict,
                detail: r.detail
            };
        };

        console.log(JSON.stringify({
            summary: s,
            gaps: result.rows.filter(function (r) {
                return r.verdict !== 'ok';
            }).map(clean),
            events: result.events
        }, null, 2));
    } else {
        printReport(result, opts);
    }

    var added = [];
    var repaired = [];

    if (opts.write) {
        repaired = fixComments(result.rows);
        added = applyAdds(defs, result.rows);
    } else if (opts.fixComments) {
        repaired = fixComments(result.rows);
    }

    if (added.length || repaired.length) {
        console.log('');

        if (repaired.length) {
            console.log('待补充函数/参数注释的 String 重载 ' + repaired.length + ' 个：');
            repaired.forEach(function (a) {
                console.log('  ~ ' + a.file + '  ' + a.signature);
            });
        }

        if (added.length) {
            console.log('待补 ' + added.length + ' 个 String 重载（IDL 回写已废弃，请手工应用）：');
            added.forEach(function (a) {
                console.log('  + ' + a.file + '  ' + a.member + ' -> ' + a.signature);
            });
            console.log('下一步：fibjs tools/idlc.js → fibjs tools/gen_scripts.js → bash build ' +
                '（native 的 String 重载一律手写，不自动生成）');
        }
    } else if (opts.write || opts.fixComments) {
        console.log('');
        console.log('没有可自动改的：缺口/注释都是齐的（复核、待裁决项请手工处理）。');
    }

    if (opts.failOnGaps && (s.gap || s.partial))
        return 1;

    return 0;
}

if (require.main === module)
    process.exitCode = main(process.argv.slice(2));

module.exports = {
    audit: audit,
    signature: signature,
    fileIndex: fileIndex,
    variantPlan: variantPlan,
    groupGaps: groupGaps
};
