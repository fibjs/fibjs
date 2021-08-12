var o = process.cpuUsage();
o.rss = process.memoryUsage().rss;
console.log(o);

setTimeout(() => { }, 100000000);