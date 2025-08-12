#!/usr/bin/env fibjs

// Test script to handle resize signals and output size changes
let lastCols = 0;
let lastRows = 0;

function outputSize() {
    const cols = process.stdout.columns || 80;
    const rows = process.stdout.rows || 24;
    if (cols !== lastCols || rows !== lastRows) {
        console.log(`RESIZE:${cols}x${rows}`);
        lastCols = cols;
        lastRows = rows;
    }
}

// Output initial size
outputSize();

// Listen for resize signals (SIGWINCH)
if (process.platform !== 'win32') {
    process.on('SIGWINCH', () => {
        outputSize();
    });
}

// Check size periodically (for Windows compatibility)
const interval = setInterval(() => {
    outputSize();
}, 50);

// Keep the process alive for testing
setTimeout(() => {
    clearInterval(interval);
    console.log("EXIT");
    process.exit(0);
}, 5000);
