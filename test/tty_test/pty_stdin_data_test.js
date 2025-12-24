// 测试 stdin data 事件在 pty 模式下工作
// 此脚本将在 pty 子进程中运行

console.log("PTY_STDIN_TEST_START");

var dataReceived = false;

process.stdin.on('data', function(chunk) {
    dataReceived = true;
    var input = chunk.toString();
    console.log("RECEIVED:" + input.trim());
    
    if (input.trim() === 'EXIT') {
        console.log("PTY_STDIN_TEST_EXIT");
        process.exit(0);
    }
});

console.log("PTY_STDIN_LISTENER_READY");

// 进程应该保持运行，等待输入
// 不需要额外的 timer 或 sleep，stdin listener 应该保持进程存活
