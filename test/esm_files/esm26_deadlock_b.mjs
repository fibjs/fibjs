// B: exports something, fires a non-awaited dynamic import of C during evaluation
export const name = "B";
import("./esm26_deadlock_c.mjs").then(c => {});
