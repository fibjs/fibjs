// Hand-written type declarations for the Node compatibility layer
//
// A declaration lives next to what it declares, in one of two places:
//
//   - `fibjs/scripts/**/<name>.d.ts` — a module implemented in plain JS keeps
//     its declaration beside the implementation (`stream.d.ts` next to
//     `stream.js`, `stream/web.d.ts` next to `stream/web.js`,
//     `timers/promises.d.ts` next to `timers/promises.js`). The file name
//     carries no unit name: the unit comes from the
//     `declare module '<runtime name>'` line that must start the file, mapped
//     back through MODULE_RUNTIME_NAMES (`stream/web` -> `stream_web`).
//
//   - `idl/types/<unit>.d.ts` — everything else: a declaration merged into a
//     module that has an IDL unit (`net` adding `AddressInfo`, `buffer`), a
//     module with no JS of its own (`events`, `https`), or an interface-level
//     declaration the IDL cannot express (`Buffer`, declared as
//     `declare class Class_Buffer extends Uint8Array`).
//
// tools/util/gen_dts.js collects both places and appends the text to
// `npm/types/dts/module/<unit>.d.ts` after the generated declarations:
//
//   - when the unit has an IDL definition (`net`, `http`, `util`, ...), the
//     file adds a second `declare module '<runtime name>' { ... }` block to the
//     same file. TypeScript merges ambient module declarations across blocks,
//     so the module keeps its generated members and gains these;
//   - when the module has no IDL at all (everything the pure-JS compatibility
//     layer provides: `events`, `buffer`, `stream`, `https`, `readline`,
//     `readline/promises`, `stream/promises`, `stream/web`, `timers/promises`,
//     `diagnostics_channel`), the file *is* the corpus file and the module
//     enters the corpus through it.
//
// The corpus file name is always `<unit>.d.ts` under `dts/module/`, regardless
// of where the source lives: tools/gen_builtin_types.js derives the `fibjs:` /
// `node:` aliases from that shape. The module name *inside* the file is the
// runtime name, not the unit name — `path_posix.d.ts` declares `path/posix`,
// `types.d.ts` declares `util/types`. The mapping lives in
// tools/util/gen_dts.js (MODULE_RUNTIME_NAMES).

Rules for these files:

  - `/// <reference path="../_import/_fibjs.d.ts" />` is required when the
    text refers to `FIBJS.GeneralObject` or any `Class_*` name; the generated
    part of the file usually carries it already, but a module with no IDL unit
    starts from nothing;
  - for a co-located file the reference path is relative to its own location
    (`fibjs/scripts/stream.d.ts` needs
    `/// <reference path="../../npm/types/dts/_import/_fibjs.d.ts" />` only if
    the final corpus file does not already have it — normally the merged file
    does, so co-located files omit it);
  - prefer referring to the existing corpus classes (`Class_Buffer`,
    `Class_EventEmitter`, `Class_Stream`, ...) over re-declaring them;
  - keep the declared surface to what the runtime really provides — a
    declaration of something the runtime lacks turns a runtime error into a
    wrong compile, which is worse (see plans/fibjs-upstream-gaps.md B4);
  - a co-located `.d.ts` must start with `declare module '<runtime name>'`;
    tools/util/gen_dts.js fails the build otherwise, so a file that was placed
    but cannot be matched never disappears silently;
  - the corpus is type-checked by tools/check_dts.js, so a declaration has to
    compile under `strict`;
  - `fibjs/scripts/**/*.d.ts` is skipped by tools/gen_scripts.js, so a
    declaration never ends up embedded in `fibjs/src/base/opt_tools.cpp`.
