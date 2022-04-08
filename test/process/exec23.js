process.exitCode = 11;

process.on("message", () => {
    process.removeAllListeners("message");
    process.exitCode = 12;
});
