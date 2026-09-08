// fork({silent}) 泄漏监控 harness（由官方测试以 spawnSync 运行，fibjs 解释执行）
//
// 职责：以 silent:true fork 子进程 exec_silent.js，把从管道捕获到的
// stdout/stderr 内容以 base64 形式上报（base64 是为了保证子进程输出的
// 原文绝不会以明文出现在本 harness 的输出里——否则外层测试无法区分
// "管道内容"与"泄漏到父进程 fd 的输出"）。
//
// 约定：
//   stdout 输出恰好两行：HARNESS-OUT:<base64>、HARNESS-ERR:<base64>
//   stderr 必须为空
//   退出码：0 = 管道齐全且内容已上报；2 = silent 失效（stdin/stdout/stderr 缺失）；3 = 看门狗超时
var cp = require('child_process');
var path = require('path');

var CHILD = path.join(__dirname, 'exec_silent.js');
var p = cp.fork(CHILD, [], { silent: true });

var rec = { out: '', err: '' };

function report(code) {
    process.stdout.write('HARNESS-OUT:' + Buffer.from(rec.out, 'utf8').toString('base64') + '\n');
    process.stdout.write('HARNESS-ERR:' + Buffer.from(rec.err, 'utf8').toString('base64') + '\n');
    process.exit(code);
}

// 看门狗：防止意外挂死拖垮整个测试套件
setTimeout(function() {
    report(3);
}, 5000);

if (!p.stdout || !p.stderr || !p.stdin) {
    // silent 失效：子进程输出会泄漏到我们的 stdout/stderr，
    // 先等子进程退出把泄漏行打全，再以 2 退出（外层会同时校验 stderr 为空）
    p.on('close', function() {
        setTimeout(function() {
            report(2);
        }, 150);
    });
} else {
    p.stdout.on('data', function(c) { rec.out += c; });
    p.stderr.on('data', function(c) { rec.err += c; });

    p.on('close', function(code) {
        // 等数据事件收尾再上报
        setTimeout(function() {
            report(code === 0 ? 0 : 2);
        }, 150);
    });
}
