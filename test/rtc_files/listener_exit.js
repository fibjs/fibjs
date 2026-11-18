var rtc = require('rtc');

const listener = new rtc.RTCListener();
listener.listen(4379, '127.0.0.1');
console.log('listening');
process.exit(7);
