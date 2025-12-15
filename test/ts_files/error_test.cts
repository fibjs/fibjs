// CTS file for testing TypeScript source in error messages
const testError = (name: string, age: number): string => { throw new Error("CTS test error"); };

testError("test", 25);
