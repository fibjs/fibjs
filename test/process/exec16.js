process.exitCode = 101;

process.on('beforeExit', exitCode => {
    console.log('beforeExit', exitCode);
});

var once = true;

process.on('beforeExit', exitCode => {
    console.log('other beforeExit', exitCode);

    if (once) {
        once = false;
        setTimeout(() => {
            console.log("new work", exitCode);
        }, 10);
    }
});

process.exit();