// With a process 'unhandledRejection' listener, the event is dispatched with
// (reason, promise) and the process keeps running.
process.on('unhandledRejection', (reason, promise) => {
    console.log('reason: ' + reason.message);
    console.log('isPromise: ' + (promise instanceof Promise));
});
Promise.reject(new Error('boom'));
