var t = setTimeout(() => {
    process.exitCode = 9;
}, 100);

t.unref();
t.ref();
t.ref();
t.ref();
t.ref();

console.log(302);