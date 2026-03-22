// C: statically imports B (which is still evaluating) - triggers deadlock in unfixed fibjs
import { name } from "./esm26_deadlock_b.mjs";
export const greeting = "hello from C";
