var test = require("test");
test.setup();

var test_util = require('./test_util');

var sse = require('sse');
var http = require('http');
var coroutine = require("coroutine");

var base_port = coroutine.vmid * 10000;

describe("sse", () => {
    after(test_util.cleanup);

    describe('EventSource', () => {
        var svr;

        before(() => {
            svr = new http.Server(8887 + base_port, function (req) {
                req.response.setHeader('Content-Type', 'text/event-stream');
                req.response.keepAlive = false;

                switch (req.address) {
                    case '/hello':
                        req.response.write('data: hello\n\n');
                        break;

                    case '/custom-event':
                        req.response.write('event: custom\ndata: custom data\n\n');
                        break;

                    case '/with-id':
                        req.response.write('id: 1\ndata: message with id\n\n');
                        break;

                    case '/retry':
                        req.response.write('retry: 1000\ndata: retry message\n\n');
                        break;

                    case '/multi-line':
                        req.response.write('data: line1\ndata: line2\n\n');
                        break;

                    case '/wrong-content-type':
                        req.response.setHeader('Content-Type', 'text/plain');
                        req.response.write('data: content type error\n\n');
                        break;

                    case '/error-status':
                        req.response.status = 404;
                        req.response.write('data: status error\n\n');
                        break;

                    case '/comments':
                        req.response.write(': this is comment\ndata: real data\n\n');
                        break;

                    case '/properties':
                        req.response.write('data: test properties\n\n');
                        break;
                }
            });

            svr.start();

            test_util.push(svr.socket);
        });

        function get_event(url) {
            return new Promise((resolve, reject) => {
                const es = new sse.EventSource(url);

                es.onmessage = (e) => {
                    resolve(e.data);
                };

                es.onerror = (e) => {
                    reject(e);
                };
            });
        }

        function get_event_with_type(url, eventType) {
            return new Promise((resolve, reject) => {
                const es = new sse.EventSource(url);

                es.addEventListener(eventType, (e) => {
                    resolve(e.data);
                });

                es.onerror = (e) => {
                    reject(e);
                };
            });
        }

        function get_event_with_id(url) {
            return new Promise((resolve, reject) => {
                const es = new sse.EventSource(url);

                es.onmessage = (e) => {
                    resolve({ data: e.data, id: e.id });
                };

                es.onerror = (e) => {
                    reject(e);
                };
            });
        }

        function get_properties(url) {
            return new Promise((resolve, reject) => {
                const es = new sse.EventSource(url);

                es.onmessage = () => {
                    resolve({
                        readyState: es.readyState,
                        url: es.url,
                        withCredentials: es.withCredentials,
                        response: es.response
                    });
                    es.close();
                };

                es.onerror = (e) => {
                    reject(e);
                };
            });
        }

        it('basic message', async () => {
            assert.equal(await get_event(`http://127.0.0.1:${8887 + base_port}/hello`), 'hello');
        });

        it('custom event', async () => {
            assert.equal(
                await get_event_with_type(`http://127.0.0.1:${8887 + base_port}/custom-event`, "custom"),
                'custom data'
            );
        });

        it('message with id', async () => {
            const result = await get_event_with_id(`http://127.0.0.1:${8887 + base_port}/with-id`);
            assert.equal(result.data, 'message with id');
            assert.equal(result.id, '1');
        });

        it('multi-line data', async () => {
            assert.equal(
                await get_event(`http://127.0.0.1:${8887 + base_port}/multi-line`),
                'line1\nline2'
            );
        });

        it('wrong content type error', async () => {
            try {
                await get_event(`http://127.0.0.1:${8887 + base_port}/wrong-content-type`);
                assert.fail('should throw an error');
            } catch (e) {
                assert.equal(e.reason, 'Invalid Content-Type: text/plain');
                assert.equal(e.target.response.body.readAll().toString(), 'data: content type error\n\n');
            }
        });

        it('error status', async () => {
            try {
                await get_event(`http://127.0.0.1:${8887 + base_port}/error-status`);
                assert.fail('should throw an error');
            } catch (e) {
                assert.equal(e.reason, 'Invalid status: File Not Found');
                assert.equal(e.target.response.body.readAll().toString(), 'data: status error\n\n');
            }
        });

        it('should ignore comment lines', async () => {
            assert.equal(
                await get_event(`http://127.0.0.1:${8887 + base_port}/comments`),
                'real data'
            );
        });

        it('should have correct properties', async () => {
            const props = await get_properties(`http://127.0.0.1:${8887 + base_port}/properties`);

            // assert.equal(props.readyState, 1);
            assert.equal(props.url, `http://127.0.0.1:${8887 + base_port}/properties`);
            assert.equal(props.withCredentials, false);
            assert.notEqual(props.response, null);
            assert.equal(props.response.status, 200);
        });

        it('should throw error when calling send on client EventSource', () => {
            const es = new sse.EventSource(`http://127.0.0.1:${8887 + base_port}/hello`);
            
            assert.throws(() => {
                es.send('test data');
            }, Error);
            
            es.close();
        });
    });

    describe('upgrade', () => {
        var svr;

        before(() => {
            svr = new http.Server(8888 + base_port, {
                "/sse": sse.upgrade((se, req) => {
                    // Test basic send
                    se.send(new Date().toISOString(), {
                        id: '1',
                        retry: 1000
                    });
                    se.close();
                }),

                "/sse-custom-event": sse.upgrade((se, req) => {
                    // Test custom event type
                    se.send('custom data', {
                        event: 'custom',
                        id: '2'
                    });
                    se.close();
                }),

                "/sse-multi-line": sse.upgrade((se, req) => {
                    // Test multi-line data
                    se.send('line1\nline2\nline3', {
                        id: '3'
                    });
                    se.close();
                }),

                "/sse-no-options": sse.upgrade((se, req) => {
                    // Test send without options
                    se.send('simple data');
                    se.close();
                }),

                "/sse-multiple-sends": sse.upgrade((se, req) => {
                    // Test multiple sends
                    se.send('first message', { id: '1' });
                    se.send('second message', { id: '2' });
                    se.send('third message', { id: '3' });
                    se.close();
                }),

                "/sse-retry-test": sse.upgrade((se, req) => {
                    // Test retry option
                    se.send('retry test', {
                        id: '1',
                        retry: 5000
                    });
                    se.close();
                }),

                "/sse-invalid-retry": sse.upgrade((se, req) => {
                    // Test invalid retry value
                    try {
                        se.send('invalid retry', {
                            retry: -100
                        });
                    } catch (e) {
                        se.send('error caught: ' + e.message);
                    }
                    se.close();
                }),

                "/sse-empty-data": sse.upgrade((se, req) => {
                    // Test empty data
                    se.send('', {
                        id: 'empty'
                    });
                    se.close();
                }),

                "/sse-data-with-empty-lines": sse.upgrade((se, req) => {
                    // Test data with empty lines
                    se.send('line1\n\nline3\n\n\nline6', {
                        id: 'empty-lines'
                    });
                    se.close();
                }),

                "/sse-only-newlines": sse.upgrade((se, req) => {
                    // Test data that is only newlines
                    se.send('\n\n\n', {
                        id: 'only-newlines'
                    });
                    se.close();
                }),

                "/sse-mixed-empty-content": sse.upgrade((se, req) => {
                    // Test mixed empty and non-empty data
                    se.send('', { id: '1' }); // Empty data
                    se.send('content', { id: '2' }); // Normal data
                    se.send('\n', { id: '3' }); // Single newline
                    se.send('\n\n', { id: '4' }); // Double newline
                    se.send('before\n\nafter', { id: '5' }); // Data with empty line
                    se.close();
                }),

                "/sse-explicit-empty-lines": sse.upgrade((se, req) => {
                    // Test explicit empty lines by sending data with actual empty content
                    se.send('line1\n\nline3', { id: 'explicit' });
                    se.close();
                })
            });

            svr.start();
            test_util.push(svr.socket);
        });

        function get_upgrade_event(url) {
            return new Promise((resolve, reject) => {
                const es = new sse.EventSource(url);

                es.onmessage = (e) => {
                    resolve({
                        data: e.data,
                        id: e.id || null,
                        type: e.type
                    });
                    es.close();
                };

                es.onerror = (e) => {
                    reject(e);
                };
            });
        }

        function get_upgrade_event_with_type(url, eventType) {
            return new Promise((resolve, reject) => {
                const es = new sse.EventSource(url);

                es.addEventListener(eventType, (e) => {
                    resolve({
                        data: e.data,
                        id: e.id || null,
                        type: e.type
                    });
                    es.close();
                });

                es.onerror = (e) => {
                    reject(e);
                };
            });
        }

        function get_all_upgrade_events(url, expectedCount = 1) {
            return new Promise((resolve, reject) => {
                const es = new sse.EventSource(url);
                const events = [];

                es.onmessage = (e) => {
                    events.push({
                        data: e.data,
                        id: e.id || null,
                        type: e.type
                    });

                    if (events.length >= expectedCount) {
                        resolve(events);
                        es.close();
                    }
                };

                es.onerror = (e) => {
                    reject(e);
                };

                // Safety timeout
                setTimeout(() => {
                    if (events.length > 0) {
                        resolve(events);
                        es.close();
                    } else {
                        reject(new Error('Timeout waiting for events'));
                    }
                }, 2000);
            });
        }

        it('basic upgrade and send', async () => {
            const result = await get_upgrade_event(`http://127.0.0.1:${8888 + base_port}/sse`);
            // Check that we get a timestamp-like string
            assert.match(result.data, /\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}/);
            assert.equal(result.id, '1');
        });

        it('custom event type in upgrade', async () => {
            const result = await get_upgrade_event_with_type(`http://127.0.0.1:${8888 + base_port}/sse-custom-event`, 'custom');
            assert.equal(result.data, 'custom data');
            assert.equal(result.id, '2');
            assert.equal(result.type, 'custom');
        });

        it('multi-line data in upgrade', async () => {
            const result = await get_upgrade_event(`http://127.0.0.1:${8888 + base_port}/sse-multi-line`);
            assert.equal(result.data, 'line1\nline2\nline3');
            assert.equal(result.id, '3');
        });

        it('send without options', async () => {
            const result = await get_upgrade_event(`http://127.0.0.1:${8888 + base_port}/sse-no-options`);
            assert.equal(result.data, 'simple data');
            assert.equal(result.id, null);
        });

        it('multiple sends in one connection', async () => {
            const events = await get_all_upgrade_events(`http://127.0.0.1:${8888 + base_port}/sse-multiple-sends`, 3);
            assert.equal(events.length, 3);
            assert.equal(events[0].data, 'first message');
            assert.equal(events[0].id, '1');
            assert.equal(events[1].data, 'second message');
            assert.equal(events[1].id, '2');
            assert.equal(events[2].data, 'third message');
            assert.equal(events[2].id, '3');
        });

        it('retry option in upgrade', async () => {
            const result = await get_upgrade_event(`http://127.0.0.1:${8888 + base_port}/sse-retry-test`);
            assert.equal(result.data, 'retry test');
            assert.equal(result.id, '1');
        });

        it('invalid retry value should cause error', async () => {
            const result = await get_upgrade_event(`http://127.0.0.1:${8888 + base_port}/sse-invalid-retry`);
            assert.match(result.data, /error caught.*retry must be a positive integer/);
        });

        it('empty data', async () => {
            const result = await get_upgrade_event(`http://127.0.0.1:${8888 + base_port}/sse-empty-data`);
            assert.equal(result.data, '');
            assert.equal(result.id, 'empty');
        });

        it('data with empty lines', async () => {
            const result = await get_upgrade_event(`http://127.0.0.1:${8888 + base_port}/sse-data-with-empty-lines`);
            assert.equal(result.data, 'line1\n\nline3\n\n\nline6');
            assert.equal(result.id, 'empty-lines');
        });

        it('data that is only newlines', async () => {
            const result = await get_upgrade_event(`http://127.0.0.1:${8888 + base_port}/sse-only-newlines`);
            // When data contains only newlines, each newline becomes an empty data line
            // Multiple empty data lines result in empty strings joined by newlines, which results in empty string
            assert.equal(result.data, '');
            assert.equal(result.id, 'only-newlines');
        });

        it('mixed empty and non-empty content', async () => {
            const events = await get_all_upgrade_events(`http://127.0.0.1:${8888 + base_port}/sse-mixed-empty-content`, 5);
            assert.equal(events.length, 5);
            
            // Test empty data
            assert.equal(events[0].data, '');
            assert.equal(events[0].id, '1');
            
            // Test normal data
            assert.equal(events[1].data, 'content');
            assert.equal(events[1].id, '2');
            
            // Test single newline - becomes empty data line
            assert.equal(events[2].data, '');
            assert.equal(events[2].id, '3');
            
            // Test double newline - becomes empty data lines
            assert.equal(events[3].data, '');
            assert.equal(events[3].id, '4');
            
            // Test data with empty line in middle
            assert.equal(events[4].data, 'before\n\nafter');
            assert.equal(events[4].id, '5');
        });

        it('explicit empty lines preservation', async () => {
            const result = await get_upgrade_event(`http://127.0.0.1:${8888 + base_port}/sse-explicit-empty-lines`);
            // This should preserve the empty line in the middle
            assert.equal(result.data, 'line1\n\nline3');
            assert.equal(result.id, 'explicit');
        });

        it('direct http request to upgrade endpoint', () => {
            // Test direct HTTP request to see the raw response
            const res = http.get(`http://127.0.0.1:${8888 + base_port}/sse`);
            const body = res.body.read().toString();
            
            // Should contain standard SSE format
            assert.notEqual(body.indexOf('id: 1'), -1);
            assert.notEqual(body.indexOf('retry: 1000'), -1);
            assert.notEqual(body.indexOf('data: '), -1);
            // Should end with double newline as per SSE spec
            assert.match(body, /\n\n$/);
            // Should be a valid timestamp
            assert.match(body, /data: \d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}/);
        });
    });
});

