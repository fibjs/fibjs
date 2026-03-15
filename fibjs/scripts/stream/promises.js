'use strict';

// Promise-based stream utilities (Node.js stream/promises compatibility)

/**
 * Returns a Promise that is fulfilled when the stream is no longer readable,
 * writable, or has experienced an error or a premature close event.
 */
function finished(stream, opts) {
    return new Promise((resolve, reject) => {
        if (opts && opts.signal) {
            if (opts.signal.aborted) {
                return reject(new Error('The operation was aborted'));
            }
            opts.signal.addEventListener('abort', () => {
                reject(new Error('The operation was aborted'));
            });
        }

        const cleanup = () => {
            stream.removeListener('finish', onfinish);
            stream.removeListener('end', onend);
            stream.removeListener('error', onerror);
            stream.removeListener('close', onclose);
        };

        const onfinish = () => { cleanup(); resolve(); };
        const onend = () => { cleanup(); resolve(); };
        const onerror = (err) => { cleanup(); reject(err); };
        const onclose = () => {
            cleanup();
            // If already ended/finished, resolve; otherwise reject
            const ws = stream._writableState;
            const rs = stream._readableState;
            if ((ws && ws.finished) || (rs && rs.ended)) {
                resolve();
            } else if (!ws && !rs) {
                // Native stream (AsyncStream) — close always follows end/finish
                resolve();
            } else {
                reject(new Error('stream was destroyed'));
            }
        };

        // Determine stream type
        const readable = stream._readableState !== undefined;
        const writable = stream._writableState !== undefined;

        if (writable) {
            // Wait for 'finish' event on writable streams
            if (stream._writableState.finished) {
                return process.nextTick(resolve);
            }
            stream.on('finish', onfinish);
        } else if (readable) {
            // Wait for 'end' event on readable streams
            if (stream._readableState.ended) {
                return process.nextTick(resolve);
            }
            stream.on('end', onend);
        } else {
            // Native stream (AsyncStream) — listen for both end and finish
            stream.on('end', onend);
            stream.on('finish', onfinish);
        }

        stream.on('error', onerror);
        stream.on('close', onclose);
    });
}

/**
 * Returns a Promise that is fulfilled when all streams in the pipeline have
 * finished processing.
 */
function pipeline(...streams) {
    return new Promise((resolve, reject) => {
        let signal;
        const lastArg = streams[streams.length - 1];
        if (lastArg && typeof lastArg === 'object' && !lastArg._readableState && !lastArg._writableState) {
            streams = streams.slice(0, -1);
            if (lastArg.signal) {
                signal = lastArg.signal;
            }
        }

        if (streams.length < 2) {
            return reject(new Error('pipeline requires at least 2 streams'));
        }

        let error;
        let cleanup;
        let destroyedCount = 0;

        const destroy = (err) => {
            if (err && !error) error = err;
            streams.forEach(s => {
                if (s && typeof s.destroy === 'function') s.destroy(err);
            });
        };

        const onfinish = () => {
            if (error) reject(error);
            else resolve();
        };

        let current = streams[0];
        for (let i = 1; i < streams.length; i++) {
            const next = streams[i];
            current.on('error', destroy);
            if (i === streams.length - 1) {
                next.on('error', destroy);
                next.on('finish', onfinish);
                next.on('end', onfinish);
            }
            current.pipe(next);
            current = next;
        }

        if (signal) {
            signal.addEventListener('abort', () => destroy(new Error('The operation was aborted')));
        }
    });
}

module.exports = { finished, pipeline };
