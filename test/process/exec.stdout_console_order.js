// Test ordering between console.log and process.stdout.write
// Each pair (console-N, stdout-N) must appear in that exact order
for (var i = 0; i < 20; i++) {
    console.log('console-' + i);
    process.stdout.write('stdout-' + i + '\n');
}
