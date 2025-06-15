// Test process that outputs data then runs for a while
console.log("process started, will run for 5 seconds");

var count = 0;
var timer = setInterval(() => {
    count++;
    console.log("tick " + count);
    
    if (count >= 5) {
        console.log("process ending normally");
        clearInterval(timer);
        process.exit(0);
    }
}, 1000);

// Handle abort signal gracefully
process.on('SIGTERM', () => {
    console.log("received SIGTERM, was aborted");
    clearInterval(timer);
    process.exit(1);
});
