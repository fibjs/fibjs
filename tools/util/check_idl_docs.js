/**
 * @description documentation checker for the IDL corpus (rules X1-X10).
 *
 * `idl/*.idl` is the only source of the generated manual (d.ts, `--man`,
 * docs/manual), so the checker keeps the comments complete and keeps every new
 * definition clean: a definition carries a summary and a detail section, every
 * non-const member is documented beyond a one-liner, `@param` names match the
 * declaration, example blocks carry a legal marker or parse as JavaScript,
 * definitions carry enough runnable examples, and the abstract base classes are
 * only shown through their concrete subclasses. The rules and the examples
 * marker convention are specified in
 * `plans/idl-doc-completion-plan-2026-10-05.md` sections 2.3, 5.5, 5.6 and 7.1.
 *
 * Ratchet: `tools/util/idl_docs_baseline.json` stores the global counts and
 * the per-definition counts of the 2026-10-05 corpus. In the default mode an
 * existing definition may not grow its per-rule count, and a definition that
 * is not in the baseline (a new one) must be clean. The `--changed`, `--batch`
 * and `--files` modes select definitions and check them with zero tolerance,
 * which is what `tools/idlc.js` runs before generating.
 *
 * Rules:
 *   X1  definition/member summary present (always fatal)
 *   X2  @param names/order/count match the declaration; no @return without a
 *       return type (ratchet)
 *   X3  thin non-const member docs, text length < 80 (ratchet); same formula
 *       as temp/idl_audit.js, const members are exempt (section 2.3)
 *   X4  non-const members without a detail section (ratchet); const exempt
 *   X5  definition without a detail section or with a doc < 160 chars
 *       (ratchet)
 *   X6  example block markers (`// fragment: <reason>` / `// requires:
 *       <service>`, service in the whitelist) and the JavaScript syntax of
 *       the unmarked blocks, through vm.Script (ratchet). A block whose owner
 *       is a passing entry of the examples report is accepted.
 *   X7  definition-level examples (unmarked + requires blocks, fragments not
 *       counted) >= 2, or >= 3 for a module with >= 30 members / an interface
 *       with >= 20 members (ratchet)
 *   X8  runnable examples failed in temp/idl_examples_report.json (fatal when
 *       the report exists; a missing report is a warning only)
 *   X9  no example block constructs one of the ten abstract base classes
 *       (ratchet)
 *   X10 an abstract base class with member examples must exercise at least
 *       one recommended concrete subclass (ratchet)
 *
 * @param {Record<string, import('./ir').IIDLDefinition>} defs
 * @param {object} [opts]
 * @param {string|null} [opts.baseline] baseline JSON path; null enters the
 *     absolute mode (every rule zero tolerance, for unit tests); when omitted
 *     the mode checks against tools/util/idl_docs_baseline.json
 * @param {string|false} [opts.report] examples report path (default
 *     temp/idl_examples_report.json); false skips X6's report escape and X8
 * @param {boolean} [opts.changed] check only the definitions of the
 *     `idl/*.idl` files reported by `git status --porcelain`, zero tolerance
 * @param {string[]} [opts.selected] explicit definition names, zero tolerance
 * @returns {Array<{rule: string, def: string, member: string|null, message: string}>}
 */

'use strict';

var fs = require('fs');
var path = require('path');
var vm = require('vm');

var REPO_ROOT = path.resolve(__dirname, '../..');
var IDL_FOLDER = path.join(REPO_ROOT, 'idl');
var DEFAULT_BASELINE = path.join(__dirname, 'idl_docs_baseline.json');
var DEFAULT_REPORT = path.join(REPO_ROOT, 'temp/idl_examples_report.json');
var TODOS_FILE = path.join(REPO_ROOT, 'plans/idl-doc-todos.md');

var THIN = 80;             // X3: member doc text below this is thin
var DECL_THIN = 160;       // X5: definition doc text below this is thin
var EXAMPLES_MIN = 2;      // X7: per definition
var EXAMPLES_MIN_BIG = 3;  // X7: big module / big interface
var BIG_MODULE_MEMBERS = 30;
var BIG_CLASS_MEMBERS = 20;

var SERVICES = ['redis', 'mysql', 'sqlite', 'network', 'windows', 'long-running'];

var RULES = ['X1', 'X2', 'X3', 'X4', 'X5', 'X6', 'X7', 'X8', 'X9', 'X10'];
var RATCHET_RULES = ['X2', 'X3', 'X4', 'X5', 'X6', 'X7', 'X9', 'X10'];

var RULE_TITLES = {
    X1: 'missing summaries',
    X2: 'signature mismatches',
    X3: 'thin non-const members',
    X4: 'non-const members without detail',
    X5: 'definitions without detail or thin',
    X6: 'example markers and syntax',
    X7: 'definitions with too few examples',
    X8: 'failed runnable examples (report)',
    X9: 'examples constructing an abstract base class',
    X10: 'abstract base classes without a concrete subclass example'
};

// plans/... section 5.6: the ten abstract base classes and the concrete paths
// their member examples must go through; `derived` patterns match a construction
// of (or an entry into) the recommended subclass.
var BASE_CLASSES = {
    XmlNode: {
        hint: 'new xml.Document() or xml.parse()',
        derived: ['\\bnew\\s+(?:xml\\.)?Document\\s*\\(', '\\bxml\\.parse\\s*\\(']
    },
    Stream: {
        hint: 'new io.MemoryStream() / new net.Socket() / fs.createReadStream()',
        derived: ['\\bnew\\s+(?:io\\.)?MemoryStream\\s*\\(', '\\bnew\\s+(?:net\\.)?Socket\\s*\\(',
            '\\bfs\\.createReadStream\\s*\\(']
    },
    HttpMessage: {
        hint: 'new http.Request() / new http.Response()',
        derived: ['\\bnew\\s+(?:http\\.)?Request\\s*\\(', '\\bnew\\s+(?:http\\.)?Response\\s*\\(']
    },
    HttpCollection: {
        hint: 'new http.Headers() / new URLSearchParams() / new FormData()',
        derived: ['\\bnew\\s+(?:http\\.)?Headers\\s*\\(', '\\bnew\\s+URLSearchParams\\s*\\(',
            '\\bnew\\s+FormData\\s*\\(']
    },
    XmlCharacterData: {
        hint: 'createTextNode() / createComment() / createCDATASection()',
        derived: ['createTextNode\\s*\\(', 'createComment\\s*\\(', 'createCDATASection\\s*\\(']
    },
    SeekableStream: {
        hint: 'new io.MemoryStream() / new io.RangeStream(...)',
        derived: ['\\bnew\\s+(?:io\\.)?MemoryStream\\s*\\(', '\\bnew\\s+(?:io\\.)?RangeStream\\s*\\(']
    },
    ZlibCodec: {
        hint: 'new zlib.Gzip() / new zlib.Inflate()',
        derived: ['\\bnew\\s+(?:zlib\\.)?Gzip\\s*\\(', '\\bnew\\s+(?:zlib\\.)?Inflate\\s*\\(']
    },
    Iterator: {
        hint: 'new Dir(...) or a collection iterator (keys()/entries())',
        derived: ['\\bnew\\s+Dir\\s*\\(', '\\.keys\\s*\\(', '\\.entries\\s*\\(']
    },
    XmlText: {
        hint: 'createTextNode()',
        derived: ['createTextNode\\s*\\(']
    },
    object: {
        hint: 'any concrete class',
        // `new object` is banned by X9, so exclude the base name itself here
        derived: ['\\bnew\\s+(?!object\\b)[A-Za-z_$][\\w$]*(?:\\.[\\w$]+)*\\s*\\(']
    }
};

var CODE_BLOCK_RE = /```[Jj]ava[Ss]cript[^\n]*\n([\s\S]*?)```/g;

/* ------------------------------- helpers -------------------------------- */

function hasSummary(doc) {
    return !!(doc && doc.descript && doc.descript.trim());
}

function textLen(doc) {
    if (!doc) return 0;
    var n = (doc.descript || '').length;
    var i;
    for (i = 0; i < (doc.detail || []).length; i++)
        n += doc.detail[i].length;
    for (i = 0; i < (doc.params || []).length; i++)
        n += (doc.params[i].descript || '').length;
    if (doc['return'])
        n += (doc['return'].descript || '').length;
    return n;
}

function hasDetail(doc) {
    return ((doc && doc.detail) || []).join('').trim().length > 0;
}

// the extraction formula of temp/idl_audit.js: the brief line plus the detail
// lines, only ```JavaScript blocks
function codeBlocks(doc) {
    var all = ((doc && doc.descript) || '') + '\n' + (((doc && doc.detail) || []).join('\n'));
    var out = [];
    var m;
    CODE_BLOCK_RE.lastIndex = 0;
    while ((m = CODE_BLOCK_RE.exec(all)) !== null)
        out.push(m[1]);
    return out;
}

// the member plus its overloads; parser output keeps every overload as its own
// member, gen_code groups them under `overs` (whose first entry is a copy of
// the member itself, skipped here)
function memberEntries(def) {
    var out = [];
    (def.members || []).forEach(function (m) {
        out.push({ name: m.name, doc: m.doc, params: m.params, type: m.type, memType: m.memType });
        (m.overs || []).forEach(function (o) {
            if (o === m || o.doc === m.doc || (o.comments !== undefined && o.comments === m.comments))
                return;
            out.push({ name: o.name || m.name, doc: o.doc, params: o.params, type: o.type, memType: o.memType });
        });
    });
    return out;
}

// every example block of a definition, with its owner: member null for the
// declaration-level blocks, the member name for member and overload blocks
function collectBlocks(def) {
    var name = def.declare.name;
    var blocks = [];

    function add(doc, member) {
        codeBlocks(doc).forEach(function (code) {
            blocks.push({ def: name, member: member, code: code });
        });
    }

    add(def.declare.doc, null);
    memberEntries(def).forEach(function (e) {
        add(e.doc, e.name);
    });
    return blocks;
}

// the first non-empty line is the marker line; a malformed `// fragment` /
// `// requires` line is reported as such, any other comment line is just code
function classifyBlock(code) {
    var lines = String(code).split('\n');
    var first = '';
    for (var i = 0; i < lines.length; i++) {
        if (lines[i].trim() !== '') {
            first = lines[i].trim();
            break;
        }
    }

    var m = /^\/\/\s*(fragment|requires)\s*:\s*(.*)$/.exec(first);
    if (m)
        return { kind: m[1], value: m[2].trim() };
    if (/^\/\/\s*(fragment|requires)\b/.test(first))
        return { kind: 'malformed', value: first };
    return { kind: 'runnable', value: null };
}

// vm.Script parses a classic script; a top-level await (or a top-level return)
// is retried inside an async IIFE, the form fibjs executes the example in
function syntaxErrorOf(code) {
    try {
        new vm.Script(code);
        return null;
    } catch (e) { /* retried below */ }

    try {
        new vm.Script('(async()=>{\n' + code + '\n})();');
        return null;
    } catch (e) {
        var msg = String((e && e.message) || e);
        return msg.split('\n')[0];
    }
}

function firstLineOf(value) {
    return String(value === undefined || value === null ? '' : value).split('\n')[0];
}

/* ------------------------- examples report (X6/X8) ----------------------- */

function loadReport(reportPath) {
    var raw;
    try {
        raw = fs.readFileSync(reportPath, 'utf8');
    } catch (e) {
        return null;
    }
    try {
        return JSON.parse(raw);
    } catch (e) {
        console.warn('[check_idl_docs] ignoring unreadable examples report ' + reportPath + ': ' + e.message);
        return null;
    }
}

function reportEntries(report) {
    if (!report) return [];
    if (Array.isArray(report)) return report;
    var keys = ['results', 'examples', 'entries'];
    for (var i = 0; i < keys.length; i++) {
        if (Array.isArray(report[keys[i]])) return report[keys[i]];
    }
    return [];
}

function entryOwner(entry) {
    if (!entry) return null;
    if (typeof entry.owner === 'string') return entry.owner;
    if (typeof entry.name === 'string') return entry.name;
    if (typeof entry.def === 'string')
        return entry.def + (entry.member ? '.' + entry.member : '');
    return null;
}

function isSkippedEntry(entry) {
    if (!entry) return true;
    if (entry.skipped === true || entry.status === 'skipped') return true;
    var kind = entry.category || entry.mode || entry.kind;
    return kind === 'fragment' || kind === 'requires';
}

function isFailedEntry(entry) {
    return !isSkippedEntry(entry) && entry.status !== 'ok';
}

function passingOwners(report) {
    var set = {};
    reportEntries(report).forEach(function (e) {
        if (e && e.status === 'ok') {
            var owner = entryOwner(e);
            if (owner) set[owner] = true;
        }
    });
    return set;
}

/* ------------------------------- rules ---------------------------------- */

function add(ctx, rule, def, member, message) {
    ctx.findings.push({ rule: rule, def: def, member: member || null, message: message });
}

function blockOwner(block) {
    return block.def + (block.member ? '.' + block.member : '');
}

function ruleX1(ctx, def) {
    var name = def.declare.name;
    if (!hasSummary(def.declare.doc))
        add(ctx, 'X1', name, null, 'definition is missing its summary (the first comment line)');
    memberEntries(def).forEach(function (e) {
        if (!hasSummary(e.doc))
            add(ctx, 'X1', name, e.name, 'member is missing its summary (the first comment line)');
    });
}

function paramNames(params) {
    return (params || []).map(function (p) { return p.name; }).join(', ');
}

function ruleX2(ctx, def) {
    var name = def.declare.name;
    memberEntries(def).forEach(function (e) {
        var declared = e.params || [];
        var documented = (e.doc && e.doc.params) || [];

        if (documented.length !== declared.length) {
            add(ctx, 'X2', name, e.name, '@param count ' + documented.length +
                ' does not match the declaration (' + declared.length + ': ' + paramNames(declared) + ')');
        } else {
            for (var i = 0; i < declared.length; i++) {
                if (documented[i].name !== declared[i].name) {
                    add(ctx, 'X2', name, e.name, '@param ' + (i + 1) + ' is `' + documented[i].name +
                        '`, the declaration has `' + declared[i].name + '`');
                    break;
                }
            }
        }

        if (e.doc && e.doc['return'] && !e.type)
            add(ctx, 'X2', name, e.name, 'documents @return, but the declaration has no return type');
    });
}

// const members keep the one-line form (section 2.3)
function isConst(entry) {
    return entry.memType === 'const';
}

function ruleX3(ctx, def) {
    var name = def.declare.name;
    memberEntries(def).forEach(function (e) {
        if (isConst(e)) return;
        var len = textLen(e.doc);
        if (!hasSummary(e.doc) || len < THIN)
            add(ctx, 'X3', name, e.name, hasSummary(e.doc) ?
                'member doc is thin (' + len + ' < ' + THIN + ' chars)' :
                'member doc is thin (no summary, ' + len + ' chars)');
    });
}

function ruleX4(ctx, def) {
    var name = def.declare.name;
    memberEntries(def).forEach(function (e) {
        if (isConst(e)) return;
        if (!hasDetail(e.doc))
            add(ctx, 'X4', name, e.name, 'member has no detail section');
    });
}

function ruleX5(ctx, def) {
    var name = def.declare.name;
    var doc = def.declare.doc;
    var reasons = [];
    if (!hasDetail(doc))
        reasons.push('has no detail section');
    var len = textLen(doc);
    if (len < DECL_THIN)
        reasons.push('doc is thin (' + len + ' < ' + DECL_THIN + ' chars)');
    if (reasons.length)
        add(ctx, 'X5', name, null, 'definition ' + reasons.join(' and '));
}

function ruleX6(ctx, def) {
    collectBlocks(def).forEach(function (block) {
        var owner = blockOwner(block);
        var c = classifyBlock(block.code);

        if (c.kind === 'malformed') {
            add(ctx, 'X6', block.def, block.member, 'example block starts with a malformed marker `' +
                firstLineOf(c.value) + '` (use `// fragment: <reason>` or `// requires: <service>`)');
        } else if (c.kind === 'fragment') {
            if (!c.value)
                add(ctx, 'X6', block.def, block.member,
                    'fragment marker needs a non-empty reason (`// fragment: <reason>`)');
        } else if (c.kind === 'requires') {
            if (!c.value)
                add(ctx, 'X6', block.def, block.member,
                    'requires marker needs a service (`// requires: <service>`)');
            else if (SERVICES.indexOf(c.value) < 0)
                add(ctx, 'X6', block.def, block.member, 'requires service `' + c.value +
                    '` is not one of ' + SERVICES.join('/'));
        } else if (!ctx.passOwners[owner]) {
            var err = syntaxErrorOf(block.code);
            if (err)
                add(ctx, 'X6', block.def, block.member,
                    'unmarked example block is not valid JavaScript: ' + err);
        }
    });
}

function ruleX7(ctx, def) {
    var name = def.declare.name;
    var count = 0;
    codeBlocks(def.declare.doc).forEach(function (code) {
        var c = classifyBlock(code);
        if (c.kind === 'runnable' || c.kind === 'requires')
            count++;
    });

    var members = (def.members || []).length;
    var big = def.declare.type === 'module' ?
        members >= BIG_MODULE_MEMBERS : members >= BIG_CLASS_MEMBERS;
    var need = big ? EXAMPLES_MIN_BIG : EXAMPLES_MIN;
    if (count < need)
        add(ctx, 'X7', name, null, 'definition has ' + count + ' definition-level example(s), needs at least ' + need);
}

function ruleX8(ctx) {
    reportEntries(ctx.report).forEach(function (e) {
        if (!isFailedEntry(e)) return;
        var owner = entryOwner(e) || '(unknown owner)';
        var dot = owner.indexOf('.');
        var defName = dot >= 0 ? owner.slice(0, dot) : owner;
        var member = dot >= 0 ? owner.slice(dot + 1) : null;
        var detail = firstLineOf(e.error || e.message || e.stderr);
        add(ctx, 'X8', defName, member, 'runnable example failed (status: ' + e.status + ')' +
            (detail ? ': ' + detail : ''));
    });
}

function ruleX9(ctx, def) {
    var bases = Object.keys(BASE_CLASSES);
    collectBlocks(def).forEach(function (block) {
        bases.forEach(function (base) {
            if (new RegExp('\\bnew\\s+' + base + '\\b').test(block.code))
                add(ctx, 'X9', block.def, block.member, 'example block constructs the abstract base class `' +
                    base + '`; use a concrete subclass (see plans/idl-doc-completion-plan-2026-10-05.md 5.6)');
        });
    });
}

function ruleX10(ctx, def) {
    var name = def.declare.name;
    var base = BASE_CLASSES[name];
    if (!base) return;

    var memberBlocks = collectBlocks(def).filter(function (b) { return b.member !== null; });
    if (!memberBlocks.length) return;

    var ok = memberBlocks.some(function (block) {
        return base.derived.some(function (re) { return new RegExp(re).test(block.code); });
    });
    if (!ok)
        add(ctx, 'X10', name, null, 'abstract base class members carry examples, but none goes through a concrete ' +
            'subclass (' + base.hint + ')');
}

/* ------------------------------ rule driver ------------------------------ */

function defList(defs) {
    return Object.keys(defs || {}).sort().map(function (n) { return defs[n]; });
}

function countGlobal(findings) {
    var global = {};
    RULES.forEach(function (r) { global[r] = 0; });
    findings.forEach(function (p) {
        if (global[p.rule] === undefined) global[p.rule] = 0;
        global[p.rule]++;
    });
    return global;
}

function runRules(defs, report) {
    var ctx = { findings: [], report: report, passOwners: passingOwners(report) };
    defList(defs).forEach(function (def) {
        ruleX1(ctx, def);
        ruleX2(ctx, def);
        ruleX3(ctx, def);
        ruleX4(ctx, def);
        ruleX5(ctx, def);
        ruleX6(ctx, def);
        ruleX7(ctx, def);
        ruleX9(ctx, def);
        ruleX10(ctx, def);
    });
    ruleX8(ctx);
    return ctx.findings;
}

/* --------------------------- baseline / ratchet -------------------------- */

function loadBaseline(baselinePath) {
    if (!fs.existsSync(baselinePath))
        throw new Error('baseline not found: ' + baselinePath +
            ' (run `node tools/util/check_idl_docs.js --update`)');
    var baseline = JSON.parse(fs.readFileSync(baselinePath, 'utf8'));
    baseline.global = baseline.global || {};
    baseline.definitions = baseline.definitions || {};
    return baseline;
}

function baselineFromFindings(findings) {
    var global = {};
    RATCHET_RULES.forEach(function (r) { global[r] = 0; });

    var counts = {};
    findings.forEach(function (p) {
        if (RATCHET_RULES.indexOf(p.rule) < 0) return;
        global[p.rule]++;
        if (!counts[p.def]) counts[p.def] = {};
        counts[p.def][p.rule] = (counts[p.def][p.rule] || 0) + 1;
    });

    var definitions = {};
    Object.keys(counts).sort().forEach(function (d) { definitions[d] = counts[d]; });

    return {
        _comment: 'Generated by tools/util/check_idl_docs.js --update; do not edit. ' +
            'The counts are the ratchet ceiling: a per-definition count may never grow, and a ' +
            'definition that is absent here must be clean (0 for every rule X2-X10 except X1/X8, ' +
            'which are always fatal). Regenerate only with --update.',
        generated: new Date().toISOString().slice(0, 10),
        global: global,
        definitions: definitions
    };
}

// a definition may keep its baseline violations, never grow them, and a new
// definition starts at zero; X1 and X8 are always fatal
function applyRatchet(findings, baseline) {
    var totals = {};
    findings.forEach(function (p) {
        if (p.rule === 'X1' || p.rule === 'X8') return;
        var key = p.def + '\u0000' + p.rule;
        totals[key] = (totals[key] || 0) + 1;
    });

    var out = [];
    findings.forEach(function (p) {
        if (p.rule === 'X1' || p.rule === 'X8') {
            out.push(p);
            return;
        }
        var entry = baseline.definitions[p.def] || {};
        var allowed = entry[p.rule] || 0;
        var total = totals[p.def + '\u0000' + p.rule];
        if (total <= allowed) return;
        p.message += ' [' + p.rule + ' count ' + total + ' > baseline ' + allowed + ']';
        out.push(p);
    });
    return out;
}

/* -------------------------- changed files / filters ----------------------- */

// the definition name of an idl file; a file holds exactly one top-level
// module/interface declaration
function defNameFromSource(src) {
    var code = String(src)
        .replace(/\/\*[\s\S]*?\*\//g, ' ')
        .replace(/\/\/[^\n]*/g, ' ');
    var m = /\b(?:module|interface)\s+([A-Za-z_$][\w$]*)/.exec(code);
    return m ? m[1] : null;
}

function changedIdlFiles() {
    var out;
    try {
        out = require('child_process').execFileSync('git', ['-C', REPO_ROOT, 'status', '--porcelain'],
            { encoding: 'utf8' });
    } catch (e) {
        console.warn('[check_idl_docs] `git status --porcelain` failed, --changed checks nothing: ' +
            firstLineOf(e && e.message));
        return [];
    }

    var files = [];
    String(out).split('\n').forEach(function (line) {
        if (!line) return;
        var p = line.slice(3).trim();
        var arrow = p.indexOf(' -> ');
        if (arrow >= 0) p = p.slice(arrow + 4);
        if (p.charAt(0) === '"') {
            try { p = JSON.parse(p); } catch (e) { /* keep the raw path */ }
        }
        if (/^idl\/[^/]+\.idl$/.test(p))
            files.push(path.join(REPO_ROOT, p));
    });
    return files;
}

function selectedFromGit(defs) {
    var names = {};
    changedIdlFiles().forEach(function (file) {
        var name = defNameFromSource(fs.readFileSync(file, 'utf8'));
        if (name && defs[name]) names[name] = true;
    });
    return Object.keys(names).sort();
}

function selectedFromFiles(defs, csv) {
    var names = {};
    String(csv).split(',').forEach(function (f) {
        f = f.trim();
        if (!f) return;
        var abs = path.isAbsolute(f) ? f : path.join(REPO_ROOT, f);
        if (!fs.existsSync(abs))
            throw new Error('file not found: ' + f);
        var name = defNameFromSource(fs.readFileSync(abs, 'utf8'));
        if (!name || !defs[name])
            throw new Error('cannot map ' + f + ' to a parsed IDL definition');
        names[name] = true;
    });
    var out = Object.keys(names).sort();
    if (!out.length)
        throw new Error('no definition selected from --files');
    return out;
}

function selectedFromBatch(defs, batch) {
    var text = fs.readFileSync(TODOS_FILE, 'utf8');
    var names = {};
    text.split('\n').forEach(function (line) {
        var m = /^- \[[ xX]\] (B\d+)-\d+ `([^`]+)`/.exec(line.trim());
        if (m && m[1] === batch && defs[m[2]])
            names[m[2]] = true;
    });
    var out = Object.keys(names).sort();
    if (!out.length)
        throw new Error('no corpus definition found for batch ' + batch + ' in ' + TODOS_FILE);
    return out;
}

/* -------------------------------- analyze -------------------------------- */

function mapOf(list) {
    var m = {};
    (list || []).forEach(function (n) { m[n] = true; });
    return m;
}

function analyze(defs, opts) {
    opts = opts || {};

    var reportPath = opts.report === false ? null :
        path.resolve(String(opts.report || DEFAULT_REPORT));
    var report = reportPath ? loadReport(reportPath) : null;
    if (reportPath && !report)
        console.warn('[check_idl_docs] no examples report at ' + reportPath + ' (X8 is skipped)');

    var findings = runRules(defs, report);

    var selected = null;
    if (opts.selected)
        selected = mapOf(opts.selected);
    else if (opts.changed)
        selected = mapOf(selectedFromGit(defs));

    if (selected)
        findings = findings.filter(function (p) { return p.rule === 'X8' || selected[p.def]; });

    var global = countGlobal(findings);

    if (selected)
        return { mode: 'selected', problems: findings, global: global, baseline: null };

    if (opts.baseline === null)
        return { mode: 'absolute', problems: findings, global: global, baseline: null };

    var baseline = loadBaseline(opts.baseline === undefined ? DEFAULT_BASELINE : opts.baseline);
    return { mode: 'ratchet', problems: applyRatchet(findings, baseline), global: global, baseline: baseline };
}

function check(defs, opts) {
    return analyze(defs, opts).problems;
}

/* ---------------------------------- CLI ---------------------------------- */

function usage(message) {
    if (message)
        console.error('[check_idl_docs] ' + message);
    console.error('usage: node tools/util/check_idl_docs.js [--json] [--update] [--changed] ' +
        '[--batch Bn] [--files idl/fs.idl,...] [--baseline <path>] [--report <path>]');
    process.exit(1);
}

function parseArgs(argv) {
    var opts = {
        json: false, update: false, changed: false,
        batch: null, files: null, baseline: null, report: null
    };

    for (var i = 0; i < argv.length; i++) {
        var a = argv[i];
        var v = null;
        var eq = a.indexOf('=');
        if (a.slice(0, 2) === '--' && eq > 2) {
            v = a.slice(eq + 1);
            a = a.slice(0, eq);
        }

        switch (a) {
            case '--json': opts.json = true; break;
            case '--update': opts.update = true; break;
            case '--changed': opts.changed = true; break;
            case '--help':
            case '-h': usage(null); break;
            case '--batch':
            case '--files':
            case '--baseline':
            case '--report':
                if (v === null) {
                    v = argv[++i];
                    if (v === undefined || v.slice(0, 2) === '--') usage('missing value for ' + a);
                }
                opts[a.slice(2)] = v;
                break;
            default: usage('unknown option: ' + a);
        }
    }
    return opts;
}

function printGlobal(global, baseline) {
    RULES.forEach(function (r) {
        var line = '  ' + r + ' ' + RULE_TITLES[r] + ': ' + (global[r] || 0);
        if (baseline && baseline.global && baseline.global[r] !== undefined)
            line += ' (baseline ' + baseline.global[r] + ')';
        console.log(line);
    });
}

function printProblems(problems, limit) {
    var byRule = {};
    problems.forEach(function (p) {
        if (!byRule[p.rule]) byRule[p.rule] = [];
        byRule[p.rule].push(p);
    });

    RULES.forEach(function (r) {
        if (!byRule[r]) return;
        console.log('\n== ' + r + ' ' + RULE_TITLES[r] + ' (' + byRule[r].length + ') ==');
        byRule[r].slice(0, limit).forEach(function (p) {
            console.log('  ' + p.def + (p.member ? '.' + p.member : '') + ': ' + p.message);
        });
        if (byRule[r].length > limit)
            console.log('  ... and ' + (byRule[r].length - limit) + ' more');
    });
}

function countMembers(defs) {
    var n = 0;
    Object.keys(defs || {}).forEach(function (d) { n += (defs[d].members || []).length; });
    return n;
}

function cli(argv) {
    var opts = parseArgs(argv);
    var parser = require('./parser');
    var defs = parser(IDL_FOLDER);

    if (opts.update) {
        var findings = runRules(defs, null);
        var missing = findings.filter(function (p) { return p.rule === 'X1'; });
        if (missing.length) {
            console.error('[check_idl_docs] refusing to write a baseline while summaries are missing:');
            printProblems(missing, 50);
            return 1;
        }

        var baseline = baselineFromFindings(findings);
        var target = opts.baseline ? path.resolve(opts.baseline) : DEFAULT_BASELINE;
        fs.writeFileSync(target, JSON.stringify(baseline, null, 2) + '\n');

        if (opts.json)
            console.log(JSON.stringify({ ok: true, global: baseline.global, problems: [] }, null, 2));
        else {
            console.log('baseline written to ' + target + ' (' + Object.keys(baseline.definitions).length +
                ' definition(s) with violations)');
            printGlobal(baseline.global, null);
        }
        return 0;
    }

    var selected = null;
    if (opts.files) selected = selectedFromFiles(defs, opts.files);
    else if (opts.batch) selected = selectedFromBatch(defs, opts.batch);
    else if (opts.changed) selected = selectedFromGit(defs);

    var res = analyze(defs, {
        baseline: selected === null ? (opts.baseline ? path.resolve(opts.baseline) : undefined) : null,
        report: opts.report ? path.resolve(opts.report) : undefined,
        selected: selected === null ? undefined : selected
    });

    if (opts.json) {
        console.log(JSON.stringify({
            ok: res.problems.length === 0,
            global: res.global,
            problems: res.problems
        }, null, 2));
    } else {
        console.log('IDL documentation check (X1-X10), mode: ' + res.mode +
            ', definitions: ' + Object.keys(defs).length + ', members: ' + countMembers(defs));
        printGlobal(res.global, res.baseline);
        if (res.problems.length)
            printProblems(res.problems, 200);
        console.log('\n' + (res.problems.length ?
            'FAIL: ' + res.problems.length + ' problem(s) beyond the baseline' :
            'OK: no problems beyond the baseline'));
    }

    return res.problems.length ? 1 : 0;
}

// fibjs has no require.main, so the entry point is recognized from argv[1]
function isEntryPoint() {
    var entry = process.argv && process.argv[1];
    if (!entry) return false;
    try {
        return path.resolve(entry) === __filename;
    } catch (e) {
        return false;
    }
}

if (isEntryPoint()) {
    var exitCode = 1;
    try {
        exitCode = cli(process.argv.slice(2));
    } catch (e) {
        console.error('[check_idl_docs] ' + firstLineOf(e && e.message));
        exitCode = 1;
    }
    process.exit(exitCode);
}

module.exports = check;
module.exports.analyze = analyze;
