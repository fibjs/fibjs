function log_err(input = "") {
  process.stderr.write(input + `\n`);
}
log_err("exec testing....");

var coroutine = require("coroutine");

process.on("SIGTERM", () => {
  log_err("killed");
  process.exit(0);
});

for (var i = 0; i < 2; i++) {
  coroutine.sleep(1000);
  log_err(new Date());
  console.print("console.print....");
  log_err();
}

process.exit(2);
log_err("not output");
