var rtc = require("rtc");

rtc.listen(3222, function (binding) { });

var count = 0;
const timer = setInterval(function () {
    console.log(count);
    if (count++ >= 3) {
        rtc.stopListen(3222);
        clearInterval(timer);

        process.exitCode = 100;
    }
}, 1000);