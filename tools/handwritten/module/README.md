// Hand-written module declarations for the Node compatibility layer
//
// One file per module, named after the IDL unit (`events.d.ts`,
// `buffer.d.ts`, ...). tools/util/gen_dts.js appends each file to
// `npm/types/dts/module/<unit>.d.ts` after the generated declarations:
//
//   - when the unit has an IDL definition (`net`, `http`, `util`, ...), the
//     file adds a second `declare module '<runtime name>' { ... }` block to the
//     same file. TypeScript merges ambient module declarations across blocks,
//     so the module keeps its generated members and gains these;
//   - when the module has no IDL at all (everything the pure-JS compatibility
//     layer provides: `events`, `buffer`, `stream`, `https`, `readline`,
//     `stream/promises`, `stream/web`, `timers/promises`,
//     `diagnostics_channel`), the file *is* the corpus file and the module
//     enters the corpus through it.
//
// The module name inside the file is the *runtime* name, not the unit name:
// `path_posix.d.ts` (IDL unit) declares `path/posix`, `types.d.ts` declares
// `util/types`, `stream_web.d.ts` declares `stream/web`. The mapping lives in
// tools/gen_dts.js (MODULE_RUNTIME_NAMES) and tools/gen_builtin_types.js
// derives the `fibjs:` / `node:` aliases from the declared name.
//
// Rules for these files:
//
//   - `/// <reference path="../_import/_fibjs.d.ts" />` is required when the
//     text refers to `FIBJS.GeneralObject` or any `Class_*` name; the generated
//     part of the file usually carries it already, but a module with no IDL
//     unit starts from nothing;
//   - prefer referring to the existing corpus classes (`Class_Buffer`,
//     `Class_EventEmitter`, `Class_Stream`, ...) over re-declaring them;
//   - keep the declared surface to what the runtime really provides - a
//     declaration of something the runtime lacks turns a runtime error into a
//     wrong compile, which is worse (see plans/fibjs-upstream-gaps.md B4);
//   - the corpus is type-checked by tools/check_dts.js, so a declaration has
//     to compile under `strict`.
