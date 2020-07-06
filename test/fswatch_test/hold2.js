const fs = require('fs')
const path = require('path')
const coroutine = require('coroutine')
const uuid = require('uuid')

process.exitCode = 9;
var uid = uuid.snowflake().hex();

const p = path.resolve(__dirname, `../fs_watchfile_output/nogit-hold-${uid}.txt`)
// ensure it existed
fs.writeFile(p, uid);

const watcher = fs.watchFile(p, { interval: 50 }, () => {
    console.log('watched');
    watcher.close();
});

console.log('after start watching')
for (let i = 0; i < 10; i++)
    fs.writeFile(p, `${uid}${Date.now()}`);

setTimeout(() => {
    console.log('closed by timer')
    watcher.close()
}, 5000).unref()