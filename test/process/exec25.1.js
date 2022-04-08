if (process.send === undefined)
    process.exit(2);
if (process.connected !== true)
    process.exit(2);
if (process.disconnect === undefined)
    process.exit(2);

process.on("message", () => {
    process.disconnect();

    if (process.connected !== false)
        process.exit(2);

    process.exit(1);
});

