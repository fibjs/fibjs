# P0 type checks

These fixtures cover the P0 work only: the npm package entry, globals, `node:`
specifiers, and the `.promises` module graph. They deliberately do **not** use
P1+ names such as `fs.FSWatcher`, `crypto.Hash`, or `NodeJS.*`.

## Local corpus (no install)

```sh
TSC=/path/to/tsc sh test/types/node-parity/p0/run-p0.sh
```

The tsconfigs use `typeRoots: ["../../../../npm"]` and `types: ["types"]`, so
they compile against the repository corpus directly. `package-globals.ts`
covers `require`/`Buffer`/`URL`/`setTimeout`/`module`/`exports`/`__filename`;
`package-node-prefix.ts` covers `node:fs`, `node:fs/promises`,
`node:stream`, `node:timers/promises`, and `node:assert/strict`;
`package-promises.ts` covers every `.promises` module.

`tsconfig.index.json` compiles `npm/types/index.d.ts` with
`skipLibCheck: false`. The corpus has 4 pre-existing `TS2416` diagnostics
(`HttpResponse`, `RangeStream`); the runner only fails if `index.d.ts` itself
reports a diagnostic.

## Packed package

```sh
cd npm/types
npm pack --json
# install the tarball into a scratch project and point typeRoots at
# node_modules (types: ["@fibjs/types"])
```

The packed-tarball test is the one that catches missing `_builtin`/`.promises`
files; the local corpus config cannot. It is kept as a manual/CI step until a
pinned tsc is available in the repository tooling.
