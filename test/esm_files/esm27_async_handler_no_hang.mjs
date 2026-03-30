import http from 'http';
import coroutine from 'coroutine';

const server = new http.Server(0, {
    '/async': async (req) => {
        req.response.json({ ok: true });
    }
});

server.start();
const port = server.socket.localPort;

coroutine.sleep(20);

let ok = false;

try {
    const resp = http.getSync(`http://127.0.0.1:${port}/async`, { timeout: 3000 });
    ok = resp.statusCode === 200 && resp.json().ok === true;
} finally {
    server.stop();
}

if (!ok)
    throw new Error('async handler response failed or timed out');

export default ok;
