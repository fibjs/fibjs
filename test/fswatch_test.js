var test = require("test");

var fs = require('fs');
var path = require('path');
var coroutine = require('coroutine');
var uuid = require('uuid');

var { rmFile } = require('./_helpers/fs')
var { ensureDirectoryExisted } = require('./_helpers/process');

test.setup();

describe('fs.watch*', () => {
    const basedir = path.resolve(__dirname)
    const relcwdbase = path.relative(process.cwd(), basedir)

    var writeFile = (rel, content = '') => {
        fs.writeTextFile(path.resolve(rel), content);
    }
    var resolve_reltocwd = (rel) => {
        return path.join(relcwdbase, rel)
    }
    var createFile = (rel, content = '') => {
        writeFile(rel, content);
    }
    var delFile = (rel) => {
        fs.unlink(rel)
    }

    describe("fs.watch()", () => {
        before(() => {
            rmFile(path.resolve(__dirname, './fswatch_files'));
            ensureDirectoryExisted(path.resolve(__dirname, './fswatch_files'));
        })
        var TRIGGER_TIME = 3

        process.env.MANUAL && describe("manual watch", () => {
            it("try not quit", () => {
                let count = 0;
                let evt = new coroutine.Event();
                const watcher = fs.watch(
                    './test/fswatch_files/nogit-manual.txt',
                    (evt_type, filename) => {
                        console.log('watched evt_type: %s; filename: %s', evt_type, filename)
                        if (++count > 10) {
                            watcher.close()
                            evt.set()
                        }
                    }
                )

                evt.wait()
                assert.ok(evt.isSet())
            });
        })

        describe("::close", () => {
            it("robust: multiple times close(though it's pointless)", () => {
                const relpath = `./fswatch_files/nogit-${uuid.snowflake().hex()}.txt`
                // ensure it existed
                writeFile(resolve_reltocwd(relpath), '')

                const watcher = fs.watch(
                    resolve_reltocwd(relpath)
                )

                for (let i = 0; i < 10; i++)
                    watcher.close();
            });

            it("robust: allow multiple times close(though it's pointless)", () => {
                const relpath = `./fswatch_files/nogit-${uuid.snowflake().hex()}.txt`
                // ensure it existed
                writeFile(resolve_reltocwd(relpath), '')

                var j = 0;
                while (++j < 20) {
                    const watcher = fs.watch(
                        resolve_reltocwd(relpath)
                    )

                    coroutine.sleep(1);
                    for (let i = 0; i < 10; i++)
                        watcher.close();
                }
            });

            it("robust: allow multiple times close(it's pointless)", () => {
                var triggedCallback = false;
                const relpath = `./fswatch_files/nogit-${uuid.snowflake().hex()}.txt`
                // ensure it existed
                writeFile(resolve_reltocwd(relpath), '')

                const watcher = fs.watch(
                    resolve_reltocwd(relpath),
                    () => {
                        triggedCallback = true;
                        watcher.close();
                    }
                )

                // change it
                for (let i = 0; i < 10; i++)
                    writeFile(resolve_reltocwd(relpath), 'abc')

                coroutine.sleep(50);
                for (let i = 0; i < 10; i++)
                    watcher.close();

                assert.ok(triggedCallback);
            });
        })

        it("hold process if last watcher not close", () => {
            var p = process.open(process.execPath, [path.join(__dirname, 'fswatch_test', 'hold1.js')]);
            assert.equal(p.stdout.readLine(), "after start watching");
            assert.equal(p.stdout.readLine(), "watched");
            assert.equal(p.wait(), 9);
        });

        describe("write file", () => {
            var proc = ({
                changeEventSource = '',
                next,
                recursive = true
            }) => {
                var _uuid = uuid.snowflake().hex()
                var relfile = resolve_reltocwd(`./fswatch_files/nogit-${_uuid}.txt`)

                writeFile(relfile, '')

                var watcher
                var changeTriggerCount = 0
                const _handler = (evtType, filename) => {
                    assert.isString(evtType)
                    assert.isString(filename)
                    console.log('\twatched file change, type: %s; filename: %s', evtType, filename)

                    changeTriggerCount++;
                }
                switch (changeEventSource) {
                    case 'listener':
                        watcher = fs.watch(relfile, { recursive }, _handler)
                        break
                    case 'register':
                        watcher = fs.watch(relfile, { recursive })
                        watcher.on('change', _handler)
                        break
                }
                let writeCount = 0
                var noTriggerAfterClose = true
                const itv = setInterval(() => {
                    if (writeCount < TRIGGER_TIME) {
                        writeFile(relfile, `this is: ${++writeCount}`)
                        return;
                    }

                    clearInterval(itv);
                    watcher.close();
                    watcher.on('change', () => {
                        noTriggerAfterClose = false;
                    });
                    /**
                     * in some system, too short file change duration would collapse to one-time, but the duration
                     * is much less than 1500ms
                     */
                }, 1500);

                watcher.on('close', () => {
                    assert.ok(changeTriggerCount > 0);
                    assert.ok(changeTriggerCount >= writeCount);

                    assert.isTrue(noTriggerAfterClose);

                    next();
                })

                assert.isObject(watcher)
                assert.isFunction(watcher.close)
            }

            it(`event: 'change', from listener`, (next) => {
                proc({ changeEventSource: 'listener', next });
            });

            it(`event: 'change', from listener, recursive`, (next) => {
                proc({ changeEventSource: 'listener', next, recursive: true });
            });

            it(`event: 'change', from register`, (next) => {
                proc({ changeEventSource: 'register', next });
            });

            it(`event: 'change', from register, recursive`, (next) => {
                proc({ changeEventSource: 'register', next, recursive: true });
            });
        });

        describe("watch Directory create file", () => {
            var proc = ({
                changeEventSource = '',
                next,
            }) => {
                var _uuid = uuid.snowflake().hex()
                var reldir = resolve_reltocwd(`./fswatch_files/${_uuid}`)

                ensureDirectoryExisted(reldir)

                var watcher
                var changeTriggerCount = 0
                const _handler = (evtType, filename) => {
                    assert.isString(evtType)
                    assert.isString(filename)
                    console.log('\twatched file change, type: %s; filename: %s', evtType, filename)

                    changeTriggerCount++;
                }
                switch (changeEventSource) {
                    case 'listener':
                        watcher = fs.watch(reldir, _handler)
                        break
                    case 'register':
                        watcher = fs.watch(reldir)
                        watcher.on('change', _handler)
                        break
                }
                let writeCount = 0
                var noTriggerAfterClose = true
                const itv = setInterval(() => {
                    if (writeCount < TRIGGER_TIME) {
                        createFile(path.join(reldir, `nogit-${writeCount}.txt`), `this is: ${++writeCount}`)
                        return;
                    }

                    clearInterval(itv);
                    watcher.close();
                    watcher.on('change', () => {
                        noTriggerAfterClose = false;
                    });
                    /**
                     * in some system, too short file change duration would collapse to one-time, but the duration
                     * is much less than 200ms
                     */
                }, 200);

                watcher.on('close', () => {
                    assert.ok(changeTriggerCount > 0);
                    assert.ok(changeTriggerCount >= writeCount);

                    assert.isTrue(noTriggerAfterClose);

                    next();
                })
            }

            it(`event: 'rename', from listener`, (next) => {
                proc({ changeEventSource: 'listener', next });
            });

            it(`event: 'rename', from register`, (next) => {
                proc({ changeEventSource: 'register', next });
            });
        });

        describe("watch Directory delete file", () => {
            var proc = ({
                changeEventSource = '',
                next,
            }) => {
                var _uuid = uuid.snowflake().hex()
                var reldir = resolve_reltocwd(`./fswatch_files/${_uuid}`)
                ensureDirectoryExisted(reldir)

                var files = Array(TRIGGER_TIME)
                    .fill(undefined)
                    .map((_, c) => {
                        var fname = path.resolve(reldir, `nogit-${c}.txt`)
                        writeFile(fname, `this is file to delete, ${c}`);
                        return fname
                    })
                var fileCount = files.length


                var watcher
                var changeTriggerCount = 0
                const _handler = (evtType, filename) => {
                    assert.isString(evtType)
                    assert.isString(filename)
                    console.log('\twatched file change, type: %s; filename: %s', evtType, filename)

                    changeTriggerCount++;
                }
                switch (changeEventSource) {
                    case 'listener':
                        watcher = fs.watch(reldir, _handler)
                        break
                    case 'register':
                        watcher = fs.watch(reldir)
                        watcher.on('change', _handler)
                        break
                }

                var noTriggerAfterClose = true
                const itv = setInterval(() => {
                    if (files.length) {
                        delFile(files.shift())
                        return;
                    }

                    clearInterval(itv);
                    watcher.close();
                    watcher.on('change', () => {
                        noTriggerAfterClose = false;
                    });
                }, 1500);

                watcher.on('close', () => {
                    assert.ok(changeTriggerCount > 0);
                    assert.ok(changeTriggerCount >= fileCount);

                    assert.isTrue(noTriggerAfterClose);

                    next();
                })
            }

            it(`event: 'rename', from listener`, (next) => {
                proc({ changeEventSource: 'listener', next });
            });

            it(`event: 'rename', from register`, (next) => {
                proc({ changeEventSource: 'register', next });
            });
        });
    })

    describe("fs.watchFile/fs.unwatchFile", () => {
        before(() => {
            rmFile(path.resolve(__dirname, './fs_watchfile_output'));
            ensureDirectoryExisted(path.resolve(__dirname, './fs_watchfile_output'));
        })
        var TRIGGER_TIME = 3

        process.env.MANUAL && describe.only("manual watchFile", () => {
            it("try hold context", () => {
                let count = 0;
                let evt = new coroutine.Event();
                const watcher = fs.watchFile(
                    './test/fs_watchfile_output/nogit-manual.txt',
                    (curStat, prevStat) => {
                        assert.isObject(prevStat)
                        assert.isObject(curStat)
                        console.log('watched prevStat', prevStat.mtime)
                        console.log('watched curStat', curStat.mtime)
                        if (++count > 10) {
                            watcher.close()
                            evt.set()
                        }
                    }
                )

                evt.wait()
                assert.ok(evt.isSet())
            });
        })

        describe("::close", () => {
            it("robust: multiple times close(though it's pointless)", () => {
                const relpath = `./fs_watchfile_output/nogit-${uuid.snowflake().hex()}.txt`
                // ensure it existed
                writeFile(resolve_reltocwd(relpath), '')

                const watcher = fs.watchFile(
                    resolve_reltocwd(relpath),
                    () => { }
                )

                for (let i = 0; i < 10; i++)
                    watcher.close();
            });

            it("robust: allow multiple times close(though it's pointless)", () => {
                const relpath = `./fs_watchfile_output/nogit-${uuid.snowflake().hex()}.txt`
                // ensure it existed
                writeFile(resolve_reltocwd(relpath), '')

                var j = 0;
                while (++j < 20) {
                    const watcher = fs.watchFile(
                        resolve_reltocwd(relpath),
                        () => { }
                    )

                    coroutine.sleep(1);
                    for (let i = 0; i < 10; i++)
                        watcher.close();
                }
            });

            it("robust: allow multiple times close(it's pointless)", () => {
                var triggedCallback = false;
                const target = resolve_reltocwd(`./fs_watchfile_output/nogit-${uuid.snowflake().hex()}.txt`)
                // ensure it existed
                writeFile(target, '')

                const watcher = fs.watchFile(
                    target,
                    { interval: 100 },
                    () => {
                        watcher.close();
                        triggedCallback = true;
                    }
                )

                // change it
                for (let i = 0; i < 10; i++)
                    writeFile(target, 'abc')

                coroutine.sleep(1000);
                for (let i = 0; i < 10; i++)
                    watcher.close();

                assert.ok(triggedCallback);
            });
        })

        it("hold process if last watcher not close", () => {
            var p = process.open(process.execPath, [path.join(__dirname, 'fswatch_test', 'hold2.js')], { timeout: 1e4 });
            assert.equal(p.stdout.readLine(), "after start watching");
            assert.equal(p.stdout.readLine(), "watched");
            assert.equal(p.wait(), 9);
        });

        it("hold process if last watcher hasn't been unref-ed", () => {
            var p = process.open(process.execPath, [path.join(__dirname, 'fswatch_test', 'hold3.js'), { timeout: 1e4 }]);
            assert.equal(p.stdout.readLine(), "after start watching");
            assert.equal(p.stdout.readLine(), "watched");
            assert.equal(p.wait(), 9);
        });

        describe("you can watchFile/unwatchFile one non-existed target", () => {
            var _uuid = uuid.snowflake().hex()
            var relfile = resolve_reltocwd(`./fs_watchfile_output/nogit-${_uuid}.txt`)
            var triggeredThoughWatchNil = false;

            it("fs.watchFile nil", () => {
                let nilTuple = []
                const watcher = fs.watchFile(relfile, { interval: 50 }, (cur, prev) => {
                    triggeredThoughWatchNil = true;
                    nilTuple = [cur, prev]
                })

                coroutine.sleep(200)

                watcher.close();
                assert.ok(triggeredThoughWatchNil);
                assert.ok(nilTuple.length === 2);
                assert.ok(nilTuple[0].ctimeMs === 0);
                assert.ok(nilTuple[1].ctimeMs === 0);
            });

            it("fs.unwatchFile nil", () => {
                fs.unwatchFile(relfile)
            });
        });

        describe("fs.watchFile", () => {
            it("wait file changed", () => {
                const target = resolve_reltocwd(`./fs_watchfile_output/nogit-${uuid.snowflake().hex()}.txt`)
                var triggered = false;
                var TRIGGER_TIME = 100;

                const watcher = fs.watchFile(
                    target,
                    { interval: 100 },
                    (curStat, prevStat) => {
                        // should be called
                        assert.isObject(prevStat)
                        assert.isObject(curStat)

                        triggered = true;
                    }
                )
                const evt = new coroutine.Event();

                var writeCount = 0;
                const itv = setInterval(() => {
                    if (writeCount < TRIGGER_TIME) {
                        writeFile(target, `this is: ${++writeCount}`)
                        return;
                    }

                    clearInterval(itv);
                    watcher.close();
                    evt.set();
                }, 10);

                evt.wait();
                assert.ok(triggered);
            });
        });

        describe("fs.unwatchFile", () => {
            it("onchange callback not be triggered after fs.unwatchFile(target)", () => {
                const target = resolve_reltocwd(`./fs_watchfile_output/nogit-${uuid.snowflake().hex()}.txt`)
                // ensure it existed
                writeFile(target, '')

                let noTriggered = true;
                const watcher = fs.watchFile(
                    target,
                    { interval: 100 },
                    (curStat, prevStat) => {
                        // should be called
                        assert.isObject(prevStat)
                        assert.isObject(curStat)

                        noTriggered = false;
                    }
                )

                fs.unwatchFile(target)
                writeFile(target, '')

                watcher.close();
                assert.ok(noTriggered);
            });

            it("onchange callback not be triggered after fs.unwatchFile(target)", () => {
                const target = resolve_reltocwd(`./fs_watchfile_output/nogit-${uuid.snowflake().hex()}.txt`)
                // ensure it existed
                writeFile(target, '')

                let calledCount = 0;
                const watcher = fs.watchFile(
                    target,
                    { interval: 100 },
                    (curStat, prevStat) => {
                        // should be called once only
                        assert.isObject(prevStat)
                        assert.isObject(curStat)

                        calledCount++;
                    }
                )

                /**
                 * @why
                 * In some machines(.e.g, with few CPUs only, or too slow), the time file written
                 * could not be catched in `window` of monitor loop created by `fs.watchFile()`,
                 * like this:
                 * 
                 * --- | (file's mtime changed?) | -------------5007s----------- | (file's mtime changed?) | ----
                 * ------------------------------| wf -- wf -- wf -- wf -- wf -- |---------------------------
                 * 
                 * if you use `fs.watchFile()`, the `moment` of writing file and `mtime` changed
                 * could be lost.
                 * 
                 * if you use `fs.watch()`, you can catch "every" time file was written, because the watcher would
                 * be notified when system detect the change of file.
                 * 
                 * When using `fs.watchFile`, we write more times to ensure we can "capture" the moment file changed.
                 */
                for (let i = 0; i < 10; i++)
                    writeFile(target, Date.now())

                coroutine.sleep(300);

                fs.unwatchFile(target)
                writeFile(target, '')

                watcher.close();

                assert.ok(calledCount >= 1);
            });

            it("canceled_onchange callback no triggered if fs.unwatchFile(target, canceled_onchange)", () => {
                const target = resolve_reltocwd(`./fs_watchfile_output/nogit-${uuid.snowflake().hex()}.txt`)
                // ensure it existed
                writeFile(target, '')

                let flag1 = '';
                let flag2 = '';

                const onchange = (curStat, prevStat) => {
                    assert.isObject(prevStat)
                    assert.isObject(curStat)

                    flag1 = 'onchange'
                };

                const canceled_onchange = (curStat, prevStat) => {
                    assert.isObject(prevStat)
                    assert.isObject(curStat)

                    flag2 = 'canceled_onchange'
                };
                const watcher = fs.watchFile(
                    target,
                    { interval: 100 },
                    canceled_onchange
                );

                watcher.on('change', onchange)

                fs.unwatchFile(target, canceled_onchange)

                for (let i = 0; i < 10; i++)
                    writeFile(target, Date.now())

                coroutine.sleep(300);

                for (let i = 0; i < 10; i++)
                    writeFile(target, Date.now())

                assert.ok(flag1 == 'onchange');
                assert.ok(flag2 == '');
                watcher.close();
            });
        });

        describe("write to existed file", () => {
            var proc = ({
                changeEventSource = '',
                next,
            }) => {
                var _uuid = uuid.snowflake().hex()
                var relfile = resolve_reltocwd(`./fs_watchfile_output/nogit-${_uuid}.txt`)

                writeFile(relfile, '')

                if (changeEventSource === 'register') {
                    assert.throws(() => {
                        fs.watchFile(relfile, { interval: 50 })
                    });

                    assert.throws(() => {
                        fs.watchFile(relfile)
                    });

                    next();
                    return
                }

                var changeTriggerCount = 0

                const _handler = (prevStat, curStat) => {
                    assert.isObject(prevStat)
                    assert.isObject(curStat)

                    changeTriggerCount++;
                }

                var watcher = fs.watchFile(relfile, { interval: 50 }, _handler)

                let writeCount = 0
                var noTriggerAfterClose = true
                const itv = setInterval(() => {
                    if (writeCount < TRIGGER_TIME) {
                        writeFile(relfile, `this is: ${++writeCount}`)
                        return;
                    }

                    clearInterval(itv);
                    watcher.close();
                    watcher.on('change', () => {
                        noTriggerAfterClose = false;
                    });
                    /**
                     * in some system, too short file change duration would collapse to one-time, but the duration
                     * is much less than 1500ms
                     */
                }, 500);

                watcher.on('close', () => {
                    assert.ok(changeTriggerCount > 0);
                    // assert.ok(changeTriggerCount >= writeCount);

                    assert.isTrue(noTriggerAfterClose);

                    next();
                })

                assert.isObject(watcher)
                assert.isFunction(watcher.close)
            }

            it(`event: 'change', from listener`, (next) => {
                proc({ changeEventSource: 'listener', next });
            });

            it(`event: 'change', onchange callback is required when watchFile(xxx, onchange)`, (next) => {
                proc({ changeEventSource: 'register', next });
            });
        });
    })
})

require.main === module && test.run(console.DEBUG);