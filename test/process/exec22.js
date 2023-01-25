var o = process.cpuUsage();
o.rss = process.memoryUsage().rss;
process.stdout.write(JSON.stringify(o));

setTimeout(() => { }, 100000000);