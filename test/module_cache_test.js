var { describe, it, after } = require('node:test');
var assert = require('assert');

var fs = require('fs');
var path = require('path');

// 场景复刻：同一进程内「探缺 → 落盘/安装 → 复探」必须看到新的文件系统事实。
// 修复前：负结果（-1 / 读取失败）被写入 isolate 级 LRU（stat 300s / file 30s），
// 复探仍报缺失。首次探测是真实缺失场景，错误形态为 fibjs 现行形状：
//   require.resolve / require → code ENOENT；import() → ERR_MODULE_NOT_FOUND
var FIXTURE = path.join(__dirname, 'module', 'late_cache');
var FIXTURE_SPEC = './module/late_cache';

function cleanup() {
    fs.rmSync(FIXTURE, { recursive: true, force: true });
}

function installCjsPackage() {
    var pkgDir = path.join(FIXTURE, 'node_modules', 'late-pkg-cjs');
    fs.mkdirSync(path.join(pkgDir, 'lib'), { recursive: true });
    fs.writeFileSync(path.join(pkgDir, 'package.json'),
        JSON.stringify({ name: 'late-pkg-cjs', version: '1.0.0', main: './lib/entry.js' }));
    fs.writeFileSync(path.join(pkgDir, 'lib', 'entry.js'), 'module.exports = "cjs-ok";');
    return pkgDir;
}

function installEsmPackage() {
    var pkgDir = path.join(FIXTURE, 'node_modules', 'late-pkg-esm');
    fs.mkdirSync(pkgDir, { recursive: true });
    fs.writeFileSync(path.join(pkgDir, 'package.json'),
        JSON.stringify({ name: 'late-pkg-esm', version: '1.0.0', type: 'module', exports: { '.': './main.mjs' } }));
    fs.writeFileSync(path.join(pkgDir, 'main.mjs'), 'export default "esm-ok";');
    return pkgDir;
}

describe('module cache staleness', () => {
    cleanup();
    after(cleanup);

    it('relative file: probe miss, then create, same-process resolve must see it', () => {
        // 空 node_modules 让后续用例的首探走到 package.json 读取路径
        fs.mkdirSync(path.join(FIXTURE, 'node_modules'), { recursive: true });

        assert.throws(() => require.resolve(FIXTURE_SPEC + '/late_file'), { code: 'ENOENT' });

        var target = path.join(FIXTURE, 'late_file.js');
        fs.writeFileSync(target, 'module.exports = 42;');

        assert.equal(require.resolve(FIXTURE_SPEC + '/late_file'), target);
        assert.equal(require(FIXTURE_SPEC + '/late_file'), 42);
    });

    it('installed CJS package: main outside index.js must be honored in same process', () => {
        fs.writeFileSync(path.join(FIXTURE, 'probe.js'),
            'module.exports = {' +
            '  resolveCjs: () => require.resolve("late-pkg-cjs"),' +
            '  loadCjs: () => require("late-pkg-cjs"),' +
            '  importEsm: () => import("late-pkg-esm")' +
            '};');
        var probe = require(FIXTURE_SPEC + '/probe.js');

        // 首探：node_modules 已存在但包不存在 → stat 与 package.json 读取的负结果都会被缓存
        assert.throws(() => probe.resolveCjs(), { code: 'ENOENT' });

        var pkgDir = installCjsPackage();

        assert.equal(probe.resolveCjs(), path.join(pkgDir, 'lib', 'entry.js'));
        assert.equal(probe.loadCjs(), 'cjs-ok');
    });

    it('installed ESM package: type/exports must be honored in same process', async () => {
        var probe = require(FIXTURE_SPEC + '/probe.js');

        var err = null;
        try { await probe.importEsm(); } catch (e) { err = e; }
        assert.ok(err, 'first import must fail');
        assert.equal(err.code, 'ERR_MODULE_NOT_FOUND');

        installEsmPackage();

        var m = await probe.importEsm();
        assert.equal(m.default, 'esm-ok');
    });
});
