const events = require('events');

Master.onmessage = (evt) => {
  if (evt.data === 'get') {
    Master.postMessage(events.defaultMaxListeners);
  } else {
    events.defaultMaxListeners = 12;
    Master.postMessage(events.defaultMaxListeners);
  }
};
