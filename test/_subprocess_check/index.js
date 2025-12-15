/**
 * @description run this script to check if any module's test case invalid in SubProcess
 */

var { describe, it, before, after, beforeEach, afterEach } = require('node:test');
var assert = require('assert');

const path = require("path");
const util = require("util");
const cmd = process.execPath;

describe("Run other Test Case In SubProcess", () => {
    it(`run: ${process.platform}`, () => {
        if (process.platform === 'win32') {
            const retcode = process.run(cmd, [path.join(__dirname, '../main.js')], {
                env1: {
                    ...util.pick(process.env, [
                        "SYSTEMROOT",
                        "SystemRoot",
                        "TEMP",
                        "TMP",
                        "CommonProgramFiles",
                        "CommonProgramFiles(x86)",
                        "CommonProgramW6432",
                        "ProgramFiles",
                        "ProgramFiles(x86)",
                        "ProgramW6432",
                    ]),
                    CI_SUBPROCESS_CHECK: 1
                }
            });

            assert.equal(retcode, 0);
        }

        if (process.platform === 'darwin') {
            const retcode = process.run(cmd, [path.join(__dirname, '../main.js')], {
                env: {
                    HOME: process.env.HOME,
                    TMPDIR: process.env.TMPDIR,
                    CI_SUBPROCESS_CHECK: 1
                }
            });

            assert.equal(retcode, 0);
        }

        if (process.platform === 'linux') {
            const retcode = process.run(cmd, [path.join(__dirname, '../main.js')], {
                env: {
                    HOME: process.env.HOME,
                    TMPDIR: process.env.TMPDIR,
                    CI_SUBPROCESS_CHECK: 1
                }
            });

            assert.equal(retcode, 0);
        }
    });
});
