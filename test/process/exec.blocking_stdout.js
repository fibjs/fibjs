/**
 * @see https://github.com/fibjs/fibjs/issues/622
 * 
 * you can use stdio as you thought based on libuv
 */
process.stdout
const len = parseInt(process.env.COUNT_LEN);
Array.apply(null, { length: len }).fill(undefined).forEach((_, idx) => {
    console.log(`${idx + 1}`)
})
