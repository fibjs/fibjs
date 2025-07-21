// Test file for execSync error case
console.log("execSync stdout before error");
console.error("execSync stderr error message");
process.exitCode = 1;
