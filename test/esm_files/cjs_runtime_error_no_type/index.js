// CJS package without "type" field in package.json.
// Throws a distinctive runtime TypeError so tests can verify the real
// error propagates through ESM import (not masked by a stale "type" message).
throw new TypeError("the REAL runtime error from cjs_runtime_error_no_type");
