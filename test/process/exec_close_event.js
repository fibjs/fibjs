// Test file for close event testing
console.log("close event test process started");
setTimeout(() => {
    console.log("process ending normally");
    process.exit(42);
}, 100);
