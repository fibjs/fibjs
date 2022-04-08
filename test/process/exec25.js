if (process.send !== undefined)
    process.exit(2);
if (process.connected !== undefined)
    process.exit(2);
if (process.disconnect !== undefined)
    process.exit(2);

process.exit(1);