const { PerformanceObserver, performance } = require('perf_hooks');
const { describe, it, beforeEach } = require('node:test');
const assert = require('assert');

function sleep(ms) {
    return new Promise((resolve) => {
        setTimeout(resolve, ms);
    });
}

describe('Performance API', () => {
    describe('Performance Marks', () => {
        it('performance.mark basic usage', () => {
            performance.mark('A');
            const entries = performance.getEntries();
            assert.equal(entries.length >= 1, true);
            assert.equal(entries[0].name, 'A');
            assert.equal(entries[0].entryType, 'mark');

            performance.clearMarks();
            const entries1 = performance.getEntries();
            assert.equal(entries1.length, 0);
        });

        it('performance.mark with options', () => {
            performance.mark('B', {
                detail: 'test detail',
                startTime: 100
            });
            const entry = performance.getEntriesByName('B')[0];
            assert.equal(entry.startTime, 100);
            assert.equal(entry.detail, 'test detail');
        });
    });

    describe('PerformanceObserver', () => {
        beforeEach(() => {
            performance.clearMarks();
            performance.clearMeasures();
        });

        it('observe mark', async () => {
            var entries;
            const observer = new PerformanceObserver((list) => {
                entries = list.getEntries();
            });
            observer.observe({ entryTypes: ['mark'] });
            performance.mark('C');

            await sleep(10);
            assert.equal(entries.length, 1);
            assert.equal(entries[0].name, 'C');
            assert.equal(entries[0].entryType, 'mark');
            observer.disconnect();
        });

        it('observe measure', async () => {
            var entries;
            performance.mark('D');
            const observer = new PerformanceObserver((list) => {
                entries = list.getEntries();
            });
            observer.observe({ entryTypes: ['measure'] });
            performance.measure('D-E', 'D');

            await sleep(10);
            assert.equal(entries.length, 1);
            assert.equal(entries[0].name, 'D-E');
            assert.equal(entries[0].entryType, 'measure');
            observer.disconnect();
        });

        it('observe multiple entry types', async () => {
            var entries;
            const observer = new PerformanceObserver((list) => {
                entries = list.getEntries();
            });
            observer.observe({ entryTypes: ['mark', 'measure'] });
            performance.mark('F');
            performance.measure('F-G', 'F');

            await sleep(10);
            assert.equal(entries.length, 2);
            assert.equal(entries[0].name, 'F');
            assert.equal(entries[0].entryType, 'mark');
            assert.equal(entries[1].name, 'F-G');
            assert.equal(entries[1].entryType, 'measure');
            observer.disconnect();
        });
    });

    describe('Performance Measurements', () => {
        describe('Basic Measure Operations', () => {
            beforeEach(() => {
                performance.clearMarks();
                performance.clearMeasures();
            });

            it('measure without parameters', async () => {
                var entries = [];
                const observer = new PerformanceObserver((list) => {
                    entries = entries.concat(list.getEntries());
                });
                observer.observe({ entryTypes: ['measure'] });

                performance.measure('simple-measure');
                await sleep(10);

                assert.equal(entries.length, 1);
                const entry = entries[0];
                assert.equal(entry.name, 'simple-measure');
                assert.ok(entry.duration >= 0);
                assert.ok(entry.startTime >= 0);

                observer.disconnect();
            });

            it('measure with start mark', async () => {
                var entries = [];
                const observer = new PerformanceObserver((list) => {
                    entries = entries.concat(list.getEntries());
                });
                observer.observe({ entryTypes: ['measure'] });

                const markTime = Math.floor(performance.now());
                performance.mark('start-mark');
                await sleep(50);
                performance.measure('mark-measure', {
                    start: 'start-mark'
                });

                await sleep(10);
                assert.equal(entries.length, 1);
                const entry = entries[0];
                assert.equal(entry.name, 'mark-measure');
                assert.ok(entry.duration >= 50);
                assert.ok(Math.abs(entry.startTime - markTime) < 1);

                observer.disconnect();
            });

            it('measure between marks', async () => {
                var entries = [];
                const observer = new PerformanceObserver((list) => {
                    entries = entries.concat(list.getEntries());
                });
                observer.observe({ entryTypes: ['measure'] });

                const startTime = Math.floor(performance.now());
                performance.mark('start-mark');
                await sleep(50);
                performance.mark('end-mark');
                performance.measure('mark-to-mark', {
                    start: 'start-mark',
                    end: 'end-mark'
                });

                await sleep(10);
                assert.equal(entries.length, 1);
                const entry = entries[0];
                assert.equal(entry.name, 'mark-to-mark');
                assert.ok(entry.duration >= 50);
                assert.ok(Math.abs(entry.startTime - startTime) < 1);

                observer.disconnect();
            });

            it('measure with numeric timestamps', async () => {
                var entries = [];
                const observer = new PerformanceObserver((list) => {
                    entries = entries.concat(list.getEntries());
                });
                observer.observe({ entryTypes: ['measure'] });

                const startTime = Math.floor(performance.now());
                await sleep(50);
                const endTime = Math.floor(performance.now());
                performance.measure('time-to-time', {
                    start: startTime,
                    end: endTime
                });

                await sleep(10);
                assert.equal(entries.length, 1);
                const entry = entries[0];
                assert.equal(entry.name, 'time-to-time');
                assert.ok(entry.duration >= 50);
                assert.equal(entry.startTime, startTime);

                observer.disconnect();
            });

            it('measure with start and end marks', async () => {
                var entries;
                const observer = new PerformanceObserver((list) => {
                    entries = list.getEntries();
                });
                observer.observe({ entryTypes: ['measure'] });

                performance.mark('start-op');
                await sleep(50);
                performance.mark('end-op');
                performance.measure('operation-time', 'start-op', 'end-op');

                await sleep(10);
                assert.equal(entries[0].name, 'operation-time');
                assert.ok(entries[0].duration >= 45); // Allow some margin for timing
                observer.disconnect();
            });
        });

        describe('Duration Measurements', () => {
            beforeEach(() => {
                performance.clearMarks();
                performance.clearMeasures();
            });

            it('measure with explicit duration', async () => {
                var entries = [];
                const observer = new PerformanceObserver((list) => {
                    entries = entries.concat(list.getEntries());
                });
                observer.observe({ entryTypes: ['measure'] });

                const startTime = Math.floor(performance.now());
                performance.measure('duration-measure', {
                    start: startTime,
                    duration: 100
                });

                await sleep(10);
                assert.equal(entries.length, 1);
                const entry = entries[0];
                assert.equal(entry.name, 'duration-measure');
                assert.equal(entry.duration, 100);
                assert.equal(entry.startTime, startTime);

                observer.disconnect();
            });

            it('measure with duration from now', async () => {
                var entries = [];
                const observer = new PerformanceObserver((list) => {
                    entries = entries.concat(list.getEntries());
                });
                observer.observe({ entryTypes: ['measure'] });

                const beforeMeasure = Math.floor(performance.now());
                performance.measure('now-duration-measure', {
                    start: beforeMeasure,  // Explicitly set start time
                    duration: 150
                });

                await sleep(10);
                assert.equal(entries.length, 1);
                const entry = entries[0];
                assert.equal(entry.name, 'now-duration-measure');
                assert.equal(entry.duration, 150);
                assert.equal(entry.startTime, beforeMeasure);

                observer.disconnect();
            });

            it('measure with start mark and duration', async () => {
                var entries;
                const observer = new PerformanceObserver((list) => {
                    entries = list.getEntries();
                });
                observer.observe({ entryTypes: ['measure'] });

                performance.mark('start');
                performance.measure('duration-test', 'start');

                await sleep(10);
                assert.equal(entries[0].name, 'duration-test');
                assert.ok(entries[0].duration >= 0);
                assert.ok(entries[0].startTime >= 0);
                observer.disconnect();
            });
        });

        describe('Detail and Options', () => {
            beforeEach(() => {
                performance.clearMarks();
                performance.clearMeasures();
            });

            it('measure with detail object', async () => {
                var entries = [];
                const observer = new PerformanceObserver((list) => {
                    entries = entries.concat(list.getEntries());
                });
                observer.observe({ entryTypes: ['measure'] });

                const detail = { phase: 'test', value: 42 };
                performance.measure('detail-measure', {
                    detail
                });

                await sleep(10);
                assert.equal(entries.length, 1);
                const entry = entries[0];
                assert.equal(entry.name, 'detail-measure');
                assert.deepEqual(entry.detail, detail);

                observer.disconnect();
            });

            it('measure with detail and timing', async () => {
                var entries = [];
                const observer = new PerformanceObserver((list) => {
                    entries = entries.concat(list.getEntries());
                });
                observer.observe({ entryTypes: ['measure'] });

                const startTime = Math.floor(performance.now());
                const detail = { phase: 'test', value: 42 };
                performance.measure('detail-timing-measure', {
                    start: startTime,
                    duration: 100,
                    detail
                });

                await sleep(10);
                assert.equal(entries.length, 1);
                const entry = entries[0];
                assert.equal(entry.name, 'detail-timing-measure');
                assert.equal(entry.duration, 100);
                assert.equal(entry.startTime, startTime);
                assert.deepEqual(entry.detail, detail);

                observer.disconnect();
            });

            it('measure with detailed options', async () => {
                var entries;
                const observer = new PerformanceObserver((list) => {
                    entries = list.getEntries();
                });
                observer.observe({ entryTypes: ['measure'] });

                const detail = { category: 'test', id: 123 };
                performance.measure('detailed-measure', {
                    detail: detail,
                    duration: 1000,
                    start: 500
                });

                await sleep(10);
                assert.equal(entries[0].name, 'detailed-measure');
                assert.equal(entries[0].duration, 1000);
                assert.equal(entries[0].startTime, 500);
                assert.deepEqual(entries[0].detail, detail);
                observer.disconnect();
            });
        });

        describe('Error Cases', () => {
            beforeEach(() => {
                performance.clearMarks();
                performance.clearMeasures();
            });

            it('should throw on invalid option combinations', () => {
                // Case 1: duration + end time
                assert.throws(() => {
                    const startTime = Math.floor(performance.now());
                    performance.measure('invalid-combo-1', {
                        start: startTime,
                        duration: 100,
                        end: startTime + 200
                    });
                });

                // Case 2: duration + end mark
                const markTime = Math.floor(performance.now());
                performance.mark('end-mark', { startTime: markTime + 200 });
                assert.throws(() => {
                    performance.measure('invalid-combo-2', {
                        start: markTime,
                        duration: 100,
                        end: 'end-mark'
                    });
                });
            });

            it('should throw on non-existent marks', () => {
                assert.throws(() => {
                    performance.measure('invalid-mark', {
                        start: 'non-existent-mark'
                    });
                });
            });

            it('measure with invalid marks should throw', () => {
                assert.throws(() => {
                    performance.measure('invalid', 'non-existent-mark');
                });
            });

            it('measure with error cases', async () => {
                var entries = [];
                const observer = new PerformanceObserver((list) => {
                    entries = entries.concat(list.getEntries());
                });
                observer.observe({ entryTypes: ['measure'] });

                // Test case 1: invalid start mark
                assert.throws(() => {
                    performance.measure('error1', {
                        start: 'non-existent-mark',
                        end: performance.now()
                    });
                });

                // Test case 2: invalid end mark
                assert.throws(() => {
                    performance.measure('error2', {
                        start: performance.now(),
                        end: 'non-existent-mark'
                    });
                });

                // Test case 3: end time before start time
                const startTime = performance.now() + 1000; // Future time
                performance.measure('error3', {
                    start: startTime,
                    end: performance.now()
                });

                // Test case 4: measure from process start
                performance.measure('from-start');

                await sleep(10);
                assert.equal(entries.length, 2); // Only error3 and from-start should succeed

                // Verify error3
                const entry1 = entries.find(e => e.name === 'error3');
                assert.ok(entry1.duration <= 0);

                // Verify from-start
                const entry2 = entries.find(e => e.name === 'from-start');
                assert.ok(entry2.duration > 0);

                observer.disconnect();
            });

            it('measure with duration and end combinations', async () => {
                var entries = [];
                const observer = new PerformanceObserver((list) => {
                    entries = entries.concat(list.getEntries());
                });
                observer.observe({ entryTypes: ['measure'] });

                // Test case 1: duration + end time (should throw)
                assert.throws(() => {
                    const startTime = Math.floor(performance.now());
                    performance.measure('duration-end-time', {
                        start: startTime,
                        duration: 100,
                        end: startTime + 200
                    });
                });

                // Test case 2: duration + end mark (should throw)
                const markTime = Math.floor(performance.now());
                performance.mark('end-point', { startTime: markTime + 200 });
                assert.throws(() => {
                    performance.measure('duration-end-mark', {
                        start: markTime,
                        duration: 100,
                        end: 'end-point'
                    });
                });

                // Test case 3: only duration (valid)
                const startTimeForDuration = Math.floor(performance.now());
                performance.measure('only-duration', {
                    start: startTimeForDuration,
                    duration: 150
                });

                // Test case 4: only end (valid)
                const startTimeForEnd = Math.floor(performance.now());
                performance.measure('only-end', {
                    start: startTimeForEnd,
                    end: startTimeForEnd + 200
                });

                await sleep(10);
                assert.equal(entries.length, 2);

                // Verify only-duration
                const entry1 = entries.find(e => e.name === 'only-duration');
                assert.equal(entry1.duration, 150);
                assert.equal(entry1.startTime, startTimeForDuration);

                // Verify only-end
                const entry2 = entries.find(e => e.name === 'only-end');
                assert.ok(Math.abs(entry2.duration - 200) < 0.0001);
                assert.equal(entry2.startTime, startTimeForEnd);

                observer.disconnect();
            });
        });

        describe('Complex Scenarios', () => {
            beforeEach(() => {
                performance.clearMarks();
                performance.clearMeasures();
            });

            it('multiple marks with same name', async () => {
                var entries;
                const observer = new PerformanceObserver((list) => {
                    entries = list.getEntries();
                });
                observer.observe({ entryTypes: ['mark'] });

                performance.mark('repeated');
                await sleep(10);
                performance.mark('repeated');

                await sleep(10);
                assert.equal(entries.length, 1);  // Latest mark replaces the previous one
                assert.equal(entries[0].name, 'repeated');
                assert.ok(entries[0].startTime >= 0, 'Mark should have valid timestamp');
                observer.disconnect();
            });

            it('measure with various option combinations', async () => {
                var entries = [];
                const observer = new PerformanceObserver((list) => {
                    entries = entries.concat(list.getEntries());
                });
                observer.observe({ entryTypes: ['measure'] });

                // Test case 1: detail with measure
                performance.measure('detail-measure', {
                    detail: { type: 'test1' }
                });

                // Test case 2: start time + end mark
                const markTime = Math.floor(performance.now());
                performance.mark('end-mark');
                performance.measure('start-end-mix', {
                    start: markTime,
                    end: 'end-mark'
                });

                // Test case 3: start mark + end time
                performance.mark('start-mark');
                const endTime = performance.now() + 100;
                performance.measure('mark-to-time', {
                    start: 'start-mark',
                    end: endTime
                });

                // Test case 4: detail + start mark + end mark
                performance.mark('complex-start', {
                    detail: { phase: 'start' }
                });
                await sleep(50);
                performance.mark('complex-end', {
                    detail: { phase: 'end' }
                });
                performance.measure('complex-measure', {
                    start: 'complex-start',
                    end: 'complex-end',
                    detail: { type: 'complex' }
                });

                await sleep(10);
                assert.equal(entries.length, 4);

                // Verify detail-measure
                const entry1 = entries.find(e => e.name === 'detail-measure');
                assert.ok(entry1.duration >= 0);
                assert.deepEqual(entry1.detail, { type: 'test1' });

                // Verify start-end-mix
                const entry2 = entries.find(e => e.name === 'start-end-mix');
                assert.equal(entry2.startTime, markTime);
                assert.ok(entry2.duration >= 0);

                // Verify mark-to-time
                const entry3 = entries.find(e => e.name === 'mark-to-time');
                assert.ok(entry3.duration >= 0);
                assert.ok(entry3.startTime <= endTime);

                // Verify complex-measure
                const entry4 = entries.find(e => e.name === 'complex-measure');
                assert.ok(entry4.duration >= 45); // Allow for some timing variance
                assert.deepEqual(entry4.detail, { type: 'complex' });

                observer.disconnect();
            });

            it('measure with relative timing combinations', async () => {
                var entries = [];
                const observer = new PerformanceObserver((list) => {
                    entries = entries.concat(list.getEntries());
                });
                observer.observe({ entryTypes: ['measure'] });

                // Create marks at specific times
                const time1 = Math.floor(performance.now());
                performance.mark('mark1', { startTime: time1 });
                await sleep(50);
                const time2 = Math.floor(performance.now());
                performance.mark('mark2', { startTime: time2 });
                await sleep(50);
                const time3 = Math.floor(performance.now());
                performance.mark('mark3', { startTime: time3 });

                // Test case 1: measure between marks
                performance.measure('between-marks', {
                    start: 'mark1',
                    end: 'mark2'
                });

                // Test case 2: measure from mark to time
                const endTime = Math.floor(performance.now());
                performance.measure('mark-to-time', {
                    start: 'mark2',
                    end: endTime
                });

                // Test case 3: measure between process start and mark
                performance.measure('process-to-mark', {
                    end: 'mark3'
                });

                // Test case 4: chained measurements
                performance.measure('chain1', {
                    start: 'mark1',
                    end: 'mark2'
                });
                performance.measure('chain2', {
                    start: 'mark2',
                    end: 'mark3'
                });

                await sleep(10);
                assert.equal(entries.length, 5);

                // Verify between-marks
                const entry1 = entries.find(e => e.name === 'between-marks');
                assert.ok(entry1.duration >= 45); // Allow for timing variance

                // Verify mark-to-time
                const entry2 = entries.find(e => e.name === 'mark-to-time');
                assert.ok(entry2.duration >= 0);

                // Verify process-to-mark
                const entry3 = entries.find(e => e.name === 'process-to-mark');
                assert.ok(entry3.duration > 0);

                // Verify chained measurements
                const entry4 = entries.find(e => e.name === 'chain1');
                const entry5 = entries.find(e => e.name === 'chain2');
                assert.ok(entry4.duration >= 45);
                assert.ok(entry5.duration >= 45);

                observer.disconnect();
            });

            it('measure with mixed mark and time options', async () => {
                var entries;
                const observer = new PerformanceObserver((list) => {
                    entries = list.getEntries();
                });
                observer.observe({ entryTypes: ['measure'] });

                performance.mark('start-point', {
                    startTime: 100
                });
                performance.measure('mixed-measure', {
                    start: 'start-point',
                    end: 500
                });

                await sleep(10);
                assert.equal(entries[0].name, 'mixed-measure');
                assert.equal(entries[0].startTime, 100);
                assert.equal(entries[0].duration, 400);
                observer.disconnect();
            });
        });
    });
});
