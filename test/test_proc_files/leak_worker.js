/*
 * leak_worker.js — scene: leaked worker_threads Worker
 *
 * Worker is never terminated (its child script keeps it alive).
 * Expects: watchdog fires, exit 124; report shows the Worker class count.
 */
var { describe, it } = require('node:test');
var worker_threads = require('worker_threads');
var path = require('path');

describe('leak worker', () => {
    it('leak a worker', () => {
        global.leakedWorker = new worker_threads.Worker(path.join(__dirname, 'leak_worker_child.js'));
    });
});
