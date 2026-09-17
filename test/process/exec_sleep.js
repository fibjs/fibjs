// 长期存活、不主动退出的子进程夹具：
// 用于验证 worker 终止时其 spawn 的子进程一并终止（R5 / D1）。
const coroutine = require('coroutine');

coroutine.sleep(30000);
