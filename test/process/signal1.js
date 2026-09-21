var cnt = 0;

process.on('SIGINT', () => {
    console.log("SIGINT received");
    if (cnt++ > 0)
        process.exit(0);
});

// Tell the parent we are ready to receive signals.  A signal delivered before
// the handler is registered kills the process with SIGINT's default action,
// and process startup is not instant (a jitless iOS build needs >1s).
console.log("ready");

while (true);
