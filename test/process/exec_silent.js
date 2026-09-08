// fork({silent}) fixture: 向 stdout/stderr 输出固定行，
// 100ms 后经 IPC 上报 "ready"，300ms 后自行退出。
// 设 CP_SILENT_QUIET=1 时不打印（供 inherit 模式的用例避免污染套件日志，
// 那些用例只关心流形状，不关心输出内容）。
var quiet = process.env.CP_SILENT_QUIET === '1';

if (!quiet) {
    console.log("silent-out-1");
    console.log("silent-out-2");
    console.error("silent-err-1");
    console.error("silent-err-2");
}

setTimeout(function() {
    if (process.send)
        process.send("ready");
}, 100);

setTimeout(function() {
    process.exit(0);
}, 300);
