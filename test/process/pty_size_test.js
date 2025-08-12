#!/usr/bin/env fibjs

// Test script to output PTY size information
if (process.stdout.isTTY) {
    console.log(`PTY_SIZE:${process.stdout.columns || 80}x${process.stdout.rows || 24}`);
} else {
    console.log("PTY_SIZE:NOT_TTY");
}

// Keep process alive for a short time to allow testing
setTimeout(() => {
    process.exit(0);
}, 100);
