/**
 * @see https://github.com/fibjs/fibjs/issues/620
 * 
 * you can access process.std, and `console.readLine()` normally.
 */
process.stdin
process.exitCode = 1;
console.log(console.readLine());