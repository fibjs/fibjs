// CJS package with "type": "commonjs" in package.json.
// Throws a distinctive runtime TypeError so tests can verify the real
// error propagates through ESM import.
throw new TypeError("the REAL runtime error from cjs_runtime_error_with_type");
