function log_stdout(input = "") {
  process.stdout.write(input + `\n`);
}
log_stdout("exec testing....");

var coroutine = require("coroutine");

process.on("SIGTERM", () => {
  log_stdout("killed");
  process.exit(0);
});

for (var i = 0; i < 2; i++) {
  coroutine.sleep(1000);
  log_stdout(new Date());
  console.print("console.print....");
  log_stdout();
}

process.exit(2);
log_stdout("not output");
