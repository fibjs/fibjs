const { parentPort } = require('node:worker_threads');

Master.onmessage = (evt) => {
    parentPort.postMessage(evt.data);
};