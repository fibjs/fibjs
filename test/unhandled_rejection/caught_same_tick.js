// Rejection caught in the same tick must not produce stderr noise and must
// not terminate the process.
const p = Promise.reject(new Error('boom'));
p.catch(e => console.log('caught: ' + e.message));
