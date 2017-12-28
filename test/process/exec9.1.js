var t = setTimeout(() => {
    process.exitCode = 9;
}, 100);

t.unref();
t.unref();
t.unref();
t.unref();

console.log(301);