var path = require('path');
var assert = require('assert');

run("buffer.js");
run("path.js");
run("fiber.js");
run("fs.js");
run("os.js");
run("encoding.js");

run('BUG_simple_api_call.js');
console.log("TEST End.");
