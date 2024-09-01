var rtc = require("rtc");

rtc.listen(60916, function (binding) {
    console.log(JSON.stringify(binding));
    rtc.stopListen(60916);
});
