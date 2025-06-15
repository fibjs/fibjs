// Test process that runs indefinitely until killed
console.log("long running process started");

// Keep the process alive
var timer = setInterval(() => {
    console.log("still running...");
}, 1000);

// Clean exit handler
process.on('SIGTERM', () => {
    console.log("received SIGTERM, exiting gracefully");
    clearInterval(timer);
    process.exit(42);
});

process.on('SIGINT', () => {
    console.log("received SIGINT, exiting gracefully");  
    clearInterval(timer);
    process.exit(43);
});
