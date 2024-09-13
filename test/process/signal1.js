var cnt = 0;

process.on('SIGINT', () => {
    console.log("SIGINT received");
    if (cnt++ > 0)
        process.exit(0);
});

while (true);
