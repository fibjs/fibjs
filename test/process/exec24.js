process.on("message", m => {
    process.send(m);
    process.removeAllListeners("message");
});
