var rtc = require("rtc");

rtc.listen(3223, function (binding) {
    console.log(JSON.stringify(binding));
    rtc.stopListen(3223);
});
