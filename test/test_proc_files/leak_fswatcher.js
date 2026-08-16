/*
 * leak_fswatcher.js — scene: leaked FSWatcher
 *
 * fs.watch is never closed.
 * Expects: watchdog fires, exit 124; report shows the FSWatcher class count.
 */
var { describe, it } = require('node:test');
var fs = require('fs');

describe('leak fswatcher', () => {
    it('leak a fs watcher', () => {
        global.leakedWatcher = fs.watch(__dirname);
    });
});
