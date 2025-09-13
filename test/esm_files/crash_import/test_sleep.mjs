import coroutine from 'coroutine';

console.log("test_sleep");
coroutine.sleep(100);

console.log("test_sleep export");
export default {
    a: 100
};
