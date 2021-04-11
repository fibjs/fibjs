console.log(`by default, process.stdin.isRaw is ${process.stdin.isRaw}`);

process.stdin.setRawMode(true);

console.log(`then, process.stdin.isRaw is ${process.stdin.isRaw}`);

process.stdin.setRawMode(false);

console.log(`finally, process.stdin.isRaw is ${process.stdin.isRaw}`);