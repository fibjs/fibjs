// Test file for unref functionality
let cnt = 10;

const tmr = setInterval(() => {
    console.log("sub process running, remaining:", cnt);
    if (--cnt === 0) {
        clearInterval(tmr);
    }
}, 1000);
