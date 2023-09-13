const { parentPort } = require('worker_threads');

parentPort.onmessage = (evt) => {
    parentPort.postMessage(evt.data);
};