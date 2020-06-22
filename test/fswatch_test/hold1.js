const fs = require('fs')
const path = require('path')
const coroutine = require('coroutine')
const uuid = require('uuid')

process.exitCode = 9;
var uid = uuid.snowflake().hex();

const p = path.resolve(__dirname, `../fswatch_files/nogit-hold-${uid}.txt`)
// ensure it existed
fs.writeFile(p, uid);

const watcher = fs.watch(p, () => {
    console.log('watched');
    watcher.close();
});
console.log('after start watching')
for (let i = 0; i < 10; i++)
    fs.writeFile(p, uid);
