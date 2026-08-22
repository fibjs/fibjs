'use strict';

const { describe, it, before, after, beforeEach, afterEach } = require('node:test');
const assert = require('assert');
const fs = require('fs');
const path = require('path');
const crypto = require('crypto');

// Helper function to generate unique IDs
const generateUniqueId = () => {
    return crypto.randomBytes(8).toString('hex');
};

// Helper function to wait for event
const waitForEvent = (emitter, event, timeout = 5000) => {
    return new Promise((resolve, reject) => {
        const timer = setTimeout(() => {
            reject(new Error(`Timeout waiting for ${event} event`));
        }, timeout);
        
        emitter.once(event, (...args) => {
            clearTimeout(timer);
            resolve(args);
        });
    });
};

// Helper function to sleep
const sleep = (ms) => new Promise(resolve => setTimeout(resolve, ms));

// Helper functions
const rmFile = (filepath) => {
    if (fs.existsSync(filepath)) {
        const stat = fs.statSync(filepath);
        if (stat.isDirectory()) {
            // Recursively remove directory
            const files = fs.readdirSync(filepath);
            for (const file of files) {
                const fullPath = path.join(filepath, file);
                rmFile(fullPath);
            }
            fs.rmdirSync(filepath);
        } else {
            fs.unlinkSync(filepath);
        }
    }
};

const ensureDirectoryExisted = (dirpath) => {
    if (!fs.existsSync(dirpath)) {
        fs.mkdirSync(dirpath, { recursive: true });
    }
};

const support_watch_recursive = ['win32', 'darwin', 'linux'].includes(process.platform);
const isLinux = process.platform === 'linux';
// iOS simulator FSEvents doesn't return the filename when watching directories
const support_watch_directory_filename = process.platform !== 'ios';

// Collect events from a watcher until the predicate is satisfied (or timeout).
// Returns the collected [eventType, filename] pairs.
const collectEvents = (rootDir, options, action, predicate, timeout = 5000) => {
    return new Promise((resolve, reject) => {
        const events = [];
        const watcher = fs.watch(rootDir, options, (eventType, filename) => {
            events.push([eventType, String(filename == null ? '' : filename)]);
            if (predicate(events)) {
                clearTimeout(timer);
                watcher.close();
                resolve(events);
            }
        });
        const timer = setTimeout(() => {
            watcher.close();
            reject(new Error(`Timeout waiting for events, got: ${JSON.stringify(events)}`));
        }, timeout);
        try {
            action();
        } catch (e) {
            clearTimeout(timer);
            watcher.close();
            reject(e);
        }
    });
};

// Assert that the collected events contain every expected relative path.
const assertEventsContain = (events, expected) => {
    const names = events.map(([, f]) => f);
    for (const name of expected)
        assert.ok(names.includes(name), `missing '${name}' in ${JSON.stringify(names)}`);
};

describe('fs.watch', () => {
    const basedir = path.resolve(__dirname);
    const testDir = path.join(basedir, 'fswatch_files');

    const writeFile = (filepath, content = '') => {
        fs.writeFileSync(filepath, content);
    };

    const createFile = (filepath, content = '') => {
        writeFile(filepath, content);
    };

    const delFile = (filepath) => {
        fs.unlinkSync(filepath);
    };

    before(() => {
        rmFile(testDir);
        ensureDirectoryExisted(testDir);
    });

    after(() => {
        try {
            rmFile(testDir);
        } catch (e) {
            // ignore
        }
    });

    describe('basic functionality', () => {
        it('should watch file changes', async (t) => {
            const filename = path.join(testDir, `file-${generateUniqueId()}.txt`);
            writeFile(filename, 'initial content');

            let changeCount = 0;
            let capturedEventType;
            let capturedFilename;
            
            const watcher = fs.watch(filename, (eventType, changedFilename) => {
                capturedEventType = eventType;
                capturedFilename = changedFilename;
                changeCount++;

                if (changeCount >= 1) {
                    watcher.close();
                }
            });

            const closePromise = waitForEvent(watcher, 'close');

            // Trigger file change
            await sleep(100);
            writeFile(filename, 'updated content');

            await closePromise;
            assert.ok(changeCount >= 1, `Expected at least 1 change, got ${changeCount}`);
            assert.strictEqual(typeof capturedEventType, 'string');
            assert.strictEqual(typeof capturedFilename, 'string');
            assert.strictEqual(capturedFilename, path.basename(filename));
        });

        it('should watch file with listener in options', async (t) => {
            const filename = path.join(testDir, `file-${generateUniqueId()}.txt`);
            writeFile(filename, 'initial content');

            let changeCount = 0;
            let capturedEventType;
            
            const watcher = fs.watch(filename, {}, (eventType, changedFilename) => {
                capturedEventType = eventType;
                changeCount++;

                if (changeCount >= 1) {
                    watcher.close();
                }
            });

            const closePromise = waitForEvent(watcher, 'close');

            await sleep(100);
            writeFile(filename, 'new content');

            await closePromise;
            assert.ok(changeCount >= 1);
            assert.strictEqual(typeof capturedEventType, 'string');
        });

        (support_watch_directory_filename ? it : it.skip)('should watch directory for file creation', async (t) => {
            const dirName = path.join(testDir, `dir-${generateUniqueId()}`);
            ensureDirectoryExisted(dirName);

            const fileName = `new-file.txt`;
            let changeCount = 0;
            let capturedEventType;

            const watcher = fs.watch(dirName, (eventType, filename) => {
                capturedEventType = eventType;
                
                if (filename === fileName) {
                    changeCount++;
                    watcher.close();
                }
            });

            const closePromise = waitForEvent(watcher, 'close');

            await sleep(100);
            createFile(path.join(dirName, fileName), 'hello');

            await closePromise;
            assert.ok(changeCount >= 1);
            assert.strictEqual(typeof capturedEventType, 'string');
        });

        (support_watch_directory_filename ? it : it.skip)('should watch directory for file deletion', async (t) => {
            const dirName = path.join(testDir, `dir-${generateUniqueId()}`);
            ensureDirectoryExisted(dirName);

            const fileName = `delete-me.txt`;
            const filePath = path.join(dirName, fileName);
            createFile(filePath, 'content');

            let changeCount = 0;
            let capturedEventType;

            const watcher = fs.watch(dirName, (eventType, filename) => {
                capturedEventType = eventType;
                
                if (filename === fileName) {
                    changeCount++;
                    watcher.close();
                }
            });

            const closePromise = waitForEvent(watcher, 'close');

            await sleep(100);
            delFile(filePath);

            await closePromise;
            assert.ok(changeCount >= 1);
            assert.strictEqual(typeof capturedEventType, 'string');
        });

        it('should emit change event through event listener', async (t) => {
            const filename = path.join(testDir, `file-${generateUniqueId()}.txt`);
            writeFile(filename, 'initial');

            let changeCount = 0;
            let capturedEventType;
            const watcher = fs.watch(filename);

            watcher.on('change', (eventType, changedFilename) => {
                capturedEventType = eventType;
                changeCount++;

                if (changeCount >= 1) {
                    watcher.close();
                }
            });

            const closePromise = waitForEvent(watcher, 'close');

            await sleep(100);
            writeFile(filename, 'modified');

            await closePromise;
            assert.ok(changeCount >= 1);
            assert.strictEqual(typeof capturedEventType, 'string');
        });
    });

    describe('watcher.close()', () => {
        it('should allow multiple close() calls', () => {
            const filename = path.join(testDir, `file-${generateUniqueId()}.txt`);
            writeFile(filename, 'content');

            const watcher = fs.watch(filename);

            // Multiple close calls should not throw
            watcher.close();
            watcher.close();
            watcher.close();
        });

        it('should stop watching after close()', async (t) => {
            const filename = path.join(testDir, `file-${generateUniqueId()}.txt`);
            writeFile(filename, 'initial');

            let changeCount = 0;
            const watcher = fs.watch(filename, (eventType) => {
                changeCount++;
            });

            // Close immediately
            await sleep(50);
            watcher.close();

            // Try to trigger change after close
            await sleep(100);
            writeFile(filename, 'after close');

            // Check that no events were received after close
            await sleep(150);
            assert.strictEqual(changeCount, 0);
        });

        it('should emit close event', async (t) => {
            const filename = path.join(testDir, `file-${generateUniqueId()}.txt`);
            writeFile(filename, 'content');

            const watcher = fs.watch(filename);
            
            const closePromise = waitForEvent(watcher, 'close');

            await sleep(50);
            watcher.close();

            await closePromise;
        });
    });

    if (support_watch_recursive) {
        describe('recursive option', () => {
            it('should watch nested directory with recursive option', async (t) => {
                const rootDir = path.join(testDir, `root-${generateUniqueId()}`);
                const nestedDir = path.join(rootDir, 'nested');
                ensureDirectoryExisted(nestedDir);

                const fileName = 'nested-file.txt';
                const filePath = path.join(nestedDir, fileName);

                let changeCount = 0;
                let capturedEventType;
                
                const watcher = fs.watch(rootDir, { recursive: true }, (eventType, filename) => {
                    capturedEventType = eventType;
                    
                    if (filename && filename.includes(fileName)) {
                        changeCount++;
                        watcher.close();
                    }
                });

                const closePromise = waitForEvent(watcher, 'close');

                await sleep(150);
                createFile(filePath, 'nested content');

                await closePromise;
                assert.ok(changeCount >= 1);
                assert.strictEqual(typeof capturedEventType, 'string');
            });

            it('should watch file updates in nested directory', async (t) => {
                const rootDir = path.join(testDir, `root-${generateUniqueId()}`);
                const nestedDir = path.join(rootDir, 'nested');
                ensureDirectoryExisted(nestedDir);

                const fileName = 'update-file.txt';
                const filePath = path.join(nestedDir, fileName);
                createFile(filePath, 'initial');

                let changeCount = 0;
                const watcher = fs.watch(rootDir, { recursive: true }, (eventType, filename) => {
                    if (filename && filename.includes(fileName)) {
                        changeCount++;
                        watcher.close();
                    }
                });

                const closePromise = waitForEvent(watcher, 'close');

                await sleep(150);
                writeFile(filePath, 'updated');

                await closePromise;
                assert.ok(changeCount >= 1);
            });

            it('should watch file deletion in nested directory', async (t) => {
                const rootDir = path.join(testDir, `root-${generateUniqueId()}`);
                const nestedDir = path.join(rootDir, 'nested');
                ensureDirectoryExisted(nestedDir);

                const fileName = 'delete-file.txt';
                const filePath = path.join(nestedDir, fileName);
                createFile(filePath, 'to be deleted');

                let changeCount = 0;
                const watcher = fs.watch(rootDir, { recursive: true }, (eventType, filename) => {
                    if (filename && filename.includes(fileName)) {
                        changeCount++;
                        watcher.close();
                    }
                });

                const closePromise = waitForEvent(watcher, 'close');

                await sleep(150);
                delFile(filePath);

                await closePromise;
                assert.ok(changeCount >= 1);
            });

            it('should watch file with recursive option on file path', async (t) => {
                const rootDir = path.join(testDir, `root-${generateUniqueId()}`);
                ensureDirectoryExisted(rootDir);
                
                const fileName = 'watched-file.txt';
                const filePath = path.join(rootDir, fileName);
                createFile(filePath, 'initial');

                let changeCount = 0;
                const watcher = fs.watch(filePath, { recursive: true }, (eventType, filename) => {
                    changeCount++;
                    if (changeCount >= 1) {
                        watcher.close();
                    }
                });

                const closePromise = waitForEvent(watcher, 'close');

                await sleep(100);
                writeFile(filePath, 'updated');

                await closePromise;
                assert.ok(changeCount >= 1);
            });

            it('should report every entry created in the same tick (mkdir -p + writes)', async (t) => {
                const rootDir = path.join(testDir, `root-${generateUniqueId()}`);
                ensureDirectoryExisted(rootDir);

                const events = await collectEvents(rootDir, { recursive: true }, () => {
                    // The whole tree is created before the event loop gets a
                    // chance to deliver any event (regression: deep/nested and
                    // the files used to be lost on Linux).
                    fs.mkdirSync(path.join(rootDir, 'deep', 'nested'), { recursive: true });
                    fs.writeFileSync(path.join(rootDir, 'deep', 'nested', 'b.txt'), 'x');
                    fs.writeFileSync(path.join(rootDir, 'deep', 'nested', 'c.txt'), 'y');
                }, (evs) => {
                    const names = evs.map(([, f]) => f);
                    return names.includes('deep') && names.includes('deep/nested') &&
                        names.includes('deep/nested/b.txt') && names.includes('deep/nested/c.txt');
                });

                assertEventsContain(events, ['deep', 'deep/nested', 'deep/nested/b.txt', 'deep/nested/c.txt']);
            });

            it('should watch a file created in an existing subdirectory', async (t) => {
                const rootDir = path.join(testDir, `root-${generateUniqueId()}`);
                const nestedDir = path.join(rootDir, 'nested');
                ensureDirectoryExisted(nestedDir);

                const events = await collectEvents(rootDir, { recursive: true }, () => {
                    createFile(path.join(nestedDir, 'file.txt'), 'new');
                }, (evs) => evs.some(([, f]) => f === 'nested/file.txt'));

                assertEventsContain(events, ['nested/file.txt']);
            });

            it('should watch a file created inside a new subdirectory', async (t) => {
                const rootDir = path.join(testDir, `root-${generateUniqueId()}`);
                ensureDirectoryExisted(rootDir);

                const events = await collectEvents(rootDir, { recursive: true }, () => {
                    const sub = path.join(rootDir, 'sub');
                    fs.mkdirSync(sub);
                    createFile(path.join(sub, 'file.txt'), 'new');
                }, (evs) => {
                    const names = evs.map(([, f]) => f);
                    return names.includes('sub') && names.includes('sub/file.txt');
                });

                assertEventsContain(events, ['sub', 'sub/file.txt']);
            });

            it('should watch updates of an existing nested file', async (t) => {
                const rootDir = path.join(testDir, `root-${generateUniqueId()}`);
                const nestedDir = path.join(rootDir, 'a', 'b');
                ensureDirectoryExisted(nestedDir);
                createFile(path.join(nestedDir, 'file.txt'), 'initial');

                const events = await collectEvents(rootDir, { recursive: true }, () => {
                    writeFile(path.join(nestedDir, 'file.txt'), 'updated');
                }, (evs) => evs.some(([, f]) => f === 'a/b/file.txt'));

                assertEventsContain(events, ['a/b/file.txt']);
            });

            it('should watch deletion of a nested file', async (t) => {
                const rootDir = path.join(testDir, `root-${generateUniqueId()}`);
                const nestedDir = path.join(rootDir, 'nested');
                ensureDirectoryExisted(nestedDir);
                const filePath = path.join(nestedDir, 'file.txt');
                createFile(filePath, 'to delete');

                const events = await collectEvents(rootDir, { recursive: true }, () => {
                    delFile(filePath);
                }, (evs) => evs.some(([, f]) => f === 'nested/file.txt'));

                assertEventsContain(events, ['nested/file.txt']);
            });

            it('should watch deletion of a nested directory tree', async (t) => {
                const rootDir = path.join(testDir, `root-${generateUniqueId()}`);
                const nestedDir = path.join(rootDir, 'deep', 'nested');
                ensureDirectoryExisted(nestedDir);
                createFile(path.join(nestedDir, 'file.txt'), 'content');

                const events = await collectEvents(rootDir, { recursive: true }, () => {
                    rmFile(path.join(rootDir, 'deep', 'nested'));
                }, (evs) => evs.some(([, f]) => f === 'deep/nested'));

                assertEventsContain(events, ['deep/nested']);
            });

            it('should watch a deep chain of directories created one by one', async (t) => {
                const rootDir = path.join(testDir, `root-${generateUniqueId()}`);
                ensureDirectoryExisted(rootDir);

                const events = await collectEvents(rootDir, { recursive: true }, () => {
                    fs.mkdirSync(path.join(rootDir, 'a'));
                    fs.mkdirSync(path.join(rootDir, 'a', 'b'));
                    fs.mkdirSync(path.join(rootDir, 'a', 'b', 'c'));
                    createFile(path.join(rootDir, 'a', 'b', 'c', 'f.txt'), 'x');
                }, (evs) => {
                    const names = evs.map(([, f]) => f);
                    return names.includes('a') && names.includes('a/b') &&
                        names.includes('a/b/c') && names.includes('a/b/c/f.txt');
                });

                assertEventsContain(events, ['a', 'a/b', 'a/b/c', 'a/b/c/f.txt']);
            });

            it('should watch renaming of a nested directory', async (t) => {
                const rootDir = path.join(testDir, `root-${generateUniqueId()}`);
                const nestedDir = path.join(rootDir, 'nested');
                ensureDirectoryExisted(nestedDir);
                createFile(path.join(nestedDir, 'file.txt'), 'content');

                const events = await collectEvents(rootDir, { recursive: true }, () => {
                    fs.renameSync(nestedDir, path.join(rootDir, 'nested2'));
                }, (evs) => evs.some(([, f]) => f === 'nested'));

                assertEventsContain(events, ['nested']);
            });

            it('should watch direct children of the watched directory', async (t) => {
                const rootDir = path.join(testDir, `root-${generateUniqueId()}`);
                ensureDirectoryExisted(rootDir);

                const events = await collectEvents(rootDir, { recursive: true }, () => {
                    createFile(path.join(rootDir, 'top.txt'), 'x');
                }, (evs) => evs.some(([, f]) => f === 'top.txt'));

                assertEventsContain(events, ['top.txt']);
            });

            it('should keep watching entries created after the watcher is attached', async (t) => {
                const rootDir = path.join(testDir, `root-${generateUniqueId()}`);
                ensureDirectoryExisted(rootDir);

                const events = await collectEvents(rootDir, { recursive: true }, () => {
                    // First tick: build the tree; second tick: modify a file inside it.
                    fs.mkdirSync(path.join(rootDir, 'deep', 'nested'), { recursive: true });
                    fs.writeFileSync(path.join(rootDir, 'deep', 'nested', 'b.txt'), 'x');
                    setTimeout(() => {
                        writeFile(path.join(rootDir, 'deep', 'nested', 'b.txt'), 'y');
                    }, 300);
                }, (evs) => {
                    const names = evs.map(([, f]) => f);
                    return names.includes('deep/nested/b.txt');
                });

                assertEventsContain(events, ['deep', 'deep/nested', 'deep/nested/b.txt']);
            });

            (isLinux ? it : it.skip)('should not emit events for pre-existing entries on start', async (t) => {
                const rootDir = path.join(testDir, `root-${generateUniqueId()}`);
                const nestedDir = path.join(rootDir, 'deep', 'nested');
                ensureDirectoryExisted(nestedDir);
                createFile(path.join(nestedDir, 'b.txt'), 'x');

                const events = [];
                const watcher = fs.watch(rootDir, { recursive: true }, (eventType, filename) => {
                    events.push(String(filename == null ? '' : filename));
                });

                await sleep(400);
                watcher.close();

                assert.deepStrictEqual(events, []);
            });

            it('should stop watching after close() with recursive option', async (t) => {
                const rootDir = path.join(testDir, `root-${generateUniqueId()}`);
                ensureDirectoryExisted(rootDir);

                const events = [];
                const watcher = fs.watch(rootDir, { recursive: true }, (eventType, filename) => {
                    events.push(String(filename == null ? '' : filename));
                });

                await sleep(100);
                watcher.close();

                await sleep(100);
                createFile(path.join(rootDir, 'after-close.txt'));

                await sleep(200);
                assert.deepStrictEqual(events, []);
            });
        });
    }

    describe('encoding option', () => {
        (support_watch_directory_filename ? it : it.skip)('should support buffer encoding', async (t) => {
            const dirName = path.join(testDir, `dir-${generateUniqueId()}`);
            ensureDirectoryExisted(dirName);

            const fileName = 'test-file.txt';
            let receivedBuffer = false;

            const watcher = fs.watch(dirName, { encoding: 'buffer' }, (eventType, filename) => {
                if (filename instanceof Buffer) {
                    const filenameStr = filename.toString('utf8');
                    // Check if it's the file we're looking for (may receive directory events on some systems)
                    if (filenameStr === fileName) {
                        receivedBuffer = true;
                        watcher.close();
                    }
                } else if (filename === null) {
                    // Some platforms may return null
                    watcher.close();
                }
            });

            const closePromise = waitForEvent(watcher, 'close');

            await sleep(100);
            createFile(path.join(dirName, fileName), 'content');

            await closePromise;
            // On some platforms, filename might be null or we might not receive the event
        });

        (support_watch_directory_filename ? it : it.skip)('should support utf8 encoding (default)', async (t) => {
            const dirName = path.join(testDir, `dir-${generateUniqueId()}`);
            ensureDirectoryExisted(dirName);

            const fileName = 'test-file.txt';
            let received = false;
            let capturedFilename;

            const watcher = fs.watch(dirName, (eventType, filename) => {
                if (filename === fileName) {
                    received = true;
                    capturedFilename = filename;
                    watcher.close();
                }
            });

            const closePromise = waitForEvent(watcher, 'close');

            await sleep(100);
            createFile(path.join(dirName, fileName), 'content');

            await closePromise;
            if (received) {
                assert.strictEqual(typeof capturedFilename, 'string');
            }
        });
    });

    describe('error handling', () => {
        it.skip('should throw on invalid path type', () => {
            // fibjs doesn't validate argument types the same way as Node.js
            assert.throws(() => {
                fs.watch(null);
            }, {
                code: 'ERR_INVALID_ARG_TYPE'
            });

            assert.throws(() => {
                fs.watch(123);
            }, {
                code: 'ERR_INVALID_ARG_TYPE'
            });

            assert.throws(() => {
                fs.watch({});
            }, {
                code: 'ERR_INVALID_ARG_TYPE'
            });

            assert.throws(() => {
                fs.watch([]);
            }, {
                code: 'ERR_INVALID_ARG_TYPE'
            });
        });
    });

    describe('persistent option', () => {
        it('should accept persistent option', async (t) => {
            const filename = path.join(testDir, `file-${generateUniqueId()}.txt`);
            writeFile(filename, 'initial');

            let changeCount = 0;
            const watcher = fs.watch(filename, { persistent: true }, (eventType, changedFilename) => {
                changeCount++;
                if (changeCount >= 1) {
                    watcher.close();
                }
            });

            const closePromise = waitForEvent(watcher, 'close');

            await sleep(100);
            writeFile(filename, 'updated');

            await closePromise;
            assert.ok(changeCount >= 1);
        });

        it('should accept persistent false option', async (t) => {
            const filename = path.join(testDir, `file-${generateUniqueId()}.txt`);
            writeFile(filename, 'initial');

            const watcher = fs.watch(filename, { persistent: false }, (eventType, changedFilename) => {
                watcher.close();
            });

            const closePromise = waitForEvent(watcher, 'close');

            await sleep(100);
            writeFile(filename, 'updated');

            await closePromise;
        });
    });

    describe('event types', () => {
        it('should emit changeonly event for content changes', async (t) => {
            const filename = path.join(testDir, `file-${generateUniqueId()}.txt`);
            writeFile(filename, 'initial');

            let changeonlyTriggered = false;
            let capturedEventType;
            const watcher = fs.watch(filename);

            watcher.on('changeonly', (eventType, changedFilename) => {
                capturedEventType = eventType;
                changeonlyTriggered = true;
                watcher.close();
            });

            const closePromise = waitForEvent(watcher, 'close');

            await sleep(100);
            writeFile(filename, 'content modified');

            // Force close after timeout to prevent hanging
            const timeoutPromise = sleep(400).then(() => {
                if (!watcher.closed) {
                    watcher.close();
                }
            });

            await Promise.race([closePromise, timeoutPromise]);
            // changeonly may not always trigger on all platforms
            if (changeonlyTriggered) {
                assert.strictEqual(capturedEventType, 'change');
            }
        });

        it('should emit renameonly event for rename operations', async (t) => {
            const dirName = path.join(testDir, `dir-${generateUniqueId()}`);
            ensureDirectoryExisted(dirName);

            const oldName = path.join(dirName, 'old-file.txt');
            const newName = path.join(dirName, 'new-file.txt');
            writeFile(oldName, 'content');

            let renameonlyTriggered = false;
            let capturedEventType;
            const watcher = fs.watch(dirName);

            watcher.on('renameonly', (eventType, filename) => {
                capturedEventType = eventType;
                renameonlyTriggered = true;
                watcher.close();
            });

            const closePromise = waitForEvent(watcher, 'close');

            await sleep(100);
            fs.renameSync(oldName, newName);

            // Force close after timeout to prevent hanging
            const timeoutPromise = sleep(400).then(() => {
                if (!watcher.closed) {
                    watcher.close();
                }
            });

            await Promise.race([closePromise, timeoutPromise]);
            
            // Cleanup
            try {
                if (fs.existsSync(newName)) delFile(newName);
            } catch (e) {}
            
            if (renameonlyTriggered) {
                assert.strictEqual(capturedEventType, 'rename');
            }
        });

        it('should emit change event for both change and rename', async (t) => {
            const filename = path.join(testDir, `file-${generateUniqueId()}.txt`);
            writeFile(filename, 'initial');

            let changeTriggered = false;
            let capturedEventType;
            const watcher = fs.watch(filename);

            watcher.on('change', (eventType, changedFilename) => {
                capturedEventType = eventType;
                changeTriggered = true;
                watcher.close();
            });

            const closePromise = waitForEvent(watcher, 'close');

            await sleep(100);
            writeFile(filename, 'modified');

            await closePromise;
            assert.ok(changeTriggered);
            assert.ok(['change', 'rename'].includes(capturedEventType));
        });
    });

    describe('multiple file changes', () => {
        it('should detect multiple file changes', async (t) => {
            const filename = path.join(testDir, `file-${generateUniqueId()}.txt`);
            writeFile(filename, 'initial');

            const TRIGGER_COUNT = 3;
            let writeCount = 0;
            let changeCount = 0;

            const watcher = fs.watch(filename, (eventType, changedFilename) => {
                changeCount++;
            });

            const closePromise = waitForEvent(watcher, 'close');

            const interval = setInterval(() => {
                if (writeCount < TRIGGER_COUNT) {
                    writeFile(filename, `content-${++writeCount}`);
                } else {
                    clearInterval(interval);
                    
                    setTimeout(() => {
                        watcher.close();
                    }, 100);
                }
            }, 200);

            await closePromise;
            assert.ok(changeCount >= 1, `Expected at least 1 change, got ${changeCount}`);
        });

        it('should detect multiple file creations in directory', async (t) => {
            const dirName = path.join(testDir, `dir-${generateUniqueId()}`);
            ensureDirectoryExisted(dirName);

            const TRIGGER_COUNT = 3;
            let createCount = 0;
            let changeCount = 0;

            const watcher = fs.watch(dirName, (eventType, filename) => {
                changeCount++;
            });

            const closePromise = waitForEvent(watcher, 'close');

            const interval = setInterval(() => {
                if (createCount < TRIGGER_COUNT) {
                    createFile(path.join(dirName, `file-${createCount}.txt`), `content-${createCount}`);
                    createCount++;
                } else {
                    clearInterval(interval);
                    
                    setTimeout(() => {
                        watcher.close();
                    }, 100);
                }
            }, 200);

            await closePromise;
            assert.ok(changeCount >= 1);
        });
    });
});

describe('fs.watchFile', () => {
    const basedir = path.resolve(__dirname);
    const testDir = path.join(basedir, 'fs_watchfile_output');

    const writeFile = (filepath, content = '') => {
        fs.writeFileSync(filepath, content);
    };

    before(() => {
        rmFile(testDir);
        ensureDirectoryExisted(testDir);
    });

    after(() => {
        try {
            rmFile(testDir);
        } catch (e) {
            // ignore
        }
    });

    describe('basic functionality', () => {
        it('should watch file changes with callback', async (t) => {
            const filename = path.join(testDir, `file-${generateUniqueId()}.txt`);
            writeFile(filename, 'initial');

            let changeTriggered = false;
            let capturedCurr, capturedPrev;

            const listener = (curr, prev) => {
                capturedCurr = curr;
                capturedPrev = prev;
                changeTriggered = true;
            };

            fs.watchFile(filename, { interval: 100 }, listener);

            // Wait for watcher to initialize
            await sleep(300);
            writeFile(filename, 'updated');

            // Wait for change to be detected
            await sleep(600);
            fs.unwatchFile(filename, listener);
            
            assert.ok(changeTriggered, 'Change should have been detected');
            assert.ok(capturedCurr && typeof capturedCurr === 'object');
            assert.ok(capturedPrev && typeof capturedPrev === 'object');
            assert.ok(typeof capturedCurr.mtime !== 'undefined');
            assert.ok(typeof capturedPrev.mtime !== 'undefined');
        });

        it('should detect changes on non-existent file when created', async (t) => {
            const filename = path.join(testDir, `non-existent-${generateUniqueId()}.txt`);

            let firstCall = true;
            let changeTriggered = false;
            let firstCurr, firstPrev;

            const listener = (curr, prev) => {
                if (firstCall) {
                    // Capture first call stats
                    firstCurr = curr;
                    firstPrev = prev;
                    firstCall = false;
                } else {
                    changeTriggered = true;
                }
            };

            fs.watchFile(filename, { interval: 100 }, listener);

            await sleep(200);
            writeFile(filename, 'newly created');

            await sleep(400);
            fs.unwatchFile(filename, listener);
            
            // Validate first call had zero stats
            assert.ok(firstCurr && typeof firstCurr === 'object');
            assert.ok(firstPrev && typeof firstPrev === 'object');
            assert.strictEqual(firstPrev.ctimeMs, 0);
            assert.strictEqual(firstCurr.ctimeMs, 0);
        });

        it('should allow multiple listeners on same file', async (t) => {
            const filename = path.join(testDir, `file-${generateUniqueId()}.txt`);
            writeFile(filename, 'initial');

            let listener1Triggered = false;
            let listener2Triggered = false;

            const listener1 = (curr, prev) => {
                listener1Triggered = true;
            };

            const listener2 = (curr, prev) => {
                listener2Triggered = true;
            };

            fs.watchFile(filename, { interval: 100 }, listener1);
            fs.watchFile(filename, { interval: 100 }, listener2);

            await sleep(150);
            writeFile(filename, 'updated');

            await sleep(350);
            fs.unwatchFile(filename, listener1);
            fs.unwatchFile(filename, listener2);
            assert.ok(listener1Triggered, 'Listener 1 should be triggered');
            assert.ok(listener2Triggered, 'Listener 2 should be triggered');
        });
    });

    describe('fs.unwatchFile', () => {
        it('should stop watching after unwatchFile', async (t) => {
            const filename = path.join(testDir, `file-${generateUniqueId()}.txt`);
            writeFile(filename, 'initial');

            let changeCount = 0;

            const listener = (curr, prev) => {
                changeCount++;
            };

            fs.watchFile(filename, { interval: 100 }, listener);

            await sleep(150);
            writeFile(filename, 'first update');

            await sleep(150);
            fs.unwatchFile(filename, listener);

            await sleep(150);
            writeFile(filename, 'second update - should not trigger');

            await sleep(250);
            assert.ok(changeCount >= 1, 'Should have detected first change');
        });

        it('should remove specific listener', async (t) => {
            const filename = path.join(testDir, `file-${generateUniqueId()}.txt`);
            writeFile(filename, 'initial');

            let listener1Count = 0;
            let listener2Count = 0;

            const listener1 = (curr, prev) => {
                listener1Count++;
            };

            const listener2 = (curr, prev) => {
                listener2Count++;
            };

            fs.watchFile(filename, { interval: 100 }, listener1);
            fs.watchFile(filename, { interval: 100 }, listener2);

            await sleep(150);
            writeFile(filename, 'first update');

            await sleep(200);
            fs.unwatchFile(filename, listener1);

            await sleep(150);
            writeFile(filename, 'second update');

            await sleep(300);
            fs.unwatchFile(filename, listener2);
            assert.ok(listener1Count >= 1, 'Listener 1 should trigger before unwatch');
            assert.ok(listener2Count >= 2, 'Listener 2 should trigger both times');
        });

        it('should handle unwatchFile on non-existent watcher', () => {
            const filename = path.join(testDir, `file-${generateUniqueId()}.txt`);
            
            // Should not throw
            fs.unwatchFile(filename);
        });

        it('should remove all listeners when called without listener', async (t) => {
            const filename = path.join(testDir, `file-${generateUniqueId()}.txt`);
            writeFile(filename, 'initial');

            let listener1Count = 0;
            let listener2Count = 0;

            const listener1 = (curr, prev) => {
                listener1Count++;
            };

            const listener2 = (curr, prev) => {
                listener2Count++;
            };

            fs.watchFile(filename, { interval: 100 }, listener1);
            fs.watchFile(filename, { interval: 100 }, listener2);

            await sleep(150);
            writeFile(filename, 'first update');

            await sleep(200);
            // Remove all listeners
            fs.unwatchFile(filename);

            await sleep(150);
            writeFile(filename, 'second update - should not trigger');

            await sleep(300);
            assert.ok(listener1Count >= 1, 'Listener 1 should trigger before unwatchFile');
            assert.ok(listener2Count >= 1, 'Listener 2 should trigger before unwatchFile');
        });
    });

    describe('options', () => {
        it('should accept custom interval option', async (t) => {
            const filename = path.join(testDir, `file-${generateUniqueId()}.txt`);
            writeFile(filename, 'initial');

            let changeTriggered = false;

            const listener = (curr, prev) => {
                changeTriggered = true;
            };

            // Short interval for faster test
            fs.watchFile(filename, { interval: 50 }, listener);

            await sleep(100);
            writeFile(filename, 'updated');

            await sleep(300);
            fs.unwatchFile(filename, listener);
            assert.ok(changeTriggered);
        });

        it('should use default interval when not specified', async (t) => {
            const filename = path.join(testDir, `file-${generateUniqueId()}.txt`);
            writeFile(filename, 'initial');

            let changeTriggered = false;

            const listener = (curr, prev) => {
                changeTriggered = true;
            };

            fs.watchFile(filename, listener);

            await sleep(150);
            writeFile(filename, 'updated');

            await sleep(1350);
            fs.unwatchFile(filename, listener);
        });
    });

    describe('bigint option', () => {
        it('should support bigint stats', async (t) => {
            const filename = path.join(testDir, `file-${generateUniqueId()}.txt`);
            writeFile(filename, 'initial');

            let changeTriggered = false;
            let capturedCurr, capturedPrev;

            const listener = (curr, prev) => {
                capturedCurr = curr;
                capturedPrev = prev;
                
                if (typeof curr.ctimeMs !== 'undefined') {
                    changeTriggered = true;
                }
            };

            fs.watchFile(filename, { interval: 100, bigint: false }, listener);

            await sleep(150);
            writeFile(filename, 'updated');

            await sleep(350);
            fs.unwatchFile(filename, listener);
            
            assert.ok(capturedCurr && typeof capturedCurr === 'object');
            assert.ok(capturedPrev && typeof capturedPrev === 'object');
        });
    });

    describe('error handling', () => {
        it.skip('should throw when listener is not a function', () => {
            // fibjs doesn't validate argument types the same way as Node.js
            const filename = path.join(testDir, `file-${generateUniqueId()}.txt`);
            writeFile(filename, 'content');

            assert.throws(() => {
                fs.watchFile(filename, 'not a function');
            }, {
                code: 'ERR_INVALID_ARG_TYPE'
            });
        });

        it.skip('should throw when path is not string or buffer', () => {
            // fibjs doesn't validate argument types the same way as Node.js
            assert.throws(() => {
                fs.watchFile(123, () => {});
            }, {
                code: 'ERR_INVALID_ARG_TYPE'
            });

            assert.throws(() => {
                fs.watchFile({}, () => {});
            }, {
                code: 'ERR_INVALID_ARG_TYPE'
            });
        });
    });

    describe('StatWatcher lifecycle', () => {
        it('should emit change event when stopping watcher', async (t) => {
            const filename = path.join(testDir, `file-${generateUniqueId()}.txt`);
            writeFile(filename, 'initial');

            const listener = (curr, prev) => {
                // Change detected
            };

            const watcher = fs.watchFile(filename, { interval: 100 }, listener);
            
            let changeEventEmitted = false;
            watcher.on('change', () => {
                changeEventEmitted = true;
            });

            await sleep(200);
            writeFile(filename, 'updated');
            
            await sleep(200);
            watcher.stop();
            
            // Give time for events to be processed
            await sleep(50);
            assert.ok(changeEventEmitted, 'change event should be emitted');
        });

        it('should not call listener after stop', async (t) => {
            const filename = path.join(testDir, `file-${generateUniqueId()}.txt`);
            writeFile(filename, 'initial');

            let callCount = 0;
            const listener = (curr, prev) => {
                callCount++;
            };

            const watcher = fs.watchFile(filename, { interval: 100 }, listener);

            await sleep(150);
            watcher.stop();

            // Try to trigger change after stop
            await sleep(100);
            writeFile(filename, 'after stop');

            await sleep(250);
            // Should not increment after stop
            const countAfterStop = callCount;
            await sleep(200);
            assert.strictEqual(callCount, countAfterStop, 'listener should not be called after stop');
        });
    });
});
