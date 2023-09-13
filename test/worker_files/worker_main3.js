const { workerData } = require('worker_threads');

Master.onmessage = (evt) => {
    Master.postMessage(workerData);
};
