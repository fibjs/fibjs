var os = require('os');

function log_err(input = "") {
  process.stderr.write(input + os.EOL);
}
log_err("exec testing....");

var coroutine = require("coroutine");

for (var i = 0; i < 2; i++) {
  coroutine.sleep(1000);
  log_err(new Date());
  console.print("console.print....");
  log_err();
}

process.exit();
log_err("not output");