// Child process: read all stdin synchronously, then echo to stdout
var buf = process.stdin.read(100);
process.stdout.write(buf);
