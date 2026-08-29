// Long-running child process that runs until killed by a signal from the parent.
// Used by child_process_test.js "signal death" tests to verify exit/close event args.
setInterval(() => {}, 1000);
