var rtc = require("rtc");

rtc.listen(60916, function (binding) { });

var count = 0;
const timer = setInterval(function () {
    console.log(count);
    if (count++ >= 3) {
        rtc.stopListen(60916);
        clearInterval(timer);

        process.exitCode = 100;
    }
}, 1000);