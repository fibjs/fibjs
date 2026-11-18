var rtc = require('rtc');

const listener = new rtc.RTCListener();
listener.listen(4359, '127.0.0.1');
console.log('listening');

setTimeout(function () {
    listener.close();
    console.log('closed');
}, 200);
