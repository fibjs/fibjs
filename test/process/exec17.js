process.exitCode = 101;

process.on('exit', exitCode => {
    console.log('exit', exitCode);
});

var once = true;

process.on('exit', exitCode => {
    console.log('other exit', exitCode);

    if (once) {
        once = false;
        setTimeout(() => {
            console.log("new work", exitCode);
        }, 10);
    }
});