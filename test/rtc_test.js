var { describe, it } = require('node:test');
var assert = require('assert');

var test_util = require('./test_util');
const coroutine = require('coroutine');
const rtc = require("rtc");

function catchOf(fn) {
    try {
        fn();
    } catch (e) {
        return e;
    }
    return null;
}

function munge(desc, ufrag) {
    assert.ok(desc.sdp !== undefined, "Can't munge a missing SDP");

    desc.sdp = desc.sdp
        .replace(/\na=ice-ufrag:[^\n]*\n/, '\na=ice-ufrag:' + ufrag + '\n')
        .replace(/\na=ice-pwd:[^\n]*\n/, '\na=ice-pwd:' + ufrag + '\n');
    return desc;
}

function answerSdp(ip, port, ufrag) {
    const family = ip.indexOf(':') >= 0 ? 'IPv6' : 'IPv4';
    const certhash = '07E56F2A1A0C2C320EC1C39C345A784EA58B3205D157D6F4E7024112E601C68F';
    const fingerprint = certhash.match(/.{1,2}/g).join(':').toUpperCase();
    return {
        "type": "answer",
        "sdp": `v=0\r\no=- 0 0 IN ${family} ${ip}\r\ns=-\r\nc=IN ${family} ${ip}\r\nt=0 0\r\na=ice-lite\r\nm=application ${port} UDP/DTLS/SCTP webrtc-datachannel\r\na=mid:0\r\na=setup:passive\r\na=ice-ufrag:${ufrag}\r\na=ice-pwd:${ufrag}\r\na=fingerprint:SHA-256 ${fingerprint}\r\na=sctp-port:5000\r\na=max-message-size:16384\r\na=candidate:1467250027 1 UDP 1467250027 ${ip} ${port} typ host\r\n`
    };
}

const charset = Array.from('ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/');
const genUfrag = (len) => [...Array(len)].map(() => charset.at(Math.floor(Math.random() * charset.length))).join('');

function connect(ip, port, ufrag) {
    const peerConnection = new rtc.RTCPeerConnection({
        iceUfrag: ufrag,
        icePwd: ufrag,
        maxMessageSize: 16384
    });

    peerConnection.createDataChannel('temp');

    peerConnection.createOffer().then(offerSdp => {
        const mungedOfferSdp = munge(offerSdp, ufrag);
        peerConnection.setLocalDescription(mungedOfferSdp);

        peerConnection.setRemoteDescription(answerSdp(ip, port, ufrag));
    }).catch(e => {
        console.error(e);
    });

    return peerConnection;
}

describe('rtc', function () {
    it('connect timeout', function () {
        const ufrag = 'libp2p+webrtc+v1/' + genUfrag(16);
        const peerConnection = connect('127.0.0.1', 60919, ufrag);

        while (peerConnection.connectionState !== 'closed') {
            console.log(peerConnection.connectionState);
            coroutine.sleep(3000);
        }
    });
});

// The RTC init-object unions (RTCSessionDescription|Object,
// RTCIceCandidate|Object) in every call form.
describe('the RTC parameter unions', () => {
    function asyncForm(promise) {
        var got;
        promise.then(function (res) {
            got = { res: res };
        }, function (err) {
            got = { err: err };
        });
        assert.ok(test_util.waitUntil(function () { return got !== undefined; }),
            'the async form did not complete');
        return got;
    }

    it('the RTC members convert an init object in every call form', () => {
        // the connection has nothing to negotiate, so the call fails; 20005
        // would mean the init object was not converted at all
        function assertConverted(err) {
            assert.ok(err, 'the call should fail on the empty connection');
            assert.notEqual(err.number, 20005, 'the init object must not be a type mismatch');
        }

        var desc = { type: 'offer', sdp: 'v=0' };
        var cand = { candidate: 'candidate:1 1 UDP 1 127.0.0.1 10000 typ host', sdpMid: '0', sdpMLineIndex: 0 };

        assertConverted(catchOf(() => new rtc.RTCPeerConnection().setLocalDescriptionSync(desc)));
        assertConverted(catchOf(() => new rtc.RTCPeerConnection().setRemoteDescriptionSync(desc)));
        assertConverted(catchOf(() => new rtc.RTCPeerConnection().addIceCandidateSync(cand)));

        assertConverted(asyncForm(new rtc.RTCPeerConnection().setLocalDescription(desc)).err);
        assertConverted(asyncForm(new rtc.RTCPeerConnection().setLocalDescriptionAsync(desc)).err);
        assertConverted(asyncForm(new rtc.RTCPeerConnection().addIceCandidateAsync(cand)).err);

        // the class form reaches the same domain error
        assertConverted(catchOf(() =>
            new rtc.RTCPeerConnection().setLocalDescriptionSync(new rtc.RTCSessionDescription(desc))));
    });
});

// The option records (RTCPeerConnectionOptions, RTCDataChannelOptions,
// RTCServerOptions, RTCSctpSettings, RTCListenerOptions) are typed by the IDL:
// a field of the wrong type is rejected at the call boundary with the field
// name, and unknown keys are ignored as before.
describe('the RTC option records', () => {
    it('rejects a field of the wrong type', () => {
        var pcErr = catchOf(() => new rtc.RTCPeerConnection({ maxMessageSize: 'big' }));
        assert.ok(pcErr, 'a string maxMessageSize must not be accepted');
        assert.equal(pcErr.number, 20005);
        assert.ok(pcErr.message.indexOf('maxMessageSize') >= 0, 'the message names the field: ' + pcErr.message);

        var iceErr = catchOf(() => new rtc.RTCPeerConnection({ iceServers: {} }));
        assert.ok(iceErr, 'iceServers must be an array');
        assert.equal(iceErr.number, 20005);

        var sctpErr = catchOf(() => rtc.setSctpSettings({ maxBurst: 'many' }));
        assert.ok(sctpErr);
        assert.equal(sctpErr.number, 20005);

        var serverErr = catchOf(() => rtc.startServer({ port: 'x' }));
        assert.ok(serverErr);
        assert.equal(serverErr.number, 20005);
    });

    it('accepts the documented shapes and ignores unknown keys', () => {
        var pc = new rtc.RTCPeerConnection({
            certificateType: 'ecdsa',
            iceTransportPolicy: 'all',
            iceServers: [
                { urls: 'stun:127.0.0.1:3478', username: 'user', credential: 'secret' },
                { urls: ['stun:127.0.0.1:3478', 'stun:127.0.0.1:3479'] }
            ],
            maxMessageSize: 16384,
            enableIceUdpMux: false,
            disableFingerprintVerification: false,
            bindAddress: '127.0.0.1',
            unknownKey: 1
        });
        assert.equal(pc.connectionState, 'new');

        var dc = pc.createDataChannel('options', {
            ordered: false,
            maxPacketLifeTime: 100,
            protocol: 'json',
            negotiated: true,
            id: 42
        });
        assert.equal(dc.id, 42);
        assert.equal(dc.protocol, 'json');
        pc.close();
    });

    it('applies only the SCTP fields that are given', () => {
        // the call has no return value: the contract is that a partial record
        // is accepted and the library keeps its defaults for the other fields
        rtc.setSctpSettings({ recvBufferSize: 1024 * 1024 });
        rtc.setSctpSettings({});
    });
});

// The typed initialization dictionaries and the one required field of
// RTCIceServer. Field failures keep the option-record error shape: 20002 names
// the missing field, 20005 names a field of the wrong type.
describe('the RTC required fields', () => {
    it('requires type in RTCSessionDescriptionInit', () => {
        var missing = catchOf(() => new rtc.RTCSessionDescription({}));
        assert.ok(missing, 'an empty description must not be accepted');
        assert.equal(missing.number, 20002);
        assert.equal(missing.name, 'TypeError');
        assert.ok(missing.message.indexOf('type') >= 0, missing.message);

        var noArg = catchOf(() => new rtc.RTCSessionDescription());
        assert.ok(noArg, 'the description argument is required');
        assert.equal(noArg.number, 20002);
        assert.equal(noArg.name, 'TypeError');
        assert.ok(noArg.message.indexOf('description') >= 0, noArg.message);
    });

    it('keeps sdp optional and validates its type', () => {
        var desc = new rtc.RTCSessionDescription({ type: 'rollback' });
        assert.equal(desc.type, 'rollback');
        assert.equal(typeof desc.sdp, 'string');

        var bad = catchOf(() => new rtc.RTCSessionDescription({ type: 'offer', sdp: 1 }));
        assert.ok(bad, 'a numeric sdp must not be accepted');
        assert.equal(bad.number, 20005);
        assert.ok(bad.message.indexOf('sdp') >= 0, bad.message);
    });

    it('names the missing urls field of an ice server', () => {
        var err = catchOf(() => new rtc.RTCPeerConnection({ iceServers: [{}] }));
        assert.ok(err, 'an ice server without urls must not be accepted');
        assert.equal(err.number, 20002);
        assert.equal(err.name, 'TypeError');
        assert.ok(err.message.indexOf('urls') >= 0, err.message);
    });
});

// The MDN-facing surface: the channel state and creation options, the channel
// events on both sides, the end-of-candidates markers and the state
// transitions the standard defines.
describe('the RTC MDN surface', () => {
    function pair() {
        var pc1 = new rtc.RTCPeerConnection({ iceServers: [] });
        var pc2 = new rtc.RTCPeerConnection({ iceServers: [] });
        var toPc1 = [];
        var toPc2 = [];

        pc1.onicecandidate = function (ev) {
            if (ev.candidate) toPc2.push(ev.candidate);
        };
        pc2.onicecandidate = function (ev) {
            if (ev.candidate) toPc1.push(ev.candidate);
        };

        return { pc1: pc1, pc2: pc2, toPc1: toPc1, toPc2: toPc2 };
    }

    function exchange(ctx) {
        var done = null;
        ctx.pc1.createOffer()
            .then(function (offer) {
                return ctx.pc1.setLocalDescription(offer)
                    .then(function () { return ctx.pc2.setRemoteDescription(offer); });
            })
            .then(function () { return ctx.pc2.createAnswer(); })
            .then(function (answer) {
                return ctx.pc2.setLocalDescription(answer)
                    .then(function () { return ctx.pc1.setRemoteDescription(answer); });
            })
            .then(function () { done = 'ok'; }, function (e) { done = e; });

        assert.ok(test_util.waitUntil(function () { return done !== null; }),
            'the signaling exchange did not settle');
        assert.equal(done, 'ok');
    }

    function pump(ctx, ready, ms) {
        var deadline = Date.now() + (ms || 8000);
        while (!ready() && Date.now() < deadline) {
            while (ctx.toPc1.length) ctx.pc1.addIceCandidate(ctx.toPc1.shift());
            while (ctx.toPc2.length) ctx.pc2.addIceCandidate(ctx.toPc2.shift());
            coroutine.sleep(10);
        }
        return ready();
    }

    it('reports the channel state and the creation options', () => {
        var pc = new rtc.RTCPeerConnection({ iceServers: [] });
        var dc = pc.createDataChannel('options', {
            ordered: false,
            maxPacketLifeTime: 120,
            protocol: 'json',
            negotiated: true,
            id: 9
        });

        assert.equal(dc.readyState, 'connecting');
        assert.equal(dc.ordered, false);
        assert.equal(dc.maxPacketLifeTime, 120);
        assert.equal(dc.maxRetransmits, null);
        assert.equal(dc.negotiated, true);
        assert.equal(dc.protocol, 'json');
        assert.equal(dc.id, 9);

        assert.equal(dc.bufferedAmountLowThreshold, 0);
        dc.bufferedAmountLowThreshold = 4096;
        assert.equal(dc.bufferedAmountLowThreshold, 4096);
        assert.throws(function () { dc.bufferedAmountLowThreshold = -1; });

        // the standard makes the reliability limits exclusive and bounds the id
        assert.throws(function () {
            pc.createDataChannel('both', { maxPacketLifeTime: 1, maxRetransmits: 1 });
        });
        assert.throws(function () { pc.createDataChannel('negative', { maxPacketLifeTime: -1 }); });
        assert.throws(function () {
            pc.createDataChannel('id', { negotiated: true, id: 65535 });
        });

        pc.close();
    });

    it('opens a remote channel with an open event and closes a local one with close', () => {
        var ctx = pair();
        var remoteOpen = false;
        var remoteState = null;
        var remoteNegotiated = null;

        ctx.pc2.ondatachannel = function (ev) {
            remoteState = ev.channel.readyState;
            remoteNegotiated = ev.channel.negotiated;
            ev.channel.onopen = function () { remoteOpen = true; };
        };

        var endOfCandidates = 0;
        ctx.pc1.onicecandidate = function (ev) {
            if (ev.candidate) ctx.toPc2.push(ev.candidate);
            else endOfCandidates++;
        };

        var dc = ctx.pc1.createDataChannel('events');
        assert.equal(dc.readyState, 'connecting');

        var localClose = 0;
        dc.onclose = function () { localClose++; };

        exchange(ctx);
        assert.ok(pump(ctx, function () { return remoteOpen; }), 'the remote channel did not open');

        assert.equal(remoteState, 'open');
        assert.equal(remoteNegotiated, false);
        assert.equal(endOfCandidates, 1, 'the end-of-candidates event did not fire once');
        assert.equal(dc.readyState, 'open');

        dc.close();
        assert.ok(test_util.waitUntil(function () { return localClose > 0; }),
            'the local close event did not fire');
        assert.equal(dc.readyState, 'closed');

        ctx.pc1.close();
        ctx.pc2.close();
    });

    it('resolves getStats before a pair is selected and rejects a pending offer on close', () => {
        var pc = new rtc.RTCPeerConnection({ iceServers: [] });

        var stats = null;
        var statsError = null;
        pc.getStats().then(function (s) { stats = s; }, function (e) { statsError = e; });
        assert.ok(test_util.waitUntil(function () { return stats || statsError; }),
            'getStats did not settle');
        assert.equal(statsError, null);
        assert.equal(stats.size, 1);

        var transport = stats.get('RTCTransport_0_1');
        assert.equal(transport.dtlsState, 'new');
        assert.equal(transport.selectedCandidatePairChanges, 0);
        assert.equal(transport.selectedCandidatePairId, undefined);

        var offer = 'pending';
        pc.createOffer().then(function () { offer = 'resolved'; }, function () { offer = 'rejected'; });
        pc.close();
        assert.ok(test_util.waitUntil(function () { return offer !== 'pending'; }),
            'the pending offer did not settle');
        assert.equal(offer, 'rejected');
    });

    it('accepts candidate dictionaries without sdpMid and rolls back a local offer', () => {
        var cand = new rtc.RTCIceCandidate({
            candidate: 'candidate:1 1 UDP 1 127.0.0.1 1000 typ host'
        });
        assert.equal(cand.sdpMid, '0');
        assert.equal(cand.type, 'host');

        var marker = new rtc.RTCIceCandidate({});
        assert.equal(marker.candidate, '');
        assert.equal(marker.sdpMid, null);

        var pc = new rtc.RTCPeerConnection({ iceServers: [] });
        var markerAdded = false;
        var markerError = null;
        pc.addIceCandidate({}).then(function () { markerAdded = true; }, function (e) { markerError = e; });
        assert.ok(test_util.waitUntil(function () { return markerAdded || markerError; }),
            'the end-of-candidates marker did not settle');
        assert.equal(markerError, null);

        pc.createDataChannel('rollback');
        var offered = null;
        var rolledBack = null;
        var error = null;
        pc.createOffer()
            .then(function (offer) { return pc.setLocalDescription(offer); })
            .then(function () {
                offered = pc.signalingState;
                return pc.setLocalDescription({ type: 'rollback' });
            })
            .then(function () { rolledBack = pc.signalingState; }, function (e) { error = e; });

        assert.ok(test_util.waitUntil(function () { return rolledBack || error; }),
            'the rollback did not settle');
        assert.equal(error, null);
        assert.equal(offered, 'have-local-offer');
        assert.equal(rolledBack, 'stable');

        pc.close();
    });
});


