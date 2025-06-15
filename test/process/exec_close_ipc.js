// Test file for close event with IPC
console.log("IPC close event test started");

// Simple exit after sending a message
process.send('ready');

setTimeout(() => {
    process.exit(0);
}, 50);
