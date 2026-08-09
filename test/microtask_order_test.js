var { describe, it } = require('node:test');
var assert = require('assert');
var child_process = require('child_process');

var nodeExpected = {
    '1.promise-siblings': ['A', 'B', 'C'],
    '2.await-vs-sibling-then': ['SETUP-START', 'EVENT-A', 'EVENT-B', 'AFTER-AWAIT-1', 'AFTER-AWAIT-2'],
    '3.queueMicrotask-vs-then': ['QM-1', 'THEN-1', 'QM-2', 'THEN-2'],
    '4.nested-then': ['OUTER-A', 'OUTER-B', 'INNER-A'],
    '5.await-vs-queueMicrotask': ['SETUP-START', 'QM-A', 'QM-B', 'AFTER-AWAIT-1'],
    '6.catch-finally': ['CATCH', 'THEN', 'FINALLY'],
    '7.nextTick-vs-promise': ['TICK-A', 'TICK-B', 'THEN-A', 'THEN-B'],
    '8.nextTick-inside-then': ['THEN-OUTER', 'THEN-SIBLING', 'THEN-INNER', 'TICK-INNER'],
    '9.listener-race-model': [
        'SETUP-START',
        'EVENT-A-CB', 'EMIT:A', 'LOST:A',
        'EVENT-B-CB', 'EMIT:B', 'LOST:B',
        'AFTER-AWAIT-1', 'AFTER-AWAIT-2', 'REGISTER', 'SETUP-END'
    ]
};

var runner = __filename.replace(/microtask_order_test\.js$/, '_helpers/microtask_order_runner.js');

function runCaseInChild(caseName) {
    var out = child_process.execFileSync(process.execPath, [runner, caseName], {
        encoding: 'utf8'
    }).trim();
    return JSON.parse(out);
}

describe('microtask order', () => {
    Object.keys(nodeExpected).forEach((caseName) => {
        it(caseName, () => {
            var actual = runCaseInChild(caseName);
            assert.deepStrictEqual(actual, nodeExpected[caseName]);
        });
    });
});