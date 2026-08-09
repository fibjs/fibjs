'use strict';

function getCases() {
    return {
        '1.promise-siblings': function (mark) {
            Promise.resolve().then(function () { mark('A'); });
            Promise.resolve().then(function () { mark('B'); });
            Promise.resolve().then(function () { mark('C'); });
        },
        '2.await-vs-sibling-then': function (mark) {
            Promise.resolve().then(async function () {
                mark('SETUP-START');
                await Promise.resolve();
                mark('AFTER-AWAIT-1');
                await Promise.resolve();
                mark('AFTER-AWAIT-2');
            });
            Promise.resolve().then(function () { mark('EVENT-A'); });
            Promise.resolve().then(function () { mark('EVENT-B'); });
        },
        '3.queueMicrotask-vs-then': function (mark) {
            queueMicrotask(function () { mark('QM-1'); });
            Promise.resolve().then(function () { mark('THEN-1'); });
            queueMicrotask(function () { mark('QM-2'); });
            Promise.resolve().then(function () { mark('THEN-2'); });
        },
        '4.nested-then': function (mark) {
            Promise.resolve().then(function () {
                mark('OUTER-A');
                Promise.resolve().then(function () { mark('INNER-A'); });
            });
            Promise.resolve().then(function () { mark('OUTER-B'); });
        },
        '5.await-vs-queueMicrotask': function (mark) {
            Promise.resolve().then(async function () {
                mark('SETUP-START');
                await Promise.resolve();
                mark('AFTER-AWAIT-1');
            });
            queueMicrotask(function () { mark('QM-A'); });
            queueMicrotask(function () { mark('QM-B'); });
        },
        '6.catch-finally': function (mark) {
            Promise.reject(new Error('x'))
                .catch(function () { mark('CATCH'); })
                .finally(function () { mark('FINALLY'); });
            Promise.resolve().then(function () { mark('THEN'); });
        },
        '7.nextTick-vs-promise': function (mark) {
            process.nextTick(function () { mark('TICK-A'); });
            Promise.resolve().then(function () { mark('THEN-A'); });
            process.nextTick(function () { mark('TICK-B'); });
            Promise.resolve().then(function () { mark('THEN-B'); });
        },
        '8.nextTick-inside-then': function (mark) {
            Promise.resolve().then(function () {
                mark('THEN-OUTER');
                process.nextTick(function () { mark('TICK-INNER'); });
                Promise.resolve().then(function () { mark('THEN-INNER'); });
            });
            Promise.resolve().then(function () { mark('THEN-SIBLING'); });
        },
        '9.listener-race-model': function (mark) {
            var listeners = [];
            function on(fn) { listeners.push(fn); mark('REGISTER'); }
            function emit(name) {
                mark('EMIT:' + name);
                if (!listeners.length) {
                    mark('LOST:' + name);
                    return;
                }
                listeners.slice().forEach(function (fn) { fn(name); });
            }

            Promise.resolve().then(async function setup() {
                mark('SETUP-START');
                await Promise.resolve();
                mark('AFTER-AWAIT-1');
                await Promise.resolve();
                mark('AFTER-AWAIT-2');
                on(function (name) { mark('DELIVER:' + name); });
                mark('SETUP-END');
            });
            Promise.resolve().then(function () { mark('EVENT-A-CB'); emit('A'); });
            Promise.resolve().then(function () { mark('EVENT-B-CB'); emit('B'); });
        }
    };
}

function waitImmediate() {
    return new Promise(function (resolve) {
        setImmediate(resolve);
    });
}

async function main() {
    var caseName = process.argv[2];
    var run = getCases()[caseName];
    if (!run)
        throw new Error('unknown case: ' + caseName);

    var order = [];
    run(function (x) {
        order.push(x);
    });

    await waitImmediate();
    await waitImmediate();
    console.log(JSON.stringify(order));
}

main().then(function () {
    process.exit(0);
}, function (e) {
    console.error(e && (e.stack || e.message || e));
    process.exit(1);
});