process.send('ready');

process.on('disconnect', () => {
    process.exit(0);
});

setInterval(() => {}, 1000);