const { isMainThread, parentPort, threadId, workerData } = require('worker_threads');

switch (workerData?.mode) {
    case 'announce':
        parentPort.postMessage({
            kind: 'announce',
            value: workerData.value,
            isMainThread,
            threadId
        });
        break;
    case 'echo':
        parentPort.on('message', (message) => {
            if (message && message.cmd === 'exit') {
                process.exit(message.code ?? 0);
                return;
            }

            parentPort.postMessage(message);
        });
        break;
    case 'throw-on-start':
        throw new Error('worker boom');
    case 'idle':
        setInterval(() => {}, 1000);
        break;
    default:
        parentPort.postMessage(workerData);
        break;
}