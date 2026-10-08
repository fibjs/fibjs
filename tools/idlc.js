var path = require('path');
var parser = require('./util/parser');
var gen_code = require('./util/gen_code');
var record_idljson = require('./util/record_idljson');
var gen_dts = require('./util/gen_dts');
var gen_builtin_types = require('./gen_builtin_types');

var idlFolder = path.resolve(__dirname, '../idl');
var baseCodeFolder = path.resolve(__dirname, "../fibjs/include/ifs/");

console.log('🚀 Starting fibjs IDL compilation...');
console.log(`📁 IDL Folder: ${idlFolder}`);
console.log(`📁 Code Output: ${baseCodeFolder}`);

console.log('\n📖 Parsing IDL definitions...');
var defs = parser(idlFolder);
console.log(`✅ Parsed ${Object.keys(defs).length} IDL definitions`);

// The call operator is spelled `operator(...)` (idl-def.pegjs). The historical
// `Function(...)` spelling would now register as a plain method - a silent
// runtime/type regression - so it is rejected loudly.
Object.values(defs).forEach((def) => {
    (def.members || []).forEach((mem) => {
        if (mem.memType === 'method' && mem.name === 'Function')
            throw new Error(`${def.declare.name}.Function: the call operator is written \`operator(...)\` (the historical \`Function(...)\` spelling was retired)`);
    });
});

console.log('\n🔎 Checking overload shadowing...');
var shadowed = require('./util/check_overloads')(defs);
if (shadowed.length) {
    console.log(`   ❌ ${shadowed.length} overload shadowing problem(s):`);
    shadowed.forEach((p) => {
        console.log(`      ${p.def}.${p.member}${p.static ? ' [static]' : ''}: (${p.stringVariant}) is declared before (${p.shadowed}), argument ${p.slot} takes String where (${p.shadowed}) takes ${p.shadowedType}`);
    });
    console.log(`   ⚠️  A lenient String parameter (v->ToString()) swallows the values meant for the
   ⚠️  overload that follows it. Move the String variants to the end of the member.`);
} else
    console.log('✅ No overload shadowing');

console.log('\n🔎 Checking callback shapes...');
var callbackShapeProblems = require('./util/check_callback_shapes')(defs);
if (callbackShapeProblems.length) {
    console.log(`   ⚠️  ${callbackShapeProblems.length} callback shape problem(s):`);
    callbackShapeProblems.forEach((p) => {
        console.log(`      ${p.def}.${p.member}${p.static ? ' [static]' : ''}: overload ${p.bareOverload} has a bare Function at slot ${p.slot}, which shadows the shaped callback of overload ${p.shapedOverload}; move the shaped overloads before the bare fallback (${p.bare} vs ${p.shaped})`);
    });
} else
    console.log('✅ No callback shape shadowing');

console.log('\n🔗 Checking union types...');
var unionProblems = require('./util/check_unions')(defs);
if (unionProblems.length) {
    console.log(`   ❌ ${unionProblems.length} union type problem(s):`);
    unionProblems.forEach((p) => {
        console.log(`      ${p.message}`);
    });
} else
    console.log('✅ No union type problems');

// Value/Variant render as `any`, so a parameter declared with them gives up the
// strictness a union would give. Every such position is a reviewed decision:
// the whitelist carries the ones that keep the generic type, each with its
// reason (plans/value-variant-union-2026-10-05.md).
console.log('\n🔎 Checking Value/Variant parameters...');
var variantProblems = require('./util/check_variant_params')(defs);
if (variantProblems.length) {
    console.log(`   ❌ ${variantProblems.length} Value/Variant parameter problem(s):`);
    variantProblems.forEach((p) => {
        if (p.kind === 'unlisted')
            console.log(`      ${p.def}.${p.member}${p.static ? ' [static]' : ''}, parameter ${p.slot + 1} (${p.param}): declared (${p.type}) and not in the whitelist`);
        else
            console.log(`      whitelist entry ${p.def}${p.member ? '.' + p.member : ''}${p.param ? '.' + p.param : ''} matches nothing (stale)`);
    });
} else
    console.log('✅ No unlisted Value/Variant parameters');

// The documentation checker runs before the generation: a definition that is
// being changed must be fully documented (X1-X10, plans/idl-doc-completion-
// plan-2026-10-05.md 7.1), so the generated manual never regresses.
console.log('\n🔎 Checking documentation (X1-X10)...');
var docProblems = require('./util/check_idl_docs')(defs, {
    baseline: path.resolve(__dirname, 'util/idl_docs_baseline.json'),
    changed: true,
    report: path.resolve(__dirname, '../temp/idl_examples_report.json')
});
if (docProblems.length) {
    console.log(`   ❌ ${docProblems.length} documentation problem(s):`);
    docProblems.slice(0, 50).forEach((p) => {
        console.log(`      [${p.rule}] ${p.def}${p.member ? '.' + p.member : ''}: ${p.message}`);
    });
    if (docProblems.length > 50)
        console.log(`      ... and ${docProblems.length - 50} more (see tools/util/check_idl_docs.js)`);
    process.exit(1);
} else
    console.log('✅ No documentation problems');

console.log('\n💾 Recording IDL JSON...');
record_idljson(defs);
console.log('✅ IDL JSON recorded');

console.log('\n⚡ Generating C++ code...');
gen_code(defs, baseCodeFolder);
console.log('✅ C++ code generated');

console.log('\n📝 Generating TypeScript definitions...');
const DTS_DIST_DIR = path.resolve(__dirname, `../npm/types/dts/`);
require('fs').mkdirSync(DTS_DIST_DIR, { recursive: true });
gen_dts(parser(idlFolder), { DTS_DIST_DIR });
console.log('✅ TypeScript definitions generated');

// The `--check` built-in types and the `--man` pages are two views of the
// corpus above: the map the runtime embeds is regenerated here and committed
// together with the corpus.
console.log('\n📚 Generating the built-in types for --check/--man...');
var builtin_stats = gen_builtin_types.generate(path.resolve(__dirname, '../npm/types'),
    path.resolve(__dirname, '../fibjs/scripts/internal/fibjs-types.js'));
console.log(`✅ built-in types generated (${builtin_stats.files} files, ${builtin_stats.globals} globals)`);

// The check runs before the generation, so the corpus is compiled and the
// generated files stay consistent; failing afterwards keeps the failure loud
// without leaving stale headers behind.
if (shadowed.length)
    throw new Error(`${shadowed.length} overload shadowing problem(s), listed above. A String
parameter declared before a structured type swallows the values that overload
expects (the lenient string conversion v->ToString() accepts everything once the
strict pass fails). Move the String variants to the end of each member.`);

if (unionProblems.length)
    throw new Error(`${unionProblems.length} union type problem(s), listed above. A union is
the runtime conversion's preference order, and an alternative that converts a
value another one after it is meant to take makes that one unreachable. The
narrowest conversion goes first: Buffer < String < Integer/Long/Number < Date <
Boolean, with Value/Variant last (plans/idl-union-types-2026-10-02.md).`);

if (variantProblems.length)
    throw new Error(`${variantProblems.length} Value/Variant parameter problem(s), listed above. A
parameter declared Value/Variant renders as \`any\` and loses the -check strictness;
decide the site under the standard of plans/value-variant-union-2026-10-05.md:
union-ize it (with the call-form tests the union's runtime acceptance needs), or
add the whitelist entry carrying the reason (tools/util/variant_params_whitelist.json).`);

console.log('\n🎉 IDL compilation completed successfully!');

module.exports = defs;
