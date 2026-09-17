//
// Worker interrupt 压力循环测试
//
// 只运行**会触发硬中断**的用例：worker.terminate() → Worker::requestTerminate()
// → v8::Isolate::RequestInterrupt() + TerminateExecution()。
//
// ── 用法 ──────────────────────────────────────────────────────────────────────
//   fibjs test/loop.js                       # 无限循环，直到某轮失败
//   fibjs test/loop.js 50                    # 最多跑 50 轮
//   fibjs test/loop.js 50 <pattern>          # 自定义 --test-name-pattern
//   fibjs test/loop.js 50 <pattern> <file>   # 自定义目标测试文件
//
// ── 失败判定 ─────────────────────────────────────────────────────────────────
//   空闲超时（说明有 terminate() 没回来、用例挂住了 —— 正是压力测试要找的问题）/
//   退出码非 0 / 被信号杀死 / 输出里有 "tests failed" / 输出里有失败用例（×）。
//   失败时把该轮完整输出写到 temp/loop_fail.log 并抛出。
//
const child_process = require('child_process');
const fs = require('fs');
const path = require('path');
const coroutine = require('coroutine');

const logFile = path.join(__dirname, '..', 'temp', 'loop_fail.log');

// 会触发硬中断的用例都必须调用 worker.terminate()，因此用 "terminate" 过滤
// （实测命中 44 个用例 + 1 个 todo；todo 不会被执行）。
const DEFAULT_TARGET = 'worker_threads_fibjs_test.js';
const DEFAULT_PATTERN = 'terminate';

// 下限断言：若过滤条件坏掉（比如 pattern 不再匹配、用例被改名），跑到的用例数会骤降，
// 这时必须**报错而不是假通过** —— 否则压力测试会在“什么都没跑”的情况下永远绿着。
// 新增用例不会让这里失败，只有丢用例才会。
const MIN_PASSED = 53;

// 连续无输出多久判定为挂住（毫秒）。单个用例通常在 500ms 内完成并打出结果行。
const IDLE_TIMEOUT = 30000;

var argv = process.argv.slice(2);
var maxRounds = parseInt(argv[0], 10);
if (!(maxRounds > 0))
    maxRounds = Infinity;
var pattern = argv[1] || DEFAULT_PATTERN;
var target = path.join(__dirname, argv[2] || DEFAULT_TARGET);
var childEnv = Object.assign({}, process.env, { FIBJS_WORKER_HARD_TERMINATE: '1' });

// 把本轮实际使用的模式打出来：读写代码才能确认“测的是哪一支”实在不方便。
// 硬中断是生产默认关闭、此处显式打开的实验特性（见 Worker::requestTerminate）。
console.log(`[loop] worker hard-terminate (interrupt): ${childEnv.FIBJS_WORKER_HARD_TERMINATE === '0' ? 'OFF' : 'ON'}`);
console.log(`[loop] target=${path.basename(target)} pattern=${JSON.stringify(pattern)}`);

// 从 runner 的汇总行（"N tests passed"）里取数，避免把汇总行本身当成用例统计。
function countOf(text, re) {
    var m = text.match(re);
    return m ? parseInt(m[1], 10) : 0;
}

function runOnce() {
    var output = [];
    var ev = new coroutine.Event();
    var exited = false;
    var stdoutClosed = false;
    var stderrClosed = false;
    var killedByIdle = false;

    var child = child_process.spawn(process.execPath, [
        '--test',
        '--test-name-pattern=' + pattern,
        target
    ], {
        env: childEnv
    });

    function checkDone() {
        if (exited && stdoutClosed && stderrClosed)
            ev.set();
    }

    function armTimer() {
        return setTimeout(() => {
            killedByIdle = true;
            output.push(`\n[loop] no output for ${IDLE_TIMEOUT}ms, killing child ` +
                `(likely a stuck terminate())\n`);
            child.kill('SIGINT');
        }, IDLE_TIMEOUT);
    }

    // 空闲看门狗：interrupt 没能打断时，用例会挂住并且不再产生输出。
    var timer = armTimer();

    function resetTimer() {
        clearTimeout(timer);
        timer = armTimer();
    }

    child.stdout.on('data', d => { output.push(d.toString()); resetTimer(); });
    child.stderr.on('data', d => { output.push(d.toString()); resetTimer(); });
    child.stdout.on('close', () => { stdoutClosed = true; checkDone(); });
    child.stderr.on('close', () => { stderrClosed = true; checkDone(); });
    child.on('exit', () => { exited = true; checkDone(); });

    ev.wait();
    clearTimeout(timer);

    return {
        status: child.exitCode,
        signal: child.signalCode,
        killedByIdle: killedByIdle,
        output: output.join('')
    };
}

function failureReason(ret) {
    var text = ret.output;

    if (ret.killedByIdle)
        return 'idle timeout: child produced no output (a terminate() did not settle?)';
    if (text.indexOf('SIGSEGV') >= 0)
        return 'SIGSEGV';
    if (text.indexOf('SIGABRT') >= 0)
        return 'SIGABRT';
    if (ret.signal)
        return 'killed by signal ' + ret.signal;
    if (text.indexOf('tests failed') >= 0)
        return 'test runner reported failures';
    if (/^\s+×\s/m.test(text))
        return 'at least one test failed';
    if (ret.status !== 0)
        return 'exit code ' + ret.status;
    if (countOf(text, /(\d+) tests passed/) < MIN_PASSED)
        return `only ${countOf(text, /(\d+) tests passed/)} interrupt tests ran ` +
            `(expected at least ${MIN_PASSED}) -- filter broken?`;

    return null;
}

var count = 0;
while (count < maxRounds) {
    count++;
    console.log(`\n========== Round ${count} ==========`);

    var startedAt = Date.now();
    const ret = runOnce();
    var reason = failureReason(ret);

    var passed = countOf(ret.output, /(\d+) tests passed/);
    var todo = countOf(ret.output, /(\d+) todo tests/);
    var seconds = Math.round((Date.now() - startedAt) / 1000);

    if (reason) {
        const header = `\n===== Round ${count} failed: ${reason} ` +
            `(status=${ret.status}, signal=${ret.signal}, idleKill=${ret.killedByIdle}) =====\n`;
        fs.writeFileSync(logFile, header + ret.output);
        console.error(`Round ${count} FAILED: ${reason}`);
        console.error(`  log written to ${logFile}`);
        throw new Error('worker interrupt loop failed: ' + reason);
    }

    console.log(`Round ${count} OK — ${passed} passed, ${todo} todo, ${seconds}s`);
}

