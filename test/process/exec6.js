var process = require('process');
var coroutine = require("coroutine");
coroutine.sleep(1000);
process.exit(process.argv[2]);