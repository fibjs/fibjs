// Inject the payload in a fresh process.  selfzip runs in the same process as
// every other test file, and on 32-bit hosts that process can already be near
// the address-space limit: LIEF's whole-image temporaries then throw
// std::bad_alloc, which escapes the addon as an unhandled C++ exception and
// kills the whole run.  A fresh process has the room for them.
//
// argv (after the script): <target executable> <payload file> <sentinel fuse>
const fs = require('fs');
const { inject } = require('./index.js');

const [target, payload, sentinelFuse] = process.argv.slice(-3);

inject(target, 'APP', fs.readFileSync(payload), { sentinelFuse });
