var { describe, it } = require('node:test');
var assert = require('assert');

var child_process = require('child_process');
var coroutine = require('coroutine');
var dgram = require('dgram');
var path = require('path');
var io = require('io');
var rtc = require('rtc');

const key_pem =
    `-----BEGIN PRIVATE KEY-----
MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQg3bbuT2SjSlMZH/J1
vHwmF0Blb/DBc/v7f1Za9GPUXHmhRANCAATDpmYxZozjVw6xlERNjJJGgfY3bEmj
xAKFRq3nbxbDHvMEs34u9HntMZWJ0hp3GUC+Ax7JHTv3cYqSaAg2SpR4
-----END PRIVATE KEY-----`

const cert_pem =
    `-----BEGIN CERTIFICATE-----
MIIBgjCCASigAwIBAgIJAPMXEoZXOaDEMAoGCCqGSM49BAMCMEoxDzANBgNVBAMM
BmNhLmNvbTELMAkGA1UEBhMCVVMxCzAJBgNVBAcMAkNBMRAwDgYDVQQKDAdleGFt
cGxlMQswCQYDVQQIDAJDQTAeFw0yNDA1MDUxNjAzMjFaFw0yNDA4MTMxNjAzMjFa
MDExCzAJBgNVBAYTAkNOMRAwDgYDVQQKDAdiYW96LmNuMRAwDgYDVQQDDAdiYW96
Lm1lMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEw6ZmMWaM41cOsZRETYySRoH2
N2xJo8QChUat528Wwx7zBLN+LvR57TGVidIadxlAvgMeyR0793GKkmgINkqUeKMQ
MA4wDAYDVR0TAQH/BAIwADAKBggqhkjOPQQDAgNIADBFAiAPNldqGJHryfjPFyX3
zfHHWlO7xSDTzdyoxzroFdwy+gIhAKmZizEVvDlBiIe+3ptCArU3dbp+bzLynTcr
Ma9ayzQy
-----END CERTIFICATE-----`

const charset = Array.from('ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/');
const genToken = (len) => [...Array(len)].map(() => charset.at(Math.floor(Math.random() * charset.length))).join('');

function munge(desc, ufrag) {
    desc.sdp = desc.sdp
        .replace(/\na=ice-ufrag:[^\n]*\n/, '\na=ice-ufrag:' + ufrag + '\n')
        .replace(/\na=ice-pwd:[^\n]*\n/, '\na=ice-pwd:' + ufrag + '\n');
    return desc;
}

function answerSdp(ip, port, ufrag, fingerprint) {
    const family = ip.indexOf(':') >= 0 ? 'IPv6' : 'IPv4';
    return {
        type: 'answer',
        sdp: `v=0\r\no=- 0 0 IN ${family} ${ip}\r\ns=-\r\nc=IN ${family} ${ip}\r\nt=0 0\r\na=ice-lite\r\nm=application ${port} UDP/DTLS/SCTP webrtc-datachannel\r\na=mid:0\r\na=setup:passive\r\na=ice-ufrag:${ufrag}\r\na=ice-pwd:${ufrag}\r\na=fingerprint:SHA-256 ${fingerprint}\r\na=sctp-port:5000\r\na=max-message-size:16384\r\na=candidate:1467250027 1 UDP 1467250027 ${ip} ${port} typ host\r\n`
    };
}

const staticFingerprint = '07E56F2A1A0C2C320EC1C39C345A784EA58B3205D157D6F4E7024112E601C68F'
    .match(/.{1,2}/g)
    .join(':')
    .toUpperCase();

function connectV1(port, fingerprint, prefix = 'libp2p+webrtc+v1/', ip = '127.0.0.1') {
    const ufrag = prefix + genToken(16);
    const options = {
        iceUfrag: ufrag,
        icePwd: ufrag,
        maxMessageSize: 16384,
        // The synthetic answer already carries the listener's host candidate,
        // so no STUN is needed; resolving the default server would block
        // gathering on a DNS lookup and make this test network dependent.
        iceServers: []
    };
    // juice never gathers loopback addresses (RFC 8445), so a host whose only
    // IPv6 address is ::1 has no IPv6 host candidate. Binding the client to
    // the loopback literal makes juice use it as that candidate.
    if (ip.indexOf(':') >= 0)
        options.bindAddress = ip;

    const pc = new rtc.RTCPeerConnection(options);

    const state = { pc, reply: null, fingerprint: null, dc: null };
    const dc = pc.createDataChannel('temp');
    state.dc = dc;
    dc.onopen = () => dc.send('hello');
    dc.onmessage = (ev) => {
        state.reply = ev.data;
    };
    pc.onsecure = (ev) => {
        state.fingerprint = ev.fingerprint;
    };

    pc.createOffer().then((offer) => {
        pc.setLocalDescription(munge(offer, ufrag));
        pc.setRemoteDescription(answerSdp(ip, port, ufrag, fingerprint));
    }).catch(() => {
    });

    return state;
}

function connectV2(port, fingerprint) {
    const clientUfrag = genToken(16);
    const clientPwd = genToken(22);
    const serverUfrag = 'libp2p+webrtc+v2/' + clientPwd;

    const pc = new rtc.RTCPeerConnection({
        iceUfrag: clientUfrag,
        icePwd: clientPwd,
        maxMessageSize: 16384,
        // See connectV1: the host candidate comes from the synthetic answer.
        iceServers: []
    });

    const state = { pc, reply: null, clientPwd, fingerprint: null, dc: null };
    const dc = pc.createDataChannel('temp');
    state.dc = dc;
    dc.onopen = () => dc.send('hello');
    dc.onmessage = (ev) => {
        state.reply = ev.data;
    };
    pc.onsecure = (ev) => {
        state.fingerprint = ev.fingerprint;
    };

    pc.createOffer().then((offer) => {
        pc.setLocalDescription(offer);
        pc.setRemoteDescription(answerSdp('127.0.0.1', port, serverUfrag, fingerprint));
    }).catch(() => {
    });

    return state;
}

function makeListener(options) {
    const events = {
        accepted: 0,
        secure: 0,
        errors: 0,
        lastRequest: null,
        fingerprints: []
    };

    const listener = new rtc.RTCListener(Object.assign({
        certPem: cert_pem,
        keyPem: key_pem,
        maxConnections: 8
    }, options));

    listener.on('connection', (ev) => {
        events.accepted++;
        events.lastRequest = ev.request;

        const pc = ev.peerConnection;
        pc.ondatachannel = (dcEv) => {
            const dc = dcEv.channel;
            dc.onmessage = (msg) => {
                dc.send(msg.data);
            };
        };
        pc.onsecure = (se) => {
            events.secure++;
            events.fingerprints.push(se.fingerprint);
        };
    });
    listener.on('error', () => {
        events.errors++;
    });

    return { listener, events };
}

function waitFor(predicate, timeout = 8000) {
    const deadline = Date.now() + timeout;
    while (!predicate() && Date.now() < deadline)
        coroutine.sleep(50);
    return predicate();
}

describe('rtc listener', function () {
    it('accepts a v1 direct connection', function () {
        const { listener, events } = makeListener();
        listener.listen(4351, '127.0.0.1');

        const client = connectV1(4351, staticFingerprint);
        assert.equal(waitFor(() => client.reply !== null && client.fingerprint !== null), true);
        assert.equal(client.reply, 'hello');
        assert.equal(events.accepted, 1);
        assert.equal(events.lastRequest.version, 1);
        assert.equal(events.lastRequest.serverUfrag, events.lastRequest.clientUfrag);
        assert.equal(events.lastRequest.clientPwd, events.lastRequest.clientUfrag);
        assert.equal(events.lastRequest.remoteAddress, '127.0.0.1');
        assert.equal(events.lastRequest.remotePort > 0, true);
        assert.equal(events.lastRequest.localPort, 4351);
        assert.equal(events.secure, 1);
        assert.equal(events.fingerprints.length, 1);
        assert.equal(events.fingerprints[0].algorithm, 'SHA-256');
        assert.equal(events.fingerprints[0].fingerprint.split(':').length, 32);
        // The client verifies the listener certificate it was given through the
        // synthetic answer, so both fingerprints must be the same one.
        assert.equal(client.fingerprint.fingerprint, listener.certificate.fingerprint);

        client.pc.close();
        listener.close();
    });

    it('accepts a v2 direct connection', function () {
        const { listener, events } = makeListener();
        listener.listen(4352, '127.0.0.1');

        const client = connectV2(4352, staticFingerprint);
        assert.equal(waitFor(() => client.reply !== null && client.fingerprint !== null), true);
        assert.equal(client.reply, 'hello');
        assert.equal(events.accepted, 1);
        assert.equal(events.lastRequest.version, 2);
        assert.equal(events.lastRequest.serverUfrag.indexOf('libp2p+webrtc+v2/'), 0);
        assert.equal(events.lastRequest.clientPwd, client.clientPwd);
        assert.equal(events.lastRequest.remoteAddress, '127.0.0.1');
        assert.equal(events.lastRequest.localPort, 4352);
        assert.equal(client.fingerprint.fingerprint, listener.certificate.fingerprint);

        client.pc.close();
        listener.close();
    });

    it('drops peers rejected silently by authorize', function () {
        const { listener, events } = makeListener({
            authorize: () => false
        });
        listener.listen(4366, '127.0.0.1');

        const client = connectV1(4366, staticFingerprint);
        coroutine.sleep(500);

        assert.equal(events.accepted, 0);
        assert.equal(events.errors, 0);

        client.pc.close();
        listener.close();
    });

    it('enforces maxConnectionsPerAddress', function () {
        const { listener, events } = makeListener({
            maxConnections: 8,
            maxConnectionsPerAddress: 1
        });
        listener.listen(4367, '127.0.0.1');

        const first = connectV1(4367, staticFingerprint);
        assert.equal(waitFor(() => first.reply !== null), true);

        const second = connectV1(4367, staticFingerprint);
        coroutine.sleep(1000);

        assert.equal(events.accepted, 1);

        // Free the slot: the blocked peer retries and must be accepted now.
        first.pc.close();
        assert.equal(waitFor(() => second.reply !== null, 6000), true);
        assert.equal(events.accepted, 2);

        second.pc.close();
        listener.close();
    });

    it('keeps the authorize policy across close and re-listen', function () {
        let calls = 0;
        const { listener, events } = makeListener({
            authorize: () => {
                calls++;
                return true;
            }
        });

        listener.listen(4368, '127.0.0.1');
        const first = connectV1(4368, staticFingerprint);
        assert.equal(waitFor(() => first.reply !== null), true);
        first.pc.close();

        coroutine.sleep(300);
        listener.close();
        listener.listen(4368, '127.0.0.1');

        const second = connectV1(4368, staticFingerprint);
        assert.equal(waitFor(() => second.reply !== null), true);
        assert.equal(events.accepted, 2);
        assert.equal(calls >= 2, true);

        second.pc.close();
        listener.close();
    });

    it('rejects invalid options and bind addresses', function () {
        assert.throws(() => new rtc.RTCListener({ authorize: 1 }));
        assert.throws(() => new rtc.RTCListener({ certPem: cert_pem }));
        assert.throws(() => new rtc.RTCListener({ certPem: 'bad', keyPem: 'bad' }));
        assert.throws(() => new rtc.RTCListener({ maxConnections: -1 }));
        assert.throws(() => new rtc.RTCListener({ maxConnectionsPerAddress: -1 }));
        assert.throws(() => new rtc.RTCListener({ maxMessageSize: 0 }));

        const { listener } = makeListener();
        assert.throws(() => listener.listen(4369, '192.0.2.1'));
        assert.throws(() => listener.listen(70000, '127.0.0.1'));

        // A failed bind must not leave the listener half-bound.
        listener.listen(4369, '127.0.0.1');
        listener.close();
    });

    it('reports a port already in use', function () {
        const blocker = dgram.createSocket('udp4');
        blocker.bind(4370, '127.0.0.1');

        try {
            assert.throws(() => {
                const { listener } = makeListener();
                listener.listen(4370, '127.0.0.1');
            });
        } finally {
            blocker.close();
        }
    });

    it('drops malformed datagrams and keeps serving', function () {
        const { listener, events } = makeListener();
        listener.listen(4371, '127.0.0.1');

        const socket = dgram.createSocket('udp4');
        socket.send(Buffer.from('not a stun packet'), 4371, '127.0.0.1');
        socket.send(Buffer.alloc(20, 0xff), 4371, '127.0.0.1');

        // A syntactically valid STUN binding request without a USERNAME.
        const stun = Buffer.alloc(20);
        stun[1] = 1;
        stun.writeUInt32BE(0x2112A442, 4);
        socket.send(stun, 4371, '127.0.0.1');

        // Binding requests whose USERNAME carries invalid ICE credentials.
        function stunUsername(value) {
            const padded = Math.ceil(value.length / 4) * 4;
            const packet = Buffer.alloc(20 + 4 + padded);
            packet[1] = 1;
            packet.writeUInt16BE(4 + padded, 2);
            packet.writeUInt32BE(0x2112A442, 4);
            packet.writeUInt16BE(0x0006, 20);
            packet.writeUInt16BE(value.length, 22);
            packet.write(value, 24);
            return packet;
        }
        socket.send(stunUsername('a:b'), 4371, '127.0.0.1');
        socket.send(stunUsername('bad\x01:credentials'), 4371, '127.0.0.1');
        socket.send(stunUsername('libp2p+webrtc+v2/short:clientufrag'), 4371, '127.0.0.1');

        coroutine.sleep(100);
        socket.close();

        const client = connectV1(4371, staticFingerprint);
        assert.equal(waitFor(() => client.reply !== null), true);
        assert.equal(events.accepted, 1);
        assert.equal(events.errors, 0);

        client.pc.close();
        listener.close();
    });

    it('enforces the global connection limit', function () {
        const { listener, events } = makeListener({
            maxConnections: 1,
            maxConnectionsPerAddress: 8
        });
        listener.listen(4372, '127.0.0.1');

        const first = connectV1(4372, staticFingerprint);
        assert.equal(waitFor(() => first.reply !== null), true);

        const second = connectV1(4372, staticFingerprint);
        coroutine.sleep(800);

        assert.equal(events.accepted, 1);

        first.pc.close();
        second.pc.close();
        listener.close();
    });

    it('treats a promise-returning authorize as a rejection', function () {
        const { listener, events } = makeListener({
            authorize: () => Promise.resolve(true)
        });
        listener.listen(4373, '127.0.0.1');

        const client = connectV1(4373, staticFingerprint);
        coroutine.sleep(500);

        assert.equal(events.accepted, 0);
        assert.equal(events.errors, 0);

        client.pc.close();
        listener.close();
    });

    it('keeps accepted connections running after the listener closes', function () {
        const { listener, events } = makeListener();
        listener.listen(4374, '127.0.0.1');

        const client = connectV1(4374, staticFingerprint);

        // Close the listener while the accepted connection is still doing its
        // DTLS/SCTP handshake: the session must finish and keep working.
        assert.equal(waitFor(() => events.accepted === 1), true);
        listener.close();

        assert.equal(waitFor(() => client.reply !== null), true);

        client.reply = null;
        client.dc.send('after-close');
        assert.equal(waitFor(() => client.reply === 'after-close'), true);
        assert.equal(events.accepted, 1);

        client.pc.close();
    });

    it('keeps an accepted session alive across a collection', function () {
        const { listener, events } = makeListener();
        listener.listen(4381, '127.0.0.1');

        // The connection handler never keeps ev.peerConnection, so only the
        // accepted connection itself keeps the session alive. A collection
        // must not close it before the transport does.
        const client = connectV1(4381, staticFingerprint);
        assert.equal(waitFor(() => client.reply !== null), true);

        gc();
        coroutine.sleep(50);
        gc();

        client.reply = null;
        client.dc.send('after-gc');
        assert.equal(waitFor(() => client.reply === 'after-gc'), true);
        assert.equal(events.accepted, 1);

        client.pc.close();
        listener.close();
    });

    it('treats an unknown ufrag prefix as v1', function () {
        const { listener, events } = makeListener();
        listener.listen(4376, '127.0.0.1');

        const client = connectV1(4376, staticFingerprint, 'custom/');
        assert.equal(waitFor(() => client.reply !== null), true);
        assert.equal(client.reply, 'hello');
        assert.equal(events.accepted, 1);
        assert.equal(events.lastRequest.version, 1);
        assert.equal(events.lastRequest.serverUfrag.indexOf('custom/'), 0);

        client.pc.close();
        listener.close();
    });

    it('serves several listeners in one process', function () {
        const first = makeListener();
        const second = makeListener();
        first.listener.listen(4353, '127.0.0.1');
        second.listener.listen(4354, '127.0.0.1');

        assert.equal(first.listener.address().port, 4353);
        assert.equal(second.listener.address().port, 4354);

        const clientA = connectV1(4353, staticFingerprint);
        const clientB = connectV1(4354, staticFingerprint);
        assert.equal(waitFor(() => clientA.reply !== null && clientB.reply !== null), true);
        assert.equal(clientA.reply, 'hello');
        assert.equal(clientB.reply, 'hello');
        assert.equal(first.events.accepted, 1);
        assert.equal(second.events.accepted, 1);

        clientA.pc.close();
        clientB.pc.close();
        first.listener.close();
        second.listener.close();
    });

    it('generates a certificate and rejects a duplicate port', function () {
        const generated = makeListener({ certPem: undefined, keyPem: undefined });
        generated.listener.listen(4355, '127.0.0.1');

        const certificate = generated.listener.certificate;
        assert.equal(certificate.fingerprint.length, 95);
        assert.equal(certificate.certPem.indexOf('BEGIN CERTIFICATE') > 0, true);

        const client = connectV1(4355, certificate.fingerprint);
        assert.equal(waitFor(() => client.reply !== null && client.fingerprint !== null), true);
        assert.equal(client.reply, 'hello');
        assert.equal(client.fingerprint.fingerprint, certificate.fingerprint);

        assert.throws(() => {
            const duplicate = new rtc.RTCListener({ certPem: cert_pem, keyPem: key_pem });
            duplicate.listen(4355, '127.0.0.1');
        });

        client.pc.close();
        generated.listener.close();
    });

    it('drops peers rejected by authorize', function () {
        const { listener, events } = makeListener({
            authorize: () => {
                throw new Error('denied');
            }
        });
        listener.listen(4356, '127.0.0.1');

        const client = connectV1(4356, staticFingerprint);
        coroutine.sleep(500);

        assert.equal(events.accepted, 0);
        assert.equal(events.errors, 1);

        client.pc.close();
        listener.close();
    });

    it('can be closed and bound again', function () {
        const { listener, events } = makeListener();
        listener.listen(4361, '127.0.0.1');
        listener.close();
        listener.listen(4361, '127.0.0.1');

        const client = connectV1(4361, staticFingerprint);
        assert.equal(waitFor(() => client.reply !== null), true);
        assert.equal(client.reply, 'hello');
        assert.equal(events.accepted, 1);

        client.pc.close();
        listener.close();
    });

    it('can be closed while a request is pending', function () {
        const { listener } = makeListener();
        listener.listen(4362, '127.0.0.1');

        const client = connectV1(4362, staticFingerprint);
        coroutine.sleep(10);
        listener.close();
        coroutine.sleep(100);

        client.pc.close();
    });

    it('releases connection slots when sessions close', function () {
        const { listener, events } = makeListener({ maxConnections: 1 });
        listener.listen(4363, '127.0.0.1');

        const first = connectV1(4363, staticFingerprint);
        assert.equal(waitFor(() => first.reply !== null), true);
        first.pc.close();

        // The close callback is asynchronous; give the listener time to free
        // the slot before dialing again.
        coroutine.sleep(500);
        const second = connectV1(4363, staticFingerprint);
        assert.equal(waitFor(() => second.reply !== null), true);
        assert.equal(events.accepted, 2);

        second.pc.close();
        listener.close();
    });

    it('accepts several data channels on one connection', function () {
        const { listener, events } = makeListener();
        listener.listen(4364, '127.0.0.1');

        const client = connectV1(4364, staticFingerprint);
        assert.equal(waitFor(() => client.reply !== null), true);

        for (let i = 0; i < 10; i++) {
            const dc = client.pc.createDataChannel('channel_' + i);
            let msg = '';
            dc.onopen = () => dc.send('hello');
            dc.onmessage = (ev) => {
                msg = ev.data;
            };
            assert.equal(waitFor(() => msg === 'hello'), true, 'channel ' + i);
        }
        assert.equal(events.accepted, 1);

        client.pc.close();
        listener.close();
    });

    it('accepts a direct connection over IPv6 loopback', function () {
        const { listener, events } = makeListener();
        try {
            listener.listen(4377, '::1');
        } catch (e) {
            return; // IPv6 loopback is not available in this environment
        }

        assert.equal(listener.address().address, '::1');
        assert.equal(listener.address().family, 'IPv6');

        const client = connectV1(4377, staticFingerprint, 'libp2p+webrtc+v1/', '::1');
        assert.equal(waitFor(() => client.reply !== null), true);
        assert.equal(client.reply, 'hello');
        assert.equal(events.accepted, 1);
        assert.equal(events.lastRequest.remoteAddress, '::1');
        assert.equal(events.lastRequest.localPort, 4377);

        client.pc.close();
        listener.close();
    });

    it('reports an empty bind address as dual-stack', function () {
        const { listener } = makeListener();
        listener.listen(4378);

        const addr = listener.address();
        assert.equal(addr.address, '::');
        assert.equal(addr.family, 'IPv6');

        listener.close();
    });

    it('tears down a bound listener when the process exits', function () {
        const p = child_process.spawn(process.execPath, [
            path.join(__dirname, 'rtc_files', 'listener_exit.js')
        ]);
        const stdout = new io.BufferedStream(p.stdout);
        assert.deepEqual(stdout.readLines(), ['listening']);
    });

    it('can be re-bound while an accepted session is active', function () {
        const { listener, events } = makeListener();
        listener.listen(4380, '127.0.0.1');

        const first = connectV1(4380, staticFingerprint);
        assert.equal(waitFor(() => first.reply !== null), true);

        listener.close();
        listener.listen(4380, '127.0.0.1');

        const second = connectV1(4380, staticFingerprint);
        assert.equal(waitFor(() => second.reply !== null), true);
        assert.equal(events.accepted, 2);

        // The session accepted before the close still works.
        first.reply = null;
        first.dc.send('still-alive');
        assert.equal(waitFor(() => first.reply === 'still-alive'), true);

        first.pc.close();
        second.pc.close();
        listener.close();
    });

    it('validates listen and address', function () {
        const { listener } = makeListener();

        assert.throws(() => listener.address());
        assert.throws(() => listener.listen(0, '127.0.0.1'));

        listener.listen(4357, '127.0.0.1');
        assert.equal(listener.address().address, '127.0.0.1');
        assert.equal(listener.address().family, 'IPv4');
        assert.throws(() => listener.listen(4358, '127.0.0.1'));

        listener.close();
        listener.close();
        assert.throws(() => listener.address());

        listener.listen(4375, '127.0.0.1');
        assert.equal(listener.stop(), listener);
        assert.throws(() => listener.address());
    });

    it('releases the process when closed', function () {
        const p = child_process.spawn(process.execPath, [
            path.join(__dirname, 'rtc_files', 'listener_hold.js')
        ]);
        const stdout = new io.BufferedStream(p.stdout);
        assert.deepEqual(stdout.readLines(), ['listening', 'closed']);
    });
});
