const { describe, it, before, after } = require('node:test');
const assert = require('node:assert');
const fs = require('fs');
const os = require('os');
const path = require('path');
const { once } = require('node:events');
const { pathToFileURL } = require('node:url');

const isFibjs = !!process.versions?.fibjs;
const workerThreads = (() => {
    try {
        return require('worker_threads');
    } catch {
        return null;
    }
})();

const zip = isFibjs ? require('zip') : null;
const v8 = isFibjs ? require('v8') : null;
const coroutine = isFibjs ? require('coroutine') : null;

function sleep(ms) {
    if (isFibjs) {
        coroutine.sleep(ms);
        return;
    }

    Atomics.wait(new Int32Array(new SharedArrayBuffer(4)), 0, 0, ms);
}

function removeFixtureRootSync(targetPath) {
    for (let attempt = 0; attempt < 5; attempt++) {
        try {
            fs.rmSync(targetPath, { recursive: true, force: true });
            return;
        } catch (err) {
            if (!err || (err.code !== 'EBUSY' && err.code !== 'EPERM'))
                throw err;

            if (attempt === 4)
                throw err;

            sleep(50 * (attempt + 1));
        }
    }
}

function toFileURL(filePath) {
    return pathToFileURL(filePath);
}

async function readStream(stream) {
    const chunks = [];

    for await (const chunk of stream)
        chunks.push(Buffer.isBuffer(chunk) ? chunk : Buffer.from(chunk));

    return Buffer.concat(chunks).toString('utf8');
}

describe('file url path-like inputs', () => {
    let fixtureRoot;
    let sourceFile;
    let sourceUrl;
    let nestedDir;
    let nestedDirUrl;
    let nestedFile;
    let nestedFileUrl;
    let symlinkPath;
    let symlinkUrl;

    before(() => {
        fixtureRoot = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-file-url-'));
        sourceFile = path.join(fixtureRoot, 'source.txt');
        nestedDir = path.join(fixtureRoot, 'nested');
        nestedFile = path.join(nestedDir, 'child.txt');
        symlinkPath = path.join(fixtureRoot, 'source.link');

        fs.writeFileSync(sourceFile, 'alpha\n', 'utf8');
        fs.mkdirSync(nestedDir);
        fs.writeFileSync(nestedFile, 'nested\n', 'utf8');
        fs.symlinkSync(sourceFile, symlinkPath);

        sourceUrl = toFileURL(sourceFile);
        nestedDirUrl = toFileURL(nestedDir);
        nestedFileUrl = toFileURL(nestedFile);
        symlinkUrl = toFileURL(symlinkPath);
    });

    after(() => {
        removeFixtureRootSync(fixtureRoot);
    });

    describe('node baseline fs', () => {
        it('accepts file URLs for basic metadata and reads', () => {
            assert.strictEqual(fs.existsSync(sourceUrl), true);
            assert.strictEqual(fs.readFileSync(sourceUrl, 'utf8'), 'alpha\n');
            assert.strictEqual(fs.statSync(sourceUrl).isFile(), true);
            assert.strictEqual(fs.realpathSync(sourceUrl), fs.realpathSync(sourceFile));
            assert.deepStrictEqual(fs.readdirSync(nestedDirUrl), ['child.txt']);
            assert.strictEqual(fs.readlinkSync(symlinkUrl), sourceFile);
        });

        it('accepts file URLs for open, write, append, copy, rename and remove', () => {
            const openFd = fs.openSync(sourceUrl, 'r');
            fs.closeSync(openFd);

            const writeTarget = path.join(fixtureRoot, 'write-target.txt');
            const appendTarget = path.join(fixtureRoot, 'append-target.txt');
            const copyTarget = path.join(fixtureRoot, 'copy-target.txt');
            const renameFrom = path.join(fixtureRoot, 'rename-from.txt');
            const renameTo = path.join(fixtureRoot, 'rename-to.txt');
            const removeTarget = path.join(fixtureRoot, 'remove-target.txt');

            fs.writeFileSync(toFileURL(writeTarget), 'first', 'utf8');
            assert.strictEqual(fs.readFileSync(writeTarget, 'utf8'), 'first');

            fs.writeFileSync(appendTarget, 'A', 'utf8');
            fs.appendFileSync(toFileURL(appendTarget), Buffer.from('B'));
            assert.strictEqual(fs.readFileSync(appendTarget, 'utf8'), 'AB');

            fs.copyFileSync(sourceUrl, toFileURL(copyTarget));
            assert.strictEqual(fs.readFileSync(copyTarget, 'utf8'), 'alpha\n');

            fs.writeFileSync(renameFrom, 'rename', 'utf8');
            fs.renameSync(toFileURL(renameFrom), toFileURL(renameTo));
            assert.strictEqual(fs.readFileSync(renameTo, 'utf8'), 'rename');

            fs.writeFileSync(removeTarget, 'remove', 'utf8');
            fs.rmSync(toFileURL(removeTarget));
            assert.strictEqual(fs.existsSync(removeTarget), false);
        });

        it('accepts file URLs for streams', async () => {
            const streamTarget = path.join(fixtureRoot, 'stream-target.txt');
            let text;

            if (isFibjs) {
                const writer = fs.createWriteStream(toFileURL(streamTarget));
                writer.write(Buffer.from('stream-write'));
                writer.close();

                const reader = fs.createReadStream(toFileURL(streamTarget));
                text = reader.readAll().toString();
                reader.close();
            } else {
                const writer = fs.createWriteStream(toFileURL(streamTarget));
                writer.end('stream-write');
                await once(writer, 'finish');

                text = await readStream(fs.createReadStream(toFileURL(streamTarget)));
            }

            assert.strictEqual(text, 'stream-write');
        });

        it('accepts file URLs for watch and watchFile', () => {
            const watchTarget = path.join(fixtureRoot, 'watch-target.txt');
            const watchTargetUrl = toFileURL(watchTarget);
            fs.writeFileSync(watchTarget, 'watch-1', 'utf8');

            const watcher = fs.watch(watchTargetUrl, () => {});
            watcher.close();

            const listener = () => {};
            fs.watchFile(watchTargetUrl, { interval: 20 }, listener);
            fs.unwatchFile(watchTargetUrl, listener);
        });

        it('accepts file URLs for cpSync when available', () => {
            if (typeof fs.cpSync !== 'function')
                return;

            const sourceDir = path.join(fixtureRoot, 'cp-source');
            const targetDir = path.join(fixtureRoot, 'cp-target');

            fs.mkdirSync(sourceDir);
            fs.writeFileSync(path.join(sourceDir, 'payload.txt'), 'cp-data', 'utf8');
            fs.cpSync(toFileURL(sourceDir), toFileURL(targetDir), { recursive: true });

            assert.strictEqual(fs.readFileSync(path.join(targetDir, 'payload.txt'), 'utf8'), 'cp-data');
        });
    });

    if (workerThreads && workerThreads.Worker && !isFibjs) {
        describe('node baseline worker', () => {
            it('accepts a file URL entry for worker_threads.Worker', async () => {
                const workerFile = path.join(fixtureRoot, 'node-worker.js');
                fs.writeFileSync(workerFile, [
                    "const { parentPort, workerData } = require('node:worker_threads');",
                    'parentPort.postMessage(workerData.value);'
                ].join('\n'), 'utf8');

                const worker = new workerThreads.Worker(toFileURL(workerFile), {
                    workerData: { value: 'worker-ok' }
                });

                const [message] = await once(worker, 'message');
                assert.strictEqual(message, 'worker-ok');
                await worker.terminate();
            });
        });
    }

    if (isFibjs) {
        describe('fibjs-only file:// string entry points', () => {
            it('accepts file:// string in global.run', () => {
                const scriptFile = path.join(fixtureRoot, 'global-run.js');
                fs.writeFileSync(scriptFile, "global.__fileUrlRunMarker = 'ok';", 'utf8');

                delete global.__fileUrlRunMarker;
                global.run(toFileURL(scriptFile).href);

                assert.strictEqual(global.__fileUrlRunMarker, 'ok');
                delete global.__fileUrlRunMarker;
            });

            it('accepts file:// string in process.loadEnvFile', () => {
                const envFile = path.join(fixtureRoot, 'sample.env');
                fs.writeFileSync(envFile, 'FILE_URL_ENV_CASE=loaded\n', 'utf8');

                delete process.env.FILE_URL_ENV_CASE;
                process.loadEnvFile(toFileURL(envFile).href);

                assert.strictEqual(process.env.FILE_URL_ENV_CASE, 'loaded');
                delete process.env.FILE_URL_ENV_CASE;
            });

            it('accepts file:// string in zip.isZipFile and zip.open', () => {
                const zipFile = path.join(fixtureRoot, 'sample.zip');
                const writer = zip.open(zipFile, 'w');
                writer.write(sourceFile, 'payload.txt');
                writer.close();

                assert.strictEqual(zip.isZipFile(toFileURL(zipFile).href), true);

                const reader = zip.open(toFileURL(zipFile).href, 'r');
                assert.deepStrictEqual(reader.namelist(), ['payload.txt']);
                assert.strictEqual(reader.read('payload.txt').toString(), 'alpha\n');
                reader.close();
            });

            it('accepts file:// string in Worker entry path', async () => {
                const workerFile = path.join(fixtureRoot, 'fibjs-worker.js');
                fs.writeFileSync(workerFile, 'global.__workerFileUrlLoaded = true;', 'utf8');

                delete global.__workerFileUrlLoaded;

                await new Promise((resolve, reject) => {
                    const worker = new workerThreads.Worker(toFileURL(workerFile).href);
                    worker.on('online', () => {
                        worker.on('exit', resolve);
                        worker.terminate();
                    });
                    worker.on('error', reject);
                });

                assert.strictEqual(global.__workerFileUrlLoaded, undefined);
            });

            it('accepts file:// string in v8.saveSnapshot and v8.loadSnapshot', () => {
                const snapshotFile = path.join(fixtureRoot, 'heap.heapsnapshot');
                const snapshotUrl = toFileURL(snapshotFile).href;

                v8.saveSnapshot(snapshotUrl);
                assert.strictEqual(fs.existsSync(snapshotFile), true);
                assert.ok(v8.loadSnapshot(snapshotUrl));
            });

            it('accepts file:// string in v8.start', () => {
                const profileFile = path.join(fixtureRoot, 'heap-profile.log');
                const profileUrl = toFileURL(profileFile).href;
                const timer = v8.start(profileUrl, 30, 10);

                coroutine.sleep(80);
                timer.clear();

                assert.strictEqual(fs.existsSync(profileFile), true);
            });
        });
    }
});